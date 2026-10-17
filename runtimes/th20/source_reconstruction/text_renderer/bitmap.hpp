#pragma once
#define NOMINMAX
#include <Windows.h>
#include <cstdint>
namespace th20::source::text {
struct BitmapFormat {std::int32_t format,bits;std::uint32_t alpha,red,green,blue;};
const BitmapFormat* bitmap_format(std::int32_t) noexcept; //415640
struct Bitmap {
    std::uint8_t reserved[256];
    std::int32_t format,width,height;
    std::uint32_t image_bytes;
    std::int32_t pitch;
    HDC dc;HGDIOBJ previous;HBITMAP bitmap;std::uint8_t* pixels;
    Bitmap();                                           //414200
    ~Bitmap();
    bool release();                                     //416360
    bool create(std::int32_t width,std::int32_t height,std::int32_t format); //415120
    bool create_with_fallback(std::int32_t,std::int32_t,std::int32_t); //4153d0
    bool invert_alpha(std::int32_t rows,std::int32_t first_x,std::int32_t columns); //417ab0 AL
    void outline(std::int32_t rows,std::int32_t first_x,std::int32_t last_x,std::uint32_t color,float radius); //416420
};
static_assert(sizeof(Bitmap)==0x124);
}
