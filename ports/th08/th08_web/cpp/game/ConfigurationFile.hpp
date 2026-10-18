#pragma once
#include "ScoreFile.hpp"
namespace th08 {
// Device discovery supplies the optional ZWAV header. The original config
// validation, defaults and option corrections execute independently of Win32.
bool load_configuration(GameConfiguration&,const u8* bytes,u32 size,const u8* wave_header,u32 wave_size);
}
