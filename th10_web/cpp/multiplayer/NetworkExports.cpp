#include "../platform/Application.hpp"
#include <cstring>

using namespace th10;
extern "C" __attribute__((export_name("multiplayer_connect")))
u32 multiplayer_connect(browser::Application* app,const char* relay){
    if(!app||app->stopped||app->world||app->state.multiplayer_session.started)return 0;
    return app->state.netplay_runtime.Connect(relay)?1:0;
}
extern "C" __attribute__((export_name("multiplayer_network_poll")))
u32 multiplayer_network_poll(browser::Application* app){
    return app&&!app->stopped&&app->multiplayer_pump_network()?1:0;
}
extern "C" __attribute__((export_name("multiplayer_network_error")))
const char* multiplayer_network_error(browser::Application* app){
    return app?app->state.netplay_runtime.NetworkError():"Application unavailable";
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
    words[13]=mode&&!std::strcmp(mode,"rtc")?1:mode&&!std::strcmp(mode,"relay")?2:0;
    words[14]=u32(runtime.Transport().BufferedAmount());return words;
}
