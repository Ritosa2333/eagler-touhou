#include "../game/CallbackNames.hpp"
#include "Application.hpp"
#include "../game/TextFormat.hpp"
#include "../game/PresentationAudit.hpp"
#ifdef TH_ENABLE_THPRAC
#include "../sdl/ThpracUi.hpp"
#include "Renderer.hpp"
#endif
#include <cstdlib>
#include <cstring>
namespace th10::browser {
namespace{u32 pointer(const void* value){return static_cast<u32>(reinterpret_cast<uintptr_t>(value));}}
AppFrames::AppFrames(Application& a):owner(a){animations=&a.manager;pending_screen=&a.state.pending_screen;background_color=&a.state.background_color;world_camera=&a.engine.world;}
void AppFrames::update_audio(){owner.audio.advance_fades();}
void AppFrames::update_input(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(!owner.state.multiplayer_session.configured||
       (owner.state.netplay_runtime.Playback()&&!owner.state.multiplayer_session.started))
        InputDevices{owner.input}.update(0,false);
    else owner.input.player_profiles[0].input=multiplayer::InputLanes::HostControls(owner.state.input_lanes);
#else
    InputDevices{owner.input}.update(0,false);
#endif
}
i32 AppFrames::process_loading(){return owner.engine.manager.process_loading(owner.engine.resources);}
i32 AppFrames::transition(ApplicationState& app){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(app.pending_screen==12){
        if(!owner.multiplayer_begin_replay(owner.state.replay_filename,u32(owner.state.game.stage))){owner.error=-6;return 4;}
        // The archive supplies all-seat inputs to the same native MP lifecycle
        // as a live session; the retail single-seat decoder is not involved.
        app.pending_screen=7;
    }
#endif
    const i32 result=app.transition(owner.screens);owner.sync_views();return owner.error?4:result;
}
void AppFrames::configure_camera(Camera& camera){owner.configure_camera(camera,false);}
void AppFrames::set_viewport(void*,const CameraViewport& viewport){owner.engine.device.viewport(viewport);}
void AppFrames::clear(u32 color){owner.engine.device.clear_target(1,color,1.f,0,nullptr,0);}
void AppFrames::flush(){owner.engine.flush();}
AppLoop::AppLoop(Application& a):owner(a){application=&a.value;animations=&a.manager;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // The authored Draw can mutate ANM state and consume visual RNG.  MP must
    // execute it once per logical 60 Hz tick regardless of a machine-local
    // frame-skip preference; high-refresh presentation remains separate.
    frame_skip=&a.multiplayer_frame_skip;
#else
    frame_skip=&a.state.configuration.options[4];
#endif
    frame_duration=&a.frame_duration;graphics_state=&a.graphics_state;fog_enabled=&a.engine.fog_enabled;}
Extended AppLoop::time(){return owner.time();}void AppLoop::sleep(u32){}void AppLoop::flush(){owner.engine.flush();}
void AppLoop::configure_flat(Camera& camera){owner.configure_camera(camera,true);}
void AppLoop::set_viewport(void*,const CameraViewport& viewport){owner.engine.device.viewport(viewport);}
i32 AppLoop::update(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    owner.multiplayer_waiting=false;
    if(owner.multiplayer_active())return owner.multiplayer_update();
#endif
    owner.engine.snapshot_presentation();const i32 result=owner.engine.update_all();if(result&&result!=-1)presentation_audit::simulation_tick();return result;
}
void AppLoop::update_audio(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Confirmed logical frames dispatch their native audio queues once. A
    // stalled or resimulated update cannot tick that external queue again.
    if(owner.multiplayer_active())return;
#endif
    owner.audio.update();
}
void AppLoop::stop_loader(){owner.value.stop_loading(owner.screens);}
i32 AppLoop::begin_scene(void*){return owner.engine.device.begin_scene();}
void AppLoop::draw(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // A stalled input frontier is not another authored tick. Keep the last
    // framebuffer; the independent high-refresh renderer may still present it.
    if(owner.multiplayer_waiting)return;
#endif
    presentation_audit::begin_reference();owner.engine.draw_all();
}
#ifdef TH_NATIVE_PLATFORM
i32 AppLoop::set_fog_enabled(bool enabled){owner.engine.device.host.set_fog(enabled);return 0;}
#else
i32 AppLoop::render_state(void*,u32 key,u32 value){return owner.engine.device.render_state(key,value);}
#endif
void AppLoop::clear_texture(void*){owner.engine.device.texture(nullptr);}
void AppLoop::end_scene(void*){owner.engine.device.end_scene();}
void AppLoop::present(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(owner.multiplayer_waiting)return;
#endif
#ifdef TH_ENABLE_THPRAC
    if(auto* renderer=touhou::sdl::current())ThpracUi::render(owner,*renderer);
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(owner.multiplayer_active()&&!owner.multiplayer_finalize_frame())return;
#endif
    Presentation{owner.presentation}.submit();presentation_audit::end_frame();
}
AppStatistics::AppStatistics(Application& a):owner(a){current=&a.statistics;chain=&a.chain;callbacks=&a.engine.callback_environment;game=&a.session_view;text=&a.common_view;timing_counters=a.timing_counters;timing_samples=a.timing_samples;pending_screen=&a.state.pending_screen;frame_skip=&a.state.configuration.options[4];draw_callback=callback_id::FrameStatisticsDraw;}
void* AppStatistics::allocate(u32 bytes){return std::malloc(bytes);}Extended AppStatistics::time(){return owner.time();}
void AppStatistics::draw_rate(CommonResources& common,const Vec3& position,float rate){char output[512];const double value=rate;u32 bits[2];std::memcpy(bits,&value,8);format_text(output,sizeof(output),"%2.1ffps",bits,2);Vec3 adjusted=position;const auto length=std::strlen(output);if(length>7)adjusted.x-=float((length-7)*7);common.queue(output,adjusted,false);common.mark_small();}
}
