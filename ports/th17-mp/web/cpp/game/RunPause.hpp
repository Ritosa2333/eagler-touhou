#pragma once
#include "PauseFrame.hpp"
#include "StagePauseActions.hpp"
#include "RunPlayTime.hpp"
#include "PauseDraw.hpp"
namespace th17 {
struct RunPausePlatform:RunClock,PauseActionServices,PauseScoreServices {
 virtual bool sound(i32)=0;virtual bool suspend_music()=0;virtual bool finish_audio_requests()=0;
 virtual bool capture_background(StageGameplay&,AnmManager&,u32&,bool playfield)=0;
 virtual bool preserve_current_music(std::string&,double&)=0;virtual bool start_game_over_music()=0;
 virtual bool read_replay_slot(i32,std::shared_ptr<Replay>&)=0;
 virtual bool save_named_replay(i32,const std::array<char,9>&)=0;virtual bool prepare_replay_save(bool)=0;
 virtual bool open_options(float)=0;virtual bool options_finished()const=0;virtual bool close_options()=0;
 virtual i32 scene_destination()const=0;
};
// One pause owner per live stage. The application routes game-over and result
// requests here, and supplies persistent files, audio and GPU captures.
class RunPause {
 struct Services;std::unique_ptr<Services> services;
 StageGameplay& scene;GameRun& progress;AnmManager& animations;RunPausePlatform& platform;i32 front;bool selection=false;
 bool fail(const std::string&);
public:
 PauseState state;StagePauseActions gameplay;RunPlayTime time;PauseActivation activation;PauseRestoration restoration;PauseActions actions;PauseMenu menu;std::unique_ptr<PauseFrame> frame;PauseDraw visuals;std::string error;
 RunPause(StageGameplay&,GameRun&,RecordStore&,AnmManager&,RunPausePlatform&,i32& frame_skip,i32 front_bank);
 ~RunPause();bool prepare();bool open(PauseEntrance,bool return_to_selection=false);
 void controls(const PauseFrameInput& input,bool return_to_selection=false)noexcept{selection=return_to_selection;if(frame)frame->controls(input);}
 void disable()noexcept{if(frame)frame->enable(false);}
};
}
