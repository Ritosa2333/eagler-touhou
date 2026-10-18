// Developer fixture for actual ending graphics, measured fonts and music.
#include "../../cpp/game/EndingScene.hpp"
#include "../../cpp/game/AsciiFrame.hpp"
#include "../../cpp/game/ScreenViews.hpp"
#include "../../cpp/game/AnmSceneEffects.hpp"
#include "../../cpp/sdl/GraphicsDevice.hpp"
#include "../../cpp/sdl/FontDevice.hpp"
#include "../../cpp/sdl/AudioDevice.hpp"
#include <memory>
using namespace th15;
namespace {
struct EndingDeviceFixture final:EndingScenePlatform,AssetSource {
 sdl::GraphicsDevice graphics;sdl::FontDevice fonts{graphics};sdl::AudioDevice audio;Rng visual;AnmEnvironment environment;AnmManager animations{visual,environment};AnmRenderer renderer{graphics};ScreenViews views{renderer,environment};AsciiText captions{animations,environment};FrameScheduler scheduler;RecordStore records;SessionState progress;std::unique_ptr<AnmDrawSchedule> drawing;std::unique_ptr<AsciiFrame> caption_frame;std::unique_ptr<AnmSceneEffects> effects;std::unique_ptr<EndingScene> ending;std::array<FrameCallback,2> callbacks;std::string error,pending_name;std::array<i32,12> observed{};std::vector<float> mixed;i32 pending_bank=-1,next=-1;u32 frames=0,loads=0,shakes=0,overlay=0;
 bool fail(const std::string& why){if(error.empty())error=why.empty()?"Ending fixture operation failed":why;return false;}
 bool read(const std::string& name,std::vector<u8>& out)override{const auto at=name.find_last_of("/\\");const auto path=std::string("/assets/")+name.substr(at==std::string::npos?0:at+1);size_t size=0;auto* data=static_cast<u8*>(SDL_LoadFile(path.c_str(),&size));if(!data)return false;out.assign(data,data+size);SDL_free(data);return true;}
 bool initialize(i32 character,i32 difficulty,i32 continues,u32 mode,bool seen){if(!graphics.initialize()||!fonts.initialize()||!audio.initialize(*this,false))return fail(graphics.error.empty()?fonts.error.empty()?audio.error:fonts.error:graphics.error);environment.screen_offsets={320,16,320,16};records.reset(visual);progress.character=character;progress.difficulty=difficulty;progress.continues=continues;records.endings.fill(seen);records.ending_seen=seen;animations.resource_release=[this](const AnmResource& r){graphics.unload(r);};std::vector<u8> data;for(const auto& b:std::array<std::pair<const char*,i32>,3>{{{"text.anm",0},{"ascii.anm",5},{"effect.anm",8}}}){if(!read(b.first,data)||!animations.load(b.second,data.data(),data.size()))return fail("Ending ANM loading: "+std::string(b.first)+": "+animations.error);if(b.second==5)environment.fallback_sprite_resource=animations.resource(5);if(!graphics.preload(*animations.resource(b.second)))return fail(graphics.error);}for(i32 style=0;style<23;style++)if(!fonts.preload(style))return fail(fonts.error);if(!captions.initialize(5))return fail(captions.error);effects=std::make_unique<AnmSceneEffects>(animations,environment.game_rng,visual,8);drawing=std::make_unique<AnmDrawSchedule>(scheduler,animations,renderer,graphics,views);caption_frame=std::make_unique<AsciiFrame>(scheduler,captions,renderer,views);for(u32 i=0;i<2;i++){auto& c=callbacks[i];c.owner=this;c.enabled=true;c.run=i==0?[](void* p)->i32{return static_cast<EndingDeviceFixture*>(p)->animations.update(true)?1:5;}:[](void* p)->i32{return static_cast<EndingDeviceFixture*>(p)->animations.update(false)?1:5;};scheduler.add(c,FramePass::Update,i==0?9:34);}ending=std::make_unique<EndingScene>(animations,records,*this,scheduler);return ending->initialize(progress,mode)||fail(ending->error);}
 ~EndingDeviceFixture(){ending.reset();for(auto& c:callbacks)scheduler.remove(c);caption_frame.reset();drawing.reset();effects.reset();animations.resource_release=nullptr;}
 bool step(u32 held,u32 pressed,i32 age){if(next>=0||!error.empty())return false;if(pending_bank>=0){std::vector<u8> bytes;const auto bank=pending_bank;pending_bank=-1;if(!read(pending_name,bytes)||!ending->animation_ready(bank,bytes.data(),bytes.size()))return fail(ending->error);loads++;}ending->controls({held,pressed,u32(age),1},0);if(scheduler.update()<0)return fail(ending->error.empty()?ending->script.error.empty()?animations.error:ending->script.error:ending->error);audio.update();if(!audio.error.empty())return fail(audio.error);graphics.select_target(nullptr,0);graphics.clear(0xff000000);views.camera(DrawCamera::Fullscreen,false);if(scheduler.draw()<0)return fail(drawing->error.empty()?caption_frame->error:drawing->error);renderer.flush();graphics.present();animations.collect_resources();frames++;return graphics.error.empty()||fail(graphics.error);}
 bool read_message(const std::string& name,MessageProgram& p)override{std::vector<u8> bytes;return read(name,bytes)&&p.open(bytes.data(),bytes.size());}
 bool text(AnmVm& vm,const DialogueText& value)override{return fonts.text(vm,value)||fail(fonts.error);}
 bool request_animation(i32 bank,const std::string& name)override{if(pending_bank>=0)return fail("Ending resource overlap");pending_bank=bank;pending_name=name;return true;}
 bool prepare_animation(AnmResource& r)override{return graphics.preload(r)||fail(graphics.error);}
 bool cancel_animation_request()override{pending_bank=-1;pending_name.clear();return true;}
 bool loading_overlay()override{if(!overlay){overlay=animations.create(8,38,-1,0);if(!overlay)return fail(animations.error);}return true;}
 bool end_loading_overlay()override{return animations.retire(overlay);}
 bool prepare_music(const std::string& name)override{return audio.queue_music_track(0,name)||fail(audio.error);}
 bool start_music(i32)override{return audio.queue_music(2,0,"dummy")||fail(audio.error);}
 bool fade_music(i32 value)override{return audio.queue_music(5,value,"FadeOut")||fail(audio.error);}
 bool sound(i32 id)override{audio.effects.enqueue(id);return true;}
 bool shake(i32,i32)override{shakes++;return true;}
 bool reset_caption_state()override{captions.spacing=9;return true;}
 bool ending_destination(i32 value)override{next=value;return true;}
 const i32* state(){auto& s=ending->script.state;observed={i32(frames),s.age.current,s.clock.current,s.wait.current,i32(s.flags),s.line,i32(fonts.writes),i32(loads),next,i32(shakes),pending_bank,i32(animations.resource_count())};return observed.data();}
};
std::unique_ptr<EndingDeviceFixture> fixture;std::vector<u8> pixels;std::string error,music_name;
}
extern "C" {
__attribute__((used)) i32 ending_initialize(i32 character,i32 difficulty,i32 continues,u32 mode,i32 seen){fixture=std::make_unique<EndingDeviceFixture>();if(!fixture->initialize(character,difficulty,continues,mode,seen)){error=fixture->error;fixture.reset();return 0;}return 1;}
__attribute__((used)) i32 ending_step(u32 held,u32 pressed,i32 age){return fixture&&fixture->step(held,pressed,age);}
__attribute__((used)) const char* ending_error(){return fixture?fixture->error.c_str():error.c_str();}
__attribute__((used)) const i32* ending_state(){return fixture?fixture->state():nullptr;}
__attribute__((used)) const char* ending_music(){if(!fixture)return "";double seconds=0;fixture->audio.current_music(music_name,seconds);return music_name.c_str();}
__attribute__((used)) const u8* ending_pixels(){if(!fixture)return nullptr;fixture->graphics.backend.read(sdl::GraphicsDevice::screen);return fixture->graphics.pixels(sdl::GraphicsDevice::screen)->pixels.data();}
__attribute__((used)) const float* ending_audio_mix(u32 frames){if(!fixture||frames>441000)return nullptr;fixture->mixed.resize(frames*2);return fixture->audio.mix(fixture->mixed.data(),frames)?fixture->mixed.data():nullptr;}
__attribute__((used)) void ending_close(){fixture.reset();}
}
