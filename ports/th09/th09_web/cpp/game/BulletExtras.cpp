#include "BulletExtras.hpp"
#include "GameMath.hpp"
#include <cmath>
#include <initializer_list>
namespace th09 {
namespace {
void velocity(Vec3& result,float angle,float speed){result.x=float(std::cos(double(angle))*double(speed));result.y=float(std::sin(double(angle))*double(speed));}
}
bool BulletExtras::begin(Bullet& b){
    while(b.extra_index>=0&&b.extra_index<18){
        const auto& e=b.extras[b.extra_index];const u32 flag=e.flags;
        if(!flag||(!e.mode&&b.active_extras))return true;
        if(!(b.available_extras&flag)){++b.extra_index;continue;}
        switch(flag){
        case 1:b.active_extras|=flag;b.boost.time.reset();b.boost.counter=0;break;
        case 0x10:
            b.active_extras|=flag;b.linear.magnitude=e.float_a;b.linear.direction=e.float_b<=-990.f?b.direction:e.float_b;
            b.linear.time.reset();b.linear.duration=e.integer_a;velocity(b.linear.velocity,b.linear.direction,timing.rate*b.linear.magnitude);
            if(b.extra_index)sound(b);break;
        case 0x20:
            b.active_extras|=flag;b.polar.magnitude=e.float_a;b.polar.rotation=e.float_b;b.polar.time.reset();b.polar.duration=e.integer_a;
            if(b.extra_index)sound(b);break;
        case 0x40:case 0x80:case 0x100:
            b.active_extras|=flag;b.turn.angle=e.float_a;b.turn.speed=e.float_b<=-999.f?b.speed:e.float_b;
            b.turn.time.reset();b.turn.duration=e.integer_a;b.turn.repetitions=e.integer_b;b.turn.count=0;break;
        case 0x400:case 0x800:
            b.active_extras|=flag;b.bounce.speed=e.float_a<0?b.speed:e.float_a;b.bounce.limit=e.integer_a;b.bounce.count=0;break;
        case 0x2000:b.offscreen_grace=e.integer_a;++b.extra_index;continue;
        case 0x4000:actions.change_type(b,e.integer_a,e.integer_b);++b.extra_index;continue;
        case 0x20000:b.active_extras|=flag;b.delay.reset(e.integer_a);break;
        case 0x40000:b.state=5;break;
        case 0x80000:actions.play_positioned_sound(e.integer_a,b.position.x);++b.extra_index;continue;
        case 0x400000:case 0x800000:b.active_extras|=flag;b.wrap.reset(e.integer_a);break;
        case 0x1000000:{
            if(b.extra_index>=17)return false;const auto& next=b.extras[b.extra_index+1];const u32 packed=u32(e.integer_a);
            BulletEmission child;child.position=b.position;child.sprite=i16((packed>>16)&255);child.color=i16((packed>>8)&255);
            child.pattern=i16((packed>>24)&127);child.extra_index=i32(packed&255);
            child.count=i16(e.integer_b);child.layers=i16(next.integer_a);child.speed=e.float_a;child.ending_speed=e.float_b;
            child.angle=next.float_a;child.spread=next.float_b;child.flags=u32(next.integer_b);
            std::memcpy(child.extras,b.extras,sizeof(b.extras));++b.extra_index;
            if(!actions.emit_children(child))return false;++b.extra_index;
            if(packed&0x80000000u){b.state=5;++b.extra_index;return true;}continue;
        }
        }
        ++b.extra_index;return true;
    }
    return b.extra_index>=0;
}
void BulletExtras::update(Bullet& b){
    const float rate=timing.rate;
    if(b.active_extras&1){
        if(b.boost.time.current<=16)velocity(b.velocity,b.direction,((5.f-b.boost.time.time*.3125f)+b.speed)*rate);
        else b.active_extras^=1;b.boost.time.tick(timing);
    }
    if(b.active_extras&0x10){
        auto& a=b.linear;if(a.time.current>=a.duration)b.active_extras&=~0x10u;
        else {b.velocity.x=rate*a.velocity.x+b.velocity.x;b.velocity.y=rate*a.velocity.y+b.velocity.y;b.velocity.z=rate*a.velocity.z+b.velocity.z;
            if(std::fabs(b.velocity.x)>.0001f||std::fabs(b.velocity.y)>.0001f)b.direction=float(std::atan2(double(b.velocity.y),double(b.velocity.x)));}
        a.time.tick(timing);
    }
    if(b.active_extras&0x20){
        auto& a=b.polar;if(a.time.current>=a.duration)b.active_extras&=~0x20u;
        else {b.direction=float(add_angle(b.direction,rate*a.rotation));b.speed=rate*a.magnitude+b.speed;velocity(b.velocity,b.direction,b.speed*rate);}
        a.time.tick(timing);
    }
    // Order is observable when parallel extras share the same turning state.
    for(u32 flag:{0x40u,0x100u,0x80u})if(b.active_extras&flag){
        auto& a=b.turn;float speed;
        if(a.time.current<a.duration)speed=b.speed-(a.time.time*b.speed)/float(a.duration);
        else {
            sound(b);a.count=wrapping_add(a.count,1);if(a.count>=a.repetitions)b.active_extras&=~flag;
            if(flag==0x40)b.direction=a.angle+b.direction;
            else if(flag==0x100)b.direction=a.angle;
            else {const float dx=player.x-b.position.x,dy=player.y-b.position.y;
                const float aim=(dx==0&&dy==0)?1.5707963705062866f:float(std::atan2(double(dy),double(dx)));
                b.direction=float(add_angle(aim,a.angle));}
            b.speed=a.speed;a.time.reset();speed=a.speed;
        }
        velocity(b.velocity,b.direction,rate*speed);a.time.tick(timing);
    }
    if((b.active_extras&0xc00)&&!b.intersects_field()){
        sound(b);
        if(b.position.x < -144.f||b.position.x>=144.f)b.direction=float(add_angle(-b.direction-3.1415927410125732f,0));
        if(b.position.y<0.f||(b.position.y>=448.f&&(b.active_extras&0x400)))b.direction=-b.direction;
        b.speed=b.bounce.speed;velocity(b.velocity,b.direction,b.bounce.speed*rate);
        b.bounce.count=wrapping_add(b.bounce.count,1);if(b.bounce.count>=b.bounce.limit)b.active_extras&=~0xc00u;
    }
    for(u32 flag:{0x400000u,0x800000u})if(b.active_extras&flag){
        float& coordinate=flag==0x400000?b.position.x:b.position.y;const float extent=flag==0x400000?384.f:448.f;
        if(coordinate<0.f)coordinate+=extent;else if(coordinate>extent)coordinate-=extent;
        if(b.wrap.current<=0)b.active_extras^=flag;else b.wrap.decrement(1,timing);
    }
    if(b.active_extras&0x20000){if(b.delay.current<=0)b.active_extras^=0x20000;else b.delay.decrement(1,timing);}
}
}
