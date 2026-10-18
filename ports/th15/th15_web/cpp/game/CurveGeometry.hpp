#pragma once
#include "CurvePath.hpp"
#include "AnmGeometry.hpp"
namespace th15 {
// Curve sides are generated on the CPU; the common renderer consumes ordinary
// screen-space vertices and an explicit animation material.
bool curve_strip(const CurvePathSample* points,u32 count,float width,const Vec2& offset,const Vec2& edge_v,std::vector<AnmGeometryVertex>& output);
}
