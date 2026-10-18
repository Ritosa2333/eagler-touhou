#pragma once
#include "bitmap.hpp"
#include "../program_entry/device.hpp"
#include <functional>
#include <memory>
namespace th20::source::text {
extern float next_outline_scale; //5ae120, initialized to1; consumed by415830
void set_next_outline_scale(float) noexcept; //416cf0
struct RasterizedText {
    std::unique_ptr<Bitmap> bitmap;
    SIZE extent;
    RECT destination,source;
};
// The GDI half of415830; split at its existing deferred-queue boundary.
RasterizedText rasterize_text(const RECT&,std::int32_t x,std::uint32_t foreground,
    std::uint32_t background,const char* cp932,std::int32_t font,std::int32_t spacing,bool outline);
void upload_text_bitmap(Bitmap*,DeviceTexture&,const RECT&,const RECT&,
                        std::uint8_t* ready,const std::function<void()>&); //4145d0, owns Bitmap
SIZE queue_text(const RECT&,std::int32_t x,std::uint32_t foreground,std::uint32_t background,
    const char* cp932,DeviceTexture&,std::int32_t font,std::int32_t spacing,bool outline,
    std::uint8_t* ready,std::function<void()> completion); //415830
}
