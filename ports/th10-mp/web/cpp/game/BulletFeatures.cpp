#include "BulletFrame.hpp"
#include "GameMath.hpp"
#include <cmath>
namespace th10 {
namespace {
void velocity(EnemyBulletMotion& motion,float speed){const auto xy=polar(motion.angle,speed);motion.velocity.x=xy.x;motion.velocity.y=xy.y;}
void reset_modifier(ProjectileModifier& modifier,const float* rate){if(!(modifier.timer_flags&1)){modifier.timer_flags|=1;modifier.timer.rate=rate;}modifier.timer.initialize(-1);}
float aim(const Vec3& from,const Vec3& to){const auto x=number(to.x)-number(from.x),y=number(to.y)-number(from.y);return (x==number(0.f)&&y==number(0.f)?number(1.57079637050628662109375f):angle_to(y,x)).to_float();}
}
// 0x4074b0..0x40802f. Modifier timers and phase transitions retain the order
// used by the original instruction stream, including completion-frame ticks.
void EnemyBullet::update_feature(BulletFeature feature,BulletFrameEnvironment& env){
    if(feature==BulletFeature::SpawnAcceleration){
        auto& m=modifiers[0];if(m.timer.current<=16)velocity(motion,(number(5.f)-number(m.timer.fractional)*number(.3125f)+number(motion.speed)).to_float());else active_features^=1;m.timer.tick();
    }else if(feature==BulletFeature::VectorAcceleration){
        auto& m=modifiers[1];
        if(m.timer.current>=m.duration)active_features&=~0x10u;
        else{
            motion.speed=(number(*env.default_rate)*number(m.first)+number(motion.speed)).to_float();
            const auto x=number(*env.default_rate)*number(m.vector.x),y=number(*env.default_rate)*number(m.vector.y);const float z=Scalar::mul(*env.default_rate,m.vector.z);
            motion.velocity.x=(x+number(motion.velocity.x)).to_float();motion.velocity.y=(y+number(motion.velocity.y)).to_float();motion.velocity.z=Scalar::add(z,motion.velocity.z);
            if(std::fabs(motion.velocity.x)>.0001f||std::fabs(motion.velocity.y)>.0001f)motion.angle=angle_to(number(motion.velocity.y),number(motion.velocity.x)).to_float();
        }
        m.timer.tick();
    }else if(feature==BulletFeature::AngularAcceleration){
        auto& m=modifiers[2];if(m.timer.current>=m.duration)active_features&=~0x20u;
        else{motion.angle=add_angle(motion.angle,Scalar::mul(*env.default_rate,m.second)).to_float();motion.speed=(number(*env.default_rate)*number(m.first)+number(motion.speed)).to_float();velocity(motion,motion.speed);}m.timer.tick();
    }else if(feature==BulletFeature::Turn||feature==BulletFeature::TurnToAngle||feature==BulletFeature::TurnAimed){
        auto& m=modifiers[3];float speed;
        if(m.timer.current>=m.duration){
            if(turn_sound>=0)env.play_turn_sound(turn_sound);
            m.iteration=wrapping_add(m.iteration,1);if(m.iteration>=m.count)active_features&=~(feature==BulletFeature::Turn?0x40u:feature==BulletFeature::TurnToAngle?0x100u:0x80u);
            if(feature==BulletFeature::Turn)motion.angle=Scalar::add(m.second,motion.angle);
            else if(feature==BulletFeature::TurnToAngle)motion.angle=m.second;
            else motion.angle=add_angle(aim(motion.position,*env.player_position),m.second).to_float();
            speed=motion.speed=m.first;reset_modifier(m,env.default_rate);
        }else speed=(number(motion.speed)-number(motion.speed)*number(m.timer.fractional)/Extended::from_int(m.duration)).to_float();
        velocity(motion,speed);m.timer.tick();
    }else if(feature==BulletFeature::Reflect){
        auto& m=modifiers[4];
        if(!bullet_outside_playfield(motion.position,0,0,false))return;
        if(turn_sound>=0)env.play_turn_sound(turn_sound);bool reflected=false;
        if(motion.position.x< -192.f||motion.position.x>=192.f){
            motion.angle=add_angle((-number(motion.angle)-number(3.1415927410125732421875f)).to_float(),0).to_float();reflected=true;
            motion.position.x=Scalar::sub(motion.position.x< -192.f?-384.f:384.f,motion.position.x);
        }
        if(!(active_features&0x8000000)&&(motion.position.y<0.f||(motion.position.y>=448.f&&(active_features&0x400)))){
            motion.angle=(-number(motion.angle)).to_float();reflected=true;
            motion.position.y=(motion.position.y<0.f?-number(motion.position.y):number(448.f)-number(motion.position.y)+number(448.f)).to_float();
        }
        if(m.first> -990.f)motion.speed=m.first;velocity(motion,motion.speed);
        if(reflected)m.duration=wrapping_add(m.duration,1);if(m.duration>=m.count)active_features&=~0x8000c00u;
    }else if(feature==BulletFeature::Homing){
        auto& m=modifiers[8];
        if(m.timer.current>=m.duration)active_features&=~0x4000000u;
        else{
            const float desired=add_angle(m.second,aim(motion.position,*env.player_position)).to_float();
            const float delta=(angle_difference(desired,motion.angle)*number(m.first)*number(*env.default_rate)).to_float();
            motion.angle=add_angle(motion.angle,delta).to_float();velocity(motion,motion.speed);
        }
        m.timer.tick();
    }else{
        const bool horizontal=feature==BulletFeature::HorizontalWrap;auto& m=modifiers[horizontal?6:7];
        if(!bullet_outside_playfield(motion.position,animation.sprite->width,animation.sprite->height,false))return;
        float& coordinate=horizontal?motion.position.x:motion.position.y;const float low=horizontal?-192.f:0.f,high=horizontal?192.f:448.f;
        const float dimension=horizontal?animation.sprite->width:animation.sprite->height,span=horizontal?384.f:448.f;bool wrapped=false;
        if(coordinate<low){coordinate=(number(dimension)+number(coordinate)+number(span)).to_float();wrapped=true;}
        else if(coordinate>high){coordinate=(number(coordinate)-(number(dimension)+number(span))).to_float();wrapped=true;}
        if(wrapped){m.timer.advance(-1);if(turn_sound>=0)env.play_turn_sound(turn_sound);}
        if(m.timer.current<=0)active_features^=horizontal?0x100000u:0x200000u;
    }
}
}
