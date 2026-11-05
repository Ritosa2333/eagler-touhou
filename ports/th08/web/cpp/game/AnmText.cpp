#include "AnmText.hpp"
#include "Localization.hpp"
#include <cstdarg>
#include <cstdio>
namespace th08 {
bool AnmText::inner(u32 texture,i32 x,i32 y,i32 width,i32 height,i32 font_width,i32 font_height,u32 color,u32 outline,const u8* text,u32 size,float scale_x,float scale_y){
    auto* record=textures.get(texture);if(!record)return false;auto& image=record->image;
    if(font_width<=0)font_width=15;if(font_height<=0)font_height=15;
    PixelSurface surface{image.format,image.width,image.height,image.width*TexturePixels::describe(image.format).bytes,image.pixels.data()};
    const bool bold=font_width>8;
    const i32 raster_height=bold?(Extended::from_int(font_width)*number(scale_x)).truncate_int():8;
    const i32 raster_width=bold?(Extended::from_int(font_height)*number(scale_y)).truncate_int():8;
    const bool result=raster.render(surface,x,y,width,height,raster_height,raster_width,color,outline,text,size,bold);
    if(result)textures.changed(texture);return result;
}
bool AnmText::draw(AnmVm& vm,TextAlignment align,u32 color,u32 outline,const char* text){
    if(!vm.loadedSprite||!text)return false;const auto& s=*vm.loadedSprite;
    const u32 size=std::strlen(text);const i32 width=vm.fontWidth>0?vm.fontWidth:15;
    auto x=number(s.startPixelInclusive.x);
    if(align!=TextAlignment::Left){const float divisor=align==TextAlignment::Right?1:2;
        const auto extent=number(s.widthPx)*number(s.scaleFactor.x)/number(divisor);
        // The vanilla formula uses the CP932 byte count as display columns.
        // UTF-8 translations count columns per code point; CP932 text and a
        // disabled/absent pack keep the original byte count unchanged.
        u32 units=size;
        if(Localization::Active()&&utf8_valid(reinterpret_cast<const u8*>(text),size))
            units=utf8_display_columns(reinterpret_cast<const u8*>(text),size);
        const auto text_extent=Extended::from_int64(units)*Extended::from_int(width)*number(s.scaleFactor.x)/number(divisor*2);
        x=(extent+x)-text_extent;
    }
    const bool result=inner(s.texture,x.truncate_int(),Scalar::truncate(s.startPixelInclusive.y),Scalar::truncate(s.width),Scalar::truncate(s.height),width,vm.fontHeight,color,outline,reinterpret_cast<const u8*>(text),size,s.scaleFactor.x,s.scaleFactor.y);
    vm.visible=true;return result;
}
bool AnmText::formatted(AnmVm& vm,TextAlignment align,u32 color,u32 outline,const char* format,...){
    char text[512];va_list args;va_start(args,format);const auto size=std::vsnprintf(text,sizeof(text),format,args);va_end(args);
    if(size<0||size>=i32(sizeof(text)))return false;return draw(vm,align,color,outline,text);
}
}
