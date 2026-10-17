#include "EnemyAnimations.hpp"
namespace th11 {
namespace {
template<class F>void family(AnmVm* vm,F operation){
    if(!vm)return;operation(*vm);
    if(!vm->child.previous)for(auto* node=vm->child.next;node;node=node->next)operation(*node->value);
}
}
AnmVm* EnemyAnimations::find(u32& id){auto* vm=manager.find(id);if(!vm)id=0;return vm;}
void EnemyAnimations::visible(u32 id,bool value){family(manager.find(id),[&](AnmVm& vm){vm.flags=value?vm.flags|2:vm.flags&~2u;});}
void EnemyAnimations::interrupt(u32 id,i16 value){family(manager.find(id),[&](AnmVm& vm){vm.pending_interrupt=value;});}
void EnemyAnimations::erase(u32 id){family(manager.find(id),[](AnmVm& vm){vm.flags|=0x4000000;});}
void EnemyAnimations::position(u32 id,Vec3 p,bool playfield){if(playfield){p.x=float((double(p.x)+32)+192);p.y=float(double(p.y)+16);}family(manager.find(id),[&](AnmVm& vm){vm.position=p;});}
i32 EnemyAnimations::sprite(u32 id){const auto* vm=manager.find(id);return vm?vm->sprite_index:0;}
u32 EnemyAnimations::effect(i32 file,i32 script,u32 layer){
    if(u32(file)>=resources.size()||!resources[file].file)return 0;
    auto* vm=manager.create(*resources[file].file,script,resources[file].id,layer,false,false);return vm?vm->id:0;
}
u32 EnemyAnimations::effect_at(i32 file,i32 script,u32 layer,Vec3 p,bool playfield){
    if(u32(file)>=resources.size()||!resources[file].file)return 0;
    if(playfield){p.x=float((double(p.x)+32)+192);p.y=float(double(p.y)+16);}
    auto* vm=manager.create(*resources[file].file,script,resources[file].id,layer,false,false,&p);return vm?vm->id:0;
}
// 0x456770 / 0x44ae60 restarts a script in-place. Unlike bind_script,
// this preserves the live manager links, transform and high flag bits.
bool EnemyAnimations::rebind(u32& id,i32 script){
    auto* vm=find(id);if(!vm)return true;
    if(!vm->resource||script<0||u32(script)>=vm->resource->scripts.size())return false;
    if(vm->flags&0x200)vm->scale.x=-vm->scale.x;
    vm->flags=(vm->flags&0xffff0000u)|7;vm->color=0xffffffff;
    vm->position_interpolation.duration=vm->color_interpolation.duration=vm->alpha_interpolation.duration=0;
    vm->rotation_interpolation.duration=vm->scale_interpolation.duration=0;
    vm->color2_interpolation.duration=vm->alpha2_interpolation.duration=0;
    vm->script_index=i16(script);vm->script_begin=vm->instruction=reinterpret_cast<AnmInstruction*>(vm->resource->scripts[script].bytes.data());
    vm->timer.set(0,&manager.rate);vm->flags&=~1u;return vm->update(manager)>=0;
}
bool EnemyAnimations::update_direction(EnemyState& e){
    if(!(e.flags&0x10000))return true;
    const i32 direction=e.current.velocity.x<-.1f?-1:e.current.velocity.x>.1f?1:0;
    if(direction==e.direction)return true;
    i32 variant=0;
    if(e.direction==-1)variant=direction?2:3;
    else if(e.direction==0)variant=direction==-1?1:2;
    else if(e.direction==1)variant=direction?1:4;
    auto* old=find(e.animations[0]);if(!old||!old->resource)return false;
    auto* resource=old->resource;const auto file=old->file_index;e.direction=direction;
    erase(e.animations[0]);e.animations[0]=0;
    auto* vm=manager.create(*resource,wrapping_add(e.base_animation,variant),file,7,false,false);
    if(!vm)return false;e.animations[0]=vm->id;return true;
}
void EnemyAnimations::update_positions(EnemyState& e){
    for(u32 slot=0;slot<8;++slot){
        if(e.flags&0x400000){position(e.animations[slot],e.current.position,false);continue;}
        if(auto* vm=find(e.animations[slot])){
            position(vm->id,e.current.position,true);
            if(vm->flags&0x8000000){vm->rotation.z=e.current.angle;vm->flags|=4;}
        }
    }
}
void EnemyAnimations::size(EnemyState& e,const AnmVm& vm){e.visual_size={float(double(vm.sprite_size.y)*vm.scale.y),float(double(vm.sprite_size.x)*vm.scale.x)};}
AnmVm* EnemyAnimations::create(EnemyState& e,u32 slot,i32 script,u32 layer){
    if(slot>=10||u32(e.animation_file)>=resources.size())return nullptr;
    auto& r=resources[e.animation_file];if(!r.file)return nullptr;
    auto* vm=manager.create(*r.file,i16(script),r.id,layer,false,true);if(!vm)return nullptr;e.animations[slot]=vm->id;return vm;
}
i32 EnemyAnimations::command(EnemyState& e,EclContext& c,EnemyGlobals& g){
    auto i=[&](u32 n){return c.integer_argument(n,g);};const u16 op=c.instruction->opcode;
    if(op==0x102){e.animation_file=i(0);return 0;}
    if(op==0x113){const u32 slot=u32(i(0));const i32 label=i(1);if(slot>=10)return -2;interrupt(e.animations[slot],i16(label));return 0;}
    if(op==0x115){const u32 slot=u32(i(0));if(slot>=10)return -2;if(auto* vm=find(e.animations[slot])){vm->rotation.z=float(c.float_argument(1,g));vm->flags|=4;}return 0;}
    if(op==0x108){const i32 file=i(0),script=i(1);if(u32(file)>=resources.size()||!resources[file].file)return -2;return manager.create(*resources[file].file,i16(script),resources[file].id,8,false,true)?0:-2;}
    if(op==0x107||op==0x110||op==0x111){
        // Original reads the script argument before the resource argument.
        const i32 script=i(1),file=i(0);if(u32(file)>=resources.size()||!resources[file].file)return -2;
        Vec3 p=e.current.position;
        if(!(e.flags&0x400000)){p.x=float((double(p.x)+32)+192);p.y=float(double(p.y)+16);}
        auto* vm=manager.create(*resources[file].file,script,resources[file].id,8,false,op!=0x110,&p);
        if(!vm)return -2;
        if(op==0x111){vm->position=p;vm->rotation.z=float(c.float_argument(2,g));vm->flags|=4;}
        return 0;
    }
    if(op!=0x103&&op!=0x106&&op!=0x10d&&op!=0x112&&op!=0x114)return -2;
    const u32 slot=op==0x114?0:u32(i(0));if(slot>=10)return -2;
    i32 script=0;if(op==0x103||op==0x106)script=i(1);
    erase(e.animations[slot]);e.animations[slot]=0;
    if(op==0x103){if(script<0)return 0;script=i(1);}
    if(op==0x10d)script=i16(e.base_animation)+5;
    if(op==0x112)script=i16(e.base_animation)+5+i16(i(1));
    if(op==0x114)script=i16(e.base_animation);
    auto* vm=create(e,slot,script,op==0x103?u32(wrapping_add(e.facing,6)):7);if(!vm)return -2;
    if(op==0x103&&slot==0){e.animation_script=i(1);e.bound_animation_file=e.animation_file;}
    if(op!=0x114){if(slot==0)size(e,*vm);if(e.flags&0x20)visible(vm->id,false);}
    if(op==0x106&&slot==0){e.flags|=0x10000;e.base_animation=script;e.direction=0;e.animation_script=script;e.bound_animation_file=e.animation_file;}
    if(op==0x114){e.flags&=~0x10000u;e.direction=0;e.animation_script=e.base_animation;e.bound_animation_file=e.animation_file;}
    return 0;
}
}
