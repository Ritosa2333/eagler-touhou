#include "Application.hpp"
#include <new>
#include <cstdlib>
using namespace th10;
#define APP_EXPORT(name) extern "C" __attribute__((export_name(name)))
#ifdef TH_NATIVE_PLATFORM
APP_EXPORT("application_resident_hits") u32 application_resident_hits(browser::Application* app){return app->engine.resources.resident_hits;}
APP_EXPORT("application_preload_stats") const u32* application_preload_stats(browser::Application* app){static u32 out[3];out[0]=app->engine.resources.prepared.size();out[1]=app->engine.resources.prepared_bytes;out[2]=app->engine.resources.cache_hits;return out;}
#endif
APP_EXPORT("application_create") browser::Application* application_create(browser::FileSystem* files,browser::Input* input,browser::GameState* state,browser::AnimationEngine* engine,browser::Fonts* fonts,browser::Audio* audio,browser::ScreenEffects* effects){auto* memory=std::malloc(sizeof(browser::Application));if(!memory)return nullptr;auto* app=new(memory)browser::Application(*files,*input,*state,*engine,*fonts,*audio,*effects);if(!app->initialize()){app->~Application();std::free(app);return nullptr;}return app;}
APP_EXPORT("application_destroy") void application_destroy(browser::Application* app){if(app){app->~Application();std::free(app);}}
APP_EXPORT("application_step") i32 application_step(browser::Application* app){return app->step();}
APP_EXPORT("application_tick") i32 application_tick(browser::Application* app){return app->step(true);}
APP_EXPORT("application_delay") double application_delay(browser::Application* app){const double delay=(Extended::from_double(app->clock.next_frame_time)-app->time()).to_double()*1000;return delay>0?delay:0;}
APP_EXPORT("application_save") void application_save(browser::Application* app){app->save();}
APP_EXPORT("application_error") i32 application_error(browser::Application* app){return app->error;}
APP_EXPORT("application_state") ApplicationState* application_state(browser::Application* app){return &app->value;}
APP_EXPORT("application_statistics") FrameStatistics* application_statistics(browser::Application* app){return app->statistics;}
APP_EXPORT("application_world") browser::World* application_world(browser::Application* app){return app->world;}
APP_EXPORT("application_title") TitleMenu* application_title(browser::Application* app){return app->title_view;}
APP_EXPORT("application_ending") Ending* application_ending(browser::Application* app){return app->ending_view;}
APP_EXPORT("application_startup") StartupScreen* application_startup(browser::Application* app){return app->startup_view;}
APP_EXPORT("application_stop") void application_stop(browser::Application* app){app->state.pending_screen=3;}
APP_EXPORT("application_touch_target") void application_touch_target(browser::Application* app,i32 mode,float x,float y){if(app&&app->world)app->world->motion.target(mode,x,y);}
APP_EXPORT("application_touch_display") void application_touch_display(browser::Application* app,i32 hitbox){if(app&&app->world)app->world->always_hitbox=hitbox!=0;}
APP_EXPORT("application_original_options") void application_original_options(browser::Application* app,i32 faith_bar,i32 hints){
    if(!app)return;
    // custom.exe's overhead gauge is bit 7 of the original display flags.
    // Stage creation copies this configuration into the player's draw owner.
    app->state.configuration.display_flags=(app->state.configuration.display_flags&~0x80u)|(faith_bar?0x80u:0u);
    app->value.display_flags=(app->value.display_flags&~0x80u)|(faith_bar?0x80u:0u);
    // Original Hint mode 1 reads the supplied files without recording new hints.
    app->state.configuration.music_mode=hints?1:0;
}
APP_EXPORT("application_touch_state") void application_touch_state(browser::Application* app,u32* output){
    std::memset(output,0,32);auto* w=app->world;auto* p=w?w->actors.player:nullptr;auto* session=w?w->actors.session:nullptr;auto* gui=w?w->actors.gui:nullptr;
    if(app->ending_view){output[0]=2;return;}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    auto* economy=&app->state.game;
    if(w&&app->state.multiplayer_session.configured&&w->local_player<w->player_count){
        p=w->pilots[w->local_player].player;economy=&w->pilots[w->local_player].game;
    }
#endif
    if(app->state.return_screen==2||(app->state.game.flags&0x20)){output[0]=3;return;}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(!p||!session||economy->lives<0||(session->session_flags&0x70)||!session->update_entry||!(session->update_entry->flags&2))return;
#else
    if(!p||!session||app->state.game.lives<0||(session->session_flags&0x70)||!session->update_entry||!(session->update_entry->flags&2))return;
#endif
    if(gui&&gui->dialogue){output[0]=2;return;}output[0]=1;output[1]=static_cast<u32>(reinterpret_cast<uintptr_t>(p));output[2]=p->state==1;
    const float values[]={p->position.x,p->position.y,p->fast_speed*.01f*app->engine.speed,p->slow_speed*.01f*app->engine.speed,app->engine.speed};std::memcpy(output+3,values,sizeof(values));
}
