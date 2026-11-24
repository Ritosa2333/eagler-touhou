#pragma once
#include "Types.hpp"
#include <vector>

namespace th13 {
struct CurveLaserSample {
    Vec3 position{}, delta{};
    float angle = 0, speed = 0;
};
static_assert(sizeof(CurveLaserSample) == 32);
struct CurveLaserSegment {
    float start_time = 0, end_time = 999999.f;
    i32 mode = 0;
    Vec3 direction{}, origin{};
    float angle = 0, speed = 0, speed_delta = 0, angle_delta = 0;
    CurveLaserSample head(float time) const;
    CurveLaserSample tail(const CurveLaserSample& previous, float time) const;
};
class CurveLaserPath {
public:
    std::vector<CurveLaserSegment> segments;
    void reset(Vec3 origin, float angle, float speed);
    void append(i32 mode, float at, i32 duration, float speed_delta, float angle_delta);
    const CurveLaserSegment* find(float time) const;
    void sample(float time, std::vector<CurveLaserSample>& points) const;
};
}
