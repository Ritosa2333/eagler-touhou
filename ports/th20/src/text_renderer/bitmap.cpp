#include "bitmap.hpp"
#include <cstring>
#include <vector>
#include <memory_resource>
#include <utility>
#include <new>
#include <emmintrin.h>
namespace th20::source::text {
namespace {
constexpr BitmapFormat formats[]{
    {22,32,0,0xff0000,0xff00,0xff},{21,32,0xff000000,0xff0000,0xff00,0xff},
    {24,16,0,0x7c00,0x3e0,0x1f},{23,16,0,0xf800,0x7e0,0x1f},
    {25,16,0x8000,0x7c00,0x3e0,0x1f},{26,16,0xf000,0xf00,0xf0,0xf},{-1,0,0,0,0,0}
};
template<class T>T load(const std::uint8_t* at){T value;std::memcpy(&value,at,sizeof(value));return value;}
template<class T>void store(std::uint8_t* at,T value){std::memcpy(at,&value,sizeof(value));}
}
const BitmapFormat* bitmap_format(std::int32_t format) noexcept {
    auto* entry=formats;while(entry->format!=-1 && entry->format!=format)++entry;return format==-1?nullptr:entry;
}
Bitmap::Bitmap(){std::memset(this,0,sizeof(*this));format=-1;}
Bitmap::~Bitmap(){release();}
bool Bitmap::release() {
#ifdef TH_SDL3
    const bool existed=pixels!=nullptr;
    if(existed){delete[] pixels;format=-1;width=height=0;pixels=nullptr;}
    return existed;
#else
    const bool existed=dc!=nullptr;if(existed){SelectObject(dc,previous);DeleteDC(dc);DeleteObject(bitmap);format=-1;width=height=0;dc=nullptr;bitmap=nullptr;previous=nullptr;pixels=nullptr;}
    return existed;
#endif
}
bool Bitmap::create(std::int32_t requested_width,std::int32_t requested_height,std::int32_t requested_format) {
    release();auto* entry=bitmap_format(requested_format);if(!entry)return false;
    const int row_bytes=((requested_width*entry->bits)/8+3)/4*4;
#ifdef TH_SDL3
    // Plain top-down pixel store; the GDI DIB allocated requested_height+1
    // rows while reporting requested_height rows of image bytes.
    const std::uint32_t image=static_cast<std::uint32_t>(requested_height)*row_bytes;
    pixels=new(std::nothrow) std::uint8_t[static_cast<std::size_t>(requested_height+1)*row_bytes]();
    if(!pixels)return false;
    image_bytes=image;
#else
    BITMAPV4HEADER info{};
    info.bV4Size=sizeof(info);info.bV4Width=requested_width;info.bV4Height=-(requested_height+1);info.bV4Planes=1;
    info.bV4BitCount=static_cast<WORD>(entry->bits);info.bV4SizeImage=requested_height*row_bytes;
    if(requested_format!=24 && requested_format!=22){info.bV4V4Compression=BI_BITFIELDS;info.bV4RedMask=entry->red;info.bV4GreenMask=entry->green;info.bV4BlueMask=entry->blue;info.bV4AlphaMask=entry->alpha;}
    void* memory=nullptr;auto handle=CreateDIBSection(nullptr,reinterpret_cast<BITMAPINFO*>(&info),DIB_RGB_COLORS,&memory,nullptr,0);if(!handle)return false;
    auto context=CreateCompatibleDC(nullptr);auto old=SelectObject(context,handle);dc=context;bitmap=handle;pixels=static_cast<std::uint8_t*>(memory);image_bytes=info.bV4SizeImage;previous=old;
#endif
    width=requested_width;height=requested_height;format=requested_format;pitch=row_bytes;
    if(format==21)for(std::uint32_t offset=0;offset<image_bytes;offset+=4)store(pixels+offset,0xff000000u);
    else if(format==26)for(std::uint32_t offset=0;offset<image_bytes;offset+=2)store(pixels+offset,std::uint16_t(0xf000));return true;
}
bool Bitmap::create_with_fallback(std::int32_t requested_width,std::int32_t requested_height,std::int32_t requested_format) {
    if(create(requested_width,requested_height,requested_format))return true;
    if(requested_format==25||requested_format==26)return create(requested_width,requested_height,21);if(requested_format==23)return create(requested_width,requested_height,22);return false;
}
bool Bitmap::invert_alpha(std::int32_t rows,std::int32_t first_x,std::int32_t columns) {
    if(format==21){for(int y=0;y<rows;++y)for(int x=0;x<columns;++x)pixels[(y*width+first_x+x)*4+3]^=0xff;}
    else if(format==25){for(int offset=0;offset<pitch*rows;offset+=2){auto value=static_cast<std::uint16_t>(load<std::uint16_t>(pixels+offset)^0x8000);if(!(value&0x8000))value=0;store(pixels+offset,value);}}
    else if(format==26){for(int y=0;y<rows;++y)for(int x=0;x<columns;++x){auto* at=pixels+(y*width+first_x+x)*2;store(at,static_cast<std::uint16_t>(load<std::uint16_t>(at)^0xf000));}}
    else return false;return true;
}
void Bitmap::outline(std::int32_t rows,std::int32_t first_x,std::int32_t last_x,std::uint32_t color,float radius) {
    std::vector<std::uint8_t> original(pixels,pixels+pitch*rows);
    if(format!=21 && format!=26)return;
    const float radius_squared=_mm_cvtss_f32(_mm_mul_ss(_mm_set_ss(radius),_mm_set_ss(radius)));
    const int bound=_mm_cvttss_si32(_mm_set_ss(radius_squared));std::pmr::vector<std::pair<int,int>> offsets;
    for(int x=0;x<bound;++x)for(int y=0;y<bound;++y)if(static_cast<float>(x*x+y*y)<radius_squared && (x||y))offsets.emplace_back(x,y);
    const std::uint32_t transformed=color^0xff000000u;
    const auto packed=static_cast<std::uint16_t>(((color>>16)&0xf0)<<4|((color>>8)&0xf0)|(color&0xf0)>>4|(((color>>28)^0xf)<<12));
    for(int y=0;y<rows;++y)for(int x=first_x;x<last_x;++x){auto* destination=pixels+y*pitch+x*(format==21?4:2);
        if(format==21 && destination[3]==0)continue;
        bool edge=false;
        for(auto [dx,dy]:offsets){
            for(auto [nx,ny]:{std::pair{x-dx,y-dy},std::pair{x+dx,y-dy},std::pair{x-dx,y+dy},std::pair{x+dx,y+dy}}){
                if(nx<0||nx>=width||ny<0||ny>=rows)continue;
                const auto* neighbor=original.data()+ny*pitch+nx*(format==21?4:2);
                if(format==21?neighbor[3]!=255:(load<std::uint16_t>(neighbor)>>12)!=15){edge=true;break;}
            }if(edge)break;
        }
        if(format==21){if(edge)store(destination,transformed);else store(destination,(load<std::uint32_t>(destination)&0xff000000u)|(transformed&0xffffffu));}
        else {if(edge)store(destination,packed);else store(destination,static_cast<std::uint16_t>((load<std::uint16_t>(destination)&0xf000u)|(packed&0xfffu)));}
    }
}
}
