#pragma once
#include <cstddef>
namespace th10::multiplayer {
// Native laser cancellation creates both a parent and child at each sample.
// The former 2048 overflow slots exhausted in stage-one Lunatic after a death.
// Keep storage, rollback coverage and canonical traversal on one capacity.
inline constexpr std::size_t kAnimationOverflowCapacity=8192;
}
