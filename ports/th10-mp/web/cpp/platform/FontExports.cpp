#include "Fonts.hpp"
#include <new>
#include <cstdlib>
using namespace th10;
#ifdef TH_SDL3
#define FONT_IMPORT(name) extern "C"
#else
#define FONT_IMPORT(name) extern "C" __attribute__((import_module("th10_fonts"),import_name(name)))
#endif
FONT_IMPORT("bitmap") u32 fonts_bitmap(const BitmapDescription*,u8**);
FONT_IMPORT("context") u32 fonts_context();
FONT_IMPORT("select") u32 fonts_select(u32,u32);
FONT_IMPORT("delete_context") void fonts_delete_context(u32);
FONT_IMPORT("delete_object") void fonts_delete_object(u32);
FONT_IMPORT("font") u32 fonts_font(i32,const char*,u32);
FONT_IMPORT("background") void fonts_background(u32,u32);
FONT_IMPORT("color") void fonts_color(u32,u32);
FONT_IMPORT("text") void fonts_text(u32,i32,i32,const char*,u32);
namespace {
struct Host final:browser::FontHost {
    u32 bitmap(const BitmapDescription& description,u8** pixels) override{return fonts_bitmap(&description,pixels);}
    u32 context() override{return fonts_context();}
    u32 select(u32 context,u32 object) override{return fonts_select(context,object);}
    void delete_context(u32 context) override{fonts_delete_context(context);}
    void delete_object(u32 object) override{fonts_delete_object(object);}
    u32 font(i32 height,const char* face,u32 charset) override{return fonts_font(height,face,charset);}
    void background(u32 context,u32 mode) override{fonts_background(context,mode);}
    void color(u32 context,u32 value) override{fonts_color(context,value);}
    void text(u32 context,i32 x,i32 y,const char* text,u32 length) override{fonts_text(context,x,y,text,length);}
} host;
}
#define FONT_EXPORT(name) extern "C" __attribute__((export_name(name)))
FONT_EXPORT("fonts_create") browser::Fonts* fonts_create(browser::GraphicsDevice* device,Rng* random,u32 chinese){auto* bytes=std::malloc(sizeof(browser::Fonts));if(!bytes)return nullptr;auto* result=new(bytes)browser::Fonts(host,*device,*random,chinese!=0);result->initialize();return result;}
FONT_EXPORT("fonts_destroy") void fonts_destroy(browser::Fonts* fonts){if(fonts){fonts->release();fonts->~Fonts();std::free(fonts);}}
FONT_EXPORT("fonts_draw") void fonts_draw(browser::Fonts* fonts,const TextureRect* rectangle,i32 offset,i32 size,u32 color,const char* text,void* texture,u32 flat){fonts->rasterize(*rectangle,offset,size,color,text,texture,flat!=0);}
FONT_EXPORT("fonts_draw_animation") void fonts_draw_animation(browser::Fonts* fonts,AnmVm* vm,u32 color,const char* text,u32 alignment){AnmText::draw(*vm,color,text,static_cast<TextAlignment>(alignment),*fonts);}
FONT_EXPORT("fonts_bitmap") RasterImage* fonts_bitmap_state(browser::Fonts* fonts){return &fonts->image;}
