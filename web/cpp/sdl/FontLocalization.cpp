#include "FontDevice.hpp"
#include "../game/Localization.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <memory>
#include <string_view>

namespace th17::sdl {
namespace {
std::string read_font_path(){
    size_t size=0;
    auto* data=static_cast<char*>(SDL_LoadFile("/thcrap/th17/localization/options.json",&size));
    std::string options;
    if(data&&size<=65536)options.assign(data,size);
    SDL_free(data);
    if(options.empty())return {};
    SDL_PathInfo shared;
    if(SDL_GetPathInfo("/unifont.otf",&shared)&&shared.type==SDL_PATHTYPE_FILE)return "/unifont.otf";
    // A standalone TH17 pack carries a subset of the same Unifont face.
    constexpr std::string_view field=R"("fontFile":")";
    const auto key=options.find(field);
    if(key==std::string::npos)return {};
    const auto start=key+field.size(),end=options.find('"',start);
    if(end==std::string::npos)return {};
    const auto file=options.substr(start,end-start);
    if(file.empty()||file.size()>128||file.find_first_of("/\\\\:")!=std::string::npos||file.find("..")!=std::string::npos)return {};
    return "/thcrap/th17/fonts/"+file;
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
    SDL_PathInfo shared;
    has_japanese_fallback=SDL_GetPathInfo("/msgothic.ttc",&shared)&&shared.type==SDL_PATHTYPE_FILE;
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
    constexpr float sizes[]={32,32,16,16,36,36,16,16,26};
    if(!font){
        font=TTF_OpenFont(font_path.c_str(),sizes[request.font]);
        if(!font)return false;
        TTF_SetFontStyle(font,TTF_STYLE_BOLD);
        if(has_japanese_fallback){
            auto& gothic=japanese_fallback[request.font];
            gothic=TTF_OpenFont("/msgothic.ttc",sizes[request.font]);
            if(!gothic||!TTF_AddFallbackFont(font,gothic))return false;
        }
    }

    auto& scratch=raster.scratch;
    std::fill(scratch.pixels.begin(),scratch.pixels.end(),0);
    bool centered=request.centered;
    const auto text=display_text(request.bytes,centered);
    if(text.empty())return raster.copy(image,rect,style);

    using Surface=std::unique_ptr<SDL_Surface,decltype(&SDL_DestroySurface)>;
    Surface glyphs(TTF_RenderText_Blended(font,text.c_str(),text.size(),{255,255,255,255}),SDL_DestroySurface);
    if(!glyphs)return false;
    const i32 width=std::min(rect.right-rect.left,i32(scratch.width));
    const i32 height=std::min(rect.bottom-rect.top,i32(scratch.height));
    const i32 left=request.right_aligned||centered?2:std::max(2,request.x*2+2);
    const i32 max_width=width-left-2,max_height=height-4;
    if(max_width<=0||max_height<=0)return raster.copy(image,rect,style);
    if(glyphs->w>max_width||glyphs->h>max_height){
        glyphs.reset(SDL_ScaleSurface(glyphs.get(),std::min(glyphs->w,max_width),std::min(glyphs->h,max_height),SDL_SCALEMODE_LINEAR));
        if(!glyphs)return false;
    }
    Surface rgba(SDL_ConvertSurface(glyphs.get(),SDL_PIXELFORMAT_RGBA32),SDL_DestroySurface);
    if(!rgba)return false;
    const i32 x=request.right_aligned?width-rgba->w-2:centered?(width-rgba->w)/2:left;
    const i32 y=(height-rgba->h)/2;
    auto draw=[&](i32 dx,i32 dy,u32 color,bool outline){
        const u32 r=(color>>16)&255,g=(color>>8)&255,b=color&255;
        for(i32 row=0;row<rgba->h;++row){
            const auto* pixels=static_cast<const u8*>(rgba->pixels)+size_t(row)*rgba->pitch;
            auto* dst=scratch.pixels.data()+size_t(row+y+dy)*scratch.pitch+(x+dx)*2;
            for(i32 col=0;col<rgba->w;++col){
                const u32 alpha=pixels[col*4+3];
                if(!alpha)continue;
                u16 before;
                std::memcpy(&before,dst+col*2,2);
                const u32 old_alpha=(before>>12)*17;
                if(outline&&alpha<=old_alpha)continue;
                const u32 remaining=outline?0:(old_alpha*(255-alpha)+127)/255;
                const u32 result_alpha=alpha+remaining;
                auto channel=[&](u32 value,u32 shift){
                    const u32 old=((before>>shift)&15)*17;
                    return ((value*alpha+old*remaining)/result_alpha+8)/17;
                };
                const u16 pixel=u16((result_alpha+8)/17<<12|channel(r,8)<<8|channel(g,4)<<4|channel(b,0));
                std::memcpy(dst+col*2,&pixel,2);
            }
        }
    };
    if(style.shadow){
        for(const auto offset:{Vec2{-1,-1},Vec2{1,-1},Vec2{-1,1},Vec2{1,1},Vec2{-2,0},Vec2{2,0},Vec2{0,-2},Vec2{0,2}})
            draw(i32(offset.x),i32(offset.y),style.outline,true);
    }
    draw(0,0,style.color,false);
    return TextRaster::bleed(scratch,u32(height))&&raster.copy(image,rect,style);
}
}
