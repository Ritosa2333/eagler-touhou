#pragma once
#include "Arithmetic.hpp"
namespace th10 {
struct FilterWeight {u32 index;float weight;};
struct FilteredRow {float* rgba;u32 reserved,uses_remaining;};
struct FilterProgress {const u8* column;const float* source;};
// Operations on the original triangle filter's already-built coefficients.
// The filter-table construction and its boundary rules stay with the platform.
struct TriangleFilter {
    static FilterProgress accumulate(const u8* column,const u8* end,const FilterWeight* y_begin,const FilterWeight* y_end,FilteredRow* rows,const float* source,u32 source_width) noexcept;
    static bool saturate(float* rgba,u32 first,u32 width,const float* minimum) noexcept;
};
}
