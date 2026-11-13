#include "EnemyAnimations.hpp"
#include "EnemyCommands.hpp"
#include <cmath>
namespace th11 {
namespace {
template<class F>void family(AnmVm* vm,F operation){
    if(!vm)return;operation(*vm);
    if(!vm->child.previous)for(auto* node=vm->child.next;node;node=node->next)operation(*node->value);
}
}
AnmVm* EnemyAnimations::find(u32& id){auto* vm=manager.find(id);if(!vm)id=0;return vm;}
void EnemyAnimations::visible(u32 id,bool value){family(manager.find(id),[&](AnmVm& vm){vm.flags=value?vm.flags|2:vm.flags&~2u;});}
void EnemyAnimations::interrupt(u32 id,i16 value){family(manager.find(id),[&](AnmVm& vm){vm.interrupt(value);});}
bool EnemyAnimations::interrupt_now(u32 id,i32 value){
    bool success=true;family(manager.find(id),[&](AnmVm& vm){
        vm.interrupt(value);if(vm.update(manager)<0)success=false;
    });return success;
}
void enemy_th16_mark_animation(AnmVm& vm)noexcept{
    // 46f220 marks retirement, retaining registered handles until update.
    if(vm.flags2&0x4000000)return;
    vm.flags2=(vm.flags2&~0x40u)|0x20;
    // The portable child list is flattened; parent links retain its branches.
    for(auto* node=vm.child.next;node;node=node->next){
        auto* child=node->value;bool blocked=false,descendant=false;
        for(auto* parent=child->parent;parent;parent=parent->parent){
            if(parent==&vm){descendant=true;break;}
            if(parent->flags2&0x4000000){blocked=true;break;}
        }
        if(descendant&&!blocked&&!(child->flags2&0x4000000))child->flags2=(child->flags2&~0x40u)|0x20;
    }
}
void EnemyAnimations::erase(u32 id){
    if constexpr(kPortTitle==16){if(auto* vm=manager.find(id))enemy_th16_mark_animation(*vm);return;}
    family(manager.find(id),[](AnmVm& vm){if constexpr(kPortTitle==14)vm.flags2|=0x20;else vm.flags|=0x4000000;});
}
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
u32 EnemyAnimations::th14_death_effect(const EnemyState& e){
#if PORT_TITLE == 14
    const u32 file=u32(e.death_animation_file);if(file>=resources.size()||!resources[file].file)return 0;
    const float dx=e.current.position.x-e.death_reference.x,dy=e.current.position.y-e.death_reference.y;
    const float xx=dx*dx,yy=dy*dy,distance=xx+yy;
    const float angle=distance<0.04000000283122063f?-1.5707963705062866f:float(std::atan2(double(dy),double(dx)));
    auto* vm=manager.create(*resources[file].file,e.death_animation,resources[file].id,3,false,false,&e.current.position,nullptr,nullptr,true,&angle);
    return vm?vm->id:0;
#else
    (void)e;return 0;
#endif
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
#if PORT_TITLE == 16
    {
    i32 direction=0,script=0;
    if(enemy_th16_direction_request(e,direction,script)){
        const u32 file=u32(e.bound_animation_file);
        if(file>=resources.size()||!resources[file].file)return false;
        Vec3 position{};
        if(auto* old=find(e.animations[0])){position=old->script_position;enemy_th16_mark_animation(*old);}
        e.animations[0]=0;const float rotation=0;const i32 layer=wrapping_add(e.facing,7);
        auto* vm=manager.create(*resources[file].file,script,resources[file].id,u32(layer),false,false,&position,nullptr,nullptr,false,&rotation,0x400);
        if(!vm)return false;
        vm->reserved_418=8; // Native +51c, child-creation mode after first update.
        e.animations[0]=vm->id;e.direction=direction;
    }
    if(auto* vm=find(e.animations[0])){size(e,*vm);e.visual_size.x=std::abs(e.visual_size.x);e.visual_size.y=std::abs(e.visual_size.y);}
    return true;
    }
#endif
#if PORT_TITLE == 14
    if(e.flags&0x100000){
        const i32 direction=e.current.velocity.x<-.03f?-1:e.current.velocity.x>.03f?1:0;
        if(direction!=e.direction){
            const i32 variant=e.direction==-1?(direction?2:3):e.direction==0?(direction==-1?1:2):e.direction==1?(direction?1:4):0;
            auto* old=find(e.animations[0]);if(!old||!old->resource)return false;
            auto* resource=old->resource;const u16 file=old->file_index;const Vec3 p=old->script_position;
            erase(e.animations[0]);e.animations[0]=0;
            const i32 layer=wrapping_add(e.facing,7);
            auto* vm=manager.create(*resource,wrapping_add(e.base_animation,variant),file,layer<0?0:u32(layer),false,false,&p,nullptr,nullptr,layer>=0&&layer<=23);
            if(!vm)return false;e.animations[0]=vm->id;e.direction=direction;
        }
    }
    if(auto* vm=find(e.animations[0])){size(e,*vm);e.visual_size.x=std::abs(e.visual_size.x);e.visual_size.y=std::abs(e.visual_size.y);}
    return true;
#endif
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
bool EnemyAnimations::update_positions(EnemyState& e){
#if PORT_TITLE == 14
    // Native 42488a synchronizes fourteen slots; 14/15 are managed separately.
    for(u32 slot=0;slot<14;++slot){
        auto& id=e.animation_handle(slot);
        auto* vm=manager.find(id);if(!vm){if(!(e.flags&0x4000000))id=0;continue;}
        Vec3 p=e.current.position;
        if(!(e.flags&0x4000000)){
            const auto& offset=e.animation_offsets[slot];p={float(p.x+offset.x),float(p.y+offset.y),float(p.z+offset.z)};
            const i32 parent=e.animation_parents[slot];
            if(parent>=0){if(parent>=16)return false;auto* source=find(e.animation_handle(u32(parent)));if(!source)return false;
                p={float(p.x+source->script_position.x),float(p.y+source->script_position.y),float(p.z+source->script_position.z)};}
            if(vm->flags2&0x40){vm->rotation.z=float(std::atan2(double(e.current.velocity.y),double(e.current.velocity.x)));vm->flags|=4;e.animation_angle=vm->rotation.z;}
        }
        vm->position=p;
    }
    return true;
#endif
    for(u32 slot=0;slot<8;++slot){
        if(e.flags&0x400000){position(e.animations[slot],e.current.position,false);continue;}
        if(auto* vm=find(e.animations[slot])){
            position(vm->id,e.current.position,true);
            if(vm->flags&0x8000000){vm->rotation.z=e.current.angle;vm->flags|=4;}
        }
    }
    return true;
}
void EnemyAnimations::size(EnemyState& e,const AnmVm& vm){e.visual_size={float(double(vm.sprite_size.y)*vm.scale.y),float(double(vm.sprite_size.x)*vm.scale.x)};}
bool EnemyAnimations::th14_stage_logo(i32 mode,u32 flags){
    if(mode==8||(flags&0x40))return true;
    auto& r=stage_logo_resource;if(!r.file)return false;const Vec3 origin{};
    return manager.create(*r.file,0,6,0,false,false,&origin)!=nullptr;
}
bool EnemyAnimations::th16_stage_logo(i32 mode,u32 flags){
    // Retail42c240 uses the player's front resource, script0, default layer.
    if(mode==8||(flags&0x40))return true;
    auto& r=stage_logo_resource;if(!r.file)return false;
    return manager.create(*r.file,0,r.id,~0u)!=nullptr;
}
AnmVm* EnemyAnimations::create(EnemyState& e,u32 slot,i32 script,u32 layer){
    if(slot>=10||u32(e.animation_file)>=resources.size())return nullptr;
    auto& r=resources[e.animation_file];if(!r.file)return nullptr;
    auto* vm=manager.create(*r.file,i16(script),r.id,layer,false,true);if(!vm)return nullptr;e.animations[slot]=vm->id;return vm;
}
i32 EnemyAnimations::command(EnemyState& e,EclContext& c,EnemyGlobals& g){
    auto i=[&](u32 n){return c.integer_argument(n,g);};const u16 op=c.instruction->opcode;
    if constexpr(kPortTitle==16){
#if PORT_TITLE == 16
        if(op==316){
            // Own41e6ab: retire the old handle, create the base variant,
            // then query size and hide the new family when requested.
            const u32 slot=u32(i(0));const i32 variant=i(1);if(slot>=16)return -2;
            auto& id=e.animation_handle(slot);erase(id);id=0;
            const i32 script=variant<0?e.base_animation:wrapping_add(e.base_animation,wrapping_add(variant,5));
            const u32 file=u32(e.animation_file);if(file>=resources.size()||!resources[file].file)return -2;
            const auto& resource=resources[file];const i32 layer=wrapping_add(e.facing,7);
            const float rotation=0;
            auto* vm=manager.create(*resource.file,script,resource.id,u32(layer),false,true,nullptr,nullptr,nullptr,layer>=0&&layer<=23,&rotation,0x400);
            if(!vm)return -2;vm->reserved_418=2;id=vm->id;
            if(slot==0)size(e,*vm);
            if(e.flags&32)visible(id,false);
            return 0;
        }
        if(op==307||op==338){
            // Own41e118 / 41dfc7: position and rotation before the first
            // VM update, prepend, then register in the enemy effect ledger.
            Vec3 position=e.current.position;float rotation=0;
            if(op==338){
                const float dy=float(c.float_argument(3,g)),dx=float(c.float_argument(2,g));
                position={float(position.x+dx),float(position.y+dy),float(position.z+0.f)};
                rotation=float(c.float_argument(4,g));
            }
            const i32 script=i(1),file=i(0);if(u32(file)>=resources.size()||!resources[file].file)return -2;
            const auto& resource=resources[file];
            auto* vm=manager.create(*resource.file,script,resource.id,0xffffffff,false,true,&position,nullptr,nullptr,false,&rotation,0x400);
            if(!vm)return -2;vm->reserved_418=2;
            return th16_register_effect&&th16_register_effect(th16_registration_user,vm->id)?0:-2;
        }
        if(op==339||op==308){
            struct Services:Th16LooseAnimationServices{
                EnemyAnimations& owner;AnmManager& manager;AnmVm* vm=nullptr;u32 id=0;
                Services(EnemyAnimations& o,AnmManager& m):owner(o),manager(m){}
                bool construct(i32 file,i32 script)override{if(u32(file)>=owner.resources.size()||!owner.resources[file].file)return false;const auto& resource=owner.resources[file];vm=manager.create(*resource.file,script,resource.id,0xffffffff,false,false,nullptr,nullptr,nullptr,false,nullptr,0x400);if(!vm)return false;vm->reserved_418=2;id=vm->id;return true;}
                bool register_effect()override{return owner.th16_register_effect&&owner.th16_register_effect(owner.th16_registration_user,id);}
                bool lookup_effect()override{vm=manager.find(id);return vm!=nullptr;}
                bool tick()override{return vm->update(manager)!=-2;}
            } services(*this,manager);
            return enemy_th16_loose_animation_command(c,g,services);
        }
#endif
        if(op==319||op==325||op==327||op==329||op==331||op==335||op==337){
            // Own4233a0: unsigned slot guard, stale handle clearing, then
            // simple setters. Other branches require their own consumers.
            const u32 slot=u32(i(0));if(slot>15)return 0;
            if(auto* vm=find(e.animation_handle(slot))){
                if(op==319){vm->rotation.z=float(c.float_argument(1,g));vm->flags|=4;}
                else if(op==325){const u32 blue=u8(i(3)),green=u8(i(2)),red=u8(i(1));vm->color=(vm->color&0xff000000u)|(red<<16)|(green<<8)|blue;}
                else if(op==327||op==331){auto& color=op==327?vm->color:vm->secondary_color;color=(color&0xffffffu)|(u32(u8(i(1)))<<24);}
                else if(op==337){const u32 blend=u32(u8(i(1)))&15;vm->flags=(vm->flags&~0x70u)|((blend&7)<<4);vm->th13_native_flags=(vm->th13_native_flags&~0x100u)|((blend&8)<<5);}
                else{const float y=float(c.float_argument(2,g)),x=float(c.float_argument(1,g));(op==329?vm->scale:vm->scale_velocity)={x,y};vm->flags|=8;}
            }
            return 0;
        }
    }
    if constexpr(kPortTitle==14){
#if PORT_TITLE == 14
        if(op==320){const u32 slot=u32(i(0));if(slot>=16)return -2;
            auto& p=e.animation_offsets[slot];p.x=float(c.float_argument(1,g));p.y=float(c.float_argument(2,g));p.z=0;return 0;}
#endif
        if(op==319||op==325||op==326||op==327||op==328||op==329||op==330||op==331||op==332||op==335||op==336){
            // 425420 -> 42a520: invalid slots are ignored, stale handles cleared.
            const u32 slot=u32(i(0));if(slot>15)return 0;
            if(auto* vm=find(e.animation_handle(slot))){
                if(op==319){vm->rotation.z=float(c.float_argument(1,g));vm->flags|=4;}
                else if(op==325){const u32 blue=u8(i(3)),green=u8(i(2)),red=u8(i(1));vm->color=(vm->color&0xff000000u)|(red<<16)|(green<<8)|blue;}
                else if(op==326){
                    const i32 red=u8(i(3)),green=u8(i(4)),blue=u8(i(5)),mode=i(2),duration=i(1);auto& v=vm->color_interpolation;
                    v.duration=duration;v.mode=InterpolationMode(mode);v.start[0]=i32(vm->color&255);v.start[1]=i32((vm->color>>8)&255);v.start[2]=i32((vm->color>>16)&255);
                    v.end[0]=blue;v.end[1]=green;v.end[2]=red;for(u32 n=0;n<3;++n)v.initial_tangent[n]=v.final_tangent[n]=0;v.timer.set(0,&manager.rate);
                }
                else if(op==327||op==331){auto& color=op==327?vm->color:vm->secondary_color;color=(color&0x00ffffffu)|(u32(u8(i(1)))<<24);}
                else if(op==336)vm->layer=u32(i(1));
                else if(op==328||op==332){
                    const i32 target=u8(i(3)),mode=i(2),duration=i(1);auto& v=op==328?vm->alpha_interpolation:vm->alpha2_interpolation;
                    v.duration=duration;v.mode=InterpolationMode(mode);v.start[0]=i32((op==328?vm->color:vm->secondary_color)>>24);v.end[0]=target;
                    if(op==328)v.initial_tangent[0]=v.final_tangent[0]=0;
                    else vm->th13_native_flags=(vm->th13_native_flags&~0x40000u)|0x20000u;
                    v.timer.set(0,&manager.rate);
                }
                else if(op==330){
                    const float y=float(c.float_argument(4,g)),x=float(c.float_argument(3,g));const i32 mode=i(2),duration=i(1);
                    auto& v=vm->scale_interpolation;v.duration=duration;v.mode=InterpolationMode(mode);
                    v.start[0]=vm->scale.x;v.start[1]=vm->scale.y;v.end[0]=x;v.end[1]=y;v.timer.set(0,&manager.rate);
                }
                else{const float y=float(c.float_argument(2,g)),x=float(c.float_argument(1,g));
                    (op==329?vm->scale:vm->scale_velocity)={x,y};vm->flags|=8;}
            }
            return 0;
        }
        if(op==302){e.animation_file=i(0);return 0;}
        if(op==337){
            // Retail 425581: offsets and rotation are installed before the
            // first ANM update, which may overwrite rotation itself.
            const float dy=float(c.float_argument(3,g)),dx=float(c.float_argument(2,g));
            const Vec3 p{float(e.current.position.x+dx),float(e.current.position.y+dy),float(e.current.position.z+0.f)};
            const float angle=float(c.float_argument(4,g));const i32 script=i(1),file=i(0);
            if(u32(file)>=resources.size()||!resources[file].file)return -2;
            return manager.create(*resources[file].file,script,resources[file].id,0,false,true,&p,nullptr,nullptr,false,&angle)?0:-2;
        }
        if(op==318){
            // 4259f6 ignores the encoded arguments and restores slot zero.
            auto& id=e.animations[0];erase(id);id=0;
            if(u32(e.animation_file)>=resources.size()||!resources[e.animation_file].file)return -2;
            auto& resource=resources[e.animation_file];const i32 layer=wrapping_add(e.facing,7);
            auto* vm=manager.create(*resource.file,e.base_animation,resource.id,layer<0?0:u32(layer),false,true,nullptr,nullptr,nullptr,layer>=0&&layer<=23);
            if(!vm)return -2;id=vm->id;e.flags&=~0x100000u;e.animation_script=e.base_animation;e.bound_animation_file=e.animation_file;e.direction=0;return 0;
        }
        if(op==315){
            // 4257cf reads script, file, then angle after the first ANM tick.
            const i32 script=i(1),file=i(0);if(u32(file)>=resources.size()||!resources[file].file)return -2;
            auto* vm=manager.create(*resources[file].file,script,resources[file].id,0,false,true,&e.current.position);
            if(!vm)return -2;vm->position=e.current.position;vm->rotation.z=float(c.float_argument(2,g));vm->flags|=4;return 0;
        }
        if(op==307||op==308){
            // 307 reads script then file; 308 reads file then script. Both
            // retain the bound layer,
            // zero the origin, execute the first ANM update, prepend normally.
            const i32 script=op==307?i(1):0,file=i(0),bound_script=op==307?script:i(1);
            if(u32(file)>=resources.size()||!resources[file].file)return -2;
#if PORT_TITLE == 14
            NativeStackTh14::CounterScope factory_scope(op==308?&manager.th14_native_stack.ecl_animation_factory_depth:nullptr);
#endif
            return manager.create(*resources[file].file,bound_script,resources[file].id,u32(-1),false,true,op==307?&e.current.position:nullptr)?0:-2;
        }
        if(op==303||op==306||op==313||op==316){
            const u32 slot=u32(i(0));i32 script=op==313?wrapping_add(e.base_animation,5):i(1);if(slot>=16)return -2;
            auto& id=e.animation_handle(slot);erase(id);id=0;
            if(op==303){if(script<0)return 0;script=i(1);}
            if(op==316)script=script<0?e.base_animation:wrapping_add(e.base_animation,wrapping_add(script,5));
            if(u32(e.animation_file)>=resources.size()||!resources[e.animation_file].file)return -2;
            auto& resource=resources[e.animation_file];const i32 layer=wrapping_add(e.facing,7);
            auto* vm=manager.create(*resource.file,script,resource.id,layer<0?0:u32(layer),false,true,nullptr,nullptr,nullptr,layer>=0&&layer<=23);
            if(!vm)return -2;id=vm->id;
            if(op==303&&slot==0){e.animation_script=i(1);e.bound_animation_file=e.animation_file;}
            if(slot==0)size(e,*vm);
            if(e.flags&32)visible(id,false);
            if(op==306&&slot==0){e.flags|=0x100000;e.base_animation=script;e.animation_script=script;e.bound_animation_file=e.animation_file;e.direction=0;}
            return 0;
        }
        return -2;
    }
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
    // The layer constant is 8 for every one of these, not 7: each handler sets it immediately before
    // the create call (0x4162c0, 0x4163f7, 0x41646f and 0x416374, the last as lea eax,[esi+8] with
    // esi zeroed at 0x416356). 0x103 is the exception and adds 7 to the facing at 0x415b7c, where the
    // port had been adding 6. Each handler also reads the resource the same way, as
    // [manager + 0x40 + 4*animation_file] with the manager at DAT_004b43dc.
    auto* vm=create(e,slot,script,op==0x103?u32(wrapping_add(e.facing,7)):8);if(!vm)return -2;
    if(op==0x103&&slot==0){e.animation_script=i(1);e.bound_animation_file=e.animation_file;}
    if(op!=0x114){if(slot==0)size(e,*vm);if(e.flags&0x20)visible(vm->id,false);}
    if(op==0x106&&slot==0){e.flags|=0x10000;e.base_animation=script;e.direction=0;e.animation_script=script;e.bound_animation_file=e.animation_file;}
    if(op==0x114){e.flags&=~0x10000u;e.direction=0;e.animation_script=e.base_animation;e.bound_animation_file=e.animation_file;}
    return 0;
}
}
