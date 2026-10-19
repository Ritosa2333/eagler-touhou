#include "TextureImage.hpp"
namespace th15 {
namespace {
using touhou::graphics::PixelFormat;
struct Format {PixelFormat storage;u32 bytes,masks[4],shifts[4];};
constexpr Format formats[]={
    {PixelFormat::Bgra8,4,{255,255,255,255},{0,8,16,24}},
    {PixelFormat::Bgra8,4,{255,255,255,255},{0,8,16,24}},
    {PixelFormat::Argb1555,2,{31,31,31,1},{0,5,10,15}},
    {PixelFormat::Rgb565,2,{31,63,31,0},{0,5,11,0}},
    {PixelFormat::Bgr8,3,{255,255,255,0},{0,8,16,0}},
    {PixelFormat::Argb4444,2,{15,15,15,15},{0,4,8,12}},
    {PixelFormat::Argb8332,2,{3,7,7,255},{0,2,5,8}},
    {PixelFormat::Alpha8,1,{0,0,0,255},{0,0,0,0}}
};
}
bool TextureImage::load(const AnmTexture& source,bool low_color){
    if(source.format>=8||source.kind==AnmTexture::Kind::External)return false;
    u32 target=source.format;const bool embedded=source.kind==AnmTexture::Kind::Embedded;
    if(low_color&&embedded){if(target<2)target=5;else if(target==4)target=3;}
    const auto& output=formats[target];const u32 w=source.width,h=source.height;
    if(!w||!h||w>16384||h>16384||u64(w)*h*output.bytes>256*1024*1024)return false;
    if(embedded&&(source.pixel_format>=8||!source.pixel_width||!source.pixel_height||source.pixel_width>w||source.pixel_height>h||u64(source.pixel_width)*source.pixel_height*formats[source.pixel_format].bytes!=source.pixels.size()))return false;
    std::vector<u8> next(size_t(w)*h*output.bytes,0);
    if(embedded){const auto& input=formats[source.pixel_format];
        for(u32 y=0;y<source.pixel_height;++y){const auto* from=source.pixels.data()+size_t(y)*source.pixel_width*input.bytes;auto* to=next.data()+size_t(y)*w*output.bytes;
            if(input.storage==output.storage)std::memcpy(to,from,source.pixel_width*input.bytes);
            else for(u32 x=0;x<source.pixel_width;++x){u32 value=0,result=0;std::memcpy(&value,from+x*input.bytes,input.bytes);
                for(u32 channel=0;channel<4;++channel){const u32 mask=input.masks[channel],dest=output.masks[channel];const u32 converted=mask?((((value>>input.shifts[channel])&mask)*dest+mask/2)/mask):(source.pixel_format==7&&channel<3?0:dest);result|=converted<<output.shifts[channel];}
                std::memcpy(to+x*output.bytes,&result,output.bytes);
            }
        }
    }
    width=w;height=h;pitch=w*output.bytes;packed_format=u16(target);format=output.storage;pixels=std::move(next);return true;
}
}
