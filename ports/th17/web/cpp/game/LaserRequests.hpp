#pragma once
#include "BulletShooter.hpp"
namespace th17 {
struct CurvePath;
struct MovingLaserRequest {
    Vec3 position{};float angle=0,maximum_length=0,initial_length=0,end_distance=0,width=0,speed=0;
    i32 type=0,color=0;float start_offset=0;i32 transform_index=0,transform_parameter=0;
    std::array<BulletTransform,18> transforms{};i32 sound=-1,reflection_sound=-1;
};
struct StationaryLaserRequest {
    Vec3 position{},drift{};float angle=0,angular_velocity=0,maximum_length=0,initial_length=0,width=0,speed=8;
    i32 delay=0,warmup=0,active=0,fade=0,sound=-1,transform_sound=-1;u32 id=0;float start_offset=0;i32 transform_index=0,type=0,color=0;u32 flags=0;
    std::array<BulletTransform,18> transforms{};
};
struct CurveLaserRequest {
    Vec3 position{};float angle=0,width=0,speed=0,start_offset=0,initial_time=0;
    i32 type=0,color=0,transform_index=0,sound=-1,transform_sound=-1;u32 count=0,flags=0;
    std::array<BulletTransform,18> transforms{};const CurvePath* inherited_path=nullptr;
};
struct SegmentedLaserRequest {
    Vec3 position{};float angle=0,length=0,width=0,start_offset=0;u32 id=0,flags=0;
    i32 color=0;std::array<BulletTransform,18> transforms{};
};
}
