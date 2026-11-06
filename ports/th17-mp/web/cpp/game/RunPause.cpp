#include "RunPause.hpp"
namespace th17 {
struct RunPause::Services final:PauseActivationServices,PauseRestorationServices,PauseMenuServices {
 RunPause& owner;explicit Services(RunPause& p):owner(p){}
 bool record_elapsed_play_time()override{return owner.time.account(owner.platform.scene_destination());}
 bool restart_elapsed_play_time()override{return owner.time.rebase();}
 bool menu_visual(u32& h,i32 script,i32 label)override{return owner.animations.retire(h)&&(h=owner.animations.create_overlay(owner.front,script))&&owner.animations.interrupt(h,label);}
 bool reset_audio_slots()override{return owner.platform.reset_audio_slots();}
 bool sound(i32 id)override{return owner.platform.sound(id);}
 bool suspend_music()override{return owner.platform.suspend_music();}
 bool finish_audio_requests()override{return owner.platform.finish_audio_requests();}
 bool capture_background(u32& h,bool field)override{return owner.platform.capture_background(owner.scene,owner.animations,h,field);}
 bool freeze_dialogue()override{return owner.gameplay.dialogue_paused(true);}
 bool freeze_chapter_result()override{return owner.scene.battle.hud&&owner.animations.pause(owner.scene.battle.hud->chapter_result_notice,true);}
 bool resume_dialogue()override{return owner.gameplay.resume_dialogue();}
 bool resume_chapter_result()override{return owner.gameplay.resume_chapter_result();}
 bool preserve_current_music(std::string& wave,double& position)override{return owner.platform.preserve_current_music(wave,position);}
 bool start_game_over_music()override{return owner.platform.start_game_over_music();}
 bool replay_destination(bool selection)override{return owner.platform.destination(selection?PauseDestination::TitleSelection:PauseDestination::Title);}
 bool interrupt_pause_visual(u32 h,i32 label)override{return owner.animations.interrupt(h,label);}
 bool capture_score_details(PauseScoreDetails& value)override{return owner.platform.capture_score_details(value);}
 bool synchronize_result_score()override{owner.scene.battle.hud->displayed_score=owner.scene.battle.score.score;if(u32(owner.progress.scoring.high_score)<u32(owner.scene.battle.score.score))owner.progress.scoring.high_score=owner.scene.battle.score.score;return true;}
 bool read_replay_slot(i32 slot,std::shared_ptr<Replay>& r)override{return owner.platform.read_replay_slot(slot,r);}
 bool save_named_replay(i32 slot,const std::array<char,9>& name)override{return owner.platform.save_named_replay(slot,name);}
 bool prepare_replay_save(bool complete)override{return owner.platform.prepare_replay_save(complete);}
 bool open_options(float offset)override{return owner.platform.open_options(offset);}
 bool options_finished()const override{return owner.platform.options_finished();}
 bool close_options()override{return owner.platform.close_options();}
 bool finish_pause_selection()override{return owner.actions.execute(owner.selection);}
};
RunPause::RunPause(StageGameplay& s,GameRun& p,RecordStore& r,AnmManager& a,RunPausePlatform& h,i32& skip,i32 bank):services(std::make_unique<Services>(*this)),scene(s),progress(p),animations(a),platform(h),front(bank),gameplay(s,a),time(r,p,h),activation(state,p,s.battle.session,skip,*services),restoration(state,p,skip,*services),actions(state,p,s.battle.session,s.battle.score,a,restoration,gameplay,h,skip),menu(state,p,s.battle.session,s.battle.score,r,a,*services),visuals(state,p,s.battle.session,r,a,menu.replays){}
RunPause::~RunPause(){disable();}
bool RunPause::fail(const std::string& why){if(error.empty())error=why.empty()?"Run pause unavailable":why;return false;}
bool RunPause::prepare(){if(!error.empty())return false;if(!scene.ready()||!scene.controller||!scene.battle.hud||!animations.resource(front))return fail("Run pause stage resources unavailable");if(frame)return true;if(!time.rebase())return fail(time.error);frame=std::make_unique<PauseFrame>(state,progress,scene.battle.session,scene.controller->age,activation,menu,scene.frame_scheduler(),front);return frame->error.empty()||fail(frame->error);}
bool RunPause::open(PauseEntrance entrance,bool return_selection){selection=return_selection;if(!error.empty()||!frame)return false;return activation.open(entrance,front,selection)||fail(activation.error);}
}
