#include "../platform/Application.hpp"
#include <cstring>

using namespace th10;
extern "C" __attribute__((export_name("multiplayer_local_player_visibility")))
u32 multiplayer_local_player_visibility(browser::Application* app,u32 enabled){
    if(!app||app->stopped||enabled>1)return 0;
    const bool readOnly=app->state.netplay_runtime.Spectator()||app->state.netplay_runtime.Playback();
    app->engine.enhance_local_player_visibility=enabled&&!readOnly;
    return !enabled||!readOnly;
}
extern "C" __attribute__((export_name("multiplayer_connect")))
u32 multiplayer_connect(browser::Application* app,const char* relay){
    if(!app||app->stopped||app->world||app->state.multiplayer_session.started)return 0;
    return app->state.netplay_runtime.Connect(relay)?1:0;
}
extern "C" __attribute__((export_name("multiplayer_spectator_connect")))
u32 multiplayer_spectator_connect(browser::Application* app,const char* relay,
                                  const char* spectatorId){
    if(!app||app->stopped||app->world||app->state.multiplayer_session.started)return 0;
    return app->state.netplay_runtime.ConnectSpectator(relay,spectatorId)?1:0;
}
extern "C" __attribute__((export_name("multiplayer_spectator_playback_state")))
const char* multiplayer_spectator_playback_state(browser::Application* app){
    return app&&!app->stopped?app->state.netplay_runtime.SpectatorPlaybackState():nullptr;
}
extern "C" __attribute__((export_name("multiplayer_spectator_playback_control")))
i32 multiplayer_spectator_playback_control(browser::Application* app,const char* run,
                                            const char* action,u32 frame){
    return app&&!app->stopped&&app->state.netplay_runtime.SpectatorPlaybackControl(run,action,frame)?1:0;
}
extern "C" __attribute__((export_name("multiplayer_network_poll")))
u32 multiplayer_network_poll(browser::Application* app){
    return app&&!app->stopped&&app->multiplayer_pump_network()?1:0;
}
extern "C" __attribute__((export_name("multiplayer_network_error")))
const char* multiplayer_network_error(browser::Application* app){
    return app?app->state.netplay_runtime.NetworkError():"Application unavailable";
}
extern "C" __attribute__((export_name("multiplayer_error_detail")))
const char* multiplayer_error_detail(browser::Application* app){
    if(!app)return "Application unavailable";
    return (app->error==-4||app->error==-5)&&app->multiplayer_failure_detail[0]
        ?app->multiplayer_failure_detail:app->state.netplay_runtime.NetworkError();
}
extern "C" __attribute__((export_name("multiplayer_pacing_status")))
const double* multiplayer_pacing_status(browser::Application* app){
    static double values[2]{1,0};
    values[0]=app?app->state.netplay_runtime.Channel().IntervalScale():1;
    values[1]=app?app->state.netplay_runtime.Channel().FrameLead():0;
    return values;
}
extern "C" __attribute__((export_name("multiplayer_network_gap_tail_set_enabled")))
u32 multiplayer_network_gap_tail_set_enabled(browser::Application* app,u32 enabled){
    if(!app||app->stopped||enabled>1)return 0;
    return app->state.netplay_runtime.SetNetworkGapTailEnabled(enabled!=0)?1:0;
}
extern "C" __attribute__((export_name("multiplayer_network_gap_tail_status")))
const double* multiplayer_network_gap_tail_status(browser::Application* app){
    // Six doubles: schema, selected, locked, gap scale, phase scale, effective.
    static double values[6]{};std::memset(values,0,sizeof(values));
    if(!app)return values;
    const auto& runtime=app->state.netplay_runtime;
    values[0]=1;values[1]=runtime.NetworkGapTailEnabled();values[2]=runtime.NetworkGapTailLocked();
    values[3]=runtime.InputGapScale();values[4]=runtime.Channel().IntervalScale();
    values[5]=runtime.IntervalScale();return values;
}
extern "C" __attribute__((export_name("multiplayer_network_tail_presentation_set_enabled")))
u32 multiplayer_network_tail_presentation_set_enabled(browser::Application* app,u32 enabled){
    if(!app||app->stopped||enabled>1)return 0;
    return app->state.netplay_runtime.SetNetworkTailPresentationEnabled(enabled!=0)?1:0;
}
extern "C" __attribute__((export_name("multiplayer_network_tail_presentation_status")))
const double* multiplayer_network_tail_presentation_status(browser::Application* app){
    // Independent display choice; the existing six-double pacing ABI is unchanged.
    static double values[3]{};std::memset(values,0,sizeof(values));
    if(!app)return values;
    const auto& runtime=app->state.netplay_runtime;
    values[0]=1;values[1]=runtime.NetworkTailPresentationEnabled();
    values[2]=runtime.NetworkTailPresentationLocked();return values;
}
extern "C" __attribute__((export_name("multiplayer_transport_status")))
const u32* multiplayer_transport_status(browser::Application* app){
    static u32 words[15]{};std::memset(words,0,sizeof(words));words[0]=1;if(!app)return words;
    const auto& runtime=app->state.netplay_runtime;const auto& channel=runtime.Channel();
    words[1]=runtime.NetworkEnabled();words[2]=runtime.Transport().IsOpen();
    words[3]=u32(channel.Error());words[4]=channel.PacketsSent();words[5]=channel.PacketsReceived();
    words[6]=channel.PacketsIgnored();words[7]=channel.RepairsSent();words[8]=channel.LatestCapture();
    words[9]=runtime.AcknowledgedLocalThroughAllRemotes();words[10]=runtime.ConfirmedThroughAllRemotes();
    words[11]=channel.Retiring();words[12]=runtime.Generation();
    const char* mode=runtime.Transport().Mode();
    words[13]=mode&&!std::strcmp(mode,"rtc")?1:
              mode&&!std::strcmp(mode,"relay")?2:
              mode&&!std::strcmp(mode,"spectator")?3:0;
    words[14]=u32(runtime.Transport().BufferedAmount());return words;
}
extern "C" __attribute__((export_name("multiplayer_spectator_status")))
const u32* multiplayer_spectator_status(browser::Application* app){
    static u32 words[7]{};std::memset(words,0,sizeof(words));words[0]=1;if(!app)return words;
    const auto& runtime=app->state.netplay_runtime;
    words[1]=runtime.Spectator();words[2]=runtime.SpectatorRetired();
    words[3]=u32(runtime.SpectatorBacklog());
    words[4]=runtime.Transport().HasSpectators();
    words[5]=runtime.NetworkEnabled();words[6]=runtime.Generation();return words;
}
