#pragma once
#include "BulletCreation.hpp"
namespace th08 {
float bullet_aim(const Vec3& origin,const Vec3& target)noexcept;
Extended bullet_aim_extended(const Vec3& origin,const Vec3& target)noexcept;
bool bullet_in_view(const Vec3& position,float width,float height)noexcept;
class BulletMotion {
public:
    FrameTiming timing;Vec3 player;BulletCreationActions* actions=nullptr;
    // Individual native helpers; caller controls which active flags run and order.
    void step(BulletState&,u32 flag);
    void update(BulletState&);
};
}
