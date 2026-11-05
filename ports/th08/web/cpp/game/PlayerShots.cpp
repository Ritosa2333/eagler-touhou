#include "PlayerShots.hpp"
#include "GameMath.hpp"
#include <cmath>
namespace th08 {
void PlayerShots::direction(PlayerShot& shot,float angle,float speed){
    shot.velocity.x=(cosine(angle)*number(speed)).to_float();shot.velocity.y=(sine(angle)*number(speed)).to_float();
}
bool PlayerShots::initialize(PlayerShot& shot,const ShotDefinition& d){
    failure=Failure::None;if(d.option<0||d.option>4){failure=Failure::InvalidOption;return false;}
    if(!actions){failure=Failure::MissingActions;return false;}
    shot.position=d.option?state.options[d.option-1]:state.position;
    shot.position.x=Scalar::add(shot.position.x,d.offset.x);shot.position.y=Scalar::add(shot.position.y,d.offset.y);shot.position.z=.495f;
    shot.size={d.size.x,d.size.y,1};shot.angle=d.angle;shot.speed=d.speed;direction(shot,d.angle,d.speed);shot.timer.set(0);
    shot.focused=state.focused;shot.kind=d.reserved22;shot.damage=d.damage;shot.animation_index=d.animation;
    if(d.sound>=0)actions->sound(d.sound,state.position.x);actions->animation(shot.animation,wrapping_add(d.animation,10));
    shot.gauge_bonus=state.youkai_bonus&&d.gauge>0;return true;
}
bool PlayerShots::create(PlayerShot& shot,i32 frame,const ShotDefinition& d){
    failure=Failure::None;
    if(d.create==ShotCreate::Laser){
        if(state.bomb||(state.game_flags&0x2000))return false;
        if(d.phase<0||d.phase>=3){failure=Failure::InvalidLaserSlot;return false;}
        auto& slot=state.lasers[d.phase];
        if(slot.shot){if(state.laser_definitions[d.phase]!=&d){slot.shot->animation.SetInterrupt(1);slot.shot=nullptr;}return false;}
        slot.timer.set(999);slot.shot=&shot;shot.laser_slot=d.phase;shot.option=d.option;shot.velocity.z=d.offset.x;shot.reserved448=d.offset.y;shot.interval=d.interval;
        if(!initialize(shot,d))return false;
        for(auto& history:shot.history)history.x=-999;shot.position.x=-999;state.laser_definitions[d.phase]=&d;return true;
    }
    if((d.create==ShotCreate::OutsideBomb||d.create==ShotCreate::OrbitAimed)&&state.bomb)return false;
    if(d.interval<=0){failure=Failure::InvalidInterval;return false;}if(frame%d.interval!=d.phase)return false;
    if(!initialize(shot,d))return false;
    switch(d.create){
    case ShotCreate::EnemyAimed:case ShotCreate::TargetAimed:{
        if(d.create==ShotCreate::EnemyAimed?!state.enemy_available:!(state.target.x>-100))break;
        const auto& target=d.create==ShotCreate::EnemyAimed?state.enemy_target:state.target;
        const float delta=Scalar::add(d.angle,1.5707963705062866f);
        const float dy=Scalar::sub(target.y,shot.position.y),dx=Scalar::sub(target.x,shot.position.x);
        const float angle=add_angle(Extended::from_double(std::atan2(double(dy),double(dx))).to_float(),delta);
        direction(shot,angle,Scalar::mul(d.speed,1.5f));shot.angle=angle;break;
    }
    case ShotCreate::OptionAimed:{const float angle=add_angle(state.option_angle,Scalar::add(d.angle,1.5707963705062866f));direction(shot,angle,d.speed);shot.angle=angle;break;}
    case ShotCreate::OrbitAimed:{const float angle=add_angle(state.orbit_angle,d.angle);direction(shot,angle,d.speed);shot.angle=angle;break;}
    case ShotCreate::RandomSpread:shot.angle=(rng.signed_unit()*number(3.1415927410125732f)/number(48)-number(1.5707963705062866f)).to_float();direction(shot,shot.angle,d.speed);break;
    default:break;
    }
    return true;
}
u32 PlayerShots::emit(const ShotStream& stream,i32 frame){
    failure=Failure::None;u32 index=0,count=0;if(stream.shots.empty())return 0;
    for(auto& shot:state.shots){if(shot.state)continue;
        while(index<stream.shots.size()){
            const auto& d=stream.shots[index++];const bool made=create(shot,frame,d);if(failure!=Failure::None)return count;
            if(made){shot.animation.flags|=0x2000;shot.state=1;shot.definition=&d;shot.update=d.update;shot.draw=d.draw;shot.hit=d.hit;++count;}
            if(index==stream.shots.size())return count;if(made)break;
        }
    }
    return count;
}
}
