# WAT4FF

A library for FFmpeg to use AudioToolbox on Windows.

Inspired by 
[AudioToolboxWrapper](https://github.com/dantmnf/AudioToolboxWrapper)

## Installation

Choose either one of the three:

* Install Apple iTunes using the official installer. No further steps needed.

* Extract files from iTunes. Same as
  [QTFiles for qaac](https://github.com/AnimMouse/QTFiles)  
  On x64 OS, The folder tree looks like this:
```
  |   ffmpeg.exe
  \-- QTfiles64
      |   ASL.dll
      |   CoreAudioToolbox.dll
      |   CoreFoundation.dll
      |   icudt62.dll
      |   libdispatch.dll
      |   libicuin.dll
      |   libicuuc.dll
      |   objc.dll
```

* Highly unrecommended, install iTunes from the Store. By default 3rd party
  software cannot load DLLs reside in the app folder. You have to manually set
  permissions by using something like icacls. This method cripples the system!
  Only builds with WAT4FF_USE_APPMODEL explicitly defined support this method.
  
## Tips

`-q 0` gives the best quality, and `-q 14` gives the smallest file.  
Some useful parameters are:  
`-q 4` gives ~192 Kbps stereo, very high quality, recommended for movies.  
`-q 3` gives ~224 Kbps stereo, transparent, use it for opera, live, etc.

For low-res lectures, use `-profile:a 4 -b:a 48k` for HE-AAC at 48 Kbps.

Examples
```
# movie:
ffmpeg -i input.mkv -c:a aac_at -q 4 output.mkv

# lecture for watching on TV:
ffmpeg -i input.mkv -c:a aac_at -profile:a 4 -b:a 64k output.mkv

# lecture for listening in car:
ffmpeg -i input.mkv -map 0:a -c:a aac_at -profile:a 4 -b:a 48k output.m4a

# To get some help
ffmpeg -h encoder=aac_at
```

## Compiling

In an MSYS2 MINGW64 shell

### build wat4ff

```
FFB_PREFIX=/opt/ffbuild

cd path/to/wat4ff
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX="${FFB_PREFIX}" ..
make && make install
```

### build ffmpeg
```
FFB_PREFIX=${FFB_PREFIX:-/opt/ffbuild}

cd path/to/ffmpeg
export CFLAGS="-I${FFB_PREFIX}/include" LDFLAGS="-L${FFB_PREFIX}/lib"
./configure --prefix="${FFB_PREFIX}" --enable-audiotoolbox
make LD="${FFB_PREFIX}/wat4ff_ld" -j$(nproc) V=1
make install

# test
ffmpeg -to 30.0 -f lavfi -i sine=1000 -c aac_at -f mp4 -y NUL
```
## License

    Zero-Clause BSD
    ===============
    
    Permission to use, copy, modify, and/or distribute this software for
    any purpose with or without fee is hereby granted.
    
    THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL
    WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE
    FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY
    DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
    AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
    OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
