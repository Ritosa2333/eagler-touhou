#pragma once
#include "RasterImage.hpp"
#include "Rng.hpp"
namespace th10 {
struct RasterFormat {u32 format,bits,alpha,red,green,blue;};
struct BitmapDescription {
    u32 size;i32 width,height;u16 planes,bits;u32 compression,byte_size;
    i32 x_resolution,y_resolution;u32 colors,important_colors;
    u32 red,green,blue,alpha;u8 reserved[52];
};
static_assert(sizeof(BitmapDescription)==108&&sizeof(RasterFormat)==24);
struct RasterEnvironment {
    const RasterFormat* formats;
    virtual u32 select_object(u32 context,u32 object)=0;
    virtual void delete_context(u32 context)=0;
    virtual void delete_object(u32 object)=0;
    virtual u32 create_bitmap(const BitmapDescription& description,u8** pixels)=0;
    virtual u32 create_context()=0;
};
struct FontEnvironment : RasterEnvironment {
    const char* face;u32 charset;
    virtual u32 create_font(i32 height,const char* face,u32 charset)=0;
};
struct FontResources {
    RasterImage& bitmap;Rng& random;u32* fonts; // Ascending memory order, largest font first.
    void initialize(FontEnvironment& environment);
    void release(FontEnvironment& environment);
};
}
