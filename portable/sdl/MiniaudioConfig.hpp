// One miniaudio configuration for every translation unit that includes it
// (the implementation lives in Audio.cpp).
#pragma once
#define MA_NO_DEVICE_IO
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_ENCODING
#define MA_NO_THREADING
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.h"
#include "third_party/miniaudio.h"
