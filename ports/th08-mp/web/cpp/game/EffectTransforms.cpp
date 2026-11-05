#include "EffectTransforms.hpp"
#include "GameMath.hpp"
namespace th08 {
namespace {
void scale(Vec3& v,float s){v.x=Scalar::mul(v.x,s);v.y=Scalar::mul(v.y,s);v.z=Scalar::mul(v.z,s);}
void add(Vec3& a,const Vec3& b){a.x=Scalar::add(a.x,b.x);a.y=Scalar::add(a.y,b.y);a.z=Scalar::add(a.z,b.z);}
i32 inward_step(EffectState& e,float duration){const float distance=(number(256)-e.age.value()*number(256)/number(duration)).to_float();e.position=e.direction;scale(e.position,distance);add(e.position,e.center);return 1;}
}
// 425d70/425ea0: the reciprocal and both vector operations store floats.
// Moving the frame-rate multiplication before the division changes the bits.
i32 EffectTransforms::spark(EffectState& e,bool large){
    auto component=[&](){auto v=rng.range(256)-number(128);return (large?v*number(4)/number(33):v/number(12)).to_float();};
    e.velocity={component(),component(),0};
    e.acceleration={-e.velocity.x,-e.velocity.y,-e.velocity.z};
    scale(e.acceleration,Scalar::div(1,large?20:19));
    scale(e.velocity,timing.rate);scale(e.acceleration,timing.rate);return 0;
}
i32 EffectTransforms::accelerate(EffectState& e){add(e.position,e.velocity);add(e.velocity,e.acceleration);return 1;}
i32 EffectTransforms::orbit(EffectState& e){e.layer=2;e.direction={};e.radius=0;return 0;}
i32 EffectTransforms::inward(EffectState& e){
    e.center=e.position;e.center.z=0;const float angle=(rng.range(6.283185482025147f)-number(3.1415927410125732f)).to_float();
    e.direction={cosine(angle).to_float(),sine(angle).to_float(),0};return 0;
}
i32 EffectTransforms::inward60(EffectState& e){inward_step(e,60);e.position.z=0;return 1;}
i32 EffectTransforms::inward240(EffectState& e){return inward_step(e,240);}
i32 EffectTransforms::outward(EffectState& e){
    const float angle=e.parameters.x<=-990?(rng.range(6.283185482025147f)-number(3.1415927410125732f)).to_float():add_angle(e.parameters.x,0);
    e.center=e.position;e.center.z=0;e.direction={cosine(angle).to_float(),sine(angle).to_float(),0};
    scale(e.direction,(rng.range(1.5f)+number(1)).to_float());return 0;
}
i32 EffectTransforms::outward90(EffectState& e){
    const auto inverse=number(1)-number((e.age.value()/number(90)).to_float());
    const float amount=(number(1)-inverse*inverse).to_float();
    e.position=e.direction;scale(e.position,amount);scale(e.position,128);add(e.position,e.center);e.position.z=0;return 1;
}
i32 EffectTransforms::follow(EffectState& e,const Vec3& player){if(!e.currentInstruction)return 0;e.position=player;return 1;}
i32 EffectTransforms::edge(EffectState& e){
    const float x=(cosine(e.parameters.x)*number(256)).to_float(),y=(sine(e.parameters.x)*number(256)).to_float();
    e.position.x=Scalar::add(e.position.x,x);e.position.y=Scalar::add(e.position.y,y);
    e.rotation.z=add_angle(e.parameters.x,1.5707963705062866f);return 0;
}
i32 EffectTransforms::ring(EffectState& e){e.geometry_dirty=1;e.width=e.scale.x;e.radius=e.pos.x;return 1;}
i32 EffectTransforms::ring_detailed(EffectState& e){
    e.segments=e.intVar0;e.frequency=Extended::from_int(e.intVar1).to_float();e.width=e.scale.x;e.radius=e.pos.x;e.height=e.pos.y;e.angle=e.rotation.z;e.angle_y=e.rotation.y;e.geometry_dirty=1;return 1;
}
i32 EffectTransforms::ring_timed(EffectState& e){e.segments=32;e.width=e.scale.x;e.radius=e.pos.x;e.height=e.pos.y;e.geometry_dirty=1;return e.age.current<120;}
i32 EffectTransforms::ring_alpha(EffectState& e){e.geometry_dirty=1;e.width=e.scale.x;e.radius=e.pos.x;e.height=e.pos.y;e.angle=e.rotation.z;return e.color1.a!=0;}
i32 EffectTransforms::moon(EffectState& e){e.segments=e.intVar0;e.frequency=Extended::from_int(e.intVar1).to_float();e.width=e.scale.x;e.radius=e.floatVar1;e.angle=e.rotation.z;e.angle_y=e.rotation.y;e.geometry_dirty=1;e.center=e.pos;return 1;}
}
