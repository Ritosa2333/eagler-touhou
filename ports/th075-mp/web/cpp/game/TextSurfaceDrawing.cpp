#include "TextSurface.hpp"
namespace th075 {void TextSurface::render(ImageRenderer& images,u32 text){
QuadPoints quad0{};
CollisionRectangle bounds0{};

  
  
  
  bounds0={i32(0),i32(0),i32(0x23b),i32(0x4e)};
  quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
  scale_quad(quad0,0.0f,39.0f,0.0f,1.0f,scaleY,1.0f);
  translate_quad(quad0,x,y,0.0f);
  if (i8(window) == '\0') {
    scaleY=1;
  }
  else {
    images.quad(0,quad0,bounds0,0xffffffff);
  }
  if (1.0 <= (double)scaleY) {
    bounds0={i32(0),i32(0),i32(0x23b),i32((fontHeight + 6) * (int)i8(previousParts))};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    scale_quad(quad0,0.0f,39.0f,0.0f,1.0f,scaleY,1.0f);
    translate_quad(quad0,x,y,0.0f);
    translate_quad(quad0,9.0f,9.0f,0.0f);
    translate_quad(quad0,-1.0f,0.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xff000000,text);
    translate_quad(quad0,-1.0f,0.0f,0.0f);
    translate_quad(quad0,0.0f,-1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xff000000,text);
    translate_quad(quad0,0.0f,-1.0f,0.0f);
    translate_quad(quad0,1.0f,0.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xff000000,text);
    translate_quad(quad0,1.0f,0.0f,0.0f);
    translate_quad(quad0,0.0f,1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xff000000,text);
    translate_quad(quad0,-1.0f,0.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xffffffff,text);
    bounds0={i32(0),i32((fontHeight + 6) * (int)i8(previousParts)),i32(0x23b),i32(0x80)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    scale_quad(quad0,0.0f,39.0f,0.0f,1.0f,scaleY,1.0f);
    translate_quad(quad0,x,y,0.0f);
    if (fade != 255) {
      fade = fade + '\n';
    }
    translate_quad(quad0,9.0f,9.0f,0.0f);
    translate_quad(quad0,-1.0f,0.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,(u32)fade << 0x18,text);
    translate_quad(quad0,-1.0f,0.0f,0.0f);
    translate_quad(quad0,0.0f,-1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,(u32)fade << 0x18,text);
    translate_quad(quad0,0.0f,-1.0f,0.0f);
    translate_quad(quad0,1.0f,0.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,(u32)fade << 0x18,text);
    translate_quad(quad0,1.0f,0.0f,0.0f);
    translate_quad(quad0,0.0f,1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,(u32)fade << 0x18,text);
    translate_quad(quad0,-1.0f,0.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,(u32)fade << 0x18 | 0xffffff,text);
  }
  else {
    scaleY = (double)scaleY + double(0.05f);
  }
  return;

}}
