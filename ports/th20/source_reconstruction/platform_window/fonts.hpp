#pragma once
#include <Windows.h>
#include <cstdint>
namespace th20::source::platform_window {
// Actual CRT zero initialized globals, 5b66ec/5b66f0/5b6748. The Yu Gothic
// branch exists in the specimen although its availability byte is not probed.
extern std::uint8_t font_available[3];
#ifndef TH_SDL3
extern HFONT fonts[22];
extern std::uint8_t* current_font_probe;
int CALLBACK mark_font_available(const LOGFONTW*,const TEXTMETRICW*,DWORD,LPARAM); // 414820
#else
// TH_SDL3 font slots carry the original sizes/weights/families; glyphs come
// from the FreeType host (th20_web/cpp/platform/Fonts.hpp).
struct FontSlot {std::int32_t height,weight,family;};
extern FontSlot font_slots[22];
#endif
void initialize_fonts(); // 416d20
}
