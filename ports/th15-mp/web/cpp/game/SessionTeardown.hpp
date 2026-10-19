#pragma once
#include "SessionState.hpp"
namespace th15 {
enum class SessionObject:u32 {Replay,Checkpoint,Background,PreviousBackground,PauseMenu,Hud,Player,Bullets,Items,Lasers,Popups,Enemies,Bomb,Spell,Count};
struct SessionExitPresentation {i32 frame_skip=0;u32 clear_color=0;};
// Object identity is owned by the application. Normal stage completion keeps
// the run's managers; retry/selection/ending take the original separate paths.
struct SessionTeardownServices {
 virtual ~SessionTeardownServices()=default;
 virtual bool owns(SessionObject)const noexcept=0;
 virtual bool stop_checkpoint_worker()=0;virtual bool save_records()=0;
 virtual bool clear_session_links()=0;virtual bool reset_transition()=0;
 virtual bool release(SessionObject)=0;virtual bool clear_checkpoint_data()=0;
 virtual bool reset_gui_for_stage()=0;virtual bool carry_background()=0;
 virtual bool disable_callbacks(SessionObject)=0;virtual bool reset_items()=0;
 virtual bool clear_enemies()=0;virtual bool retire_stage_animations()=0;
 virtual bool detach_scene_callbacks()=0;virtual bool queue_music(i32 mode)=0;
 virtual bool reset_audio_slots()=0;
};
class SessionTeardown {
 SessionState& progress;PlayerLifeSession& player;SessionExitPresentation& presentation;SessionTeardownServices& host;
 bool check(bool,const char*);bool release(SessionObject);
public:
 std::string error;
 SessionTeardown(SessionState& p,PlayerLifeSession& s,SessionExitPresentation& v,SessionTeardownServices& h):progress(p),player(s),presentation(v),host(h){}
 bool finish(i32 destination,bool restart_audio);
};
}
