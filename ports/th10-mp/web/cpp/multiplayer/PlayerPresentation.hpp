#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer presentation helpers must not enter an ordinary build
#endif
#include "../game/Types.hpp"
#include <algorithm>
#include <cmath>

namespace th10::multiplayer {
constexpr float kRemotePlayerFadeStartDistance = 100.0f;
constexpr float kRemotePlayerFadeFullDistance = 50.0f;
constexpr u8 kRemotePlayerFadeMinAlpha = 51;

inline u8 player_proximity_alpha(float dx,float dy) {
    float distance=std::sqrt(dx*dx+dy*dy);
    if(distance>=kRemotePlayerFadeStartDistance)return 128;
    if(distance<kRemotePlayerFadeFullDistance)distance=kRemotePlayerFadeFullDistance;
    const float progress=(distance-kRemotePlayerFadeFullDistance)/
        (kRemotePlayerFadeStartDistance-kRemotePlayerFadeFullDistance);
    return static_cast<u8>(std::clamp<int>(
        static_cast<int>(progress*(255-kRemotePlayerFadeMinAlpha))+kRemotePlayerFadeMinAlpha,0,128));
}

inline u8 clamp_player_alpha(u8 authored,u8 proximity) {
    return authored<proximity?authored:proximity;
}
}
