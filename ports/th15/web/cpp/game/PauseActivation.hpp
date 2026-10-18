#pragma once
#include "PauseState.hpp"
#include "SessionState.hpp"
namespace th15 {
enum class PauseEntrance {Pause,RetryPause,GameOver,Results};
struct PauseActivationServices {
 virtual ~PauseActivationServices()=default;
 virtual bool record_elapsed_play_time()=0;
 virtual bool menu_visual(u32& handle,i32 script,i32 interrupt)=0;
 virtual bool reset_audio_slots()=0;virtual bool sound(i32)=0;
 virtual bool suspend_music()=0;virtual bool finish_audio_requests()=0;
 virtual bool capture_background(u32& handle,bool playfield)=0;
 virtual bool freeze_dialogue()=0;virtual bool freeze_chapter_result()=0;
 virtual bool preserve_current_music(std::string&,double&)=0;
 virtual bool start_game_over_music()=0;
 virtual bool replay_destination(bool selection)=0;
};
// The four original pause/result entrances differ in audio and capture order.
// Rendering, asynchronous audio completion and file time accounting are
// explicit device/application services, never original device pointers.
class PauseActivation {
 PauseState& state;SessionState& progress;PlayerLifeSession& player;i32& frame_skip;PauseActivationServices& host;
 bool check(bool,const char*);bool save_presentation(i32);bool freeze_overlays();
public:
 std::string error;
 PauseActivation(PauseState& s,SessionState& p,PlayerLifeSession& v,i32& skip,PauseActivationServices& h):state(s),progress(p),player(v),frame_skip(skip),host(h){}
 bool open(PauseEntrance,i32 front_bank,bool return_to_selection=false);
};
}
