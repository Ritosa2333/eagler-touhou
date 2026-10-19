#include "EclControl.hpp"
#include "EclVm.hpp"
#include "GameMath.hpp"
namespace th08 {
// ECL object settings and local math from the original 004184b0 switch.
// Raw-byte flags and raw animation indices intentionally bypass variables.
bool configure_enemy_control(EclVm& vm,const EclInstruction& ins){
    const auto n=[&](u32 i){return vm.operand_int(ins,i);};
    const auto f=[&](u32 i){return vm.operand_float(ins,i);};
    const auto raw=[&](u32 i){u32 value=0;std::memcpy(&value,reinterpret_cast<const u8*>(&ins)+12+i*4,4);return value;};
    u32 arguments=1;switch(ins.opcode){case 144:case 150:arguments=2;break;case 152:arguments=6;break;case 153:arguments=0;break;case 166:arguments=4;break;}
    if(ins.size<12+4*arguments){vm.invalid=true;return false;}
    switch(ins.opcode){
    case 143:vm.item_reward=n(0);break;
    case 144:vm.point_items=n(0);vm.power_items=n(1);break;
    case 145:vm.flags=(vm.flags&~0x2000000u)|((raw(0)&1)<<25);break;
    case 149:vm.animation[0].pendingInterrupt=i16(n(0));break;
    case 150:{const u32 index=raw(0);if(index>=2){vm.invalid=true;return false;}vm.animation[index+1].pendingInterrupt=i16(raw(1));break;}
    case 151:vm.flags=(vm.flags&~0x4000000u)|((raw(0)&1)<<26);break;
    case 152:vm.rank_speed_low=f(0);vm.rank_speed_high=f(1);vm.rank_count_low=i16(n(2));vm.rank_count_high=i16(n(3));vm.rank_layers_low=i16(n(4));vm.rank_layers_high=i16(n(5));break;
    case 153:vm.timeout_subroutine=vm.death_subroutine;vm.lifetime.set(0);break;
    case 155:vm.flags=(vm.flags&~0x8000000u)|((raw(0)&1)<<27);if(vm.environment)vm.environment->spell_capture_bonus=99999990;break;
    case 156:vm.flags=(vm.flags&~0x80u)|((raw(0)&1)<<7);vm.draw_layer=2;break;
    case 159:vm.draw_layer=u8(n(0));break;
    case 165:vm.animation[0].rotation.z=f(0);break;
    case 166:{
        const float ay=f(2),sy=f(3);const Extended y=sine(ay);auto* py=vm.float_target(ins,1);if(!py)return false;*py=(y*number(sy)).to_float();
        const float ax=f(2),sx=f(3);const Extended x=cosine(ax);auto* px=vm.float_target(ins,0);if(!px)return false;*px=(x*number(sx)).to_float();break;
    }
    case 169:{
        if(!vm.random||!vm.environment){vm.invalid=true;return false;}
        const bool left=(vm.environment->player.x<vm.position.x&&96<vm.position.x)||288<vm.position.x;
        const Extended random=vm.random->range(1.5707963705062866f);
        const float angle=left?add_angle((random+number(2.356194496154785f)).to_float(),0):(random-number(.7853981852531433f)).to_float();
        auto* target=vm.float_target(ins,0);if(!target)return false;*target=angle;break;
    }
    case 173:vm.flags=(vm.flags&~0x40000000u)|((u32(n(0))&1)<<30);break;
    default:return false;
    }
    return !vm.invalid;
}
}
