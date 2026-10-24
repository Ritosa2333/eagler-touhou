#pragma once
#include <cstdint>
#include <string_view>
#include <vector>

namespace th12::ascii_text {
struct Glyph {
  int32_t sprite = 0;
  float x = 0, y = 0, z = 0, sx = 1, sy = 1;
  uint32_t argb = 0xffffffffu;
};
// The recovered path is the numeric Font5 used by Extra UFO score callbacks.
// It leaves unsupported text visible to the caller as false.
bool font5(std::string_view text, float x, float y, float z, float sx, float sy,
           int horizontalAlignment, int verticalAlignment, uint32_t color,
           std::vector<Glyph>& output);
} // namespace th12::ascii_text
