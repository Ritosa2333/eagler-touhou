#include "FontDevice.hpp"
#include "../game/Localization.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <memory>
#include <string_view>

namespace th17::sdl {
namespace {
bool is_file(const char* path){
    SDL_PathInfo info;
    return SDL_GetPathInfo(path,&info)&&info.type==SDL_PATHTYPE_FILE;
}

std::string read_font_path(){
    size_t size=0;
    auto* data=static_cast<char*>(SDL_LoadFile("/thcrap/th17/localization/options.json",&size));
    std::string options;
    if(data&&size<=65536)options.assign(data,size);
    SDL_free(data);
    if(options.empty())return {};

    // Prefer the compiled Unifont subset, as TH15 does. The shared face also
    // supports sessions whose language pack has no embedded font.
    constexpr std::string_view field=R"("fontFile":")";
    const auto key=options.find(field);
    if(key!=std::string::npos){
        const auto start=key+field.size(),end=options.find('"',start);
        const auto file=options.substr(start,end-start);
        if(end!=std::string::npos&&!file.empty()&&file.size()<=128&&
           file.find_first_of("/\\:")==std::string::npos&&file.find("..")==std::string::npos){
            const auto path="/thcrap/th17/fonts/"+file;
            if(is_file(path.c_str()))return path;
        }
    }
    return is_file("/unifont.otf")?"/unifont.otf":"";
}

// Blend into the same inverted-alpha ARGB4444 scratch surface used by TH15.
u16 blend_pixel(u16 before,u32 color,u8 alpha){
    const u32 old_alpha=255-(before>>12)*17;
    const u32 result_alpha=alpha+(old_alpha*(255-alpha)+127)/255;
    auto channel=[&](u32 shift,u32 source){
        const u32 old=((before>>shift)&15)*17;
        return ((source*alpha+old*(255-alpha)+127)/255+8)/17;
    };
    return u16(((255-result_alpha+8)/17)<<12|channel(8,(color>>16)&255)<<8|
               channel(4,(color>>8)&255)<<4|channel(0,color&255));
}

// Endings use silent tab definitions and right-aligned speaker names. Keep
// the text of visible runs when drawing them into TH17's single-line atlas.
std::string display_text(std::string_view text,bool& centered){
    std::string result;
    while(!text.empty()){
        const auto start=text.find('<');
        result.append(text.substr(0,start));
        if(start==std::string_view::npos)break;
        text.remove_prefix(start);
        const auto delimiter=text.find('$');
        const auto end=text.find('>');
        if(delimiter==std::string_view::npos||end==std::string_view::npos||delimiter>=end){
            result+='<';
            text.remove_prefix(1);
            continue;
        }
        const auto command=text.substr(1,delimiter-1);
        if(command!="ts"&&command!="r"&&command!="l"&&command!="c"&&command!="i"){
            result+='<';
            text.remove_prefix(1);
            continue;
        }
        auto content=text.substr(delimiter+1,end-delimiter-1);
        content=content.substr(0,content.find('$'));
        if(command!="ts")result.append(content);
        if(command=="r")result+=' ';
        if(command=="c"&&start==0&&end+1==text.size())centered=true;
        text.remove_prefix(end+1);
    }
    return result;
}
}

FontDevice::~FontDevice(){
    for(auto* font:localized_fonts)if(font)TTF_CloseFont(font);
    for(auto* font:japanese_fallback)if(font)TTF_CloseFont(font);
    if(ttf_initialized)TTF_Quit();
}

bool FontDevice::load_localized_font(){
    if(localization_checked)return font_path.empty()||ttf_initialized;
    localization_checked=true;
    font_path=read_font_path();
    if(font_path.empty())return true;
    ttf_initialized=TTF_Init();
    if(!ttf_initialized)error=SDL_GetError();
    has_japanese_fallback=is_file("/msgothic.ttc");
    size_t size=0;
    auto* locale=static_cast<char*>(SDL_LoadFile("/thcrap/th17/runtime-language.txt",&size));
    english_music_details=locale&&std::string_view(locale,size)=="lang_en";
    SDL_free(locale);
    return ttf_initialized;
}

bool FontDevice::translated_message(const std::string& text)const{
    return !font_path.empty()&&!text.empty()&&valid_utf8(text);
}

bool FontDevice::write_localized(TextureImage& image,const TextLayout& layout,const TextureText& request){
    if(font_path.empty()||request.bytes.size()>4096)return false;
    const auto& rect=layout.region;
    auto style=layout.style;
    style.fallback=false;
    auto& font=localized_fonts[request.font];
    auto& gothic=japanese_fallback[request.font];
    constexpr i32 sizes[]={32,32,15,15,40,40,15,15,26,24};
    const i32 preferred=request.music_detail?(english_music_details?24:30):sizes[request.font];
    if(!font){
        font=TTF_OpenFont(font_path.c_str(),float(preferred));
        if(!font)return false;
        if(has_japanese_fallback){
            gothic=TTF_OpenFont("/msgothic.ttc",float(preferred));
            if(!gothic||!TTF_AddFallbackFont(font,gothic))return false;
        }
    }

    auto& scratch=raster.scratch;
    const i32 width=std::min(rect.right-rect.left,i32(scratch.width));
    const i32 height=std::min(rect.bottom-rect.top,i32(scratch.height));
    const u32 rows=u32(std::max(height,std::max(17,style.height)*2+12));
    if(width<=4||height<=4||rows>scratch.height)return false;
    std::fill(scratch.pixels.begin(),scratch.pixels.end(),0);
    bool centered=request.centered;
    const auto text=display_text(request.bytes,centered);
    if(text.empty())return raster.copy(image,rect,style);

    const i32 offset=request.right_aligned||centered?0:std::max(0,style.offset);
    const i32 max_width=width-offset-4,max_height=height-4;
    if(max_width<=0)return raster.copy(image,rect,style);

    // Fit long lines by changing the point size before rasterization. Scaling
    // a completed glyph bitmap independently in X and Y changes its shape.
    i32 text_width=0,text_height=0;
    for(i32 size=preferred;;){
        if(TTF_GetFontSize(font)!=size&&(!TTF_SetFontSize(font,float(size))||
           (gothic&&!TTF_SetFontSize(gothic,float(size)))))return false;
        if(!TTF_GetStringSize(font,text.c_str(),text.size(),&text_width,&text_height))return false;
        if(text_width<=max_width&&text_height<=max_height)break;
        if(size==1)return false;
        const float scale=std::min(float(max_width)/std::max(1,text_width),float(max_height)/std::max(1,text_height));
        size=std::clamp(i32(size*scale),1,size-1);
    }

    using Surface=std::unique_ptr<SDL_Surface,decltype(&SDL_DestroySurface)>;
    Surface glyphs(TTF_RenderText_Blended(font,text.c_str(),text.size(),{255,255,255,255}),SDL_DestroySurface);
    if(!glyphs)return false;
    Surface rgba(SDL_ConvertSurface(glyphs.get(),SDL_PIXELFORMAT_RGBA32),SDL_DestroySurface);
    if(!rgba||!TextRaster::invert_alpha(scratch,rows))return false;
    const i32 x=request.right_aligned?width-rgba->w-2:centered?(width-rgba->w)/2:offset+2;
    auto stamp=[&](i32 dx,i32 dy,u32 color){
        for(i32 row=0;row<rgba->h&&row+dy<height;++row){
            const auto* pixels=static_cast<const u8*>(rgba->pixels)+size_t(row)*rgba->pitch;
            auto* dst=scratch.pixels.data()+size_t(row+dy)*scratch.pitch;
            for(i32 col=0;col<rgba->w;++col){
                const i32 dst_x=x+col+dx;
                const u8 alpha=pixels[col*4+3];
                if(!alpha||dst_x<0||dst_x>=width)continue;
                u16 before;
                std::memcpy(&before,dst+dst_x*2,2);
                const u16 pixel=blend_pixel(before,color,alpha);
                std::memcpy(dst+dst_x*2,&pixel,2);
            }
        }
    };
    if(style.shadow){
        for(const auto delta:{Vec2{1,1},Vec2{-1,1},Vec2{1,3},Vec2{-1,3},Vec2{2,2},Vec2{-2,2},Vec2{0,0},Vec2{0,4}})
            stamp(i32(delta.x),i32(delta.y),style.outline);
    }
    stamp(0,2,style.color);
    return TextRaster::invert_alpha(scratch,rows)&&TextRaster::expand_alpha(scratch,rows)&&
           TextRaster::bleed(scratch,rows)&&raster.copy(image,rect,style);
}
}
