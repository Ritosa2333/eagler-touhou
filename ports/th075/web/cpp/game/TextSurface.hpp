#pragma once
#include "ImageRenderer.hpp"
#include "TextRaster.hpp"
namespace th075 {
struct TextSurface {
 float x=35,y=360,scaleY=0;u8 fade=5,parts=0,previousParts=0,fontHeight=24,window=1;
 void configure(float horizontal,float vertical,bool box){x=horizontal;y=vertical;scaleY=0;parts=previousParts=0;window=box;}
 TextRasterOptions begin(u8 height,u8 red,u8 green,u8 blue,u8 part,bool continuation){previousParts=continuation?parts:0;parts=i8(part)>5?5:part;fontHeight=height;fade=5;TextRasterOptions o;o.fontHeight=height;o.red=red;o.green=green;o.blue=blue;o.lineSpacing=6;o.lineLimit=parts;return o;}
 void render(ImageRenderer& images,u32 text);
};
}
