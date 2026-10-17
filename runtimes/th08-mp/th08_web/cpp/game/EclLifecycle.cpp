#include "EclVm.hpp"
#include "GuiState.hpp"
#include "AsciiManager.hpp"
#include "EffectState.hpp"
namespace th08 {
// Original ECL boss/lifetime settings. Threshold triggering, death execution
// and timeout cancellation are separate enemy-manager phases.
bool EclVm::execute_lifecycle(const EclInstruction& instruction){
    if(!environment){invalid=true;return false;}auto& globals=*environment;
    const i32 op=instruction.opcode;const u32 arguments=op==133?3:op==134?2:1;
    if(instruction.size<12+arguments*4){invalid=true;return false;}
    const auto n=[&](u32 i){return operand_int(instruction,i);};
    const auto raw=[&](u32 i){u32 value;std::memcpy(&value,reinterpret_cast<const u8*>(&instruction)+12+i*4,4);return value;};
    const auto boss_index=[&](i32 index){if(index<0||index>=8){invalid=true;return false;}return true;};
    const auto life_index=[&](i32 index){if(index<0||index>=4){invalid=true;return false;}return true;};
    const bool restricted=(globals.game_flags&0x4000)&&(globals.game_flags&0x180);
    switch(op){
    case 127:
        if(n(0)<0){
            const u32 index=u8(boss_id);if(!boss_index(i32(index)))return false;
            if(index<4){globals.gui_blocks_spawn=false;if(globals.gui)globals.gui->boss_present=false;}
            globals.boss_slots[index]=nullptr;flags&=~2u;
            if(globals.ascii&&index<4)globals.ascii->boss_markers[index].pendingInterrupt=2;
            if(effect_count<0||effect_count>24){invalid=true;return false;}
            for(i32 i=0;i<effect_count;++i)if(effects[i]){effects[i]->dying=1;effects[i]=nullptr;}effect_count=0;
            if(globals.ascii&&index<4)globals.ascii->boss_markers[index].pos={-999,-999,0};
        }else{
            const i32 index=n(0);if(!boss_index(index))return false;globals.boss_slots[index]=this;
            if(n(0)==0){globals.gui_blocks_spawn=true;if(globals.gui){globals.gui->boss_present=true;globals.gui->boss_life_max=1;}}
            flags|=2;boss_id=u8(n(0));
            if(globals.ascii&&u32(boss_id)<4)globals.ascii->boss_markers[boss_id].pendingInterrupt=1;
            player_protect_squared=0;
        }
        break;
    case 129:if(!restricted)flags=(flags&~0x700000u)|(raw(0)&7)<<20;break;
    case 130:if(!restricted)death_subroutine=i16(raw(0));break;
    case 131:{const i32 value=n(0);life=initial_life=remaining_life=value;if(u8(boss_id)==0&&(flags&2)&&globals.gui){for(auto& x:globals.gui->segment_start)x=0;for(auto& x:globals.gui->segment_end)x=0;}break;}
    case 132:lifetime.set(n(0));break;
    case 133:{
        const i32 value=n(1),index=n(0);if(!life_index(index))return false;life_thresholds[index]=value;
        if(!restricted){const i32 sub=n(2),target=n(0);if(!life_index(target))return false;life_subroutines[target]=sub;}
        break;
    }
    case 134:timeout=n(0);if(!restricted)timeout_subroutine=n(1);lifetime.set(0);break;
    case 138:{const u32 value=raw(0);for(u32 i=0;i<3;++i)death_effects[i]=u8(value>>(8*i));break;}
    case 160:damage_protection.set(n(0));break;
    case 175:globals.stop_spawn=n(0);break;
    case 177:remaining_life=n(0);break;
    case 182:flags2=(flags2&~0x100u)|((u32(n(0))&1)<<8);break;
    case 183:flags=(flags&~0x80000000u)|(u32(n(0))<<31);break;
    default:return false;
    }
    return !invalid;
}
}
