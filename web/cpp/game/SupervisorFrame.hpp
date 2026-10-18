#pragma once
#include "AnmRenderer.hpp"
#include "InputController.hpp"
#include "Chain.hpp"
namespace th08 {
enum class Scene:i32 {Exit=-1,Init=0,Title=1,Game=2,Reinitialize=3,Error=4,Results=5,GameResults=6,FinishReplay=7,Music=8,Ending=9,Restart=10,SpellRestart=11,NextStage=12};
struct SupervisorFrameState {
    i32 active=0,target=0,previous=0,calculations=0;
    i32 background_status=0,background_wait=0;
    bool close_requested=false,background_running=false,keep_resources=false,sticky_input=false,practice=false;
    i32 difficulty=0,stage=0,screen_effect_counter=0;
    u32 fog=0xff;
};
struct SupervisorActions {
    virtual ~SupervisorActions()=default;
    virtual void animate_loading()=0;
    virtual bool service_animations()=0;
    virtual void update_audio_fades()=0;
    virtual u16 poll_input()=0;
    virtual void discard_graphics()=0;
    virtual bool title(bool replay_finished)=0;
    virtual bool game()=0;
    virtual bool results(bool from_game)=0;
    virtual bool music()=0;
    virtual bool ending()=0;
    virtual void cut_game()=0;
    virtual void save_replay()=0;
    virtual void advance_stage()=0;
    virtual bool version_valid()=0;
};
class SupervisorFrame {
public:
    SupervisorFrame(AnmRenderer& renderer,SupervisorActions& actions):renderer(renderer),actions(actions){}
    SupervisorFrameState state;
    InputFrame input;
    JobResult update();
private:
    AnmRenderer& renderer;SupervisorActions& actions;
    JobResult transition();
    JobResult title(bool replay_finished=false);
};
}
