#include "OptionsScene.hpp"
namespace th075 {
void OptionsScene::render(ImageRenderer& images,u32 description,const GameSettings& settings)const{
QuadPoints quad0{};
CollisionRectangle bounds0{};

  i8 cVar1;
  
  
  
  cVar1 = true;
  if (cVar1 != '\0') {
    quad0={};
    images.graphics.depth(DepthMode(0));
    images.blend(SpriteBlend(0));
    bounds0={i32(0),i32(0),i32(0x280),i32(0x1e0)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    images.quad(0,quad0,bounds0,0xffffffff);
    bounds0={i32(0),i32(0xe0),i32(0x80),i32(0x100)};
    quad0=rectangle_quad(0.0f,32.0f,128.0f,64.0f);
    images.quad(1,quad0,bounds0,0xffffffff);
    bounds0={i32(0x80),i32(0xe0),i32(0x100),i32(0x100)};
    quad0=rectangle_quad(128.0f,32.0f,640.0f,64.0f);
    images.quad(1,quad0,bounds0,0xffffffff);
    bounds0={i32(0),i32(0),i32(0x100),i32(0xe0)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    translate_quad(quad0,144.0f,64.0f,0.0f);
    images.quad(1,quad0,bounds0,(u32)alpha << 0x18 | 0xffffff);
    bounds0={i32(0),i32(0),i32(0x100),i32(0x20)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    translate_quad(quad0,240.0f,64.0f,0.0f);
    images.quad(2,quad0,bounds0,(u32)alpha << 0x18 | 0x404040);
    switch(settings.difficulty) {
    case 0:
      bounds0={i32(0),i32(0),i32(0x38),i32(0x20)};
      break;
    case 1:
      bounds0={i32(0x38),i32(0),i32(0x80),i32(0x20)};
      break;
    case 2:
      bounds0={i32(0x80),i32(0),i32(0xb4),i32(0x20)};
      break;
    case 3:
      bounds0={i32(0xb4),i32(0),i32(0x100),i32(0x20)};
    }
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    translate_quad(quad0,240.0f,64.0f,0.0f);
    images.quad(2,quad0,bounds0,(u32)alpha << 0x18 | 0xffffff);
    bounds0={i32(0),i32((u32)settings.animatedBackground * 0x20 + 0x20),i32(0x40),i32((u32)settings.animatedBackground * 0x20 + 0x40)};
    quad0=rectangle_quad(0.0f,0.0f,64.0f,32.0f);
    translate_quad(quad0,432.0f,96.0f,0.0f);
    images.quad(2,quad0,bounds0,(u32)alpha << 0x18 | 0xffffff);
    bounds0={i32(0),i32((u32)settings.renderEveryFrame * 0x20 + 0x20),i32(0x40),i32((u32)settings.renderEveryFrame * 0x20 + 0x40)};
    quad0=rectangle_quad(0.0f,0.0f,64.0f,32.0f);
    translate_quad(quad0,432.0f,128.0f,0.0f);
    images.quad(2,quad0,bounds0,(u32)alpha << 0x18 | 0xffffff);
    bounds0={i32(0),i32((1 - (u32)settings.recoveryMarkers) * 0x20 + 0x20),i32(0x40),i32((1 - (u32)settings.recoveryMarkers) * 0x20 + 0x40)};
    quad0=rectangle_quad(0.0f,0.0f,64.0f,32.0f);
    translate_quad(quad0,432.0f,160.0f,0.0f);
    images.quad(2,quad0,bounds0,(u32)alpha << 0x18 | 0xffffff);
    bounds0={i32(0x40),i32(0x34),i32(0x100),i32(0x48)};
    quad0=rectangle_quad(0.0f,20.0f,192.0f,40.0f);
    translate_quad(quad0,304.0f,192.0f,0.0f);
    images.quad(2,quad0,bounds0,(u32)alpha << 0x18 | 0xffffff);
    bounds0={i32(0x4f),i32(0x6d),i32(0xf1),i32(0x73)};
    quad0=rectangle_quad(15.0f,13.0f,177.0f,19.0f);
    translate_quad(quad0,304.0f,192.0f,0.0f);
    images.quad(2,quad0,bounds0,(u32)alpha << 0x18 | 0xffffff);
    bounds0={i32(0x4f),i32(0x2d),i32(0xf1),i32(0x33)};
    quad0=rectangle_quad(15.0f,13.0f,177.0f,19.0f);
    scale_quad(quad0,15.0f,13.0f,0.0f,(double)(int)settings.musicVolume / 100.0,1.0f,1.0f);
    translate_quad(quad0,304.0f,192.0f,0.0f);
    images.quad(2,quad0,bounds0,(u32)alpha << 0x18 | 0xffffff);
    bounds0={i32(0x4f),i32(0x6d),i32(0xf1),i32(0x73)};
    quad0=rectangle_quad(15.0f,45.0f,177.0f,51.0f);
    translate_quad(quad0,304.0f,192.0f,0.0f);
    images.quad(2,quad0,bounds0,(u32)alpha << 0x18 | 0xffffff);
    bounds0={i32(0x4f),i32(0x4d),i32(0xf1),i32(0x53)};
    quad0=rectangle_quad(15.0f,45.0f,177.0f,51.0f);
    scale_quad(quad0,15.0f,45.0f,0.0f,(double)(int)settings.soundVolume / 100.0,1.0f,1.0f);
    translate_quad(quad0,304.0f,192.0f,0.0f);
    images.quad(2,quad0,bounds0,(u32)alpha << 0x18 | 0xffffff);
    bounds0={i32(0),i32(0x60),i32(0x40),i32(0x80)};
    quad0=rectangle_quad(0.0f,0.0f,64.0f,32.0f);
    translate_quad(quad0,96.0f,(double)(selected * 0x20 + 0x40),0.0f);
    images.quad(2,quad0,bounds0,0xffffffff);
    bounds0={i32(0),i32(0),i32(500),i32(0x40)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    translate_quad(quad0,100.0f,400.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,(u32)alpha << 0x18,description);
    translate_quad(quad0,1.0f,1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,(u32)alpha << 0x18,description);
    translate_quad(quad0,-1.0f,1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,(u32)alpha << 0x18,description);
    translate_quad(quad0,-1.0f,-1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,(u32)alpha << 0x18,description);
    translate_quad(quad0,1.0f,-1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,(u32)alpha << 0x18 | 0xffffff,description);
    static_cast<void>(0);
  }
  return;

}
}
