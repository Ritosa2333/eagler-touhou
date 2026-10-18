#include "TextureImage.hpp"
namespace th09 {
namespace {
using touhou::graphics::PixelFormat;
struct Format {PixelFormat storage;u32 bytes;u32 masks[4],shifts[4];};
constexpr Format formats[]{
    {PixelFormat::Bgra8,4,{255,255,255,255},{0,8,16,24}},
    {PixelFormat::Bgra8,4,{255,255,255,255},{0,8,16,24}},
    {PixelFormat::Argb1555,2,{31,31,31,1},{0,5,10,15}},
    {PixelFormat::Rgb565,2,{31,63,31,0},{0,5,11,0}},
    {PixelFormat::Bgr8,3,{255,255,255,0},{0,8,16,0}},
    {PixelFormat::Argb4444,2,{15,15,15,15},{0,4,8,12}}
};
}
bool TextureImage::load(const AnmTextureSource& source,const u8* data,u32 size,bool low_color){
    if(source.format>=6||(!source.embedded&&!source.empty))return false;
    u32 target=source.format;if(low_color&&source.embedded){if(target<2)target=5;else if(target==4)target=3;}
    const auto& out=formats[target];const u32 w=source.embedded?source.pixel_width:source.width,h=source.embedded?source.pixel_height:source.height;
    if(!w||!h||w>16384||h>16384||u64(w)*h*out.bytes>256*1024*1024)return false;
    if(source.embedded&&(source.pixel_format>=6||!data||u64(w)*h*formats[source.pixel_format].bytes>size))return false;
    std::vector<u8> next(u64(w)*h*out.bytes,0);
    if(source.embedded){const auto& in=formats[source.pixel_format];
        if(in.storage==out.storage)std::memcpy(next.data(),data,next.size());
        else for(u32 n=0;n<w*h;++n){u32 pixel=0,value=0;std::memcpy(&pixel,data+u64(n)*in.bytes,in.bytes);
            for(u32 channel=0;channel<4;++channel){const u32 mask=in.masks[channel],target_mask=out.masks[channel];const u32 result=mask?((((pixel>>in.shifts[channel])&mask)*target_mask+mask/2)/mask):target_mask;value|=result<<out.shifts[channel];}
            std::memcpy(next.data()+u64(n)*out.bytes,&value,out.bytes);
        }
    }
    width=w;height=h;pitch=w*out.bytes;format=out.storage;pixels=std::move(next);return true;
}
}
