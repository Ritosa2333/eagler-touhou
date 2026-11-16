#pragma once
#include "CurveLaserPath.hpp"
#include "AnmGeometry.hpp"

namespace th13 {
// 436370: two screen vertices per sample, with normals averaged across
// adjacent path angles. The caller supplies the live body VM's V coordinates.
void curve_laser_strip(const std::vector<CurveLaserSample>& samples, float width,
                       float v0, float v1, std::vector<AnmGeometryVertex>& out);
}
