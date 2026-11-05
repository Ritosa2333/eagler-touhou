// Font rasterization for the TH_SDL3 text path, replacing the GDI font stack
// (CreateFontW/EnumFontFamiliesExW/TextOutW/GetTextExtentPoint32W) with
// FreeType. Game CP932 strings are decoded with the generated cp932 table.
// Glyph coverage is blended with the same per-channel formula GDI applies on
// 32bpp DIBs (alpha source 0), so the game's outline/invert_alpha pipeline
// sees the same intermediate bitmap state. Exact glyph metrics depend on the
// provisioned font file (Phase 3 assets; msgothic-class face expected) and
// are a documented deviation from GDI.
#pragma once

#include <cstdint>

namespace th20::web::fonts {

// Face family indices used by the game's 22 font slots.
constexpr int gothic = 0;
constexpr int mincho = 1;

// Loads the face files. Idempotent; returns false when no usable font file is
// provisioned (text then rasterizes to empty bitmaps, matching a Windows
// machine without any of the original font families).
bool initialize();
bool available(int family);

// GetTextExtentPoint32W analog: summed advances and the font cell height.
void measure(const char* cp932, int height_px, int family, int& cx, int& cy);

// TextOutW-on-DIB analog: coverage-blends the string into a BGRA buffer.
// text_color is packed 0xRRGGBB (COLORREF). spacing>0 draws per character at
// fixed advance (the game's spaced-text mode).
void draw(std::uint8_t* bgra, int pitch, int rows, int x, int y, const char* cp932,
          int height_px, int family, std::uint32_t text_color, int spacing);
}
