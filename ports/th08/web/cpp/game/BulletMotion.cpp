#include "BulletMotion.hpp"
#include "GameMath.hpp"
#include <cmath>
namespace th08 {
namespace {
void direction(Vec3& vector,float angle,float speed){vector.x=(cosine(angle)*number(speed)).to_float();vector.y=(sine(angle)*number(speed)).to_float();}
}
Extended bullet_aim_extended(const Vec3& origin,const Vec3& target)noexcept{
    const float x=Scalar::sub(target.x,origin.x),y=Scalar::sub(target.y,origin.y);
    if(x==0&&y==0)return number(1.5707963705062866f);
    return Extended::from_double(std::atan2(double(y),double(x)));
}
float bullet_aim(const Vec3& origin,const Vec3& target)noexcept{return bullet_aim_extended(origin,target).to_float();}
bool bullet_in_view(const Vec3& p,float width,float height)noexcept{
    return !(number(width)/number(2)+number(p.x)<number(0))&&!(number(384)<number(p.x)-number(width)/number(2))
        &&!(number(height)/number(2)+number(p.y)<number(0))&&!(number(448)<number(p.y)-number(height)/number(2));
}
void BulletMotion::step(BulletState& b,u32 flag){
    const auto rate=number(timing.rate);
    auto sound=[&](){if(b.transform_sound>=0&&actions)actions->sound(b.transform_sound,0,false);};
    switch(flag){
    case 1:{
        auto& s=b.extra_state[0];
        if(s.timer.current<=16){const float boost=(number(5)-s.timer.value()*number(5)/number(16)).to_float();direction(b.velocity,b.angle,((number(boost)+number(b.speed))*rate).to_float());}
        else b.extra_flags^=1;
        s.timer.tick(timing);break;
    }
    case 0x10:{
        auto& s=b.extra_state[1];
        if(s.timer.current>=s.integer_a)b.extra_flags&=~0x10u;
        else{
            b.velocity.x=(number(b.velocity.x)+number((number(s.vector.x)*rate).to_float())).to_float();
            b.velocity.y=(number(b.velocity.y)+number((number(s.vector.y)*rate).to_float())).to_float();
            b.velocity.z=(number(b.velocity.z)+number((number(s.vector.z)*rate).to_float())).to_float();
            if(std::fabs(b.velocity.x)>.0001f||std::fabs(b.velocity.y)>.0001f)b.angle=Extended::from_double(std::atan2(double(b.velocity.y),double(b.velocity.x))).to_float();
        }
        s.timer.tick(timing);break;
    }
    case 0x20:{
        auto& s=b.extra_state[2];
        if(s.timer.current>=s.integer_a)b.extra_flags&=~0x20u;
        else{b.angle=add_angle(b.angle,(rate*number(s.float_b)).to_float());b.speed=(rate*number(s.float_a)+number(b.speed)).to_float();direction(b.velocity,b.angle,(rate*number(b.speed)).to_float());}
        s.timer.tick(timing);break;
    }
    case 0x40:case 0x80:case 0x100:{
        auto& s=b.extra_state[3];float speed;
        if(s.timer.current>=s.integer_a){
            sound();s.integer_c=wrapping_add(s.integer_c,1);if(s.integer_c>=s.integer_b)b.extra_flags&=~flag;
            if(flag==0x40)b.angle=Scalar::add(b.angle,s.float_b);
            else if(flag==0x100)b.angle=s.float_b;
            else b.angle=add_angle(bullet_aim(b.position,player),s.float_b);
            speed=b.speed=s.float_a;s.timer.set(0);
        }else speed=(number(b.speed)-s.timer.value()*number(b.speed)/Extended::from_int(s.integer_a)).to_float();
        direction(b.velocity,b.angle,(number(speed)*rate).to_float());s.timer.tick(timing);break;
    }
    case 0xc00:case 0x400:case 0x800:{
        const auto* sprite=b.sprites.animation[0].loadedSprite;if(!sprite)return;
        if(!bullet_in_view(b.position,sprite->widthPx,sprite->heightPx)){
            sound();if(b.position.x<0||b.position.x>=384)b.angle=add_angle((-number(b.angle)-number(3.1415927410125732f)).to_float(),0);
            if(b.position.y<0||(b.position.y>=448&&(b.extra_flags&0x400)))b.angle=(-number(b.angle)).to_float();
            auto& s=b.extra_state[4];b.speed=s.float_a;direction(b.velocity,b.angle,(number(b.speed)*rate).to_float());s.integer_a=wrapping_add(s.integer_a,1);if(s.integer_a>=s.integer_b)b.extra_flags&=~0xc00u;
        }
        break;
    }
    case 0x400000:case 0x800000:{
        auto& position=flag==0x400000?b.position.x:b.position.y;const float size=flag==0x400000?384:448;
        if(position<0)position=Scalar::add(position,size);else if(position>size)position=Scalar::sub(position,size);
        auto& timer=b.extra_state[6].timer;if(timer.current<=0)b.extra_flags^=flag;else timer.decrement(1,timing);break;
    }
    case 0x20000:{auto& timer=b.extra_state[5].timer;if(timer.current<=0)b.extra_flags^=flag;else timer.decrement(1,timing);break;}
    default:break;
    }
}
void BulletMotion::update(BulletState& b){
    static constexpr u32 flags[]{1u,0x10u,0x20u,0x40u,0x100u,0x80u,0xc00u,0x400000u,0x800000u,0x20000u};
    for(const u32 flag:flags)if(b.extra_flags&flag)step(b,flag);
}
}
