#pragma once
#include "TriangleFilter.hpp"
namespace th10 {
// Forward contribution lists: total byte count, then one length-prefixed list
// of {destination index, weight} per source pixel. Caller owns the allocation.
struct TriangleCoefficients {
    static u8* create(u32 source_size,u32 destination_size,bool wrap) noexcept;
};
}
