#include "EclLasers.hpp"
#include "EclVm.hpp"
#include "GameMath.hpp"
#include <cmath>
namespace th09 {
bool EclLasers::command(EclVm& vm,const EclInstruction& i,LaserEmissionActions& actions){
    const auto iv=[&](u32 n){return vm.integer(i,n);};
    if(i.opcode==114||i.opcode==115){
        if(i.size<64||selected<0||selected>=32)return false;auto& e=parameters;
        const Vec3& p=vm.values.resolved_position;const Vec3& d=vm.emitter.offset;e.position={p.x+d.x,p.y+d.y,p.z+d.z};
        const u32 packed=vm.raw(i,0);e.sprite=i16(packed);e.color=i16(packed>>16);if(i.variable_mask&2)e.color=i16(vm.values.read_int(e.color));
        const auto real=[&](u32 n){const float value=vm.raw_float(i,n);return (i.variable_mask&(1u<<(n+1)))?vm.values.read_float(value):value;};
        const auto integer=[&](u32 n){const i32 value=signed_bits(vm.raw(i,n));return (i.variable_mask&(1u<<(n+1)))?vm.values.read_int(value):value;};
        e.angle=real(1);e.speed=real(2);e.laser.start_offset=real(3);e.laser.end_offset=real(4);e.laser.length=real(5);e.laser.width=real(6);
        e.laser.start=integer(7);e.laser.duration=integer(8);e.laser.stop=integer(9);e.laser.hitbox_start=signed_bits(vm.raw(i,10));e.laser.hitbox_stop=signed_bits(vm.raw(i,11));
        e.flags=vm.raw(i,12);e.pattern=i.opcode!=115;references[selected]=actions.emit_laser(e);return references[selected]!=nullptr;
    }
    if(i.opcode==116){selected=iv(0);return !vm.invalid;}
    if(i.opcode==154){for(auto& reference:references)reference=nullptr;return true;}
    const i32 index=iv(0);if(index<0||index>=32)return false;auto* laser=references[index];
    if(i.opcode==120){vm.context().locals.counters[2]=laser&&laser->active;return !vm.invalid;}
    if(!laser)return true;
    switch(i.opcode){
    case 117:laser->direction=float(add_angle(laser->direction,vm.real(i,1)));break;
    case 118:{
        if(!vm.values.field)return false;const float offset=vm.real(i,1);
        const auto& p=vm.values.field->player;const float dx=p.x-laser->position.x,dy=p.y-laser->position.y;
        const double aim=dx==0&&dy==0?double(1.5707963705062866f):std::atan2(double(dy),double(dx));laser->direction=float(aim+double(offset));break;
    }
    case 119:
        laser->position.x=float(vm.wide(i,1)+double(vm.values.resolved_position.x));
        laser->position.y=float(vm.wide(i,2)+double(vm.values.resolved_position.y));
        laser->position.z=float(vm.wide(i,3)+double(vm.values.resolved_position.z));break;
    case 121:if(laser->active&&laser->phase<2){laser->phase=2;laser->time.reset();laser->width=laser->active_width;}break;
    case 167:laser->direction=vm.real(i,1);break;
    case 170:laser->reserved=u8(iv(1));break;
    case 171:laser->length=vm.real(i,1);break;
    case 172:laser->start_offset=vm.real(i,1);laser->end_offset=vm.real(i,2);break;
    default:return false;
    }
    return !vm.invalid;
}
}
