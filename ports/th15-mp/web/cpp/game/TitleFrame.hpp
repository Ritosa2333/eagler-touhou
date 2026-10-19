#pragma once
#include "TitleState.hpp"
#include "TitleReplayMenu.hpp"
#include "RecordStore.hpp"
#include "FrameScheduler.hpp"
namespace th15 {
struct TitleFrameInput {u32 held=0,pressed=0,repeated=0,system_flags=0;};
struct TitleFramePlatform {
 virtual ~TitleFramePlatform()=default;
 virtual bool release_transient_animations()=0;virtual bool clear_title_overlay()=0;
 virtual bool read_demo(i32,std::shared_ptr<Replay>&)=0;virtual bool begin_demo(const ReplayStartRequest&)=0;
 virtual bool queue_title_music(i32 code,i32 value,const std::string&)=0;virtual bool clear_current_wave()=0;
 virtual bool reset_replay_selection()=0;virtual bool return_practice_transition()=0;
 virtual bool title_exit(i32 destination)=0;virtual bool fade_out_title_music()=0;
};
struct TitleFrameServices:TitleFramePlatform {
 virtual bool update_title_menu(TitleScreen,const TitleFrameInput&)=0;virtual bool draw_title_menu(TitleScreen)=0;
};
// Original priority-6 update and priority-66 text drawing around the named
// title menus. The host owns asynchronous files, audio and app destinations.
class TitleFrame {
 TitleState& state;SessionState& progress;PlayerLifeSession& player;RecordStore& records;TitleFrameServices& host;FrameScheduler& scheduler;
 std::array<FrameCallback,2> callbacks;TitleFrameInput input;TitleAnimations visuals;
 bool check(bool,const char*);bool menu();bool initialize();bool restore_music(bool stop_previous);bool demo();
public:
 i32 demo_idle=0,demo_index=0,saved_difficulty=0,music_age=0;bool alternate_audio=false;std::string error;
 TitleFrame(TitleState&,SessionState&,PlayerLifeSession&,RecordStore&,AnmManager&,TitleFrameServices&,FrameScheduler&,i32 title_bank=16,i32 ascii_bank=5);
 ~TitleFrame();void controls(const TitleFrameInput& value)noexcept{input=value;}
 bool update(const TitleFrameInput&);bool draw();bool drawing()const noexcept{return callbacks[1].enabled;}
};
}
