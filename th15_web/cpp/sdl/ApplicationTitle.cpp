#include "ApplicationState.hpp"
#include <SDL3/SDL.h>
#include <ctime>
#include <cstdio>
namespace th15::sdl {
bool ApplicationState::sound(i32 id){return audio_device.effects.enqueue(id)||fail(audio_device.effects.error);}
bool ApplicationState::text(AnmVm& vm,const DialogueText& value){return fonts.text(vm,value)||fail(fonts.error);}
bool ApplicationState::hud_text(const HudTextDraw& value){return captions.enqueue(value,0,value.style.shadow)||fail(captions.error);}
bool ApplicationState::keyboard(TitleKeyboard& value){value=keyboard_state;return true;}
bool ApplicationState::calendar(i64 stamp,ReplayCalendar& value){const time_t time=stamp;const auto* tm=std::localtime(&time);if(!tm)return fail("Replay timestamp outside calendar range");value={tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min};return true;}
bool ApplicationState::seconds(double& value){value=double(SDL_GetTicksNS())/1e9;return true;}
bool ApplicationState::capture_score_details(PauseScoreDetails& value){const auto time=i64(std::time(nullptr));value={{signed_bits(u32(time)),signed_bits(u32(u64(time)>>32))},double(frames),double(frames)};return true;}
bool ApplicationState::music(const std::string& name){return audio_device.queue_music(1,0,name.size()>=4&&name.compare(name.size()-4,4,".wav")==0?name:name+".wav")||fail(audio_device.error);}
bool ApplicationState::music(const std::string& name,i32 track){return music(name)&&music_command(2)&&records.unlock_music(track);}
bool ApplicationState::music_command(i32 code){return audio_device.queue_music(code,0,"dummy")||fail(audio_device.error);}
bool ApplicationState::start_title_music(){return music_command(2)&&records.unlock_music(0);}
bool ApplicationState::volume(i32 music,i32 sound,i32){audio_device.music_volume=music;audio_device.effects.master_volume=sound;audio_device.refresh_volume();return true;}
bool ApplicationState::save(const TitleControllerSettings& value){controller=value;return save_settings();}
bool ApplicationState::checkpoint_available(i32 character,i32 difficulty,bool& value){CheckpointHeader header;bool present=false;if(!files.checkpoint_header(character,difficulty,header,present))return fail(files.error);value=present&&header.compatible(character,difficulty,0)&&header.stage()>=1&&header.stage()<=7;return true;}
bool ApplicationState::checkpoint_stage(i32 character,i32 difficulty,i32& stage){CheckpointHeader header;bool present=false;if(!files.checkpoint_header(character,difficulty,header,present))return fail(files.error);if(!present||!header.compatible(character,difficulty,0)||header.stage()<1||header.stage()>7)return fail("Selected Pointdevice checkpoint is unavailable");stage=header.stage();return true;}
bool ApplicationState::reset_resume_selection(){progress.new_run=true;return true;}
bool ApplicationState::prepare_game_music(){return music_command(5);}
bool ApplicationState::begin_transition(u32& value){value=animations.create(8,0,-1,0);transition_overlay=value;auto* vm=animations.registry.find(value);return vm&&(vm->geometry.overlay||scene_effects&&scene_effects->configure(*vm,0))||fail(animations.error);}
bool ApplicationState::start_game(i32 stage){selected_stage=stage;selected_replay.clear();progress.replay=false;progress.new_run=true;pending_destination=13;return true;}
bool ApplicationState::fade_music(float duration){return audio_device.queue_music(5,truncate_int(duration),"FadeOut")||fail(audio_device.error);}
bool ApplicationState::transition_size(float width,float height){auto* vm=animations.registry.find(transition_overlay);if(!vm)return fail("Title transition animation unavailable");vm->visual.sprite_size={width*2,height*2};return true;}
bool ApplicationState::request_catalog(){if(!title)return fail("Replay catalog has no title owner");if(!files.catalog(title->catalog))return fail(files.error);title->replay_catalog_ready();return true;}
bool ApplicationState::release_catalog(){if(title)title->catalog.clear();return true;}
bool ApplicationState::start_replay(const ReplayStartRequest& value){
 const auto at=value.filename.find_last_of("/\\");const auto name=value.filename.substr(at==std::string::npos?0:at+1);if(name.empty()||name=="."||name=="..")return fail("Invalid replay filename");
 size_t size=0;auto* bytes=static_cast<u8*>(SDL_LoadFile(("/save/replay/"+name).c_str(),&size));if(!bytes)return fail("Selected replay file unavailable");selected_replay.assign(bytes,bytes+size);SDL_free(bytes);Replay validate;if(!validate.open(selected_replay.data(),selected_replay.size()))return fail(validate.error());
 selected_stage=value.stage;progress.stage=value.stage;progress.character=value.character;progress.subcharacter=value.subcharacter;progress.difficulty=value.difficulty;progress.spell_id=value.spell;progress.replay=true;progress.new_run=true;selection_player.mode_flags&=~0x300u;pending_destination=13;return true;
}
bool ApplicationState::clear_page(){auto* bank=animations.resource(19);return bank&&graphics.clear_image(graphics.texture(*bank,1))||fail(graphics.error);}
bool ApplicationState::request_page(i32 page){pending_page=page;return true;}
bool ApplicationState::upload_page(i32){auto* bank=animations.resource(19);return bank&&graphics.upload_png(graphics.texture(*bank,1),pending_png.data(),pending_png.size())||fail(graphics.error);}
bool ApplicationState::read_slot(i32 slot,std::shared_ptr<Replay>& value){return files.replay_slot(slot,value)||fail(files.error);}
bool ApplicationState::prepare_live_replay(bool cleared){
 if(!replay||!replay->live())return fail("No live recording to save");if(!completed_recording){replay->live()->finish(i64(std::time(nullptr)),progress.stage,cleared);completed_recording=true;}live_description=replay->live()->description();if(title)title->live=replay->live();return true;
}
bool ApplicationState::save_slot(i32 slot,const std::array<char,9>& name){
 if(!replay||!replay->live())return fail("Replay save has no recording");ReplayCalendar date;if(!calendar(i64(std::time(nullptr)),date))return false;const auto score=scene()?scene()->battle.score.score:selection_score.score;
 const ReplayExportDetails details{score,double(frames),double(frames?frames:1),date.year,date.month,date.day,date.hour,date.minute};return files.save_replay(slot,*replay->live(),name,details)||fail(files.error);
}
bool ApplicationState::release_live_replay(){if(title)title->live=nullptr;return true;}
bool ApplicationState::release_transient_animations(){for(i32 bank=20;bank<24;bank++)animations.unload(bank);return true;}
bool ApplicationState::clear_title_overlay(){return animations.retire(transition_overlay)||fail(animations.error);}
bool ApplicationState::read_demo(i32 index,std::shared_ptr<Replay>& value){std::vector<u8> bytes;if(index<0||index>=3||!read("demo"+std::to_string(index)+".rpy",bytes))return fail("Original demo unavailable");value=std::make_shared<Replay>();return value->open(bytes.data(),bytes.size())||fail(value->error());}
bool ApplicationState::begin_demo(const ReplayStartRequest& value){if(!read(value.filename,selected_replay))return false;selected_stage=value.stage;progress.replay=true;progress.new_run=true;pending_destination=13;return true;}
bool ApplicationState::queue_title_music(i32 code,i32 value,const std::string& name){return audio_device.queue_music(code,value,name)||fail(audio_device.error);}
bool ApplicationState::clear_current_wave(){audio_device.clear_current_wave();return true;}
bool ApplicationState::reset_replay_selection(){progress.replay=false;return true;}
bool ApplicationState::return_practice_transition(){progress.new_run=true;return true;}
bool ApplicationState::title_exit(i32 value){if(pending_destination==13)return true;pending_destination=value;return true;}
bool ApplicationState::fade_out_title_music(){return music_command(5);}
}
