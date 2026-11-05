#pragma once
#include "MenuCursor.hpp"
#include "AnmManager.hpp"
#include "UpdateChain.hpp"
#include "BackgroundThread.hpp"
namespace th10 {
struct Replay;
struct TitleMainEnvironment;
struct TitleAnimationEnvironment {
    AnmRegistry* registry;
    const float* rate;
    const Vec3* tangent;
    virtual u32 create(AnmFile& file,i32 script)=0;
};
struct TitleMenu {
    u32 original_virtual_table,flags,manager_state;
    UpdateChainEntry *update_entry,*draw_entry;
    AnmFile *animations,*version_animations;
    i32 screen,phase;
    MenuCursor menu,secondary,tertiary;
    u32 reserved_2ac;
    Timer elapsed;
    u32 timer_flags;
    u32 animation_ids[232];
    u32 music_comment_animations[8],playing_animation;
    i32 music_track_count,music_filled_comments,music_playing,music_warning;
    char music_files[32][64];
    char music_titles[32][66];
    char music_comments[32][8][66];
    i32 music_scroll;
    char name[9];
    u8 reserved_58e5[3];
    i32 name_length,no_rank;
    i32 saved_difficulty;
    MenuCursor replay_menu;
    u16 configured_keys[5];
    u16 reserved_59d6;
    i32 replay_page,replay_index,replay_stage;
    Replay* previews[50];
    char* music_file;
    BackgroundThread loader;
    void initialize(TitleMenu** current) noexcept;
    void reset_timer(const float* rate) noexcept;
    void set_screen(i32 screen,const float* rate) noexcept;
    void set_phase(i32 phase,const float* rate) noexcept;
    void create_script(i32 script,TitleAnimationEnvironment& environment);
    void dismiss_script(i32 script,TitleAnimationEnvironment& environment);
    void signal_script(i32 script,i32 signal,TitleAnimationEnvironment& environment);
    i32 update_prompt(TitleMainEnvironment& environment);
    i32 update_main(TitleMainEnvironment& environment);
    u32& music_title_animation(i32 index) noexcept {return animation_ids[212+index];}
};
static_assert(offsetof(TitleMenu,elapsed)==0x2b0 && offsetof(TitleMenu,animation_ids)==0x2c4);
static_assert(offsetof(TitleMenu,music_comment_animations)==0x664 && offsetof(TitleMenu,music_files)==0x698);
static_assert(offsetof(TitleMenu,music_titles)==0xe98 && offsetof(TitleMenu,music_comments)==0x16d8);
static_assert(offsetof(TitleMenu,music_scroll)==0x58d8 && offsetof(TitleMenu,music_file)==0x5aac && sizeof(TitleMenu)==0x5acc);
void interpolate_menu_position(AnmVm& vm,const Vec3& end,const Vec3& start,i32 duration,u8 mode,const Vec3& tangent,const float* rate) noexcept;
}
