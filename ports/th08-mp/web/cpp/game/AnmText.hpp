#pragma once
#include "AnmLayout.hpp"
#include "TextRaster.hpp"
#include "TextureStore.hpp"
namespace th08 {
enum class TextAlignment {Left,Right,Center};
struct TextWriter {
    virtual ~TextWriter()=default;
    virtual bool draw(AnmVm&,TextAlignment,u32 color,u32 outline,const char* text)=0;
};
class AnmText : public TextWriter {
public:
    AnmText(TextureStore& textures,JapaneseFonts& fonts):textures(textures),raster(fonts){}
    bool inner(u32 texture,i32 x,i32 y,i32 width,i32 height,i32 font_width,i32 font_height,
               u32 color,u32 outline,const u8* text,u32 size,float scale_x,float scale_y);
    bool draw(AnmVm&,TextAlignment,u32 color,u32 outline,const char* text) override;
    bool formatted(AnmVm&,TextAlignment,u32 color,u32 outline,const char* format,...);
private:
    TextureStore& textures;
    TextRaster raster;
};
}
