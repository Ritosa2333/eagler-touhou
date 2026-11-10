// Development integration fixture: actual gameplay and full shared renderer.
// Audio, Japanese font upload and application destinations are observed here;
// this file is not the complete application or a release entry point.
#include "../../cpp/sdl/SceneDisplay.hpp"
#include "../../cpp/game/GameInput.hpp"
#include "../../cpp/game/Replay.hpp"
#include "../../cpp/sdl/FontDevice.hpp"
#include "../../cpp/sdl/AudioDevice.hpp"
#include <emscripten.h>
using namespace th17;
namespace {
struct Files final:AssetSource {
    bool read(const std::string& name,std::vector<u8>& out)override{size_t size=0;auto* p=static_cast<u8*>(SDL_LoadFile(("/assets/"+name).c_str(),&size));if(!p)return false;out.assign(p,p+size);SDL_free(p);return true;}
};
struct StageDeviceFixture final:StageGameplayServices,StageDrawServices {
    sdl::GraphicsDevice graphics;Rng visual;AnmEnvironment environment;AnmManager animations{visual,environment};AnmRenderer renderer{graphics};
    Files files;GameRun run;GameRecords records;SpellRecords spells;GameInput controls;Replay replay;
    sdl::FontDevice fonts{graphics};sdl::AudioDevice audio_device;std::array<bool,18> unlocked{};
    std::unique_ptr<StageAssets> assets;std::unique_ptr<StageGameplay> scene;std::unique_ptr<sdl::SceneDisplay> display;
    std::string error;u32 frames=0;bool playback=false;std::array<i32,12> projected{};
    bool fail(const std::string& value){if(error.empty())error=value.empty()?"TH17 stage rendering fixture failed":value;return false;}
    ~StageDeviceFixture(){display.reset();scene.reset();if(assets)for(auto& bank:assets->loaded_animations())if(auto* resource=animations.resource(bank.second))graphics.unload(*resource);assets.reset();}
    bool initialize(i32 stage,i32 character,i32 animal,i32 difficulty,u32 scale,const char* replay_file){
        graphics.render_scale=scale==2?2:1;if(!graphics.initialize()||!fonts.initialize()||!audio_device.initialize(files,false))return fail(graphics.error.empty()?fonts.error.empty()?audio_device.error:fonts.error:graphics.error);graphics.backend.defer=true;
        environment.screen_offsets={320,16,320,16};run.stage=run.initial_stage=stage;run.character=character;run.animal=animal;run.difficulty=difficulty;run.screen=7;
        ReplayStageState entry;
        if(replay_file&&*replay_file){std::vector<u8> bytes;if(!files.read(replay_file,bytes)||!replay.open(bytes.data(),bytes.size())||!replay.select(stage)||!replay.initial_state(stage,entry))return fail(replay.error());playback=true;run.scoring.mode_flags=2;entry.seed_random(environment.game_rng,visual);}
        assets=std::make_unique<StageAssets>(files,animations);if(!assets->load(stage,character,animal))return fail(assets->error);
        if(!audio_device.queue_music_track(0,assets->definition->stage_music)||!audio_device.queue_music_track(1,assets->definition->boss_music)||!audio_device.finish_music_requests())return fail(audio_device.error);
        for(const auto& bank:assets->loaded_animations()){auto* resource=animations.resource(bank.second);if(!resource||!graphics.preload(*resource))return fail(graphics.error);}
        scene=std::make_unique<StageGameplay>(*assets,animations,environment,environment.game_rng,visual,run,records,spells,*this);
        StageCamera camera;camera.fov=.6f;camera.direction={0,0,1};if(!scene->prepare(camera,playback?&entry:nullptr))return fail(scene->error);
        display=std::make_unique<sdl::SceneDisplay>(*scene,animations,environment,graphics,renderer);return display->initialize(run,records,spells,*this,assets->text,assets->ascii)||fail(display->error);
    }
    bool step(u32 held,bool draw){
        if(!error.empty())return false;if(playback)held=replay.tick().held;controls.update(held);
        StageGameplayFrame frame;frame.battle.held=held;frame.battle.pressed=controls.pressed;frame.battle.shoot_frames=controls.duration[0];frame.battle.skip_frames=controls.duration[9];
        if(!scene->step(frame))return fail(scene->error);audio_device.update();if(!audio_device.error.empty())return fail(audio_device.error);if(draw){if(!display->draw())return fail(display->error);graphics.backend.commit();}frames++;return true;
    }
    bool audio(i32 id,float pan,PlayerShots::SoundAction action,bool)override{if(action==PlayerShots::SoundAction::stop)return audio_device.effects.stop(id);if(action==PlayerShots::SoundAction::pan){audio_device.sound_pan(u32(id),truncate_int(float(float(pan*1000.f)/192.f)));return true;}return audio_device.effects.positioned(id,pan);}bool popup(const Vec3&,i32,u32)override{return true;}bool trophy(i32)override{return true;}
    bool game_over()override{run.scene_flags|=0x10;return true;}bool finish_practice()override{return true;}bool record_play_time()override{return true;}bool return_to_title()override{return true;}
    bool dialogue_text(AnmVm& vm,u32 color,u32 shadow,i32 style,i32 x,i32 y,const std::string& text)override{return fonts.dialogue(vm,color,shadow,style,x,y,text)||fail(fonts.error);}bool spell_title(AnmVm& vm,const std::string& text)override{return fonts.spell_title(vm,text)||fail(fonts.error);}
    bool music_control(i32 op,i32 value)override{if(op==0)return audio_device.music(value);return audio_device.queue_music(op,value);}bool music_fade(i32 seconds)override{return audio_device.queue_music(5,seconds);}bool unlock_music(i32 index)override{if(u32(index)<unlocked.size())unlocked[index]=true;return true;}bool transition(const SceneTransition&)override{return true;}
    i32 loading_state()const noexcept override{return 0;}bool retire_loading()override{return true;}bool flush_audio()override{return audio_device.finish_music_requests();}bool resume_environment(double seconds)override{return audio_device.seek_music(seconds);}
    bool background_fade(i32,i32,i32)override{return true;}
    const i32* inspect(){auto& b=scene->battle;projected={i32(frames),run.stage,run.screen,b.session.extra_lives,b.session.life_pieces,b.score.score,i32(b.enemies->count()),b.enemy_world.current_chapter,i32(animations.registry.ordered_count(false)),i32(animations.registry.ordered_count(true)),i32(b.player->motion.position.x),i32(b.player->motion.position.y)};return projected.data();}
};std::unique_ptr<StageDeviceFixture> app;
}
extern "C" {
EMSCRIPTEN_KEEPALIVE int stage_initialize(i32 stage,i32 character,i32 animal,i32 difficulty,u32 scale,const char* replay){app=std::make_unique<StageDeviceFixture>();return app->initialize(stage,character,animal,difficulty,scale,replay);}
EMSCRIPTEN_KEEPALIVE int stage_step(u32 held,u32 draw){return app&&app->step(held,draw);}
EMSCRIPTEN_KEEPALIVE const char* stage_error(){return app?app->error.c_str():"TH17 stage fixture unavailable";}
EMSCRIPTEN_KEEPALIVE const i32* stage_state(){return app->inspect();}
EMSCRIPTEN_KEEPALIVE const u8* stage_pixels(){app->graphics.backend.read(sdl::GraphicsDevice::screen);return app->graphics.pixels(sdl::GraphicsDevice::screen)->pixels.data();}
EMSCRIPTEN_KEEPALIVE const touhou::sdl::Statistics* stage_graphics_stats(){return &app->graphics.backend.stats;}
EMSCRIPTEN_KEEPALIVE void stage_close(){app.reset();}
}
