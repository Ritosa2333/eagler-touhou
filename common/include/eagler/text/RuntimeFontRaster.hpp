#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <vector>

namespace eagler::text {
// Rasterize only glyphs the running game uses. Layout, outlines and scaling
// remain with the title's TextRaster. Shared font bytes are installed by Host.
class RuntimeFontRaster {
public:
    struct Profile { float height; int weight; };
private:
    struct Glyph { int advance=0,width=0,height=0; std::vector<std::uint8_t> alpha; };
    struct Face {
        Profile profile; TTF_Font* font=nullptr; TTF_Font* fallback=nullptr;bool fallbackTried=false;
        std::unordered_map<std::uint32_t,Glyph> glyphs; std::size_t bytes=0;
    };
    std::vector<Face> faces;
    std::vector<std::uint16_t> encoding;
    bool initialized=false;
    static constexpr std::size_t maxGlyphs=256,maxBytes=1024*1024;
    static TTF_Font* open(const char* path,Profile profile) {
        auto* font=TTF_OpenFont(path,profile.height);
        if(font){TTF_SetFontKerning(font,false);TTF_SetFontHinting(font,TTF_HINTING_NORMAL);
            if(profile.weight>=600)TTF_SetFontStyle(font,TTF_STYLE_BOLD);}
        return font;
    }
    const Glyph* glyph(Face& face,std::uint32_t code) {
        const auto requested=code;
        const auto found=face.glyphs.find(code);if(found!=face.glyphs.end())return &found->second;
        if(!face.font){face.font=open("/msgothic.ttc",face.profile);
            if(!face.font)face.font=open("/fonts/msgothic.ttc",face.profile);
            if(!face.font)return nullptr;}
        auto* font=face.font;
        if(!TTF_FontHasGlyph(font,code)){
            if(!face.fallbackTried){face.fallback=open("/unifont.otf",face.profile);face.fallbackTried=true;}
            if(face.fallback&&TTF_FontHasGlyph(face.fallback,code))font=face.fallback;
            else code=0x30fb;
        }
        Glyph value;int left,right,bottom,top;
        if(!TTF_GetGlyphMetrics(font,code,&left,&right,&bottom,&top,&value.advance))return nullptr;
        auto* surface=TTF_RenderGlyph_Blended(font,code,{255,255,255,255});if(!surface)return nullptr;
        auto* rgba=SDL_ConvertSurface(surface,SDL_PIXELFORMAT_RGBA32);SDL_DestroySurface(surface);if(!rgba)return nullptr;
        if(rgba->w<0||rgba->h<0||rgba->w>256||rgba->h>256){SDL_DestroySurface(rgba);return nullptr;}
        value.width=rgba->w;value.height=rgba->h;value.alpha.resize(std::size_t(value.width)*value.height);
        const auto* pixels=static_cast<const std::uint8_t*>(rgba->pixels);
        for(int y=0;y<value.height;++y)for(int x=0;x<value.width;++x)value.alpha[y*value.width+x]=pixels[y*rgba->pitch+x*4+3];
        SDL_DestroySurface(rgba);
        if(face.glyphs.size()>=maxGlyphs||face.bytes+value.alpha.size()>maxBytes){face.glyphs.clear();face.bytes=0;}
        face.bytes+=value.alpha.size();return &face.glyphs.emplace(requested,std::move(value)).first->second;
    }
    static std::uint16_t blend(std::uint16_t before,std::uint32_t color,std::uint8_t alpha) {
        auto channel=[&](unsigned old,unsigned target){return (old*(255-alpha)+((target*15+127)/255)*alpha+127)/255;};
        return std::uint16_t(channel((before>>8)&15,color&255)<<8|
            channel((before>>4)&15,(color>>8)&255)<<4|channel(before&15,(color>>16)&255));
    }
public:
    explicit RuntimeFontRaster(std::initializer_list<Profile> profiles){for(auto profile:profiles){Face face;face.profile=profile;faces.push_back(std::move(face));}}
    RuntimeFontRaster(const RuntimeFontRaster&)=delete;
    RuntimeFontRaster& operator=(const RuntimeFontRaster&)=delete;
    ~RuntimeFontRaster(){for(auto& face:faces){if(face.font)TTF_CloseFont(face.font);if(face.fallback)TTF_CloseFont(face.fallback);}if(initialized)TTF_Quit();}
    bool initialize(const std::uint8_t* mapping,std::size_t size) {
        if(initialized)return true;if(!mapping||size!=131072||faces.empty())return false;
        if(!TTF_Init())return false;
        faces[0].font=open("/msgothic.ttc",faces[0].profile);
        if(!faces[0].font)faces[0].font=open("/fonts/msgothic.ttc",faces[0].profile);
        if(!faces[0].font){TTF_Quit();return false;}
        encoding.resize(65536);for(std::size_t i=0;i<encoding.size();++i)encoding[i]=std::uint16_t(mapping[i*2])|(std::uint16_t(mapping[i*2+1])<<8);
        initialized=true;return true;
    }
    template<class Image> bool draw(Image& image,int x,int y,std::uint32_t profile,std::uint32_t color,const std::string& text) {
        if(!initialized||profile>=faces.size()||image.pitch<image.width*2||image.pixels.size()<std::uint64_t(image.pitch)*image.height)return false;
        auto& face=faces[profile];const auto* cursor=reinterpret_cast<const std::uint8_t*>(text.data());const auto* end=cursor+text.size();
        while(cursor<end&&*cursor){std::uint32_t bytes=*cursor++;
            if((bytes>=0x81&&bytes<=0x9f)||(bytes>=0xe0&&bytes<=0xfc))if(cursor<end&&*cursor)bytes=(bytes<<8)|*cursor++;
            const auto* raster=glyph(face,encoding[bytes]);if(!raster)return false;
            for(int gy=0;gy<raster->height;++gy){const int dy=y+gy;if(dy<0||dy>=int(image.height))continue;
                for(int gx=0;gx<raster->width;++gx){const int dx=x+gx;if(dx<0||dx>=int(image.width))continue;
                    const auto alpha=raster->alpha[gy*raster->width+gx];if(!alpha)continue;
                    auto* pixel=image.pixels.data()+dy*image.pitch+dx*2;std::uint16_t before;std::memcpy(&before,pixel,2);
                    const auto result=blend(before,color,alpha);std::memcpy(pixel,&result,2);}}
            x+=raster->advance;
        }return true;
    }
    std::size_t cachedGlyphs()const {std::size_t result=0;for(const auto& face:faces)result+=face.glyphs.size();return result;}
    std::size_t cachedBytes()const {std::size_t result=0;for(const auto& face:faces)result+=face.bytes;return result;}
};
}
