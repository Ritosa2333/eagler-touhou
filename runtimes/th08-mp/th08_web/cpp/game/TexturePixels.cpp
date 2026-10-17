#include "TexturePixels.hpp"
namespace th08 {
namespace {
u32 short_word(const u8* p) { return u32(p[0])|(u32(p[1])<<8); }
u32 word(const u8* p) { return short_word(p)|(short_word(p+2)<<16); }
u32 channel(u32 pixel,u32 mask,u32 shift,u32 destination) {
    if(!mask)return destination;
    return (((pixel>>shift)&mask)*destination+mask/2)/mask;
}
}
PixelFormat TexturePixels::describe(u32 value) {
    switch(value) {
    case 20:return {3,255,255,255,0,16,8,0,0};
    case 21:return {4,255,255,255,255,16,8,0,24};
    case 22:return {4,255,255,255,0,16,8,0,0};
    case 23:return {2,31,63,31,0,11,5,0,0};
    case 24:return {2,31,31,31,0,10,5,0,0};
    case 25:return {2,31,31,31,1,10,5,0,15};
    case 26:return {2,15,15,15,15,8,4,0,12};
    default:return {};
    }
}
u32 TexturePixels::anm_format(u32 value,bool force_16bit) {
    static constexpr u32 formats[]{0,21,25,23,20,26};
    if(value>=6)return 0;
    if(force_16bit) { if(value==0 || value==1)value=5; else if(value==4)value=3; }
    return formats[value];
}
bool TexturePixels::create(u32 w,u32 h,u32 pixel_format) {
    const auto description=describe(pixel_format);
    if(!w||!h||w>16384||h>16384||!description.bytes||u64(w)*h*description.bytes>256*1024*1024)return false;
    width=w;height=h;format=pixel_format;pixels.assign(w*h*description.bytes,0);
    return true;
}
bool TexturePixels::convert(u8* output,u32 output_format,const u8* source,u32 source_format,u32 count) {
    const auto in=describe(source_format),out=describe(output_format);
    if(!in.bytes||!out.bytes||!output||!source)return false;
    if(output_format==source_format) { std::memmove(output,source,u64(count)*in.bytes); return true; }
    for(u32 i=0;i<count;++i) {
        u32 pixel=0;std::memcpy(&pixel,source+i*in.bytes,in.bytes);
        const u32 value=(channel(pixel,in.red_mask,in.red_shift,out.red_mask)<<out.red_shift)|
                        (channel(pixel,in.green_mask,in.green_shift,out.green_mask)<<out.green_shift)|
                        (channel(pixel,in.blue_mask,in.blue_shift,out.blue_mask)<<out.blue_shift)|
                        (channel(pixel,in.alpha_mask,in.alpha_shift,out.alpha_mask)<<out.alpha_shift);
        std::memcpy(output+i*out.bytes,&value,out.bytes);
    }
    return true;
}
bool TexturePixels::from_anm(const u8* bytes,u32 size,u32 requested_format,bool force_16bit) {
    if(!bytes||size<16||std::memcmp(bytes,"THTX",4))return false;
    const u32 w=short_word(bytes+8),h=short_word(bytes+10),source_format=anm_format(short_word(bytes+6),false);
    const u32 count_bytes=word(bytes+12),source_bytes=describe(source_format).bytes;
    if(!source_bytes||count_bytes>size-16||u64(w)*h*source_bytes>count_bytes)return false;
    u32 destination=anm_format(requested_format,force_16bit);
    if(requested_format==0&&!force_16bit)destination=21;
    if(!create(w,h,destination))return false;
    return convert(pixels.data(),format,bytes+16,source_format,w*h);
}
bool TexturePixels::from_rgba(const u8* rgba,u32 w,u32 h,u32 pixel_format) {
    const auto out=describe(pixel_format);
    if(!rgba||!w||!h||!out.bytes||u64(w)*h>256*1024*1024)return false;
    if(!create(w,h,pixel_format))return false;
    const auto scale=[](u32 value,u32 mask){return mask?u32((u64(value)*mask+127)/255):0;};
    for(u64 i=0;i<u64(w)*h;++i) {
        const u8 red=rgba[i*4],green=rgba[i*4+1],blue=rgba[i*4+2],alpha=rgba[i*4+3];
        const u32 value=(scale(red,out.red_mask)<<out.red_shift)|
                        (scale(green,out.green_mask)<<out.green_shift)|
                        (scale(blue,out.blue_mask)<<out.blue_shift)|
                        (out.alpha_mask?(scale(alpha,out.alpha_mask)<<out.alpha_shift):0u);
        std::memcpy(pixels.data()+i*out.bytes,&value,out.bytes);
    }
    return true;
}
std::vector<u8> TexturePixels::rgba() const {
    const auto source=describe(format);
    if(!source.bytes||pixels.size()!=u64(width)*height*source.bytes)return {};
    std::vector<u8> output(u64(width)*height*4);
    for(u32 i=0;i<width*height;++i) {
        u32 pixel=0;std::memcpy(&pixel,pixels.data()+i*source.bytes,source.bytes);
        output[i*4]=channel(pixel,source.red_mask,source.red_shift,255);
        output[i*4+1]=channel(pixel,source.green_mask,source.green_shift,255);
        output[i*4+2]=channel(pixel,source.blue_mask,source.blue_shift,255);
        output[i*4+3]=channel(pixel,source.alpha_mask,source.alpha_shift,255);
    }
    return output;
}
}
