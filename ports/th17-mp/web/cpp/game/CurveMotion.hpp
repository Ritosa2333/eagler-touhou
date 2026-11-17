#pragma once
#include "CurvePath.hpp"
#include "Timer.hpp"
namespace th17 {
struct CurveLaserMotion {
    Vec3 position{},velocity{},source{};float angle=0,width=0,speed=0,source_angle=0,source_speed=0,emission_distance=0,render_width=0;
    CurvePath path;std::vector<CurvePathSample> points;Timer path_age,outside_delay;
    bool initialize(const Vec3&,float angle,float width,float speed,u32 count,float initial_time=0,const CurvePath* inherited=nullptr);
    bool advance(float rate,u32 program_flags,u32 object_flags=0)noexcept;
    void finish_frame(float rate)noexcept{path_age.tick(&rate);}
};
}
