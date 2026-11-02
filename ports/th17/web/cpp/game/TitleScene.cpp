#include "TitleScene.hpp"
namespace th17 {
TitleScene::TitleScene(GameRun& p,GameSelection& selection,PlayerLifeSession& player,ItemScoreState& points,RecordStore& r,TitleSelectionSettings& choice,TitleAudioSettings& audio,TitleControllerSettings& bindings,const MusicComments& comments,const TrophyCatalog& achievements,TitleSpellMemory& spell_memory,AnmManager& a,TitleScenePlatform& h,FrameScheduler& schedule,TitleSceneResources bank):progress(p),selection(selection),records(r),animations(a),host(h),scheduler(schedule),resources(bank),main(state,selection,choice,a,bank.title,bank.portrait),difficulty(state,selection,choice,a),character(state,selection,choice,a),animal(state,selection,choice,a,h),practice(state,selection,choice,a,h),options(state,audio,a,h,bank.title),controller(state,a,bindings,h,bank.title),music_room(state,a,comments,h,bank.title,bank.ascii,bank.text),replay_menu(state,p,player,a,catalog,h,saved_replay_selection,bank.title,bank.ascii),results(state,p,player,points,r,a,h,bank.title,bank.ascii),replay_save(state,p,r,a,results,h,bank.title,bank.ascii),player_data(state,r,a,h,bank.title,bank.ascii,bank.text),spell_practice(state,selection,choice,r,a,h,spell_memory,bank.title,bank.ascii),trophies(state,r,achievements,a,h,bank.title,bank.ascii),frame(state,selection,a,*this,bank.title,bank.ascii){
 state.screen=TitleScreen::Initialize;state.flags=0;main.play_sound=difficulty.play_sound=character.play_sound=[this](i32 id){return host.sound(id);};manual_callback.owner=this;manual_callback.run=[](void* p){return static_cast<TitleScene*>(p)->manual_step()?i32(FrameAction::Continue):i32(FrameAction::Error);};
}
TitleScene::~TitleScene(){close_manual();for(auto& handle:state.handles)animations.retire(handle);animations.retire(state.portrait);for(i32 bank:{resources.title,resources.portrait,resources.help})animations.retire_resource(bank);}
bool TitleScene::check(bool ok,const std::string& why){if(!ok&&error.empty())error=why.empty()?"Title scene operation failed":why;return ok;}
bool TitleScene::start_manual(){if(manual)return true;manual=std::make_unique<Manual>(animations,host,resources.help);manual_menu=std::make_unique<TitleManualMenu>(state,animations,*manual,resources.title,resources.ascii);manual_callback.enabled=true;return check(scheduler.add(manual_callback,FramePass::Update,11)>=0,"Title manual callback registration failed");}
void TitleScene::close_manual(){scheduler.remove(manual_callback);manual_menu.reset();manual.reset();}
bool TitleScene::manual_step(){return !manual||check(manual->update(input.pressed,input.repeated,input.rate),manual->error);}
bool TitleScene::manual_page_ready(){return manual&&check(manual->page_loaded(),manual->error);}
bool TitleScene::release_transient_animations(){return host.release_transient_animations();}bool TitleScene::clear_title_overlay(){return host.clear_title_overlay();}
bool TitleScene::read_demo(i32 i,std::shared_ptr<Replay>& out){return host.read_demo(i,out);}bool TitleScene::begin_demo(const DemoRequest& value){return host.begin_demo(value);}
bool TitleScene::queue_title_music(i32 code,i32 value,const std::string& name){return host.queue_title_music(code,value,name);}bool TitleScene::clear_current_wave(){return host.clear_current_wave();}
bool TitleScene::unlock_title_music(){return host.unlock_title_music();}bool TitleScene::selection_return_transition(){return host.selection_return_transition();}
bool TitleScene::title_exit(i32 value){return host.title_exit(value);}bool TitleScene::fade_out_title_music(){return host.fade_out_title_music();}
bool TitleScene::ending_active()const{return host.ending_active();}bool TitleScene::ending_complete()const{return host.ending_complete();}bool TitleScene::retire_ending(){return host.retire_ending();}
bool TitleScene::update_title_menu(TitleScreen screen,const TitleFrameInput& value){
 input=value;switch(screen){
 case TitleScreen::Main:{bool extra=false;for(i32 c=0;c<3;c++)extra|=records.game.title.extra(c);return check(main.update(value.pressed,value.repeated,extra),main.error);}
 case TitleScreen::Options:return check(options.update(value.pressed,value.repeated),options.error);
 case TitleScreen::Controller:return check(controller.update(value.pressed,value.repeated,gamepad_buttons),controller.error);
 case TitleScreen::Difficulty:return check(difficulty.update(value.pressed,value.repeated,records.game.title),difficulty.error);
 case TitleScreen::Character:return check(character.update(value.pressed,value.repeated,records.game.title),character.error);
 case TitleScreen::Animal:return check(animal.update(value.pressed,value.repeated,records.game.title),animal.error);
 case TitleScreen::Stage:return check(practice.update(value.pressed,value.repeated,records.game.title),practice.error);
 case TitleScreen::PlayerData:case TitleScreen::PlayerDataDetails:return check(player_data.update(value.pressed,value.repeated,selection.difficulty),player_data.error);
 case TitleScreen::Replay:return check(replay_menu.update(value.pressed,value.repeated),replay_menu.error);
 case TitleScreen::MusicRoom:
  if(state.substate==0){for(u32 i=0;i<18;i++)music_room.unlocked[i]=records.music[i]!=0;music_room.alternate_audio=frame.alternate_audio;}
  return check(music_room.update(value.pressed,value.repeated),music_room.error);
 case TitleScreen::Ranking:return check(results.update(value.pressed,value.repeated),results.error);
 case TitleScreen::RankingName:return check(replay_save.update(value.pressed,value.repeated),replay_save.error);
 case TitleScreen::SpellPractice:case TitleScreen::SpellGroup:case TitleScreen::SpellRank:return check(spell_practice.update(value.pressed,value.repeated),spell_practice.error);
 case TitleScreen::Trophies:return check(trophies.update(value.pressed,value.repeated),trophies.error);
 case TitleScreen::Manual:if(!start_manual()||!check(manual_menu->update(),manual_menu->error))return false;if(state.screen!=TitleScreen::Manual)close_manual();return true;
 default:return check(false,"Unsupported title scene menu");
 }
}
bool TitleScene::draw_title_menu(TitleScreen screen){switch(screen){
 case TitleScreen::Stage:return check(practice.draw(records.game,host),practice.error);
 case TitleScreen::PlayerDataDetails:return check(player_data.draw(host,host),player_data.error);
 case TitleScreen::Replay:return check(replay_menu.draw(host,host),replay_menu.error);
 case TitleScreen::Ranking:return check(results.draw(host,host),results.error);
 case TitleScreen::RankingName:return check(replay_save.draw(host,host,live?&live->description():nullptr),replay_save.error);
 case TitleScreen::SpellGroup:case TitleScreen::SpellRank:return check(spell_practice.draw(host),spell_practice.error);
 case TitleScreen::Trophies:return check(trophies.draw(host),trophies.error);
 default:return true;
}}
}
