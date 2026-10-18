#include "EnemyStatus.hpp"
#include "EclVm.hpp"
namespace th09 {
bool EnemyStatus::command(EclVm& vm,EclInstruction& i){
    const auto iv=[&](u32 n){return vm.integer(i,n);};const auto fv=[&](u32 n){return vm.real(i,n);};
    const auto raw=[&](u32 n){return vm.raw(i,n);};
    const auto flag=[&](u32 shift,u32 value){const u32 mask=1u<<shift;vm.behavior_flags=(vm.behavior_flags&~mask)|((value&1u)<<shift);};
    auto& v=vm.values;
    switch(i.opcode){
    case 128:attached_effect_radius=vm.raw_float(i,4);attached_effect_count=wrapping_add(attached_effect_count,1);break;
    case 129:vm.behavior_flags=(vm.behavior_flags&~0xe0000u)|((raw(0)&7)<<17);break;
    case 130:death_subroutine=i16(raw(0));break;
    case 131:v.life=iv(0);initial_life=maximum_life=v.life;break;
    case 132:v.lifetime.reset(iv(0));break;
    case 133:{
        const i32 threshold=iv(1),index=iv(0);if(index<0||index>=4)return false;v.life_thresholds[index]=threshold;
        const i32 sub=iv(2),second_index=iv(0);if(second_index<0||second_index>=4)return false;life_subroutines[second_index]=sub;break;
    }
    case 134:timeout=iv(0);timeout_subroutine=iv(1);v.lifetime.reset();break;
    case 138:{const u32 packed=raw(0);for(u32 n=0;n<3;++n)effects[n]=i8(packed>>(n*8));break;}
    case 143:v.item_reward=iv(0);break;
    case 144:v.drop_count=iv(0);v.drop_item=iv(1);break;
    case 145:flag(22,raw(0));break;
    case 149:vm.animation.layers[0].pendingInterrupt=i16(iv(0));break;
    case 150:{const i32 index=signed_bits(raw(0));if(index<0||index>=2)return false;vm.animation.layers[index+1].pendingInterrupt=i16(raw(1));break;}
    case 152:motion_range={fv(0),fv(1)};for(u32 n=0;n<4;++n)motion_modes[n]=i16(iv(n+2));break;
    case 153:timeout_subroutine=death_subroutine;v.lifetime.reset();break;
    case 155:flag(24,raw(0));break;
    case 156:flag(7,raw(0));draw_group=2;break;
    case 159:draw_group=u8(iv(0));break;
    case 160:invulnerable.reset(iv(0));break;
    case 161:(void)fv(0);break; // Original helper 0x4125a0 is empty; evaluation remains observable.
    case 162:break; // Original helper 0x412590 is empty.
    case 165:vm.animation.layers[0].rotation.z=fv(0);break;
    case 173:flag(27,u32(iv(0)));break;
    case 177:maximum_life=iv(0);break;
    case 182:v.flags=(v.flags&~0x20u)|((u32(iv(0))&1)<<5);break;
    case 183:flag(28,u32(iv(0)));break;
    case 185:v.flags=(v.flags&~0xc00u)|((u32(iv(0))&3)<<10);break;
    case 187:v.flags=(v.flags&~0x4000u)|((u32(iv(0))&1)<<14);break;
    default:return false;
    }
    return !vm.invalid;
}
}
