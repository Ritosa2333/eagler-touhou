#include "TextGlyphs.hpp"

namespace th12::ascii_text {
bool font5(std::string_view text, float x, float y, float z, float sx, float sy,
           int horizontalAlignment, int verticalAlignment, uint32_t color,
           std::vector<Glyph>& out) {
  for (unsigned char c : text)
    if (c < '0' || c > '9')
      return false;
  const float step = float(double(sx) * 7);
  if (horizontalAlignment == 0)
    x = float(double(x) - double(text.size()) * step * .5);
  else if (horizontalAlignment == 2)
    x = float(double(x) - double(text.size()) * step);
  // 401b79 uses the unscaled font height here, even for scaleY2.
  if (verticalAlignment == 0)
    y = float(double(y) - 5);
  else if (verticalAlignment == 2)
    y = float(double(y) - 10);
  for (unsigned char c : text) {
    out.push_back({int(c) + 179, x, y, z, sx, sy, color});
    x = float(double(x) + step);
  }
  return true;
}
} // namespace th12::ascii_text
