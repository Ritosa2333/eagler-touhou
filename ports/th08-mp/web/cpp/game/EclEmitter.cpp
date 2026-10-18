#include "EclVm.hpp"
namespace th08 {
namespace {
i32 rank_integer(i32 from,i32 to,i32 rank){return wrapping_add(signed_bits(u32(wrapping_sub(to,from))*u32(rank))/32,from);}
Extended rank_float(float from,float to,i32 rank){return (number(to)-number(from))*Extended::from_int(rank)/number(32)+number(from);}
}
bool EclExecutor::shoot(EclVm& vm,const EclInstruction& ins){
    if(ins.size<44){vm.invalid=true;return false;}
    const auto raw=[&](u32 offset){u32 v;std::memcpy(&v,reinterpret_cast<const u8*>(&ins)+offset,4);return v;};
    const auto integer=[&](u32 offset,u32 bit){const i32 v=signed_bits(raw(offset));return ins.variable_mask&(1u<<bit)?vm.read_int(v):v;};
    const auto real=[&](u32 offset,u32 bit){float v;const u32 word=raw(offset);std::memcpy(&v,&word,4);return ins.variable_mask&(1u<<bit)?vm.resolve_float(v).to_float():v;};
    const u32 flags=raw(40);
    if(((flags&0x8000)&&!(vm.flags&0x800))||((flags&0x10000)&&(vm.flags&0x800)))return true;
    const auto dy=number(vm.resolved_position.y)-number(globals.player.y),dx=number(vm.resolved_position.x)-number(globals.player.x);
    const auto distance=dy*dy+dx*dx,protection=number(vm.player_protect_squared);
    if(!(vm.player_protect_squared<=0)&&!(protection<distance||protection==distance))return true;
    auto& e=vm.emitter;
    e.position={Scalar::add(vm.resolved_position.x,vm.emission_offset.x),Scalar::add(vm.resolved_position.y,vm.emission_offset.y),Scalar::add(vm.resolved_position.z,vm.emission_offset.z)};
    const u32 packed=raw(12);i16 sprite=i16(packed);if(ins.variable_mask&1)sprite=i16(vm.read_int(sprite));e.sprite=sprite;e.pattern=i16(ins.opcode-96);
    e.count=i16(integer(16,2));e.layers=i16(integer(20,3));
    e.angle=real(32,6);e.speed=real(24,4);e.spread=real(36,7);e.ending_speed=real(28,5);
    if(!(globals.spell_flags&1)){
        e.count=i16(u16(e.count)+u16(rank_integer(vm.rank_count_low,vm.rank_count_high,globals.current_rank())));if(e.count<1)e.count=1;
        e.layers=i16(u16(e.layers)+u16(rank_integer(vm.rank_layers_low,vm.rank_layers_high,globals.current_rank())));if(e.layers<1)e.layers=1;
        if(e.speed!=0){e.speed=(rank_float(vm.rank_speed_low,vm.rank_speed_high,globals.current_rank())+number(e.speed)).to_float();if(e.speed<.3f)e.speed=.3f;}
        e.ending_speed=(rank_float(vm.rank_speed_low,vm.rank_speed_high,globals.current_rank())/number(2)+number(e.ending_speed)).to_float();if(e.ending_speed<.3f)e.ending_speed=.3f;
    }
    e.unknown1fa=0;e.flags=flags;i16 color=i16(packed>>16);if(ins.variable_mask&2)color=i16(vm.read_int(color));e.color=color;
    if(globals.bullet_actions)globals.bullet_actions->emit(e);return true;
}
bool EclExecutor::configure_emitter(EclVm& vm,const EclInstruction& i){
    const auto n=[&](u32 arg){return vm.operand_int(i,arg);};const auto f=[&](u32 arg){return vm.operand_float(i,arg);};
    const i32 op=i.opcode;
    if(op>=96&&op<=104){
        if(vm.life<=0)return true;if(i.size<44){vm.invalid=true;return false;}
        if(vm.flags&0x20000){std::memcpy(vm.repeated_shot,&i,44);return true;}
        return shoot(vm,i);
    }
    if(op==105||op==106){
        vm.emission_period=n(0);
        if(vm.emission_period){const i32 low=vm.emission_period/5,high=wrapping_sub(0,vm.emission_period)/5;vm.emission_period=wrapping_add(vm.emission_period,rank_integer(low,high,globals.current_rank()));vm.emission_time.set(op==105||!vm.emission_period?0:signed_bits(rng.next32()%u32(vm.emission_period)));}
    }else if(op==107)vm.flags|=0x20000;
    else if(op==108)vm.flags&=~0x20000u;
    else if(op==109){vm.emitter.position={Scalar::add(vm.resolved_position.x,vm.emission_offset.x),Scalar::add(vm.resolved_position.y,vm.emission_offset.y),Scalar::add(vm.resolved_position.z,vm.emission_offset.z)};if(globals.bullet_actions)globals.bullet_actions->emit(vm.emitter);}
    else if(op==110){vm.emission_offset.x=f(0);vm.emission_offset.y=f(1);vm.emission_offset.z=0;}
    else if(op==111){const i32 slot=n(0);if(slot<0||slot>=18){vm.invalid=true;return false;}auto& ex=vm.emitter.extras[slot];ex.flags=u32(n(1));ex.mode=n(2);ex.integer_a=n(3);ex.integer_b=n(4);ex.float_a=f(5);ex.float_b=f(6);}
    else if(op==112){if(globals.bullet_actions)globals.bullet_actions->clear(1);}
    else if(op==113){if(n(0)<0)vm.emitter.flags&=~0x200u;else{vm.emitter.sound=n(0);vm.emitter.flags|=0x200;}vm.emitter.transform_sound=n(1);}
    return true;
}
bool EclExecutor::repeat_shot(EclVm& vm){
    if(vm.life>0&&vm.emission_period>0){vm.emission_time.tick(timing);if(vm.emission_time.current>=vm.emission_period){if(!shoot(vm,*reinterpret_cast<const EclInstruction*>(vm.repeated_shot)))return false;vm.emission_time.set(0);}}
    return true;
}
}
