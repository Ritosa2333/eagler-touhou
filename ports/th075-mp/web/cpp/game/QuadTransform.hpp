#pragma once
#include "Types.hpp"
namespace th075 {
// Angles in the original game are quantized to tenths of a degree.
float angle_cos(float degrees);
float angle_sin(float degrees);
double angle_tan(float degrees);
using QuadPoints=std::array<std::array<float,4>,4>;
QuadPoints rectangle_quad(float left,float top,float right,float bottom);
void translate_quad(QuadPoints&,float x,float y,float z);
void scale_quad(QuadPoints&,float centerX,float centerY,float centerZ,float x,float y,float z);
void rotate_quad(QuadPoints&,float centerX,float centerY,float centerZ,float xDegrees,float yDegrees,float zDegrees);
}
