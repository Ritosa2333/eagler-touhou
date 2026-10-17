#include "PlayerBombPatterns.hpp"
#include "AnmTransitions.hpp"
namespace th08 {
namespace {
Vec3 interpolate(const Vec3& from,const Vec3& to,float amount){
    const Vec3 delta{Scalar::sub(to.x,from.x),Scalar::sub(to.y,from.y),Scalar::sub(to.z,from.z)};
    const Vec3 scaled{Scalar::mul(delta.x,amount),Scalar::mul(delta.y,amount),Scalar::mul(delta.z,amount)};
    return {Scalar::add(scaled.x,from.x),Scalar::add(scaled.y,from.y),Scalar::add(scaled.z,from.z)};
}
}
void PlayerBombPatterns::remilia(bool last){
    if(!frame.options)return;const i32 time=bomb.timer.current;const bool changed=bomb.timer.changed();
    if(changed&&time==0){begin(last?PlayerBombKind::RemiliaLast:PlayerBombKind::Remilia,1,last?280:240,last?320:290,last);actions.sound(13,0);
        for(u32 i=0;i<4;++i)objects.objects[i].position=frame.options[i].center;actions.sound(6,0);bomb.sequence=0;movement.multiplier={0,0};}
    Vec3 target=movement.position;target.x=Scalar::sub(target.x,32);
    Vec3 targets[4];targets[0]=target;target.x=Scalar::add(target.x,32);target.y=Scalar::sub(target.y,32);targets[1]=target;
    target.y=Scalar::add(target.y,64);targets[2]=target;target.x=Scalar::add(target.x,32);target.y=Scalar::sub(target.y,32);targets[3]=target;
    if(time<60){const auto fraction=bomb.timer.value()/number(60);const float amount=(fraction*number(fraction.to_float())).to_float();for(u32 i=0;i<4;++i)frame.options[i].center=interpolate(objects.objects[i].position,targets[i],amount);return;}
    if(changed&&time==60){movement.multiplier={last?3.0f:2.0f,last?3.0f:2.0f};for(u32 i=0;i<4;++i)frame.options[i].animation.pendingInterrupt=2;}
    auto& object=objects.objects[0];const Vec2 position{movement.position.x,movement.position.y};
    if(last)object.cancel=&regions.circle(false,position,96,0,6,0);
    for(u32 i=0;i<4;++i)frame.options[i].center=targets[i];
    if(!last)object.cancel=&regions.circle(false,position,96,0,6,0);
    if(changed&&time%10==0){if(auto* e=actions.fixed_effect(53,movement.position,bomb.sequence%4+4,0xffffffff)){
        e->segments=32;e->frequency=4;const float height=rng.range(128).to_float();animation_position_transition(*e,30,4,{0,0,0},{192,height,0});animation_scale_transition(*e,30,1,{64,0},{last?128.0f:64.0f,0});
        animation_alpha_transition(*e,30,3,255,0);animation_rgb_transition(*e,30,0,0xffffffff,0xffff0000);bomb.sequence=wrapping_add(bomb.sequence,1);actions.panned_sound(17,movement.position.x);actions.step_animation(*e);
    }}
    if(frame.shooting_timer.current>=5){object.cancel=&regions.rectangle(false,position,96,800,6,0);object.cancel=&regions.rectangle(false,position,800,96,6,0);}
    if(changed&&time==(last?279:239))for(u32 i=0;i<4;++i){frame.options[i].state=1;frame.options[i].timer.set(0);}
}
}
