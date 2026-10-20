#include "CinemaScene.hpp"
#include <cmath>
namespace th075 {
void CinemaScene::render_opening(ImageRenderer& images)const{
QuadPoints quad0{};

  float fVar1;
  i8 cVar2;
  u8 bVar3;
  double fVar4;
  float fVar5;
  float fVar6;
  u32 uVar7;
  u32 uVar8;
  int local_ac;
  float local_a0;
  float local_8;
  
  cVar2 = true;
  if (cVar2 != '\0') {
    quad0={};
    local_8 = double(0.0f);
    local_a0 = double(0.0f);
    images.graphics.depth(DepthMode(0));
    images.blend(SpriteBlend(0));
    if (elapsed < 0xc0f8) {
      images.image(0,320.0f,240.0f,0xffffffff,320.0f,240.0f,10.0f,0.0f,0);
    }
    images.graphics.filter(Filter::Linear);
    if (elapsed < 0x5398) {
      fVar1 = ((double)elapsed * double(896.0f)) / double(21400.0f);
      images.image(0xf,double(fVar1) / double(8.0f) + double(384.0f),140.0f,0xffffffff,512.0f,256.0f,1.0f,0.0f,0);
      images.image(0x10,double(fVar1) / double(8.0f) + double(896.0f),140.0f,0xffffffff,512.0f,256.0f,1.0f,0.0f,0);
    }
    if (elapsed < 0x5398) {
      fVar1 = ((double)elapsed * double(760.0f)) / double(21400.0f);
      images.image(0x12,double(fVar1) / double(2.0f) + double(52.0f),140.0f,0xffffffff,512.0f,256.0f,1.0f,0.0f,0);
      images.image(0x13,double(fVar1) / double(2.0f) + double(564.0f),140.0f,0xffffffff,512.0f,256.0f,1.0f,0.0f,0);
      images.image(0x14,double(fVar1) / double(2.0f) + double(1076.0f),140.0f,0xffffffff,512.0f,256.0f,1.0f,0.0f,0);
    }
    if (elapsed < 0x5398) {
      fVar1 = ((double)elapsed * double(896.0f)) / double(21400.0f);
      images.image(0x11,double(fVar1) + -double(1664.0f),140.0f,0xffffffff,512.0f,-128.0f,1.0f,0.0f,0);
      images.image(0x11,double(fVar1) + -double(1152.0f),140.0f,0xffffffff,512.0f,-128.0f,1.0f,0.0f,0);
      images.image(0x11,double(fVar1) + -double(640.0f),140.0f,0xffffffff,512.0f,-128.0f,1.0f,0.0f,0);
      images.image(0x11,double(fVar1) + -double(128.0f),140.0f,0xffffffff,512.0f,-128.0f,1.0f,0.0f,0);
      images.image(0x11,double(fVar1) + double(384.0f),140.0f,0xffffffff,512.0f,-128.0f,1.0f,0.0f,0);
      images.image(0x11,double(fVar1) + double(896.0f),140.0f,0xffffffff,512.0f,-128.0f,1.0f,0.0f,0);
    }
    if (elapsed < 0x5398) {
      fVar1 = ((double)elapsed * double(1408.0f)) / double(21400.0f);
      images.image(0x11,double(fVar1) + -double(1920.0f),155.0f,0xcdffffff,563.2000122070312f,-128.0f,1.100000023841858f,0.0f,0);
      images.image(0x11,double(fVar1) + -double(1356.8f),155.0f,0xcdffffff,563.2000122070312f,-128.0f,1.100000023841858f,0.0f,0);
      images.image(0x11,double(fVar1) + -double(793.60004f),155.0f,0xcdffffff,563.2000122070312f,-128.0f,1.100000023841858f,0.0f,0);
      images.image(0x11,double(fVar1) + -double(230.40002f),155.0f,0xcdffffff,563.2000122070312f,-128.0f,1.100000023841858f,0.0f,0);
      images.image(0x11,double(fVar1) + double(332.8f),155.0f,0xcdffffff,563.2000122070312f,-128.0f,1.100000023841858f,0.0f,0);
      images.image(0x11,double(fVar1) + double(896.0f),155.0f,0xcdffffff,563.2000122070312f,-128.0f,1.100000023841858f,0.0f,0);
    }
    if (elapsed < 0x5398) {
      fVar1 = ((double)elapsed * double(1920.0f)) / double(21400.0f);
      images.image(0x11,double(fVar1) + -double(2176.0f),170.0f,0xcdffffff,614.4000244140625f,-128.0f,1.2000000476837158f,0.0f,0);
      images.image(0x11,double(fVar1) + -double(1561.6001f),170.0f,0xcdffffff,614.4000244140625f,-128.0f,1.2000000476837158f,0.0f,0);
      images.image(0x11,double(fVar1) + -double(947.2001f),170.0f,0xcdffffff,614.4000244140625f,-128.0f,1.2000000476837158f,0.0f,0);
      images.image(0x11,double(fVar1) + -double(332.80005f),170.0f,0xcdffffff,614.4000244140625f,-128.0f,1.2000000476837158f,0.0f,0);
      images.image(0x11,double(fVar1) + double(281.59998f),170.0f,0xcdffffff,614.4000244140625f,-128.0f,1.2000000476837158f,0.0f,0);
      images.image(0x11,double(fVar1) + double(896.0f),170.0f,0xcdffffff,614.4000244140625f,-128.0f,1.2000000476837158f,0.0f,0);
    }
    if ((0x5397 < elapsed) && (elapsed < 0xa1b8)) {
      if (elapsed < 27000) {
        local_a0 = (double)((double)(27000 - elapsed) / (double)0x3840);
      }
      if (33999 < elapsed) {
        local_8 = (double)((double)(elapsed - 34000) / (double)0x1ce8);
      }
      fVar1 = ((double)(elapsed - 0x5398) * double(736.0f)) / double(20000.0f);
      images.image(0x15,320.0f,double(480.0f) - double(fVar1),0xffffffff,(double(local_a0) * double(local_a0) * double(4.0f) + double(1.0f)) * double(320.0f),(double(local_a0) * double(local_a0) + double(local_a0) * double(local_a0) + double(1.0f)) * double(384.0f),double(local_a0) * double(local_a0) * double(4.0f) + double(1.0f),0.0f,0);
      images.image(0x16,320.0f,double(992.0f) - double(fVar1),0xffffffff,((double(local_8) * double(local_8)) / double(3.0f) + double(1.0f)) * double(320.0f),((double(local_8) * double(local_8)) / double(8.0f) + double(1.0f)) * double(384.0f),(double(local_8) * double(local_8)) / double(1.5f) + double(1.0f),(double(local_8) * double(20.0f) * double(local_8)) / double(2.0f),0);
    }
    if ((0xa1b7 < elapsed) && (elapsed < 0xbc48)) {
      fVar1 = (double)((double)(elapsed - 0xa1b8) / (double)0x1a90);
      uVar8 = 0;
      uVar7 = 0;
      fVar4 = (double)std::sqrt(double(double(fVar1)));
      fVar6 = (double)((double)double(2.0f) - fVar4);
      fVar4 = (double)std::sqrt(double(double(fVar1)));
      fVar5 = (double)(((double)double(2.0f) - fVar4) * (double)double(480.0f));
      fVar4 = (double)std::sqrt(double(double(fVar1)));
      images.image(0xe,320.0f,480.0f,0xffffffff,(double)(((double)double(2.0f) - fVar4) * (double)double(320.0f)),double(fVar5),double(fVar6),uVar7,uVar8);
    }
    if ((0xbc47 < elapsed) && (elapsed < 0xd098)) {
      images.image(0xe,320.0f,240.0f,0xffffffff,320.0f,240.0f,1.0f,0.0f,0);
    }
    if (elapsed < 0x578) {
      bVar3 = i32(double(1400u-elapsed)*double(255.0f)/double(1400.0f));
      images.image(0,320.0f,240.0f,(u32)bVar3 << 0x18 | 0xffffff,320.0f,240.0f,10.0f,0.0f,0);
    }
    if ((0x4c8f < elapsed) && (elapsed < 0x5398)) {
      bVar3 = i32(double(elapsed-19600u)*double(255.0f)/double(1800.0f));
      images.image(0,320.0f,240.0f,(u32)bVar3 << 0x18 | 0xffffff,320.0f,240.0f,10.0f,0.0f,0);
    }
    if ((0x5397 < elapsed) && (elapsed < 0x5aa0)) {
      bVar3 = i32(double(23200u-elapsed)*double(255.0f)/double(1800.0f));
      images.image(0,320.0f,240.0f,(u32)bVar3 << 0x18 | 0xffffff,320.0f,240.0f,10.0f,0.0f,0);
    }
    if ((0x9aaf < elapsed) && (elapsed < 0xa1b8)) {
      bVar3 = i32(double(elapsed-39600u)*double(255.0f)/double(1800.0f));
      images.image(0,320.0f,240.0f,(u32)bVar3 << 0x18 | 0xffffff,320.0f,240.0f,10.0f,0.0f,0);
    }
    if ((0xa1b7 < elapsed) && (elapsed < 0xa8c0)) {
      bVar3 = i32(double(43200u-elapsed)*double(255.0f)/double(1800.0f));
      images.image(0,320.0f,240.0f,(u32)bVar3 << 0x18 | 0xffffff,320.0f,240.0f,10.0f,0.0f,0);
    }
    if (elapsed < 0xc0f8) {
      images.image(0xd,320.0f,-32.0f,0xffffffff,512.0f,0.0f,32.0f,0.0f,0);
      images.image(0xd,320.0f,512.0f,0xffffffff,512.0f,128.0f,32.0f,0.0f,0);
    }
    for (local_ac = 0; local_ac < 0xd; local_ac = local_ac + 1) {
      if ((local_ac * 4000 + 0x578U <= elapsed) &&
         (elapsed < local_ac * 4000 + 0x708U)) {
        bVar3 = i32(double(elapsed-u32(local_ac*4000+1400))*double(255.0f)/double(400.0f));
        images.image(local_ac + 1,320.0f,464.0f,(u32)bVar3 << 0x10 | 0xff000000 | (u32)bVar3 << 8 | (u32)bVar3,256.0f,64.0f,1.0f,0.0f,0);
      }
      if ((local_ac * 4000 + 0x708U <= elapsed) &&
         (elapsed < local_ac * 4000 + 5000U)) {
        images.image(local_ac + 1,320.0f,464.0f,0xffffffff,256.0f,64.0f,1.0f,0.0f,0);
      }
      if ((local_ac * 4000 + 5000U <= elapsed) &&
         (elapsed < local_ac * 4000 + 0x1518U)) {
        bVar3 = i32(double(255.0f)-double(elapsed-u32(local_ac*4000+5000))/double(400.0f)*double(255.0f));
        images.image(local_ac + 1,320.0f,464.0f,(u32)bVar3 << 0x10 | 0xff000000 | (u32)bVar3 << 8 | (u32)bVar3,256.0f,64.0f,1.0f,0.0f,0);
      }
    }
    if ((47999 < elapsed) && (elapsed < 0xc0f8)) {
      bVar3 = i32(double(elapsed-48000u)*double(255.0f)/double(1400.0f));
      images.image(0x17,320.0f,240.0f,(u32)bVar3 << 0x18 | 0xffffff,320.0f,240.0f,1.0f,0.0f,0);
    }
    if ((0xc0f7 < elapsed) && (elapsed < 51000)) {
      images.image(0x17,320.0f,240.0f,0xffffffff,320.0f,240.0f,1.0f,0.0f,0);
    }
    static_cast<void>(0);
  }
  return;

}
void CinemaScene::render_credits(ImageRenderer& images)const{


  float fVar1;
  i8 cVar2;
  u8 bVar3;
  
  cVar2 = true;
  if (cVar2 != '\0') {
    images.graphics.depth(DepthMode(0));
    images.blend(SpriteBlend(0));
    if (elapsed < 0x1edc) {
      images.image(0,320.0f,240.0f,0xffffffff,320.0f,240.0f,10.0f,0.0f,0);
    }
    images.graphics.filter(Filter::Linear);
    if ((999 < elapsed) && (elapsed < 0x14b4)) {
      bVar3 = i32(double(elapsed-1000u)/double(4300.0f)*double(255.0f));
      fVar1 = (double)(0x14b4 - elapsed) / double(100.0f);
      images.image(6,double(260.0f) - double(fVar1),220.0f,(u32)bVar3 << 0x18 | 0xffffff,128.0f,32.0f,1.0f,0.0f,0);
      images.image(7,double(fVar1) + double(300.0f),350.0f,(u32)bVar3 << 0x18 | 0xffffff,256.0f,128.0f,1.0f,0.0f,0);
    }
    if ((0x14b3 < elapsed) && (elapsed < 0x1964)) {
      images.image(6,260.0f,220.0f,0xffffffff,128.0f,32.0f,1.0f,0.0f,0);
      images.image(7,300.0f,350.0f,0xffffffff,256.0f,128.0f,1.0f,0.0f,0);
    }
    if ((0x1963 < elapsed) && (elapsed < 0x1d4c)) {
      bVar3 = i32((double(1.0f)-double(elapsed-6500u)/double(1000.0f))*double(255.0f));
      images.image(6,260.0f,220.0f,(u32)bVar3 << 0x18 | 0xffffff,128.0f,32.0f,1.0f,0.0f,0);
      images.image(7,300.0f,350.0f,(u32)bVar3 << 0x18 | 0xffffff,256.0f,128.0f,1.0f,0.0f,0);
    }
    if ((0x1e77 < elapsed) && (elapsed < 61000)) {
      images.image(1,320.0f,240.0f,0xffffffff,320.0f,240.0f,10.0f,0.0f,0);
    }
    if ((0x1e77 < elapsed) && (elapsed < 0x2134)) {
      fVar1 = (double)((-(double)(elapsed - 0x1e78) / (double)700) * (double)double(64.0f)
                      * (double)double(16.0f) * (double)double(4.0f));
      images.graphics.filter(Filter::Nearest);
      images.image(2,320.0f,double(fVar1) + double(480.0f),0xffffffff,512.0f,0.0f,16.0f,0.0f,0);
      images.image(3,320.0f,double(fVar1) + double(480.0f) + double(1024.0f),0xffffffff,512.0f,0.0f,16.0f,0.0f,0);
      images.image(4,320.0f,double(fVar1) + double(480.0f) + double(2048.0f),0xffffffff,512.0f,0.0f,16.0f,0.0f,0);
      images.image(5,320.0f,double(fVar1) + double(480.0f) + double(3072.0f),0xffffffff,512.0f,0.0f,16.0f,0.0f,0);
      images.graphics.filter(Filter::Linear);
    }
    if ((0x1edb < elapsed) && (elapsed < 8000)) {
      fVar1 = (double)(((double)(8000 - elapsed) / (double)100) * (double)double(320.0f));
      images.image(8,double(320.0f) - double(fVar1),220.0f,0xffffffff,192.0f,12.0f,1.0f,0.0f,0);
      images.image(9,double(fVar1) + double(320.0f),260.0f,0xffffffff,32.0f,16.0f,1.0f,0.0f,0);
    }
    if ((7999 < elapsed) && (elapsed < 0x2db4)) {
      images.image(8,320.0f,220.0f,0xffffffff,192.0f,12.0f,1.0f,0.0f,0);
      images.image(9,320.0f,260.0f,0xffffffff,32.0f,16.0f,1.0f,0.0f,0);
    }
    if ((0x2db3 < elapsed) && (elapsed < 0x2e18)) {
      fVar1 = (double)((double)double(320.0f) -
                     ((double)(0x2e18 - elapsed) / (double)100) * (double)double(320.0f));
      images.image(8,double(fVar1) + double(320.0f),220.0f,0xffffffff,192.0f,12.0f,1.0f,0.0f,0);
      images.image(9,double(320.0f) - double(fVar1),260.0f,0xffffffff,32.0f,16.0f,1.0f,0.0f,0);
    }
    if ((0x2f43 < elapsed) && (elapsed < 0x2fa8)) {
      fVar1 = (double)(((double)(0x2fa8 - elapsed) / (double)100) * (double)double(320.0f))
      ;
      images.image(10,double(320.0f) - double(fVar1),220.0f,0xffffffff,128.0f,12.0f,1.0f,0.0f,0);
      images.image(0xb,double(fVar1) + double(320.0f),260.0f,0xffffffff,96.0f,16.0f,1.0f,0.0f,0);
    }
    if ((0x2fa7 < elapsed) && (elapsed < 0x3a34)) {
      images.image(10,320.0f,220.0f,0xffffffff,128.0f,12.0f,1.0f,0.0f,0);
      images.image(0xb,320.0f,260.0f,0xffffffff,96.0f,16.0f,1.0f,0.0f,0);
    }
    if ((0x3a33 < elapsed) && (elapsed < 15000)) {
      fVar1 = (double)((double)double(320.0f) -
                     ((double)(15000 - elapsed) / (double)100) * (double)double(320.0f));
      images.image(10,double(fVar1) + double(320.0f),220.0f,0xffffffff,128.0f,12.0f,1.0f,0.0f,0);
      images.image(0xb,double(320.0f) - double(fVar1),260.0f,0xffffffff,96.0f,16.0f,1.0f,0.0f,0);
    }
    if ((15999 < elapsed) && (elapsed < 63000)) {
      images.image(0xc,320.0f,double(480.0f) - (double)(elapsed - 16000) / double(20.0f),0xffffffff,256.0f,0.0f,1.0f,0.0f,0);
      images.image(0xd,320.0f,double(864.0f) - (double)(elapsed - 16000) / double(19.0f),0xffffffff,256.0f,0.0f,1.0f,0.0f,0);
      images.image(0xe,320.0f,double(1376.0f) - (double)(elapsed - 16000) / double(19.0f),0xffffffff,256.0f,0.0f,1.0f,0.0f,0);
      images.image(0xf,320.0f,double(1824.0f) - (double)(elapsed - 16000) / double(19.0f),0xffffffff,256.0f,0.0f,1.0f,0.0f,0);
      images.image(0x10,320.0f,double(2112.0f) - (double)(elapsed - 16000) / double(19.0f),0xffffffff,256.0f,0.0f,1.0f,0.0f,0);
    }
    if ((59999 < elapsed) && (elapsed < 63000)) {
      bVar3 = i32(double(elapsed-60000u)/double(3000.0f)*double(255.0f));
      images.image(0,320.0f,240.0f,(u32)bVar3 << 0x18 | 0xffffff,320.0f,240.0f,10.0f,0.0f,0);
    }
    if (62999 < elapsed) {
      images.image(0,320.0f,240.0f,0xffffffff,320.0f,240.0f,10.0f,0.0f,0);
    }
    if ((62999 < elapsed) && (elapsed < 65000)) {
      bVar3 = i32(double(elapsed-63000u)/double(2000.0f)*double(255.0f));
      images.image(0x11,320.0f,240.0f,(u32)bVar3 << 0x18 | 0xffffff,320.0f,240.0f,1.0f,0.0f,0);
    }
    if ((64999 < elapsed) && (elapsed < 69000)) {
      images.image(0x11,320.0f,240.0f,0xffffffff,320.0f,240.0f,1.0f,0.0f,0);
    }
    if ((68999 < elapsed) && (elapsed < 71000)) {
      bVar3 = i32((double(1.0f)-double(elapsed-69000u)/double(2000.0f))*double(255.0f));
      images.image(0x11,320.0f,240.0f,(u32)bVar3 << 0x18 | 0xffffff,320.0f,240.0f,1.0f,0.0f,0);
    }
    if ((0x116e7 < elapsed) && (elapsed < 0x1174c)) {
      fVar1 = (double)(((double)(0x1174c - elapsed) / (double)100) * (double)double(320.0f)
                     );
      images.image(0x12,double(320.0f) - double(fVar1),200.0f,0xffffffff,64.0f,16.0f,1.0f,0.0f,0);
      images.image(0x13,double(fVar1) + double(320.0f),240.0f,0xffffffff,160.0f,16.0f,1.0f,0.0f,0);
      images.image(0x14,double(320.0f) - double(fVar1),280.0f,0xffffffff,160.0f,16.0f,1.0f,0.0f,0);
    }
    if ((0x1174b < elapsed) && (elapsed < 0x1336c)) {
      images.image(0x12,320.0f,200.0f,0xffffffff,64.0f,16.0f,1.0f,0.0f,0);
      images.image(0x13,320.0f,240.0f,0xffffffff,160.0f,16.0f,1.0f,0.0f,0);
      images.image(0x14,320.0f,280.0f,0xffffffff,160.0f,16.0f,1.0f,0.0f,0);
    }
    if ((0x1336b < elapsed) && (elapsed < 0x133d0)) {
      fVar1 = (double)((double)double(320.0f) -
                     ((double)(0x133d0 - elapsed) / (double)100) * (double)double(320.0f))
      ;
      images.image(0x12,double(fVar1) + double(320.0f),200.0f,0xffffffff,64.0f,16.0f,1.0f,0.0f,0);
      images.image(0x13,double(320.0f) - double(fVar1),240.0f,0xffffffff,160.0f,16.0f,1.0f,0.0f,0);
      images.image(0x14,double(fVar1) + double(320.0f),280.0f,0xffffffff,160.0f,16.0f,1.0f,0.0f,0);
    }
    if ((0x13627 < elapsed) && (elapsed < 0x1368c)) {
      images.image(0x15,320.0f-float(double(0x1368c-elapsed)/double(100.0f)*double(320.0f)),200.0f,0xffffffff,128.0f,16.0f,1.0f,0.0f,0);
    }
    if (0x1368b < elapsed) {
      images.image(0x15,320.0f,200.0f,0xffffffff,128.0f,16.0f,1.0f,0.0f,0);
    }
    if ((0x1381b < elapsed) && (elapsed < 80000)) {
      bVar3 = i32(double(elapsed-79900u)*double(255.0f)/double(100.0f));
      images.image(0x16,320.0f,280.0f,(u32)bVar3 << 0x18 | 0xffffff,32.0f,16.0f,1.0f,0.0f,0);
    }
    if (79999 < elapsed) {
      images.image(0x16,320.0f,280.0f,0xffffffff,32.0f,16.0f,1.0f,0.0f,0);
    }
    images.graphics.filter(Filter::Nearest);
    static_cast<void>(0);
  }
  return;

}
}
