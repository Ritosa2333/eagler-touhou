#include "AnmText.hpp"
#ifdef TH_ENABLE_THCRAP
#include <cstring>
#include "Localization.hpp"
#endif
namespace th10 {
namespace {
#ifdef TH_ENABLE_THCRAP
bool text_utf8_valid(const unsigned char* text,std::size_t size){
    const unsigned char* end=text+size;
    while(text<end){
        const unsigned char first=*text;std::size_t length;
        if(first<0x80)length=1;
        else if(first>=0xc2&&first<=0xdf)length=2;
        else if(first>=0xe0&&first<=0xef)length=3;
        else if(first>=0xf0&&first<=0xf4)length=4;
        else return false;
        if(std::size_t(end-text)<length)return false;
        for(std::size_t index=1;index<length;index++)if((text[index]&0xc0)!=0x80)return false;
        text+=length;
    }
    return true;
}
#endif
// Double-byte original encodings use one byte per display column (CP932/CP936),
// so byte length is the original column model. A pack's UTF-8 text needs the
// real column count instead: ASCII 1, everything else 2.
u32 display_columns(const char* text){
    const std::size_t size=std::strlen(text);
#ifdef TH_ENABLE_THCRAP
    if(Localization::Active()&&text_utf8_valid(reinterpret_cast<const unsigned char*>(text),size)){
        u32 columns=0;const unsigned char* cursor=reinterpret_cast<const unsigned char*>(text);
        const unsigned char* end=cursor+size;
        while(cursor<end){
            const unsigned char first=*cursor;
            const std::size_t length=first<0x80?1:(first&0xe0)==0xc0?2:(first&0xf0)==0xe0?3:4;
            columns+=first<0x80?1:2;cursor+=length;
        }
        return columns;
    }
#endif
    return u32(size);
}

}
// 0x4479d0. Sprite coordinates are converted separately, with truncation.
void AnmText::draw_sprite(const AnmSprite& sprite,void* texture,i32 offset,i32 size,u32 color,const char* text,bool flat,AnmTextEnvironment& env){
    if(size<=0)size=17;else if(size<=8)return;
    const TextureRect rectangle{Scalar::truncate(sprite.left),Scalar::truncate(sprite.top),Scalar::truncate(sprite.right),Scalar::truncate(sprite.bottom)};
    env.rasterize(rectangle,offset,size,color,text,texture,flat);
}
// 0x447a50/0x447ae0/0x447bb0 after their C varargs formatting. Alignment uses
// encoded byte length, as in the original Japanese and translated builds.
void AnmText::draw(AnmVm& vm,u32 color,const char* text,TextAlignment alignment,AnmTextEnvironment& env){
#ifdef TH_ENABLE_THCRAP
    // Keep layout commands intact for the font host. Their alignment applies
    // to individual runs and persistent tabstops, never to the whole string.
#endif
    i32 size=vm.text_settings[0],offset=0;
    if(alignment!=TextAlignment::Left){
        if(!size)size=17;
        const u32 width=static_cast<u32>(size-1)*display_columns(text);
        if(alignment==TextAlignment::Right)offset=(number(vm.sprite->width)-Extended::from_int64(width>>1)).truncate_int();
        else offset=static_cast<i32>(static_cast<u32>(Scalar::truncate(vm.sprite->width)/2)-(width>>2));
    }
    draw_sprite(*vm.sprite,vm.sprite->texture,offset,size,color,text,(vm.reserved_360[0]&2)!=0,env);
    vm.flags|=1;
}
// 0x437db0/0x437fe0. GDI draws at twice the requested resolution. Alpha is
// inverted around the platform drawing, then transparent RGB edges are filled
// before the original triangle-filter upload to the animation's sprite.
void TextRaster::draw(const TextureRect& rectangle,i32 offset,i32 size,u32 color,const char* text,void* texture,bool flat,TextRasterEnvironment& env){
    const i32 font_index=size<=17?0:size>=31?14:size-17;
    if(size<17)size=17;
    auto& image=*env.bitmap;
    if(flat)image.fill_text_background(color);else std::memset(image.pixels,0,static_cast<u32>(image.byte_size));
    const auto context=image.device_context,old_font=env.select_font(context,env.fonts[font_index]);
    const i32 rows=static_cast<i32>(static_cast<u32>(size)*2+6),x=static_cast<i32>(static_cast<u32>(offset)*2);
    image.invert_alpha(rows);env.transparent_background(context);
    const auto length=static_cast<u32>(std::strlen(text));
    if(!flat){env.text_color(context,0);env.text_out(context,static_cast<i32>(static_cast<u32>(x)+2),2,text,length);}
    env.text_color(context,color);env.text_out(context,x,0,text,length);
    env.select_font(context,old_font);image.invert_alpha(rows);image.fill_transparent_edges(rows);env.select_font(context,old_font);
    const i32 width=static_cast<i32>((static_cast<u32>(rectangle.right)-static_cast<u32>(rectangle.left))*2+22);
    const TextureRect source{0,0,width>1024?1024:width,static_cast<i32>(static_cast<u32>(size)*2+2)};
    auto* surface=env.get_surface(texture);env.upload(surface,rectangle,image,source);if(surface)env.release_surface(surface);
}
}
