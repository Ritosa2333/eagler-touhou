#include "MusicRoomScene.hpp"
namespace th075 {
void MusicRoomScene::render(ImageRenderer& images,const MusicRoomArtwork& artwork)const{
QuadPoints quad0{};
QuadPoints quad1{};
CollisionRectangle bounds0{};
CollisionRectangle bounds1{};

  i8 cVar1;
  u8 local_139;
  int local_138;
  
  
  
  
  
  
  cVar1 = true;
  if (cVar1 != '\0') {
    quad0={};
    quad1={};
    images.graphics.depth(DepthMode(0));
    images.blend(SpriteBlend(0));
    bounds0={i32(0),i32(0),i32(0x280),i32(0x1e0)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    images.quad(0,quad0,bounds0,0xffffffff);
    bounds0={i32(0),i32(0x22),i32(0x100),i32(0x62)};
    quad0=rectangle_quad(10.0f,10.0f,266.0f,74.0f);
    images.quad(1,quad0,bounds0,0xffffffff);
    quad0=rectangle_quad(90.0f,90.0f,602.0f,108.0f);
    bounds1={i32(0x96),i32(10),i32(0x9d),i32(0x10)};
    quad1=rectangle_quad(80.0f,100.0f,87.0f,106.0f);
    for (local_138 = 0;
        (local_138 < 10 &&
        (i8(firstVisible) + local_138 < (int)i8(catalog.tracks.size())));
        local_138 = local_138 + 1) {
      if (i8(firstVisible) + local_138 == (int)i8(selected)) {
        local_139 = 0xff;
      }
      else {
        local_139 = 0x80;
      }
      number(images,80.0f,quad0[0][1] + 1.0,(u32)local_139 << 0x18 | 0xffffff,i8(firstVisible) + 1 + local_138,2);
      images.quad(1,quad1,bounds1,(u32)local_139 << 0x18 | 0xffffff);
      bounds0={i32(0),i32((i8(firstVisible) + local_138) * 0x12),i32(0x200),i32((i8(firstVisible) + local_138) * 0x12 + 0x12)};
      translate_quad(quad0,-1.0f,0.0f,0.0f);
      images.graphics.textured_quad(quad0,bounds0,(u32)local_139 << 0x18,artwork.catalog);
      translate_quad(quad0,1.0f,1.0f,0.0f);
      images.graphics.textured_quad(quad0,bounds0,(u32)local_139 << 0x18,artwork.catalog);
      translate_quad(quad0,1.0f,-1.0f,0.0f);
      images.graphics.textured_quad(quad0,bounds0,(u32)local_139 << 0x18,artwork.catalog);
      translate_quad(quad0,-1.0f,-1.0f,0.0f);
      images.graphics.textured_quad(quad0,bounds0,(u32)local_139 << 0x18,artwork.catalog);
      translate_quad(quad0,0.0f,1.0f,0.0f);
      images.graphics.textured_quad(quad0,bounds0,(u32)local_139 << 0x18 | 0xffffff,artwork.catalog);
      translate_quad(quad0,0.0f,18.0f,0.0f);
      translate_quad(quad1,0.0f,18.0f,0.0f);
    }
    bounds0={i32(0xa5),i32(0),i32(0xb4),i32(0x10)};
    quad0=rectangle_quad(30.0f,(double)(((int)i8(selected) - (int)i8(firstVisible)) * 0x12
                        + 0x5a),45.0f,(double)(((int)i8(selected) - (int)i8(firstVisible)) * 0x12
                        + 0x6a));
    images.quad(1,quad0,bounds0,0xffffffff);
    bounds0={i32(0),i32(0),i32(0x200),i32(0x20)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    translate_quad(quad0,64.0f,320.0f,0.0f);
    translate_quad(quad0,-1.0f,0.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xff000000,artwork.credit);
    translate_quad(quad0,1.0f,1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xff000000,artwork.credit);
    translate_quad(quad0,1.0f,-1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xff000000,artwork.credit);
    translate_quad(quad0,-1.0f,-1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xff000000,artwork.credit);
    translate_quad(quad0,0.0f,1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xffffffff,artwork.credit);
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    translate_quad(quad0,300.0f,40.0f,0.0f);
    translate_quad(quad0,-1.0f,0.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xff000000,artwork.title);
    translate_quad(quad0,1.0f,1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xff000000,artwork.title);
    translate_quad(quad0,1.0f,-1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xff000000,artwork.title);
    translate_quad(quad0,-1.0f,-1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xff000000,artwork.title);
    translate_quad(quad0,0.0f,1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xffffffff,artwork.title);
    bounds0={i32(0),i32(0x10),i32(0x78),i32(0x20)};
    quad0=rectangle_quad(300.0f,20.0f,420.0f,36.0f);
    images.quad(1,quad0,bounds0,0xffffffff);
    bounds0={i32(0),i32(0),i32(0x200),i32(0x200)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    translate_quad(quad0,80.0f,340.0f,0.0f);
    translate_quad(quad0,-1.0f,0.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xff000000,artwork.details);
    translate_quad(quad0,1.0f,1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xff000000,artwork.details);
    translate_quad(quad0,1.0f,-1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xff000000,artwork.details);
    translate_quad(quad0,-1.0f,-1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xff000000,artwork.details);
    translate_quad(quad0,0.0f,1.0f,0.0f);
    images.graphics.textured_quad(quad0,bounds0,0xffffffff,artwork.details);
    static_cast<void>(0);
  }
  return;

}
}
