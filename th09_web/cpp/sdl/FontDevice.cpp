#include "FontDevice.hpp"
#include "Assets.hpp"
#include "../game/MessageResource.hpp"
#include "../game/ShotResource.hpp"
#include "../game/Localization.hpp"
#include <set>
#include "../game/ImageResample.hpp"
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
namespace th09::sdl {
struct FontDevice::Impl {
    struct Glyph {i32 advance=0,width=0,height=0;std::vector<u8> coverage;};
    std::map<i32,TTF_Font*> faces;std::map<std::pair<i32,u16>,Glyph> glyphs;std::vector<u8> encoding,blend;bool ready=false;
    ~Impl(){for(auto& f:faces)TTF_CloseFont(f.second);if(ready)TTF_Quit();}
    u16 next(const u8*& p,const u8* end){
        if(Localization::Active()){
            const u8 first=*p++;if(first<0x80)return first;
            const u32 count=(first&0xe0)==0xc0?2:(first&0xf0)==0xe0?3:(first&0xf8)==0xf0?4:0;
            if(!count||size_t(end-p)<count-1)return 0x30fb;
            u32 code=first&((1u<<(7-count))-1u);
            for(u32 n=1;n<count;++n){if((*p&0xc0)!=0x80)return 0x30fb;code=(code<<6)|(*p++&0x3f);}
            return code<=0xffff?u16(code):0x30fb;
        }
        u32 code=*p++;if((code>=0x81&&code<=0x9f)||(code>=0xe0&&code<=0xfc)){if(p==end)return 0x30fb;code=(code<<8)|*p++;}return u16(encoding[code*2])|(u16(encoding[code*2+1])<<8);
    }
    const Glyph* glyph(i32 height,u16 code){
        const auto key=std::make_pair(height,code);const auto known=glyphs.find(key);if(known!=glyphs.end())return &known->second;
        auto& font=faces[height];if(!font){
            if(const char* pack=Localization::FontFile())font=TTF_OpenFont((std::string("/thcrap/th09/fonts/")+pack).c_str(),float(height));
            if(!font)font=TTF_OpenFont("/fonts/msgothic.ttc",float(height));
            if(!font)return nullptr;TTF_SetFontKerning(font,false);TTF_SetFontHinting(font,TTF_HINTING_NORMAL);
        }
        if(!TTF_FontHasGlyph(font,code))code=0x30fb;int left,right,bottom,top,advance;
        if(!TTF_GetGlyphMetrics(font,code,&left,&right,&bottom,&top,&advance))return nullptr;
        auto* image=TTF_RenderGlyph_Blended(font,code,{255,255,255,255});if(!image)return nullptr;auto* rgba=SDL_ConvertSurface(image,SDL_PIXELFORMAT_RGBA32);SDL_DestroySurface(image);if(!rgba)return nullptr;
        Glyph g;g.advance=advance;g.width=rgba->w;g.height=rgba->h;g.coverage.resize(size_t(g.width)*g.height);const auto* pixels=static_cast<const u8*>(rgba->pixels);
        for(i32 y=0;y<g.height;++y)for(i32 x=0;x<g.width;++x)g.coverage[y*g.width+x]=u8((u32(pixels[y*rgba->pitch+x*4+3])*15+127)/255);
        SDL_DestroySurface(rgba);return &glyphs.emplace(key,std::move(g)).first->second;
    }
    bool draw(TextureImage& image,i32 x,i32 y,i32 height,u32 color,const char* text){
        text=Localization::Utf8(text);const auto* p=reinterpret_cast<const u8*>(text);const auto* end=p+std::strlen(text);const u32 red=color&255,green=(color>>8)&255,blue=(color>>16)&255;
        while(p<end){const auto* g=glyph(height,next(p,end));if(!g)return false;
            for(i32 iy=0;iy<g->height;++iy){const i32 dy=y+iy;if(dy<0||dy>=i32(image.height))continue;for(i32 ix=0;ix<g->width;++ix){const i32 dx=x+ix;const u32 a=g->coverage[iy*g->width+ix];if(dx<0||dx>=i32(image.width)||!a)continue;u8* out=image.pixels.data()+dy*image.pitch+dx*2;const u16 before=u16(out[0])|(u16(out[1])<<8);const u32 base=a*8192;const u16 v=(u16(blend[base+((before>>10)&31)*256+red])<<10)|(u16(blend[base+((before>>5)&31)*256+green])<<5)|blend[base+(before&31)*256+blue];out[0]=u8(v);out[1]=u8(v>>8);}}
            x+=g->advance;
        }return true;
    }
};
FontDevice::FontDevice(GraphicsDevice& g):impl(std::make_unique<Impl>()),graphics(g){}
FontDevice::~FontDevice()=default;
bool FontDevice::initialize(){auto& f=*impl;if(!TTF_Init()){error=SDL_GetError();return false;}f.ready=true;if(!read_file("/fonts/cp932.bin",f.encoding)||f.encoding.size()!=131072||!read_file("/fonts/blend.bin",f.blend)||f.blend.size()!=131072){error="Missing Japanese font tables";return false;}return true;}
void FontDevice::prewarm(const char* text,i32 height){if(!impl->ready||height<1||height>128)return;text=Localization::Utf8(text);const auto* p=reinterpret_cast<const u8*>(text);const auto* end=p+std::strlen(text);while(p<end)impl->glyph(height,impl->next(p,end));}
void FontDevice::prewarm_game(ResourceReader& resources){
    std::set<u16> codepoints;
    const auto collect=[&](const char* text){text=Localization::Utf8(text);const auto* p=reinterpret_cast<const u8*>(text);const auto* end=p+std::strlen(text);while(p<end){const auto code=impl->next(p,end);if(code)codepoints.insert(code);}};
    {using namespace th09;
#include "../game/TitleData.inc"
        for(const auto* t:title_help)collect(t);for(const auto* t:options_help)collect(t);for(const auto* t:key_help)collect(t);for(const auto* t:locked_music_help)collect(t);collect(locked_music_name);
    }
    for(i32 character=0;character<16;++character){const auto* profile=character_resources(character);for(const char* filename:{profile->story_messages,profile->versus_messages}){std::vector<u8> bytes;MessageResource messages;if(!resources.read(filename,bytes)||!messages.load(bytes.data(),u32(bytes.size())))continue;
        for(const auto& entry:messages.entries){MessageInstruction ins;u32 at=entry.offset;if(!at)continue;while(messages.instruction(at,ins)&&ins.opcode){if(ins.opcode==3||ins.opcode==16){std::string text;if(MessageResource::decode_text(ins,ins.opcode==3?4:0,text))collect(text.c_str());}at+=4+ins.size;}}}
        std::vector<u8> bytes;ShotResource shot;if(resources.read(profile->shots,bytes)&&shot.load(bytes.data(),u32(bytes.size())))for(u32 level=0;level<shot.spell_names.size();++level)collect(Localization::SpellName(u32(character)*10+level,shot.spell_names[level].c_str()));
    }
    std::vector<u8> comments;if(resources.read("musiccmt.txt",comments)){comments.push_back(0);collect(reinterpret_cast<const char*>(comments.data()));}
    for(i32 ending=0;ending<15;++ending){char name[24];if(ending<14)std::snprintf(name,sizeof(name),"end%02d.end",ending);else std::snprintf(name,sizeof(name),"endstaff.end");std::vector<u8> bytes;if(!resources.read(name,bytes))continue;std::string line;for(const u8 byte:bytes){if(byte==0||byte==10||byte==13){if(!line.empty()&&line[0]!='@')collect(line.c_str());line.clear();}else line+=char(byte);}if(!line.empty())collect(line.c_str());}
    for(const auto height:{16,30,48})for(const auto code:codepoints)impl->glyph(height,code);
}
bool FontDevice::text(AnmVm& vm,const char* text,u32 color,u32 outline){
    if(!vm.loadedSprite||!text||!impl->ready)return false;const auto& s=*vm.loadedSprite;auto* target=graphics.pixels(s.texture);if(!target)return false;
    const i32 fw=vm.fontWidth?vm.fontWidth:15,fh=vm.fontHeight?vm.fontHeight:15;const bool large=fw>8;
    const i32 height=large?i32(float(fw)*s.scaleFactor.x):8,destination_height=large?i32(float(fh)*s.scaleFactor.y):8;
    const i32 x=i32(s.startPixelInclusive.x),y=i32(s.startPixelInclusive.y),width=i32(s.width);if(height<=0||height>64||width<=0||width>1024||destination_height<=0)return false;
    TextureImage scratch;scratch.width=1024;scratch.height=std::max(128,height*2+6);scratch.pitch=2048;scratch.format=PixelFormat::Argb1555;scratch.pixels.resize(size_t(scratch.pitch)*scratch.height);
    const i32 area_width=large?width*2:1024,area=area_width*(height*2+6);if(size_t(area)*2>scratch.pixels.size())return false;
    for(i32 n=0;n<area;++n)scratch.pixels[n*2+1]=0x80;
    const i32 center=x*2+2,shift=outline==0xffffffff?1:2;
    for(const auto& offset:{Vec2{float(shift),0},Vec2{-float(shift),0},Vec2{0,-float(shift)},Vec2{0,float(shift)},Vec2{0,0}}){const bool main=offset.x==0&&offset.y==0;if(!impl->draw(scratch,center+i32(offset.x),2+i32(offset.y),height*2,main?color:0,text)){error="Japanese glyph rendering failed";return false;}}
    for(i32 n=0;n<area;++n){u8* p=scratch.pixels.data()+n*2;u16 v=(u16(p[0])|(u16(p[1])<<8))^0x8000;if(!(v&0x8000))v=0;else if(outline==0xffffffff){i32 r=(v>>10)&31,g=(v>>5)&31,b=v&31;const auto shade=[&](i32 c){return c-(c*n/area)/4;};if(r>=b)r=shade(r);else b=shade(b);g=shade(g);v=u16(0x8000|(r<<10)|(g<<5)|b);}p[0]=u8(v);p[1]=u8(v>>8);}
    const i32 source_width=std::min(1024,width*2+(large?2:0)),source_height=height*2+(large?2:0);
    if(!ImageResample::triangle(*target,{0,y,width,y+destination_height},scratch,{0,0,source_width,source_height})){error="Japanese text texture bounds";return false;}
    graphics.changed(s.texture);vm.visible=true;return true;
}
}
