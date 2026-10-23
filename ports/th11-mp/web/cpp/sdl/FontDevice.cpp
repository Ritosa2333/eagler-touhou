#include "FontDevice.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cstring>
#include <utility>
namespace th11::sdl {
#if defined(TH_ENABLE_THCRAP)
namespace {
bool valid_utf8(const std::string& text){
    for(size_t p=0;p<text.size();){
        const u8 first=u8(text[p++]);if(first<0x80)continue;
        const int trailing=first>=0xc2&&first<=0xdf?1:first>=0xe0&&first<=0xef?2:first>=0xf0&&first<=0xf4?3:-1;
        if(trailing<0||p+size_t(trailing)>text.size())return false;
        for(int j=0;j<trailing;++j)if((u8(text[p++])&0xc0)!=0x80)return false;
    }
    return true;
}
std::string pack_font_file(){
    size_t size=0;auto* raw=static_cast<char*>(SDL_LoadFile("/thcrap/th11/localization/options.json",&size));
    if(!raw)return {};
    std::string options(raw,size);SDL_free(raw);
    const auto key=options.find("\"fontFile\"");if(key==std::string::npos)return {};
    const auto colon=options.find(':',key+10),open=colon==std::string::npos?colon:options.find('"',colon+1);
    const auto close=open==std::string::npos?open:options.find('"',open+1);
    if(close==std::string::npos)return {};
    const auto name=options.substr(open+1,close-open-1);
    if(name.empty()||name.size()>128||name.find("..")!=std::string::npos||
       name.find('/')!=std::string::npos||name.find('\\')!=std::string::npos||
       name.find(':')!=std::string::npos)return {};
    return "/thcrap/th11/fonts/"+name;
}
u16 blend_pixel(u16 before,u32 color,u8 alpha){
    const u32 r=color&255,g=(color>>8)&255,b=(color>>16)&255;
    // TextRaster keeps alpha inverted while drawing and XORs it back before
    // resampling. 0xf000 is an untouched transparent scratch pixel here.
    const u32 a=255-(before>>12)*17,old_r=((before>>8)&15)*17,old_g=((before>>4)&15)*17,old_b=(before&15)*17;
    const u32 out_a=alpha+(a*(255-alpha)+127)/255;
    const u32 out_r=(r*alpha+old_r*(255-alpha)+127)/255;
    const u32 out_g=(g*alpha+old_g*(255-alpha)+127)/255;
    const u32 out_b=(b*alpha+old_b*(255-alpha)+127)/255;
    return u16(((255-out_a+8)/17)<<12|((out_r+8)/17)<<8|((out_g+8)/17)<<4|((out_b+8)/17));
}
}
#endif
FontDevice::~FontDevice(){
    TH11_SYSTEM_SCOPE;
#if defined(TH_ENABLE_THCRAP)
    for(auto* font:localized)if(font)TTF_CloseFont(font);
#endif
}
bool FontDevice::initialize(){
    TH11_SYSTEM_SCOPE;
    if(ready)return true;
    size_t size=0;auto* data=static_cast<u8*>(SDL_LoadFile("/fonts/cp932.bin",&size));
    const bool valid=data&&runtimeFonts.initialize(data,size);SDL_free(data);
    if(!valid){error="Shared Gothic font or CP932 mapping unavailable";return false;}
    raster.glyphs.renderer(&runtimeFonts,[](void* owner,TextureImage& image,i32 x,i32 y,u32 font,u32 color,const std::string& text){return static_cast<eagler::text::RuntimeFontRaster*>(owner)->draw(image,x,y,font,color,text);});
    ready=raster.glyphs.ready();
#if defined(TH_ENABLE_THCRAP)
    const auto path=pack_font_file();
    if(!path.empty()){
        if(!TTF_Init()){error="Unable to initialize THCRAP font renderer";return false;}
        constexpr float heights[]={32.f,32.f,15.f,15.f};
        for(u32 n=0;n<4;++n)if(!(localized[n]=TTF_OpenFont(path.c_str(),heights[n]))){
            error="Missing THCRAP font: "+path;return false;
        }
    }
#endif
    return ready;
}
#if defined(TH_ENABLE_THCRAP)
void FontDevice::font_style(int index,const std::string& commands){
    TTF_FontStyleFlags flags=TTF_STYLE_NORMAL;
    for(char c:commands){if(c=='b')flags|=TTF_STYLE_BOLD;else if(c=='i')flags|=TTF_STYLE_ITALIC;else if(c=='u')flags|=TTF_STYLE_UNDERLINE;}
    TTF_SetFontStyle(localized[index],flags);
}
LayoutLine FontDevice::layout(const std::string& text,int index){
    auto line=thcrap_layout(text,layout_tabs,1024,[&](const std::string& value,const std::string& commands){
        font_style(index,commands);int width=0,height=0;
        if(!value.empty())TTF_GetStringSize(localized[index],value.c_str(),value.size(),&width,&height);
        return width;
    });
    font_style(index,"");return line;
}
bool FontDevice::write_unicode(TextureImage& output,const ImageRect& rect,const std::string& text,const TextStyle& style){
    const i32 height=std::max(8,style.height),index=style.font>=0&&style.font<=2?style.font:3;
    const u32 rows=u32(height*2+6);if(rows>raster.scratch.height||text.size()>1024)return false;
    auto& scratch=raster.scratch;
    const u16 background=style.plain?u16(((style.color>>4)&15)|((style.color>>8)&0xf0)|((style.color>>12)&0xf00)):0;
    auto* pixels=reinterpret_cast<u16*>(scratch.pixels.data());
    std::fill(pixels,pixels+scratch.pixels.size()/2,background);
    for(u32 n=0;n<rows*1024;++n)pixels[n]^=0xf000;
    const auto line=layout(text,index);
    for(const auto& run:line.runs){
    font_style(index,run.commands);
    auto* rendered=TTF_RenderText_Blended(localized[index],run.text.c_str(),run.text.size(),SDL_Color{255,255,255,255});
    font_style(index,"");
    if(!rendered)return false;
    auto* rgba=SDL_ConvertSurface(rendered,SDL_PIXELFORMAT_RGBA32);SDL_DestroySurface(rendered);
    if(!rgba)return false;
    auto stamp=[&](i32 ox,i32 oy,u32 color){
        for(i32 y=0;y<rgba->h;++y){const i32 dy=y+oy;if(dy<0||dy>=i32(rows))continue;
            const auto* row=static_cast<const u8*>(rgba->pixels)+size_t(y)*rgba->pitch;
            for(i32 x=0;x<rgba->w;++x){const i32 dx=x+ox;if(dx<0||dx>=1024)continue;
                const u8 alpha=row[x*4+3];if(alpha)pixels[size_t(dy)*1024+dx]=blend_pixel(pixels[size_t(dy)*1024+dx],color,alpha);
            }
        }
    };
    const i32 x=style.offset*2+run.x;
    if(!style.plain){for(const auto [dx,dy]:{std::pair<int,int>{2,4},{-2,4},{2,0},{-2,0}})stamp(x+dx,dy,style.outline);stamp(x,2,style.color);}
    else stamp(x,0,style.color);
    SDL_DestroySurface(rgba);
    }
    for(u32 n=0;n<rows*1024;++n)pixels[n]^=0xf000;
    TextRaster::bleed(scratch,rows);
    return ImageResample::triangle(output,rect,scratch,{0,0,std::min(1024,(rect.right-rect.left)*2+22),height*2+2});
}
#endif
bool FontDevice::text(AnmVm& vm,const DialogueText& request){
    // Glyph/layout caches and raster scratch outlive game checkpoints. The
    // texture observer below preserves pixels, while VM writes stay tracked.
    TH11_SYSTEM_SCOPE;
    if(!ready||!vm.sprite||!vm.resource){error="Missing text animation";return false;}
    const u32 handle=graphics.texture(*vm.resource,vm.sprite->texture);auto* image=graphics.pixels(handle);if(!image){error="Missing text texture";return false;}
    const auto& s=*vm.sprite;TextStyle style;style.offset=request.offset;style.height=vm.rectangle_columns?vm.rectangle_columns:17;style.font=request.style;
    style.color=request.color;style.spacing=u32(request.font);style.plain=(vm.flags2&2)!=0;
    i32 text_width=i32((style.height-1)*request.bytes.size()/2);
#if defined(TH_ENABLE_THCRAP)
    const bool unicode=localized[0]&&valid_utf8(request.bytes);
    if(unicode){
        const i32 index=style.font>=0&&style.font<=2?style.font:3;
        if(request.right_aligned||request.centered)text_width=layout(request.bytes,index).width/2;
    }
#endif
    if(request.right_aligned)style.offset=i32(s.width)-text_width;
    if(request.centered)style.offset=i32(s.width)/2-text_width/2;
    const ImageRect rect{i32(s.x),i32(s.y),i32(s.x+s.width),i32(s.y+s.height)};
#ifdef TH11_MULTIPLAYER
    // Preserve complete old CPU pixels before either raster path writes. A
    // redraw of old text cannot reconstruct untouched atlas regions exactly.
    if(!graphics.BeforeTextureWrite(handle,*image)){error=graphics.error;return false;}
#endif
    bool written=false;
#if defined(TH_ENABLE_THCRAP)
    if(unicode)written=write_unicode(*image,rect,request.bytes,style);
    else
#endif
    written=raster.write(*image,rect,request.bytes,style);
    if(!written){error="Text raster bounds: "+std::to_string(style.height);return false;}
    graphics.changed(handle);vm.flags|=1;++writes;return true;
}
}
