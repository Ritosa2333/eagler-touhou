#pragma once
#include <cstdint>
namespace th20::source::replay {
// Non-owning views over the one actual Replay owner; its allocation/lifecycle
// is still a separate source dependency. These accessors create no storage.
bool is_playback() noexcept; //488800, owner+10==1
void* recording_stage(int) noexcept; //488740, owner+20[index]
const void* recorded_stage(int) noexcept; //488770, owner+e8[index*2c]+10
std::uint8_t inherited_stone(int) noexcept; //owner+1c -> header+ec[slot]
}
