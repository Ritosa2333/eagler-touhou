#pragma once
#include "TitleMenu.hpp"
#include "GameProgression.hpp"
#include "Replay.hpp"
namespace th10 {
struct TitleLoopEnvironment : TitleAnimationEnvironment {
    GameEconomy* game;
    i32 *return_screen,*inactive_frames,*demo_index,*pending_screen;
    const u32 *held,*engine_flags;
    AnmFile** files;
    u32* loading_animation;
    const StageConfiguration* stages;
    const StageConfiguration** current_stage;
    const char* const* demo_files;
    char* replay_filename;
    virtual Replay* load_replay(const char* name)=0;
    virtual void delete_replay(Replay* replay)=0;
    virtual void play_title_music()=0;
    virtual void stop_music()=0;
    virtual void update_menu(TitleMenu& title,i32 screen)=0;
    virtual void draw_menu(TitleMenu& title,i32 screen)=0;
};
i32 update_title(TitleMenu& title,TitleLoopEnvironment& environment);
i32 draw_title(TitleMenu& title,TitleLoopEnvironment& environment);
}
