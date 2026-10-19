#include "BombSanae.hpp"
#include <cmath>
namespace th15 {
bool BombSanae::create(u32& handle,i32 script){handle=context.animations.create(context.player_resource,script,-1,0,position);if(!handle){error=context.animations.error;return false;}context.animations.registry.find(handle)->visual.inherited_color=0;return true;}
bool BombSanae::interrupt(u32 handle,i32 label){if(context.animations.interrupt(handle,label))return true;error=context.animations.error;return false;}
bool BombSanae::start(){position=context.motion.position;angle=-1.5707963705062866f;if(!context.world.sound(30,true)){error="Sanae bomb audio failed";return false;}if(!create(field,16)||!create(aura,20))return false;invalidate_spell();context.life.invulnerability.set(120);context.enemies.uses=wrapping_add(context.enemies.uses,1);if(!context.world.shake({3,60,240,30})){error="Sanae bomb shake failed";return false;}return true;}
bool BombSanae::frame(bool& finished){
    auto* vm=context.animations.registry.find(field);if(!vm)field=0;context.life.invulnerability.set(40);if(!vm){if(!interrupt(aura,1))return false;finished=true;return true;}
    if(age.current==180){if(!context.world.sound(6,true)||!context.world.shake({4,1,60,10})){error="Sanae bomb burst effect failed";return false;}}
    if(age.current==0||age.current==180){if(damage.circle(position,age.current==0?16.f:80.f,age.current==0?0.35555556416511536f:20.f,age.current==0?180:100,age.current==0?13:80)<=0){error=damage.error;return false;}}
    if(age.current>=30){Vec3 p;if(age.current<250){const float direction=float(random.signed_unit()*3.1415927410125732f),radius=float(random.unit()*vm->visual.scale.x);p={float(position.x+float(std::cos(double(direction))*double(radius))),float(position.y+float(std::sin(double(direction))*double(radius))),float(position.z+0.f)};}else p={float(random.signed_unit()*192.f),float(random.unit()*448.f),0};
        const u32 tracked=effects.trail(p,visual_random);if(!(tracked&0x80000000u)){error=effects.error.empty()?"Sanae bomb effect tracking failed":effects.error;return false;}auto* effect=context.animations.registry.find(effects.handles[tracked&0xffff]);if(!effect){error="Sanae bomb particle unavailable";return false;}effect->visual.flags=(effect->visual.flags&~0x1c0u)|0x20;
    }
    if(age.current>=400){if(!interrupt(aura,1)||!interrupt(field,1))return false;field=0;return true;}
    float radius;if(age.current<180)radius=float(float(float(age.fractional*48.f)/180.f)+16.f);else if(age.current<200)radius=float(float(float(float(age.fractional-180.f)*416.f)/20.f)+64.f);else radius=480;
    if(!context.world.cancel_bullets(position,radius,(~context.spell.flags)&1)||!context.world.cancel_lasers(position,radius,(~context.spell.flags)&1,true)){error="Sanae bomb cancellation failed";return false;}return true;
}
}
