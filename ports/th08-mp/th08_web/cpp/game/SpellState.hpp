#pragma once
#include "AnmLayout.hpp"
namespace th08 {
struct EclVm;struct EffectState;
// Original 004ea670 spellcard owner. Animation objects and effect references
// are ordinary C++ objects shared by ECL, score history and presentation.
struct SpellState {
    u32 spell_flags=0;EclVm* spell_enemy=nullptr;u32 spell_number=0,spell_enemy_index=0;i32 spell_time_items=0;
    char spell_name[48]{},reserved_name[48]{},spell_comment1[64]{},spell_comment2[64]{};
    EffectState* spell_effect=nullptr;EffectState* spell_reward_effect=nullptr;
    u32 spell_bonus=0,spell_bonus_decay=0,spell_pending_bonus=0;
    Timer spell_remaining,spell_initial;
    AnmVm spell_vms[14];float spell_player_name_width=0,spell_enemy_name_width=0;u32 spell_panel_color=0;
    AnmLoaded *spell_human_face=nullptr,*spell_youkai_face=nullptr,*spell_enemy_face=nullptr,*spell_enemy_face2=nullptr;
    AnmLoaded* spell_banners=nullptr;i32 spell_capture_bonus=0;u32 reserved263c=0;
    SpellState(){std::memset(this,0,sizeof(*this));}
    void end_player_announcement(){spell_vms[6].pendingInterrupt=1;spell_vms[10].pendingInterrupt=2;}
    void end_enemy_announcement(){spell_vms[7].pendingInterrupt=1;spell_vms[11].pendingInterrupt=2;if(!(spell_flags&1024))spell_vms[13].pendingInterrupt=2;}
};
static_assert(sizeof(SpellState)==0x2640&&offsetof(SpellState,spell_effect)==0xf4&&offsetof(SpellState,spell_vms)==0x120&&offsetof(SpellState,spell_capture_bonus)==0x2638);
}
