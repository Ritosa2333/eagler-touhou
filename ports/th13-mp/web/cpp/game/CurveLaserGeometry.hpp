#pragma once
#include "AnmGeometry.hpp"
#include "CurveLaserPath.hpp"

namespace th13 {
void curve_laser_strip(const std::vector<CurveLaserSample>& samples, float width, float v0, float v1,
                       std::vector<AnmGeometryVertex>& out);
}
