#include "BattleOverlay.hpp"
namespace th075 {
void BattleOverlay::advance(){

  if (0 < timer) {
    if (kind == '\0') {
      if (timer < 0x96) {
        if (timer < 0x8c) {
          if (timer < 0x4b) {
            if (timer < 0x46) {
              if (timer < 0x41) {
                if (timer < 0x3c) {
                  if (timer < 10) {
                    if (0 < timer) {
                      vertical = double(vertical) - 30.0;
                      horizontal = double(horizontal) - 30.0;
                      label = double(label) - 30.0;
                      secondaryLabel = double(secondaryLabel) + 0.5;
                      secondaryLabel = double(secondaryLabel) + 200.0;
                    }
                  }
                  else {
                    vertical = double(vertical) - 30.0;
                    horizontal = double(horizontal) - 30.0;
                    label = double(label) - 30.0;
                    secondaryLabel = double(secondaryLabel) + 0.5;
                    ring = double(ring) + double(0.15f);
                  }
                }
                else {
                  vertical = double(vertical) - double(0.3f);
                  horizontal = double(horizontal) - double(0.3f);
                  label = double(label) - 30.0;
                  secondaryLabel = double(secondaryLabel) + 0.5;
                  ring = double(ring) + double(0.15f);
                }
              }
              else {
                vertical = double(vertical) - double(0.3f);
                horizontal = double(horizontal) - double(0.3f);
                label = double(label) - 30.0;
                secondaryLabel = double(secondaryLabel) + 30.0;
                ring = double(ring) + double(0.15f);
              }
            }
            else {
              vertical = double(vertical) - double(0.3f);
              horizontal = double(horizontal) - double(0.3f);
              label = double(label) - double(0.3f);
              secondaryLabel = double(secondaryLabel) + 30.0;
              ring = double(ring) + double(0.2f);
            }
          }
          else {
            vertical = double(vertical) - double(0.3f);
            horizontal = double(horizontal) - double(0.3f);
            label = double(label) - double(0.3f);
            pulse = double(pulse) + double(pulseVelocity);
            pulseVelocity = double(pulseVelocity) + double(0.05f);
          }
        }
        else {
          vertical = double(vertical) - 40.0;
          horizontal = double(horizontal) - 70.0;
          label = double(label) - 30.0;
        }
      }
      else {
        vertical=350.0f;
        horizontal=430.0f;
        label=300.0f;
        secondaryLabel=-300.0f;
        pulse=0.0f;
        pulseVelocity=0.0f;
        ring=0.0f;
      }
    }
    else if (kind == '\x01') {
      if (timer < 0x96) {
        if (timer < 0x8c) {
          if (timer < 0x4b) {
            if (timer < 0x46) {
              if (timer < 0x41) {
                if (0 < timer) {
                  vertical = double(vertical) - 30.0;
                  horizontal = double(horizontal) - 30.0;
                  label = double(label) - 30.0;
                  ring = double(ring) + double(0.15f);
                }
              }
              else {
                vertical = double(vertical) - double(0.3f);
                horizontal = double(horizontal) - double(0.3f);
                label = double(label) - 30.0;
                secondaryLabel = double(secondaryLabel) + 30.0;
                ring = double(ring) + double(0.15f);
              }
            }
            else {
              vertical = double(vertical) - double(0.3f);
              horizontal = double(horizontal) - double(0.3f);
              label = double(label) - double(0.3f);
              secondaryLabel = double(secondaryLabel) + 30.0;
              ring = double(ring) + double(0.15f);
            }
          }
          else {
            vertical = double(vertical) - double(0.3f);
            horizontal = double(horizontal) - double(0.3f);
            label = double(label) - double(0.3f);
            ring = double(ring) + double(0.15f);
          }
        }
        else {
          vertical = double(vertical) - 40.0;
          horizontal = double(horizontal) - 70.0;
          label = double(label) - 30.0;
        }
      }
      else {
        vertical=350.0f;
        horizontal=430.0f;
        label=300.0f;
        secondaryLabel=-300.0f;
        pulse=0.0f;
        pulseVelocity=0.0f;
        ring=0.0f;
      }
    }
    timer = timer + -1;
  }
  return;

}
void BattleOverlay::render(ImageRenderer& images){
QuadPoints quad0{};
CollisionRectangle bounds0{};

  u32 uVar1;
  
  
  
  quad0={};
  images.graphics.depth(DepthMode(0));
  images.blend(SpriteBlend(0));
  if (i8(kind) == '\0') {
    bounds0={i32(frame * 5),i32(0),i32(frame * 5 + 0x280),i32(0x40)};
    quad0=rectangle_quad(0.0f,208.0 - double(vertical),640.0f,272.0 - double(vertical))
    ;
    images.quad(3,quad0,bounds0,0xffffffff);
    bounds0={i32(frame * -5),i32(0),i32(frame * -5 + 0x280),i32(0x40)};
    quad0=rectangle_quad(0.0f,double(vertical) + 208.0,640.0f,double(vertical) + 272.0)
    ;
    images.quad(3,quad0,bounds0,0xffffffff);
    bounds0={i32(frame * 5),i32(0),i32(frame * 5 + 0x1e0),i32(0x40)};
    quad0=rectangle_quad(80.0f,208.0 - double(horizontal),560.0f,272.0 - double(horizontal));
    rotate_quad(quad0,320.0f,240.0f,0.0f,0.0f,0.0f,90.0f);
    images.quad(3,quad0,bounds0,0xffffffff);
    bounds0={i32(frame * -5),i32(0),i32(frame * -5 + 0x1e0),i32(0x40)};
    quad0=rectangle_quad(80.0f,double(horizontal) + 208.0,560.0f,double(horizontal) + 272.0);
    rotate_quad(quad0,320.0f,240.0f,0.0f,0.0f,0.0f,90.0f);
    images.quad(3,quad0,bounds0,0xffffffff);
    if (-200.0 <= double(label)) {
      bounds0={i32(0),i32(0),i32(0x200),i32(0x40)};
      quad0=rectangle_quad(64.0f,208.0f,576.0f,272.0f);
      if (double(label) <= 0.0) {
        if (timer < 0x47) {
          scale_quad(quad0,320.0f,240.0f,0.0f,-double(label) / 200.0 + double(1.2f),double(label) / 200.0 + double(1.2f),1.0f);
        }
        else {
          scale_quad(quad0,320.0f,240.0f,0.0f,double(label) / 200.0 + double(1.2f),double(label) / 200.0 + double(1.2f),1.0f);
        }
      }
      else {
        rotate_quad(quad0,320.0f,240.0f,0.0f,0.0f,0.0f,label);
        scale_quad(quad0,320.0f,240.0f,0.0f,double(label) / 20.0 + double(1.2f),double(label) / 20.0 + double(1.2f),1.0f);
      }
      images.quad(4,quad0,bounds0,0xffffffff);
    }
    if ((timer < 0x8c) && (double(pulse) * 40.0 < 255.0)) {
      bounds0={i32(0),i32(0),i32(0x200),i32(0x40)};
      quad0=rectangle_quad(64.0f,208.0f,576.0f,272.0f);
      scale_quad(quad0,320.0f,240.0f,0.0f,double(pulse) + double(1.2f),double(pulse) + double(1.2f),1.0f);
      images.blend(SpriteBlend(2));
      uVar1 = u32(i32(double(pulse)*40.0));
      images.quad(6,quad0,bounds0,(0xff - (uVar1 & 0xff)) * 0x1000000 | 0xffffff);
      images.blend(SpriteBlend(0));
    }
    if ((timer < 0x4c) && (-300.0 < double(secondaryLabel))) {
      bounds0={i32(0),i32(0),i32(0x200),i32(0x80)};
      quad0=rectangle_quad(64.0f,176.0f,576.0f,304.0f);
      if (timer < 0xb) {
        rotate_quad(quad0,320.0f,240.0f,0.0f,0.0f,0.0f,double(secondaryLabel) / 5.0);
      }
      if (timer < 0x42) {
        scale_quad(quad0,320.0f,240.0f,0.0f,double(secondaryLabel) / 100.0 + 1.0,double(secondaryLabel) / 100.0 + 1.0,1.0f);
      }
      else {
        scale_quad(quad0,320.0f,240.0f,0.0f,-double(secondaryLabel) / 15.0 + 1.0,-double(secondaryLabel) / 15.0 + 1.0,1.0f);
      }
      images.quad(5,quad0,bounds0,0xffffffff);
    }
    if ((timer < 0x46) && (double(ring) * 60.0 < 255.0)) {
      bounds0={i32(0),i32(0),i32(0x100),i32(0x100)};
      quad0=rectangle_quad(192.0f,112.0f,448.0f,368.0f);
      scale_quad(quad0,320.0f,240.0f,0.0f,ring,ring,1.0f);
      images.blend(SpriteBlend(2));
      uVar1 = u32(i32(double(ring)*60.0));
      images.quad(2,quad0,bounds0,(0xff - (uVar1 & 0xff)) * 0x1000000 | 0xffffff);
      images.blend(SpriteBlend(0));
    }
  }
  else if (i8(kind) == '\x01') {
    bounds0={i32(frame * 5),i32(0),i32(frame * 5 + 0x280),i32(0x40)};
    quad0=rectangle_quad(0.0f,208.0 - double(vertical),640.0f,272.0 - double(vertical))
    ;
    images.quad(3,quad0,bounds0,0xffffffff);
    bounds0={i32(frame * -5),i32(0),i32(frame * -5 + 0x280),i32(0x40)};
    quad0=rectangle_quad(0.0f,double(vertical) + 208.0,640.0f,double(vertical) + 272.0)
    ;
    images.quad(3,quad0,bounds0,0xffffffff);
    bounds0={i32(frame * 5),i32(0),i32(frame * 5 + 0x1e0),i32(0x40)};
    quad0=rectangle_quad(80.0f,208.0 - double(horizontal),560.0f,272.0 - double(horizontal));
    rotate_quad(quad0,320.0f,240.0f,0.0f,0.0f,0.0f,90.0f);
    images.quad(3,quad0,bounds0,0xffffffff);
    bounds0={i32(frame * -5),i32(0),i32(frame * -5 + 0x1e0),i32(0x40)};
    quad0=rectangle_quad(80.0f,double(horizontal) + 208.0,560.0f,double(horizontal) + 272.0);
    rotate_quad(quad0,320.0f,240.0f,0.0f,0.0f,0.0f,90.0f);
    images.quad(3,quad0,bounds0,0xffffffff);
    if (-200.0 <= double(label)) {
      bounds0={i32(0),i32(0),i32(0x200),i32(0x80)};
      quad0=rectangle_quad(64.0f,176.0f,576.0f,304.0f);
      if (double(label) <= 0.0) {
        if (timer < 0x47) {
          scale_quad(quad0,320.0f,240.0f,0.0f,-double(label) / 200.0 + double(1.2f),double(label) / 200.0 + double(1.2f),1.0f);
        }
        else {
          scale_quad(quad0,320.0f,240.0f,0.0f,double(label) / 200.0 + double(1.2f),double(label) / 200.0 + double(1.2f),1.0f);
        }
      }
      else {
        rotate_quad(quad0,320.0f,240.0f,0.0f,0.0f,0.0f,label);
        scale_quad(quad0,320.0f,240.0f,0.0f,double(label) / 20.0 + double(1.2f),double(label) / 20.0 + double(1.2f),1.0f);
      }
      images.quad(7,quad0,bounds0,0xffffffff);
    }
    if ((timer < 0xaa) && (double(ring) * 60.0 < 255.0)) {
      bounds0={i32(0),i32(0),i32(0x100),i32(0x100)};
      quad0=rectangle_quad(192.0f,112.0f,448.0f,368.0f);
      scale_quad(quad0,320.0f,240.0f,0.0f,ring,ring,1.0f);
      images.blend(SpriteBlend(2));
      uVar1 = u32(i32(double(ring)*60.0));
      images.quad(2,quad0,bounds0,(0xff - (uVar1 & 0xff)) * 0x1000000 | 0xffffff);
      images.blend(SpriteBlend(0));
    }
  }
  return;

}
}
