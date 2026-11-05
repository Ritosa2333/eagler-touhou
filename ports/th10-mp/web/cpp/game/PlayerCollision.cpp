#include "Player.hpp"
#include "GameMath.hpp"
#ifdef TH_ENABLE_THPRAC
#include "PracticeRuntime.hpp"
#endif
namespace th10 {
namespace {
Extended aim_direction(Extended x,Extended y){return x==number(0.0f)&&y==number(0.0f)?number(1.5707963705062866f):angle_to(y,x);}
bool vulnerable_state(const Player& player,const PlayerCollisionEnvironment& env){return !env.dialogue_active&&player.state!=2&&player.state!=3&&player.state!=4;}
}
// 0x426660 / 0x426610, respectively.
Extended Player::angle_from(const Vec3& point) const noexcept {return aim_direction(number(position.x)-number(point.x),number(position.y)-number(point.y));}
Extended Player::angle_towards(const Vec3& point) const noexcept {return aim_direction(number(point.x)-number(position.x),number(point.y)-number(position.y));}
// 0x4266b0. A rectangle overlap still returns 1 during the invulnerability timer;
// actual damage is suppressed. The laser check below instead returns 0 then.
i32 Player::collide_rectangle(const Vec3& center,const Vec2& size,PlayerCollisionEnvironment& env){
    const auto half_x=number(size.x)*number(.5f),half_y=number(size.y)*number(.5f);
    const float left=(number(center.x)-half_x).to_float(),top=(number(center.y)-half_y).to_float(),right=(half_x+number(center.x)).to_float();
    const auto bottom=half_y+number(center.y);
    if(!(number(right)<number(collision_bounds.minimum.x)||bottom<number(collision_bounds.minimum.y)||
         number(collision_bounds.maximum.x)<number(left)||number(collision_bounds.maximum.y)<number(top))){
        if(!vulnerable_state(*this,env))return 0;
#ifdef TH_ENABLE_THPRAC
        // F1 invincibility skips the damage call but keeps the original
        // rectangle hit-test result, matching the thprac hit-test patch.
        if(!(env.practice&&practice_invincible(*env.practice))&&invulnerability.current<=0)env.hit(*this);return 1;
#else
        if(invulnerability.current<=0)env.hit(*this);return 1;
#endif
    }
    const float graze_left=Scalar::sub(center.x,24.0f),graze_top=Scalar::sub(center.y,24.0f);
    const auto graze_right=number(center.x)+number(24.0f);const float graze_bottom=Scalar::add(center.y,24.0f);
    if(!(graze_right<number(collision_bounds.minimum.x)||number(graze_bottom)<number(collision_bounds.minimum.y)||
         number(collision_bounds.maximum.x)<number(graze_left)||number(collision_bounds.maximum.y)<number(graze_top)))return 2;
    return 0;
}
// 0x4267f0. Rotation and bounds retain the original float storage points.
i32 Player::collide_laser(const Vec3& origin,float angle,float width,float length,PlayerCollisionEnvironment& env){
    const float x=Scalar::sub(position.x,origin.x),y=Scalar::sub(position.y,origin.y);
    const auto sin=sine(-number(angle)),cos=cosine(-number(angle));
    const float rotated_x=(number(x)*cos-sin*number(y)).to_float();const auto rotated_y=sin*number(x)+cos*number(y);
    const float left=Scalar::sub(rotated_x,hitbox_half_size.x),top=(rotated_y-number(hitbox_half_size.y)).to_float();
    const float right=Scalar::add(rotated_x,hitbox_half_size.x),bottom=(rotated_y+number(hitbox_half_size.y)).to_float();
    if(number(length)<number(left))return 0;
    if(number(width)*number(.5f)<number(top)||number(right)<number(0.0f)||number(bottom)<number(width)*number(-.5f)){
        if(!(number(length)<number(left)||number(right)<number(0.0f)||number(width)*number(1.5f)<number(top)||number(top)<number(width)*number(-1.5f)))return 2;
        return 0;
    }
#ifdef TH_ENABLE_THPRAC
    if(!vulnerable_state(*this,env)||(env.practice&&practice_invincible(*env.practice))||invulnerability.current>0)return 0;
#else
    if(!vulnerable_state(*this,env)||invulnerability.current>0)return 0;
#endif
    env.hit(*this);return 1;
}
// 0x427b50. A full pool returns its end pointer, matching the original contract.
DamageArea* Player::emit_circle(const Vec3& position,float radius,float growth,i32 frames,i32 damage,const float* rate){
    for(auto& area:damage_areas)if(!(area.flags&1)){
        std::memset(&area,0,sizeof(area));area.flags=3;area.motion.position=position;area.radius=radius;area.radial_speed=growth;
        area.timer.rate=rate;area.timer_flags=1;area.timer.current=frames;area.timer.previous=wrapping_add(frames,-1);area.timer.fractional=Extended::from_int(frames).to_float();
        area.damage=damage;area.damage_limit=999999;area.interval=4;return &area;
    }
    return damage_areas+32;
}
}
