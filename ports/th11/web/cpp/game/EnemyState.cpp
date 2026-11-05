#include "EnemyState.hpp"
#include <algorithm>
#include <cmath>
namespace th11 {
void EnemyState::initialize(Enemy* owner,const float* rate)noexcept{
    std::memset(this,0,sizeof(*this));script_owner=owner;
    hitbox=collision_box={24,24};boss_slot=-1;facing=1;manager_link.value=owner;
    drops.spread={32,32};
    lifetime.set(0,rate);damage_immunity.set(0,rate);collision_immunity.set(0,rate);
    for(auto& interrupt:interrupts)interrupt.health=interrupt.time=-1;
}
namespace {
Vec3 add(Vec3 a,Vec3 b){return{float(double(a.x)+b.x),float(double(a.y)+b.y),float(double(a.z)+b.z)};}
Vec3 subtract(Vec3 a,Vec3 b){return{float(double(a.x)-b.x),float(double(a.y)-b.y),float(double(a.z)-b.z)};}
void angle_step(Movement& movement,Vec2Interpolator& interpolation,const float* rate){
    if(!interpolation.duration)return;const Vec2 value=sample(interpolation,rate);
    if(!(movement.flags&1))movement.angle=normalize_angle(value.x);movement.speed=value.y;
}
void radius_step(Movement& movement,Vec2Interpolator& interpolation,const float* rate){
    if(!interpolation.duration)return;const Vec2 value=sample(interpolation,rate);movement.radius=value.x;movement.radial_velocity=value.y;
}
void velocity_step(Movement& movement,Vec3Interpolator& interpolation,const float* rate){
    if(interpolation.duration)movement.velocity=subtract(sample(interpolation,rate),movement.position);else movement.update_velocity();
}
float clamp_axis(float value,float center,float width){const double half=double(width)*.5,low=double(center)-half,high=double(center)+half;return value<low?float(low):value>high?float(high):value;}
}
// 0x411610: the coordinate sum is rounded before the displacement subtraction.
void EnemyState::combine_movement()noexcept{
    current.velocity=subtract(add(relative.position,absolute.position),current.position);current.update();
    if(flags&0x2000){current.position.x=clamp_axis(current.position.x,clamp_center.x,clamp_size.x);current.position.y=clamp_axis(current.position.y,clamp_center.y,clamp_size.y);absolute.position=subtract(current.position,relative.position);}
}
// Movement/visibility prefix of 0x411750, before script and collision updates.
i32 EnemyState::update_movement(const float* rate,const Vec3& camera_delta)noexcept{
    if(flags&0x4000)return 0;flags|=0x4000;previous=current;
    angle_step(absolute,absolute_angle,rate);radius_step(absolute,absolute_radius,rate);
    angle_step(relative,relative_angle,rate);radius_step(relative,relative_radius,rate);
    velocity_step(absolute,absolute_position,rate);velocity_step(relative,relative_position,rate);
    absolute.update();if(flags&0x400000)relative.position=add(relative.position,camera_delta);relative.update();combine_movement();
    const double half_x=double(visual_size.x)*.5,half_y=double(visual_size.y)*.5;
    if(double(current.position.x)+half_x< -192||double(current.position.x)-half_x>192){if((flags&0x1000)&&!(flags&4))return -1;}
    else if(double(current.position.y)+half_y<0||double(current.position.y)-half_y>448){if((flags&0x1000)&&!(flags&8))return -1;}
    else flags|=0x1000;
    return 0;
}
float aim_angle(const Vec3& origin,const Vec3& player)noexcept{
    const float dx=float(double(player.x)-origin.x),dy=float(double(player.y)-origin.y);
    if(dx==0&&dy==0)return 1.57079637050628662109375f;
    return float(std::atan2(double(dy),double(dx)));
}
float position_distance(const Vec3& a,const Vec3& b)noexcept{const double dx=double(a.x)-b.x,dy=double(a.y)-b.y;return float(std::sqrt(double(float(dx*dx+dy*dy))));}
i32* EnemyGlobals::integer_reference(i32 id){
    if(id>=-9985&&id<=-9982)return enemy.integers+(id+9985);
    if(id>=-9949&&id<=-9947)return environment.shared_integers+(id+9949);
    if(id>=-9943&&id<=-9940)return environment.boss?environment.boss->integers+(id+9943):nullptr;
    return nullptr;
}
float* EnemyGlobals::float_reference(i32 id){
    if(id>=-9981&&id<=-9978)return enemy.floats+(id+9981);
    if(id>=-9939&&id<=-9936)return environment.boss?environment.boss->floats+(id+9939):nullptr;
    if(id>=-9935&&id<=-9932)return enemy.extra_floats+(id+9935);
    return nullptr;
}
double EnemyGlobals::floating(i32 id){
    if(auto* p=integer_reference(id))return *p;if(auto* p=float_reference(id))return *p;
    switch(id){
    case -10000:return environment.rng().next32();case -9999:return environment.rng().unit();
    case -9998:return float(double(environment.rng().signed_unit())*3.1415927410125732421875);
    case -9987:return environment.rng().signed_unit();
    case -9997:case -9977:return enemy.current.position.x;case -9996:case -9976:return enemy.current.position.y;
    case -9995:case -9975:return enemy.absolute.position.x;case -9994:case -9974:return enemy.absolute.position.y;
    case -9993:case -9973:return enemy.relative.position.x;case -9992:case -9972:return enemy.relative.position.y;
    case -9991:case -9965:return environment.player_position.x;case -9990:case -9964:return environment.player_position.y;
    case -9989:return aim_angle(enemy.current.position,environment.player_position);
    case -9988:return enemy.lifetime.fractional;case -9986:return (enemy.flags>>20)&1;
    case -9971:return enemy.absolute.angle;case -9970:return enemy.relative.angle;
    case -9969:return enemy.absolute.speed;case -9968:return enemy.relative.speed;
    case -9967:return enemy.absolute.radius;case -9966:return enemy.relative.radius;
    case -9963:return environment.boss?environment.boss->current.position.x:0;
    case -9962:return environment.boss?environment.boss->current.position.y:0;
    case -9960:return environment.rank;case -9959:return environment.difficulty;
    case -9958:return float(std::atan2(double(enemy.current.velocity.y),double(enemy.current.velocity.x)));
    case -9957:return 1;case -9956:return aim_angle(enemy.absolute.position,environment.player_position);
    case -9955:return aim_angle(enemy.relative.position,environment.player_position);case -9954:return enemy.health;
    case -9953:case -9952:case -9951:case -9950:return environment.difficulty==id+9953;
    case -9946:return environment.enemy_count;case -9945:return signed_bits(u32(environment.character)*3+u32(environment.subtype));
    case -9944:return position_distance(enemy.current.position,environment.player_position);
    default:return 0;
    }
}
i32 EnemyGlobals::integer(i32 id){
    if(auto* p=integer_reference(id))return *p;
    switch(id){
    case -10000:return signed_bits(environment.rng().next32());
    case -9998:case -9989:case -9956:case -9955:return 0;
    case -9988:return enemy.lifetime.current;
    case -9961:return environment.animation_sprite?environment.animation_sprite(enemy.animations[0],environment.animation_user):0;
    default:return truncate_int(floating(id));
    }
}
}
