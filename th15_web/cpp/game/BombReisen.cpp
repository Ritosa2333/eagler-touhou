#include "BombReisen.hpp"
namespace th15 {
bool BombReisen::practice_shields(i32 count){
    if(count<1||count>3||barrier||context.session.bomb_state)return false;
    // Purple MakeReisenShieldANM + 43c68c: no aura, stock decrement,
    // spell invalidation or startup sound. Start beyond frame 30.
    position=context.motion.position;if(!create(barrier,10))return false;
    auto* root=context.animations.registry.find(barrier);if(!root)return false;
    if(count<=2)for(auto* child:context.animations.registry.children(*root))if(child->source_script==11||(count==1&&child->source_script==12))if(!context.animations.registry.destroy_tree(*child))return false;
    context.session.bomb_state=1;age.set(31);charges=count;return true;
}
bool BombReisen::create(u32& handle,i32 script){handle=context.animations.create(context.player_resource,script,-1,0,position);if(!handle){error=context.animations.error;return false;}context.animations.registry.find(handle)->visual.inherited_color=0;return true;}
bool BombReisen::interrupt(u32 handle,i32 label){if(context.animations.interrupt(handle,label,false))return true;error=context.animations.error;return false;}
void BombReisen::hitbox(float size)noexcept{context.header.hitbox=size;const float half=float(size*.5f);context.bounds.hit_half_size.x=context.bounds.hit_half_size.y=half;}
bool BombReisen::start(){position=context.motion.position;angle=-1.5707963705062866f;if(!create(barrier,10))return false;invalidate_spell();context.life.invulnerability.set(120);context.enemies.uses=wrapping_add(context.enemies.uses,1);if(!create(aura,17))return false;charges=3;context.life.invulnerability.set(30);return true;}
bool BombReisen::frame(bool& finished){
    const auto cancel=[&](float radius){const i32 reward=(~context.spell.flags)&1;if(!context.world.cancel_bullets(position,radius,reward)||!context.world.cancel_lasers(position,radius,(~context.spell.flags)&1,true)){error="Reisen bomb cancellation failed";return false;}return true;};
    if(age.current<31){if(!cancel(float(age.fractional*4.f)))return false;if(age.current==30&&!context.world.sound(54,true)){error="Reisen barrier audio failed";return false;}}
    else{hitbox(float(float(float(charges)*1.5f)+3.f));if(context.life.state==4){position=context.motion.position;context.life.invulnerability.set(30);age.set(0);charges=wrapping_sub(charges,1);if(!interrupt(barrier,wrapping_add(charges,7)))return false;context.life.cancel_death();context.life.invulnerability.set(30);invalidate_spell();if(!cancel(64.f))return false;
            if(charges==0){hitbox(3.f);if(!interrupt(barrier,1)||(aura&&!interrupt(aura,1)))return false;finished=true;return true;}}}
    if(auto* vm=context.animations.registry.find(barrier))vm->visual.translation=context.motion.position;
    if(!context.animations.registry.find(barrier))barrier=0;if(!context.animations.registry.find(aura))aura=0;return true;
}
}
