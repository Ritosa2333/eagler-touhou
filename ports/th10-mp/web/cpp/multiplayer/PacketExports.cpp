#include "../platform/Application.hpp"
#include <algorithm>

using namespace th10;
extern "C" __attribute__((export_name("multiplayer_packet_build")))
u32 multiplayer_packet_build(browser::Application* app,u32 peer,u32 frame,u32 sequence,u32 ack,u8* out,u32 capacity){
    if(!app||app->stopped||peer>=3||!out)return 0;
    std::vector<u8> wire;
    if(!app->state.netplay_runtime.BuildInputWire(u8(peer),frame,sequence,ack,wire)||wire.size()>capacity)return 0;
    std::memcpy(out,wire.data(),wire.size());return u32(wire.size());
}
extern "C" __attribute__((export_name("multiplayer_packet_apply")))
u32 multiplayer_packet_apply(browser::Application* app,const u8* bytes,u32 size){
    if(!app||app->stopped)return u32(multiplayer::NetplayRuntime::WireResult::Malformed);
    return u32(app->state.netplay_runtime.ApplyWire(bytes,size));
}
extern "C" __attribute__((export_name("multiplayer_generation_status")))
const u32* multiplayer_generation_status(browser::Application* app){
    static u32 words[9]{};std::fill(words,words+9,0);if(!app)return words;
    const auto& runtime=app->state.netplay_runtime;const auto& config=runtime.Config();
    words[0]=1;words[1]=runtime.Generation();words[2]=runtime.Retired();
    words[3]=app->multiplayer_generation_pending;words[4]=u32(config.sessionId);
    words[5]=u32(config.sessionId>>32);words[6]=config.seed;words[7]=config.gameplayAbi;
    words[8]=runtime.CanStart();return words;
}
