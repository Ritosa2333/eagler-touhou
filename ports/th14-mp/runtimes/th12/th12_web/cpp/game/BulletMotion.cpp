#include "BulletState.hpp"
#include "Movement.hpp"
#include <cmath>
namespace th11 {
namespace {
constexpr double pi=3.1415927410125732421875,tau=6.283185482025146484375;
void velocity(BulletState& b,float speed)noexcept {const auto v=polar(b.angle,speed);b.velocity.x=v.x;b.velocity.y=v.y;}
void add(Vec3& p,const Vec3& v,float rate)noexcept {
    p.x=float(double(p.x)+float(double(v.x)*rate));
    p.y=float(double(p.y)+float(double(v.y)*rate));
    p.z=float(double(p.z)+float(double(v.z)*rate));
}
void direction(BulletState& b)noexcept {if(std::abs(double(b.velocity.x))>.0001||std::abs(double(b.velocity.y))>.0001)b.angle=float(std::atan2(double(b.velocity.y),double(b.velocity.x)));}
bool sound(BulletState& b,BulletEnvironment& e) {return b.transform_sound<0||e.sound(b.transform_sound,0,false);}
}
float bullet_aim(const Vec3& p,const Vec3& player)noexcept {
    const float x=float(double(player.x)-p.x),y=float(double(player.y)-p.y);
    return x==0&&y==0?1.57079637050628662109375f:float(std::atan2(double(y),double(x)));
}
i32 bullet_motion(BulletState& b,u32 kind,BulletEnvironment& e)noexcept {
    switch(kind){
    case 1:{auto& s=b.motion[0];if(s.timer.current<=16){const float v=float(5-double(s.timer.fractional)*5*.0625);velocity(b,float(double(v)+b.speed));}else b.active_transforms^=1;s.timer.tick();break;}
    case 4:{auto& s=b.motion[1];if(s.timer.current<s.duration){b.speed=float(double(s.a)*e.rate+b.speed);add(b.velocity,s.vector,e.rate);direction(b);}else b.active_transforms&=~4u;s.timer.tick();break;}
    case 8:{auto& s=b.motion[2];if(s.timer.current<s.duration){b.angle=normalize_angle(float(double(b.angle)+float(double(s.b)*e.rate)));b.speed=float(double(s.a)*e.rate+b.speed);velocity(b,b.speed);}else b.active_transforms&=~8u;s.timer.tick();break;}
    case 0x10:case 0x20:case 0x40:{auto& s=b.motion[3];float v;
        if(s.timer.current<s.duration)v=float(double(b.speed)-double(s.timer.fractional)*b.speed/s.duration);
        else {if(!sound(b,e))return -2;s.count=wrapping_add(s.count,1);if(s.count>=s.limit)b.active_transforms&=~kind;
            b.angle=kind==0x10?float(double(s.b)+b.angle):kind==0x40?s.b:normalize_angle(float(double(bullet_aim(b.position,e.player))+s.b));
            v=b.speed=s.a;s.timer.set(0,&e.rate);}
        velocity(b,v);s.timer.tick();break;}
    case 0x100:{auto& s=b.motion[4];const auto& p=b.position;
        if(!(p.x>-192&&p.x<192&&p.y>0&&p.y<448)){
            bool reflected=false;
            if((s.count&1)&&b.position.y<0){b.position.y=-b.position.y;b.angle=-b.angle;reflected=true;}
            if((s.count&2)&&b.position.y>=448){b.position.y=float((448-double(b.position.y))+448);b.angle=-b.angle;reflected=true;}
            if((s.count&8)&&b.position.x>=192){b.position.x=float(384-double(b.position.x));b.angle=normalize_angle(float(-double(b.angle)-pi));reflected=true;}
            if((s.count&4)&&b.position.x<-192){b.position.x=float(-384-double(b.position.x));b.angle=normalize_angle(float(-double(b.angle)-pi));reflected=true;}
            if(s.a>-990)b.speed=s.a;velocity(b,b.speed);
            if(reflected){s.duration=wrapping_add(s.duration,1);if(!sound(b,e))return -2;}
            if(s.duration>=s.limit)b.active_transforms&=~0x100u;
        }break;}
    case 0x1000:{auto& t=b.motion[5].timer;if(t.current<1)b.active_transforms^=0x1000;else t.advance(-1);break;}
    case 0x20000:case 0x40000:{if(!b.vm.sprite)return -2;const double w=b.vm.sprite->width,h=b.vm.sprite->height;
        if(double(b.position.x)+w*.5>-192&&double(b.position.x)-w*.5<192&&double(b.position.y)+h*.5>0&&double(b.position.y)-h*.5<448)break;
        bool wrapped=false;auto& t=b.motion[kind==0x20000?6:7].timer;
        if(kind==0x20000){if(b.position.x<-192){b.position.x=float(w+384+b.position.x);wrapped=true;}else if(b.position.x>192){b.position.x=float(double(b.position.x)-(w+384));wrapped=true;}}
        else {if(b.position.y<0){b.position.y=float(h+448+b.position.y);wrapped=true;}else if(b.position.y>448){b.position.y=float(double(b.position.y)-(h+448));wrapped=true;}}
        if(wrapped){t.advance(-1);if(!sound(b,e))return -2;}if(t.current<1)b.active_transforms^=kind;break;}
    case 0x800000:{auto& s=b.motion[8];if(s.timer.current<s.duration){
        const float target=normalize_angle(float(double(s.b)+bullet_aim(b.position,e.player)));
        const float diff=double(target)-b.angle>pi?float(double(target)-(double(b.angle)+tau)):double(b.angle)-target>pi?float(double(target)-(double(b.angle)-tau)):float(double(target)-b.angle);
        const float delta=float(double(diff)*s.a*e.rate);b.angle=normalize_angle(float(double(b.angle)+delta));velocity(b,b.speed);
        }else b.active_transforms&=~0x800000u;s.timer.tick();break;}
    case 0x2000000:{auto& s=b.motion[9];if(s.timer.current<s.duration){const auto p=sample(b.position_interpolation,&e.rate);b.velocity={float(double(p.x)-b.position.x),float(double(p.y)-b.position.y),float(double(p.z)-b.position.z)};direction(b);}
        else {b.active_transforms&=~0x2000000u;b.speed=s.a;b.position=s.vector;velocity(b,b.speed);}b.velocity.z=0;s.timer.tick();break;}
    case 0x8000000:{auto& s=b.motion[10];if(s.timer.current<s.duration)add(b.position,s.vector,e.rate);else b.active_transforms&=0xfffffff5u;
        // The original resets this timer and clears bits 2/8 on expiry.
        s.timer.set(0,&e.rate);break;}
    default:return -2;
    }return 0;
}
}
