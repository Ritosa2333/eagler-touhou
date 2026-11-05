#include "texture_edges.hpp"
#include <cstring>

namespace th20::source::sprite {
namespace {
std::uint16_t load_word(const void* address) {std::uint16_t value;std::memcpy(&value,address,2);return value;}
void store_word(void* address,std::uint16_t value) {std::memcpy(address,&value,2);}
}
namespace texture_edge_detail {
void accumulate_argb1555(std::uint32_t* rgb,const std::uint16_t* pixel,std::uint32_t& count) {
    if(!rgb) return;const auto value=load_word(pixel);
    if(value&0x8000) {rgb[0]+=(value>>10)&31;rgb[1]+=(value>>5)&31;rgb[2]+=value&31;++count;}
}
void accumulate_argb4444(std::uint32_t* rgb,const std::uint16_t* pixel,std::uint32_t& count) {
    if(!rgb) return;const auto value=load_word(pixel);
    if(value>>12) {rgb[0]+=(value>>8)&15;rgb[1]+=(value>>4)&15;rgb[2]+=value&15;++count;}
}
void accumulate_argb8332(std::uint32_t* rgb,const std::uint16_t* pixel,std::uint32_t& count) {
    if(!rgb) return;const auto value=load_word(pixel);
    if(value>>8) {rgb[0]+=(value>>5)&7;rgb[1]+=(value>>2)&7;rgb[2]+=value&3;++count;}
}
void accumulate_argb8888(std::uint32_t* rgb,const std::uint8_t* pixel,std::uint32_t& count) {
    if(rgb && pixel[3]) {rgb[0]+=pixel[2];rgb[1]+=pixel[1];rgb[2]+=pixel[0];++count;}
}
}
void repair_transparent_texels(DeviceTexture& texture) {
#ifdef TH_SDL3
    auto& device=web::shared_device();
    const auto locked=device.lock(&texture);
    if(!locked.pixels)return;
    const std::uint32_t width=device.texture_width(&texture),height=device.texture_height(&texture);
    const bool argb8888=true,argb1555=false,argb4444=false,argb8332=false;
    const auto pitch=locked.pitch;
    auto* memory=static_cast<std::uint8_t*>(locked.pixels);
#else
    IDirect3DSurface9* surface=nullptr;texture.GetSurfaceLevel(0,&surface);
    if(!surface) return;
    D3DSURFACE_DESC description;surface->GetDesc(&description);
    D3DLOCKED_RECT locked;surface->LockRect(&locked,nullptr,0);
    const auto format=description.Format;
    const bool argb8888=format==D3DFMT_UNKNOWN || format==D3DFMT_A8R8G8B8;
    const bool argb1555=format==D3DFMT_A1R5G5B5;
    const bool argb4444=format==D3DFMT_A4R4G4B4;
    const bool argb8332=format==D3DFMT_A8R3G3B2;
    const auto width=description.Width,height=description.Height;
    const auto pitch=locked.Pitch;
    auto* memory=static_cast<std::uint8_t*>(locked.pBits);
#endif
    if(argb8888 || argb1555 || argb4444 || argb8332) {
        const int bytes=argb8888?4:2;
        // Original addresses vertical neighbors through a signed pixel-pitch
        // division, truncating toward zero, and then scales back to bytes.
        const int vertical=(pitch/bytes)*bytes;
        for(std::uint32_t y=0;y<height;++y) {
            auto* pixel=memory+static_cast<std::int32_t>(static_cast<std::uint32_t>(pitch)*y);
            for(std::uint32_t x=0;x<width;++x,pixel+=bytes) {
                const auto value=argb8888?0:load_word(pixel);
                const bool transparent=argb8888?pixel[3]==0:argb1555?!(value&0x8000):argb4444?!(value>>12):!(value>>8);
                if(!transparent) continue;
                std::uint32_t rgb[3]{},count=0;
                const auto add=[&](const std::uint8_t* neighbor) {
                    using namespace texture_edge_detail;
                    if(argb8888) accumulate_argb8888(rgb,neighbor,count);
                    else if(argb1555) accumulate_argb1555(rgb,reinterpret_cast<const std::uint16_t*>(neighbor),count);
                    else if(argb4444) accumulate_argb4444(rgb,reinterpret_cast<const std::uint16_t*>(neighbor),count);
                    else accumulate_argb8332(rgb,reinterpret_cast<const std::uint16_t*>(neighbor),count);
                };
                if(x) add(pixel-bytes);
                if(x<width-1) add(pixel+bytes);
                if(y) add(pixel-vertical);
                if(y<height-1) add(pixel+vertical);
                if(count>1) for(auto& component:rgb) component/=count;
                if(argb8888) {pixel[2]=static_cast<std::uint8_t>(rgb[0]);pixel[1]=static_cast<std::uint8_t>(rgb[1]);pixel[0]=static_cast<std::uint8_t>(rgb[2]);}
                else if(argb1555) store_word(pixel,static_cast<std::uint16_t>((value&0x8000)|((rgb[0]&31)<<10)|((rgb[1]&31)<<5)|(rgb[2]&31)));
                else if(argb4444) store_word(pixel,static_cast<std::uint16_t>((value&0xf000)|((rgb[0]&15)<<8)|((rgb[1]&15)<<4)|(rgb[2]&15)));
                else store_word(pixel,static_cast<std::uint16_t>((value&0xff00)|((rgb[0]&7)<<5)|((rgb[1]&7)<<2)|(rgb[2]&3)));
            }
        }
    }
#ifdef TH_SDL3
    device.unlock(&texture);
#else
    surface->UnlockRect();surface->Release();
#endif
}
}
