#pragma once
#include "../platform/BrowserRuntime.hpp"
namespace th08 {
bool sdl_read_file(const char*,std::vector<u8>&);
bool sdl_load_assets(BrowserRuntime&);
bool sdl_prepare_asset(BrowserRuntime&,u32);
bool sdl_decode_image(BrowserRuntime&,const char*,const std::vector<u8>&);
void sdl_audio_pump();void sdl_audio_pause(bool);void sdl_audio_shutdown();
void sdl_fonts_shutdown();
u32 sdl_game_time();
}
