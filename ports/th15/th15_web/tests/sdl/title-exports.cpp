// Developer fixture: actual title/audio/font/graphics composition. It records
// run destinations; it is deliberately not a complete-game executable.
#include "../../cpp/game/TitleScene.hpp"
#include "../../cpp/game/ScreenViews.hpp"
#include "../../cpp/game/AsciiFrame.hpp"
#include "../../cpp/game/AnmSceneEffects.hpp"
#include "../../cpp/sdl/GraphicsDevice.hpp"
#include "../../cpp/sdl/FontDevice.hpp"
#include "../../cpp/sdl/AudioDevice.hpp"
#include "../../cpp/sdl/SceneDisplay.hpp"
#include <SDL3/SDL.h>
#include <ctime>
#include <cstdio>
#include <memory>
using namespace th15;
namespace {
struct TitleDeviceFixture final:TitleScenePlatform,AssetSource {
 sdl::GraphicsDevice graphics;sdl::FontDevice fonts{graphics};sdl::AudioDevice audio;
 Rng random;AnmEnvironment environment;AnmManager animations{random,environment};AnmRenderer renderer{graphics};ScreenViews views{renderer,environment};AsciiText captions{animations,environment};
 FrameScheduler scheduler;std::unique_ptr<sdl::SceneDisplay> display;std::unique_ptr<AnmDrawSchedule> drawing;std::unique_ptr<AnmSceneEffects> effects;std::unique_ptr<AsciiFrame> caption_frame;std::unique_ptr<TitleScene> scene;
 SessionState progress;PlayerLifeSession player;ItemScoreState points;RecordStore records;TitleSelectionSettings selection;TitleAudioSettings volume_settings;TitleControllerSettings controller;MusicComments comments;
 std::array<FrameCallback,2> callbacks;std::array<i32,12> projected{};std::vector<u8> pending_png;std::vector<float> mixed;std::string error;TitleKeyboard keyboard_state;i32 destination=-1,pending_page=-1;u32 overlay=0,frames=0;bool initialized=false;
 bool fail(const std::string& why){if(error.empty())error=why.empty()?"Title fixture operation failed":why;return false;}
 bool read(const std::string& name,std::vector<u8>& bytes)override{const auto at=name.find_last_of("/\\");const auto path=std::string("/assets/")+name.substr(at==std::string::npos?0:at+1);size_t size=0;auto* data=static_cast<u8*>(SDL_LoadFile(path.c_str(),&size));if(!data)return false;bytes.assign(data,data+size);SDL_free(data);return true;}
 bool initialize(bool device){if(!graphics.initialize()||!fonts.initialize()||!audio.initialize(*this,device))return fail(graphics.error.empty()?fonts.error.empty()?audio.error:fonts.error:graphics.error);environment.screen_offsets={320,16,320,16};records.reset(random);std::vector<u8> data;for(const auto& bank:std::array<std::pair<const char*,i32>,6>{{{"text.anm",0},{"ascii.anm",5},{"effect.anm",8},{"title.anm",16},{"title_v.anm",17},{"help.anm",19}}}){if(!read(bank.first,data)||!animations.load(bank.second,data.data(),data.size()))return fail("Title ANM loading failed: "+std::string(bank.first)+": "+animations.error);if(bank.second==5)environment.fallback_sprite_resource=animations.resource(5);if(!graphics.preload(*animations.resource(bank.second)))return fail(graphics.error);}environment.fallback_sprite_resource=animations.resource(5);
 for(i32 style=0;style<23;style++)if(!fonts.preload(style))return fail(fonts.error);if(!captions.initialize(5)||!read("musiccmt.txt",data)||!comments.open(data.data(),data.size()))return fail(comments.error.empty()?captions.error:comments.error);
 effects=std::make_unique<AnmSceneEffects>(animations,environment.game_rng,random,8);display=std::make_unique<sdl::SceneDisplay>(scheduler,animations,environment,graphics,renderer,views,captions,0,5);if(!display->initialize())return fail(display->error);scene=std::make_unique<TitleScene>(progress,player,points,records,selection,volume_settings,controller,comments,animations,*this,scheduler);scene->state.return_reason=0;
 const i32 priorities[]{9,34};for(u32 i=0;i<callbacks.size();i++){auto& c=callbacks[i];c.owner=this;c.enabled=true;if(i==0)c.run=[](void* p){auto& f=*static_cast<TitleDeviceFixture*>(p);return f.animations.update(true)?1:5;};else c.run=[](void* p){auto& f=*static_cast<TitleDeviceFixture*>(p);return f.animations.update(false)?1:5;};scheduler.add(c,FramePass::Update,priorities[i]);}initialized=true;return true;
 }
 ~TitleDeviceFixture(){for(auto& c:callbacks)scheduler.remove(c);scene.reset();display.reset();drawing.reset();caption_frame.reset();effects.reset();animations.resource_release=nullptr;}
 bool step(u32 held,u32 pressed,u32 repeated){if(!initialized||!error.empty()||destination>=0)return false;if(pending_page>=0){pending_page=-1;if(!scene->manual_page_ready())return fail(scene->error);}scene->controls({held,pressed,repeated,0});if(scheduler.update()<0)return fail(scene->error.empty()?scene->frame.error.empty()?animations.error:scene->frame.error:scene->error);if(!audio.error.empty())return fail(audio.error);audio.update();if(!display->draw())return fail(display->error);audio.pump();frames++;return graphics.error.empty()||fail(graphics.error);}
 bool sound(i32 id)override{audio.effects.enqueue(id);return true;}bool text(AnmVm& vm,const DialogueText& text)override{return fonts.text(vm,text)||fail(fonts.error);}bool hud_text(const HudTextDraw& text)override{return captions.enqueue(text)||fail(captions.error);}bool keyboard(TitleKeyboard& value)override{value=keyboard_state;return true;}
 bool calendar(i64 stamp,ReplayCalendar& value)override{const time_t time=stamp;const auto* tm=std::localtime(&time);if(!tm)return fail("Replay timestamp outside calendar range");value={tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min};return true;}bool capture_score_details(PauseScoreDetails& value)override{const auto time=i64(std::time(nullptr));value={{signed_bits(u32(time)),signed_bits(u32(u64(time)>>32))},double(frames),double(frames)};return true;}
 bool music(const std::string& name)override{return audio.queue_music_track(0,name)||fail(audio.error);}bool music(const std::string& name,i32 track)override{return audio.queue_music_track(0,name)&&audio.queue_music(2,0,"dummy")&&records.unlock_music(track);}bool music_command(i32 code)override{return audio.queue_music(code,0,"dummy")||fail(audio.error);}bool start_title_music()override{return audio.queue_music(2,0,"dummy")&&records.unlock_music(0);}
 bool volume(i32 music,i32 sound,i32)override{audio.music_volume=music;audio.effects.master_volume=sound;audio.refresh_volume();return true;}bool save(const TitleControllerSettings& value)override{controller=value;return true;}
 bool checkpoint_available(i32,i32,bool& value)override{value=false;return true;}bool checkpoint_stage(i32,i32,i32&)override{return fail("No checkpoint in title-only fixture");}bool reset_resume_selection()override{return true;}
 bool prepare_game_music()override{return audio.queue_music(5,0,"FadeOut")||fail(audio.error);}bool begin_transition(u32& value)override{value=animations.create(8,0,-1,0);overlay=value;auto* vm=animations.registry.find(value);return vm&&(vm->geometry.overlay||effects->configure(*vm,0));}
 bool start_game(i32 stage)override{destination=13;projected[11]=stage;return true;}bool fade_music(float duration)override{return audio.queue_music(5,truncate_int(duration),"FadeOut")||fail(audio.error);}bool transition_size(float width,float height)override{auto* vm=animations.registry.find(overlay);if(vm)vm->visual.sprite_size={width*2,height*2};return true;}
 bool request_catalog()override{scene->catalog.clear();scene->replay_catalog_ready();return true;}bool release_catalog()override{scene->catalog.clear();return true;}bool start_replay(const ReplayStartRequest& value)override{destination=13;projected[11]=value.stage;return true;}
 bool clear_page()override{return graphics.clear_image(graphics.texture(*animations.resource(19),1));}bool request_page(i32 page)override{char name[32];std::snprintf(name,sizeof name,"help_%.2d.png",page+1);if(!read(name,pending_png))return fail("Missing manual page");pending_page=page;return true;}bool upload_page(i32)override{return graphics.upload_png(graphics.texture(*animations.resource(19),1),pending_png.data(),pending_png.size())||fail(graphics.error);}
 bool read_slot(i32,std::shared_ptr<Replay>& value)override{value.reset();return true;}bool prepare_live_replay(bool)override{return fail("No live recording in title-only fixture");}bool save_slot(i32,const std::array<char,9>&)override{return fail("No live recording in title-only fixture");}bool release_live_replay()override{scene->live=nullptr;return true;}
 bool release_transient_animations()override{return true;}bool clear_title_overlay()override{return animations.retire(overlay);}bool read_demo(i32 index,std::shared_ptr<Replay>& value)override{std::vector<u8> bytes;if(!read("demo"+std::to_string(index)+".rpy",bytes))return fail("Missing original demo");value=std::make_shared<Replay>();return value->open(bytes.data(),bytes.size());}bool begin_demo(const ReplayStartRequest& value)override{destination=13;projected[11]=value.stage;return true;}
 bool queue_title_music(i32 code,i32 value,const std::string& name)override{return audio.queue_music(code,value,name)||fail(audio.error);}bool clear_current_wave()override{audio.clear_current_wave();return true;}bool reset_replay_selection()override{return true;}bool return_practice_transition()override{return true;}bool title_exit(i32 value)override{destination=value;return true;}bool fade_out_title_music()override{return audio.queue_music(5,0,"FadeOut")||fail(audio.error);}
 const i32* state(){auto& s=scene->state;projected[0]=i32(s.screen);projected[1]=s.substate;projected[2]=s.age.current;projected[3]=s.menu.cursor;projected[4]=s.menu.count;projected[5]=s.menu.depth;projected[6]=progress.character;projected[7]=progress.difficulty;projected[8]=progress.stage;projected[9]=player.mode_flags;projected[10]=destination;return projected.data();}
};std::unique_ptr<TitleDeviceFixture> app;std::string music_name;
}
extern "C" {
int title_initialize(int device){app=std::make_unique<TitleDeviceFixture>();return app->initialize(device!=0);}
int title_step(u32 held,u32 pressed,u32 repeated){return app&&app->step(held,pressed,repeated);}
const char* title_error(){return app?app->error.c_str():"No title fixture";}
const i32* title_state(){return app->state();}
void title_select(i32 cursor){app->scene->state.menu.select(cursor);}
int title_unlock(){return app->records.unlock_all();}
const u8* title_pixels(){app->graphics.backend.read(sdl::GraphicsDevice::screen);return app->graphics.pixels(sdl::GraphicsDevice::screen)->pixels.data();}
const char* title_music(){double seconds=0;app->audio.current_music(music_name,seconds);return music_name.c_str();}
const float* title_audio_mix(u32 frames){app->mixed.resize(frames*2);return app->audio.mix(app->mixed.data(),frames)?app->mixed.data():nullptr;}
const u32* title_audio_stats(){return app->audio.statistics();}
u32 title_font_writes(){return app->fonts.writes;}
const touhou::sdl::Statistics* title_graphics_stats(){return &app->graphics.backend.stats;}
void title_close(){app.reset();}
}
