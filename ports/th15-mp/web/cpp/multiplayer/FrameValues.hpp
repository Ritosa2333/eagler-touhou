#pragma once
#include "../game/AnmVm.hpp"
#include <tuple>
namespace th15::multiplayer::frame_values {
template<class... T>auto copy(const std::tuple<T&...>& fields){return std::apply([](const auto&... v){return std::make_tuple(v...);},fields);}
// Deep overlays construct VMs when copied. A frame copy must not advance the
// live presentation identity allocator.
struct IdentityGuard {u64 value=AnmVm::presentation_counter;~IdentityGuard(){AnmVm::presentation_counter=value;}};
}
