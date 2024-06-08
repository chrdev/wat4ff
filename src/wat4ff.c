/*
 * This file is part of wat4ff.
 * 2024 github.com/chrdev
 * Zero-Clause BSD
 *
 * wat4ff is an AudioToolbox wrapper for FFmpeg on Windows.
 * Support aac_at encoder and decoder.
 * 
 * Inspired by github.com/dantmnf/AudioToolboxWrapper
*/

#include <wchar.h>
#include <stdbool.h>

#include <windows.h>
#ifdef WAT4FF_USE_APPMODEL
#include <appmodel.h>
#endif

#include <AudioToolbox/AudioToolbox.h>


static const wchar_t kLibFile[] = L"CoreAudioToolbox.dll";
#ifdef _WIN64
static const wchar_t kLibDir[] = L"QTfiles64";
#else
static const wchar_t kLibDir[] = L"QTfiles";
#endif

#define PROCTYPE(fn) fn ## Proc
#define PROCPTR(fn) p ## fn

#define DECL_PROC(rt, fn, ...) typedef rt (* PROCTYPE(fn))(__VA_ARGS__); static PROCTYPE(fn) PROCPTR(fn) = NULL; rt fn(__VA_ARGS__)
#define GET_PROC(lib, fn)  PROCPTR(fn) = (PROCTYPE(fn))GetProcAddress(lib, #fn)
#define CALL_PROC(fn, ...) PROCPTR(fn)(__VA_ARGS__)

#define LOAD_LIB                                    \
    if (!loaded_) {                                 \
        load();                                     \
        if (!loaded_) return NSExecutableLoadError; \
    }

enum {
    kMaxPath = MAX_PATH,
};

static HMODULE lib_ = NULL;
static volatile LONG loading_ = 0;
static volatile LONG loaded_ = 0;

static void load(void);


// Buf assumed to be kMaxPath in cch size.
static bool
get_app_root(wchar_t* buf, wchar_t** rootp) {
    size_t cch = GetModuleFileNameW(NULL, buf, kMaxPath);
    if (cch >= kMaxPath) return false;

    wchar_t* p = buf + cch - 1;
    for (; *p != L'\\'; --p) {
        if (p == buf) return false;
    }
    *++p = L'\0';
    *rootp = p;

    return true;
}

// Buf assumed to be kMaxPath in cch size.
static bool
calc_portable_lib_path(wchar_t* buf, wchar_t* rootp) {
    wchar_t* p = rootp;
    const wchar_t* end = buf + kMaxPath;
    const wchar_t* pp = p + sizeof(kLibDir) / sizeof(*kLibDir) + sizeof(kLibFile) / sizeof(*kLibFile);
    if (pp > end) return false;

    memcpy(p, kLibDir, sizeof(kLibDir));
    p += sizeof(kLibDir) / sizeof(*kLibDir) - 1;
    *p++ = L'\\';

    memcpy(p, kLibFile, sizeof(kLibFile));

    return true;
}

// Buf assumed to be kMaxPath in cch size.
static bool
get_portable_lib_path(wchar_t* buf) {
    wchar_t* rootp;
    bool ok;

    ok = get_app_root(buf, &rootp);
    if (!ok) return false;

    return calc_portable_lib_path(buf, rootp);
}

// Buf assumed to be kMaxPath in cch size.
static bool
calc_app_lib_path(wchar_t* buf, wchar_t* rootp) {
    const wchar_t* end = buf + kMaxPath;
    const wchar_t* pp = rootp + sizeof(kLibFile) / sizeof(*kLibFile);
    if (pp > end) return false;

    memcpy(rootp, kLibFile, sizeof(kLibFile));

    return true;
}

// Buf assumed to be kMaxPath in cch size.
static bool
get_itunes_prog_lib_path(wchar_t* buf) {
    const wchar_t kSubKey[] = L"SOFTWARE\\Apple Computer, Inc.\\iTunes";
    const wchar_t kValName[] = L"InstallDir";

    DWORD sz = sizeof(wchar_t) * kMaxPath;
    if (ERROR_SUCCESS != RegGetValueW(HKEY_LOCAL_MACHINE, kSubKey, kValName, RRF_RT_REG_SZ, NULL, buf, &sz)) return false;
    sz = (sz / sizeof(wchar_t)) - 1;
    return calc_app_lib_path(buf, buf + sz);
}

#ifdef WAT4FF_USE_APPMODEL
static wchar_t*
gen_wapp_full_name(const wchar_t* family_name) {
    wchar_t* rlt = NULL;
    UINT32 count = 0;
    UINT32 buf_len = 0;
    wchar_t* buf = NULL;
    wchar_t** full_names = NULL;

    LONG rc = FindPackagesByPackageFamily(
        family_name, PACKAGE_FILTER_HEAD,
        &count, NULL, &buf_len, NULL, NULL
    );
    // If Not found, rc == 0
    if (rc != ERROR_INSUFFICIENT_BUFFER) return false;

    buf = HeapAlloc(GetProcessHeap(), 0, sizeof(*buf) * buf_len);
    if (!buf) goto fin;
    full_names = HeapAlloc(GetProcessHeap(), 0, sizeof(*full_names) * count);
    if (!full_names) goto fin;

    rc = FindPackagesByPackageFamily(
        family_name, PACKAGE_FILTER_HEAD,
        &count, full_names, &buf_len, buf, NULL
    );
    if (rc) goto fin;

    rlt = buf;

fin:
    if (!rlt && buf) HeapFree(GetProcessHeap(), 0, buf);
    if (full_names) HeapFree(GetProcessHeap(), 0, full_names);
    return rlt;
}

// Buf assumed to be kMaxPath in cch size.
static bool
get_wapp_dir_by_full_name(wchar_t* buf, wchar_t** rootp, const wchar_t* full_name) {
    UINT32 cch = 0;
    LONG rc = GetPackagePathByFullName(full_name, &cch, NULL);
    if (rc != ERROR_INSUFFICIENT_BUFFER) return false;
    if (cch >= kMaxPath - 1) return false; // We'll add trailing backslash, so kMaxPath - 1
    
    rc = GetPackagePathByFullName(full_name, &cch, buf);
    if (rc) return false;

    buf[cch - 1] = L'\\';
    buf[cch]   = L'\0';
    *rootp = buf + cch;

    return true;
}

// Buf assumed to be kMaxPath in cch size.
static bool
get_wapp_dir_by_family_name(wchar_t* buf, wchar_t** rootp, const wchar_t* family_name) {
    wchar_t* full_name = gen_wapp_full_name(family_name);
    if (!full_name) return false;

    bool rlt = get_wapp_dir_by_full_name(buf, rootp, full_name);
    HeapFree(GetProcessHeap(), 0, full_name);
    return rlt;
}

// Buf assumed to be kMaxPath in cch size.
static bool
get_itunes_app_lib_path(wchar_t* buf) {
    const wchar_t kFamilyName[] = L"AppleInc.iTunes_nzyj5cx40ttqa";

    wchar_t* rootp;
    bool ok = get_wapp_dir_by_family_name(buf, &rootp, kFamilyName);
    if (!ok) return false;

    return calc_app_lib_path(buf, rootp);
}
#endif //#ifdef WAT4FF_USE_APPMODEL

static HMODULE
load_lib(void) {
    enum { kLLFlags = LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_IGNORE_CODE_AUTHZ_LEVEL};
    wchar_t* path = HeapAlloc(GetProcessHeap(), 0, sizeof(wchar_t) * kMaxPath);
    if (!path) return NULL;

    HMODULE lib = NULL;

    if (get_portable_lib_path(path)) {
        if (lib = LoadLibraryExW(path, NULL, kLLFlags)) goto fin;
    }
    if (get_itunes_prog_lib_path(path)) {
        if (lib = LoadLibraryExW(path, NULL, kLLFlags)) goto fin;
    }
#ifdef WAT4FF_USE_APPMODEL
    if (get_itunes_app_lib_path(path)) {
        if (lib = LoadLibraryExW(path, NULL, kLLFlags)) goto fin;
    }
#endif

fin:
    HeapFree(GetProcessHeap(), 0, path);
    return lib;
}

// Shared by dec, enc +++

DECL_PROC(OSStatus, AudioConverterDispose, AudioConverterRef p1) {
    LOAD_LIB;
    return CALL_PROC(AudioConverterDispose, p1);
}

DECL_PROC(OSStatus, AudioConverterFillComplexBuffer, AudioConverterRef p1, AudioConverterComplexInputDataProc p2, void* p3, UInt32* p4, AudioBufferList* p5, AudioStreamPacketDescription* p6) {
    LOAD_LIB;
    return CALL_PROC(AudioConverterFillComplexBuffer, p1, p2, p3, p4, p5, p6);
}

DECL_PROC(OSStatus, AudioConverterGetProperty, AudioConverterRef p1, AudioConverterPropertyID p2, UInt32* p3, void* p4) {
    LOAD_LIB;
    return CALL_PROC(AudioConverterGetProperty, p1, p2, p3, p4);
}

DECL_PROC(OSStatus, AudioConverterGetPropertyInfo, AudioConverterRef p1, AudioConverterPropertyID p2, UInt32* p3, Boolean* p4) {
    LOAD_LIB;
    return CALL_PROC(AudioConverterGetPropertyInfo, p1, p2, p3, p4);
}

DECL_PROC(OSStatus, AudioConverterNew, const AudioStreamBasicDescription* p1, const AudioStreamBasicDescription* p2, AudioConverterRef* p3) {
    LOAD_LIB;
    return CALL_PROC(AudioConverterNew, p1, p2, p3);
}

DECL_PROC(OSStatus, AudioConverterReset, AudioConverterRef p1) {
    LOAD_LIB;
    return CALL_PROC(AudioConverterReset, p1);
}

DECL_PROC(OSStatus, AudioConverterSetProperty, AudioConverterRef p1, AudioConverterPropertyID p2, UInt32 p3, const void* p4) {
    LOAD_LIB;
    return CALL_PROC(AudioConverterSetProperty, p1, p2, p3, p4);
}

// Shared by dec, enc ---


// Used only by dec +++

DECL_PROC(OSStatus, AudioFormatGetPropertyInfo, AudioFormatPropertyID p1, UInt32 p2, const void* p3, UInt32* p4) {
    LOAD_LIB;
    return CALL_PROC(AudioFormatGetPropertyInfo, p1, p2, p3, p4);
}

DECL_PROC(OSStatus, AudioFormatGetProperty, AudioFormatPropertyID p1, UInt32 p2, const void* p3, UInt32* p4, void* p5) {
    LOAD_LIB;
    return CALL_PROC(AudioFormatGetProperty, p1, p2, p3, p4, p5);
}

// Used only by dec ---


static void
load(void) {
    if (InterlockedCompareExchange(&loading_, 1, 0) == 1) {
        while (loading_) SwitchToThread();
        return;
    }
    
    if (loaded_) goto fin;

    lib_ = load_lib();
    if (!lib_) goto fin;

    // CAUTION: Assume all procedure names found without further checking.
    GET_PROC(lib_, AudioConverterDispose);
    GET_PROC(lib_, AudioConverterFillComplexBuffer);
    GET_PROC(lib_, AudioConverterGetProperty);
    GET_PROC(lib_, AudioConverterGetPropertyInfo);
    GET_PROC(lib_, AudioConverterNew);
    GET_PROC(lib_, AudioConverterReset);
    GET_PROC(lib_, AudioConverterSetProperty);

    GET_PROC(lib_, AudioFormatGetPropertyInfo);
    GET_PROC(lib_, AudioFormatGetProperty);

    loaded_ = 1;

fin:
    InterlockedCompareExchange(&loading_, 0, 1);
}
