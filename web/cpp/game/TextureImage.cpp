#include "TextureImage.hpp"
namespace th13 {
namespace {
using touhou::graphics::PixelFormat;
struct Format {PixelFormat storage;u32 bytes,masks[4],shifts[4];};
constexpr Format formats[]={
    {PixelFormat::Bgra8,4,{255,255,255,255},{16,8,0,24}},
    {PixelFormat::Bgra8,4,{255,255,255,255},{16,8,0,24}},
    {PixelFormat::Argb1555,2,{31,31,31,1},{10,5,0,15}},
    {PixelFormat::Rgb565,2,{31,63,31,0},{11,5,0,0}},
    {PixelFormat::Bgr8,3,{255,255,255,0},{16,8,0,0}},
    {PixelFormat::Argb4444,2,{15,15,15,15},{8,4,0,12}},
    {PixelFormat::Argb8332,2,{7,7,3,255},{5,2,0,8}},
    {PixelFormat::Alpha8,1,{0,0,0,255},{0,0,0,0}}
};
}
// Blank "@" 纹理原版创建路径 0x46dc10：D3DXCreateTexture(device, entry.w,
// entry.h, 1 mip, 0, DAT_004acbc4[entry.format], D3DPOOL_MANAGED, &tex)，
// 不写像素；0x46cf10 只服务文件纹理（0x46d600/0x46d8f0），不参与 Blank。
// 故全零缓冲即等价（text.anm entry1 format3=R5G6B5 → 不透明黑；entry0
// format5=ARGB4444 → 透明，由对话写入）。全库 6 个 Blank 纹理（text.anm
// 0/1/2/3——entry2/3 为 640×480，即 0x4418b0 另两个调用方 0x43e5a0/
// 0x41930 的目标；help.anm 1、title.anm 21）均为运行时写入目标，无白色
// 实心 quad 用途；不要改成白色。
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
