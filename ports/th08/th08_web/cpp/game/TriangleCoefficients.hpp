#pragma once
#include "Arithmetic.hpp"
namespace th08 {
struct FilterWeight {u32 index;float weight;};
// Total byte count, then one length-prefixed contribution list per source
// pixel. The caller owns the returned allocation.
struct TriangleCoefficients {
    static u8* create(u32 source_size,u32 destination_size,bool wrap)noexcept;
};
}
