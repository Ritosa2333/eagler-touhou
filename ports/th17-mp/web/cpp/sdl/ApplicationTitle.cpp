#include "ApplicationState.hpp"
#include <SDL3/SDL.h>
#include <ctime>
#include <cstdio>
namespace th17::sdl {
bool ApplicationState::sound(i32 id){
#if TH17_MULTIPLAYER
 if(room_silent)return true;
#endif
 return audio_device.effects.enqueue(id)||fail(audio_device.effects.error);
}
bool ApplicationState::text(AnmVm& vm,const TextureText& value){if(!ending||value.utf8||!fonts.translated_message(value.bytes))return fonts.text(vm,value)||fail(fonts.error);
 auto translated=value;translated.utf8=true;
 return fonts.text(vm,translated)||fail(fonts.error);}
bool ApplicationState::hud_text(const HudTextDraw& value){return (display?display->hud_text(value):captions.hud_text(value))||fail(captions.error);}
bool ApplicationState::keyboard(TitleKeyboard& value){value=keyboard_state;return true;}
bool ApplicationState::calendar(i64 stamp,ReplayCalendar& value){const time_t time=stamp;const auto* tm=std::localtime(&time);if(!tm)return fail("Replay date outside calendar range");value={tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min};return true;}
bool ApplicationState::seconds(double& value){value=double(SDL_GetTicksNS())/1e9;return true;}
bool ApplicationState::capture_score_details(PauseScoreDetails& value){const auto stamp=i64(std::time(nullptr));value={{signed_bits(u32(stamp)),signed_bits(u32(u64(stamp)>>32))},performance.requested?performance.delivered:1,performance.requested?performance.requested:1};return true;}
bool ApplicationState::music(const std::string& name){return audio_device.queue_music(1,0,name.size()>=4&&name.substr(name.size()-4)==".wav"?name:name+".wav")||fail(audio_device.error);}
bool ApplicationState::music(const std::string& name,i32 track){return music(name)&&music_command(2)&&records.unlock_music(track);}
bool ApplicationState::music_command(i32 code){return audio_device.queue_music(code,0,"dummy")||fail(audio_device.error);}
bool ApplicationState::start_title_music(){return music_command(2)&&records.unlock_music(0);}
bool ApplicationState::volume(i32 bgm,i32 se,i32){audio_device.music_volume=bgm;audio_device.effects.master_volume=se;audio_device.refresh_volume();return true;}
bool ApplicationState::save(const TitleControllerSettings& value){controller=value;return save_settings();}
bool ApplicationState::fade_menu_music(){return music_command(5);}
bool ApplicationState::show_loading_indicator(){return loading_overlay();}
bool ApplicationState::begin_transition(u32& handle){
 if(!animations.retire(transition_overlay))return fail(animations.error);
 handle=animations.create(8,0,-1,0);transition_overlay=handle;auto* vm=animations.registry.find(handle);
 // Native effect descriptor 0 installs the four shutter panels and patterned
 // backdrop. The base ANM script alone is an invisible waiting controller.
 return vm&&animations.spawn_effect(*vm,0)||fail(animations.error.empty()?"Title transition owner unavailable":animations.error);
}
PracticeMenuResult ApplicationState::configure_practice(i32 stage,i32 spell,bool available){
 if(!practice.enabled||spell<-1||(spell>=0&&practice_phase_count(0,spell)==1))return PracticeMenuResult::Native;
 if(practice.confirmed){practice.confirmed=false;practice.accepted=true;return PracticeMenuResult::Confirm;}
 if(practice.cancelled){practice.cancelled=false;return PracticeMenuResult::Cancel;}
 if(practice.menu)return PracticeMenuResult::Configure;
 if(!available)practice.purple=true;
 auto& patch=practice.settings.patch;
 if(patch.stage!=stage||practice.spell!=spell){
  patch.section=patch.phase=0;patch.dialogue=patch.keep_drops=false;
  patch.stage=stage;
 }
 practice.spell=spell;
 practice.menu=true;
 return PracticeMenuResult::Configure;
}
bool ApplicationState::start_game(i32 stage){
 practice.active={};
 if(practice.accepted){
  practice.accepted=false;
  stage=selection.stage=practice.settings.patch.stage;
  if(practice.spell<0){
   selection.spell_id=-1;
   if(stage<=6)choices.practice_stage=stage-1;
  }
  if(practice.spell>=0||practice.purple)practice.active=practice.settings.patch;
 }
 progress.stage=progress.initial_stage=stage;progress.character=selection.character;progress.animal=selection.animal;progress.difficulty=stage==7?4:selection.difficulty;progress.spell_id=selection.spell_id;progress.scoring.mode_flags=selection.mode_flags;progress.practice_lives=selection.practice_checkpoint;progress.demo_slot=-1;selected_replay.reset();flow.title_return_reason=title?title->state.return_reason:1;flow.pending=AppScene::Game;return true;
}
#if TH17_MULTIPLAYER
bool ApplicationState::begin_room(u16 seed,i32 difficulty,u32 seats,const std::array<std::array<i32,2>,3>& loadouts,i32 initial_stage,bool challenge){
 if(!initialized||!title||room_seats||seats<2||seats>3||u32(difficulty)>4)return fail("TH17 room cannot start");
 room_seats=seats;room_loadouts=loadouts;room_challenge=challenge;
 Rng record_seed;record_seed.seed=seed;records.reset(record_seed);
 environment.game_rng.seed=visual.seed=seed;
 environment.game_rng.calls=visual.calls=0;
 selection={};
 selection.character=loadouts[0][0];selection.animal=loadouts[0][1];selection.difficulty=difficulty;
 return start_game(initial_stage)&&advance_flow();
}
#endif
i32 ApplicationState::checkpoint_key()const{for(i32 number=1;number<=9;number++)if(keyboard_state.keys[(keyboard_state.format==2?48:1)+number]&0x80)return number;return 0;}
bool ApplicationState::fade_music(float duration){return audio_device.queue_music(5,truncate_int(duration),"FadeOut")||fail(audio_device.error);}
bool ApplicationState::transition_size(float width,float height){auto* vm=animations.registry.find(transition_overlay);if(!vm)return fail("Title transition animation missing");vm->visual.sprite_size={width*2,height*2};return true;}
bool ApplicationState::request_catalog(){if(!title)return fail("Replay catalog has no title owner");if(!files.catalog(title->catalog))return fail(files.error);title->replay_catalog_ready();return true;}
bool ApplicationState::release_catalog(){if(title)title->catalog.clear();return true;}
bool ApplicationState::start_replay(const ReplayStartRequest& value){
 if(!files.replay(value.filename,selected_replay)||!selected_replay)return fail(files.error.empty()?"Selected replay unavailable":files.error);progress.stage=progress.initial_stage=value.stage;progress.character=value.character;progress.animal=value.animal;progress.difficulty=value.difficulty;progress.spell_id=value.spell;progress.scoring.mode_flags=(progress.scoring.mode_flags&~0x30u)|(value.spell_practice?0x20u:selected_replay->practice()?0x10u:0);progress.demo_slot=-1;
 selection={progress.scoring.mode_flags,progress.spell_id,progress.difficulty,progress.character,progress.animal,progress.stage,0};flow.title_return_reason=2;flow.pending=AppScene::ReplayStart;return true;
}
bool ApplicationState::clear_page(){auto* bank=animations.resource(19);return bank&&graphics.clear_image(graphics.texture(*bank,1))||fail(graphics.error);}
bool ApplicationState::request_page(i32 page){char name[32];std::snprintf(name,sizeof name,"help_%.2d.png",page+1);if(!read(name,pending_png))return false;pending_page=page;return true;}
bool ApplicationState::upload_page(i32){auto* bank=animations.resource(19);return bank&&graphics.upload_png(graphics.texture(*bank,1),pending_png.data(),pending_png.size())||fail(graphics.error);}
bool ApplicationState::read_slot(i32 slot,std::shared_ptr<Replay>& value){
#if TH17_MULTIPLAYER
 if(room_seats){value.reset();return true;}
#endif
 return files.replay_slot(slot,value)||fail(files.error);
}
bool ApplicationState::prepare_live_replay(bool cleared){
#if TH17_MULTIPLAYER
 if(room_seats){
  if(!room_replay_save)return fail("Multiplayer Replay owner is unavailable");
  if(!room_description)room_description=std::make_unique<ReplayRecording>();
  auto& metadata=room_description->description();
  const i64 now=std::time(nullptr);
  std::memcpy(metadata.data()+0x10,&now,8);
  std::memcpy(metadata.data()+0x88,&progress.character,4);
  std::memcpy(metadata.data()+0x8c,&progress.animal,4);
  std::memcpy(metadata.data()+0x90,&progress.difficulty,4);
  metadata[0x94]=u8(cleared?(progress.difficulty==4?9:8):progress.stage);
  live_description=metadata;
  if(title)title->live=room_description.get();
  completed_recording|=cleared;
  return true;
 }
#endif
 if(!recording)return fail("No live recording to save");
 if(!completed_recording){recording->finish(i64(std::time(nullptr)),progress.stage,cleared);completed_recording=true;}
 live_description=recording->description();if(title)title->live=recording.get();return true;
}
bool ApplicationState::save_slot(i32 slot,const std::array<char,9>& name){
#if TH17_MULTIPLAYER
 if(room_seats)return room_replay_save&&room_replay_save(slot,name,completed_recording);
#endif
 if(!recording)return fail("Replay save has no recording");
 ReplayCalendar date;if(!calendar(i64(std::time(nullptr)),date))return false;
 const i32 score=scene()?scene()->battle.score.score:selection_score.score;
 const ReplayExportDetails details{score,performance.requested?performance.delivered:1,performance.requested?performance.requested:1,date.year,date.month,date.day,date.hour,date.minute};
 return files.save_replay(slot,*recording,name,details)||fail(files.error);
}
bool ApplicationState::release_live_replay(){
 if(title)title->live=nullptr;
#if TH17_MULTIPLAYER
 if(room_seats)exiting=true;
#endif
 return true;
}
bool ApplicationState::release_transient_animations(){for(i32 bank=20;bank<24;bank++)animations.unload(bank);return true;}
bool ApplicationState::clear_title_overlay(){return animations.retire(transition_overlay)||fail(animations.error);}
bool ApplicationState::read_demo(i32 index,std::shared_ptr<Replay>& value){std::vector<u8> bytes;if(index<0||index>=3||!read("demo"+std::to_string(index+1)+".rpy",bytes))return fail("Original demonstration unavailable");value=std::make_shared<Replay>();return value->open(bytes.data(),bytes.size())||fail(value->error());}
bool ApplicationState::begin_demo(const DemoRequest& value){if(!read_demo(value.index,selected_replay))return false;progress.stage=progress.initial_stage=value.stage;progress.character=value.character;progress.animal=value.animal;progress.difficulty=value.difficulty;progress.spell_id=-1;progress.scoring.mode_flags=selection.mode_flags;progress.demo_slot=-1;flow.title_return_reason=1;flow.pending=AppScene::ReplayStart;return true;}
bool ApplicationState::queue_title_music(i32 code,i32 value,const std::string& name){return audio_device.queue_music(code,value,name)||fail(audio_device.error);}
bool ApplicationState::clear_current_wave(){audio_device.clear_current_wave();return true;}
bool ApplicationState::unlock_title_music(){return records.unlock_music(0);}
bool ApplicationState::selection_return_transition(){return screen_fade(30,20,83,false,true);}
bool ApplicationState::title_exit(i32 next){if(flow.pending==AppScene::Game||flow.pending==AppScene::ReplayStart)return true;flow.pending=AppScene(next);return true;}
bool ApplicationState::fade_out_title_music(){return music_command(5);}
bool ApplicationState::ending_active()const{return ending!=nullptr;}
bool ApplicationState::ending_complete()const{return ending&&ending->script.finished&&(ending->script.run.first_seen_flags&8);}
bool ApplicationState::retire_ending(){ending.reset();return true;}
bool ApplicationState::preview_ending(i32 index){if(ending)return fail("Ending preview already active");preview_index=index;return begin_ending();}
bool ApplicationState::trophy_stage(i32 character,i32 animal){selection.mode_flags&=~0x30u;selection.character=character;selection.animal=animal;selection.difficulty=4;selection.spell_id=-1;if(!start_game(7))return false;progress.demo_slot=1;flow.title_return_reason=6;return true;}
}
