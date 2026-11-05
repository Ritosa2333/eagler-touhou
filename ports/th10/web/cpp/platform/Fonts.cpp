#include "Fonts.hpp"
namespace th10::browser {
namespace {
constexpr RasterFormat formats[]={
    {22,32,0,0xff0000,0xff00,0xff},{21,32,0xff000000,0xff0000,0xff00,0xff},
    {24,16,0,0x7c00,0x3e0,0x1f},{23,16,0,0xf800,0x7e0,0x1f},
    {25,16,0x8000,0x7c00,0x3e0,0x1f},{26,16,0xf000,0xf00,0xf0,0xf},{0xffffffff,0,0,0,0,0}
};
}
Fonts::Fonts(FontHost& h,GraphicsDevice& d,Rng& rng,bool chinese):host(h),device(d),random(rng),textures(d,display){RasterEnvironment::formats=browser::formats;face=chinese?"SimHei":u8"ＭＳ ゴシック";charset=chinese?134:128;TextRasterEnvironment::bitmap=&image;TextRasterEnvironment::fonts=draw_handles;image.initialize();}
void Fonts::initialize(){FontResources{image,random,handles}.initialize(*this);for(u32 i=0;i<15;++i)draw_handles[i]=handles[14-i];}
void Fonts::release(){FontResources{image,random,handles}.release(*this);}
void Fonts::upload(void* surface,const TextureRect& destination,const RasterImage& bitmap,const TextureRect& source){
    const auto desc=textures.describe_surface(surface);const auto lock=textures.lock_surface(surface);
    PixelSurface output{desc.format,desc.width,desc.height,lock.pitch,lock.pixels},input{bitmap.format,static_cast<u32>(bitmap.width),static_cast<u32>(bitmap.height),bitmap.pitch,bitmap.pixels};
    TextureResample::triangle(output,destination,input,source);textures.unlock_surface(surface);
}
}
