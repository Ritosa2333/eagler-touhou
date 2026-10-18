#pragma once
#include "EclVm.hpp"
#include "UiMenus.hpp"
#include "Chain.hpp"
namespace th08 {
struct GameplayControlState {
    i32 frames=0,restore_viewport=0,next_scene=-1,load_state=0,load_frames=0,start_music=0;
    u32 slow_frames=0,play_frames=0;i32 demo_frames=0,demo_index=3,replay_mode=0;
    u16 stage_mask=1;bool sticky_input=false,loading_images_setup=false;
};
struct GameplayControlInput {i32 replay_fps=60,active_bullets=0;u32 fog=0xff000000;bool dialogue=false;};
struct GameplayControlActions {
    virtual ~GameplayControlActions()=default;
    virtual bool replay_stage(i32 stage)=0;
    virtual void update_enemy_name()=0;
    virtual void release_loading_surface()=0;
    virtual void play_music(i32 slot,i32 song)=0;
    virtual void pause_audio()=0;
    virtual void sound(i32 index)=0;
    virtual void update_game_time()=0;
    virtual void capture_arcade()=0;
    virtual void demo_fade()=0;
};
// Original GameManager 00439bc7 / 0043aa03 and branching 0043c4b3.
// Device/resource/replay operations use explicit services; score, transitions,
// pause, timing gates, clears and numeric side effects execute here in C++.
class GameplayControl {
    EclGlobals& game;MenuContext& menu;GameGlobals& numbers;GameValues& values;GameConfiguration& config;
    HighScore& high_score;ClearRecord* clears;PlayRecord& play;Rng& random;AnmRenderer& renderer;GameplayControlActions& actions;
    void refresh_integrity();void late_integrity();void display_score();
public:
    GameplayControlState state;GameplayControlInput input;
    GameplayControl(EclGlobals& g,MenuContext& m,GameGlobals& n,GameValues& v,GameConfiguration& c,HighScore& h,ClearRecord* records,PlayRecord& p,Rng& rng,AnmRenderer& r,GameplayControlActions& a)
      :game(g),menu(m),numbers(n),values(v),config(c),high_score(h),clears(records),play(p),random(rng),renderer(r),actions(a){}
    JobResult update();JobResult draw();void advance_stage();
};
}
