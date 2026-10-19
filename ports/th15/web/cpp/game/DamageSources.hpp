#pragma once
#include "MotionState.hpp"
#include "Timer.hpp"
#include <array>
#include <functional>
#include <string>
namespace th15 {
struct DamageSource {
    u32 flags=0;float radius=0,radius_growth=0,angle=0,angular_speed=0;Vec2 size{};
    MotionState motion;Timer lifetime;
    i32 damage=0,accumulated=0,damage_limit=0,interval=0,last_target=0,shot_index=0,callback_target=0,callback_kind=0;
};
static_assert(sizeof(DamageSource)==0x94);
struct DamageQuery {
    Vec3 position{};Vec2 size{};float radius=0,angle=0;bool rectangle=false,preview=false;
    i32 target=0;
};
struct DamageResult {i32 amount=0;bool direct=false;Vec3 position{};bool position_written=false;};
class DamageSources {
public:
    std::array<DamageSource,256> sources{};i32 cursor=0,maximum_damage=60,score=0;
    std::string error;
    std::function<i32(DamageSource&,const DamageQuery&)> hit_callback;
    i32 circle(const Vec3&,float radius,float growth,i32 lifetime,i32 damage);
    i32 rectangle(const Vec3&,float angle,const Vec2&,i32 lifetime,i32 damage);
    DamageSource* find(i32 id){return id>0&&id<=256?&sources[id-1]:nullptr;}
    void tick(float rate=1);
    DamageResult query(const DamageQuery&,bool frame_changed=true,i32 bomb_damage=0);
private:
    i32 allocate();
};
bool rectangle_circle(const Vec2&,const Vec2&,float angle,const Vec2&,float radius);
bool rectangle_rectangle(const Vec2&,const Vec2&,float angle,const Vec2&,const Vec2&,float other_angle);
bool line_rectangle(const Vec2& origin,float angle,const Vec2& center,const Vec2& size,float rotation,Vec2& near,Vec2& far);
}
