#include "EclEmitter.hpp"
#include "EclVm.hpp"
namespace th09 {
bool EclEmitter::shoot(EclVm& vm,const EclInstruction& i,BulletEmissionActions& actions){
    if(i.size<44)return false;
    const auto word=[&](u32 at){u32 v;std::memcpy(&v,reinterpret_cast<const u8*>(&i)+at,4);return v;};
    const auto integer=[&](u32 at,u32 bit){const i32 value=signed_bits(word(at));return i.variable_mask&(1u<<bit)?vm.values.read_int(value):value;};
    const auto real=[&](u32 at,u32 bit){const u32 b=word(at);float value;std::memcpy(&value,&b,4);return i.variable_mask&(1u<<bit)?vm.values.read_float(value):value;};
    auto& e=parameters;const auto& p=vm.values.resolved_position;e.position={p.x+offset.x,p.y+offset.y,p.z+offset.z};
    const u32 packed=word(12);i16 sprite=i16(packed);if(i.variable_mask&1)sprite=i16(vm.values.read_int(sprite));e.sprite=sprite;e.pattern=i16(i.opcode-96);
    e.count=i16(integer(16,2));e.layers=i16(integer(20,3));e.angle=real(32,6);e.speed=real(24,4);e.spread=real(36,7);e.ending_speed=real(28,5);
    e.reserved1fa=0;e.flags=word(40);i16 color=i16(packed>>16);if(i.variable_mask&2)color=i16(vm.values.read_int(color));e.color=color;
    return actions.emit(e);
}
bool EclEmitter::command(EclVm& vm,const EclInstruction& i,BulletEmissionActions& actions){
    const auto iv=[&](u32 n){return vm.integer(i,n);};const auto fv=[&](u32 n){return vm.real(i,n);};
    if(i.opcode>=96&&i.opcode<=104){
        if(vm.values.life<=0)return true;if(i.size<44)return false;
        if(vm.behavior_flags&0x4000){std::memcpy(repeated,&i,44);return true;}return shoot(vm,i,actions);
    }
    switch(i.opcode){
    case 105:case 106:period=iv(0);if(period){if(i.opcode==106){if(!vm.values.world)return false;time.reset(signed_bits(vm.values.world->random.bounded32(u32(period))));}else time.reset();}break;
    case 107:vm.behavior_flags|=0x4000;break;
    case 108:vm.behavior_flags&=~0x4000u;break;
    case 109:{const auto& p=vm.values.position;parameters.position={p.x+offset.x,p.y+offset.y,p.z+offset.z};return actions.emit(parameters);}
    case 110:offset.x=fv(0);offset.y=fv(1);offset.z=0;break;
    case 111:{const i32 slot=iv(0);if(slot<0||slot>=18)return false;auto& ex=parameters.extras[slot];ex.flags=u32(iv(1));ex.mode=iv(2);ex.integer_a=iv(3);ex.integer_b=iv(4);ex.float_a=fv(5);ex.float_b=fv(6);break;}
    // The original 1.50a helper called by this opcode is an empty RET 4.
    case 112:break;
    case 113:if(iv(0)<0)parameters.flags&=~0x200u;else {parameters.sound=iv(0);parameters.flags|=0x200;}parameters.transform_sound=iv(1);break;
    default:return false;
    }
    return !vm.invalid;
}
bool EclEmitter::update(EclVm& vm,const FrameTiming& timing,BulletEmissionActions& actions){
    if(vm.values.life>0&&period>0){time.tick(timing);if(time.current>=period){if(!shoot(vm,*reinterpret_cast<const EclInstruction*>(repeated),actions))return false;time.reset();}}
    return true;
}
}
