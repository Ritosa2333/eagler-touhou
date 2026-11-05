#ifdef TH_NATIVE_PLATFORM
#include "PlatformHost.hpp"
#ifdef TH_ENABLE_THCRAP
#include "../game/Localization.hpp"
#include <string>
#endif
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <set>
namespace th08 {
namespace {
std::map<std::pair<i32,i32>,TTF_Font*> faces;
#ifdef TH_ENABLE_THCRAP
// Localized text needs CJK glyphs MS Gothic does not carry. The chain mirrors
// the pack contract: the pack's declared subset font first, then Unifont.
std::map<std::pair<i32,i32>,TTF_Font*> pack_faces;
std::map<std::pair<i32,i32>,TTF_Font*> fallback_faces;
#endif
bool started=false;
TTF_Font* open_face(std::map<std::pair<i32,i32>,TTF_Font*>& cache,i32 height,i32 weight,const char* path){
    auto& face=cache[{height,weight}];
    if(!face){face=TTF_OpenFont(path,float(height));if(!face)return nullptr;
        TTF_SetFontKerning(face,false);TTF_SetFontHinting(face,TTF_HINTING_NORMAL);if(weight>=600)TTF_SetFontStyle(face,TTF_STYLE_BOLD);}
    return face;
}
bool render_glyph(TTF_Font* face,u16 code,JapaneseFont::Glyph& glyph,std::vector<u8>& pixels){
    int left,right,bottom,top,advance;
    if(!TTF_GetGlyphMetrics(face,code,&left,&right,&bottom,&top,&advance))return false;
    auto* image=TTF_RenderGlyph_Blended(face,code,{255,255,255,255});if(!image)return false;
    auto* rgba=SDL_ConvertSurface(image,SDL_PIXELFORMAT_RGBA32);SDL_DestroySurface(image);if(!rgba)return false;
    // SDL glyph surfaces already include the font's baseline placement.
    glyph={code,0,i16(advance),0,0,u16(rgba->w),u16(rgba->h)};pixels.resize(size_t(rgba->w)*rgba->h);
    const auto* data=static_cast<const u8*>(rgba->pixels);for(int y=0;y<rgba->h;y++)for(int x=0;x<rgba->w;x++)pixels[y*rgba->w+x]=(u32(data[y*rgba->pitch+x*4+3])*15+127)/255;
    SDL_DestroySurface(rgba);return true;
}
}
bool sdl_glyph(i32 height,i32 weight,u16 code,JapaneseFont::Glyph& glyph,std::vector<u8>& pixels){
    if(!started){if(!TTF_Init())return false;started=true;}
    auto* face=open_face(faces,height,weight,"/fonts/msgothic.ttc");
#ifdef TH_ENABLE_THCRAP
    // TTF_GetGlyphMetrics alone cannot report a missing glyph (FreeType
    // happily renders .notdef), so test coverage before trusting MS Gothic.
    if(face&&TTF_FontHasGlyph(face,code))return render_glyph(face,code,glyph,pixels);
    if(const char* pack=Localization::FontFile()){
        const std::string path=std::string("/thcrap/th08/fonts/")+pack;
        if(auto* pack_face=open_face(pack_faces,height,weight,path.c_str());pack_face&&TTF_FontHasGlyph(pack_face,code)&&render_glyph(pack_face,code,glyph,pixels)){
            static u16 reported=0;
            if(reported!=code){reported=code;SDL_Log("th08 thcrap font fallback: U+%04X from pack font %s",code,path.c_str());}
            return true;
        }
    }
    for(const char* path:{"/fonts/unifont.otf","/unifont.otf"})
        if(auto* fallback=open_face(fallback_faces,height,weight,path);fallback&&TTF_FontHasGlyph(fallback,code)&&render_glyph(fallback,code,glyph,pixels)){
            static u16 reported=0;
            if(reported!=code){reported=code;SDL_Log("th08 thcrap font fallback: U+%04X from %s",code,path);}
            return true;
        }
    return face&&render_glyph(face,code,glyph,pixels);
#else
    return face&&render_glyph(face,code,glyph,pixels);
#endif
}
void sdl_fonts_shutdown(){for(auto& face:faces)TTF_CloseFont(face.second);faces.clear();
#ifdef TH_ENABLE_THCRAP
    for(auto& face:pack_faces)TTF_CloseFont(face.second);pack_faces.clear();
    for(auto& face:fallback_faces)TTF_CloseFont(face.second);fallback_faces.clear();
#endif
    if(started)TTF_Quit();started=false;}
bool BrowserRuntime::native_fonts(){std::vector<u8> bytes;if(!sdl_read_file("/fonts/cp932.bin",bytes)||!put_font(0,bytes.data(),bytes.size()))return false;
    return sdl_read_file("/fonts/blend.bin",bytes)&&put_font(1,bytes.data(),bytes.size());}
u32 BrowserRuntime::native_font_steps(){
    if(warm_glyphs.empty()){std::set<u16> codes;for(u16 code=32;code<127;code++)codes.insert(code);
        const auto text=read("musiccmt.txt");const auto* p=text.data();const auto* end=p+text.size();
#ifdef TH_ENABLE_THCRAP
        // A pack-supplied musiccmt.txt is UTF-8; warm the glyphs it actually
        // decodes to instead of misreading it as CP932 (which could fail
        // preparation on bogus codes).
        const bool utf8=Localization::Active()&&utf8_valid(p,u32(text.size()));
#else
        const bool utf8=false;
#endif
        while(p<end){const auto code=utf8?utf8_next(p,end):fonts.encoding.next(p,end);if(code>=32)codes.insert(code);}
        warm_glyphs.assign(codes.begin(),codes.end());}
    return (warm_glyphs.size()*4+15)/16;
}
bool BrowserRuntime::native_font_step(u32 step){
    const i32 profiles[][2]={{16,400},{16,600},{26,600},{28,600}};
    for(u32 i=step*16;i<std::min<u32>((step+1)*16,warm_glyphs.size()*4);i++){
        const auto* profile=profiles[i/warm_glyphs.size()];const auto* font=fonts.find(profile[0],profile[1]);if(!font)return false;
        if(!font->find(warm_glyphs[i%warm_glyphs.size()])&&!font->find(0x30fb))return false;
    }return true;
}
}
#endif
