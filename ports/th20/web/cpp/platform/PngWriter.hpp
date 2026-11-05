// Minimal PNG writer for the TH_SDL3 screenshot path, replacing the GDI+
// encoder (Gdiplus::Bitmap::Save). Uses the zlib port already linked for
// FreeType. Output is an 8-bit RGBA PNG with filter type 0 rows.
#pragma once

#include <cstdint>
#include <vector>

namespace th20::web {
// Encodes tightly packed RGBA pixels (top-down rows) as PNG.
bool write_png(const char* path, const std::uint8_t* rgba, std::uint32_t width, std::uint32_t height);
// Same encoder into memory (for routed writes through the file host).
bool encode_png(std::vector<std::uint8_t>& out, const std::uint8_t* rgba, std::uint32_t width, std::uint32_t height);
}
