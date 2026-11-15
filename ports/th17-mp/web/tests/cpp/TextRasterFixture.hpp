#pragma once
#include "../../cpp/game/TextRaster.hpp"
API(text_create) TextRaster* text_create(){return new TextRaster;}
API(text_delete) void text_delete(TextRaster* p){delete p;}
API(text_load) u32 text_load(TextRaster* p,u32 slot,const u8* data,u32 size){return p->glyphs.load(slot,data,size);}
API(text_style) TextStyle* text_style(){return new TextStyle;}
API(text_style_delete) void text_style_delete(TextStyle* p){delete p;}
API(text_rasterize) u32 text_rasterize(TextRaster* p,const char* value,const TextStyle* style){return p->rasterize(value,*style);}
API(text_pixels) const u8* text_pixels(TextRaster* p){return p->scratch.pixels.data();}
API(text_write) u32 text_write(TextRaster* p,u32 width,u32 height,const TextRect* rect,const char* value,const TextStyle* style,u8* output){TextureImage image;image.width=width;image.height=height;image.pitch=width*2;image.format=touhou::graphics::PixelFormat::Argb4444;image.pixels.resize(image.pitch*height);if(!p->write(image,*rect,value,*style))return false;std::memcpy(output,image.pixels.data(),image.pixels.size());return true;}
