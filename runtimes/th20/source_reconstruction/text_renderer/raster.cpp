#include "raster.hpp"
#include "deferred_queue.hpp"
#include "../platform_window/fonts.hpp"
#include "../runtime_core/runtime_core.hpp"
#ifdef TH_SDL3
#include "platform/Fonts.hpp"
#endif
#include <cwchar>
#include <cstring>
#include <mutex>
#include <emmintrin.h>
namespace th20::source::text {
float next_outline_scale=1.f;
void set_next_outline_scale(float value) noexcept {next_outline_scale=value;}
RasterizedText rasterize_text(const RECT& rectangle,std::int32_t x,std::uint32_t foreground,
    std::uint32_t background,const char* string,std::int32_t font,std::int32_t spacing,bool outline) {
    std::lock_guard lock(runtime::shared_locks().slot(18));
    float radius=2.f;int top=3;
    auto bitmap=std::make_unique<Bitmap>();
    switch(font){case 4:case 5:radius=2.5f;break;case 6:case 8:radius=3;break;case 9:radius=3.5f;break;
    case 10:radius=4;top=4;break;case 12:case 13:top=6;break;case 14:case 15:radius=3;top=6;break;
    case 16:case 17:radius=4;top=6;break;case 18:case 19:top=2;break;case 20:case 21:radius=3;top=7;break;}
    radius=_mm_cvtss_f32(_mm_mul_ss(_mm_set_ss(radius),_mm_set_ss(next_outline_scale)));next_outline_scale=1.f;
    const int margin=_mm_cvttss_si32(_mm_set_ss(radius));
    const int width=rectangle.right-rectangle.left+3+margin,height=rectangle.bottom-rectangle.top+3+margin;
    bitmap->create_with_fallback(width,height+6,21);
    const auto color=RGB((foreground>>16)&0xff,(foreground>>8)&0xff,foreground&0xff);
    SIZE extent{};
    std::function<void()> draw;
#ifdef TH_SDL3
    const auto& slot=platform_window::font_slots[font];
    {int cx=0,cy=0;web::fonts::measure(string,slot.height,slot.family,cx,cy);extent.cx=cx;extent.cy=cy;}
    draw=[&]{web::fonts::draw(bitmap->pixels,bitmap->pitch,height+6,spacing?x:x+2,top,string,
        slot.height,slot.family,color,spacing);};
#else
    const auto selected_font=platform_window::fonts[font];auto dc=bitmap->dc;auto old=SelectObject(dc,selected_font);
    wchar_t converted[257]{};MultiByteToWideChar(932,0,string,-1,converted,256);
    SetBkMode(dc,TRANSPARENT);const auto count=static_cast<int>(std::wcslen(converted));
    GetTextExtentPoint32W(dc,converted,count,&extent);SetTextColor(dc,color);
    draw=[&]{if(!spacing)TextOutW(dc,x+2,top,converted,count);else{int at=x;for(int i=0;i<count;++i){const wchar_t character[]{converted[i],0};TextOutW(dc,at,top,character,1);at+=spacing;}}};
#endif
    if(outline){draw();bitmap->outline(height+6,0,width,background,radius);}draw();
#ifndef TH_SDL3
    SelectObject(dc,old);
#endif
    bitmap->invert_alpha(height+6,0,width);
#ifndef TH_SDL3
    SelectObject(dc,old);
#endif
    RECT source{0,0,rectangle.right-rectangle.left,rectangle.bottom-rectangle.top};
    if(source.right>1024)source.right=1024;
    if(platform_window::font_available[0]&&font!=18&&font!=19){source.top=6;source.bottom+=6;}
    return {std::move(bitmap),extent,rectangle,source};
}
void upload_text_bitmap(Bitmap* bitmap,DeviceTexture& texture,const RECT& destination,
                        const RECT& source,std::uint8_t* ready,const std::function<void()>& completion) {
#ifdef TH_SDL3
    auto& device=web::shared_device();
    const auto lock=device.lock(&texture,reinterpret_cast<const std::int32_t*>(&destination.left)); // RECT = 4 contiguous i32
    auto* out=static_cast<std::uint8_t*>(lock.pixels);
    auto* in=bitmap->pixels+bitmap->pitch*source.top;
    //4145d0 adds source.left as a BYTE offset; normal producers always use0.
    for(int y=0;y<destination.bottom-destination.top;++y){std::memcpy(out,in+source.left,(source.right-source.left)*4);out+=lock.pitch;in+=bitmap->pitch;}
    device.unlock(&texture);
#else
    IDirect3DSurface9* surface;texture.GetSurfaceLevel(0,&surface);D3DLOCKED_RECT lock;
    surface->LockRect(&lock,&destination,0);auto* out=static_cast<std::uint8_t*>(lock.pBits);
    auto* in=bitmap->pixels+bitmap->pitch*source.top;
    //4145d0 adds source.left as a BYTE offset; normal producers always use0.
    for(int y=0;y<destination.bottom-destination.top;++y){std::memcpy(out,in+source.left,(source.right-source.left)*4);out+=lock.Pitch;in+=bitmap->pitch;}
    surface->UnlockRect();if(surface)surface->Release();
#endif
    bitmap->release();
    {bitmap->~Bitmap();std::lock_guard allocation(runtime::shared_locks().slot(1));::operator delete(bitmap);}
    if(ready)*ready=1;if(completion)completion();
}
SIZE queue_text(const RECT& rectangle,std::int32_t x,std::uint32_t foreground,std::uint32_t background,
    const char* string,DeviceTexture& texture,std::int32_t font,std::int32_t spacing,bool outline,
    std::uint8_t* ready,std::function<void()> completion) {
    std::lock_guard consumer(runtime::shared_locks().slot(18));
    auto data=rasterize_text(rectangle,x,foreground,background,string,font,spacing,outline);
    std::lock_guard producer(runtime::shared_locks().slot(19));auto* bitmap=data.bitmap.release();
    enqueue_deferred_task([bitmap,&texture,destination=data.destination,source=data.source,ready,completion=std::move(completion)]{
        upload_text_bitmap(bitmap,texture,destination,source,ready,completion);
    });
    return data.extent;
}
}
