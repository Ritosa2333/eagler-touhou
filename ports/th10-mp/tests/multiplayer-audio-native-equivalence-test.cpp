#include "../th10_web/cpp/multiplayer/AudioEvents.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using namespace th10;
using th10::multiplayer::AudioEvents;
namespace {
SoundDefinition definitions[128]{};
struct Observed final:AudioCommandSink {
    std::vector<std::string> values;
    void capture_effect(i32 id,i32 pan,const SoundDefinition* defs) noexcept override{
        assert(defs==definitions);values.push_back("e:"+std::to_string(id)+":"+std::to_string(pan));
    }
    void capture_stop(i32 id) noexcept override{values.push_back("s:"+std::to_string(id));}
    void capture_music(i32 kind,i32 argument,const char* name) override{
        values.push_back("m:"+std::to_string(kind)+":"+std::to_string(argument)+":"+name);
    }
};
struct DigestOracle final:AudioCommandSink {
    u32 digest=2166136261u,effects=0,stops=0,music=0;
    void word(u32 value){for(u32 shift=0;shift<32;shift+=8){digest^=(value>>shift)&255u;digest*=16777619u;}}
    void begin(u32 frame,u32 count){word(frame);word(count);}
    void capture_effect(i32 id,i32 pan,const SoundDefinition* defs) noexcept override{
        assert(defs==definitions);word(0);word(u32(id));word(u32(pan));++effects;
    }
    void capture_stop(i32 id) noexcept override{word(1);word(u32(id));word(0);++stops;}
    void capture_music(i32 kind,i32 argument,const char* name) override{
        word(2);word(u32(kind));word(u32(argument));
        for(const char* c=name;*c;++c){digest^=u8(*c);digest*=16777619u;}++music;
    }
};
void initialize(AudioManager& manager){for(auto& slot:manager.pending_effects)slot=-1;}
void equal_native(const AudioManager& a,const AudioManager& b){
    // command_sink is intentionally different during prediction.
    assert(std::memcmp(&a,&b,offsetof(AudioManager,command_sink))==0);
}
u32 random_state=0x293ac1d9;
u32 random(){random_state=random_state*1664525+1013904223;return random_state;}
void command(AudioManager& manager,u32 selector,u32 sample){
    const auto effect=i32(sample%15),pan=i32((sample>>8)%128);
    if(selector%17==0)manager.queue_music(i32(sample%8+1),i32(sample%3),"retained-music.wav");
    else if(selector%13==0)manager.stop_effect(effect);
    else manager.queue_effect(effect,pan,definitions);
}
bool finish(void* value){++*static_cast<u32*>(value);return true;}
}
int main(){
    static_assert(sizeof(AudioEvents)==352,"Keep the wasm32 audio outbox layout stable");
    for(u32 i=0;i<128;++i)definitions[i].lifetime=i32(i+1);
    // Original <=1024 stream semantics, including persistent native queue
    // state as when output is disabled or the device is unavailable.
    AudioEvents candidate;DigestOracle digest_oracle;
    AudioManager native{},confirmed{},capture{},expected{};
    initialize(native);initialize(confirmed);
    capture.command_sink=&candidate;expected.command_sink=&digest_oracle;
    for(u32 frame=0;frame<200;++frame){
        assert(candidate.BeginFrame(frame));digest_oracle.begin(frame,1024);
        for(u32 i=0;i<1024;++i){
            const auto selector=random(),sample=random();
            command(native,selector,sample);command(capture,selector,sample);command(expected,selector,sample);
        }
        assert(candidate.EndFrame());
        assert(candidate.CommitThrough(frame,frame,confirmed,nullptr,nullptr));
        equal_native(native,confirmed);
        assert(candidate.Digest()==digest_oracle.digest);
        assert(candidate.EffectsCommitted()==digest_oracle.effects);
        assert(candidate.StopsCommitted()==digest_oracle.stops);
        assert(candidate.MusicCommitted()==digest_oracle.music);
    }
    std::puts("204800 mixed calls: native bytes, logical digest and counters identical across 200 unflushed frames");

    // All previous maximum-size music records still fit the original budget.
    candidate.Reset();Observed observer,old_observer;DigestOracle music_digest;music_digest.begin(0,1024);
    AudioManager observed{},old_observed{};observed.command_sink=&observer;old_observed.command_sink=&old_observer;
    assert(candidate.BeginFrame(0));
    char long_name[256];std::memset(long_name,'x',255);long_name[255]=0;
    for(u32 i=0;i<1024;++i){capture.queue_music(2,i32(i),long_name);old_observed.queue_music(2,i32(i),long_name);music_digest.capture_music(2,i32(i),long_name);}
    long_name[0]='y';assert(candidate.EndFrame());
    assert(candidate.CommitThrough(0,0,observed,nullptr,nullptr));
    assert(observer.values==old_observer.values&&candidate.Digest()==music_digest.digest);
    std::puts("1024 maximum-length copied music names: original order, bytes and digest identical");

    // More than the old limit without dropping/interchanging any command,
    // including unknown prior native queue contents and stop/play aliasing.
    candidate.Reset();std::memset(&native,0,sizeof(native));std::memset(&confirmed,0,sizeof(confirmed));
    initialize(native);initialize(confirmed);
    assert(candidate.BeginFrame(0));
    for(u32 i=0;i<12000;++i){
        const auto selector=i%31==0?13u:1u,sample=random();
        command(native,selector,sample);command(capture,selector,sample);
    }
    assert(candidate.EndFrame());assert(candidate.CommitThrough(0,0,confirmed,nullptr,nullptr));
    equal_native(native,confirmed);assert(candidate.EffectsCommitted()+candidate.StopsCommitted()==12000);
    std::puts("12000 dense mixed effect/stop calls: original native queue bytes identical");

    // Wrong speculative dense frame is replaced, never emitted, and the
    // committed cursor cannot be rewound or replayed into output twice.
    candidate.Reset();observer.values.clear();u32 ticks=0;
    assert(candidate.BeginFrame(0));for(u32 i=0;i<12000;++i)capture.queue_effect(3,i32(i),definitions);
    assert(candidate.EndFrame());assert(candidate.DiscardFrom(0));
    assert(candidate.BeginFrame(0));capture.queue_music(7,0,"correct.wav");assert(candidate.EndFrame());
    assert(candidate.CommitThrough(Netplay::INVALID_FRAME,0,observed,finish,&ticks)&&observer.values.empty());
    assert(candidate.CommitThrough(0,0,observed,finish,&ticks));
    assert(observer.values==std::vector<std::string>{"m:7:0:correct.wav"}&&ticks==1);
    const auto digest=candidate.Digest();
    assert(candidate.CommitThrough(0,0,observed,finish,&ticks)&&ticks==1&&candidate.Digest()==digest);
    assert(!candidate.DiscardFrom(0));
    std::puts("dense rollback/discard/recommit: no speculative output or double emission");

    // Exactly full byte budget succeeds; one more command fails explicitly.
    candidate.Reset();assert(candidate.BeginFrame(0));
    for(u32 i=0;i<AudioEvents::events_per_frame;++i)capture.stop_effect(0);
    assert(!candidate.Failed());capture.stop_effect(0);assert(candidate.Failed()&&!candidate.EndFrame());
    candidate.Reset();assert(candidate.BeginFrame(0));
    for(u32 i=0;i<1024;++i)capture.queue_music(2,0,long_name);
    assert(!candidate.Failed());capture.stop_effect(0);assert(candidate.Failed()&&!candidate.EndFrame());
    std::printf("bounded capacity: %zu bytes/frame, %zu ordinary events/frame, both overflow checks reject explicitly\n",AudioEvents::bytes_per_frame,AudioEvents::events_per_frame);
}
