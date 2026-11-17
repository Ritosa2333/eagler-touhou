#pragma once
#include "Types.hpp"
#include <array>
namespace th11 {
struct Th16EffectLookup {virtual ~Th16EffectLookup()=default;virtual bool alive(u32)=0;};
struct Th16EffectSlots {
 i32 cursor=0;std::array<u32,1024> ids{};
 i32 choose(Th16EffectLookup&)noexcept;
};
}
