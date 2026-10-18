#include "../../cpp/multiplayer/FrameAudio.hpp"
#include "../../cpp/game/SoundEffects.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#define CHECK(x) do {if(!(x)){std::fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);std::abort();}}while(0)
using th09::multiplayer::FrameAudio;
struct Output:th09::SoundOutput {
    std::array<unsigned,55> played{};std::array<int,55> pans{};
    void sound_stop(th09::u32)override{}
    void sound_position(th09::u32,th09::u32)override{}
    void sound_pan(th09::u32 id,th09::i32 pan)override{pans[id]=pan;}
    void sound_volume(th09::u32,th09::i32)override{}
    void sound_play(th09::u32 id)override{++played[id];}
};
int main(){
    Output output;th09::SoundEffects effects(output);for(unsigned i=0;i<54;++i)effects.buffers[i]=i+1;
    const auto emit=[&](const auto& e){if(e.kind==0)effects.enqueue(e.id,int(e.value));else effects.positioned(e.id,e.value);};
    FrameAudio frame;
    std::vector<FrameAudio::Event> first={{0,0,100},{0,0,300},{1,2,0},{1,4,0},{0,48,0},{0,49,0},{2,12,0}};
    frame.Emit(first,false,emit);effects.process();
    CHECK(output.played[1]==1&&output.pans[1]==200);CHECK(output.played[3]==1);
    CHECK(!output.played[5]&&!output.played[49]&&!output.played[50]);CHECK(!output.played[13]);
    // Even if a caller repeats forward publication, a frame cannot restart a
    // heard effect. First-pass simultaneous requests still average normally.
    frame.Emit(first,false,emit);effects.process();CHECK(output.played[1]==1&&output.played[3]==1);
    frame.Emit(first,true,emit);effects.process();
    CHECK(output.played[1]==1&&output.played[3]==1);CHECK(output.played[5]==1&&output.played[49]==1&&output.played[50]==1);
    // A corrected history introduces a new effect, changes an old pan and
    // removes another event. Only the previously unheard corrected ID plays.
    std::vector<FrameAudio::Event> corrected={{0,0,-500},{0,3,0},{1,4,0}};
    frame.Emit(corrected,true,emit);effects.process();
    CHECK(output.played[1]==1&&output.pans[1]==200);CHECK(output.played[4]==1&&output.played[5]==1);
    frame.Emit(corrected,true,emit);effects.process();CHECK(output.played[4]==1);
    // Ring reuse/new epochs reset output ownership. A later logical frame
    // with the same ID is a distinct sound, not suppressed across time.
    FrameAudio next;next.Emit({{0,0,0}},false,emit);effects.process();CHECK(output.played[1]==2);
    frame.Clear();frame.Emit({{0,0,0}},false,emit);effects.process();CHECK(output.played[1]==3);
    frame.Emit({{0,-1,0},{0,54,0},{4,0,0},{5,0,0},{6,0,0}},true,emit);effects.process();CHECK(output.played[1]==3);
    std::puts("Frame audio: immediate ordinary feedback, original pan averaging, confirmed outcomes, corrected new effects, dedup and ring reuse PASS");
}
