/*
 * This file is part of wat4ff.
 * 2024 github.com/chrdev
 * Zero-Clause BSD
 * 
 * wat4ff is an AudioToolbox wrapper for FFMpeg on Windows.
 * With it we can use aac_at encoder and decoder.
 * 
 * This header is prepared for these 3 files:
 * dev - libavdevice/audiotoolbox.m
 * dec - libavcodec/audiotoolboxdec.c
 * enc - libavcodec/audiotoolboxenc.c
*/

#ifndef AudioToolbox_H
#define AudioToolbox_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultichar"
#endif


#include <stdint.h>
#include <stdbool.h>


typedef uint8_t                      Boolean;
typedef int16_t                      SInt16;
typedef int32_t                      SInt32;
typedef uint32_t                     UInt32;
typedef int64_t                      SInt64;
typedef uint64_t                     UInt64;
typedef float                        Float32;
typedef double                       Float64;
                                     
typedef SInt32                       OSStatus;
                                     
typedef const struct __CFString*     CFStringRef;
typedef const void*                  CFTypeRef;
typedef UInt32                       CFStringEncoding;

typedef CFStringRef                  CFRunLoopMode;
typedef struct __CFRunLoop*          CFRunLoopRef;

typedef UInt32                       AudioObjectID;
typedef AudioObjectID                AudioDeviceID;
typedef UInt32                       AudioQueuePropertyID;
typedef UInt32                       AudioTimeStampFlags;

typedef struct OpaqueAudioConverter* AudioConverterRef;
typedef UInt32                       AudioConverterPropertyID;

typedef UInt32                       AudioChannelLabel;
typedef UInt32                       AudioChannelLayoutTag;
typedef UInt32                       AudioChannelBitmap;
typedef UInt32                       AudioChannelFlags;

typedef UInt32                       AudioFormatID;
typedef UInt32                       AudioFormatFlags;
typedef UInt32                       AudioFormatPropertyID;


enum
{
    noErr                     = 0,
    kAudio_UnimplementedError = -4,   // Not used by ffmpeg, but we may return this.
    NSExecutableLoadError     = 3587, // Not used by ffmpeg, but we may return this if DLL failed to load.
};

enum used_only_by_dev_
{
    kCFStringEncodingMacRoman              = 0,

    kAudioHardwarePropertyDevices          = 'dev#',

    kAudioObjectSystemObject               = 1,

    kAudioObjectPropertyElementMaster      = 0,
    kAudioObjectPropertyScopeGlobal        = 'glob',

    kAudioDevicePropertyScopeInput         = 'inpt',
    kAudioDevicePropertyDeviceUID          = 'uid ',
    kAudioDevicePropertyDeviceNameCFString = 'lnam',

    kAudioQueueProperty_CurrentDevice      = 'aqcd',

    kAudioFormatUnsupportedDataFormatError = 'fmt?',

    kAudioFormatFlagIsBigEndian            = (1U << 1),
    kAudioFormatFlagIsNonInterleaved       = (1U << 5),

    kLinearPCMFormatFlagIsFloat            = (1U << 0),
    kLinearPCMFormatFlagIsSignedInteger    = (1U << 2),
};

enum shared_by_dev_dec_enc_
{
    kAudioFormatLinearPCM = 'lpcm',
};

enum used_only_by_dec_
{
    kAudioFormatAC3                             = 'ac-3',
    kAudioFormatQDesign                         = 'QDMC',
    kAudioFormatQDesign2                        = 'QDM2',
    kAudioFormatMPEGLayer1                      = '.mp1',
    kAudioFormatMPEGLayer2                      = '.mp2',
    kAudioFormatMPEGLayer3                      = '.mp3',
    kAudioFormatAMR                             = 'samr',
    kAudioFormatEnhancedAC3                     = 'ec-3',

    kAudioChannelLabel_LFEScreen                = 4,
    kAudioChannelLabel_RightSurround            = 6,
    kAudioChannelLabel_CenterSurround           = 9,
    kAudioChannelLabel_RightSurroundDirect      = 11,
    kAudioChannelLabel_TopBackRight             = 18,
    kAudioChannelLabel_RearSurroundLeft         = 33,
    kAudioChannelLabel_RearSurroundRight        = 34,
    kAudioChannelLabel_RightWide                = 36,
    kAudioChannelLabel_Mono                     = 42,

    kAudioConverterDecompressionMagicCookie     = 'dmgc',

    kAudioChannelLayoutTag_UseChannelBitmap     = (1U << 16),

    kAudioFormatProperty_FormatInfo             = 'fmti',
    kAudioFormatProperty_ChannelLayoutForBitmap = 'cmpb',
    kAudioFormatProperty_ChannelLayoutForTag    = 'cmpl',
};

enum shared_by_dec_enc_ {
    kAudioFormatFlagIsSignedInteger               = (1U << 2),
    kAudioFormatFlagIsPacked                      = (1U << 3),

    kAudioFormatMPEG4AAC                          = 'aac ',
    kAudioFormatAppleIMA4                         = 'ima4',
    kAudioFormatULaw                              = 'ulaw',
    kAudioFormatALaw                              = 'alaw',
    kAudioFormatAppleLossless                     = 'alac',

    kAudioChannelLabel_LFE2                       = 37,

    kAudioConverterCurrentOutputStreamDescription = 'acod',
    kAudioConverterOutputChannelLayout            = 'ocl ',

    kAudioChannelLayoutTag_UseChannelDescriptions = (0U << 16),
};

enum used_only_by_enc_
{
    kAudioFormatFlagIsFloat                           = (1U << 0),

    kAudioFormatMPEG4AAC_LD                           = 'aacl',
    kAudioFormatMPEG4AAC_HE                           = 'aach',
    kAudioFormatMPEG4AAC_HE_V2                        = 'aacp',

    kAudioConverterPrimeInfo                          = 'prim',
    kAudioConverterCurrentInputStreamDescription      = 'acid',
    kAudioConverterPropertyMaximumOutputPacketSize    = 'xops',
    kAudioConverterPropertyBitDepthHint               = 'acbd',

    kAudioConverterCompressionMagicCookie             = 'cmgc',
    kAudioConverterCodecQuality                       = 'cdqu',

    kAudioConverterEncodeBitRate                      = 'brat',
    kAudioConverterInputChannelLayout                 = 'icl ',
    kAudioConverterApplicableEncodeBitRates           = 'aebr',

    kAudioCodecPropertyBitRateControlMode             = 'acbf',
    kAudioCodecPropertySoundQualityForVBR             = 'vbrq',

    kAudioChannelLayoutTag_Mono                       = (100U << 16) | 1,
    kAudioChannelLayoutTag_Stereo                     = (101U << 16) | 2,
    kAudioChannelLayoutTag_MPEG_7_1_C                 = (128U << 16) | 8,
    kAudioChannelLayoutTag_AAC_6_0                    = (141U << 16) | 6,
    kAudioChannelLayoutTag_AAC_6_1                    = (142U << 16) | 7,
    kAudioChannelLayoutTag_AAC_7_0                    = (143U << 16) | 7,
    kAudioChannelLayoutTag_AAC_7_1                    = (127U << 16) | 8,
    kAudioChannelLayoutTag_AAC_Octagonal              = (144U << 16) | 8,
    kAudioChannelLayoutTag_AAC_Quadraphonic           = (108U << 16) | 4,
    kAudioChannelLayoutTag_AAC_3_0                    = (114U << 16) | 3,
    kAudioChannelLayoutTag_AAC_4_0                    = (116U << 16) | 4,
    kAudioChannelLayoutTag_AAC_5_0                    = (120U << 16) | 5,
    kAudioChannelLayoutTag_AAC_5_1                    = (124U << 16) | 6,

    kAudioCodecBitRateControlMode_Constant            = 0,
    kAudioCodecBitRateControlMode_LongTermAverage     = 1,
    kAudioCodecBitRateControlMode_VariableConstrained = 2,
    kAudioCodecBitRateControlMode_Variable            = 3,
};


// Shared by dev, dec, enc +++

typedef struct AudioStreamBasicDescription
{
    Float64          mSampleRate;
    AudioFormatID    mFormatID;
    AudioFormatFlags mFormatFlags;
    UInt32           mBytesPerPacket;
    UInt32           mFramesPerPacket;
    UInt32           mBytesPerFrame;
    UInt32           mChannelsPerFrame;
    UInt32           mBitsPerChannel;
    UInt32           mReserved;
}AudioStreamBasicDescription;

typedef struct AudioStreamPacketDescription
{
    SInt64 mStartOffset;
    UInt32 mVariableFramesInPacket;
    UInt32 mDataByteSize;
}AudioStreamPacketDescription;

// Shared by dev, dec, enc ---


// Used only by dev +++

typedef struct AudioQueueBuffer
{
    const UInt32                         mAudioDataBytesCapacity;
    void* const                          mAudioData;
    UInt32                               mAudioDataByteSize;
    void*                                mUserData;

    const UInt32                         mPacketDescriptionCapacity;
    AudioStreamPacketDescription* const  mPacketDescriptions;
    UInt32                               mPacketDescriptionCount;
}AudioQueueBuffer;

typedef AudioQueueBuffer* AudioQueueBufferRef;

typedef struct OpaqueAudioQueue*  AudioQueueRef;

typedef UInt32 AudioObjectPropertySelector;
typedef UInt32 AudioObjectPropertyScope;
typedef UInt32 AudioObjectPropertyElement;

typedef struct AudioObjectPropertyAddress
{
    AudioObjectPropertySelector mSelector;
    AudioObjectPropertyScope    mScope;
    AudioObjectPropertyElement  mElement;
}AudioObjectPropertyAddress;

typedef UInt32 SMPTETimeType;
typedef UInt32 SMPTETimeFlags;

typedef struct SMPTETime
{
    SInt16         mSubframes;
    SInt16         mSubframeDivisor;
    UInt32         mCounter;
    SMPTETimeType  mType;
    SMPTETimeFlags mFlags;
    SInt16         mHours;
    SInt16         mMinutes;
    SInt16         mSeconds;
    SInt16         mFrames;
}SMPTETime;

typedef struct AudioTimeStamp
{
    Float64             mSampleTime;
    UInt64              mHostTime;
    Float64             mRateScalar;
    UInt64              mWordClockTime;
    SMPTETime           mSMPTETime;
    AudioTimeStampFlags mFlags;
    UInt32              mReserved;
}AudioTimeStamp;

typedef void (*AudioQueueOutputCallback)(void*, AudioQueueRef, AudioQueueBufferRef);

static const CFRunLoopMode kCFRunLoopCommonModes = 0;

static inline const char*
CFStringGetCStringPtr(CFStringRef p1, CFStringEncoding p2) {
    return (const char*)0;
}

static inline OSStatus
AudioObjectGetPropertyDataSize(AudioObjectID p1, const AudioObjectPropertyAddress* p2, UInt32 p3, const void* p4, UInt32* p5) {
    return kAudio_UnimplementedError;
}

static inline OSStatus
AudioObjectGetPropertyData(AudioObjectID p1, const AudioObjectPropertyAddress* p2, UInt32 p3, const void* p4, UInt32* p5, void* p6) {
    return kAudio_UnimplementedError;
}

static inline OSStatus
AudioQueueSetProperty(AudioQueueRef p1, AudioQueuePropertyID p2, const void* p3, UInt32 p4) {
    return kAudio_UnimplementedError;
}

static inline OSStatus
AudioQueueNewOutput(const AudioStreamBasicDescription* p1, AudioQueueOutputCallback p2, void* p3, CFRunLoopRef p4, CFStringRef p5, UInt32 p6, AudioQueueRef* p7) {
    return kAudio_UnimplementedError;
}

static inline OSStatus
AudioQueueAllocateBuffer(AudioQueueRef p1, UInt32 p2, AudioQueueBufferRef* p3) {
    return kAudio_UnimplementedError;
}

static inline OSStatus
AudioQueueEnqueueBuffer(AudioQueueRef p1, AudioQueueBufferRef p2, UInt32 p3, const AudioStreamPacketDescription* p4) {
    return kAudio_UnimplementedError;
}

static inline OSStatus
AudioQueueStart(AudioQueueRef p1, const AudioTimeStamp* p2) {
    return kAudio_UnimplementedError;
}

static inline OSStatus
AudioQueueDispose(AudioQueueRef p1, Boolean p2) {
    return kAudio_UnimplementedError;
}

static inline OSStatus
AudioQueueFlush(AudioQueueRef p1) {
    return kAudio_UnimplementedError;
}

// Used only by dev ---


// Shared by dec, enc +++

typedef struct AudioChannelDescription
{
    AudioChannelLabel mChannelLabel;
    AudioChannelFlags mChannelFlags;
    Float32           mCoordinates[3];
}AudioChannelDescription;

typedef struct AudioChannelLayout
{
    AudioChannelLayoutTag   mChannelLayoutTag;
    AudioChannelBitmap      mChannelBitmap;
    UInt32                  mNumberChannelDescriptions;
    AudioChannelDescription mChannelDescriptions[1];
}AudioChannelLayout;

typedef struct AudioBuffer
{
    UInt32 mNumberChannels;
    UInt32 mDataByteSize;
    void* mData;
}AudioBuffer;

typedef struct AudioBufferList
{
    UInt32      mNumberBuffers;
    AudioBuffer mBuffers[1];
}AudioBufferList;

typedef struct AudioValueRange
{
    Float64 mMinimum;
    Float64 mMaximum;
}AudioValueRange;

typedef OSStatus(*AudioConverterComplexInputDataProc)(AudioConverterRef, UInt32*, AudioBufferList*, AudioStreamPacketDescription**, void*);

// Shared by dec, enc ---


// Used only by enc +++

typedef struct AudioConverterPrimeInfo
{
    UInt32 leadingFrames;
    UInt32 trailingFrames;
}AudioConverterPrimeInfo;

// Used only by enc ---


// Used only by dec +++

OSStatus
AudioFormatGetPropertyInfo(AudioFormatPropertyID p1, UInt32 p2, const void* p3, UInt32* p4);

OSStatus
AudioFormatGetProperty(AudioFormatPropertyID p1, UInt32 p2, const void* p3, UInt32* p4, void* p5);

// Used only by dec ---


// Shared by dec, enc +++

OSStatus
AudioConverterDispose(AudioConverterRef p1);

OSStatus
AudioConverterFillComplexBuffer(AudioConverterRef p1, AudioConverterComplexInputDataProc p2, void* p3, UInt32* p4, AudioBufferList* p5, AudioStreamPacketDescription* p6);

OSStatus
AudioConverterGetProperty(AudioConverterRef p1, AudioConverterPropertyID p2, UInt32* p3, void* p4);

OSStatus
AudioConverterGetPropertyInfo(AudioConverterRef p1, AudioConverterPropertyID p2, UInt32* p3, Boolean* p4);

OSStatus
AudioConverterNew(const AudioStreamBasicDescription* p1, const AudioStreamBasicDescription* p2, AudioConverterRef* p3);

OSStatus
AudioConverterReset(AudioConverterRef p1);

OSStatus
AudioConverterSetProperty(AudioConverterRef p1, AudioConverterPropertyID p2, UInt32 p3, const void* p4);

// Shared by dec, enc ---


#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif
