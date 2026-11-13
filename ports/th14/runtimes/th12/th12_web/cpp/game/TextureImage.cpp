#include "TextureImage.hpp"
#include <cstdio>
#include <utility>
#if PORT_TITLE == 14
#include "ImageResample.hpp"
#endif
namespace th11 {
namespace {
using touhou::graphics::PixelFormat;
// Layout of an ANM texture's embedded pixels, indexed by the header's texture format.
// `reference/thtk/thanm/anm_types.h` names the enum as 1 = BGRA8888, 3 = RGB565,
// 5 = ARGB4444 and 7 = GRAY8; 2, 4 and 6 are not in it. 2 and 4 are the older
// 16/24-bit variants still seen in th06-th09 archives. **6 is TH13's**: of the 718
// texture records the two titles ship (`tools/th-anm-texture-table.py`), every TH12 one
// uses 1, 3, 5 or 7 - the value at +14 equals the THTX pixel format at +6 - and exactly
// one TH13 one differs: `enemy/enemy_aura.png` in `enemy.anm` declares 6 while its
// embedded pixels are THTX format 1, 4 bytes each, 512x48 inside a 512x64 surface. Those
// pixels are grey in every one of their 24576 entries with their alpha in the fourth
// byte, so the value means a 32-bit surface sampled as they are stored: the same row as
// 1. `bytes == 0` marks a format the runtime cannot sample.
struct Format {PixelFormat storage;u32 bytes,masks[4],shifts[4];};
constexpr Format formats[]={
    {PixelFormat::Bgra8,4,{255,255,255,255},{0,8,16,24}},   // 0
    {PixelFormat::Bgra8,4,{255,255,255,255},{0,8,16,24}},   // 1 BGRA8888
    {PixelFormat::Argb1555,2,{31,31,31,1},{0,5,10,15}},     // 2 legacy 16-bit
    {PixelFormat::Rgb565,2,{31,63,31,0},{0,5,11,0}},        // 3 RGB565
    {PixelFormat::Bgr8,3,{255,255,255,0},{0,8,16,0}},       // 4 legacy 24-bit
    {PixelFormat::Argb4444,2,{15,15,15,15},{0,4,8,12}},     // 5 ARGB4444
    {PixelFormat::Bgra8,4,{255,255,255,255},{0,8,16,24}},   // 6 TH13's 32-bit surface, see above
#if PORT_TITLE == 14
    {PixelFormat::Alpha8,1,{0,0,0,255},{0,0,0,0}}          // TH14: D3DFMT_A8
#else
    {PixelFormat::Bgra8,1,{255,255,255,0},{0,0,0,0}}        // 7 GRAY8
#endif
};
constexpr u32 format_count=sizeof(formats)/sizeof(formats[0]);
}
bool TextureImage::load(const AnmTexture& source,bool low_color){
#if PORT_TITLE == 14
    return load(source,low_color,false);
}
bool TextureImage::load(const AnmTexture& source,bool low_color,bool high_precision){
#endif
    failure.clear();
    char buffer[256];
    auto describe=[&](){
        std::snprintf(buffer,sizeof buffer,
            "format=%u kind=%u %ux%u pixel_format=%u pixel=%ux%u pixel_bytes=%llu",
            (unsigned)source.format,(unsigned)source.kind,source.width,source.height,
            (unsigned)source.pixel_format,source.pixel_width,source.pixel_height,
            (unsigned long long)source.pixels.size());
        return buffer;
    };
    auto reject=[&](const char* why){failure=std::string(why)+" ["+describe()+"]";return false;};
    if(source.format>=format_count||!formats[source.format].bytes)return reject("texture format out of range");
    if(source.kind==AnmTexture::Kind::External)return reject("external texture has no embedded image");
    u32 target=source.format;const bool embedded=source.kind==AnmTexture::Kind::Embedded;
    if(low_color&&embedded){if(target<2)target=5;else if(target==4)target=3;}
    // GRAY8 (7) stores one luminance byte per pixel. It cannot be sampled as-is,
    // so it expands to BGRA8888 with opaque alpha, the same interpretation
    // AnmTexture::rgba gives it. Every other format renders in its own layout.
    const bool gray=target==7&&embedded
#if PORT_TITLE == 14
        &&false
#endif
        ;
    const auto& output=formats[gray?1:target];
    const u32 w=source.width,h=source.height,out_bytes=gray?4u:output.bytes;
    if(!w||!h)return reject("zero-sized texture");
    if(w>16384||h>16384)return reject("texture larger than 16384");
    if(u64(w)*h*out_bytes>256*1024*1024)return reject("texture larger than 256 MiB");
    if(embedded){
        if(source.pixel_format>=format_count||!formats[source.pixel_format].bytes)return reject("embedded pixel format out of range");
        if(!source.pixel_width||!source.pixel_height)return reject("embedded pixel dimensions unset");
        if(source.pixel_width>w||source.pixel_height>h)return reject("embedded pixels exceed texture dimensions");
        const u64 expected=u64(source.pixel_width)*source.pixel_height*formats[source.pixel_format].bytes;
        if(expected!=source.pixels.size()){
            char detail[128];
            std::snprintf(detail,sizeof detail,"embedded pixel data is %llu bytes, expected %llu",
                          (unsigned long long)source.pixels.size(),(unsigned long long)expected);
            return reject(detail);
        }
    }
#if PORT_TITLE == 14
    if(embedded&&source.half_resolution){
        // Retail47cb70: at the fixed640x480 logical scale1, flag+34 halves
        // allocation and upload rectangles and selects TRIANGLE|MIRROR_U|MIRROR_V.
        const u32 tw=w/2,th=h/2,pw=source.pixel_width/2,ph=source.pixel_height/2;
        if(!tw||!th||!pw||!ph)return reject("half-resolution texture is empty");
        TextureImage input,filtered;const auto& native_input=formats[source.pixel_format];
        input.width=source.pixel_width;input.height=source.pixel_height;input.pitch=input.width*native_input.bytes;
        input.format=native_input.storage;input.pixels=source.pixels;
        filtered.width=tw;filtered.height=th;filtered.pitch=tw*out_bytes;filtered.format=output.storage;
        filtered.packed_format=u16(target);filtered.pixels.resize(size_t(filtered.pitch)*th,0);
        if(!ImageResample::triangle_mirror(filtered,{0,0,i32(pw),i32(ph)},input,{0,0,i32(input.width),i32(input.height)},high_precision))return reject("half-resolution image filter failed");
        *this=std::move(filtered);return true;
    }
#endif
    std::vector<u8> next(size_t(w)*h*out_bytes,0);
    if(gray){
        for(u32 y=0;y<source.pixel_height;++y){
            const auto* from=source.pixels.data()+size_t(y)*source.pixel_width;
            auto* to=next.data()+size_t(y)*w*4;
            for(u32 x=0;x<source.pixel_width;++x){const u8 v=from[x];to[x*4]=v;to[x*4+1]=v;to[x*4+2]=v;to[x*4+3]=255;}
        }
    } else if(embedded){const auto& input=formats[source.pixel_format];
        for(u32 y=0;y<source.pixel_height;++y){const auto* from=source.pixels.data()+size_t(y)*source.pixel_width*input.bytes;auto* to=next.data()+size_t(y)*w*out_bytes;
            if(input.storage==output.storage)std::memcpy(to,from,source.pixel_width*input.bytes);
            else for(u32 x=0;x<source.pixel_width;++x){u32 value=0,result=0;std::memcpy(&value,from+x*input.bytes,input.bytes);
                for(u32 channel=0;channel<4;++channel){const u32 mask=input.masks[channel],dest=output.masks[channel];const u32 converted=mask?((((value>>input.shifts[channel])&mask)*dest+mask/2)/mask):dest;result|=converted<<output.shifts[channel];}
                std::memcpy(to+x*out_bytes,&result,out_bytes);
            }
        }
    }
    width=w;height=h;pitch=w*out_bytes;packed_format=u16(target);format=output.storage;pixels=std::move(next);return true;
}
}
