#include "ResultListScene.hpp"
namespace th075 {void ResultListScene::render(ImageRenderer& images,u32 textTexture,const GameProfile& profile,u32 cardCount)const{const auto& progress=profile.characters[character].difficulty[cardsMode?0:difficulty];
QuadPoints quad0{};
CollisionRectangle bounds0{};

  i8 cVar1;
  int iVar2;
  int local_a8;
  int local_a4;
  int local_a0;
  
  
  
  cVar1 = true;
  if (cVar1 != '\0') {
    quad0={};
    images.graphics.depth(DepthMode(0));
    images.blend(SpriteBlend(0));
    bounds0={i32(0),i32(0),i32(0x280),i32(0x1e0)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    images.quad(0,quad0,bounds0,0xffffffff);
    bounds0={i32(0),i32(0),i32(0x200),i32(0x200)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    scale_quad(quad0,256.0f,0.0f,0.0f,-1.0f,1.0f,1.0f);
    translate_quad(quad0,128.0f,0.0f,0.0f);
    images.quad(0,quad0,bounds0,0xff808080);
    if (i8(cardsMode) == '\0') {
      bounds0={i32(0),i32(0x18),i32(0x78),i32(0x78)};
      quad0=rectangle_quad(0.0f,0.0f,120.0f,96.0f);
      translate_quad(quad0,80.0f,72.0f,0.0f);
      images.quad(1,quad0,bounds0,0xffffffff);
      bounds0={i32(0),i32(0),i32(0xf0),i32(0xf0)};
      quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
      translate_quad(quad0,200.0f,74.0f,0.0f);
      translate_quad(quad0,0.0f,-1.0f,0.0f);
      images.graphics.textured_quad(quad0,bounds0,0xff000000,textTexture);
      translate_quad(quad0,1.0f,1.0f,0.0f);
      images.graphics.textured_quad(quad0,bounds0,0xff000000,textTexture);
      translate_quad(quad0,-1.0f,1.0f,0.0f);
      images.graphics.textured_quad(quad0,bounds0,0xff000000,textTexture);
      translate_quad(quad0,-1.0f,-1.0f,0.0f);
      images.graphics.textured_quad(quad0,bounds0,0xff000000,textTexture);
      translate_quad(quad0,1.0f,0.0f,0.0f);
      images.graphics.textured_quad(quad0,bounds0,0xffffffff,textTexture);
      bounds0={i32(0),i32(0),i32(0xf0),i32(0x18)};
      quad0=rectangle_quad(0.0f,0.0f,240.0f,24.0f);
      translate_quad(quad0,80.0f,20.0f,0.0f);
      images.quad(1,quad0,bounds0,0xffffffff);
      bounds0={i32(0),i32(i8(character) * 0x18),i32(0x80),i32(i8(character) * 0x18 + 0x18)};
      quad0=rectangle_quad(0.0f,0.0f,128.0f,24.0f);
      translate_quad(quad0,320.0f,20.0f,0.0f);
      images.quad(3,quad0,bounds0,0xffffffff);
      bounds0={i32(0xa8),i32(i8(difficulty) * 0x18 + 0x18),i32(0xf0),i32(i8(difficulty) * 0x18 + 0x30)};
      quad0=rectangle_quad(0.0f,0.0f,72.0f,24.0f);
      translate_quad(quad0,464.0f,20.0f,0.0f);
      images.quad(1,quad0,bounds0,0xffffffff);
      bounds0={i32(0x78),i32(0x18),i32(0xa8),i32(0x30)};
      quad0=rectangle_quad(0.0f,0.0f,48.0f,24.0f);
      translate_quad(quad0,80.0f,200.0f,0.0f);
      images.quad(1,quad0,bounds0,0xffffffff);
      bounds0={i32(0x78),i32(0x30),i32(0xa8),i32(0x48)};
      quad0=rectangle_quad(0.0f,0.0f,48.0f,24.0f);
      translate_quad(quad0,160.0f,200.0f,0.0f);
      images.quad(1,quad0,bounds0,0xffffffff);
      bounds0={i32(0x78),i32(0x48),i32(0xa8),i32(0x60)};
      quad0=rectangle_quad(0.0f,0.0f,48.0f,24.0f);
      translate_quad(quad0,300.0f,200.0f,0.0f);
      images.quad(1,quad0,bounds0,0xffffffff);
      bounds0={i32(0x78),i32(0x60),i32(0xa8),i32(0x78)};
      quad0=rectangle_quad(0.0f,0.0f,48.0f,24.0f);
      translate_quad(quad0,500.0f,200.0f,0.0f);
      images.quad(1,quad0,bounds0,0xffffffff);
      for (local_a0 = 0; local_a0 < 10; local_a0 = local_a0 + 1) {
        MenuDrawing::slot(images,80.0f,(double)(local_a0 * 0x14 + 0xe6),0xffffffff,local_a0 + 1);
        MenuDrawing::name(images,160.0f,(double)(local_a0 * 0x14 + 0xe6),0xffffffff,progress.ranking[local_a0].name);
        MenuDrawing::digits(images,330.0f,(double)(local_a0 * 0x14 + 0xe6),0xffffffff,(int)progress.ranking[local_a0].month,2);
        bounds0={i32(0xe4),i32(0x28),i32(0xf7),i32(0x3c)};
        quad0=rectangle_quad(325.0f,(double)(local_a0 * 0x14 + 0xe6),344.0f,(double)(local_a0 * 0x14 + 0xfa));
        images.quad(2,quad0,bounds0,0xffffffff);
        MenuDrawing::digits(images,370.0f,(double)(local_a0 * 0x14 + 0xe6),0xffffffff,(int)progress.ranking[local_a0].day,2);
        MenuDrawing::digits(images,560.0f,(double)(local_a0 * 0x14 + 0xe6),0xffffffff,progress.ranking[local_a0].score,0);
      }
    }
    else {
      images.blend(SpriteBlend(2));
      for (local_a8 = 0; local_a8 < 10; local_a8 = local_a8 + 1) {
        bounds0={i32(0x80),i32(0x43),i32(0x82),i32(0x4d)};
        quad0=rectangle_quad(10.0f,(double)(local_a8 * 0x20 + 0x46),630.0f,(double)(local_a8 * 0x20 + 0x50));
        images.quad(1,quad0,bounds0,0xffffffff);
      }
      images.blend(SpriteBlend(0));
      local_a4 = 0;
      bounds0={i32(0),i32(0x90),i32(0x18),i32(0xa8)};
      for (local_a8 = page * 10;
          (local_a8 < page * 10 + 10 &&
          (iVar2 = i32(cardCount), local_a8 < iVar2)); local_a8 = local_a8 + 1) {
        MenuDrawing::digits(images,540.0f,(double)(local_a4 * 0x20 + 0x3f),0xff8080ff,progress.cards[local_a8].highestScore,0);
        if (progress.cards[local_a8].perfect != '\0') {
          quad0=rectangle_quad(535.0f,(double)(local_a4 * 0x20 + 0x3d),559.0f,(double)(local_a4 * 0x20 + 0x55));
          images.quad(1,quad0,bounds0,0xffffffff);
        }
        local_a4 = local_a4 + 1;
      }
      bounds0={i32(0),i32(0),i32(0x26c),i32(0x140)};
      quad0=rectangle_quad(10.0f,62.0f,630.0f,382.0f);
      translate_quad(quad0,-1.0f,0.0f,0.0f);
      images.graphics.textured_quad(quad0,bounds0,0xff000000,textTexture);
      translate_quad(quad0,1.0f,1.0f,0.0f);
      images.graphics.textured_quad(quad0,bounds0,0xff000000,textTexture);
      translate_quad(quad0,1.0f,-1.0f,0.0f);
      images.graphics.textured_quad(quad0,bounds0,0xff000000,textTexture);
      translate_quad(quad0,-1.0f,-1.0f,0.0f);
      images.graphics.textured_quad(quad0,bounds0,0xff000000,textTexture);
      translate_quad(quad0,0.0f,1.0f,0.0f);
      images.graphics.textured_quad(quad0,bounds0,0xffffffff,textTexture);
      bounds0={i32(0),i32(0x78),i32(0xf0),i32(0x90)};
      quad0=rectangle_quad(0.0f,0.0f,240.0f,24.0f);
      translate_quad(quad0,80.0f,20.0f,0.0f);
      images.quad(1,quad0,bounds0,0xffffffff);
      bounds0={i32(0),i32(i8(character) * 0x18),i32(0x80),i32(i8(character) * 0x18 + 0x18)};
      quad0=rectangle_quad(0.0f,0.0f,128.0f,24.0f);
      translate_quad(quad0,320.0f,20.0f,0.0f);
      images.quad(3,quad0,bounds0,0xffffffff);
    }
    static_cast<void>(0);
  }
  return;

}}
