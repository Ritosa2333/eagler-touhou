#include "../platform/Fonts.hpp"
#ifdef TH_ENABLE_THCRAP
#include "../game/Localization.hpp"
#endif
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <vector>
using namespace th10;
namespace {
struct Object {
    enum Kind{Bitmap,Context,Font} kind;u32 font=0,bitmap=0,color=0,mode=2,charset=0;
    int width=0,height=0,pitch=0,bpp=0;std::vector<u8> pixels;TTF_Font* face=nullptr;SDL_Surface* raster=nullptr;TTF_Font* raster_face=nullptr;bool packed=false;std::string text;
    ~Object(){if(raster)SDL_DestroySurface(raster);if(face)TTF_CloseFont(face);}
};
std::map<u32,std::unique_ptr<Object>> objects;u32 next=1,failures=0;
std::vector<u8> blend,codepages;bool initialized=false;
Object& get(u32 id){auto it=objects.find(id);if(it==objects.end())std::abort();return *it->second;}
u32 add(Object::Kind kind){const auto id=next++;auto value=std::make_unique<Object>();value->kind=kind;objects[id]=std::move(value);return id;}
bool load(const char* name,std::vector<u8>& output){size_t size=0;void* bytes=SDL_LoadFile(name,&size);if(!bytes)return false;output.assign(static_cast<u8*>(bytes),static_cast<u8*>(bytes)+size);SDL_free(bytes);return true;}
void initialize(){if(initialized)return;if(!TTF_Init()||!load("/fonts/blend.bin",blend)||!load("/fonts/codepages.bin",codepages)||codepages.size()!=262144)std::abort();initialized=true;}
void utf8(std::string& out,u32 c){if(c<128)out+=char(c);else if(c<2048){out+=char(0xc0|(c>>6));out+=char(0x80|(c&63));}else{out+=char(0xe0|(c>>12));out+=char(0x80|((c>>6)&63));out+=char(0x80|(c&63));}}
bool utf8_valid(const u8* text,u32 size){
    if(!text)return size==0;
    const u8* end=text+size;
    while(text<end){
        const u8 first=*text;u32 length;
        if(first<0x80)length=1;
        else if(first>=0xc2&&first<=0xdf)length=2;
        else if(first>=0xe0&&first<=0xef)length=3;
        else if(first>=0xf0&&first<=0xf4)length=4;
        else return false;
        if(u32(end-text)<length)return false;
        for(u32 i=1;i<length;i++)if((text[i]&0xc0)!=0x80)return false;
        text+=length;
    }
    return true;
}
std::string decode(const char* bytes,u32 length,u32 charset){std::string result;const auto* data=reinterpret_cast<const u8*>(bytes);const auto* map=codepages.data()+(charset==134?131072:0);
    for(u32 i=0;i<length;i++){u32 code=data[i];const bool lead=charset==134?(code>=0x81&&code<=0xfe):((code>=0x81&&code<=0x9f)||(code>=0xe0&&code<=0xfc));if(lead&&i+1<length&&data[i+1])code=(code<<8)|data[++i];const auto c=u32(map[code*2])|(u32(map[code*2+1])<<8);utf8(result,c);}
    return result;
}
std::string decode_or_utf8(const char* bytes,u32 length,u32 charset){
#ifdef TH_ENABLE_THCRAP
    // A pack substitutes prepared UTF-8 text into the channels that normally
    // carry CP932/CP936 game bytes; detect it at this rasterization boundary.
    if(Localization::Active()&&utf8_valid(reinterpret_cast<const u8*>(bytes),length))
        return std::string(bytes,length);
#endif
    return decode(bytes,length,charset);
}
u32 blend_channel_4444(u32 before,u32 target,u32 coverage){
    const u32 source=(target*15+127)/255;
    return (before*(255-coverage)+source*coverage+127)/255;
}
#ifdef TH_ENABLE_THCRAP
// A pack font only covers the characters its translation uses. Original text
// that the pack does not translate is still CP932/CP936 and needs the base face
// (MS Gothic/SimHei), or Japanese falls back to the pack subset's .notdef and
// every missing kana shows as '？'. Cache one shared face per path/height so
// the per-object pack fonts never own these fallback handles.
std::map<std::pair<std::string,int>,TTF_Font*> fallback_faces;
TTF_Font* fallback_face(const char* path,int height){
    const auto key=std::make_pair(std::string(path),height);
    const auto found=fallback_faces.find(key);
    if(found!=fallback_faces.end())return found->second;
    TTF_Font* face=TTF_OpenFont(path,float(height));
    if(face){TTF_SetFontKerning(face,false);TTF_SetFontHinting(face,TTF_HINTING_NORMAL);}
    fallback_faces[key]=face;
    return face;
}
#endif
}
extern "C" {
u32 fonts_bitmap(const BitmapDescription* d,u8** out){const auto* bytes=reinterpret_cast<const u8*>(d);i32 width,height;u16 bpp;std::memcpy(&width,bytes+4,4);std::memcpy(&height,bytes+8,4);std::memcpy(&bpp,bytes+14,2);height=std::abs(height);if(width<=0||height<=0||bpp!=16||uint64_t(width)*height>16777216)return 0;
    const auto id=add(Object::Bitmap);auto& b=get(id);b.width=width;b.height=height;b.bpp=bpp;b.pitch=((width*bpp+31)>>5)*4;b.pixels.resize(size_t(b.pitch)*(height+4));*out=b.pixels.data();return id;
}
u32 fonts_context(){return add(Object::Context);}
u32 fonts_select(u32 context,u32 id){auto& dc=get(context);auto it=objects.find(id);if(it==objects.end())return 0;u32& target=it->second->kind==Object::Font?dc.font:dc.bitmap;const auto old=target;target=id;return old;}
void fonts_delete_context(u32 id){objects.erase(id);}
void fonts_delete_object(u32 id){objects.erase(id);}
u32 fonts_font(i32 height,const char*,u32 charset){initialize();const auto id=add(Object::Font);auto& f=get(id);f.charset=charset;f.height=height;
#ifdef TH_ENABLE_THCRAP
 // A language pack ships its own subset face under /thcrap/th10/fonts/; prefer
 // it so translated text has full coverage before the original faces are used.
 if(const char* pack=Localization::FontFile()){const std::string path=std::string("/thcrap/th10/fonts/")+pack;f.face=TTF_OpenFont(path.c_str(),float(height));f.packed=f.face!=nullptr;}
#endif
 if(!f.face)f.face=TTF_OpenFont(charset==134?"/fonts/simhei.ttf":"/fonts/msgothic.ttc",float(height));
 if(!f.face){std::fprintf(stderr,"SDL_ttf: %s\n",SDL_GetError());std::abort();}TTF_SetFontKerning(f.face,false);TTF_SetFontHinting(f.face,TTF_HINTING_NORMAL);return id;}
void fonts_background(u32 id,u32 mode){get(id).mode=mode;}
void fonts_color(u32 id,u32 color){get(id).color=color;}
void fonts_text(u32 id,i32 x,i32 y,const char* bytes,u32 length){auto& dc=get(id);auto& b=get(dc.bitmap);auto& f=get(dc.font);const auto value=decode_or_utf8(bytes,length,f.charset);if(value.empty())return;
    // The pack's UTF-8 strings use its subset face; untranslated original bytes
    // are not valid UTF-8, so rasterize them with the base face that has full
    // Japanese coverage instead of the pack subset.
    TTF_Font* face=f.face;
#ifdef TH_ENABLE_THCRAP
    if(f.packed&&Localization::Active()&&!utf8_valid(reinterpret_cast<const u8*>(bytes),length)){
        if(TTF_Font* base=fallback_face(f.charset==134?"/fonts/simhei.ttf":"/fonts/msgothic.ttc",f.height))face=base;
    }
#endif
    if(!face)return;
    // Shadow and foreground use identical coverage. Keep only the most recent
    // run per font+face, so those two original draws share one TTF rasterization.
    if(!f.raster||f.text!=value||f.raster_face!=face){const SDL_Color white{255,255,255,255};auto* original=TTF_RenderText_Blended(face,value.c_str(),value.size(),white);if(!original){failures++;return;}auto* raster=SDL_ConvertSurface(original,SDL_PIXELFORMAT_RGBA32);SDL_DestroySurface(original);if(!raster){failures++;return;}if(f.raster)SDL_DestroySurface(f.raster);f.raster=raster;f.text=value;f.raster_face=face;}
    const auto* raster=f.raster;
    const auto r=dc.color&255,g=(dc.color>>8)&255,blue=(dc.color>>16)&255;const auto* pixels=static_cast<const u8*>(raster->pixels);
    for(int j=0;j<raster->h;j++){const int row=y+j;if(row<0||row>=b.height)continue;for(int i=0;i<raster->w;i++){const int column=x+i;if(column<0||column>=b.width)continue;const u32 coverage=pixels[j*raster->pitch+i*4+3];if(!coverage)continue;
        auto* target=b.pixels.data()+row*b.pitch+column*2;const u32 before=target[0]|(u32(target[1])<<8);
        // The original GDI target is A4R4G4B4. TextOut writes antialiased RGB
        // into the low 12 bits and clears the alpha nibble on touched pixels;
        // TextRaster::draw() flips that nibble afterwards. Keep the channels
        // separate here instead of treating the DIB as RGB555, which creates
        // the visible red/green/blue fringe around otherwise white text.
        const u16 result=u16((blend_channel_4444((before>>8)&15,r,coverage)<<8)|(blend_channel_4444((before>>4)&15,g,coverage)<<4)|blend_channel_4444(before&15,blue,coverage));target[0]=u8(result);target[1]=u8(result>>8);
    }}
}
__attribute__((export_name("sdl_fonts_errors"))) u32 sdl_fonts_errors(){return failures;}
__attribute__((export_name("sdl_fonts_shutdown"))) void sdl_fonts_shutdown(){objects.clear();blend.clear();codepages.clear();
#ifdef TH_ENABLE_THCRAP
    for(auto& face:fallback_faces)if(face.second)TTF_CloseFont(face.second);fallback_faces.clear();
#endif
    if(initialized)TTF_Quit();initialized=false;}
}
