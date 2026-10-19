#pragma once
#include "pool.hpp"
namespace th20::source::sprite {
void set_animation_interrupt(Animation&,std::int32_t); //4388f0
// 44ee90: current animation plus its immediate child chain. It does not recurse
// into grandchildren; the returned-iterator copy clears its initial observer
// links, an original quirk preserved for callbacks that mutate this chain.
void interrupt_animation_children(Controller&,std::uint32_t,std::int32_t);
}
