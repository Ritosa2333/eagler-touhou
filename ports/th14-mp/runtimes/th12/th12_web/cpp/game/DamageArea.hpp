#pragma once
#include "Movement.hpp"
#include "Timer.hpp"
#include <array>
namespace th11 {
struct DamageArea {
    float radius=0,growth=0,angle=0,angular_velocity=0;
    Vec2 size;Movement movement;Timer timer;
    i32 damage=0,total_damage=0,max_damage=0,period=0;
    u32 flags=0;
    void update() noexcept;
    bool overlaps(const Vec3&,const Vec2&) const noexcept;
};
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(DamageArea)==0x74);
#endif
class DamageAreas {
public:
    std::array<DamageArea,32> areas{};
    DamageArea* circle(const Vec3&,float radius,float growth,i32 lifetime,i32 damage,const float* rate) noexcept;
    void update() noexcept;
    bool collect(const Vec3&,const Vec2&,i32& damage) noexcept;
};
}
