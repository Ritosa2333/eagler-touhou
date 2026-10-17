#include "EclVm.hpp"
#include "BulletMotion.hpp"
#include "GameMath.hpp"
namespace th08 {
bool EclExecutor::configure_laser(EclVm& vm,const EclInstruction& i){
    const auto integer=[&](u32 arg){return vm.operand_int(i,arg);};const auto real=[&](u32 arg){return vm.operand_float(i,arg);};
    if(i.opcode==114||i.opcode==115){
        if(i.size<64||u32(vm.laser_slot)>=32){vm.invalid=true;return false;}
        const auto raw=[&](u32 arg){i32 word;std::memcpy(&word,reinterpret_cast<const u8*>(&i)+12+arg*4,4);return word;};
        const auto scalar=[&](u32 arg,u32 bit){float value;const i32 word=raw(arg);std::memcpy(&value,&word,4);return i.variable_mask&(1u<<bit)?vm.resolve_float(value).to_float():value;};
        auto& e=vm.laser_emitter;e.position={Scalar::add(vm.resolved_position.x,vm.emission_offset.x),Scalar::add(vm.resolved_position.y,vm.emission_offset.y),Scalar::add(vm.resolved_position.z,vm.emission_offset.z)};
        const u32 packed=u32(raw(0));e.sprite=i16(packed);const i16 color=i16(packed>>16);e.color=i.variable_mask&2?i16(vm.read_int(color)):color;
        e.angle=scalar(1,2);e.speed=scalar(2,3);e.laser.start_offset=scalar(3,4);e.laser.end_offset=scalar(4,5);e.laser.length=scalar(5,6);e.laser.width=scalar(6,7);
        e.laser.start=i.variable_mask&256?vm.read_int(raw(7)):raw(7);e.laser.duration=i.variable_mask&512?vm.read_int(raw(8)):raw(8);e.laser.stop=i.variable_mask&1024?vm.read_int(raw(9)):raw(9);
        e.laser.hitbox_start=raw(10);e.laser.hitbox_stop=raw(11);e.flags=u32(raw(12));e.pattern=i.opcode==115?0:1;
        vm.laser_slots[vm.laser_slot]=globals.laser_actions?globals.laser_actions->laser(e):nullptr;return true;
    }
    if(i.opcode==116){vm.laser_slot=integer(0);return !vm.invalid;}
    if(i.opcode==154){for(auto& slot:vm.laser_slots)slot=nullptr;return true;}
    const i32 slot=integer(0);if(u32(slot)>=32){vm.invalid=true;return false;}auto* laser=vm.laser_slots[slot];
    if(i.opcode==120){vm.context().locals.counters[2]=laser&&laser->in_use?1:0;return true;}
    if(!laser)return true;
    switch(i.opcode){
    case 117:laser->angle=add_angle(laser->angle,real(1));break;
    case 118:{const float offset=real(1);laser->angle=(bullet_aim_extended(laser->position,globals.player)+number(offset)).to_float();break;}
    case 119:
        laser->position.x=(number(real(1))+number(vm.resolved_position.x)).to_float();laser->position.y=(number(real(2))+number(vm.resolved_position.y)).to_float();laser->position.z=(number(real(3))+number(vm.resolved_position.z)).to_float();break;
    case 121:if(laser->in_use&&laser->state<2){laser->state=2;laser->timer.set(0);laser->width=laser->width2;}break;
    case 167:laser->angle=real(1);break;
    case 170:laser->unknown599=i8(integer(1));break;
    case 171:laser->length=real(1);break;
    case 172:laser->start_offset=real(1);laser->end_offset=real(2);break;
    default:break;
    }
    return !vm.invalid;
}
}
