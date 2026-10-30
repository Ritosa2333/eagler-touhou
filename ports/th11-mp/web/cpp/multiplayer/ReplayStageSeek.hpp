#pragma once
#ifndef TH11_MULTIPLAYER
#error Multiplayer chapter seeking belongs to the MP runtime
#endif
#include <cstdint>
namespace th11::multiplayer {
inline bool ReplayStageSeeking(std::uint32_t cursor,std::uint32_t target,std::uint32_t count,
                               std::uint32_t selected,std::uint32_t current,bool active){
    if(cursor>=count)return false;
    if(cursor<target)return true;
    // Chapter labels change when loading starts. Native activation follows
    // after the result delay and transition, before the selected stage input.
    return selected&&(current<selected||(current==selected&&!active));
}
}
