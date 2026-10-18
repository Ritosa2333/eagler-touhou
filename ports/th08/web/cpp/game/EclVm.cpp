#include "EclVm.hpp"
#include "GameMath.hpp"
#include "EclSpawn.hpp"
#include "EclControl.hpp"
#include "EclScene.hpp"
#include "EclNative.hpp"
#include <cmath>
#include <algorithm>

namespace th08 {
namespace {
u32 raw_arg(const EclInstruction& instruction,u32 index) noexcept {
    u32 value=0;
    if(instruction.size>=16&&index<u32(instruction.size-12)/4) std::memcpy(&value,reinterpret_cast<const u8*>(&instruction)+12+index*4,sizeof(value));
    return value;
}
float raw_float(const EclInstruction& instruction,u32 index) noexcept {
    const u32 value=raw_arg(instruction,index);float result=0;std::memcpy(&result,&value,sizeof(result));return result;
}
bool is_variable(const EclInstruction& instruction,u32 index) noexcept {return index<16&&(instruction.variable_mask&(u16(1)<<index))!=0;}
i32 bits_to_int(u32 value) noexcept {return signed_bits(value);}
float safe_sin(float value) noexcept {return sine(value).to_float();}
float safe_cos(float value) noexcept {return cosine(value).to_float();}
i32 divide(i32 a,i32 b) noexcept {return b==0?0:signed_bits(u32(i64(a)/b));}
i32 modulo(i32 a,i32 b) noexcept {return b==0?0:i32(i64(a)%b);}
}
i32 EclVm::variable_int(const EclInstruction& instruction,u32 index) const noexcept {return bits_to_int(raw_arg(instruction,index));}
i32 EclVm::variable_float(const EclInstruction& instruction,u32 index) const noexcept {return number(raw_float(instruction,index)).truncate_int();}
i32* EclVm::integer_target(const EclInstruction& instruction,u32 index)noexcept {
    if(instruction.size<16||index>=u32(instruction.size-12)/4){invalid=true;return nullptr;}
    const i32 id=variable_int(instruction,index);
    if(is_variable(instruction,index))if(auto* field=integer_field(id))return field;
    return reinterpret_cast<i32*>(const_cast<u8*>(reinterpret_cast<const u8*>(&instruction))+12+index*4);
}
float* EclVm::float_target(const EclInstruction& instruction,u32 index)noexcept {
    if(instruction.size<16||index>=u32(instruction.size-12)/4){invalid=true;return nullptr;}
    const i32 id=variable_float(instruction,index);
    if(is_variable(instruction,index))if(auto* field=float_field(id))return field;
    return reinterpret_cast<float*>(const_cast<u8*>(reinterpret_cast<const u8*>(&instruction))+12+index*4);
}
i32 EclVm::operand_int(const EclInstruction& instruction,u32 index) const noexcept {
    const i32 value=bits_to_int(raw_arg(instruction,index));return is_variable(instruction,index)?read_int(value):value;
}
float EclVm::operand_float(const EclInstruction& instruction,u32 index) const noexcept {
    return is_variable(instruction,index)?resolve_float(raw_float(instruction,index)).to_float():raw_float(instruction,index);
}

bool EclExecutor::start(EclVm& vm,EclProgram& program,i32 subroutine) {
    vm=EclVm{};vm.random=&rng;vm.environment=&globals;vm.program=&program;vm.context().subroutine=subroutine;vm.context().instruction=program.sub(subroutine);
    std::memset(vm.animation,0,sizeof(vm.animation));
    if(!vm.context().instruction){vm.invalid=true;return false;}vm.context().timer.set(0);return true;
}

void EclExecutor::record(EclVm& vm,const EclInstruction& instruction) noexcept {
    u32 args[6]{};const u32 count=instruction.size<12?0:(instruction.size-12)/4;
    for(u32 i=0;i<count&&i<6;++i)args[i]=raw_arg(instruction,i);
    switch(instruction.opcode){
    case 95:for(auto& enemy:globals.enemies)enemy.active=0;break;
    case 162:globals.clear_bullets();break;
    case 128:globals.spell_flags|=1u;break;
    case 148:globals.boss_lives=vm.operand_int(instruction,0);break;
    case 147:globals.stage_interrupt=vm.operand_int(instruction,0);break;
    case 152:globals.bullet_rank=u32(vm.operand_int(instruction,0));break;
    case 176:globals.player_nullified=1;break;
    default:break;
    }
    if(globals.combat){
        switch(instruction.opcode){
        case 95:globals.combat->clear_enemies();break;
        case 162:globals.combat->cancel_bullets();break;
        case 141:case 142:case 143:case 144:globals.combat->spawn_item(CombatItem::Point,vm.position);break;
        default:break;
        }
    }
    globals.record(instruction.opcode,args,count,vm.context().timer.current);
}

bool EclExecutor::jump(EclVm& vm,const EclInstruction& instruction,i32 time,i32 relative) noexcept {
    if(!vm.context().instruction||!vm.program){vm.invalid=true;return false;}
    const auto base=reinterpret_cast<std::uintptr_t>(vm.program->data());
    const i64 offset=i64(reinterpret_cast<std::uintptr_t>(&instruction)-base)+relative;
    if(offset<0||offset>=vm.program->size()){vm.invalid=true;return false;}
    auto* destination=reinterpret_cast<EclInstruction*>(base+u32(offset));
    if(!vm.program->has_instruction(destination)){vm.invalid=true;return false;}
    // Original jump writes only the integer time, preserving its fraction.
    vm.context().timer.current=time;vm.context().branch_instruction=destination;vm.context().branched=true;return true;
}
bool EclExecutor::switch_main(EclVm& vm,i32 subroutine){
    const i32 sub=i16(subroutine);if(sub<0)return true;
    auto* entry=vm.program?vm.program->sub(sub):nullptr;if(!entry){vm.invalid=true;return false;}
    vm.main_context.instruction=entry;vm.main_context.subroutine=sub;vm.main_context.timer.set(0);vm.main_context.wait_timer.set(0);return true;
}

bool EclExecutor::call(EclVm& vm,const EclInstruction& instruction,i32 subroutine){
    auto& context=vm.context();const i32 sub=i16(subroutine);auto* entry=vm.program?vm.program->sub(sub):nullptr;
    if(!entry){vm.invalid=true;return false;}
    if(!(vm.flags&0x4000000)){
        auto& saved=context.call_frames[vm.scratch_depth];saved.time=context.timer;saved.wait=context.wait_timer;saved.subroutine=context.subroutine;saved.locals=context.locals;
        std::memcpy(saved.interpolations,context.interpolations,sizeof(context.interpolations));
        saved.native_callback=context.native_callback;saved.native_instruction=context.native_instruction;
        context.call_stack[vm.scratch_depth]=reinterpret_cast<EclInstruction*>(const_cast<u8*>(reinterpret_cast<const u8*>(&instruction))+instruction.size);
        if(vm.scratch_depth<15)++vm.scratch_depth;
    }
    context.instruction=entry;context.branch_instruction=entry;context.branched=true;context.subroutine=sub;context.timer.set(0);context.wait_timer.set(0);
    for(u32 n=0;n<4;++n){context.locals.integer_arguments[n]=globals.integer_arguments[n];context.locals.float_arguments[n]=globals.float_arguments[n];}
    return true;
}

bool EclExecutor::interrupt(EclVm& vm,const EclInstruction& instruction){
    if(vm.pending_interrupt<0||vm.pending_interrupt>=32){vm.invalid=true;return false;}
    auto& active=vm.context();auto& primary=vm.main_context;
    active.instruction=reinterpret_cast<EclInstruction*>(const_cast<u8*>(reinterpret_cast<const u8*>(&instruction))+instruction.size);
    // The original copies the primary context into the active context's stack,
    // even when an asynchronous script raised the interrupt (0041c8b4).
    if(!(vm.flags&0x4000000)){
        auto& saved=active.call_frames[vm.scratch_depth];saved.time=primary.timer;saved.wait=primary.wait_timer;saved.subroutine=primary.subroutine;saved.locals=primary.locals;
        std::memcpy(saved.interpolations,primary.interpolations,sizeof(primary.interpolations));active.call_stack[vm.scratch_depth]=primary.instruction;
        saved.native_callback=primary.native_callback;saved.native_instruction=primary.native_instruction;
    }
    const i32 sub=vm.interrupt_subroutines[vm.pending_interrupt];
    // InitializeContext leaves everything untouched for a negative subroutine.
    // Interrupts preserve local parameters instead of copying global arguments.
    if(sub>=0){
        auto* entry=vm.program?vm.program->sub(sub):nullptr;if(!entry){vm.invalid=true;return false;}
        primary.instruction=entry;primary.subroutine=sub;primary.timer.set(0);primary.wait_timer.set(0);
    }
    if(vm.scratch_depth<15)++vm.scratch_depth;
    vm.pending_interrupt=-1;active.branch_instruction=active.instruction;active.branched=true;return true;
}

bool EclExecutor::dispatch(EclVm& vm,const EclInstruction& i){
    const auto target_int=[&](u32 n){auto* p=vm.integer_target(i,n);return p?*p:0;};
    const auto target_float=[&](u32 n){auto* p=vm.float_target(i,n);return p?*p:0.f;};
    const auto int_value=[&](u32 n){return vm.operand_int(i,n);};
    const auto float_value=[&](u32 n){return vm.operand_float(i,n);};
    const auto assign_int=[&](u32 n,i32 value){if(auto* p=vm.integer_target(i,n))*p=value;};
    const auto assign_float=[&](u32 n,float value){if(auto* p=vm.float_target(i,n))*p=value;};
    const auto boss=[&](i32 index){return index>=0&&index<8?globals.boss_slots[index]:nullptr;};
    const auto compare_jump=[&](bool condition,u32 time_index=2,u32 offset_index=3){return condition?jump(vm,i,i32(raw_arg(i,time_index)),i32(raw_arg(i,offset_index))):true;};
    switch(i.opcode){
    case -1: case 1: vm.finished=true;return false;
    case 0: case 3: return true;
    case 2: vm.context().wait_timer.set(int_value(0));return true;
    case 4: return jump(vm,i,i32(raw_arg(i,0)),i32(raw_arg(i,1)));
    case 5: {assign_int(2,wrapping_sub(target_int(2),1));return int_value(2)>0?jump(vm,i,i32(raw_arg(i,0)),i32(raw_arg(i,1))):true;}
    case 6: assign_int(0,int_value(1));return true;
    case 7: assign_float(0,float_value(1));return true;
    case 8: {const i32 value=int_value(1);assign_int(0,(rng.next16()&1)?value:wrapping_sub(0,value));return true;}
    case 9: {const float sign=(rng.next16()&1)?1.f:-1.f;assign_float(0,(number(sign)*number(float_value(1))).to_float());return true;}
    case 10: {const i32 b=int_value(1);assign_int(0,wrapping_add(target_int(0),b));return true;}
    case 11: {const i32 b=int_value(1);assign_int(0,wrapping_sub(target_int(0),b));return true;}
    case 12: {const i32 b=int_value(1);assign_int(0,signed_bits(u32(target_int(0))*u32(b)));return true;}
    case 13: {const i32 b=int_value(1);if(!b){vm.invalid=true;return false;}assign_int(0,divide(target_int(0),b));return true;}
    case 14: {const i32 b=int_value(1);if(!b){vm.invalid=true;return false;}assign_int(0,modulo(target_int(0),b));return true;}
    case 15: {const float b=float_value(1);assign_float(0,(number(b)+number(target_float(0))).to_float());return true;}
    case 16: {const float b=float_value(1);assign_float(0,(number(target_float(0))-number(b)).to_float());return true;}
    case 17: {const float b=float_value(1);assign_float(0,(number(b)*number(target_float(0))).to_float());return true;}
    case 18: {const float b=float_value(1);assign_float(0,(number(target_float(0))/number(b)).to_float());return true;}
    case 19: {const float b=float_value(1);assign_float(0,float_remainder(float_value(0),b).to_float());return true;}
    case 20: {const i32 a=int_value(1),b=int_value(2);assign_int(0,wrapping_add(a,b));return true;}
    case 21: {const i32 a=int_value(1),b=int_value(2);assign_int(0,wrapping_sub(a,b));return true;}
    case 22: {const i32 a=int_value(1),b=int_value(2);assign_int(0,signed_bits(u32(a)*u32(b)));return true;}
    case 23: {const i32 a=int_value(1),b=int_value(2);if(!b){vm.invalid=true;return false;}assign_int(0,divide(a,b));return true;}
    case 24: {const i32 a=int_value(1),b=int_value(2);if(!b){vm.invalid=true;return false;}assign_int(0,modulo(a,b));return true;}
    case 25: {const float a=float_value(1),b=float_value(2);assign_float(0,Scalar::add(a,b));return true;}
    case 26: {const float a=float_value(1),b=float_value(2);assign_float(0,Scalar::sub(a,b));return true;}
    case 27: {const float a=float_value(1),b=float_value(2);assign_float(0,Scalar::mul(a,b));return true;}
    case 28: {const float a=float_value(1),b=float_value(2);assign_float(0,Scalar::div(a,b));return true;}
    case 29: {const float b=float_value(2),a=float_value(1);assign_float(0,float_remainder(a,b).to_float());return true;}
    case 30: assign_int(0,wrapping_add(target_int(0),1));return true;
    case 31: assign_int(0,wrapping_sub(target_int(0),1));return true;
    case 32: assign_float(0,safe_sin(float_value(1)));return true;
    case 33: assign_float(0,safe_cos(float_value(1)));return true;
    case 34: {
        const float x2=float_value(3),x1=float_value(1),y2=float_value(4),y1=float_value(2);
        const float dx=Scalar::sub(x2,x1),dy=Scalar::sub(y2,y1);
        // 004194fb uses the original CRT atan2, not a float-only libm path.
        assign_float(0,Extended::from_double(std::atan2(double(dy),double(dx))).to_float());return true;
    }
    case 35: {const float a=float_value(1),b=float_value(2),t=float_value(3),base=float_value(2);assign_float(0,((number(a)-number(b))*number(t)+number(base)).to_float());return true;}
    case 36: {
        if(i.size<44){vm.invalid=true;return false;}
        for(auto& interpolation:vm.context().interpolations)if(!interpolation.active||interpolation.target==raw_float(i,0)){
            interpolation.time.set(0);interpolation.target=raw_float(i,0);
            interpolation.duration=int_value(1);interpolation.curve=int_value(2);interpolation.easing=int_value(3);
            if(interpolation.curve<0||interpolation.curve>7){vm.invalid=true;return false;}
            interpolation.active=1;for(u32 a=0;a<4;++a)interpolation.values[a]=float_value(a+4);break;
        }
        return true;
    }
    case 37: assign_float(0,add_angle(float_value(0),0));return true;
    case 38: {const float angle=add_angle(float_value(2),0),radius=float_value(3);assign_float(0,(cosine(angle)*number(radius)).to_float());assign_float(1,(sine(angle)*number(radius)).to_float());return true;}
    case 39: {
        const float x1=float_value(1),x2=float_value(3),y1=float_value(2),y2=float_value(4);
        const float dx=Scalar::sub(x1,x2),dy=Scalar::sub(y1,y2);
        const float squared=(number(dx)*number(dx)+number(dy)*number(dy)).to_float();
        assign_float(0,number(squared).square_root().to_float());return true;
    }
    case 40: return compare_jump(int_value(0)==int_value(1));
    case 41: return compare_jump(float_value(0)==float_value(1));
    case 42: return compare_jump(int_value(0)!=int_value(1));
    case 43: return compare_jump(float_value(0)!=float_value(1));
    case 44: return compare_jump(int_value(0)<int_value(1));
    case 45: return compare_jump(float_value(0)<float_value(1));
    case 46: return compare_jump(int_value(0)<=int_value(1));
    case 47: return compare_jump(float_value(0)<=float_value(1));
    case 48: return compare_jump(int_value(0)>int_value(1));
    case 49: return compare_jump(float_value(0)>float_value(1));
    case 50: return compare_jump(int_value(0)>=int_value(1));
    case 51: return compare_jump(float_value(0)>=float_value(1));
    case 52: {
        call(vm,i,i32(raw_arg(i,0)));return false;
    }
    case 53: {
        if(vm.scratch_depth==0){vm.context().returned=true;vm.scratch_depth=vm.main_context.call_depth;return false;}
        vm.context().instruction=vm.context().call_stack[--vm.scratch_depth];const auto& saved=vm.context().call_frames[vm.scratch_depth];
        vm.context().timer=saved.time;vm.context().wait_timer=saved.wait;vm.context().subroutine=saved.subroutine;
        std::memcpy(vm.context().interpolations,saved.interpolations,sizeof(vm.context().interpolations));
        vm.context().locals=saved.locals;
        vm.context().native_callback=saved.native_callback;vm.context().native_instruction=saved.native_instruction;
        vm.context().branch_instruction=vm.context().instruction;vm.context().branched=true;
        return false;
    }
    case 63:case 64:case 65:case 66:case 67:case 68:case 69:case 70:case 71:case 72:case 73:case 74:case 75:case 76:case 178:
        if(!vm.execute_motion(i))return false;record(vm,i);return true;
    case 77: vm.hitbox.x=float_value(0);vm.hitbox.y=float_value(1);record(vm,i);return true;
    case 78: vm.low_damage_hitbox.x=float_value(0);vm.low_damage_hitbox.y=float_value(1);record(vm,i);return true;
    case 79:{const u32 value=u32(int_value(0));vm.flags=(vm.flags&~0x1000005cu)|((value&1)?0:0x40)|((value&2)?0:4)|((value&4)?0:8)|((value&8)?0x10:0)|((value&16)?0x10000000:0);vm.flags2=(vm.flags2&~0x40u)|((value&32)?0x40:0);record(vm,i);return true;}
    case 80:case 81:{
        const u32 value=u32(int_value(0));const bool enable=i.opcode==80;
        const auto flag=[&](u32 bit,bool set){if(set)vm.flags|=bit;else vm.flags&=~bit;};
        if(value&1)flag(0x40,!enable);
        if(value&2){flag(4,!enable);if(vm.familiar_effect)vm.familiar_effect->flag17=!enable;}
        if(value&4)flag(8,!enable);if(value&8)flag(16,enable);if(value&16)flag(0x10000000,enable);
        if(value&32){if(enable)vm.flags2|=0x40;else vm.flags2&=~0x40u;}
        record(vm,i);return true;
    }
    case 82:{const float range=float_value(0);vm.player_protect_squared=Scalar::mul(range,range);record(vm,i);return true;}
    case 83:vm.flags2=(vm.flags2&~2u)|((u32(int_value(0))&1)<<1);record(vm,i);return true;
    case 86:{
        i32 value=i32(raw_arg(i,1));
        if(is_variable(i,1)){auto* source=boss(int_value(2));if(!source){vm.failure=EclVm::Failure::MissingBoss;vm.invalid=true;return false;}value=source->read_int(value);}
        assign_int(0,value);record(vm,i);return true;
    }
    case 87:{
        if(boss(int_value(2))){float value=raw_float(i,1);
            if(is_variable(i,1)){auto* source=boss(int_value(2));if(!source){vm.failure=EclVm::Failure::MissingBoss;vm.invalid=true;return false;}value=source->resolve_float(value).to_float();}
            assign_float(0,value);
        }
        record(vm,i);return true;
    }
    case 88:{
        auto* target=boss(int_value(0));if(!target||!target->context().instruction){vm.failure=EclVm::Failure::MissingBoss;vm.invalid=true;return false;}
        if(!call(*target,*target->context().instruction,i32(raw_arg(i,1)))){vm.invalid=true;return false;}
        record(vm,i);return true;
    }
    case 89:{
        if(boss(int_value(0))){const i16 interrupt=i16(int_value(1));auto* target=boss(int_value(0));if(!target){vm.invalid=true;return false;}target->pending_interrupt=interrupt;}
        record(vm,i);return true;
    }
    case 125:vm.pending_interrupt=i16(int_value(0));return interrupt(vm,i);
    case 126:{
        const i16 sub=i16(int_value(0));const i32 index=int_value(1);
        if(index<0||index>=32){vm.invalid=true;return false;}vm.interrupt_subroutines[index]=sub;return true;
    }
    case 135:{
        if(i.size<20){vm.invalid=true;return false;}const i32 slot=int_value(0);
        if(slot<0||slot>=4){vm.invalid=true;return false;}
        ++vm.asynchronous_generations[slot];vm.asynchronous[slot].reset();
        if(int_value(1)>=0){
            const i32 sub=i16(int_value(1));auto* entry=vm.program?vm.program->sub(sub):nullptr;
            if(!entry){vm.invalid=true;return false;}
            auto next=std::make_unique<EclContext>();next->instruction=entry;next->subroutine=sub;next->timer.set(0);next->wait_timer.set(0);
            next->locals=vm.context().locals;
            vm.asynchronous[slot]=std::move(next);
        }
        record(vm,i);return true;
    }
    case 127:case 129:case 130:case 131:case 132:case 133:case 134:case 138:case 160:case 175:case 177:case 182:case 183:
        if(!vm.execute_lifecycle(i))return false;record(vm,i);return true;
    case 54: case 55: case 56: case 57: case 58: case 59: case 60: case 61: case 62:return animate(vm,i);
    case 96: case 97: case 98: case 99: case 100: case 101: case 102: case 103: case 104:
    case 105: case 106: case 107: case 108: case 109: case 110: case 111: case 112: case 113:
        if(!configure_emitter(vm,i))return false;record(vm,i);return true;
    case 114:case 115:case 116:case 117:case 118:case 119:case 120:case 121:case 154:case 167:case 170:case 171:case 172:
        if(!configure_laser(vm,i))return false;record(vm,i);return true;
    case 90:case 91:case 92:case 93:case 94:
        if(!globals.enemy_actions){vm.invalid=true;vm.failure=EclVm::Failure::MissingEnemyActions;return false;}
        if(!execute_enemy_spawn(vm,i,globals,*globals.enemy_actions))return false;record(vm,i);return true;
    case 157:if(!configure_enemy_trail(vm,i))return false;record(vm,i);return true;
    case 143:case 144:case 145:case 149:case 150:case 151:case 152:case 153:case 155:case 156:case 159:case 165:case 166:case 169:case 173:
        if(!configure_enemy_control(vm,i))return false;globals.record(i.opcode,reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(&i)+12),(i.size-12)/4,vm.context().timer.current);return true;
    case 95:case 124:case 128:case 139:case 140:case 141:case 142:case 146:case 147:case 148:case 158:case 161:case 162:case 163:case 164:case 168:case 174:case 176:case 179:case 180:case 181:case 184:
        if(!execute_enemy_scene(vm,i,timing))return false;globals.record(i.opcode,reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(&i)+12),(i.size-12)/4,vm.context().timer.current);return true;
    case 136:case 137:return configure_ecl_native(vm,i);
    case 122: case 123:
        if(!globals.spell_actions){vm.invalid=true;vm.failure=EclVm::Failure::MissingSpellServices;return false;}
        if(!(i.opcode==122?globals.spell_actions->begin(vm,i):globals.spell_actions->end())){vm.invalid=true;return false;}
        globals.record(i.opcode,reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(&i)+12),(i.size-12)/4,vm.context().timer.current);return true;
    default: record(vm,i);return true;
    }
}

bool EclExecutor::step_context(EclVm& vm){
    if(vm.finished||vm.invalid||!vm.context().instruction)return false;
    vm.context().returned=false;
    vm.scratch_depth=vm.context().call_depth;
    auto* cursor=vm.context().instruction;
    if(vm.pending_interrupt>=0){
        if(!vm.program||!vm.program->has_instruction(cursor)){vm.invalid=true;return false;}
        if(!interrupt(vm,*cursor))return false;cursor=vm.context().instruction;
    }
    u32 budget=0;
    while(cursor&&budget++<10000){
        vm.refresh_position();
        if(vm.context().wait_timer.current>0){vm.context().wait_timer.decrement(1,timing);vm.context().timer.decrement(1,timing);break;}
        if(!vm.program||!vm.program->has_instruction(cursor)){vm.invalid=true;return false;}
        const EclInstruction& instruction=*cursor;
        if(instruction.time!=vm.context().timer.current)break;
        const u32 difficulty=globals.difficulty_mask|vm.difficulty_flags;
        vm.context().branched=false;
        const bool advance=(instruction.difficulties&difficulty)==difficulty?dispatch(vm,instruction):true;
        if(vm.finished||vm.invalid)return false;
        if(vm.context().returned)return true;
        if(vm.context().branched){cursor=vm.context().branch_instruction;continue;}if(!advance)break;
        cursor=reinterpret_cast<EclInstruction*>(reinterpret_cast<u8*>(cursor)+instruction.size);
    }
    if(budget>=10000){vm.invalid=true;return false;}
    if(cursor&&!vm.finished&&!vm.invalid){if(vm.life>0)vm.interpolate(timing);if(vm.invalid)return false;vm.context().call_depth=vm.scratch_depth;vm.context().instruction=cursor;vm.context().timer.tick(timing);}
    return !vm.finished&&!vm.invalid;
}

bool EclExecutor::step(EclVm& vm){
    if(vm.finished||vm.invalid||!vm.main_context.instruction)return false;
    vm.active_context=nullptr;vm.active_slot=-1;
    if(!step_context(vm))return false;
    // Original order: primary context, slots 0..3, then movement. Temporarily
    // owning the active slot keeps deletion/replacement memory-safe without
    // copying call stacks or local-variable arrays every frame.
    for(u32 slot=0;slot<4;++slot)if(vm.asynchronous[slot]){
        const u32 generation=vm.asynchronous_generations[slot];auto current=std::move(vm.asynchronous[slot]);
        vm.active_context=current.get();vm.active_slot=i32(slot);const bool result=step_context(vm);
        vm.active_context=nullptr;vm.active_slot=-1;
        if(current->returned)vm.asynchronous[slot].reset();
        else if(generation==vm.asynchronous_generations[slot])vm.asynchronous[slot]=std::move(current);
        if(!result)return false;
    }
    vm.update_motion(timing);return repeat_shot(vm)&&update_pose(vm);
}

}
