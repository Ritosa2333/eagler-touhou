#pragma once
#include "../../cpp/game/SoundEffects.hpp"
#include <vector>
namespace th11::test {
struct SoundFixture:SoundOutput {
    struct Call{i32 op,id,value;};std::vector<Call> calls;SoundEffects effects{*this};
    SoundFixture(){for(u32 n=0;n<56;++n)effects.buffers[n]=n+1;}
    void sound_stop(u32 n)override{calls.push_back({0,i32(n),0});}
    void sound_position(u32 n,u32 v)override{calls.push_back({1,i32(n),i32(v)});}
    void sound_pan(u32 n,i32 v)override{calls.push_back({2,i32(n),v});}
    void sound_volume(u32 n,i32 v)override{calls.push_back({3,i32(n),v});}
    void sound_play(u32 n)override{calls.push_back({4,i32(n),0});}
};
}
