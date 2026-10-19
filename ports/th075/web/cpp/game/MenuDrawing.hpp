#pragma once
#include "ImageRenderer.hpp"
namespace th075 {
struct MenuDrawing {
 static void region(ImageRenderer&,i16 image,const CollisionRectangle&,float left,float top,float right,float bottom,u32 color=0xffffffff);
 static void name(ImageRenderer&,float x,float y,u32 color,const std::array<u8,8>&);
 static void digits(ImageRenderer&,float x,float y,u32 color,i32 value,i32 count=0);
 static void slot(ImageRenderer&,float x,float y,u32 color,u8);
 static void replay_file(ImageRenderer&,float x,float y,u32 color,const std::string&);
 static void score(ImageRenderer&,float x,float y,u32 color,i32 value,i8 style=0,i32 count=0);
};
}
