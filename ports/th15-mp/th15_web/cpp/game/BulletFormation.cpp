#include "BulletFormation.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
namespace {constexpr float pi=3.1415927410125732421875f,tau=6.283185482025146484375f;
Vec2 polar(float angle,float speed){return {float(std::cos(double(angle))*double(speed)),float(std::sin(double(angle))*double(speed))};}
float ring(i32 column,i16 count){return float(float(float(column)*tau)/float(count));}
}
float bullet_aim(const Vec3& position,const Vec3& player)noexcept{const float x=float(player.x-position.x),y=float(player.y-position.y);return x==0&&y==0?1.57079637050628662109375f:float(std::atan2(double(y),double(x)));}
bool form_bullet(const BulletShooter& s,i32 column,i32 row,float aim,Rng& random,const Vec3& player,float minimum_distance_squared,BulletInitialMotion& out)noexcept{
    float speed=s.rows>1?float(s.speed-float(float(float(s.speed-s.speed_step)*float(row))/float(s.rows-1))):s.speed,angle=0;
    switch(s.pattern){
        case 0:case 1:{float delta=s.count&1?float(float(wrapping_add(column,1)/2)*s.angle_step):float(float(float(column/2)*s.angle_step)+float(s.angle_step*.5f));delta=float(delta+0.f);if(column&1)delta=float(delta*-1.f);if(s.pattern==0)delta=float(delta+aim);angle=float(s.angle+delta);break;}
        case 2:case 3:{angle=s.pattern==2?float(aim+0.f):0.f;angle=float(angle+ring(column,s.count));angle=float(angle+float(float(float(row)*s.angle_step)+s.angle));break;}
        case 4:case 5:{angle=s.pattern==4?float(aim+0.f):0.f;angle=float(angle+float(pi/float(s.count)));angle=float(angle+ring(column,s.count));angle=float(angle+float(float(float(row)*s.angle_step)+s.angle));break;}
        case 6:angle=float(s.angle+float(random.signed_unit()*s.angle_step));break;
        case 7:speed=float(float(random.unit()*s.speed_step)+s.speed);angle=float(ring(column,s.count)+0.f);angle=float(angle+float(float(float(row)*s.angle_step)+s.angle));break;
        case 8:angle=float(s.angle+float(random.signed_unit()*s.angle_step));speed=float(float(random.unit()*s.speed_step)+s.speed);break;
        case 9:case 10:{float delta=ring(column,s.count);if(s.rows&1){delta=float(float(float(wrapping_add(row,1)/2)*s.angle_step)+delta);if(s.rows>1)speed=float(float(float(float(s.speed_step-s.speed)*float(u32(wrapping_add(row,1))&0xfffe))/float(s.rows-1))+s.speed);}else{delta=float(float(float(float(row/2)*s.angle_step)+float(s.angle_step*.5f))+delta);if(s.rows>1)speed=float(float(float(float(s.speed_step-s.speed)*float(u32(row)&0xfffe))/float(s.rows-1))+s.speed);}if(row&1)delta=float(delta*-1.f);if(s.pattern==9)delta=float(delta+aim);angle=float(s.angle+delta);break;}
        case 11:case 12:{float delta=ring(column,s.count);if(s.pattern==12)delta=float(delta+float(pi/float(s.count)));angle=float(float(s.angle+delta)+0.f);const float sine=float(std::sin(double(delta)));speed=float(float(1.f-float(std::fabs(sine)*s.speed_step))*speed);break;}
        default:break;
    }
    out.speed=speed;out.angle=normalize_angle(normalize_angle(float(angle+0.f)));out.position=s.position;
    if(s.radius!=0){const auto offset=polar(out.angle,s.radius);out.position.x=float(out.position.x+offset.x);out.position.y=float(offset.y+out.position.y);}out.position.z=.1f;
    const float x=float(out.position.x-player.x),y=float(out.position.y-player.y);
    if(minimum_distance_squared>0&&minimum_distance_squared>float(float(x*x)+float(y*y)))return false;
    const auto velocity=polar(angle,speed);out.velocity.x=velocity.x;out.velocity.y=velocity.y;return true;
}
}
