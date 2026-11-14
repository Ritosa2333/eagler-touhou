#pragma once
#include <cmath>

namespace th11 {
// One inverse rotation for all targets of a rectangular cancel. Keep the
// original angle as the laser cutters also rotate their individual headings.
struct Th14RectangleRotation {
    float angle, sine, cosine;
    constexpr explicit Th14RectangleRotation(float a) noexcept
        :angle(a),sine(a==0.f?-a:float(std::sin(-double(a)))),
         cosine(a==0.f?1.f:float(std::cos(-double(a)))){}
};
}
