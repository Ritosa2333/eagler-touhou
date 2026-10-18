#include "EffectSpace.hpp"
#include "GameMath.hpp"
#include "GraphicsMath.hpp"
namespace th08 {
namespace {
void add(Vec3& a,const Vec3& b){a.x=Scalar::add(a.x,b.x);a.y=Scalar::add(a.y,b.y);a.z=Scalar::add(a.z,b.z);}
Vec3 subtract(const Vec3& a,const Vec3& b){return {Scalar::sub(a.x,b.x),Scalar::sub(a.y,b.y),Scalar::sub(a.z,b.z)};}
void scale(Vec3& a,float s){a.x=Scalar::mul(a.x,s);a.y=Scalar::mul(a.y,s);a.z=Scalar::mul(a.z,s);}
Extended dot(const Vec3& a,const Vec3& b){return number(a.x)*number(b.x)+number(a.y)*number(b.y)+number(a.z)*number(b.z);}
}
i32 EffectSpace::ambient(EffectState& e){
    const auto& camera=environment.camera;const Vec3 negative{-camera.target_offset.x,-camera.target_offset.y,-camera.target_offset.z};
    e.world_position=camera.target_offset;add(e.world_position,camera.position);
    e.world_position.x=(number(negative.x)/number(2)+(rng.range(120)-number(60))+number(e.world_position.x)).to_float();
    e.world_position.y=(number(negative.y)/number(2)+(rng.range(200)-number(100))+number(e.world_position.y)).to_float();
    e.world_position.z=(number(negative.z)/number(2)+(rng.range(100)-number(100))+number(e.world_position.z)).to_float();
    e.velocity.x=(rng.range(.06f)-number(.03f)+number(e.parameters.x)).to_float();
    e.velocity.y=(rng.range(.06f)-number(.03f)+number(e.parameters.y)).to_float();
    e.velocity.z=(rng.range(.1f)+number(.03f)+number(e.parameters.z)).to_float();
    e.acceleration.x=(rng.range(.0002f)-number(.0001f)).to_float();e.acceleration.y=(rng.range(.0002f)-number(.0001f)).to_float();
    scale(e.velocity,timing.rate);scale(e.acceleration,timing.rate);e.layer=1;
    e.rotation.z=(rng.range(6.283185482025147f)-number(3.1415927410125732f)).to_float();
    e.rotation.x=(rng.range(.03141592815518379f)-number(.015707964077591896f)).to_float();return 0;
}
i32 EffectSpace::glow(EffectState& e,bool tall){
    const auto& camera=environment.camera;const Vec3 negative{-camera.target_offset.x,-camera.target_offset.y,-camera.target_offset.z};
    e.world_position=camera.target_offset;add(e.world_position,camera.position);
    e.world_position.x=(number(negative.x)/number(2)+rng.signed_range(60)+number(e.world_position.x)).to_float();
    e.world_position.y=(number(negative.y)/number(2)+(rng.signed_range(tall?200:100)-number(tall?200:50))+number(e.world_position.y)).to_float();
    e.world_position.z=(number(negative.z)/number(2)+(rng.range(100)-number(100))+number(e.world_position.z)).to_float();
    e.velocity.x=(rng.signed_range(.001f)+number(e.parameters.x)).to_float();
    e.velocity.y=(rng.signed_range(.03f)+number(tall?.4f:e.parameters.y)).to_float();
    e.velocity.z=(-rng.range(.1f)-number(.3f)+number(e.parameters.z)).to_float();
    e.acceleration={rng.signed_range(.0001f).to_float(),rng.signed_range(.0001f).to_float(),-.0003f};
    scale(e.velocity,timing.rate);scale(e.acceleration,timing.rate);e.layer=1;e.pos2.x=-9999;e.posInitial.x=0;e.posFinal={};e.rotateInitial={};return 0;
}
bool EffectSpace::advance(EffectState& e){
    add(e.velocity,e.acceleration);add(e.world_position,e.velocity);e.position=e.world_position;
    Vec3 v=subtract(e.position,environment.camera.position);GraphicsMath::normalize(v,v);
    return dot(environment.camera.unused24,v).to_float()>=.94f;
}
i32 EffectSpace::ambient_step(EffectState& e){if(!advance(e))return 0;e.rotation.z=add_angle(e.rotation.z,e.rotation.x);e.updateRotation=1;return e.position.z<0;}
void EffectSpace::track_boss(EffectState& e){
    if(e.pos2.x<=-9999){e.pos2=environment.boss;return;}
    auto delta=subtract(environment.boss,e.pos2);scale(delta,.1f);add(delta,e.pos2);e.pos2=delta;
}
i32 EffectSpace::glow_step(EffectState& e,bool tall){
    if(!advance(e))return 0;
    if((tall?environment.boss_present:environment.any_boss)&&environment.boss_active)track_boss(e);
    if(!tall){e.flag17=1;e.color2.r=(u32(e.color1.r)*environment.tint.r)>>8;e.color2.g=(u32(e.color1.g)*environment.tint.g)>>8;e.color2.b=(u32(e.color1.b)*environment.tint.b)>>8;e.color2.a=(u32(e.color1.a)*environment.tint.a)>>8;}
    return 1;
}
i32 EffectSpace::orbit_step(EffectState& e){
    Vec3 axis;GraphicsMath::normalize(axis,e.direction);
    const float sin=sine(e.angle).to_float();e.quaternion[0]=Scalar::mul(axis.x,sin);e.quaternion[1]=Scalar::mul(axis.y,sin);e.quaternion[2]=Scalar::mul(axis.z,sin);e.quaternion[3]=cosine(e.angle).to_float();
    Matrix4 rotation;GraphicsMath::quaternion(rotation,e.quaternion);
    Vec3 side{(number(axis.y)*number(1)-number(axis.z)*number(0)).to_float(),(number(axis.z)*number(0)-number(axis.x)*number(1)).to_float(),(number(axis.x)*number(0)-number(axis.y)*number(0)).to_float()};
    // The original degeneracy branch assigns to the axis local, not side;
    // its zero cross product therefore stays zero for a vertical axis.
    if(!(dot(side,side)<number(.00001f)))GraphicsMath::normalize(side,side);
    scale(side,e.radius);GraphicsMath::transform_coordinate(side,side,rotation);side.z=Scalar::mul(side.z,6);add(side,e.center);e.position=side;e.position.z=0;
    if(e.dying){e.fade_frames=i8(u8(e.fade_frames)+1);if(e.fade_frames>=16)return 0;
        const float fade=(number(1)-Extended::from_int(e.fade_frames)/number(16)).to_float();e.color1.a=u8((number(fade)*number(255)).truncate_int());e.scale.y=Scalar::sub(2,fade);e.scale.x=e.scale.y;
    }return 1;
}
void EffectSpace::projected(AnmVm& vm,Vec3& projected){
    if(!environment.dialogue&&!environment.transition){
        Vec3 position=projected;add(position,vm.posFinal);auto delta=subtract(vm.pos2,position);
        if(vm.pos2.x>-9999){delta.x=Scalar::add(delta.x,32);delta.y=Scalar::add(delta.y,16);delta.z=0;
            if(dot(delta,delta)<number(25600)){vm.posInitial.x=Scalar::add(vm.posInitial.x,.0005f);scale(delta,vm.posInitial.x);add(vm.posFinal,delta);}
        }
        delta=subtract(position,environment.player);delta.x=Scalar::sub(delta.x,32);delta.y=Scalar::sub(delta.y,16);delta.z=0;
        if(dot(delta,delta)<number(7744)){scale(delta,.02f);add(vm.posFinal,delta);}
    }
    add(projected,vm.posFinal);
}
}
