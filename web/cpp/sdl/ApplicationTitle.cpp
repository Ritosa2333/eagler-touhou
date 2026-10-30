#include "ApplicationState.hpp"
#include <SDL3/SDL.h>
#include <ctime>
#include <cstdio>
namespace th17::sdl {
bool ApplicationState::sound(i32 id){return audio_device.effects.enqueue(id)||fail(audio_device.effects.error);}
bool ApplicationState::text(AnmVm& vm,const TextureText& value){return fonts.text(vm,value)||fail(fonts.error);}
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
bool ApplicationState::start_game(i32 stage){
 progress.stage=progress.initial_stage=stage;progress.character=selection.character;progress.animal=selection.animal;progress.difficulty=selection.difficulty;progress.spell_id=selection.spell_id;progress.scoring.mode_flags=selection.mode_flags;progress.practice_lives=selection.practice_checkpoint;progress.demo_slot=-1;selected_replay.reset();flow.title_return_reason=title?title->state.return_reason:1;flow.pending=AppScene::Game;return true;
}
i32 ApplicationState::checkpoint_key()const{for(i32 number=1;number<=9;number++)if(keyboard_state.keys[(keyboard_state.format==2?48:1)+number]&0x80)return number;return 0;}
bool ApplicationState::fade_music(float duration){return audio_device.queue_music(5,truncate_int(duration),"FadeOut")||fail(audio_device.error);}
bool ApplicationState::transition_size(float width,float height){auto* vm=animations.registry.find(transition_overlay);if(!vm)return fail("Title transition animation missing");vm->visual.sprite_size={width*2,height*2};return true;}
bool ApplicationState::request_catalog(){if(!title)return fail("Replay catalog has no title owner");if(!files.catalog(title->catalog))return fail(files.error);title->replay_catalog_ready();return true;}
bool ApplicationState::release_catalog(){if(title)title->catalog.clear();return true;}
bool ApplicationState::start_replay(const ReplayStartRequest& value){
 if(!files.replay(value.filename,selected_replay)||!selected_replay)return fail(files.error.empty()?"Selected replay unavailable":files.error);progress.stage=progress.initial_stage=value.stage;progress.character=value.character;progress.animal=value.animal;progress.difficulty=value.difficulty;progress.spell_id=value.spell;progress.scoring.mode_flags=(progress.scoring.mode_flags&~0x30u)|(value.spell_practice?0x20u:0);progress.demo_slot=-1;
 selection={progress.scoring.mode_flags,progress.spell_id,progress.difficulty,progress.character,progress.animal,progress.stage,0};flow.title_return_reason=2;flow.pending=AppScene::ReplayStart;return true;
}
bool ApplicationState::clear_page(){auto* bank=animations.resource(19);return bank&&graphics.clear_image(graphics.texture(*bank,1))||fail(graphics.error);}
bool ApplicationState::request_page(i32 page){char name[32];std::snprintf(name,sizeof name,"help_%.2d.png",page+1);if(!read(name,pending_png))return false;pending_page=page;return true;}
bool ApplicationState::upload_page(i32){auto* bank=animations.resource(19);return bank&&graphics.upload_png(graphics.texture(*bank,1),pending_png.data(),pending_png.size())||fail(graphics.error);}
bool ApplicationState::read_slot(i32 slot,std::shared_ptr<Replay>& value){return files.replay_slot(slot,value)||fail(files.error);}
bool ApplicationState::prepare_live_replay(bool cleared){if(!recording)return fail("No live recording to save");if(!completed_recording){recording->finish(i64(std::time(nullptr)),progress.stage,cleared);completed_recording=true;}live_description=recording->description();if(title)title->live=recording.get();return true;}
bool ApplicationState::save_slot(i32 slot,const std::array<char,9>& name){if(!recording)return fail("Replay save has no recording");ReplayCalendar date;if(!calendar(i64(std::time(nullptr)),date))return false;const i32 score=scene()?scene()->battle.score.score:selection_score.score;const ReplayExportDetails details{score,performance.requested?performance.delivered:1,performance.requested?performance.requested:1,date.year,date.month,date.day,date.hour,date.minute};return files.save_replay(slot,*recording,name,details)||fail(files.error);}
bool ApplicationState::release_live_replay(){if(title)title->live=nullptr;return true;}
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
