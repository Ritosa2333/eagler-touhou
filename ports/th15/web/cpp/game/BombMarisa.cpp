#include "BombMarisa.hpp"
#include <cmath>
namespace th15 {
namespace {
Vec3 local_position(const AnmVm& vm)noexcept{const auto& v=vm.visual;const auto& p=vm.variables.position;return {float(float(p.x+v.translation.x)+v.child_anchor.x),float(float(v.child_anchor.y+p.y)+v.translation.y),float(float(v.child_anchor.z+p.z)+v.translation.z)};}
bool add_parent(const AnmVm& vm,Vec3& p,u32 depth);
bool game_position(const AnmVm& vm,Vec3& p,u32 depth){p=local_position(vm);return add_parent(vm,p,depth);}
bool add_parent(const AnmVm& vm,Vec3& p,u32 depth){if(depth>=64)return false;if(vm.rotation_parent&&!(vm.visual.render_flags&0x10000)){const auto& parent=*vm.rotation_parent;if(vm.visual.render_flags&0x800000){const float s=float(std::sin(double(parent.variables.rotation.z))),c=float(std::cos(double(parent.variables.rotation.z))),x=p.x;p.x=float(float(x*c)-float(p.y*s));p.y=float(float(p.y*c)+float(x*s));}Vec3 origin;if(!game_position(parent,origin,depth+1))return false;p={float(p.x+origin.x),float(p.y+origin.y),float(p.z+origin.z)};}return true;}
}
bool BombMarisa::create(u32& handle,i32 script){handle=context.animations.create(context.player_resource,script,-1,0,position);if(!handle){error=context.animations.error;return false;}context.animations.registry.find(handle)->visual.inherited_color=0;return true;}
bool BombMarisa::interrupt(u32 handle,i32 label){if(context.animations.interrupt(handle,label))return true;error=context.animations.error;return false;}
bool BombMarisa::start(){position=context.motion.position;angle=-1.5707963705062866f;if(!context.world.sound(49,true)){error="Marisa bomb audio failed";return false;}if(!create(beam,16))return false;invalidate_spell();context.life.invulnerability.set(120);context.enemies.uses=wrapping_add(context.enemies.uses,1);if(!context.world.shake({3,60,240,30})){error="Marisa bomb shake failed";return false;}if(!create(aura,24))return false;context.motion.behavior_flags|=4;return true;}
bool BombMarisa::cancel(){for(i32 i=0;;i++){auto* root=context.animations.registry.find(beam);if(!root){beam=0;return true;}auto* segment=context.animations.registry.find_child_script(*root,23,i);if(!segment)return true;const auto& v=segment->visual;const Vec2 size{float(v.scale.x*48.f),float(v.scale.y*160.f)};Vec3 p=local_position(*segment);if(!add_parent(*segment,p,0)){error="Marisa beam ancestry too deep";return false;}if(!context.world.cancel_bullets_rectangle(p,size,angle,(~context.spell.flags)&1)||!context.world.cancel_lasers_rectangle(p,size,angle,(~context.spell.flags)&1,true)){error="Marisa beam cancellation failed";return false;}}}
bool BombMarisa::frame(bool& finished){
    auto* vm=context.animations.registry.find(beam);if(!vm)beam=0;context.life.invulnerability.set(40);if(!vm){if(!interrupt(aura,1))return false;finished=true;return true;}if(age.current>300)return true;if(age.current==300){if(!interrupt(beam,1)||!interrupt(aura,1))return false;context.motion.behavior_flags&=~4u;context.motion.movement_scale=1;}
    vm->visual.flags|=4;vm->variables.rotation.z=angle;if(context.motion.velocity.x<0)angle=float(angle-0.0026179938577115536f);else if(context.motion.velocity.x>0)angle=float(angle+0.0026179938577115536f);context.motion.movement_scale=.2f;position=context.motion.position;
    if(age.current!=age.previous&&age.current%3==0){constexpr float distances[]={208,240,304},heights[]={32,128,256};for(u32 i=0;i<3;i++){const Vec3 p{float(position.x+float(std::cos(double(angle))*double(distances[i]))),float(position.y+float(std::sin(double(angle))*double(distances[i]))),float(position.z+0.f)};const i32 id=damage.rectangle(p,angle,{512,heights[i]},i==0?age.current%3:0,i==0?60:20);auto* source=damage.find(id);if(!source){error=damage.error;return false;}source->flags|=4;}}
    if(auto* root=context.animations.registry.find(beam))root->visual.translation=position;if(auto* root=context.animations.registry.find(aura))root->visual.translation=position;return cancel();
}
}
