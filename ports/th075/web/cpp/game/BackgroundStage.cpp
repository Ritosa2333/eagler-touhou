#include "BackgroundStage.hpp"
#include <cmath>
namespace th075 {
namespace {float float_bits(u32 v){float f;std::memcpy(&f,&v,4);return f;}}

void BackgroundStage::draw_back_BG00a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.blend(SpriteBlend(0));
  images.image(2,(double)iVar1 * 0.6 + 320.0,(double)iVar2 * 0.6 + 200.0,0xffffffff,768.0f,384.0f,3.0f,0.0f,0);
  images.image(1,(double)iVar1 * 0.75 + 320.0,(double)(iVar2 + 0x118),0xffffffff,700.0f,350.0f,1.0f,0.0f,0);
  images.image(1,(double)iVar1 * 0.75 + 320.0,(double)(iVar2 + 0x118),0xffffffff,700.0f,350.0f,1.0f,0.0f,1);
  images.image(0,(double)(iVar1 + 0x140),(double)(iVar2 + 0xf0),0xffffffff,700.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG00a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG00a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG00b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  double fVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.blend(SpriteBlend(0));
  images.image(2,(double)iVar1 * 0.6 + 320.0,(double)iVar2 * 0.6 + 200.0,0xffffffff,768.0f,384.0f,3.0f,0.0f,0);
  images.image(1,(double)iVar1 * 0.75 + 320.0,(double)(iVar2 + 0x118),0xffffffff,700.0f,350.0f,1.0f,0.0f,0);
  images.image(1,(double)iVar1 * 0.75 + 320.0,(double)(iVar2 + 0x118),0xffffffff,700.0f,350.0f,1.0f,0.0f,1);
  images.image(0,(double)(iVar1 + 0x140),(double)(iVar2 + 0xf0),0xffffffff,700.0f,620.0f,1.0f,0.0f,0);
  images.blend(SpriteBlend(2));
  g.filter(Filter((2)-1));
  uVar6 = 1;
  uVar5 = 0;
  uVar4 = 0;
  fVar3 = (double)std::sin((double)timer * 0.11);
  images.image(3,(double)(iVar1 + 0x23),(double)(iVar2 + 0x110),0xffffffff,32.0f,32.0f,(double)(fVar3 * (double)0.1 + (double)2.5),float_bits(uVar4),uVar5,uVar6);
  uVar6 = 1;
  uVar5 = 0;
  uVar4 = 0;
  fVar3 = (double)std::sin((double)timer * 0.1 + 2.0);
  images.image(3,(double)(iVar1 + 0x25d),(double)(iVar2 + 0x110),0xffffffff,32.0f,32.0f,(double)(fVar3 * (double)0.1 + (double)2.5),float_bits(uVar4),uVar5,uVar6);
  g.filter(Filter((1)-1));
  images.blend(SpriteBlend(0));
  return;

}

void BackgroundStage::draw_middle_BG00b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG00b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG00c(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.blend(SpriteBlend(0));
  images.image(2,(double)iVar1 * 0.6 + 320.0,(double)iVar2 * 0.6 + 200.0,0xffffffff,768.0f,384.0f,3.0f,0.0f,0);
  images.image(1,(double)iVar1 * 0.75 + 320.0,(double)(iVar2 + 0x118),0xffffffff,700.0f,350.0f,1.0f,0.0f,0);
  images.image(1,(double)iVar1 * 0.75 + 320.0,(double)(iVar2 + 0x118),0xffffffff,700.0f,350.0f,1.0f,0.0f,1);
  images.image(0,(double)(iVar1 + 0x140),(double)(iVar2 + 0xf0),0xffffffff,700.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG00c(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG00c(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG00x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG00x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG00x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG00y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG00y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG00y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG00z(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG00z(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG00z(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG01a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.blend(SpriteBlend(0));
  images.image(2,(double)iVar1 * 0.6 + 320.0,(double)iVar2 * 0.6 + 200.0,0xffffffff,768.0f,384.0f,3.0f,0.0f,0);
  images.image(1,(double)iVar1 * 0.75 + 345.0,(double)(iVar2 + 0xf0),0xffffffff,700.0f,350.0f,1.0f,0.0f,0);
  images.image(0,(double)(iVar1 + 0x140),(double)(iVar2 + 0xf0),0xffffffff,700.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG01a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG01a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG01b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.blend(SpriteBlend(0));
  images.image(2,(double)iVar1 * 0.6 + 320.0,(double)iVar2 * 0.6 + 200.0,0xffffffff,768.0f,384.0f,3.0f,0.0f,0);
  images.image(1,(double)iVar1 * 0.75 + 345.0,(double)(iVar2 + 0xf0),0xffffffff,700.0f,350.0f,1.0f,0.0f,0);
  images.image(0,(double)(iVar1 + 0x140),(double)(iVar2 + 0xf0),0xffffffff,700.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG01b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG01b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG01x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG01x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG01x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG01y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG01y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG01y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG02a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 QuadPoints layerQuad0{};
 QuadPoints layerQuad1{};
 CollisionRectangle textureRect0{};
 CollisionRectangle textureRect1{};

  int iVar1;
  
  
  
  
  
  
  
  
  
  int local_90;
  
  
  iVar1 = i32(double(camera.x)+160.0);
  local_90 = i32(camera.y);
  g.depth(DepthMode(0));
  images.blend(SpriteBlend(0));
  textureRect0.left = 0;
  textureRect0.top = 0;
  textureRect0.right = 0x20;
  textureRect0.bottom = 0x29;
  layerQuad0=rectangle_quad(float(textureRect0.left),float(textureRect0.top),float(textureRect0.right),float(textureRect0.bottom));
  rotate_quad(layerQuad0,16.0f,41.0f,0.0f,-15.0f,(double)(timer << 4),0.0f);
  translate_quad(layerQuad0,(double)(iVar1 + 0x31c),(double)(local_90 + -0x1a),0.0f);
  textureRect1.left = 0;
  textureRect1.top = 0;
  textureRect1.right = 0x20;
  textureRect1.bottom = 0x29;
  layerQuad1=rectangle_quad(float(textureRect1.left),float(textureRect1.top),float(textureRect1.right),float(textureRect1.bottom));
  rotate_quad(layerQuad1,16.0f,41.0f,0.0f,-15.0f,(double)(timer * 0x10 + 0x5a),0.0f)
  ;
  translate_quad(layerQuad1,(double)(iVar1 + 0x31c),(double)(local_90 + -0x1a),0.0f);
  images.image(2,(double)iVar1 * 0.6 + 320.0,(double)local_90 * 0.6 + 200.0,0xffffffff,768.0f,384.0f,3.0f,0.0f,0);
  images.image(1,(double)iVar1 * 0.75 + 320.0,(double)local_90 * 0.75 + 570.0,0xffffffff,600.0f,300.0f,1.0f,0.0f,0);
  images.image(0,(double)(iVar1 + 0x140),(double)(local_90 + 0xf0),0xffffffff,700.0f,620.0f,1.0f,0.0f,0);
  images.projected(3,layerQuad0,textureRect0,320.0f,160.0f,1500.0f,0xffffffff);
  images.projected(4,layerQuad1,textureRect1,320.0f,160.0f,1500.0f,0xffffffff);
  return;

}

void BackgroundStage::draw_middle_BG02a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG02a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  u32 uVar1;
  
  uVar1 = i32(double(camera.x)+160.0);
  i32(camera.y);
  images.blend(SpriteBlend(0));
  return;

}

void BackgroundStage::draw_back_BG02b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 QuadPoints layerQuad0{};
 QuadPoints layerQuad1{};
 CollisionRectangle textureRect0{};
 CollisionRectangle textureRect1{};

  int iVar1;
  
  
  
  
  
  
  
  
  
  int local_90;
  
  
  iVar1 = i32(double(camera.x)+160.0);
  local_90 = i32(camera.y);
  g.depth(DepthMode(0));
  images.blend(SpriteBlend(0));
  textureRect0.left = 0;
  textureRect0.top = 0;
  textureRect0.right = 0x20;
  textureRect0.bottom = 0x29;
  layerQuad0=rectangle_quad(float(textureRect0.left),float(textureRect0.top),float(textureRect0.right),float(textureRect0.bottom));
  rotate_quad(layerQuad0,16.0f,41.0f,0.0f,-15.0f,(double)(timer << 4),0.0f);
  translate_quad(layerQuad0,(double)(iVar1 + 0x31c),(double)(local_90 + -0x1a),0.0f);
  textureRect1.left = 0;
  textureRect1.top = 0;
  textureRect1.right = 0x20;
  textureRect1.bottom = 0x29;
  layerQuad1=rectangle_quad(float(textureRect1.left),float(textureRect1.top),float(textureRect1.right),float(textureRect1.bottom));
  rotate_quad(layerQuad1,16.0f,41.0f,0.0f,-15.0f,(double)(timer * 0x10 + 0x5a),0.0f)
  ;
  translate_quad(layerQuad1,(double)(iVar1 + 0x31c),(double)(local_90 + -0x1a),0.0f);
  images.image(2,(double)iVar1 * 0.6 + 320.0,(double)local_90 * 0.6 + 200.0,0xffffffff,768.0f,384.0f,3.0f,0.0f,0);
  images.image(1,(double)iVar1 * 0.75 + 320.0,(double)local_90 * 0.75 + 570.0,0xffffffff,600.0f,300.0f,1.0f,0.0f,0);
  images.image(0,(double)(iVar1 + 0x140),(double)(local_90 + 0xf0),0xffffffff,700.0f,620.0f,1.0f,0.0f,0);
  images.projected(3,layerQuad0,textureRect0,320.0f,160.0f,1500.0f,0xffffffff);
  images.projected(4,layerQuad1,textureRect1,320.0f,160.0f,1500.0f,0xffffffff);
  return;

}

void BackgroundStage::draw_middle_BG02b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG02b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  u32 uVar1;
  
  uVar1 = i32(double(camera.x)+160.0);
  i32(camera.y);
  images.blend(SpriteBlend(0));
  return;

}

void BackgroundStage::draw_back_BG02c(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 QuadPoints layerQuad0{};
 QuadPoints layerQuad1{};
 CollisionRectangle textureRect0{};
 CollisionRectangle textureRect1{};

  int iVar1;
  
  
  
  
  
  
  
  
  
  int local_90;
  
  
  iVar1 = i32(double(camera.x)+160.0);
  local_90 = i32(camera.y);
  g.depth(DepthMode(0));
  images.blend(SpriteBlend(0));
  textureRect0.left = 0;
  textureRect0.top = 0;
  textureRect0.right = 0x20;
  textureRect0.bottom = 0x29;
  layerQuad0=rectangle_quad(float(textureRect0.left),float(textureRect0.top),float(textureRect0.right),float(textureRect0.bottom));
  rotate_quad(layerQuad0,16.0f,41.0f,0.0f,-15.0f,(double)(timer << 4),0.0f);
  translate_quad(layerQuad0,(double)(iVar1 + 0x31c),(double)(local_90 + -0x1a),0.0f);
  textureRect1.left = 0;
  textureRect1.top = 0;
  textureRect1.right = 0x20;
  textureRect1.bottom = 0x29;
  layerQuad1=rectangle_quad(float(textureRect1.left),float(textureRect1.top),float(textureRect1.right),float(textureRect1.bottom));
  rotate_quad(layerQuad1,16.0f,41.0f,0.0f,-15.0f,(double)(timer * 0x10 + 0x5a),0.0f)
  ;
  translate_quad(layerQuad1,(double)(iVar1 + 0x31c),(double)(local_90 + -0x1a),0.0f);
  images.image(2,(double)iVar1 * 0.6 + 320.0,(double)local_90 * 0.6 + 200.0,0xffffffff,768.0f,384.0f,3.0f,0.0f,0);
  images.image(1,(double)iVar1 * 0.75 + 320.0,(double)local_90 * 0.75 + 570.0,0xffffffff,600.0f,300.0f,1.0f,0.0f,0);
  images.image(0,(double)(iVar1 + 0x140),(double)(local_90 + 0xf0),0xffffffff,700.0f,620.0f,1.0f,0.0f,0);
  images.projected(3,layerQuad0,textureRect0,320.0f,160.0f,1500.0f,0xffffffff);
  images.projected(4,layerQuad1,textureRect1,320.0f,160.0f,1500.0f,0xffffffff);
  return;

}

void BackgroundStage::draw_middle_BG02c(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG02c(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  u32 uVar1;
  
  uVar1 = i32(double(camera.x)+160.0);
  i32(camera.y);
  images.blend(SpriteBlend(0));
  return;

}

void BackgroundStage::draw_back_BG02x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG02x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG02x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG02y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG02y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG02y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG02z(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG02z(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG02z(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG03a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.blend(SpriteBlend(0));
  images.image(2,(double)iVar1 * 0.6 + 320.0,(double)iVar2 * 0.6 + 200.0,0xffffffff,768.0f,384.0f,3.0f,0.0f,0);
  images.image(1,(double)iVar1 * 0.75 + 320.0,(double)(iVar2 + 0x118),0xffffffff,700.0f,350.0f,1.0f,0.0f,1);
  images.image(0,(double)(iVar1 + 0x140),(double)(iVar2 + 0xf0),0xffffffff,700.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG03a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG03a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG03b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.blend(SpriteBlend(0));
  images.image(2,(double)iVar1 * 0.6 + 320.0,(double)iVar2 * 0.6 + 200.0,0xffffffff,768.0f,384.0f,3.0f,0.0f,0);
  images.image(1,(double)iVar1 * 0.75 + 320.0,(double)(iVar2 + 0x118),0xffffffff,700.0f,350.0f,1.0f,0.0f,1);
  images.image(0,(double)(iVar1 + 0x140),(double)(iVar2 + 0xf0),0xffffffff,700.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG03b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG03b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG03x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG03x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG03x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG03y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG03y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG03y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG04a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 std::array<ScreenVertex,6> stripVertices0{};

  int iVar1;
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  int local_8;
  
  iVar1 = i32(double(camera.x)+160.0);
  local_8 = i32(camera.y);
  stripVertices0[0].x = (double)iVar1 + 280.0;
  stripVertices0[0].y = (double)local_8 + -512.0;
  stripVertices0[0].z=0.0f;
  stripVertices0[0].reciprocalW=0.0f;
  stripVertices0[0].color = 0xc8ffffff;
  stripVertices0[0].u=0.0f;
  stripVertices0[0].v = (double)timer * 0.0004;
  stripVertices0[1].x = (double)iVar1 + 360.0;
  stripVertices0[1].y = (double)local_8 + -512.0;
  stripVertices0[1].z=0.0f;
  stripVertices0[1].reciprocalW=0.0f;
  stripVertices0[1].color = 0xc8ffffff;
  stripVertices0[1].u=1.0f;
  stripVertices0[1].v = (double)timer * 0.0004;
  stripVertices0[2].x = (double)iVar1 + 170.0;
  stripVertices0[2].y = (double)local_8 + 192.0;
  stripVertices0[2].z=0.0f;
  stripVertices0[2].reciprocalW=0.0f;
  stripVertices0[2].color = 0xb4ffffff;
  stripVertices0[2].u=0.0f;
  stripVertices0[2].v = (double)timer * 0.0004 + 0.9166666666666666;
  stripVertices0[3].x = (double)iVar1 + 470.0;
  stripVertices0[3].y = (double)local_8 + 192.0;
  stripVertices0[3].z=0.0f;
  stripVertices0[3].reciprocalW=0.0f;
  stripVertices0[3].color = 0xb4ffffff;
  stripVertices0[3].u=1.0f;
  stripVertices0[3].v = (double)timer * 0.0004 + 0.9166666666666666;
  stripVertices0[4].x = (double)iVar1 + 160.0;
  stripVertices0[4].y = (double)local_8 + 256.0;
  stripVertices0[4].z=0.0f;
  stripVertices0[4].reciprocalW=0.0f;
  stripVertices0[4].color = 0xffffff;
  stripVertices0[4].u=0.0f;
  stripVertices0[4].v = (double)timer * 0.0004 + 1.0;
  stripVertices0[5].x = (double)iVar1 + 480.0;
  stripVertices0[5].y = (double)local_8 + 256.0;
  stripVertices0[5].z=0.0f;
  stripVertices0[5].reciprocalW=0.0f;
  stripVertices0[5].color = 0xffffff;
  stripVertices0[5].u=1.0f;
  stripVertices0[5].v = (double)timer * 0.0004 + 1.0;
  g.depth(DepthMode(0));
  images.image(0,(double)(iVar1 + 0x140),(double)(local_8 + 0xf0),0xffffffff,700.0f,620.0f,1.0f,0.0f,0);
  images.blend(SpriteBlend(2));
  images.strip(1,stripVertices0.data(),6);
  images.blend(SpriteBlend(0));
  return;

}

void BackgroundStage::draw_middle_BG04a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG04a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG04b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  double fVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  g.depth(DepthMode(0));
  images.image(0,(double)(iVar1 + 0x140),(double)(iVar2 + 0xf0),0xffffffff,700.0f,620.0f,1.0f,0.0f,0);
  images.blend(SpriteBlend(2));
  g.filter(Filter((2)-1));
  uVar6 = 1;
  uVar5 = 0;
  uVar4 = 0;
  fVar3 = (double)std::sin((double)timer * 0.11);
  images.image(1,(double)(iVar1 + 0xb1),(double)(iVar2 + 0x1e),0xdfffffff,32.0f,32.0f,(double)(fVar3 * (double)0.05 + (double)1.5),float_bits(uVar4),uVar5,uVar6);
  uVar6 = 1;
  uVar5 = 0;
  uVar4 = 0;
  fVar3 = (double)std::sin((double)timer * 0.09 + 1.0);
  images.image(1,(double)(iVar1 + 0x11e),(double)(iVar2 + 0x19),0xbfffffff,32.0f,32.0f,(double)(fVar3 * (double)0.05 + (double)0.699999988079071),float_bits(uVar4),uVar5,uVar6);
  uVar6 = 1;
  uVar5 = 0;
  uVar4 = 0;
  fVar3 = (double)std::sin((double)timer * 0.12 + 2.0);
  images.image(1,(double)(iVar1 + 0x169),(double)(iVar2 + -0x90),0xa8ffffff,32.0f,32.0f,(double)(fVar3 * (double)0.05 + (double)0.699999988079071),float_bits(uVar4),uVar5,uVar6);
  uVar6 = 1;
  uVar5 = 0;
  uVar4 = 0;
  fVar3 = (double)std::sin((double)timer * 0.1 + 2.0);
  images.image(1,(double)(iVar1 + 0x195),(double)(iVar2 + 0x5a),0xbfffffff,32.0f,32.0f,(double)(fVar3 * (double)0.05 + (double)1.0),float_bits(uVar4),uVar5,uVar6);
  uVar6 = 1;
  uVar5 = 0;
  uVar4 = 0;
  fVar3 = (double)std::sin((double)timer * 0.09 + 2.0);
  images.image(1,(double)(iVar1 + 0x201),(double)(iVar2 + -0xd1),0xdfffffff,32.0f,32.0f,(double)(fVar3 * (double)0.05 + (double)1.7000000476837158),float_bits(uVar4),uVar5,uVar6);
  g.filter(Filter((1)-1));
  images.blend(SpriteBlend(0));
  return;

}

void BackgroundStage::draw_middle_BG04b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG04b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG04x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG04x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG04x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG04y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG04y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG04y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG05a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 QuadPoints layerQuad0{};
 QuadPoints layerQuad1{};
 CollisionRectangle textureRect0{};

  int iVar1;
  int iVar2;
  
  
  int local_18;
  
  
  
  
  
  iVar1 = i32(double(camera.x)+160.0);
  local_18 = i32(camera.y);
  g.depth(DepthMode(0));
  images.blend(SpriteBlend(0));
  images.image(2,(double)iVar1 * 0.6 + 320.0,(double)local_18 * 0.6 + 200.0,0xffffffff,768.0f,384.0f,3.0f,0.0f,0);
  images.image(1,(double)iVar1 * 0.75 + 320.0,(double)local_18 * 0.75 + 150.0,0xffffffff,700.0f,200.0f,1.0f,0.0f,0);
  iVar2 = i32((1536.0-double(timer)*0.4)/384.0);
  textureRect0.left = 0;
  textureRect0.top = 0;
  textureRect0.right = iVar2 << 7;
  textureRect0.bottom = 0x80;
  g.filter(Filter((2)-1));
  layerQuad0=rectangle_quad(0.0f,0.0f,(double)(iVar2 * 0x180),256.0f);
  translate_quad(layerQuad0,(double)timer * 0.4 + (double)iVar1 * 0.75,(double)local_18 * 0.75 + 160.0,0.0f);
  images.projected(3,layerQuad0,textureRect0,320.0f,240.0f,1500.0f,0xffffffff);
  textureRect0.left = 0;
  textureRect0.top = 0;
  textureRect0.right = (5 - iVar2) * 0x80;
  textureRect0.bottom = 0x80;
  layerQuad1=rectangle_quad(0.0f,0.0f,(double)((5 - iVar2) * 0x180),256.0f);
  translate_quad(layerQuad1,((double)timer * 0.4 + (double)iVar1 * 0.75) -
               (double)((5 - iVar2) * 0x180),(double)local_18 * 0.75 + 160.0,0.0f);
  images.projected(3,layerQuad1,textureRect0,320.0f,240.0f,1500.0f,0xffffffff);
  g.filter(Filter((1)-1));
  images.image(0,(double)(iVar1 + 0x140),(double)(local_18 + 0xf0),0xffffffff,700.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG05a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG05a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG05b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 QuadPoints layerQuad0{};
 QuadPoints layerQuad1{};
 CollisionRectangle textureRect0{};

  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int local_144;
  
  int local_b4;
  int local_b0;
  int local_ac;
  int local_a8;
  
  int local_1c;
  
  
  
  
  int local_8;
  
  iVar1 = i32(double(camera.x)+160.0);
  local_1c = i32(camera.y);

  local_a8 = i32(std::sin(double(secondaryTimer)*0.1+1.0)*5.0);

  iVar2 = i32(std::sin(double(secondaryTimer)*0.1+2.0)*4.0);

  local_b0 = i32(std::sin(double(secondaryTimer)*0.1+3.0)*5.0);

  iVar3 = i32(std::sin(double(secondaryTimer)*0.1+4.0)*6.0);
  iVar4 = animationFrame / 3;
  local_b4 = iVar4 + 4;
  local_144 = iVar4 + 7;
  local_8 = iVar4 + 0xb;
  local_ac = iVar4 + 0xf;
  if (0x11 < iVar4 + 7) {
    local_144 = iVar4 + -7;
  }
  if (0x11 < iVar4 + 0xb) {
    local_8 = iVar4 + -3;
  }
  if (0x11 < iVar4 + 0xf) {
    local_ac = iVar4 + 1;
  }
  g.depth(DepthMode(0x0));
  images.blend(SpriteBlend(0x0));
  images.image(2,(double)iVar1 * 0.6 + 320.0,(double)local_1c * 0.6 + 200.0,0xffffffff,768.0f,384.0f,3.0f,0.0f,0);
  images.image(1,(double)iVar1 * 0.75 + 320.0,(double)local_1c * 0.75 + 150.0,0xffffffff,700.0f,200.0f,1.0f,0.0f,0);
  iVar4 = i32((1536.0-double(timer)*0.4)/384.0);
  textureRect0.left = 0;
  textureRect0.top = 0;
  textureRect0.right = iVar4 << 7;
  textureRect0.bottom = 0x80;
  g.filter(Filter((0x2)-1));
  layerQuad0=rectangle_quad(0.0f,0.0f,(double)(iVar4 * 0x180),256.0f);
  translate_quad(layerQuad0,(double)timer * 0.4 + (double)iVar1 * 0.75,(double)local_1c * 0.75 + 160.0,0.0f);
  images.projected(3,layerQuad0,textureRect0,320.0f,240.0f,1500.0f,0x7fffffff);
  textureRect0.left = 0;
  textureRect0.top = 0;
  textureRect0.right = (5 - iVar4) * 0x80;
  textureRect0.bottom = 0x80;
  layerQuad1=rectangle_quad(0.0f,0.0f,(double)((5 - iVar4) * 0x180),256.0f);
  translate_quad(layerQuad1,((double)timer * 0.4 + (double)iVar1 * 0.75) -
               (double)((5 - iVar4) * 0x180),(double)local_1c * 0.75 + 160.0,0.0f);
  images.projected(3,layerQuad1,textureRect0,320.0f,240.0f,1500.0f,0x7fffffff);
  g.filter(Filter((0x1)-1));
  images.blend(SpriteBlend(0x2));
  g.filter(Filter((0x2)-1));
  images.image(local_144,(double)(iVar1 + -0x104),(double)(local_1c + 200 + iVar3),0x87ffffff,32.0f,64.0f,0.6000000238418579f,0.0f,0);
  images.image(local_8,(double)(iVar1 + 0x2a8),(double)(local_1c + 0xaa + local_b0),0x87ffffff,32.0f,64.0f,1.0f,0.0f,0);
  g.filter(Filter((0x1)-1));
  images.blend(SpriteBlend(0x0));
  images.image(0,(double)(iVar1 + 0x140),(double)(local_1c + 0xf0),0xffffffff,700.0f,620.0f,1.0f,0.0f,0);
  images.blend(SpriteBlend(0x2));
  g.filter(Filter((0x2)-1));
  images.image(local_ac,(double)(iVar1 + -0xfa),(double)(local_1c + 0xf0 + iVar3),0x87ffffff,32.0f,64.0f,1.100000023841858f,0.0f,0);
  images.image(local_8,(double)(iVar1 + -0x96),(double)(local_1c + 200 + local_b0),0x87ffffff,32.0f,64.0f,1.0f,0.0f,0);
  images.image(local_b4,(double)(iVar1 + 0x96),(double)(local_1c + 200 + local_a8),0x87ffffff,32.0f,64.0f,1.0f,0.0f,0);
  images.image(local_8,(double)(iVar1 + 0x168),(double)(local_1c + 0xbe + iVar3),0x87ffffff,32.0f,64.0f,0.800000011920929f,0.0f,0);
  images.image(local_b4,(double)(iVar1 + 0x1c2),(double)(local_1c + 200 + local_a8),0x87ffffff,32.0f,64.0f,1.0f,0.0f,0);
  images.image(local_144,(double)(iVar1 + 0x28a),(double)(local_1c + 0xe6 + iVar2),0x87ffffff,32.0f,64.0f,1.2999999523162842f,0.0f,0);
  images.image(local_8,(double)(iVar1 + 0x2f8),(double)(local_1c + 200 + iVar3),0x87ffffff,32.0f,64.0f,0.800000011920929f,0.0f,0);
  g.filter(Filter((0x1)-1));
  images.blend(SpriteBlend(0x0));
  return;

}

void BackgroundStage::draw_middle_BG05b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG05b(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG05x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG05x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG05x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG05y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG05y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG05y(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG06a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  double fVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  g.depth(DepthMode(0));
  images.image(0,(double)(iVar1 + 0x140),(double)(iVar2 + 0xf0),0xffffffff,700.0f,620.0f,1.0f,0.0f,0);
  images.blend(SpriteBlend(2));
  g.filter(Filter((2)-1));
  uVar6 = 1;
  uVar5 = 0;
  uVar4 = 0;
  fVar3 = (double)std::sin((double)timer * 0.11);
  images.image(4,(double)(iVar1 + -0xbe),(double)(iVar2 + 0x3c),0xffffffff,32.0f,32.0f,(double)(fVar3 * (double)0.05 + (double)1.2000000476837158),float_bits(uVar4),uVar5,uVar6);
  uVar6 = 1;
  uVar5 = 0;
  uVar4 = 0;
  fVar3 = (double)std::sin((double)timer * 0.09 + 1.0);
  images.image(4,(double)(iVar1 + 0xa7),(double)(iVar2 + 0x86),0xffffffff,32.0f,32.0f,(double)(fVar3 * (double)0.05 + (double)0.800000011920929),float_bits(uVar4),uVar5,uVar6);
  uVar6 = 1;
  uVar5 = 0;
  uVar4 = 0;
  fVar3 = (double)std::sin((double)timer * 0.12 + 2.0);
  images.image(4,(double)(iVar1 + 0x1db),(double)(iVar2 + 0x86),0xffffffff,32.0f,32.0f,(double)(fVar3 * (double)0.05 + (double)0.800000011920929),float_bits(uVar4),uVar5,uVar6);
  uVar6 = 1;
  uVar5 = 0;
  uVar4 = 0;
  fVar3 = (double)std::sin((double)timer * 0.1 + 2.0);
  images.image(4,(double)(iVar1 + 0x341),(double)(iVar2 + 0x3c),0xffffffff,32.0f,32.0f,(double)(fVar3 * (double)0.05 + (double)1.2000000476837158),float_bits(uVar4),uVar5,uVar6);
  g.filter(Filter((1)-1));
  g.depth(DepthMode(1));
  g.depth(DepthMode(8));
  return;

}

void BackgroundStage::draw_middle_BG06a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 QuadPoints layerQuad0{};
 QuadPoints layerQuad1{};
 CollisionRectangle textureRect0{};
 CollisionRectangle textureRect1{};

  
  int local_b8;
  
  
  
  
  
  int local_18;
  
  
  
  
  
  local_b8 = i32(double(camera.x)+160.0);
  local_18 = i32(camera.y);
  textureRect0.left = 0;
  textureRect0.top = 0;
  textureRect0.right = 0x100;
  textureRect0.bottom = 0x100;
  layerQuad0=rectangle_quad(float(textureRect0.left),float(textureRect0.top),float(textureRect0.right),float(textureRect0.bottom));
  rotate_quad(layerQuad0,128.0f,128.0f,0.0f,85.0f,0.0f,(double)timer * 0.2);
  scale_quad(layerQuad0,128.0f,128.0f,0.0f,3.5f,3.5f,1.0f);
  translate_quad(layerQuad0,(double)(local_b8 + 0xc0),(double)(local_18 + 0x122),0.0f);
  layerQuad1=rectangle_quad(float(textureRect0.left),float(textureRect0.top),float(textureRect0.right),float(textureRect0.bottom));
  rotate_quad(layerQuad1,128.0f,128.0f,0.0f,85.0f,0.0f,(double)-timer * 0.2);
  scale_quad(layerQuad1,128.0f,128.0f,0.0f,4.0f,4.0f,1.0f);
  translate_quad(layerQuad1,(double)(local_b8 + 0xc0),(double)(local_18 + 0x122),0.0f);
  textureRect1.left = 0;
  textureRect1.top = 0;
  textureRect1.right = 0x100;
  textureRect1.bottom = 0x100;
  images.blend(SpriteBlend(2));
  g.filter(Filter((2)-1));
  images.quad(1,layerQuad0,textureRect1,0xffffffff);
  images.quad(2,layerQuad1,textureRect1,0xffffffff);
  g.filter(Filter((1)-1));
  return;

}

void BackgroundStage::draw_front_BG06a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 QuadPoints layerQuad0{};
 QuadPoints layerQuad1{};
 CollisionRectangle textureRect0{};

  int iVar1;
  double fVar2;
  u32 uVar3;
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  int local_18;
  
  
  
  
  
  iVar1 = i32(double(camera.x)+160.0);
  local_18 = i32(camera.y);
  textureRect0.left = 0;
  textureRect0.top = 0;
  textureRect0.right = 0x100;
  textureRect0.bottom = 0x80;
  ;
  layerQuad0[0][0]=0.0f;
  layerQuad0[0][1]=0.0f;
  layerQuad0[0][2]=0.0f;
  layerQuad0[3][0]=0.0f;
  layerQuad0[3][1]=128.0f;
  layerQuad0[3][2]=0.0f;
  layerQuad0[1][0]=256.0f;
  layerQuad0[1][1]=0.0f;
  layerQuad0[1][2]=0.0f;
  layerQuad0[2][0]=256.0f;
  layerQuad0[2][1]=128.0f;
  layerQuad0[2][2]=0.0f;
  uVar3 = 0x3f800000;
  fVar2 = (double)std::sin((double)(((double)timer / 0.017453288659453392) * 0.001));
  scale_quad(layerQuad0,0.0f,0.0f,0.0f,2.0f,(double)((fVar2 + (double)6.0) * (double)0.1),float_bits(uVar3));
  translate_quad(layerQuad0,(double)(iVar1 + 0x140),(double)(local_18 + 0x184),0.25f);
  ;
  layerQuad1[0][0]=256.0f;
  layerQuad1[0][1]=0.0f;
  layerQuad1[0][2]=0.0f;
  layerQuad1[3][0]=256.0f;
  layerQuad1[3][1]=128.0f;
  layerQuad1[3][2]=0.0f;
  layerQuad1[1][0]=0.0f;
  layerQuad1[1][1]=0.0f;
  layerQuad1[1][2]=0.0f;
  layerQuad1[2][0]=0.0f;
  layerQuad1[2][1]=128.0f;
  layerQuad1[2][2]=0.0f;
  uVar3 = 0x3f800000;
  fVar2 = (double)std::sin((double)(((double)timer / 0.017453288659453392) * 0.001));
  scale_quad(layerQuad1,0.0f,0.0f,0.0f,2.0f,(double)((fVar2 + (double)6.0) * (double)0.1),float_bits(uVar3));
  translate_quad(layerQuad1,(double)(iVar1 + -0xc0),(double)(local_18 + 0x184),0.25f);
  g.depth(DepthMode(3));
  g.depth(DepthMode(7));
  images.blend(SpriteBlend(2));
  images.quad(3,layerQuad0,textureRect0,0xffffffff);
  images.quad(3,layerQuad1,textureRect0,0xffffffff);
  g.depth(DepthMode(4));
  return;

}

void BackgroundStage::draw_back_BG06x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG06x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG06x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG07a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 QuadPoints layerQuad0{};
 QuadPoints layerQuad1{};
 CollisionRectangle textureRect0{};
 CollisionRectangle textureRect1{};

  int iVar1;
  int iVar2;
  
  
  
  int local_18;
  
  
  
  
  
  iVar1 = i32(double(camera.x)+160.0);
  local_18 = i32(camera.y);
  textureRect0={0,0,0x3c0,0x2d0};
  images.blend(SpriteBlend(0));
  g.filled_rectangle(textureRect0,0xff21212c);
  images.image(3,(double)iVar1 * 0.75 + 320.0,(double)local_18 * 0.75 + 240.0,0xffffffff,64.0f,64.0f,1.0f,0.0f,0);
  iVar2 = i32((3072.0-double(timer)*0.1)/768.0);
  textureRect1.left = 0;
  textureRect1.top = 1;
  textureRect1.right = iVar2 << 7;
  textureRect1.bottom = 0x80;
  g.filter(Filter((2)-1));
  layerQuad0=rectangle_quad(0.0f,1.0f,(double)(iVar2 * 0x300),256.0f);
  translate_quad(layerQuad0,(double)timer * 0.1 + (double)iVar1 * 0.75,(double)local_18 * 0.75 + 120.0,0.0f);
  images.projected(2,layerQuad0,textureRect1,320.0f,240.0f,1500.0f,0x64ffffff);
  textureRect1.left = 0;
  textureRect1.top = 1;
  textureRect1.right = (5 - iVar2) * 0x80;
  textureRect1.bottom = 0x80;
  layerQuad1=rectangle_quad(0.0f,1.0f,(double)((5 - iVar2) * 0x300),256.0f);
  translate_quad(layerQuad1,((double)timer * 0.1 + (double)iVar1 * 0.75) -
               (double)((5 - iVar2) * 0x300),(double)local_18 * 0.75 + 120.0,0.0f);
  images.projected(2,layerQuad1,textureRect1,320.0f,240.0f,1500.0f,0x64ffffff);
  g.filter(Filter((1)-1));
  images.image(1,(double)(iVar1 + 0x140),(double)local_18 * 0.75 + 60.0,0xffffffff,700.0f,350.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x140),(double)local_18 * 0.75 + 60.0,0xffffffff,700.0f,350.0f,1.0f,0.0f,1);
  images.image(0,(double)(iVar1 + 0x140),(double)(local_18 + 0xf0),0xffffffff,700.0f,120.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG07a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG07a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG07x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG07x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG07x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_back_BG08a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  double fVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.blend(SpriteBlend(0));
  images.image(2,(double)iVar1 * 0.6 + 384.0,(double)iVar2 * 0.6 + 200.0,0xffffffff,768.0f,384.0f,3.0f,0.0f,0);
  images.image(1,(double)iVar1 * 0.75 + 320.0,(double)(iVar2 + 0x118),0xffffffff,700.0f,350.0f,1.0f,0.0f,0);
  images.image(4,(double)iVar1 * 0.75 + 320.0,(double)(iVar2 + 0x118),0xffffffff,700.0f,350.0f,1.0f,0.0f,1);
  images.image(0,(double)(iVar1 + 0x140),(double)(iVar2 + 0xf0),0xffffffff,700.0f,620.0f,1.0f,0.0f,0);
  images.blend(SpriteBlend(2));
  g.filter(Filter((2)-1));
  uVar6 = 1;
  uVar5 = 0;
  uVar4 = 0;
  fVar3 = (double)std::sin((double)timer * 0.1 + 2.0);
  images.image(3,(double)(iVar1 + 0x25d),(double)(iVar2 + 0x110),0xffffffff,32.0f,32.0f,(double)(fVar3 * (double)0.1 + (double)2.5),float_bits(uVar4),uVar5,uVar6);
  images.image(animationFrame / 3 + 0x1d,(double)(iVar1 + 0xdd),(double)(iVar2 + -0x54),0xffffffff,32.0f,128.0f,2.0f,0.0f,0);
  images.image(animationFrame / 3 + 5,(double)(iVar1 + 0x34),(double)(iVar2 + 0x1ac),0xffffffff,256.0f,128.0f,2.0f,0.0f,0);
  g.filter(Filter((1)-1));
  images.blend(SpriteBlend(0));
  return;

}

void BackgroundStage::draw_middle_BG08a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG08a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 QuadPoints layerQuad0{};
 CollisionRectangle textureRect0{};

  int iVar1;
  double fVar2;
  u32 uVar3;
  
  
  
  
  
  
  
  
  
  
  
  
  int local_18;
  
  
  
  
  
  iVar1 = i32(double(camera.x)+160.0);
  local_18 = i32(camera.y);
  textureRect0.left = 0;
  textureRect0.top = 0;
  textureRect0.right = 0x40;
  textureRect0.bottom = 0x100;
  ;
  layerQuad0[0][0]=0.0f;
  layerQuad0[0][1]=0.0f;
  layerQuad0[0][2]=0.0f;
  layerQuad0[3][0]=0.0f;
  layerQuad0[3][1]=128.0f;
  layerQuad0[3][2]=0.0f;
  layerQuad0[1][0]=64.0f;
  layerQuad0[1][1]=0.0f;
  layerQuad0[1][2]=0.0f;
  layerQuad0[2][0]=64.0f;
  layerQuad0[2][1]=128.0f;
  layerQuad0[2][2]=0.0f;
  uVar3 = 0x3f800000;
  fVar2 = (double)std::sin((double)(((double)timer / 0.017453288659453392) * 0.0005));
  scale_quad(layerQuad0,0.0f,0.0f,0.0f,1.0f,(double)((fVar2 + (double)12.0) * (double)0.1),float_bits(uVar3));
  translate_quad(layerQuad0,(double)(iVar1 + 0x6e),(double)(local_18 + 0x184),0.25f);
  g.depth(DepthMode(7));
  images.blend(SpriteBlend(2));
  images.quad(0x35,layerQuad0,textureRect0,0xffffffff);
  return;

}

void BackgroundStage::draw_back_BG08x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG08x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG08x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_middle_BG09a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG09a(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 QuadPoints layerQuad0{};
 CollisionRectangle textureRect0{};

  int iVar1;
  double fVar2;
  u32 uVar3;
  
  
  
  
  
  
  
  
  
  
  
  
  int local_18;
  
  
  
  
  
  iVar1 = i32(double(camera.x)+160.0);
  local_18 = i32(camera.y);
  textureRect0.left = 0;
  textureRect0.top = 0;
  textureRect0.right = 0x40;
  textureRect0.bottom = 0x100;
  ;
  layerQuad0[0][0]=0.0f;
  layerQuad0[0][1]=0.0f;
  layerQuad0[0][2]=0.0f;
  layerQuad0[3][0]=0.0f;
  layerQuad0[3][1]=128.0f;
  layerQuad0[3][2]=0.0f;
  layerQuad0[1][0]=64.0f;
  layerQuad0[1][1]=0.0f;
  layerQuad0[1][2]=0.0f;
  layerQuad0[2][0]=64.0f;
  layerQuad0[2][1]=128.0f;
  layerQuad0[2][2]=0.0f;
  uVar3 = 0x3f800000;
  fVar2 = (double)std::sin((double)(((double)timer / 0.017453288659453392) * 0.0005));
  scale_quad(layerQuad0,32.0f,0.0f,0.0f,32.0f,(double)((fVar2 + (double)12.0) * (double)0.1),float_bits(uVar3));
  translate_quad(layerQuad0,(double)(iVar1 + 0xf0),(double)(local_18 + 0x184),0.25f);
  g.depth(DepthMode(3));
  g.depth(DepthMode(7));
  images.blend(SpriteBlend(2));
  images.quad(4,layerQuad0,textureRect0,0xffffffff);
  images.blend(SpriteBlend(0));
  return;

}

void BackgroundStage::draw_back_BG09x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  int iVar1;
  int iVar2;
  
  iVar1 = i32(double(camera.x)+160.0);
  iVar2 = i32(camera.y);
  images.image(0,(double)(iVar1 + -0x1e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  images.image(1,(double)(iVar1 + 0x29e),(double)(iVar2 + 0xf0),0xffffffff,350.0f,620.0f,1.0f,0.0f,0);
  return;

}

void BackgroundStage::draw_middle_BG09x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::draw_front_BG09x(ImageRenderer& images,const CameraMotion& camera){
 auto& g=images.graphics;
 

  return;

}

void BackgroundStage::reset(BackgroundKind value){kind=value;timer=animationFrame=secondaryTimer=0;waterVertices={};switch(kind){
case BackgroundKind::BG00a:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG00b:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG00c:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG00x:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG00y:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG00z:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG01a:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG01b:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG01x:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG01y:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG02a:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG02b:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG02c:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG02x:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG02y:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG02z:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG03a:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG03b:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG03x:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG03y:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG04a:shadowOrigin={640.0f,100.0f,100.0f};break;
case BackgroundKind::BG04b:shadowOrigin={640.0f,100.0f,100.0f};break;
case BackgroundKind::BG04x:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG04y:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG05a:shadowOrigin={640.0f,100.0f,100.0f};break;
case BackgroundKind::BG05b:shadowOrigin={640.0f,100.0f,100.0f};break;
case BackgroundKind::BG05x:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG05y:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG06a:shadowOrigin={640.0f,100.0f,100.0f};break;
case BackgroundKind::BG06x:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG07a:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG07x:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG08a:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG08x:shadowOrigin={0.0f,0.0f,0.0f};break;
case BackgroundKind::BG09a:shadowOrigin={640.0f,100.0f,100.0f};break;
case BackgroundKind::BG09x:shadowOrigin={0.0f,0.0f,0.0f};break;
}}
void BackgroundStage::advance(){switch(kind){
case BackgroundKind::BG00a:{
  return;
}break;
case BackgroundKind::BG00b:{
  timer=increment_wrapped(timer,1);
  return;
}break;
case BackgroundKind::BG00c:{
  return;
}break;
case BackgroundKind::BG00x:{
  return;
}break;
case BackgroundKind::BG00y:{
  return;
}break;
case BackgroundKind::BG00z:{
  return;
}break;
case BackgroundKind::BG01a:{
  return;
}break;
case BackgroundKind::BG01b:{
  return;
}break;
case BackgroundKind::BG01x:{
  return;
}break;
case BackgroundKind::BG01y:{
  return;
}break;
case BackgroundKind::BG02a:{
  timer=increment_wrapped(timer,1);
  if (0x168 < timer) {
    timer = 0;
  }
  return;
}break;
case BackgroundKind::BG02b:{
  timer=increment_wrapped(timer,1);
  if (0x168 < timer) {
    timer = 0;
  }
  return;
}break;
case BackgroundKind::BG02c:{
  timer=increment_wrapped(timer,1);
  if (0x168 < timer) {
    timer = 0;
  }
  return;
}break;
case BackgroundKind::BG02x:{
  return;
}break;
case BackgroundKind::BG02y:{
  return;
}break;
case BackgroundKind::BG02z:{
  return;
}break;
case BackgroundKind::BG03a:{
  return;
}break;
case BackgroundKind::BG03b:{
  return;
}break;
case BackgroundKind::BG03x:{
  return;
}break;
case BackgroundKind::BG03y:{
  return;
}break;
case BackgroundKind::BG04a:{
  timer=increment_wrapped(timer,1);
  if (2500.0 < (double)timer) {
    timer = 0;
  }
  return;
}break;
case BackgroundKind::BG04b:{
  timer=increment_wrapped(timer,1);
  return;
}break;
case BackgroundKind::BG04x:{
  return;
}break;
case BackgroundKind::BG04y:{
  return;
}break;
case BackgroundKind::BG05a:{
  timer=increment_wrapped(timer,-1);
  if (timer < 0) {
    timer = 0xf00;
  }
  return;
}break;
case BackgroundKind::BG05b:{
  timer=increment_wrapped(timer,-1);
  if (timer < 0) {
    timer = 0xf00;
  }
  animationFrame=increment_wrapped(animationFrame,1);
  if (0x29 < animationFrame) {
    animationFrame = 0;
  }
  secondaryTimer=increment_wrapped(secondaryTimer,1);
  return;
}break;
case BackgroundKind::BG05x:{
  return;
}break;
case BackgroundKind::BG05y:{
  return;
}break;
case BackgroundKind::BG06a:{
  timer=increment_wrapped(timer,1);
  return;
}break;
case BackgroundKind::BG06x:{
  return;
}break;
case BackgroundKind::BG07a:{
  timer=increment_wrapped(timer,1);
  if (15360.0 <= (double)timer) {
    timer = 0;
  }
  return;
}break;
case BackgroundKind::BG07x:{
  return;
}break;
case BackgroundKind::BG08a:{
  timer=increment_wrapped(timer,1);
  animationFrame=increment_wrapped(animationFrame,1);
  if (0x47 < animationFrame) {
    animationFrame = 0;
  }
  return;
}break;
case BackgroundKind::BG08x:{
  return;
}break;
case BackgroundKind::BG09a:{
  timer=increment_wrapped(timer,1);
  return;
}break;
case BackgroundKind::BG09x:{
  return;
}break;
}}
void BackgroundStage::render_back(ImageRenderer& images,const CameraMotion& camera){switch(kind){case BackgroundKind::BG00a:draw_back_BG00a(images,camera);break;
case BackgroundKind::BG00b:draw_back_BG00b(images,camera);break;
case BackgroundKind::BG00c:draw_back_BG00c(images,camera);break;
case BackgroundKind::BG00x:draw_back_BG00x(images,camera);break;
case BackgroundKind::BG00y:draw_back_BG00y(images,camera);break;
case BackgroundKind::BG00z:draw_back_BG00z(images,camera);break;
case BackgroundKind::BG01a:draw_back_BG01a(images,camera);break;
case BackgroundKind::BG01b:draw_back_BG01b(images,camera);break;
case BackgroundKind::BG01x:draw_back_BG01x(images,camera);break;
case BackgroundKind::BG01y:draw_back_BG01y(images,camera);break;
case BackgroundKind::BG02a:draw_back_BG02a(images,camera);break;
case BackgroundKind::BG02b:draw_back_BG02b(images,camera);break;
case BackgroundKind::BG02c:draw_back_BG02c(images,camera);break;
case BackgroundKind::BG02x:draw_back_BG02x(images,camera);break;
case BackgroundKind::BG02y:draw_back_BG02y(images,camera);break;
case BackgroundKind::BG02z:draw_back_BG02z(images,camera);break;
case BackgroundKind::BG03a:draw_back_BG03a(images,camera);break;
case BackgroundKind::BG03b:draw_back_BG03b(images,camera);break;
case BackgroundKind::BG03x:draw_back_BG03x(images,camera);break;
case BackgroundKind::BG03y:draw_back_BG03y(images,camera);break;
case BackgroundKind::BG04a:draw_back_BG04a(images,camera);break;
case BackgroundKind::BG04b:draw_back_BG04b(images,camera);break;
case BackgroundKind::BG04x:draw_back_BG04x(images,camera);break;
case BackgroundKind::BG04y:draw_back_BG04y(images,camera);break;
case BackgroundKind::BG05a:draw_back_BG05a(images,camera);break;
case BackgroundKind::BG05b:draw_back_BG05b(images,camera);break;
case BackgroundKind::BG05x:draw_back_BG05x(images,camera);break;
case BackgroundKind::BG05y:draw_back_BG05y(images,camera);break;
case BackgroundKind::BG06a:draw_back_BG06a(images,camera);break;
case BackgroundKind::BG06x:draw_back_BG06x(images,camera);break;
case BackgroundKind::BG07a:draw_back_BG07a(images,camera);break;
case BackgroundKind::BG07x:draw_back_BG07x(images,camera);break;
case BackgroundKind::BG08a:draw_back_BG08a(images,camera);break;
case BackgroundKind::BG08x:draw_back_BG08x(images,camera);break;
case BackgroundKind::BG09a:draw_back_BG09a(images,camera);break;
case BackgroundKind::BG09x:draw_back_BG09x(images,camera);break;}}
void BackgroundStage::render_middle(ImageRenderer& images,const CameraMotion& camera){switch(kind){case BackgroundKind::BG00a:draw_middle_BG00a(images,camera);break;
case BackgroundKind::BG00b:draw_middle_BG00b(images,camera);break;
case BackgroundKind::BG00c:draw_middle_BG00c(images,camera);break;
case BackgroundKind::BG00x:draw_middle_BG00x(images,camera);break;
case BackgroundKind::BG00y:draw_middle_BG00y(images,camera);break;
case BackgroundKind::BG00z:draw_middle_BG00z(images,camera);break;
case BackgroundKind::BG01a:draw_middle_BG01a(images,camera);break;
case BackgroundKind::BG01b:draw_middle_BG01b(images,camera);break;
case BackgroundKind::BG01x:draw_middle_BG01x(images,camera);break;
case BackgroundKind::BG01y:draw_middle_BG01y(images,camera);break;
case BackgroundKind::BG02a:draw_middle_BG02a(images,camera);break;
case BackgroundKind::BG02b:draw_middle_BG02b(images,camera);break;
case BackgroundKind::BG02c:draw_middle_BG02c(images,camera);break;
case BackgroundKind::BG02x:draw_middle_BG02x(images,camera);break;
case BackgroundKind::BG02y:draw_middle_BG02y(images,camera);break;
case BackgroundKind::BG02z:draw_middle_BG02z(images,camera);break;
case BackgroundKind::BG03a:draw_middle_BG03a(images,camera);break;
case BackgroundKind::BG03b:draw_middle_BG03b(images,camera);break;
case BackgroundKind::BG03x:draw_middle_BG03x(images,camera);break;
case BackgroundKind::BG03y:draw_middle_BG03y(images,camera);break;
case BackgroundKind::BG04a:draw_middle_BG04a(images,camera);break;
case BackgroundKind::BG04b:draw_middle_BG04b(images,camera);break;
case BackgroundKind::BG04x:draw_middle_BG04x(images,camera);break;
case BackgroundKind::BG04y:draw_middle_BG04y(images,camera);break;
case BackgroundKind::BG05a:draw_middle_BG05a(images,camera);break;
case BackgroundKind::BG05b:draw_middle_BG05b(images,camera);break;
case BackgroundKind::BG05x:draw_middle_BG05x(images,camera);break;
case BackgroundKind::BG05y:draw_middle_BG05y(images,camera);break;
case BackgroundKind::BG06a:draw_middle_BG06a(images,camera);break;
case BackgroundKind::BG06x:draw_middle_BG06x(images,camera);break;
case BackgroundKind::BG07a:draw_middle_BG07a(images,camera);break;
case BackgroundKind::BG07x:draw_middle_BG07x(images,camera);break;
case BackgroundKind::BG08a:draw_middle_BG08a(images,camera);break;
case BackgroundKind::BG08x:draw_middle_BG08x(images,camera);break;
case BackgroundKind::BG09a:draw_middle_BG09a(images,camera);break;
case BackgroundKind::BG09x:draw_middle_BG09x(images,camera);break;}}
void BackgroundStage::render_front(ImageRenderer& images,const CameraMotion& camera){switch(kind){case BackgroundKind::BG00a:draw_front_BG00a(images,camera);break;
case BackgroundKind::BG00b:draw_front_BG00b(images,camera);break;
case BackgroundKind::BG00c:draw_front_BG00c(images,camera);break;
case BackgroundKind::BG00x:draw_front_BG00x(images,camera);break;
case BackgroundKind::BG00y:draw_front_BG00y(images,camera);break;
case BackgroundKind::BG00z:draw_front_BG00z(images,camera);break;
case BackgroundKind::BG01a:draw_front_BG01a(images,camera);break;
case BackgroundKind::BG01b:draw_front_BG01b(images,camera);break;
case BackgroundKind::BG01x:draw_front_BG01x(images,camera);break;
case BackgroundKind::BG01y:draw_front_BG01y(images,camera);break;
case BackgroundKind::BG02a:draw_front_BG02a(images,camera);break;
case BackgroundKind::BG02b:draw_front_BG02b(images,camera);break;
case BackgroundKind::BG02c:draw_front_BG02c(images,camera);break;
case BackgroundKind::BG02x:draw_front_BG02x(images,camera);break;
case BackgroundKind::BG02y:draw_front_BG02y(images,camera);break;
case BackgroundKind::BG02z:draw_front_BG02z(images,camera);break;
case BackgroundKind::BG03a:draw_front_BG03a(images,camera);break;
case BackgroundKind::BG03b:draw_front_BG03b(images,camera);break;
case BackgroundKind::BG03x:draw_front_BG03x(images,camera);break;
case BackgroundKind::BG03y:draw_front_BG03y(images,camera);break;
case BackgroundKind::BG04a:draw_front_BG04a(images,camera);break;
case BackgroundKind::BG04b:draw_front_BG04b(images,camera);break;
case BackgroundKind::BG04x:draw_front_BG04x(images,camera);break;
case BackgroundKind::BG04y:draw_front_BG04y(images,camera);break;
case BackgroundKind::BG05a:draw_front_BG05a(images,camera);break;
case BackgroundKind::BG05b:draw_front_BG05b(images,camera);break;
case BackgroundKind::BG05x:draw_front_BG05x(images,camera);break;
case BackgroundKind::BG05y:draw_front_BG05y(images,camera);break;
case BackgroundKind::BG06a:draw_front_BG06a(images,camera);break;
case BackgroundKind::BG06x:draw_front_BG06x(images,camera);break;
case BackgroundKind::BG07a:draw_front_BG07a(images,camera);break;
case BackgroundKind::BG07x:draw_front_BG07x(images,camera);break;
case BackgroundKind::BG08a:draw_front_BG08a(images,camera);break;
case BackgroundKind::BG08x:draw_front_BG08x(images,camera);break;
case BackgroundKind::BG09a:draw_front_BG09a(images,camera);break;
case BackgroundKind::BG09x:draw_front_BG09x(images,camera);break;}}
const char* BackgroundStage::resource_path()const{switch(kind){case BackgroundKind::BG00a:return "data/background/BG00a.dat";
case BackgroundKind::BG00b:return "data/background/BG00b.dat";
case BackgroundKind::BG00c:return "data/background/BG00c.dat";
case BackgroundKind::BG00x:return "data/background/BG00x.dat";
case BackgroundKind::BG00y:return "data/background/BG00y.dat";
case BackgroundKind::BG00z:return "data/background/BG00z.dat";
case BackgroundKind::BG01a:return "data/background/BG01a.dat";
case BackgroundKind::BG01b:return "data/background/BG01b.dat";
case BackgroundKind::BG01x:return "data/background/BG01x.dat";
case BackgroundKind::BG01y:return "data/background/BG01y.dat";
case BackgroundKind::BG02a:return "data/background/BG02a.dat";
case BackgroundKind::BG02b:return "data/background/BG02b.dat";
case BackgroundKind::BG02c:return "data/background/BG02c.dat";
case BackgroundKind::BG02x:return "data/background/BG02x.dat";
case BackgroundKind::BG02y:return "data/background/BG02y.dat";
case BackgroundKind::BG02z:return "data/background/BG02z.dat";
case BackgroundKind::BG03a:return "data/background/BG03a.dat";
case BackgroundKind::BG03b:return "data/background/BG03b.dat";
case BackgroundKind::BG03x:return "data/background/BG03x.dat";
case BackgroundKind::BG03y:return "data/background/BG03y.dat";
case BackgroundKind::BG04a:return "data/background/BG04a.dat";
case BackgroundKind::BG04b:return "data/background/BG04b.dat";
case BackgroundKind::BG04x:return "data/background/BG04x.dat";
case BackgroundKind::BG04y:return "data/background/BG04y.dat";
case BackgroundKind::BG05a:return "data/background/BG05a.dat";
case BackgroundKind::BG05b:return "data/background/BG05b.dat";
case BackgroundKind::BG05x:return "data/background/BG05x.dat";
case BackgroundKind::BG05y:return "data/background/BG05y.dat";
case BackgroundKind::BG06a:return "data/background/BG06a.dat";
case BackgroundKind::BG06x:return "data/background/BG06x.dat";
case BackgroundKind::BG07a:return "data/background/BG07a.dat";
case BackgroundKind::BG07x:return "data/background/BG07x.dat";
case BackgroundKind::BG08a:return "data/background/BG08a.dat";
case BackgroundKind::BG08x:return "data/background/BG08x.dat";
case BackgroundKind::BG09a:return "data/background/BG09a.dat";
case BackgroundKind::BG09x:return "data/background/BG09x.dat";}return "";}
ShadowStyle BackgroundStage::shadow_style()const{ShadowStyle s;s.projection=shadowOrigin;switch(kind){
case BackgroundKind::BG00a:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG00b:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG00c:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG00x:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG00y:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG00z:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG01a:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG01b:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG01x:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG01y:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG02a:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG02b:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG02c:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG02x:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG02y:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG02z:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG03a:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG03b:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG03x:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG03y:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG04a:s.scale={1.0f,0.30000001192092896f,0.0f};s.color=0x80000000;break;
case BackgroundKind::BG04b:s.scale={1.0f,0.30000001192092896f,0.0f};s.color=0x80000000;break;
case BackgroundKind::BG04x:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG04y:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG05a:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG05b:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG05x:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG05y:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG06a:s.scale={1.0f,0.30000001192092896f,0.0f};s.color=0x40ffffff;break;
case BackgroundKind::BG06x:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG07a:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG07x:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG08a:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG08x:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
case BackgroundKind::BG09a:s.scale={1.0f,0.30000001192092896f,0.0f};s.color=0x40ffffff;break;
case BackgroundKind::BG09x:s.scale={1.0f,0.30000001192092896f,1.0f};s.color=0x80000000;break;
}return s;}
}
