#include "PlayerShots.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
i32 PlayerShots::laser_hit(PlayerShot& shot,const DamageQuery& q){
    shot.contact_this_frame=1;if(shot.contact_effect==0){if(!animations.interrupt(shot.animation,2)){error=animations.error;return -1;}shot.contact_effect=1;}auto* source=damage.find(shot.damage_source);if(!source){error="Laser hit damage source unavailable";return -1;}
    if(!q.rectangle){
        const float radius=float(float(source->size.y*.5f)+q.radius),dx=float(q.position.x-shot.motion.position.x),dy=float(q.position.y-shot.motion.position.y),angle=-shot.motion.angle,sine=float(std::sin(double(angle))),cosine=float(std::cos(double(angle)));float x=float(float(cosine*dx)-float(dy*sine));const float y=float(float(dy*cosine)+float(dx*sine));
        if(std::abs(y)<=radius&&-radius<=x&&(0<=x||float(float(x*x)+float(y*y))<=float(radius*radius))){const float t=float(y/radius),weight=float(std::sqrt(double(float(1.f-float(t*t)))));x=float(x-float(weight*radius));}shot.hitbox.x=float(x+8);if(shot.hitbox.x<0)shot.hitbox.x=0;source->size.x=shot.hitbox.x;
    }else{
        const Vec2 origin={shot.motion.position.x,shot.motion.position.y},target={q.position.x,q.position.y},expanded={float(q.size.x+source->size.y),float(q.size.y+source->size.y)};Vec2 near,far;
        if(!line_rectangle(origin,shot.motion.angle,target,expanded,q.angle,near,far)){const float x=float(target.x-origin.x),y=float(target.y-origin.y),distance=float(std::sqrt(double(float(float(x*x)+float(y*y)))));shot.hitbox.x=float(distance-24);if(shot.hitbox.x<0)shot.hitbox.x=0;source->size.x=float(shot.hitbox.x+16);}
        else{const float x=float(near.x-origin.x),y=float(near.y-origin.y),bearing=float(std::atan2(double(y),double(x)));float difference=float(bearing-shot.motion.angle);if(difference>3.1415927f)difference=float(bearing-float(shot.motion.angle+6.2831855f));else if(float(shot.motion.angle-bearing)>3.1415927f)difference=float(bearing-float(shot.motion.angle-6.2831855f));shot.hitbox.x=std::abs(difference)>=1.5707964f?8.f:float(float(std::sqrt(double(float(float(x*x)+float(y*y)))))+8.f);if(shot.hitbox.x<0)shot.hitbox.x=0;source->size.x=shot.hitbox.x;}
    }
    if(shot.age.current!=shot.age.previous&&shot.age.current%2==0){
        const double cosine=std::cos(double(shot.motion.angle)),sine=std::sin(double(shot.motion.angle));const Vec3 position={float(float(cosine*double(shot.hitbox.x))+shot.motion.position.x),float(float(sine*double(shot.hitbox.x))+shot.motion.position.y),0},velocity={float(cosine*64.),float(sine*64.),0};const u32 handle=animations.create(player_resource,8,-1,0,position);auto* vm=animations.registry.find(handle);if(!vm){error=animations.error;return -1;}if(track_effect)track_effect(handle);vm->visual.flags|=4;vm->variables.rotation.z=shot.motion.angle;const auto& delta=context.background_delta;vm->interpolators.position.begin(20,4,{delta.x,delta.y,delta.z},{velocity.x,velocity.y,velocity.z});vm->interpolators.position.control1={delta.x,delta.y,delta.z};vm->interpolators.position.control2={delta.x,delta.y,delta.z};
    }return shot.age.current!=shot.age.previous&&shot.age.current%4==0?source->damage:0;
}
}
