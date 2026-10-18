#pragma once
#include "../../cpp/game/SoundEffects.hpp"
#include <array>
#include <vector>
namespace sound_test {
using namespace th09;
struct Fixture:SoundOutput {
    SoundEffects sound{*this};std::vector<std::array<i32,3>> calls;
    Fixture(){for(u32 n=0;n<54;++n)sound.buffers[n]=n+1;}
    void sound_stop(u32 n)override{calls.push_back({0,i32(n),0});}
    void sound_position(u32 n,u32 x)override{calls.push_back({1,i32(n),i32(x)});}
    void sound_pan(u32 n,i32 x)override{calls.push_back({2,i32(n),x});}
    void sound_volume(u32 n,i32 x)override{calls.push_back({3,i32(n),x});}
    void sound_play(u32 n)override{calls.push_back({4,i32(n),0});}
};
}
