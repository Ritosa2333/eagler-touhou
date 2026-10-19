#include "EclSpawn.hpp"
namespace th08 {
bool execute_enemy_spawn(EclVm& vm,const EclInstruction& instruction,EclGlobals& globals,EnemySpawnActions& actions){
    const i32 op=instruction.opcode;const bool familiar=op<=92;
    if(op<90||op>94||instruction.size<(familiar?36:40)){vm.invalid=true;return false;}
    EclVm* tail=&vm;
    if(familiar&&!vm.parent){u32 count=0;while(tail->next_familiar){if(++count>480){vm.invalid=true;return false;}tail=tail->next_familiar;}}
    if(vm.life>0&&(!familiar||!(vm.flags&0x400))){
        TimelineSpawn request;u32 sub;std::memcpy(&sub,reinterpret_cast<const u8*>(&instruction)+12,4);request.subroutine=i16(sub);
        request.position.x=vm.operand_float(instruction,1);request.position.y=vm.operand_float(instruction,2);request.position.z=familiar?0:vm.operand_float(instruction,3);
        if(op==91||op==94){const auto& origin=op==91?vm.resolved_position:vm.position;auto& p=request.position;p={Scalar::add(p.x,origin.x),Scalar::add(p.y,origin.y),Scalar::add(p.z,origin.z)};}
        const u32 life=familiar?3:4;request.score=vm.operand_int(instruction,life+2);request.item=i8(vm.operand_int(instruction,life+1));request.life=vm.operand_int(instruction,life);
        const auto result=actions.spawn(request,vm.context().locals);
        if(familiar&&!result.failed){
            if(!result.enemy){vm.invalid=true;return false;}auto& child=*result.enemy;child.flags|=0x100;child.flags=(child.flags&~0x800u)|(u32(globals.youkai)<<11);child.draw_layer=globals.youkai?0:2;
            if(op==92){child.position_offset=vm.position;child.refresh_position();}
            child.flags&=~4u;
            if(!child.familiar_effect){
                child.familiar_effect=actions.overlay(32,op==92?child.resolved_position:child.position,1,0xffffffff);
                if(!child.familiar_effect){vm.invalid=true;vm.failure=EclVm::Failure::MissingEffect;return false;}
                child.familiar_effect->pendingInterrupt=globals.youkai?2:1;child.familiar_effect->flag17=bool(child.flags&4);
                if(child.pool_index&1)child.familiar_effect->angleVel.z=-child.familiar_effect->angleVel.z;
            }
            if(op==92)child.flags|=0x200;child.parent=&vm;tail->next_familiar=&child;child.previous_familiar=tail;vm.summoned_familiars=wrapping_add(vm.summoned_familiars,1);
        }
    }
    if(familiar)actions.panned_sound(36,vm.position.x);
    return !vm.invalid;
}
}
