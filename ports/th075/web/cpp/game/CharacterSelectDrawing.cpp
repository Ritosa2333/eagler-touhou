#include "CharacterSelectScene.hpp"
#include <cmath>
namespace th075 {
void CharacterSelectScene::render_spells(ImageRenderer& images,ImageRenderer& characters,ImageRenderer& backgrounds,const GameSelection& config,const SelectionSettings& settings,const SelectionProgress& progress)const{
QuadPoints quad0{};
CollisionRectangle bounds0{};

  float fVar1;
  u32 uVar2;
  int iVar3;
  u32 local_cc;
  u32 local_c4;
  i16 local_bc;
  u32 local_b4;
  u8 local_ad;
  int local_ac;
  
  u8 abStack_98 [4];
  
  u8 abStack_8 [4];
  
  quad0={};
  local_ac = 0;
  do {
    if (((1 < local_ac) || ((u8(config.mode) == '\x03' && (local_ac == 1)))) ||
       ((u8(config.mode) == '\x04' && (local_ac == 1)))) {
      return;
    }
    abStack_98[local_ac] = i8(players[local_ac].panel) * -0x80 - 1;
    abStack_8[local_ac] = i8(players[local_ac].panel) * -0x40 + 0x80;
    fVar1 = double(players[local_ac].cardTurn);
    if (double(fVar1) < 15.0 == (double(fVar1) == 15.0)) {
      fVar1 = double(players[local_ac].cardTurn);
      if (double(fVar1) < 45.0 == (double(fVar1) == 45.0)) {
        uVar2 = u32(i32(double(players[local_ac].cardTurn)-45.0));
        local_ad = (u8)((uVar2 & 0xff) / 2);
      }
      else {
        local_ad = u32(i32((45.0-double(players[local_ac].cardTurn))/2.0));
      }
    }
    else {
      local_ad = u32(i32(double(players[local_ac].cardTurn)));
    }
    local_c4 = (u32)(45.0 < double(players[local_ac].cardTurn));
    iVar3 = local_c4 * 0x80;
    quad0=rectangle_quad(0.0f,0.0f,128.0f,160.0f);
    rotate_quad(quad0,64.0f,80.0f,0.0f,0.0f,90.0 - double(players[local_ac].cardTurn) * 6.0,0.0f)
    ;
    if ((double(players[local_ac].cardTurn) <= 30.0) ||
       (60.0 <= double(players[local_ac].cardTurn))) {
      bounds0={i32(iVar3 + 0x100),i32(0),i32(iVar3 + 0x180),i32(0xa0)};
    }
    else {
      bounds0={i32(0x100),i32(0x1e0),i32(0x180),i32(0x280)};
    }
    translate_quad(quad0,(double)(local_ac * 0x140 + 0x96),75.0f,0.0f);
    if ((i8(players[local_ac].spell) == '\0') ||
       (i8(players[local_ac].highlightAll) == '\0')) {
      local_b4 = (u32)abStack_98[local_ac] << 0x10 | 0xff000000 | (u32)abStack_98[local_ac] << 8 |
                 (u32)abStack_98[local_ac];
    }
    else {
      local_b4 = (u32)abStack_8[local_ac] << 0x10 | 0xff000000 | (u32)abStack_8[local_ac] << 8 |
                 (u32)abStack_8[local_ac];
    }
    characters.quad(i8(players[local_ac].character) * 2 + 1,quad0,bounds0,local_b4);
    if ((double(players[local_ac].cardTurn) <= 30.0) ||
       (60.0 <= double(players[local_ac].cardTurn))) {
      bounds0={i32(iVar3 + 0x100),i32(0xa0),i32(iVar3 + 0x180),i32(0x140)};
    }
    else {
      bounds0={i32(0x100),i32(0x1e0),i32(0x180),i32(0x280)};
    }
    translate_quad(quad0,-120.0f,120.0f,0.0f);
    if ((i8(players[local_ac].spell) == '\x01') ||
       (i8(players[local_ac].highlightAll) == '\0')) {
      local_b4 = (u32)abStack_98[local_ac] << 0x10 | 0xff000000 | (u32)abStack_98[local_ac] << 8 |
                 (u32)abStack_98[local_ac];
    }
    else {
      local_b4 = (u32)abStack_8[local_ac] << 0x10 | 0xff000000 | (u32)abStack_8[local_ac] << 8 |
                 (u32)abStack_8[local_ac];
    }
    characters.quad(i8(players[local_ac].character) * 2 + 1,quad0,bounds0,local_b4);
    if ((double(players[local_ac].cardTurn) <= 30.0) ||
       (60.0 <= double(players[local_ac].cardTurn))) {
      bounds0={i32(iVar3 + 0x100),i32(0x140),i32(iVar3 + 0x180),i32(0x1e0)};
    }
    else {
      bounds0={i32(0x100),i32(0x1e0),i32(0x180),i32(0x280)};
    }
    translate_quad(quad0,60.0f,120.0f,0.0f);
    if ((i8(players[local_ac].spell) == '\x02') ||
       (i8(players[local_ac].highlightAll) == '\0')) {
      local_b4 = (u32)abStack_98[local_ac] << 0x10 | 0xff000000 | (u32)abStack_98[local_ac] << 8 |
                 (u32)abStack_98[local_ac];
    }
    else {
      local_b4 = (u32)abStack_8[local_ac] << 0x10 | 0xff000000 | (u32)abStack_8[local_ac] << 8 |
                 (u32)abStack_8[local_ac];
    }
    characters.quad(i8(players[local_ac].character) * 2 + 1,quad0,bounds0,local_b4);
    local_cc = (u32)(45.0 < double(players[local_ac].cardTurn));
    iVar3 = local_cc * 0x60;
    quad0=rectangle_quad(0.0f,0.0f,256.0f,32.0f);
    bounds0={i32(0x100),i32(iVar3 + 0x280),i32(0x200),i32(iVar3 + 0x2a0)};
    translate_quad(quad0,(double)(local_ac * 0x140 + 100),150.0f,0.0f);
    translate_quad(quad0,0.0f,(double)(int)(local_ad - 0xf),0.0f);
    if ((i8(players[local_ac].spell) == '\0') ||
       (i8(players[local_ac].highlightAll) == '\0')) {
      local_b4 = (u32)local_ad * 0x11000000 | (u32)abStack_98[local_ac] << 0x10 |
                 (u32)abStack_98[local_ac] << 8 | (u32)abStack_98[local_ac];
    }
    else {
      local_b4 = (u32)local_ad * 0x11000000 | (u32)abStack_8[local_ac] << 0x10 |
                 (u32)abStack_8[local_ac] << 8 | (u32)abStack_8[local_ac];
    }
    characters.quad(i8(players[local_ac].character) * 2 + 1,quad0,bounds0,local_b4);
    bounds0={i32(0x100),i32(iVar3 + 0x2a0),i32(0x200),i32(iVar3 + 0x2c0)};
    translate_quad(quad0,-120.0f,120.0f,0.0f);
    translate_quad(quad0,(double)(int)-(local_ad - 0xf),(double)(int)-(local_ad - 0xf),0.0f);
    if ((i8(players[local_ac].spell) == '\x01') ||
       (i8(players[local_ac].highlightAll) == '\0')) {
      local_b4 = (u32)local_ad * 0x11000000 | (u32)abStack_98[local_ac] << 0x10 |
                 (u32)abStack_98[local_ac] << 8 | (u32)abStack_98[local_ac];
    }
    else {
      local_b4 = (u32)local_ad * 0x11000000 | (u32)abStack_8[local_ac] << 0x10 |
                 (u32)abStack_8[local_ac] << 8 | (u32)abStack_8[local_ac];
    }
    characters.quad(i8(players[local_ac].character) * 2 + 1,quad0,bounds0,local_b4);
    bounds0={i32(0x100),i32(iVar3 + 0x2c0),i32(0x200),i32(iVar3 + 0x2e0)};
    translate_quad(quad0,60.0f,120.0f,0.0f);
    translate_quad(quad0,(double)(int)(local_ad - 0xf),(double)(int)-(local_ad - 0xf),0.0f);
    if ((i8(players[local_ac].spell) == '\x02') ||
       (i8(players[local_ac].highlightAll) == '\0')) {
      local_b4 = (u32)local_ad * 0x11000000 | (u32)abStack_98[local_ac] << 0x10 |
                 (u32)abStack_98[local_ac] << 8 | (u32)abStack_98[local_ac];
    }
    else {
      local_b4 = (u32)local_ad * 0x11000000 | (u32)abStack_8[local_ac] << 0x10 |
                 (u32)abStack_8[local_ac] << 8 | (u32)abStack_8[local_ac];
    }
    characters.quad(i8(players[local_ac].character) * 2 + 1,quad0,bounds0,local_b4);
    if (i8(players[local_ac].help) != '\0') {
      if (i8(players[local_ac].phase) == '\x01') {
        local_bc = 0;
      }
      else {
        if (i8(players[local_ac].phase) != '\x02') goto LAB_004373a6;
        local_bc = 0xc0;
      }
      bounds0={i32(0),i32(local_bc + 0x280),i32(0x100),i32(local_bc + 0x2c0)};
      quad0=rectangle_quad(0.0f,0.0f,256.0f,64.0f);
      translate_quad(quad0,(double)(local_ac * 0x140 + 0x20),64.0f,0.0f);
      characters.quad(i8(players[local_ac].character) * 2 + 1,quad0,bounds0,0xffffffff);
      bounds0={i32(0),i32(local_bc + 0x2c0),i32(0x100),i32(local_bc + 0x300)};
      translate_quad(quad0,0.0f,128.0f,0.0f);
      characters.quad(i8(players[local_ac].character) * 2 + 1,quad0,bounds0,0xffffffff);
      bounds0={i32(0),i32(local_bc + 0x300),i32(0x100),i32(local_bc + 0x340)};
      translate_quad(quad0,0.0f,128.0f,0.0f);
      characters.quad(i8(players[local_ac].character) * 2 + 1,quad0,bounds0,0xffffffff);
    }
LAB_004373a6:
    local_ac = local_ac + 1;
  } while( true );

}
void CharacterSelectScene::render_options(ImageRenderer& images,ImageRenderer& characters,ImageRenderer& backgrounds,const GameSelection& config,const SelectionSettings& settings,const SelectionProgress& progress)const{
QuadPoints quad0{};
CollisionRectangle bounds0{};

  int local_a0;
  
  
  
  quad0={};
  for (local_a0 = 0; local_a0 < 2; local_a0 = local_a0 + 1) {
    if (i8(players[local_a0].panel) != '\0') {
      bounds0={i32(0),i32(0),i32(0x100),i32(0x70)};
      quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
      translate_quad(quad0,(double)(local_a0 * 0x140 + 0x20),120.0f,0.0f);
      images.quad(5,quad0,bounds0,0xffffffff);
      bounds0={i32(0xd0),i32((u32)(u8)settings.easyCommands[local_a0] * -0x20 + 0x90),i32(0x100),i32((u32)(u8)settings.easyCommands[local_a0] * -0x20 + 0xb0)};
      quad0=rectangle_quad(208.0f,0.0f,256.0f,32.0f);
      translate_quad(quad0,(double)(local_a0 * 0x140 + 0x20),120.0f,0.0f);
      images.quad(5,quad0,bounds0,0xffffffff);
      bounds0={i32(0x4d),i32(0x90),i32(0xca),i32(0xb0)};
      quad0=rectangle_quad(77.0f,56.0f,202.0f,88.0f);
      translate_quad(quad0,(double)(local_a0 * 0x140 + 0x20),120.0f,0.0f);
      images.quad(5,quad0,bounds0,0xffffffff);
      bounds0={i32(0x4d),i32(0x70),i32((i8)settings.handicap[local_a0] * 0x19 + 0x66),i32(0x90)};
      quad0=rectangle_quad(77.0f,56.0f,(double)((i8)settings.handicap[local_a0] * 0x19 + 0x66),88.0f);
      translate_quad(quad0,(double)(local_a0 * 0x140 + 0x20),120.0f,0.0f);
      images.quad(5,quad0,bounds0,0xffffffff);
      bounds0={i32(0),i32(0x70),i32(0x40),i32(0x90)};
      quad0=rectangle_quad(0.0f,0.0f,64.0f,32.0f);
      translate_quad(quad0,(double)(local_a0 * 0x140 + 0x20),(double)(i8(players[local_a0].panelCursor) * 0x20 + 0x78),0.0f);
      images.quad(5,quad0,bounds0,0xffffffff);
      if (u8(config.mode) != '\x04') {
        bounds0={i32(0x100),i32(0x340),i32(0x200),i32(0x400)};
        quad0=rectangle_quad(0.0f,0.0f,256.0f,192.0f);
        translate_quad(quad0,(double)(local_a0 * 0x140 + 0x20),280.0f,0.0f);
        characters.quad(i8(players[local_a0].character) * 2 + 1,quad0,bounds0,0xffffffff);
      }
    }
  }
  return;

}
void CharacterSelectScene::render_story_options(ImageRenderer& images,ImageRenderer& characters,ImageRenderer& backgrounds,const GameSelection& config,const SelectionSettings& settings,const SelectionProgress& progress)const{
QuadPoints quad0{};
CollisionRectangle bounds0{};

  i8 cVar1;
  int xRight;
  float uVar2;
  int yBottom;
  float uVar3;
  
  
  
  if (i8(players[0].panel) != '\0') {
    quad0={};
    bounds0={i32(0),i32(0),i32(0x90),i32(0xe0)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    translate_quad(quad0,408.0f,120.0f,0.0f);
    images.quad(6,quad0,bounds0,0xffffffff);
    cVar1 = progress.stages();
    bounds0={i32(0x90),i32(0),i32(0xe0),i32((int)cVar1 << 5)};
    cVar1 = progress.stages();
    quad0=rectangle_quad(0.0f,0.0f,80.0f,(double)((int)cVar1 << 5));
    translate_quad(quad0,472.0f,120.0f,0.0f);
    images.quad(6,quad0,bounds0,0xffffffff);
    yBottom = 0xe0;
    xRight = 0xe0;
    cVar1 = progress.stages();
    bounds0={i32(0x90),i32((int)cVar1 << 5),i32(xRight),i32(yBottom)};
    uVar3 = 224.0f;
    uVar2 = 80.0f;
    cVar1 = progress.stages();
    quad0=rectangle_quad(0.0f,(double)((int)cVar1 << 5),uVar2,uVar3);
    translate_quad(quad0,472.0f,120.0f,0.0f);
    images.quad(6,quad0,bounds0,0xff808080);
    bounds0={i32(0),i32(0xe0),i32(0x40),i32(0x100)};
    quad0=rectangle_quad(0.0f,0.0f,64.0f,32.0f);
    translate_quad(quad0,408.0f,(double)(i8(players[0].panelCursor) * 0x20 + 0x78),0.0f);
    images.quad(6,quad0,bounds0,0xffffffff);
  }
  return;

}
void CharacterSelectScene::render(ImageRenderer& images,ImageRenderer& characters,ImageRenderer& backgrounds,const GameSelection& config,const SelectionSettings& settings,const SelectionProgress& progress)const{
QuadPoints quad0{};
CollisionRectangle bounds0{};

  i8 cVar1;
  u32 uVar2;
  int iVar3;
  int xLeft;
  double fVar4;
  int yTop;
  float uVar5;
  float fVar6;
  int yBottom;
  
  u8 local_94;
  u8 local_93;
  i8 local_8d;
  
  
  cVar1 = true;
  if (cVar1 != '\0') {
    quad0={};
    local_94 = i8(players[0].panel) * -0x80 - 1;
    local_93 = i8(players[1].panel) * -0x80 - 1;
    images.graphics.depth(DepthMode(0));
    images.blend(SpriteBlend(0));
    bounds0={i32(0),i32(0),i32(0x280),i32(0x1e0)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    images.quad(0,quad0,bounds0,0xffffffff);
    bounds0={i32(0),i32(0),i32(0x200),i32(0x200)};
    quad0=rectangle_quad(-96.0 - double(players[0].portraitSlide),0.0f,416.0 - double(players[0].portraitSlide),512.0f)
    ;
    uVar2 = u32(i32(((double(players[0].portraitOffset)*255.0)/100.0)));
    characters.quad((int)i8(players[0].character) << 1,quad0,bounds0,(0xff - (uVar2 & 0xff)) * 0x1000000 | (u32)local_94 << 0x10 | (u32)local_94 << 8
                 | (u32)local_94);
    if ((i8(players[0].cardsHidden) == '\0') && (i8(players[0].phase) == '\0')) {
      bounds0={i32(0),i32(0),i32(0x200),i32(0x200)};
      quad0=rectangle_quad(-96.0 - (100.0 - double(players[0].portraitSlide)),0.0f,416.0 - (100.0 - double(players[0].portraitSlide)),512.0f);
      iVar3 = u32(i32(((double(players[0].portraitOffset)*255.0)/100.0)));
      characters.quad((int)i8(players[0].previousCharacter) << 1,quad0,bounds0,iVar3 << 0x18 | (u32)local_94 << 0x10 | (u32)local_94 << 8 | (u32)local_94);
    }
    if (u8(config.mode) != '\x03') {
      bounds0={i32(0),i32(0),i32(0x200),i32(0x200)};
      quad0=rectangle_quad(-96.0 - double(players[1].portraitSlide),0.0f,416.0 - double(players[1].portraitSlide),512.0f);
      rotate_quad(quad0,320.0f,0.0f,0.0f,0.0f,180.0f,0.0f);
      uVar2 = u32(i32(((double(players[1].portraitOffset)*255.0)/100.0)));
      characters.quad((int)i8(players[1].character) << 1,quad0,bounds0,(0xff - (uVar2 & 0xff)) * 0x1000000 | (u32)local_93 << 0x10 |
                   (u32)local_93 << 8 | (u32)local_93);
      bounds0={i32(0),i32(0xc0),i32(0x100),i32(0x1c0)};
      quad0=rectangle_quad(double(players[1].firstCardX) + 20.0,244.0f,double(players[1].firstCardX) + 276.0,500.0f);
      scale_quad(quad0,160.0f,372.0f,0.0f,-double(players[1].firstCardX) / 80.0 + 1.0,-double(players[1].firstCardX) / 80.0 + 1.0,-double(players[1].firstCardX) / 80.0 + 1.0);
      rotate_quad(quad0,160.0f,372.0f,0.0f,0.0f,0.0f,-double(players[1].firstCardX) + -double(players[1].firstCardX));
      translate_quad(quad0,320.0f,0.0f,0.0f);
      uVar2 = u32(i32(((double(players[1].firstCardX)*255.0)/80.0)));
      characters.quad(i8(players[1].character) * 2 + 1,quad0,bounds0,(0xff - (uVar2 & 0xff)) * 0x1000000 | (u32)local_93 << 0x10 |
                   (u32)local_93 << 8 | (u32)local_93);
      bounds0={i32(0),i32(0x80),i32(0x100),i32(0xc0)};
      quad0=rectangle_quad(20.0f,303.0 - double(players[1].firstCardY),276.0f,367.0 - double(players[1].firstCardY));
      translate_quad(quad0,320.0f,0.0f,0.0f);
      uVar2 = u32(i32(((double(players[1].firstCardY)*255.0)/40.0)));
      characters.quad(i8(players[1].character) * 2 + 1,quad0,bounds0,(0xff - (uVar2 & 0xff)) * 0x1000000 | (u32)local_93 << 0x10 |
                   (u32)local_93 << 8 | (u32)local_93);
      bounds0={i32(0),i32(0),i32(0x100),i32(0x40)};
      quad0=rectangle_quad(20.0 - double(players[1].secondCardX),333.0f,276.0 - double(players[1].secondCardX),397.0f);
      translate_quad(quad0,320.0f,0.0f,0.0f);
      uVar2 = u32(i32(((double(players[1].secondCardX)*255.0)/80.0)));
      characters.quad(i8(players[1].character) * 2 + 1,quad0,bounds0,(0xff - (uVar2 & 0xff)) * 0x1000000 | (u32)local_93 << 0x10 |
                   (u32)local_93 << 8 | (u32)local_93);
      bounds0={i32(0),i32(0x40),i32(0x100),i32(0x80)};
      quad0=rectangle_quad(20.0f,double(players[1].secondCardY) + 358.0,276.0f,double(players[1].secondCardY) + 422.0);
      translate_quad(quad0,320.0f,0.0f,0.0f);
      uVar2 = u32(i32(((double(players[1].secondCardY)*255.0)/40.0)));
      characters.quad(i8(players[1].character) * 2 + 1,quad0,bounds0,(0xff - (uVar2 & 0xff)) * 0x1000000 | (u32)local_93 << 0x10 |
                   (u32)local_93 << 8 | (u32)local_93);
      if ((i8(players[1].cardsHidden) == '\0') && (i8(players[1].phase) == '\0')) {
        bounds0={i32(0),i32(0),i32(0x200),i32(0x200)};
        quad0=rectangle_quad(-96.0 - (100.0 - double(players[1].portraitSlide)),0.0f,416.0 - (100.0 - double(players[1].portraitSlide)),512.0f);
        rotate_quad(quad0,320.0f,0.0f,0.0f,0.0f,180.0f,0.0f);
        iVar3 = u32(i32(((double(players[1].portraitOffset)*255.0)/100.0)));
        characters.quad((int)i8(players[1].previousCharacter) << 1,quad0,bounds0,iVar3 << 0x18 | (u32)local_93 << 0x10 | (u32)local_93 << 8 | (u32)local_93);
        bounds0={i32(0),i32(0x80),i32(0x100),i32(0xc0)};
        quad0=rectangle_quad(20.0f,303.0 - (40.0 - double(players[1].firstCardY)),276.0f,367.0 - (40.0 - double(players[1].secondCardX)));
        translate_quad(quad0,320.0f,0.0f,0.0f);
        iVar3 = u32(i32(((double(players[1].firstCardY)*255.0)/40.0)));
        characters.quad(i8(players[1].previousCharacter) * 2 + 1,quad0,bounds0,iVar3 << 0x18 | (u32)local_93 << 0x10 | (u32)local_93 << 8 | (u32)local_93);
        bounds0={i32(0),i32(0),i32(0x100),i32(0x40)};
        quad0=rectangle_quad((80.0 - double(players[1].secondCardX)) + 20.0,333.0f,(80.0 - double(players[1].secondCardX)) + 276.0,397.0f);
        translate_quad(quad0,320.0f,0.0f,0.0f);
        iVar3 = u32(i32(((double(players[1].secondCardX)*255.0)/80.0)));
        characters.quad(i8(players[1].previousCharacter) * 2 + 1,quad0,bounds0,iVar3 << 0x18 | (u32)local_93 << 0x10 | (u32)local_93 << 8 | (u32)local_93);
        bounds0={i32(0),i32(0x40),i32(0x100),i32(0x80)};
        quad0=rectangle_quad(20.0f,(40.0 - double(players[1].secondCardY)) + 358.0,276.0f,(40.0 - double(players[1].secondCardY)) + 422.0);
        translate_quad(quad0,320.0f,0.0f,0.0f);
        iVar3 = u32(i32(((double(players[1].secondCardY)*255.0)/40.0)));
        characters.quad(i8(players[1].previousCharacter) * 2 + 1,quad0,bounds0,iVar3 << 0x18 | (u32)local_93 << 0x10 | (u32)local_93 << 8 | (u32)local_93);
      }
    }
    bounds0={i32(0),i32(0xc0),i32(0x100),i32(0x1c0)};
    quad0=rectangle_quad(20.0f,244.0f,276.0f,500.0f);
    scale_quad(quad0,160.0f,372.0f,0.0f,-double(players[0].firstCardX) / 80.0 + 1.0,-double(players[0].firstCardX) / 80.0 + 1.0,-double(players[0].firstCardX) / 80.0 + 1.0);
    rotate_quad(quad0,160.0f,372.0f,0.0f,0.0f,0.0f,-double(players[0].firstCardX) + -double(players[0].firstCardX));
    uVar2 = u32(i32(((double(players[0].firstCardX)*255.0)/80.0)));
    characters.quad(i8(players[0].character) * 2 + 1,quad0,bounds0,(0xff - (uVar2 & 0xff)) * 0x1000000 | (u32)local_94 << 0x10 | (u32)local_94 << 8
                 | (u32)local_94);
    bounds0={i32(0),i32(0x80),i32(0x100),i32(0xc0)};
    quad0=rectangle_quad(20.0f,303.0 - double(players[0].firstCardY),276.0f,367.0 - double(players[0].firstCardY));
    uVar2 = u32(i32(((double(players[0].firstCardY)*255.0)/40.0)));
    characters.quad(i8(players[0].character) * 2 + 1,quad0,bounds0,(0xff - (uVar2 & 0xff)) * 0x1000000 | (u32)local_94 << 0x10 | (u32)local_94 << 8
                 | (u32)local_94);
    bounds0={i32(0),i32(0),i32(0x100),i32(0x40)};
    quad0=rectangle_quad(double(players[0].secondCardX) + 20.0,333.0f,double(players[0].secondCardX) + 276.0,397.0f);
    uVar2 = u32(i32(((double(players[0].secondCardX)*255.0)/80.0)));
    characters.quad(i8(players[0].character) * 2 + 1,quad0,bounds0,(0xff - (uVar2 & 0xff)) * 0x1000000 | (u32)local_94 << 0x10 | (u32)local_94 << 8
                 | (u32)local_94);
    bounds0={i32(0),i32(0x40),i32(0x100),i32(0x80)};
    quad0=rectangle_quad(20.0f,double(players[0].secondCardY) + 358.0,276.0f,double(players[0].secondCardY) + 422.0);
    uVar2 = u32(i32(((double(players[0].secondCardY)*255.0)/40.0)));
    characters.quad(i8(players[0].character) * 2 + 1,quad0,bounds0,(0xff - (uVar2 & 0xff)) * 0x1000000 | (u32)local_94 << 0x10 | (u32)local_94 << 8
                 | (u32)local_94);
    if ((i8(players[0].cardsHidden) == '\0') && (i8(players[0].phase) == '\0')) {
      bounds0={i32(0),i32(0x80),i32(0x100),i32(0xc0)};
      quad0=rectangle_quad(20.0f,303.0 - (40.0 - double(players[0].firstCardY)),276.0f,367.0 - (40.0 - double(players[0].secondCardX)));
      iVar3 = u32(i32(((double(players[0].firstCardY)*255.0)/40.0)));
      characters.quad(i8(players[0].previousCharacter) * 2 + 1,quad0,bounds0,iVar3 << 0x18 | (u32)local_94 << 0x10 | (u32)local_94 << 8 | (u32)local_94);
      bounds0={i32(0),i32(0),i32(0x100),i32(0x40)};
      quad0=rectangle_quad(20.0 - (80.0 - double(players[0].secondCardX)),333.0f,276.0 - (80.0 - double(players[0].secondCardX)),397.0f);
      iVar3 = u32(i32(((double(players[0].secondCardX)*255.0)/80.0)));
      characters.quad(i8(players[0].previousCharacter) * 2 + 1,quad0,bounds0,iVar3 << 0x18 | (u32)local_94 << 0x10 | (u32)local_94 << 8 | (u32)local_94);
      bounds0={i32(0),i32(0x40),i32(0x100),i32(0x80)};
      quad0=rectangle_quad(20.0f,(40.0 - double(players[0].secondCardY)) + 358.0,276.0f,(40.0 - double(players[0].secondCardY)) + 422.0);
      iVar3 = u32(i32(((double(players[0].secondCardY)*255.0)/40.0)));
      characters.quad(i8(players[0].previousCharacter) * 2 + 1,quad0,bounds0,iVar3 << 0x18 | (u32)local_94 << 0x10 | (u32)local_94 << 8 | (u32)local_94);
    }
    render_spells(images,characters,backgrounds,config,settings,progress);
    if ((i8(backgroundCursor) == '\b') && (i8(backgroundVariant) == '\x01')) {
      local_8d = '\x10';
    }
    else {
      local_8d = i8(backgroundCursor) +
                 i8(backgroundVariant) * '\n' * (i8(backgroundCursor) < '\x06');
    }
    bounds0={i32(0),i32(0),i32(0x100),i32(0x1e0)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    translate_quad(quad0,192.0f,0.0f,0.0f);
    backgrounds.quad((int)local_8d,quad0,bounds0,(u32)u8(stageAlpha) << 0x18 | 0xffffff)
    ;
    bounds0={i32(0),i32(local_8d * 0x39),i32(0x200),i32(local_8d * 0x39 + 0x39)};
    quad0=rectangle_quad(64.0f,300.0f,576.0f,357.0f);
    images.quad(2,quad0,bounds0,(u32)u8(stageAlpha) << 0x18 | 0xffffff);
    bounds0={i32(0),i32(0),i32(0x200),i32(0x40)};
    fVar4 = (double)std::sin(double(rotation));
    fVar6 = (double)(fVar4 * (double)double(entryZoom) + (double)74.0);
    uVar5 = 576.0f;
    fVar4 = (double)std::sin(double(rotation));
    quad0=rectangle_quad(64.0f,(double)(fVar4 * (double)double(entryZoom) + (double)10.0),uVar5,double(fVar6));
    images.quad(3,quad0,bounds0,(u32)u8(stageAlpha) << 0x18 | 0xffffff);
    images.quad(1,quad0,bounds0,(0xff - (u32)u8(stageAlpha)) * 0x1000000 | 0xffffff);
    yBottom = 0x10;
    iVar3 = u32(i32(640.0+double(backgroundScroll)));
    yTop = 0;
    xLeft = u32(i32(double(backgroundScroll)));
    bounds0={i32(xLeft),i32(yTop),i32(iVar3),i32(yBottom)};
    quad0=rectangle_quad(0.0f,448.0f,640.0f,464.0f);
    if (u8(config.mode) == '\x03') {
      images.quad(7,quad0,bounds0,0xffffffff);
    }
    else {
      images.quad(4,quad0,bounds0,0xffffffff);
    }
    if (((u8(config.mode) == '\x03') && (i8(players[0].panel) == '\0')) ||
       (u8(config.mode) == '\x04')) {
      bounds0={i32(0x100),i32(0x340),i32(0x200),i32(0x400)};
      quad0=rectangle_quad(0.0f,0.0f,256.0f,192.0f);
      translate_quad(quad0,352.0f,128.0f,0.0f);
      uVar2 = u32(i32(((double(players[0].portraitOffset)*255.0)/100.0)));
      characters.quad(i8(players[0].character) * 2 + 1,quad0,bounds0,(0xff - (uVar2 & 0xff)) * 0x1000000 | 0xffffff);
    }
    if (config.resumeStory != '\0') {
      bounds0={i32(0x90),i32(config.startingStage * 0x20 + -0x20),i32(0xe0),i32((int)config.startingStage << 5)};
      quad0=rectangle_quad(0.0f,0.0f,80.0f,32.0f);
      translate_quad(quad0,530.0f,400.0f,0.0f);
      images.quad(6,quad0,bounds0,0xffffffff);
    }
    if (u8(config.mode) == '\x03') {
      render_story_options(images,characters,backgrounds,config,settings,progress);
    }
    else {
      render_options(images,characters,backgrounds,config,settings,progress);
    }
    static_cast<void>(0);
  }
  return;

}
}
