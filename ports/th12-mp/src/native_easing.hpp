#pragma once
#include <algorithm>

namespace th12::native_easing {
// Retail D3D9 gameplay thread has x87 CW007f (24-bit significand).
// 464f80's operations round individually, including intermediates kept on
// the x87 stack. A double pow followed by one float cast changes positions.
inline float power(float value, int degree) {
  float result = value * value;
  for (int i = 2; i < degree; ++i)
    result = result * value;
  return result;
}
inline float sample(float value, int mode) {
  const float t = std::clamp(value, 0.f, 1.f);
  if (mode >= 1 && mode <= 3)
    return power(t, mode + 1);
  if (mode >= 4 && mode <= 6) {
    const float u = 1.f - t;
    return 1.f - power(u, mode - 2);
  }
  if (mode >= 9 && mode <= 11) {
    const float u = t * 2.f;
    if (u < 1.f)
      return power(u, mode - 7) * .5f;
    const float v = 2.f - u;
    return (2.f - power(v, mode - 7)) * .5f;
  }
  if (mode >= 12 && mode <= 14) {
    const float u = t * 2.f;
    if (u < 1.f) {
      const float v = 1.f - u;
      return .5f - power(v, mode - 10) * .5f;
    }
    const float v = u - 1.f;
    return power(v, mode - 10) * .5f + .5f;
  }
  return mode == 15 ? 0.f : mode == 16 ? 1.f : t;
}
} // namespace th12::native_easing
