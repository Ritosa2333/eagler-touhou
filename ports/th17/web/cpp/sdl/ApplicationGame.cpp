#include "ApplicationState.hpp"
namespace th17::sdl {
bool ApplicationState::audio(i32 id,float pan,PlayerShots::SoundAction action,bool queued){if(action==PlayerShots::SoundAction::stop)return audio_device.effects.stop(id);if(action==PlayerShots::SoundAction::pan){audio_device.sound_pan(u32(id),truncate_int(float(float(pan*1000.f)/192.f)));return true;}return (queued?audio_device.effects.enqueue(id,truncate_int(float(float(pan*1000.f)/192.f))):audio_device.effects.positioned(id,pan))||fail(audio_device.effects.error);}
bool ApplicationState::popup(const Vec3&,i32,u32){return true;}bool ApplicationState::trophy(i32 id){return records.trophy(id)||fail(records.error);}
bool ApplicationState::game_over(){return pause&&pause->open(PauseEntrance::GameOver)||fail(pause?pause->error:"Game-over menu unavailable");}
bool ApplicationState::finish_practice(){return pause&&pause->open(PauseEntrance::Results)||fail(pause?pause->error:"Practice result menu unavailable");}
bool ApplicationState::record_play_time(){return pause&&pause->time.account(i32(flow.current))||fail(pause?pause->time.error:"Run play-time owner unavailable");}
bool ApplicationState::return_to_title(){if(progress.practice_record)return replay_finished();flow.pending=AppScene::Title;return true;}
bool ApplicationState::dialogue_text(AnmVm& vm,u32 color,u32 shadow,i32 style,i32 x,i32 spacing,const std::string& bytes){return fonts.dialogue(vm,color,shadow,style,x,spacing,bytes)||fail(fonts.error);}
bool ApplicationState::spell_title(AnmVm& vm,const std::string& bytes){return fonts.spell_title(vm,bytes)||fail(fonts.error);}
bool ApplicationState::music_control(i32 operation,i32 value){return (operation==0?audio_device.music(value):audio_device.queue_music(operation,value))||fail(audio_device.error);}
bool ApplicationState::music_fade(i32 duration){return audio_device.queue_music(5,duration)||fail(audio_device.error);}
bool ApplicationState::unlock_music(i32 index){return records.unlock_music(index)||fail(records.error);}
bool ApplicationState::screen_fade(i32 duration,i32 update,i32 draw,bool covering,bool full,u32 color){auto value=std::make_unique<ScreenFade>(active_scheduler(),renderer,graphics,environment,duration,update,draw,color,covering,full);value->context=[this](){return ScreenFadeContext{exiting,scene()!=nullptr,progress.scene_flags,progress.rate};};fades.push_back(std::move(value));return true;}
bool ApplicationState::transition(const SceneTransition& value){if(value.mode!=2&&value.mode!=5)return fail("Unexpected game transition effect");return screen_fade(value.frames,20,value.layer,true,value.mode==5,value.color&0xffffff);}
i32 ApplicationState::loading_state()const noexcept{return loading_handle?2:0;}
bool ApplicationState::retire_loading(){return end_loading_overlay();}
bool ApplicationState::flush_audio(){return audio_device.finish_music_requests()||fail(audio_device.error);}
bool ApplicationState::resume_environment(double value){return audio_device.seek_music(value)||fail(audio_device.error);}
bool ApplicationState::stage_input_ready(StageGameplay& value){return run_input&&scene()==&value&&run_input->activate()||fail(run_input?run_input->error:"Stage input owner unavailable");}
bool ApplicationState::replay_finished(){return pause&&pause->open(PauseEntrance::RetryPause)||fail(pause?pause->error:"Replay completion menu unavailable");}
bool ApplicationState::suspend_music(){audio_device.pause_music(true);return true;}
bool ApplicationState::finish_audio_requests(){return flush_audio();}
bool ApplicationState::capture_background(StageGameplay&,AnmManager&,u32& handle,bool field){return display&&display->capture(handle,!field)||fail(display?display->error:"Capture requested without a game display");}
bool ApplicationState::preserve_current_music(std::string& name,double& position){return audio_device.current_music(name,position)||fail(audio_device.error);}
bool ApplicationState::start_game_over_music(){return music("th128_08.wav")&&music_command(2);}
bool ApplicationState::read_replay_slot(i32 slot,std::shared_ptr<Replay>& value){return read_slot(slot,value);}
bool ApplicationState::save_named_replay(i32 slot,const std::array<char,9>& name){return save_slot(slot,name);}
bool ApplicationState::prepare_replay_save(bool cleared){return prepare_live_replay(cleared);}
bool ApplicationState::open_options(float offset){
 close_options();if(!scene())return fail("Pause manual requires a run");pause_manual=std::make_unique<Manual>(animations,*this,19);pause_manual->offset_x=offset;manual_callback.owner=this;manual_callback.enabled=true;manual_callback.run=[](void* p)->i32{auto& app=*static_cast<ApplicationState*>(p);if(!app.pause_manual||app.pause_manual->update(app.manual_pressed,app.manual_repeated,app.progress.rate))return 1;app.fail(app.pause_manual->error);return 5;};return scene()->frame_scheduler().add(manual_callback,FramePass::Update,11)>=0||fail("Pause manual callback registration failed");
}
bool ApplicationState::options_finished()const{return pause_manual&&pause_manual->finished;}
bool ApplicationState::close_options(){if(scene())scene()->frame_scheduler().remove(manual_callback);if(pause_manual){for(auto handle:pause_manual->choices)animations.retire(handle);animations.retire(pause_manual->page);pause_manual.reset();pending_page=-1;}return true;}
i32 ApplicationState::scene_destination()const{return i32(flow.current);}
bool ApplicationState::resume_looping_sounds(){audio_device.effects.resume();return true;}
bool ApplicationState::resume_music(){audio_device.pause_music(false);return true;}
bool ApplicationState::resume_saved_music(const std::string& name,double position){if(name.empty())return true;return audio_device.music_file(name)&&audio_device.seek_music(position)||fail(audio_device.error);}
bool ApplicationState::reset_audio_slots(){audio_device.effects.suspend();return true;}
bool ApplicationState::destination(PauseDestination value){if(value==PauseDestination::TitleSelection){flow.title_return_reason=4;flow.pending=AppScene::Title;}else{if(value==PauseDestination::Title)flow.title_return_reason=progress.practice_record?2:flow.title_return_reason;flow.pending=AppScene(i32(value));}return true;}
bool ApplicationState::background_fade(i32 duration,i32 update,i32 draw){return screen_fade(duration,update,draw,false,false);}
}
