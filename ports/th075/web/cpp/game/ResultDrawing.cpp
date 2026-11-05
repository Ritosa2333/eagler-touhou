#include "ResultScene.hpp"
namespace th075 {void ResultScene::render(ImageRenderer& images,u32 captureTexture,const FighterActor& first,u8 difficulty)const{
QuadPoints quad0{};
CollisionRectangle bounds0{};

  i8 cVar1;
  int iVar2;
  
  
  
  cVar1 = true;
  if (cVar1 != '\0') {
    quad0={};
    images.graphics.depth(DepthMode(0));
    images.blend(SpriteBlend(0));
    bounds0={i32(0),i32(0),i32(0x280),i32(0x1e0)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    images.quad(0,quad0,bounds0,0xffffffff);
    images.graphics.filter(Filter::Linear);
    bounds0={i32(0),i32(0),i32(0x280),i32(0x1e0)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    rotate_quad(quad0,320.0f,240.0f,0.0f,((double(backgroundScale) - double(0.4f)) / double(0.6f)) * double(360.0f),((double(backgroundScale) - double(0.4f)) / double(0.6f)) * double(360.0f),((double(backgroundScale) - double(0.4f)) / double(0.6f)) * double(360.0f));
    scale_quad(quad0,520.0f,360.0f,0.0f,backgroundScale,backgroundScale,backgroundScale);
    iVar2 = i32(double(255.0f)*double(backgroundScale));
    images.graphics.textured_quad(quad0,bounds0,iVar2 << 0x18 | 0xffffff,captureTexture);
    images.graphics.filter(Filter::Nearest);
    bounds0={i32(0),i32(0),i32(0x100),i32(0x100)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    translate_quad(quad0,30.0f,30.0f,0.0f);
    images.quad(1,quad0,bounds0,(u32)alpha << 0x18 | 0xffffff);
    MenuDrawing::score(images,(double)(slide[0] + 300),70.0f,(u32)alpha << 0x18 | 0xffffff,base,0,0);
    MenuDrawing::score(images,(double)(slide[1] + 300),94.0f,(u32)alpha << 0x18 | 0xffffff,combo,0,0);
    MenuDrawing::score(images,(double)(slide[2] + 300),118.0f,(u32)alpha << 0x18 | 0xffffff,capture,0,0);
    MenuDrawing::score(images,(double)(slide[3] + 300),142.0f,(u32)alpha << 0x18 | 0xffffff,lives,0,0);
    bounds0={i32(0),i32((int)difficulty << 5),i32(0xa0),i32(difficulty * 0x20 + 0x20)};
    quad0=rectangle_quad(156.0f,168.0f,316.0f,200.0f);
    translate_quad(quad0,(double)(int)slide[4],0.0f,0.0f);
    images.quad(2,quad0,bounds0,(u32)alpha << 0x18 | 0xffffff);
    MenuDrawing::score(images,300.0f,221.0f,(u32)alpha << 0x18 | 0xffffff,pending,1,0);
    MenuDrawing::score(images,300.0f,245.0f,(u32)alpha << 0x18 | 0xffffff,first.score,1,0);
    static_cast<void>(0);
  }
  return;

}}
