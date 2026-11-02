#include "BombMarisa.hpp"
#include <cmath>
namespace th17 {
namespace {
Vec3 polar(float angle,float radius){return {float(std::cos(double(angle))*double(radius)),float(std::sin(double(angle))*double(radius)),0};}
Vec3 parent_position(AnmVm& vm,u32 depth=0){
    Vec3 p{float(float(vm.variables.position.x+vm.visual.translation.x)+vm.visual.child_anchor.x),float(float(vm.variables.position.y+vm.visual.translation.y)+vm.visual.child_anchor.y),float(float(vm.variables.position.z+vm.visual.translation.z)+vm.visual.child_anchor.z)};
    auto* parent=vm.rotation_parent;if(parent&&!(vm.visual.render_flags&0x10000)&&depth<64){if(vm.visual.render_flags&0x800000){const float s=float(std::sin(double(parent->variables.rotation.z))),c=float(std::cos(double(parent->variables.rotation.z))),x=p.x;p.x=float(float(x*c)-float(p.y*s));p.y=float(float(p.y*c)+float(x*s));}const auto q=parent_position(*parent,depth+1);p={float(p.x+q.x),float(q.y+p.y),float(q.z+p.z)};}return p;
}
}
bool BombMarisa::start(){position=context.motion.position;angle=-1.57079637050628662109375f;if(!sound(49,true))return false;main_handle=animation(context.session.animal==1?29:22,position);if(!main_handle)return false;invalidate_spell();context.invulnerability.set(120);context.session.roaring_breaks=wrapping_add(context.session.roaring_breaks,1);if(!context.world.screen_effect({8,3,60,context.session.animal==1?420:240,30}))return fail("Bomb screen effect unavailable");aura_handle=animation(36,position);context.motion.behavior_flags|=4;return aura_handle!=0;}
bool BombMarisa::frame(bool& finished){
    auto* vm=context.animations.registry.find(main_handle);if(!vm)main_handle=0;context.invulnerability.set(40);
    if(!vm){finished=true;return interrupt(aura_handle);}const i32 duration=context.session.animal==1?450:300;if(age.current>duration)return true;
    if(age.current==duration){if(!interrupt(main_handle)||!interrupt(aura_handle))return false;context.motion.behavior_flags&=~4u;context.motion.movement_scale=1;}
    vm->visual.flags|=4;vm->variables.rotation.z=angle;
    if(context.motion.velocity.x<0)angle=float(angle-0.0026179938577115536f);else if(context.motion.velocity.x>0)angle=float(angle+0.0026179938577115536f);
    context.motion.movement_scale=.5f;position=context.motion.position;
    if(age.current!=age.previous&&age.current%3==0){const float distances[]={208,240,304},heights[]={32,128,256};const i32 damage[]={50,15,context.session.animal==1?40:15};for(u32 i=0;i<3;i++){const auto delta=polar(angle,distances[i]);const Vec3 center{float(position.x+delta.x),float(position.y+delta.y),float(position.z+delta.z)};auto* source=rectangle(center,angle,{512,heights[i]},0,damage[i]);if(!source)return false;if(i==2&&context.session.animal==1)source->callback_kind=3;}}
    if(auto* current=context.animations.registry.find(main_handle))current->visual.translation=position;if(auto* aura=context.animations.registry.find(aura_handle))aura->visual.translation=position;return cancel();
}
bool BombMarisa::cancel(){
    const i32 child_script=context.session.animal==1?35:28;
    for(i32 occurrence=0;;occurrence++){auto* root=context.animations.registry.find(main_handle);if(!root){main_handle=0;break;}auto* child=context.animations.registry.find_child_script(*root,child_script,occurrence);if(!child)break;const Vec2 size{float(child->visual.scale.x*48),float(child->visual.scale.y*160)};const auto p=parent_position(*child);if(!context.world.cancel_bullets_rectangle(p,size,angle,0)||!context.world.cancel_lasers_rectangle(p,size,angle,0,true))return fail("Marisa bomb rectangle cancellation failed");}return true;
}
i32 BombMarisa::hit(DamageSource&,const DamageQuery& query){const auto handle=animation(20,query.position);auto* vm=context.animations.registry.find(handle);if(!vm)return -1;const auto velocity=polar(angle,6.5f);vm->variables.vector[0]=velocity.x;vm->variables.vector[1]=velocity.y;return -1;}
}
