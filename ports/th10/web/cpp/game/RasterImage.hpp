#pragma once
#include "Types.hpp"
namespace th10 {
struct RasterEnvironment;
struct RasterImage {
    u8 resource_fields[0x100];
    u32 format;
    i32 width,height;
    i32 byte_size;
    i32 pitch;
    u32 device_context;
    u32 previous_bitmap,bitmap_handle;
    u8* pixels;
    void initialize() noexcept;
    bool release(RasterEnvironment& environment);
    bool create(i32 width,i32 height,u32 format,RasterEnvironment& environment);
    bool create_compatible(i32 width,i32 height,u32 format,RasterEnvironment& environment);
    bool invert_alpha(i32 rows) noexcept;
    bool fill_transparent_edges(u32 rows) noexcept;
    void fill_text_background(u32 color) noexcept;
};
static_assert(offsetof(RasterImage,pixels)==0x120);
}
