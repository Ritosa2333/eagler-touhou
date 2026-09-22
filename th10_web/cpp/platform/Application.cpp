#include "../game/CallbackNames.hpp"
#include "../game/HighRefresh.hpp"
#include "../game/PresentationAudit.hpp"
#include "Application.hpp"
#ifdef TH_ENABLE_THPRAC
#include "../sdl/ThpracUi.hpp"
#include "Renderer.hpp"
#endif
#include <new>
#include <cstdlib>
#include <cstdio>
namespace th10::browser {
namespace {
#ifdef TH_SDL3
extern "C" double monotonic();
#else
extern "C" __attribute__((import_module("th10_time"),import_name("monotonic"))) double monotonic();
#endif
template<class T>void dispose(T*& value){if(value){value->~T();std::free(value);value=nullptr;}}
template<class T,class... A>T* create(A&... args){auto* p=std::malloc(sizeof(T));return p?new(p)T(args...):nullptr;}
}
Application::Application(FileSystem& f,Input& i,GameState& s,AnimationEngine& e,Fonts& ft,Audio& a,ScreenEffects& fx):files(f),input(i),state(s),engine(e),fonts(ft),audio(a),effects(fx),value(s.application),captures(e),manager(&e.manager),chain(&e.chain_value),screens(*this),frames(*this),loop(*this),rates(*this),presentation(*this),screenshots(*this),config(*this){
    value.device=reinterpret_cast<void*>(static_cast<uintptr_t>(e.device.handle));value.world_camera=e.world;value.ui_camera=e.ui;value.active_camera=&value.ui_camera;
    engine.register_receiver(*this);effects.quitting=&state.quitting;
}
Application::~Application(){shutdown();for(auto*& entry:entries)if(entry){chain->remove_locked(entry,engine.callback_environment);entry=nullptr;}engine.unregister_receiver(*this);}
bool Application::initialize(){
    if(initialized)return !error;initialized=true;
    if(ApplicationConfiguration{config}.load("th10.cfg",state.configuration,disable_vsync)<0)notice_code=static_cast<i32>(ConfigurationNotice::SaveFailed);
    value.display_flags=state.configuration.display_flags;engine.resources.display=value.display_flags;fonts.display=value.display_flags;
    input.thresholds[0]=state.configuration.axis_x;input.thresholds[1]=state.configuration.axis_y;
    clock_origin=monotonic();statistics=FrameStatistics::create(rates);if(!statistics){error=-1;return false;}
    value.screen=-2;value.pending_screen=0;value.reserved_398=0;
    constexpr CallbackToken callbacks[]={callback_id::ApplicationUpdate,callback_id::ApplicationBeginDraw,callback_id::ApplicationDrawBarrier,callback_id::ApplicationFinishDraw};constexpr i32 priorities[]={1,1,40,50};
    for(u32 i=0;i<4;++i)entries[i]=chain->add(callbacks[i],this,priorities[i],i!=0,true,engine.callback_environment);
    Presentation{presentation}.configure_defaults();
#ifdef TH_NATIVE_PLATFORM
    if(!engine.resources.preload_transition(0)||!engine.resources.preload_transition(1)){error=-3;return false;}
    for(u32 resource=2;resource<6;++resource)engine.resources.preload_transition(resource);
#endif
    return true;
}
Extended Application::time(){return Extended::from_double(monotonic()-clock_origin);}
void Application::sync_views(){
    startup_view=startup?startup->value:nullptr;title_view=title?title->value:nullptr;session_view=world?world->actors.session:nullptr;ending_view=credits?credits->value:nullptr;common_view=startup?startup->common_value:nullptr;
    value.startup=startup_view;value.loading_animations=startup?startup->loading_file:nullptr;
    effects.controller_flags=session_view?&session_view->session_flags:nullptr;
}
bool Application::ensure_world(){
    if(world)return true;if(!startup||!startup->shared||!startup->scores){error=-2;return false;}
    world=create<World>(state,engine,*startup->shared,fonts,input,audio,*startup->scores,effects);if(!world)error=-1;return world!=nullptr;
}
void Application::advance_loading(){
    if(startup)startup->advance_loading();sync_views();
    if(title)title->advance_loading();if(world)world->advance_loading();if(credits)credits->advance_loading();sync_views();
    if((startup&&startup->error)||(title&&title->error)||(world&&world->error)||(credits&&credits->error))error=-3;
}
i32 Application::step(bool scheduled_tick){
    if(stopped)return error?2:1;if(!initialized&&!initialize())return 2;
    advance_loading();if(error){stopped=true;return 2;}
    value.ui_camera=engine.ui;value.world_camera=engine.world;
    // A platform presentation clock may own the 60 Hz accumulator. Run one
    // original tick when requested, while retaining the real monotonic clock
    // for frame cost, FPS statistics, screenshots and other device services.
    // This is a platform-clock deadline, not a gameplay calculation. Do the
    // subtraction in native double precision: the game's x87 single mode can
    // round a microsecond away after 32 seconds and accidentally skip a tick.
    if(scheduled_tick)clock.next_frame_time=time().to_double()-0.000001;
    const i32 result=clock.step(loop);sync_views();
    if(writer_pending){writer_pending=false;Screenshot{screenshots}.write();}
    if(result){stopped=true;save();}return result;
}
bool Application::presentation_draw(float alpha,bool interpolate,bool world_interpolate){
    if(stopped||!initialized||error)return false;
    const auto saved_world=engine.world,saved_ui=engine.ui;auto* saved_active=engine.active;
    const auto saved_value_world=value.world_camera,saved_value_ui=value.ui_camera;auto* saved_value_active=value.active_camera;
    const u32 saved_screen_space=engine.screen_space,saved_fog=engine.fog_enabled,saved_graphics=graphics_state;
    const auto saved_reserved=manager->reserved_050,saved_submitted=manager->submitted_draws,saved_started=manager->started_scripts,saved_flushed=manager->flushed_batches;
    AnmVm saved_characters{},saved_small{};i32 saved_text_count=0,saved_early_count=0;u32 saved_text_color=0;Vec2 saved_text_scale{};i32 saved_text_camera=0,saved_text_shadow=0;
    if(common_view){saved_characters=common_view->characters;saved_small=common_view->small_characters;saved_text_count=common_view->text_count;saved_early_count=common_view->early_text_count;saved_text_color=common_view->color;saved_text_scale=common_view->scale;saved_text_camera=common_view->camera;saved_text_shadow=common_view->shadow;}
    high_refresh::begin(alpha,interpolate,true,world_interpolate);presentation_audit::begin_sample(alpha);
    engine.flush();value.active_camera=&value.ui_camera;configure_camera(value.ui_camera,true);engine.device.viewport(value.active_camera->viewport);value.screen_space=1;
    bool presented=false;
    if(engine.device.begin_scene()>=0){
        auto& animations=*manager;animations.batch_quads=0;animations.vertex_write=animations.batch_start=animations.vertex_buffer;graphics_state=255;
        u32 fog=saved_fog;ApplicationLoop::disable_fog(value,fog,loop);engine.draw_all();engine.flush();engine.device.texture(nullptr);engine.device.end_scene();
#ifdef TH_ENABLE_THPRAC
        if(auto* renderer=touhou::sdl::current())ThpracUi::render(*this,*renderer);
#endif
        presented=engine.device.present_frame()>=0;
    }
    engine.world=saved_world;engine.ui=saved_ui;engine.active=saved_active;engine.screen_space=saved_screen_space;engine.fog_enabled=saved_fog;graphics_state=saved_graphics;
    manager->reserved_050=saved_reserved;manager->submitted_draws=saved_submitted;manager->started_scripts=saved_started;manager->flushed_batches=saved_flushed;
    if(common_view){common_view->characters=saved_characters;common_view->small_characters=saved_small;common_view->text_count=saved_text_count;common_view->early_text_count=saved_early_count;common_view->color=saved_text_color;common_view->scale=saved_text_scale;common_view->camera=saved_text_camera;common_view->shadow=saved_text_shadow;}
    value.world_camera=saved_value_world;value.ui_camera=saved_value_ui;value.active_camera=saved_value_active;presentation_audit::end_frame();high_refresh::end();return presented;
}
void Application::presentation_frame(){
    const double now=time().to_double();if(!presentation_origin){presentation_origin=now;presentation_frames=0;}++presentation_frames;const double elapsed=now-presentation_origin;if(elapsed<.5)return;
    presentation_fps=float(double(presentation_frames)/elapsed);presentation_origin=now;presentation_frames=0;
}
i32 Application::draw_statistics(){
    if(high_refresh::render_only){
        if(state.pending_screen!=14&&common_view){
            const u32 color=presentation_fps<30?0xff5050ff:presentation_fps<40?0xffa0a0ff:0xffffffff;
            for(i32 i=common_view->text_count-1;i>=0;--i){auto& entry=common_view->text[i];if(entry.position.x==590&&entry.position.y==470){std::snprintf(entry.text,sizeof(entry.text),"%.2ffps",presentation_fps);entry.color=color;break;}}
        }return 1;
    }
    statistics->actual_ticks=state.active_time;statistics->expected_ticks=state.total_time;const auto result=statistics->draw(rates);state.active_time=statistics->actual_ticks;state.total_time=statistics->expected_ticks;if(world)world->measured_fps=statistics->frames_per_second;return result;
}
void Application::save(){
    config.save("th10.cfg",state.configuration);if(startup&&startup->scores)startup->scores->save();
}
void Application::shutdown(){
    if(writer_pending){writer_pending=false;Screenshot{screenshots}.write();}
    // Wrappers reference Startup's common resources and score store. Release
    // every dependent owner before the store, including retained replay pools.
    if(initialized)save();dispose(title);dispose(credits);dispose(world);sync_views();
    if(statistics){statistics->shutdown(rates);std::free(statistics);statistics=nullptr;}
    dispose(startup);sync_views();stopped=true;
}
void Application::configure_camera(Camera& camera,bool flat){
    const bool ui=&camera==&value.ui_camera;auto& native=ui?engine.ui:engine.world;native=camera;engine.active=&native;engine.screen_space=ui?1:0;engine.configure_camera(flat);camera=native;value.active_camera=&camera;
}
#ifndef TH_NATIVE_PLATFORM
bool Application::invoke(CallbackToken token,void*,i32& result){
    switch(token){
    case callback_id::ApplicationUpdate:result=ApplicationFrame{value,frames}.update();return true;
    case callback_id::ApplicationBeginDraw:value.ui_camera=engine.ui;result=ApplicationFrame{value,frames}.begin_draw();return true;
    case callback_id::ApplicationDrawBarrier:result=1;return true;
    case callback_id::ApplicationFinishDraw:result=ApplicationFrame{value,frames}.finish_draw();return true;
    case callback_id::FrameStatisticsDraw:result=draw_statistics();return true;
    default:return false;
    }
}
#endif
AppScreens::AppScreens(Application& a):owner(a){
    game=&a.state.game;current_game=&a.session_view;current_title=&a.title_view;current_ending=&a.ending_view;current_startup=&a.startup_view;current_replay=&a.state.replay;stages=menu_data(a.state.chinese).stages;current_stage=&a.state.current_stage;registry=&a.engine.manager.registry;loading_ids=a.loading_ids;return_menu=&a.state.return_screen;loading_pause=&a.loading_pause;
}
void AppScreens::enter_lock(ApplicationState&,u32){}void AppScreens::leave_lock(ApplicationState&,u32){}
StartupScreen* AppScreens::create_startup_screen(ApplicationState&){auto& a=owner;a.startup=create<Startup>(a.state,a.engine,a.files,a.audio);if(!a.startup||!a.startup->initialize()){dispose(a.startup);a.error=-1;}a.sync_views();return a.startup_view;}
void AppScreens::destroy_screens(ApplicationState&){owner.shutdown();}
void AppScreens::create_title(){auto& a=owner;if(!a.ensure_world())return;a.title=create<Title>(a.state,a.engine,*a.startup->shared,a.fonts,a.input,a.audio,*a.startup->scores,a.effects);if(a.title){a.title->startup=a.startup->value;a.title->results=&a.world->results_services();if(!a.title->initialize()){dispose(a.title);a.error=-1;}}else a.error=-1;a.sync_views();}
void AppScreens::destroy_title(TitleMenu*){dispose(owner.title);owner.sync_views();}
void AppScreens::create_game(i32 mode){auto& a=owner;if(!a.ensure_world())return;
#ifdef TH_ENABLE_THPRAC
    // th08 GameApplication::enter_game parity: when a game is entered from the
    // title (screen 4), re-derive the live practice run for THIS launch from the
    // launch flags instead of trusting the previous run. A normal launch has no
    // practice flag, so active and the trainer cheats are cleared and a stale
    // thprac run can never be applied to it; a practice launch/restart keeps
    // them. Replay mode derives its run from the PRAC block in the replay.
    if(a.value.previous_screen==4){
        auto& p=a.state.practice;
        p.cheats=0;p.assisted=false;p.replay=mode!=0;
        p.active=p.enabled&&p.run.mode==1&&(p.replay||(a.state.game.flags&0x10)!=0);
    }
#endif
    a.world->new_game=a.value.new_game;if(!a.world->start(mode))a.error=-1;a.sync_views();}
void AppScreens::destroy_game(GameSession*){if(owner.world)owner.world->stop_session();owner.sync_views();}
void AppScreens::create_ending(){auto& a=owner;if(!a.ensure_world())return;a.credits=create<Credits>(*a.world,a.captures);if(!a.credits||!a.credits->initialize()){dispose(a.credits);a.error=-1;}a.sync_views();}
void AppScreens::destroy_ending(Ending*){dispose(owner.credits);owner.sync_views();}
void AppScreens::destroy_startup(StartupScreen*){dispose(owner.credits);dispose(owner.world);dispose(owner.startup);owner.sync_views();}
void AppScreens::destroy_replay(Replay* replay){if(owner.world)owner.world->release_replay(replay);else __builtin_trap();owner.sync_views();}
u32 AppScreens::create_loading_animation(AnmFile& file,i32 script){auto& e=owner.engine;return e.manager.create(file,script,15,AnimationPlacement::WorldBack,e,e);}
// Screen resource owners use typed cooperative loading tasks; no raw callback
// address or opaque worker argument is executed by this adapter.
u32 AppScreens::begin_thread(CallbackToken,void*,u32,u32&){__builtin_trap();}
u32 AppScreens::wait_thread(u32,u32){return 0;}void AppScreens::close_thread(u32){}void AppScreens::sleep(u32){owner.audio.pump();}
}

namespace th10::browser {
void Application::bind_callbacks(Callbacks& b){callback_context=this;
 b.bind(callback_id::ApplicationUpdate,this,[](void* p,void*,i32){auto& s=*static_cast<Application*>(p);return ApplicationFrame{s.value,s.frames}.update();});
 b.bind(callback_id::ApplicationBeginDraw,this,[](void* p,void*,i32){auto& s=*static_cast<Application*>(p);s.value.ui_camera=s.engine.ui;return ApplicationFrame{s.value,s.frames}.begin_draw();});
 b.bind(callback_id::ApplicationDrawBarrier,this,[](void*,void*,i32){return 1;});
 b.bind(callback_id::ApplicationFinishDraw,this,[](void* p,void*,i32){auto& s=*static_cast<Application*>(p);return ApplicationFrame{s.value,s.frames}.finish_draw();});
 b.bind(callback_id::FrameStatisticsDraw,this,[](void* p,void*,i32){return static_cast<Application*>(p)->draw_statistics();});
}
}
