// Original TH08 GUI data layouts. Runtime resources use normal C++ pointers.
#pragma once
#include "AnmLayout.hpp"
#include "MessageProgram.hpp"
namespace th08 {
struct DialogueState {
    const u8* file=nullptr;const u8* instruction=nullptr;i32 message=-1;Timer timer;
    i32 paused_frames=0,minimum_wait=0;
    AnmVm portraits[4],lines[2],intro[2];
    u32 colors[4]{},outline_colors[4]{},font_size=0,ignore_wait=0;
    bool skippable=false;u8 color=0;bool reset_lines=false;u8 next_line=0,portrait=0;bool textbox_visible=false;u8 selected=0,padding=0;
};
static_assert(sizeof(DialogueState)==0x1570);
struct GuiFormattedText {Vec3 position;i32 argument=0,display=0;Timer timer;};
struct GuiDisplayFlags {u32 lives:2,bombs:2,power:2,graze:2,points:2,time:2,reserved:20;};
struct GuiState {
    u32 frame=0;GuiDisplayFlags flags{};struct GuiImplState* implementation=nullptr;
    AnmLoaded *front=nullptr,*stage_text=nullptr,*times=nullptr,*loading_portrait=nullptr;
    u32 boss_opacity=0;i32 ecl_lives=0,spell_seconds=0,previous_spell_seconds=0;
    bool boss_present=false;u8 padding29[3]{};float boss_life_max=0,boss_life=0;u32 unknown38=0;
    float segment_end[8]{},segment_start[8]{};i32 segment_colors[8]{};
};
struct GuiImplState {
    AnmVm front[16];u8 boss_life_state=0,padding[3]{};
    AnmVm stage_text[4],clock_intro,loading_portrait,unknown3a1c,arcade,arcade_blur[8],nullify,transition[168],stage_rank,clock;
    i32 transition_count=0;DialogueState dialogue;
    i32 clear_frames=0,clear_total=0;GuiFormattedText bonus,popup,spell_bonus;
    i32 clear_stage=0,clear_power=0,clear_points=0,clear_graze=0,clear_time=0,clock_increment=0,clear_clock_old=0,clear_clock=0,clear_clock_display=0,clear_clock_delay=0;
    AnmVm difficulty;
};
static_assert(sizeof(GuiState)==0x9c&&sizeof(GuiImplState)==0x230b8);
}
