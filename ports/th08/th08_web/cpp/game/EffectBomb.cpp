#include "EffectBomb.hpp"
#include "GameMath.hpp"
namespace th08 {
i32 EffectBomb::pulsing(EffectState& e){e.geometry_dirty=1;e.segments=48;e.width=32;e.radius=(e.age.current&1)?72:64;e.angle=0;return 1;}
i32 EffectBomb::expanding(EffectState& e){e.radius=Scalar::add(e.radius,8);e.geometry_dirty=1;e.segments=12;e.width=32;return 1;}
i32 EffectBomb::ripple(EffectState& e,u32 variant){
    const auto inverse=number(1)-e.age.value()/number(40);
    const float amount=(number(1)-number((inverse*number(inverse.to_float())).to_float())).to_float();
    e.radius=Scalar::mul(amount,variant==3?192:256);e.geometry_dirty=1;
    if(variant==3){e.segments=8;e.width=8;return 1;}
    e.segments=variant?48:64;
    if(variant==0){e.frequency=5;e.angle_y=0;if(e.age.current<40)e.width=8;else{e.height=Scalar::mul(amount,64);e.width=Scalar::add(e.width,2);}}
    else{if(variant==1)e.frequency=0;e.height=Scalar::mul(amount,128);e.angle_y=variant==1?.7853981852531433f:-.7853981852531433f;e.width=e.age.current<40?8:Scalar::add(e.width,1.5f);}
    return 1;
}
i32 EffectBomb::quartic(EffectState& e){
    if(e.age.current<30){e.radius=192;e.segments=48;e.width=3;e.height=.0001f;e.angle_y=1.5707963705062866f;}
    else{const auto t=(e.age.value()-number(30))/number(30),square=t*number(t.to_float());const float amount=(square*number(square.to_float())).to_float();e.height=(number(192)*number(amount)+number(.0001f)).to_float();e.width=(number(80)*number(amount)+number(3)).to_float();}
    e.geometry_dirty=1;return 1;
}
i32 EffectBomb::burst(EffectState& e,bool rotating){
    if(rotating)e.angle=add_angle(e.angle,e.slot&1?.039269909262657166f:-.039269909262657166f);
    e.geometry_dirty=1;const i32 duration=rotating?50:40;
    if(e.age.current<duration){
        const float inverse=(number(1)-e.age.value()/Extended::from_int(duration)).to_float();
        e.width=(number(88)-e.age.value()*number(80)/Extended::from_int(duration)).to_float();
        if(rotating){const auto offset=Extended::from_int(wrapping_sub(e.slot,4))*number(32);const float distance=(offset+number(384)).to_float();e.radius=(offset+number(192)-number(distance)*number(inverse)*number(inverse)).to_float();}
        else e.radius=(number(192)-number(384)*number(inverse)*number(inverse)).to_float();
        e.segments=wrapping_sub(e.segments,1);return 1;
    }
    if(e.age.current!=duration)return 1;
    if(rotating)screen.create(ScreenEffectType::Shake,16,8,0,0,21);
    screen.create(ScreenEffectType::Flash,8,1,rotating?i32(0x8f6060f0):i32(0x8ff08080),0,21);
    float angle=Scalar::add(e.angle,.7853981852531433f);const float radius=Scalar::mul(e.radius,.7071067094802856f);
    AnmRenderer::texture_strip(e,e.vertices,e.segments*2+2,true);
    for(i32 i=0;i<4;++i){
        if(angle>=3.1415927410125732f)angle=Scalar::sub(angle,6.283185482025147f);
        const Vec2 position{(number((cosine(angle)*number(radius)).to_float())+number(e.center.x)).to_float(),(number((sine(angle)*number(radius)).to_float())+number(e.center.y)).to_float()};
        const float width=Scalar::mul(e.width,4);
        auto& hit=damage.rectangle(true,position,Scalar::mul(radius,8),width,60,rotating?100:70);hit.interval=rotating?2:4;
        angle=add_angle(angle,1.5707963705062866f);hit.angle=angle;
        auto& cancel=damage.rectangle(false,position,Scalar::mul(radius,4),width,6,rotating?150:100);cancel.angle=angle;
    }
    return 1;
}
}
