// Actual SDL3 rendering fixture for independently verified TH17 menu logic.
// Game destinations are recorded here; this is not the production application.
#include "../../cpp/game/TitleMainMenu.hpp"
#include "../../cpp/game/TitleSelectionMenu.hpp"
#include "../../cpp/game/TitleAnimalMenu.hpp"
#include "../../cpp/game/AnmRenderer.hpp"
#include "../../cpp/sdl/GraphicsDevice.hpp"
#include <emscripten.h>
#include <memory>
using namespace th17;
namespace {
struct TitleDeviceFixture final:TitleGameStartServices {
    sdl::GraphicsDevice graphics;Rng random;AnmEnvironment environment;AnmManager animations{random,environment};AnmRenderer renderer{graphics};
    TitleState state;GameSelection selection;TitleSelectionSettings settings;TitleRecords records;
    TitleMainMenu main{state,selection,settings,animations};TitleDifficultyMenu difficulty{state,selection,settings,animations};TitleCharacterMenu character{state,selection,settings,animations};
    TitleAnimalMenu animal{state,selection,settings,animations,*this};TitleStageMenu stage{state,selection,settings,animations,*this};
    std::string error;std::array<i32,12> projected{};i32 destination=-1;u32 frames=0;
    bool fail(const std::string& value){if(error.empty())error=value.empty()?"TH17 title rendering failed":value;return false;}
    bool initialize(u32 scale){
        graphics.render_scale=scale>=2?2:1;if(!graphics.initialize())return fail(graphics.error);
        graphics.backend.defer=true;environment.screen_offsets={320,16,320,16};
        for(const auto& bank:std::array<std::pair<const char*,i32>,3>{{{"ascii.anm",0},{"title.anm",16},{"title_v.anm",17}}}){
            const auto path=std::string("/assets/")+bank.first;size_t size=0;auto* bytes=static_cast<u8*>(SDL_LoadFile(path.c_str(),&size));if(!bytes)return fail("Missing title asset: "+path);
            const bool loaded=animations.load(bank.second,bytes,size);SDL_free(bytes);if(!loaded)return fail(animations.error);
            if(!graphics.preload(*animations.resource(bank.second)))return fail(graphics.error);
        }
        if(!animations.sprite_fallback(0))return fail("Title fallback atlas unavailable");
        auto output=[&](i32 id){return sound(id);};main.play_sound=output;difficulty.play_sound=output;character.play_sound=output;return true;
    }
    bool step(u32 pressed,u32 repeated){
        if(!error.empty()||destination>=0)return false;bool ok=false;
        switch(state.screen){
        case TitleScreen::Main:ok=main.update(pressed,repeated,records.extra(0)||records.extra(1)||records.extra(2));if(!ok)return fail(main.error);break;
        case TitleScreen::Difficulty:ok=difficulty.update(pressed,repeated,records);if(!ok)return fail(difficulty.error);break;
        case TitleScreen::Character:ok=character.update(pressed,repeated,records);if(!ok)return fail(character.error);break;
        case TitleScreen::Animal:ok=animal.update(pressed,repeated,records);if(!ok)return fail(animal.error);break;
        case TitleScreen::Stage:ok=stage.update(pressed,repeated,records);if(!ok)return fail(stage.error);break;
        default:return fail("Screen is outside this development rendering fixture");
        }
        const float rate=1;state.age.tick(&rate);animations.rate=rate;if(!animations.update(true)||!animations.update(false))return fail(animations.error);
        renderer.invalidate();if(!graphics.select_target(nullptr,0))return fail(graphics.error);graphics.clear(0xff000000);renderer.set_viewport({});graphics.set_fog(false);
        for(u32 layer=0;layer<AnmRegistry::scene_layer_count;layer++)if(!renderer.draw_layer(animations.registry.layer(layer)))return fail(renderer.error);
        renderer.flush();graphics.present();graphics.backend.commit();frames++;return graphics.error.empty()||fail(graphics.error);
    }
    bool sound(i32)override{return true;}bool fade_menu_music()override{return true;}bool show_loading_indicator()override{return true;}
    bool begin_transition(u32& value)override{value=0;return true;}bool start_game(i32 value)override{destination=value;return true;}i32 checkpoint_key()const override{return 0;}
    const i32* inspect(){projected={i32(state.screen),state.substate,state.age.current,state.menu.selected,state.menu.count,state.menu.depth,selection.character,selection.animal,selection.difficulty,selection.stage,destination,i32(frames)};return projected.data();}
};std::unique_ptr<TitleDeviceFixture> app;
}
extern "C" {
EMSCRIPTEN_KEEPALIVE int title_initialize(u32 scale){app=std::make_unique<TitleDeviceFixture>();return app->initialize(scale);}
EMSCRIPTEN_KEEPALIVE int title_step(u32 pressed,u32 repeated){return app&&app->step(pressed,repeated);}
EMSCRIPTEN_KEEPALIVE const char* title_error(){return app?app->error.c_str():"No TH17 rendering fixture";}
EMSCRIPTEN_KEEPALIVE const i32* title_state(){return app->inspect();}
EMSCRIPTEN_KEEPALIVE void title_select(i32 value){app->state.menu.select(value);}
EMSCRIPTEN_KEEPALIVE void title_unlock(){for(auto& values:app->records.cleared)values.fill(1);for(auto& variant:app->records.stage_available)for(auto& stages:variant)stages.fill(1);}
EMSCRIPTEN_KEEPALIVE const u8* title_pixels(){app->graphics.backend.read(sdl::GraphicsDevice::screen);return app->graphics.pixels(sdl::GraphicsDevice::screen)->pixels.data();}
EMSCRIPTEN_KEEPALIVE const touhou::sdl::Statistics* title_graphics_stats(){return &app->graphics.backend.stats;}
EMSCRIPTEN_KEEPALIVE void title_close(){app.reset();}
}
