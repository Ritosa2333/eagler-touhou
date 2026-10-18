#pragma once
#include "Types.hpp"
#include <array>
#include <vector>
namespace th15 {
struct BulletTransform {
    std::array<float,4> floats{};
    std::array<i32,4> integers{};
    u32 type=0;i32 active=0;
    std::vector<u8> payload;
};
struct BulletShooter {
    i32 sprite=0,color=0;Vec3 position{};
    float angle=0,angle_step=0,speed=0,speed_step=0,radius=0;
    std::array<BulletTransform,18> transforms{};
    std::array<u32,9> extra{};
    i16 count=0,rows=0,pattern=0;u16 reserved=0;
    u32 flags=0;i32 shoot_sound=0,transform_sound=0;
    std::array<u32,2> tail{};
    void reset();
};
struct EnemyShooters {
    std::array<BulletShooter,16> shooters{};
    std::array<Vec3,16> offset{},origin{};
    std::array<i32,16> next_transform{};
};
struct BulletEmissionHost {
    virtual ~BulletEmissionHost()=default;
    virtual bool emit(const BulletShooter& shooter,float minimum_distance_squared)=0;
};
}
