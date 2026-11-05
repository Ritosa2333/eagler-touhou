#pragma once
#include "Types.hpp"
#include <vector>
#include <string>
namespace th15 {
enum class CurvePathMode:i32 {linear=0,acceleration=1,angular=2};
struct CurvePathSegment {
    float begin=0,end=999999;CurvePathMode mode=CurvePathMode::linear;
    Vec3 direction{},origin{};float angle=0,speed=0,acceleration=0,angular_acceleration=0;
};
struct CurvePathSample {Vec3 position{};float angle=0,speed=0;};
class CurvePath {
public:
    std::vector<CurvePathSegment> segments;std::string error;
    static bool evaluate(const CurvePathSegment&,float time,CurvePathSample&)noexcept;
    static bool preceding(const CurvePathSegment&,float time,const CurvePathSample& newer,CurvePathSample&)noexcept;
    // Samples outside every interval retain their prior data, like the original.
    bool sample(float time,const CurvePathSample* newer,CurvePathSample&)const noexcept;
};
}
