#include "utils.hpp"

#include "ZunMath.hpp"

void utils::Rotate(ZunVec3 *out, ZunVec3 *point, f32 angle)
{
    f32 sinAngle;
    f32 cosAngle;

    sinAngle = sinf(angle);
    cosAngle = cosf(angle);
    out->x = cosAngle * point->x + sinAngle * point->y;
    out->y = cosAngle * point->y - sinAngle * point->x;
}
