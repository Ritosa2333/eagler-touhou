#include "TitleScene.hpp"
namespace th15 {
TitleScene::TitleScene(SessionState& p,PlayerLifeSession& v,ItemScoreState& points,RecordStore& r,TitleSelectionSettings& choice,TitleAudioSettings& audio,TitleControllerSettings& bindings,const MusicComments& comments,AnmManager& a,TitleScenePlatform& h,FrameScheduler& schedule,TitleSceneResources bank):progress(p),player(v),records(r),animations(a),host(h),scheduler(schedule),resources(bank),main(state,p,v,choice,a,bank.title,bank.portrait),mode(state,p,v,choice,a,bank.title,bank.ascii),difficulty(state,p,v,choice,a,bank.title,bank.ascii),character(state,p,v,choice,a,h,bank.title,bank.ascii),practice(state,p,choice,a,h,bank.title,bank.ascii),resume(state,p,v,a,h,bank.title,bank.ascii),options(state,audio,a,h,bank.title),controller(state,a,bindings,h,bank.title),music_room(state,a,comments,h,bank.title,bank.ascii,bank.text),replay_menu(state,p,v,a,catalog,h,saved_replay_selection,bank.title,bank.ascii),practice_draw(state,p,r),results(state,p,v,points,r,a,h,bank.title,bank.ascii),replay_save(state,p,r,a,results,h,bank.title,bank.ascii),player_data(state,r,a,h,bank.title,bank.ascii,bank.text),frame(state,p,v,r,a,*this,schedule,bank.title,bank.ascii){
 state.screen=TitleScreen::Initialize;state.flags=0;main.play_sound=mode.play_sound=difficulty.play_sound=[this](i32 id){return host.sound(id);};manual_callback.owner=this;manual_callback.run=[](void* p){return static_cast<TitleScene*>(p)->manual_step()?i32(FrameAction::Continue):i32(FrameAction::Error);};
}
TitleScene::~TitleScene(){close_manual();for(auto& handle:state.handles)animations.retire(handle);animations.retire(state.portrait);for(const i32 bank:{resources.title,resources.portrait,resources.help})animations.retire_resource(bank);}
bool TitleScene::check(bool ok,const std::string& why){if(!ok&&error.empty())error=why.empty()?"Title scene operation failed":why;return ok;}
void TitleScene::controls(const TitleFrameInput& value,u32 pad,i32 chapter)noexcept{input=value;gamepad_buttons=pad;numbered_chapter=chapter;frame.controls(value);}
bool TitleScene::start_manual(){if(manual)return true;manual=std::make_unique<Manual>(animations,host,resources.help);manual_menu=std::make_unique<TitleManualMenu>(state,animations,*manual,resources.title,resources.ascii);manual_callback.run=[](void* p){return static_cast<TitleScene*>(p)->manual_step()?i32(FrameAction::Continue):i32(FrameAction::Error);};manual_callback.enabled=true;return check(scheduler.add(manual_callback,FramePass::Update,11)>=0,"Title manual callback registration failed");}
void TitleScene::close_manual(){scheduler.remove(manual_callback);manual_menu.reset();manual.reset();}
bool TitleScene::manual_step(){return !manual||check(manual->update(input.pressed,input.repeated,progress.rate),manual->error);}
bool TitleScene::manual_page_ready(){return manual&&check(manual->page_loaded(),manual->error);}
bool TitleScene::release_transient_animations(){return host.release_transient_animations();}bool TitleScene::clear_title_overlay(){return host.clear_title_overlay();}
bool TitleScene::read_demo(i32 i,std::shared_ptr<Replay>& out){return host.read_demo(i,out);}bool TitleScene::begin_demo(const ReplayStartRequest& value){return host.begin_demo(value);}
bool TitleScene::queue_title_music(i32 code,i32 value,const std::string& name){return host.queue_title_music(code,value,name);}bool TitleScene::clear_current_wave(){return host.clear_current_wave();}
bool TitleScene::reset_replay_selection(){return host.reset_replay_selection();}bool TitleScene::return_practice_transition(){return host.return_practice_transition();}
bool TitleScene::title_exit(i32 value){return host.title_exit(value);}bool TitleScene::fade_out_title_music(){return host.fade_out_title_music();}
bool TitleScene::update_title_menu(TitleScreen screen,const TitleFrameInput& value){
 input=value;switch(screen){
 case TitleScreen::Main:return check(main.update(value.pressed,value.repeated,records.title_records().extra_available()),main.error);
 case TitleScreen::Options:return check(options.update(value.pressed,value.repeated),options.error);
 case TitleScreen::Controller:return check(controller.update(value.pressed,value.repeated,gamepad_buttons),controller.error);
 case TitleScreen::Mode:return check(mode.update(value.pressed,value.repeated),mode.error);
 case TitleScreen::Difficulty:return check(difficulty.update(value.pressed,value.repeated,records.title_records().all_characters(player.mode_flags)),difficulty.error);
 case TitleScreen::Character:return check(character.update(value.pressed,value.repeated,records.title_records()),character.error);
 case TitleScreen::Stage:return check(practice.update(value.pressed,value.repeated,numbered_chapter,records.title_records()),practice.error);
 case TitleScreen::ContinuePrompt:return check(resume.update(value.pressed,value.repeated),resume.error);
 case TitleScreen::PlayerData:return check(player_data.update(value.pressed,value.repeated),player_data.error);
 case TitleScreen::Replay:return check(replay_menu.update(value.pressed,value.repeated),replay_menu.error);
 case TitleScreen::MusicRoom:
  if(state.substate==0){const u32 mask=records.music_mask();for(u32 i=0;i<20;i++)music_room.unlocked[i]=(mask>>i)&1;music_room.alternate_audio=frame.alternate_audio;}
  return check(music_room.update(value.pressed,value.repeated),music_room.error);
 case TitleScreen::Records:return check(results.update(value.pressed,value.repeated),results.error);
 case TitleScreen::ReplaySave:return check(replay_save.update(value.pressed,value.repeated),replay_save.error);
 case TitleScreen::Manual:
  if(!start_manual()||!check(manual_menu->update(),manual_menu->error))return false;if(state.screen!=TitleScreen::Manual)close_manual();return true;
 default:return check(false,"Unsupported title scene menu");
 }
}
bool TitleScene::draw_title_menu(TitleScreen screen){switch(screen){
 case TitleScreen::Stage:return check(practice_draw.draw(host),practice_draw.error);
 case TitleScreen::PlayerData:return check(player_data.draw(host,host),player_data.error);
 case TitleScreen::Replay:return check(replay_menu.draw(host,host),replay_menu.error);
 case TitleScreen::Records:return check(results.draw(host,host),results.error);
 case TitleScreen::ReplaySave:return check(replay_save.draw(host,host,live?&live->description():nullptr),replay_save.error);
 default:return true;
}}
}
