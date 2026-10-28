#pragma once
#include "Types.hpp"
#include <vector>

namespace th13 {
// Native LaserCurveInf path node (+1464, 3c bytes), 435b80 / 435fa0.
// Samples carry their own angle/speed: the tail is evaluated backwards from
// its preceding sample, not independently rounded from the head expression.
struct CurveLaserSample {
    Vec3 position{},delta{};
    float angle=0,speed=0;
};
static_assert(sizeof(CurveLaserSample)==32);
struct CurveLaserSegment {
    float start_time=0,end_time=999999.f;
    i32 mode=0;
    Vec3 direction{},origin{};
    float angle=0,speed=0,speed_delta=0,angle_delta=0;
    CurveLaserSample head(float time)const;
    CurveLaserSample tail(const CurveLaserSample& previous,float time)const;
};
class CurveLaserPath {
public:
    std::vector<CurveLaserSegment> segments;
    void reset(Vec3 origin,float angle,float speed);
    // 4366c0 op4/op8: append at absolute time, derive initial position from
    // the preceding segment, optionally append a straight continuation.
    void append(i32 mode,float at,i32 duration,float speed_delta,float angle_delta);
    // Native start <= time < end. No matching segment leaves a sample intact.
    const CurveLaserSegment* find(float time)const;
    void sample(float time,std::vector<CurveLaserSample>& points)const;
};
}
