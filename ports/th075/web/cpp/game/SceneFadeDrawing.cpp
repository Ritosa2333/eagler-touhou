#include "SceneFade.hpp"
namespace th075 {void SceneFade::render(ImageRenderer& images,u32 captureTexture)const{switch(style){
case 256:{
QuadPoints quad0{};
CollisionRectangle bounds0{};

  
  u32 local_90;
  
  
  bounds0={i32(0),i32(0),i32(0x280),i32(0x1e0)};
  quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
  images.graphics.blend(SpriteBlend(1));
  local_90 = (count * -6 + 0xff) * 0x1000000 | 0xffffff;
  images.graphics.textured_quad(quad0,bounds0,0xffffffff,captureTexture);
  images.graphics.blend(SpriteBlend(2));
  images.graphics.filled_rectangle(bounds0,local_90);
  return;

break;}
case 4096:{
QuadPoints quad0{};
CollisionRectangle bounds0{};

  
  u32 local_90;
  
  
  bounds0={i32(0),i32(0),i32(0x280),i32(0x1e0)};
  quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
  images.graphics.blend(SpriteBlend(1));
  local_90 = count * 0x6000000 | 0xffffff;
  images.graphics.textured_quad(quad0,bounds0,0xffffffff,captureTexture);
  images.graphics.blend(SpriteBlend(2));
  images.graphics.filled_rectangle(bounds0,local_90);
  return;

break;}
case 512:{
QuadPoints quad0{};
CollisionRectangle bounds0{};

  
  u32 local_90;
  
  
  bounds0={i32(0),i32(0),i32(0x280),i32(0x1e0)};
  quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
  images.graphics.blend(SpriteBlend(1));
  local_90 = (count * 0x18 & 0xffU) << 0x10 | 0xff000000 | (count * 0x18 & 0xffU) << 8
             | count * 0x18 & 0xffU;
  images.graphics.textured_quad(quad0,bounds0,local_90,captureTexture);
  return;

break;}
case 8192:{
QuadPoints quad0{};
CollisionRectangle bounds0{};

  
  u32 local_90;
  
  
  bounds0={i32(0),i32(0),i32(0x280),i32(0x1e0)};
  quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
  images.graphics.blend(SpriteBlend(1));
  local_90 = (count * -0x18 + 0xffU & 0xff) << 0x10 | 0xff000000 |
             (count * -0x18 + 0xffU & 0xff) << 8 | count * -0x18 + 0xffU & 0xff;
  images.graphics.textured_quad(quad0,bounds0,local_90,captureTexture);
  return;

break;}
case 768:{
QuadPoints quad0{};
CollisionRectangle bounds0{};

  int local_a8;
  int local_a4;
  
  u32 local_90;
  
  
  quad0={};
  images.graphics.blend(SpriteBlend(0));
  local_90 = (count * 2 + 0x87) * 0x1000000 | 0xffffff;
  for (local_a4 = 0; local_a4 < 0x280; local_a4 = local_a4 + 0x28) {
    for (local_a8 = 0; local_a8 < 0x1e0; local_a8 = local_a8 + 0x28) {
      bounds0={i32(local_a4),i32(local_a8),i32(local_a4 + 0x28),i32(local_a8 + 0x28)};
      quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
      rotate_quad(quad0,(double)(local_a4 + 0x14),(double)(local_a8 + 0x14),0.0f,0.0f,90.0 - (double)count * 1.5,(double)(0x3c - count));
      images.graphics.textured_quad(quad0,bounds0,local_90,captureTexture);
    }
  }
  return;

break;}
case 12288:{
QuadPoints quad0{};
CollisionRectangle bounds0{};

  int local_a8;
  int local_a4;
  
  u32 local_90;
  
  
  quad0={};
  images.graphics.blend(SpriteBlend(0));
  local_90 = (count * -2 + 0xff) * 0x1000000 | 0xffffff;
  for (local_a4 = 0; local_a4 < 0x280; local_a4 = local_a4 + 0x28) {
    for (local_a8 = 0; local_a8 < 0x1e0; local_a8 = local_a8 + 0x28) {
      bounds0={i32(local_a4),i32(local_a8),i32(local_a4 + 0x28),i32(local_a8 + 0x28)};
      quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
      rotate_quad(quad0,(double)(local_a4 + 0x14),(double)(local_a8 + 0x14),0.0f,0.0f,(double)-count * 1.5,(double)-count);
      images.graphics.textured_quad(quad0,bounds0,local_90,captureTexture);
    }
  }
  return;

break;}
case 0x400:case 0x500:case 0x600:case 0x700:{SceneFade simple=*this;simple.style=0x200;simple.render(images,captureTexture);break;}case 0x6000:{SceneFade simple=*this;simple.style=0x2000;simple.render(images,captureTexture);break;}default:break;}}}
