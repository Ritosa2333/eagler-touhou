#include "EclVm.hpp"
#include "AnmExecutor.hpp"
#include "EffectState.hpp"
#include "GameMath.hpp"
namespace th08 {
bool EclExecutor::load_animation(EclVm& vm,u32 slot,i32 file,i32 script){
    auto* resource=globals.enemy_animation_files[file];
    if(!resource){vm.failure=EclVm::Failure::MissingAnimation;vm.invalid=true;return false;}
    if(slot>=3||script<0||u32(script)>=resource->scriptCount){vm.invalid=true;return false;}
    auto& animation=vm.animation[slot];animation.anmFile=resource;animation.scriptIndex=i16(script);
    AnmExecutor executor(rng);executor.timing=timing;executor.start(*resource,animation,resource->scripts[script]);
    if(executor.invalid){vm.invalid=true;return false;}return true;
}
bool EclExecutor::animate(EclVm& vm,const EclInstruction& instruction){
    const i32 opcode=instruction.opcode;
    const auto value=[&](u32 i){return vm.operand_int(instruction,i);};
    const u32 needed=opcode==56||opcode==60?6:opcode==57||opcode==61?2:opcode==62?0:1;
    if(instruction.size<12+needed*4){vm.invalid=true;return false;}
    if(opcode==54||opcode==58){
        if(!load_animation(vm,0,opcode==58,value(0)))return false;
    }else if(opcode==55||opcode==59||opcode==56||opcode==60){
        i16 scripts[6];
        if(opcode==55||opcode==59){const u32 base=u32(value(0));for(u32 i=0;i<6;++i)scripts[i]=i16(base+i);}
        else for(i32 i=5;i>=0;--i)scripts[i]=i16(value(u32(i)));
        constexpr u32 destination[6]{0,3,4,1,2,5};for(u32 i=0;i<6;++i)vm.poses[destination[i]]=scripts[i];
        vm.pose_direction=-1;
    }else if(opcode==57||opcode==61){
        // 61 selects the stage resource before the helper; 57 clears the flag
        // after it, so a preceding stage instruction affects its resource.
        if(opcode==61)vm.flags2|=4;
        const i32 checked=value(0);if(checked<0||checked>=2){vm.invalid=true;return false;}
        if(value(1)>=0){
            const i32 script=value(1),slot=value(0);if(slot<0||slot>=2){vm.invalid=true;return false;}
            if(!load_animation(vm,u32(slot+1),bool(vm.flags2&4),script))return false;
        }else{
            const i32 slot=value(0);if(slot<0||slot>=2){vm.invalid=true;return false;}
            vm.animation[slot+1].scriptIndex=-1;
        }
    }else if(opcode==62)return load_animation(vm,0,bool(vm.flags2&4),vm.poses[5]);
    if(opcode>=58)vm.flags2|=4;else vm.flags2&=~4u;
    return true;
}
bool EclExecutor::update_pose(EclVm& vm){
    // Pose selection is the final part of original 00423150. Its preceding
    // periodic bullet emission belongs to the emitter component.
    if(vm.life<=0||vm.poses[3]<0)return true;
    i8 direction=0;const bool mirror=vm.flags&0x40000;
    if(vm.velocity.x<-.01f)direction=mirror?2:1;
    else if(vm.velocity.x>.01f)direction=mirror?1:2;
    if(direction==vm.pose_direction)return true;
    const i16 script=direction==1?vm.poses[3]:direction==2?vm.poses[4]:vm.pose_direction==-1?vm.poses[0]:vm.pose_direction==1?vm.poses[1]:vm.poses[2];
    if(!load_animation(vm,0,bool(vm.flags2&4),script))return false;
    vm.pose_direction=direction;return true;
}
bool EclExecutor::update_animations(EclVm& vm){
    AnmExecutor executor(rng);executor.timing=timing;
    vm.animation[0].color1.d3dColor=vm.animation_color;executor.execute(vm.animation[0]);vm.animation_color=vm.animation[0].color1.d3dColor;
    for(u32 slot=1;slot<3;++slot)if(vm.animation[slot].scriptIndex>=0&&executor.execute(vm.animation[slot]))vm.animation[slot].scriptIndex=-1;
    if(executor.invalid){vm.invalid=true;return false;}return true;
}
void EclVm::update_attached_effects()noexcept{
    if(effect_count<0||effect_count>24){invalid=true;return;}
    for(i32 index=0;index<effect_count;++index)if(auto* effect=effects[index]){
        effect->flag1=(flags&16)==0;effect->center=position;
        if(effect->radius<effect_radius)effect->radius=Scalar::add(effect->radius,.3f);else effect->radius=effect_radius;
        effect->angle=add_angle(effect->angle,0.03141592815518379f);
    }
}
}
