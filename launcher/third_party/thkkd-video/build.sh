#!/usr/bin/bash
set -euo pipefail
export PATH="/build/shims:${THKKD_EMSDK_POSIX}/node/24.19.0_64bit:/usr/bin:/bin"
export EM_CONFIG="${THKKD_EMSDK_WINDOWS}/.emscripten"
export EM_PKG_CONFIG_PATH=/build/install/lib/pkgconfig
export PKG_CONFIG_PATH=/build/install/lib/pkgconfig
export CFLAGS="-O3 -msimd128"
export CXXFLAGS="$CFLAGS"
export CC=emcc CXX=em++ AR=emar RANLIB=emranlib LD=emcc NM=emnm
mkdir -p /build/install /build/output /build/tmp
export TMPDIR=/build/tmp
export SOURCE_DATE_EPOCH=1704067200
cd /build/src/libvpx
if [ -f config.mk ]; then make clean; fi
./configure --prefix=/build/install --target=generic-gnu --disable-install-bins --disable-examples --disable-tools --disable-docs --disable-unit-tests --disable-dependency-tracking --disable-multithread --disable-vp9 --disable-vp8-decoder --enable-vp8-encoder --extra-cflags="$CFLAGS" --extra-cxxflags="$CXXFLAGS"
make -j8 install
emranlib /build/install/lib/libvpx.a
cd /build/src/ffmpeg
if [ -f ffbuild/config.mak ]; then make clean; fi
./configure --tempprefix=/build/tmp/ffconf --prefix=/build/install --target-os=none --arch=x86_32 --enable-cross-compile --disable-asm --disable-stripping --disable-programs --disable-doc --disable-debug --disable-runtime-cpudetect --disable-autodetect --nm=emnm --ar=emar --ranlib=emranlib --cc=emcc --cxx=em++ --objcc=emcc --dep-cc=emcc --extra-cflags="-I/build/install/include -O3 -msimd128" --extra-ldflags="-L/build/install/lib" --disable-pthreads --disable-w32threads --disable-os2threads --disable-everything --disable-avdevice --disable-postproc --disable-swresample --disable-network --enable-libvpx --enable-demuxer=asf --enable-decoder=wmv3 --enable-parser=vc1 --enable-encoder=libvpx_vp8 --enable-muxer=webm --enable-protocol=file --enable-filter=format,null,scale
make -j8
emcc -O3 -msimd128 -I. -Isrc/fftools -I/build/install/include -L/build/install/lib -Llibavcodec -Llibavfilter -Llibavformat -Llibavutil -Llibswscale -Wno-deprecated-declarations -sENVIRONMENT=worker -sWASM_BIGINT -sSTACK_SIZE=5MB -sMODULARIZE -sEXPORT_ES6 -sINITIAL_MEMORY=32MB -sALLOW_MEMORY_GROWTH -sEXPORT_NAME=createFFmpegCore -sEXPORTED_FUNCTIONS=_ffmpeg,_abort,_malloc -sEXPORTED_RUNTIME_METHODS=FS,setValue,getValue,UTF8ToString,lengthBytesUTF8,stringToUTF8 --pre-js src/bind/ffmpeg/bind.js src/fftools/cmdutils.c src/fftools/ffmpeg.c src/fftools/ffmpeg_filter.c src/fftools/ffmpeg_hw.c src/fftools/ffmpeg_mux.c src/fftools/ffmpeg_opt.c src/fftools/opt_common.c -lavcodec -lavfilter -lavformat -lavutil -lswscale -lvpx -o /build/output/ffmpeg-core.js
