#include "BattleHud.hpp"
namespace th075 {
namespace {const PatternFrame* hud_frame(const FighterActor& actor,i32 sequence,i32 frame){static const PatternFrame empty{};if(!actor.animation.pattern||sequence<0||u32(sequence)>=actor.animation.pattern->sequences.size())return &empty;const auto& s=actor.animation.pattern->sequences[sequence];return frame>=0&&u32(frame)<s.frames.size()?&s.frames[frame]:&empty;}}
void BattleHud::render_status(ImageRenderer& images,ImageRenderer& firstImages,ImageRenderer& secondImages,const BattleHudView& view){
 const auto& first=*view.fighters[0];const auto& second=*view.fighters[1];const FighterActor* selectedActor=&first;i32 selectedSequence=0;
QuadPoints quad0{};
CollisionRectangle bounds0{};

  i8 cVar1;
  double dVar2;
  i16 sVar3;
  i32 iVar4;const PatternFrame* selectedFrame;
  const PatternFrame* psVar5;
  u32 uVar6;
  int local_c0;
  int local_bc;
  int local_b8;
  
  int local_a4;
  u32 local_a0;
  
  i16 local_10;
  int local_c;
  float local_8;
  
  quad0={};
  images.blend(SpriteBlend(0));
  local_a0 = (u32)view.hudAlpha[0] << 0x18 | 0xffffff;
  bounds0={i32(0),i32(0),i32(0x80),i32(0x80)};
  quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
  if (double(view.slide) != 0.0) {
    translate_quad(quad0,0.0f,view.slide,0.0f);
  }
  uVar6 = 0;
  selectedActor=&first;selectedSequence=0x29;
  selectedFrame = hud_frame(*selectedActor,selectedSequence,uVar6);
  local_10 = i16(selectedFrame->sprite);
  firstImages.quad((int)local_10,quad0,bounds0,local_a0);
  uVar6 = 0;
  selectedActor=&second;selectedSequence=0x29;
  selectedFrame = hud_frame(*selectedActor,selectedSequence,uVar6);
  local_10 = i16(selectedFrame->sprite);
  scale_quad(quad0,320.0f,0.0f,0.0f,-1.0f,1.0f,1.0f);
  secondImages.quad((int)local_10,quad0,bounds0,local_a0);
  bounds0={i32(0),i32(0),i32(0x280),i32(0x7b)};
  quad0=rectangle_quad(0.0f,0.0f,640.0f,123.0f);
  if (double(view.slide) != 0.0) {
    translate_quad(quad0,0.0f,view.slide,0.0f);
  }
  images.quad(0,quad0,bounds0,local_a0);
  if (i8(first.combat.spellPhase) == '\x02') {
    bounds0={i32(0x281),i32(10),i32(0x362),i32(0x15)};
    quad0=rectangle_quad(31.0f,27.0f,256.0f,38.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,view.slide,0.0f);
    }
    images.quad(0,quad0,bounds0,(u32)view.hudAlpha[0] << 0x18 | 0x808080);
    bounds0={i32(0x362 - (first.combat.maximumHealth * 0xe1) / 10000),i32(0x15),i32(0x362),i32(0x20)};
    quad0=rectangle_quad((double)(0x100 - (first.combat.maximumHealth * 0xe1) / 10000),27.0f,256.0f,38.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,view.slide,0.0f);
    }
    images.quad(0,quad0,bounds0,local_a0);
    bounds0={i32(0x361 - (first.combat.health * 0xe1) / 10000),i32(0xec),i32(0x361),i32(0xf7)};
    quad0=rectangle_quad((double)(0x100 - (first.combat.health * 0xe1) / 10000),27.0f,256.0f,38.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,view.slide,0.0f);
    }
    images.quad(0,quad0,bounds0,local_a0);
  }
  else {
    bounds0={i32(0x362 - (first.combat.maximumHealth * 0xe1) / 10000),i32(0x15),i32(0x362),i32(0x20)};
    quad0=rectangle_quad((double)(0x100 - (first.combat.maximumHealth * 0xe1) / 10000),27.0f,256.0f,38.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,view.slide,0.0f);
    }
    images.quad(0,quad0,bounds0,local_a0);
    bounds0={i32(0x362 - (first.combat.health * 0xe1) / 10000),i32(10),i32(0x362),i32(0x15)};
    quad0=rectangle_quad((double)(0x100 - (first.combat.health * 0xe1) / 10000),27.0f,256.0f,38.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,view.slide,0.0f);
    }
    images.quad(0,quad0,bounds0,(u32)view.hudAlpha[0] << 0x18 | (u32)view.healthFade[0] << 0x10 |
                 (u32)view.healthFade[0] << 8 | (u32)view.healthFade[0]);
  }
  if (u8(view.mode) == '\x03') {
    cVar1 = i8(second.phaseKinds[i8(second.combat.spellPhase)]);
    if (cVar1 == '\0') {
      if ((i8(view.phase) == '\t') || (i8(view.phase) == '\n')) {
        bounds0={i32(0x362 - (second.combat.maximumHealth * 0xe1) /
                        (int)second.phaseMaximum[i8(second.combat.spellPhase)]),i32(10),i32(0x362),i32(0x15)};
        quad0=rectangle_quad((double)((second.combat.maximumHealth * 0x38) /
                             (int)second.phaseMaximum[i8(second.combat.spellPhase)] + 0x180),27.0f,384.0f,38.0f);
      }
      else {
        bounds0={i32(0x281),i32(10),i32(0x362),i32(0x15)};
        quad0=rectangle_quad(440.0f,27.0f,384.0f,38.0f);
      }
      if (double(view.slide) != 0.0) {
        translate_quad(quad0,0.0f,view.slide,0.0f);
      }
      images.quad(0,quad0,bounds0,local_a0);
      bounds0={i32(0x361 - (second.combat.health * 0xe1) /
                      (int)second.phaseMaximum[i8(second.combat.spellPhase)]),i32(0xec),i32(0x361),i32(0xf7)};
      quad0=rectangle_quad((double)((second.combat.health * 0xa9) /
                           (int)second.phaseMaximum[i8(second.combat.spellPhase)] + 0x1b8),27.0f,440.0f,38.0f);
      if (double(view.slide) != 0.0) {
        translate_quad(quad0,0.0f,view.slide,0.0f);
      }
      images.quad(0,quad0,bounds0,local_a0);
    }
    else if (cVar1 == '\x01') {
      bounds0={i32(0x362 - (second.combat.health * 0xe1) /
                      (int)second.phaseMaximum[i8(second.combat.spellPhase)]),i32(10),i32(0x362),i32(0x15)}
      ;
      quad0=rectangle_quad((double)((second.combat.health * 0x38) /
                           (int)second.phaseMaximum[i8(second.combat.spellPhase)] + 0x180),27.0f,384.0f,38.0f);
      if (double(view.slide) != 0.0) {
        translate_quad(quad0,0.0f,view.slide,0.0f);
      }
      images.quad(0,quad0,bounds0,local_a0);
    }
    else if (cVar1 == '\x02') {
      bounds0={i32(0x362 - (second.combat.health * 0xe1) /
                      (int)second.phaseMaximum[i8(second.combat.spellPhase)]),i32(10),i32(0x362),i32(0x15)}
      ;
      quad0=rectangle_quad((double)((second.combat.health * 0xe1) /
                           (int)second.phaseMaximum[i8(second.combat.spellPhase)] + 0x180),27.0f,384.0f,38.0f);
      if (double(view.slide) != 0.0) {
        translate_quad(quad0,0.0f,view.slide,0.0f);
      }
      images.quad(0,quad0,bounds0,local_a0);
    }
  }
  else if (i8(second.combat.spellPhase) == '\x02') {
    bounds0={i32(0x281),i32(10),i32(0x362),i32(0x15)};
    quad0=rectangle_quad(609.0f,27.0f,384.0f,38.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,view.slide,0.0f);
    }
    images.quad(0,quad0,bounds0,(u32)view.hudAlpha[0] << 0x18 | 0x808080);
    bounds0={i32(0x362 - (second.combat.maximumHealth * 0xe1) / 10000),i32(0x15),i32(0x362),i32(0x20)};
    quad0=rectangle_quad((double)((second.combat.maximumHealth * 0xe1) / 10000 + 0x180),27.0f,384.0f,38.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,view.slide,0.0f);
    }
    images.quad(0,quad0,bounds0,local_a0);
    bounds0={i32(0x361 - (second.combat.health * 0xe1) / 10000),i32(0xec),i32(0x361),i32(0xf7)};
    quad0=rectangle_quad((double)((second.combat.health * 0xe1) / 10000 + 0x180),27.0f,384.0f,38.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,view.slide,0.0f);
    }
    images.quad(0,quad0,bounds0,local_a0);
  }
  else {
    bounds0={i32(0x362 - (second.combat.maximumHealth * 0xe1) / 10000),i32(0x15),i32(0x362),i32(0x20)};
    quad0=rectangle_quad((double)((second.combat.maximumHealth * 0xe1) / 10000 + 0x180),27.0f,384.0f,38.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,view.slide,0.0f);
    }
    images.quad(0,quad0,bounds0,local_a0);
    bounds0={i32(0x362 - (second.combat.health * 0xe1) / 10000),i32(10),i32(0x362),i32(0x15)};
    quad0=rectangle_quad((double)((second.combat.health * 0xe1) / 10000 + 0x180),27.0f,384.0f,38.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,view.slide,0.0f);
    }
    images.quad(0,quad0,bounds0,(u32)view.hudAlpha[0] << 0x18 | (u32)view.healthFade[1] << 0x10 |
                 (u32)view.healthFade[1] << 8 | (u32)view.healthFade[1]);
  }
  uVar6 = 0;
  selectedActor=&first;selectedSequence=0x28;
  selectedFrame = hud_frame(*selectedActor,selectedSequence,uVar6);
  local_10 = i16(selectedFrame->sprite);
  uVar6 = 0;
  selectedActor=&first;selectedSequence=0x28;
  psVar5 = hud_frame(*selectedActor,selectedSequence,uVar6);
  local_8 = (double)(int)psVar5->originX;
  bounds0={i32(0),i32(0),i32(0x80),i32(0x10)};
  quad0=rectangle_quad(0.0 - double(local_8),72.0f,128.0 - double(local_8),88.0f);
  if (double(view.slide) != 0.0) {
    translate_quad(quad0,0.0f,view.slide,0.0f);
  }
  firstImages.quad((int)local_10,quad0,bounds0,local_a0);
  uVar6 = 0;
  selectedActor=&second;selectedSequence=0x28;
  selectedFrame = hud_frame(*selectedActor,selectedSequence,uVar6);
  local_10 = i16(selectedFrame->sprite);
  uVar6 = 1;
  selectedActor=&second;selectedSequence=0x28;
  psVar5 = hud_frame(*selectedActor,selectedSequence,uVar6);
  local_8 = (double)(int)psVar5->originX;
  bounds0={i32(0),i32(0),i32(0x80),i32(0x10)};
  quad0=rectangle_quad(0.0 - double(local_8),72.0f,128.0 - double(local_8),88.0f);
  if (double(view.slide) != 0.0) {
    translate_quad(quad0,0.0f,view.slide,0.0f);
  }
  secondImages.quad((int)local_10,quad0,bounds0,local_a0);
  if (u8(view.mode) == '\x03') {
    bounds0={i32(0x2f4),i32(0x27),i32(0x31d),i32(0x35)};
    quad0=rectangle_quad(115.0f,5.0f,156.0f,19.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,view.slide,0.0f);
    }
    images.quad(0,quad0,bounds0,local_a0);
    number(images,280.0f,double(view.slide) + 5.0,local_a0,first.score,0xc,10);
    bounds0={i32(0x323),i32(0x27),i32(0x35a),i32(0x35)};
    quad0=rectangle_quad(376.0f,5.0f,431.0f,19.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,view.slide,0.0f);
    }
    images.quad(0,quad0,bounds0,local_a0);
    number(images,555.0f,double(view.slide) + 5.0,local_a0,view.highScore,0xc,10);
    bounds0={i32(0x362),i32(0x29),i32(0x39b),i32(0x36)};
    quad0=rectangle_quad(1.0f,5.0f,58.0f,18.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,view.slide,0.0f);
    }
    images.quad(0,quad0,bounds0,local_a0);
    number(images,70.0f,double(view.slide) + 5.0,local_a0,(int)view.remainingLives,0xb);
  }
  else {
    bounds0={i32(0x2f4),i32(0x27),i32(0x31d),i32(0x35)};
    quad0=rectangle_quad(115.0f,5.0f,156.0f,19.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,view.slide,0.0f);
    }
    images.quad(0,quad0,bounds0,local_a0);
    number(images,280.0f,double(view.slide) + 5.0,local_a0,first.score,0xc,10);
    bounds0={i32(0x2f4),i32(0x27),i32(0x31d),i32(0x35)};
    quad0=rectangle_quad(370.0f,5.0f,411.0f,19.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,view.slide,0.0f);
    }
    images.quad(0,quad0,bounds0,local_a0);
    number(images,535.0f,double(view.slide) + 5.0,local_a0,second.score,0xc,10);
  }
  local_a0 = (u32)view.hudAlpha[1] << 0x18 | 0xffffff;
  if (900.0 <= double(first.combat.spellExtent)) {
    local_a4 = 8;
    local_b8 = 0x3a;
  }
  else {
    local_a4 = i32(double(first.combat.spellExtent)/100.0);
    iVar4 = i32(double(first.combat.spellExtent)*10.0);
    local_b8 = ((iVar4 % 1000 + 1) * 0x3a) / 1000;
  }
  bounds0={i32(local_a4 % 3 << 6),i32(((local_a4 / 3) * 0x3a + 0x88) - local_b8),i32((local_a4 % 3) * 0x40 + 0x40),i32((local_a4 / 3) * 0x3a + 0x88)};
  quad0=rectangle_quad(0.0f,(double)(0x3a - local_b8),64.0f,58.0f);
  translate_quad(quad0,1.0f,416.0 - double(view.slide),0.0f);
  images.quad(8,quad0,bounds0,local_a0);
  if (900.0 <= double(second.combat.spellExtent)) {
    local_a4 = 8;
    local_b8 = 0x3a;
  }
  else {
    local_a4 = i32(double(second.combat.spellExtent)/100.0);
    iVar4 = i32(double(second.combat.spellExtent)*10.0);
    local_b8 = ((iVar4 % 1000 + 1) * 0x3a) / 1000;
  }
  bounds0={i32(local_a4 % 3 << 6),i32(((local_a4 / 3) * 0x3a + 0x88) - local_b8),i32((local_a4 % 3) * 0x40 + 0x40),i32((local_a4 / 3) * 0x3a + 0x88)};
  quad0=rectangle_quad(0.0f,(double)(0x3a - local_b8),64.0f,58.0f);
  translate_quad(quad0,1.0f,416.0 - double(view.slide),0.0f);
  scale_quad(quad0,320.0f,0.0f,0.0f,-1.0f,1.0f,1.0f);
  images.quad(8,quad0,bounds0,local_a0);
  bounds0={i32(0),i32(0xbc),i32(0x12d),i32(0x100)};
  quad0=rectangle_quad(0.0f,412.0f,301.0f,480.0f);
  if (double(view.slide) != 0.0) {
    translate_quad(quad0,0.0f,-double(view.slide),0.0f);
  }
  images.quad(0,quad0,bounds0,local_a0);
  scale_quad(quad0,320.0f,0.0f,0.0f,-1.0f,1.0f,1.0f);
  images.quad(0,quad0,bounds0,local_a0);
  if (first.combat.spiritBreak < 1) {
    local_bc = 0;
    dVar2 = (double)(int)first.combat.spirit;
    if (dVar2 < 250.0 == (dVar2 == 250.0)) {
      dVar2 = (double)(int)first.combat.spirit;
      if (dVar2 < 500.0 == (dVar2 == 500.0)) {
        dVar2 = (double)(int)first.combat.spirit;
        if (dVar2 < 750.0 != (dVar2 == 750.0)) {
          local_bc = 9;
        }
      }
      else {
        local_bc = 0x12;
      }
    }
    else {
      local_bc = (u32)((u32)u16(first.combat.elapsed) % 0x14 < 10) * 9 + 0x12;
    }
    bounds0={i32(0x280),i32(local_bc + 200),i32(0x34c),i32(local_bc + 0xd1)};
    quad0=rectangle_quad((double)(0x111 - (first.combat.spirit * 0xc6) / 1000),462.0f,279.0f,471.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,-double(view.slide),0.0f);
    }
    images.quad(0,quad0,bounds0,local_a0);
  }
  else {
    bounds0={i32(0x280),i32(0xe3),i32(0x34c),i32(0xec)};
    quad0=rectangle_quad((double)((first.combat.spiritBreak * 0xcc) / 1000 + 0x4b),462.0f,279.0f,471.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,-double(view.slide),0.0f);
    }
    images.quad(0,quad0,bounds0,local_a0);
  }
  if (second.combat.spiritBreak < 1) {
    local_c0 = 0;
    dVar2 = (double)(int)second.combat.spirit;
    if (dVar2 < 250.0 == (dVar2 == 250.0)) {
      dVar2 = (double)(int)second.combat.spirit;
      if (dVar2 < 500.0 == (dVar2 == 500.0)) {
        dVar2 = (double)(int)second.combat.spirit;
        if (dVar2 < 750.0 != (dVar2 == 750.0)) {
          local_c0 = 9;
        }
      }
      else {
        local_c0 = 0x12;
      }
    }
    else {
      local_c0 = (u32)((u32)u16(second.combat.elapsed) % 0x14 < 10) * 9 + 0x12;
    }
    bounds0={i32(0x280),i32(local_c0 + 200),i32(0x34c),i32(local_c0 + 0xd1)};
    quad0=rectangle_quad((double)((second.combat.spirit * 0xcc) / 1000 + 0x169),462.0f,361.0f,471.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,-double(view.slide),0.0f);
    }
    images.quad(0,quad0,bounds0,local_a0);
  }
  else {
    bounds0={i32(0x280),i32(0xe3),i32(0x34c),i32(0xec)};
    quad0=rectangle_quad((double)(0x235 - (second.combat.spiritBreak * 0xcc) / 1000),462.0f,361.0f,471.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,-double(view.slide),0.0f);
    }
    images.quad(0,quad0,bounds0,local_a0);
  }
  bounds0={i32(0x362),i32(0),i32(0x392),i32(0x13)};
  quad0=rectangle_quad(65.0f,437.0f,113.0f,456.0f);
  if (double(view.slide) != 0.0) {
    translate_quad(quad0,0.0f,-double(view.slide),0.0f);
  }
  images.quad(0,quad0,bounds0,local_a0);
  if (u8(view.mode) != '\x03') {
    translate_quad(quad0,431.0f,0.0f,0.0f);
    images.quad(0,quad0,bounds0,local_a0);
  }
  bounds0={i32(0x3b4),i32(3),i32(0x3c4),i32(0x12)};
  quad0=rectangle_quad(115.0f,440.0f,131.0f,455.0f);
  if (double(view.slide) != 0.0) {
    translate_quad(quad0,0.0f,-double(view.slide),0.0f);
  }
  images.quad(0,quad0,bounds0,local_a0);
  translate_quad(quad0,16.0f,0.0f,0.0f);
  images.quad(0,quad0,bounds0,local_a0);
  if (u8(view.mode) != '\x03') {
    translate_quad(quad0,431.0f,0.0f,0.0f);
    images.quad(0,quad0,bounds0,local_a0);
    translate_quad(quad0,-16.0f,0.0f,0.0f);
    images.quad(0,quad0,bounds0,local_a0);
  }
  bounds0={i32(0x394),i32(3),i32(0x3a4),i32(0x12)};
  quad0=rectangle_quad(115.0f,440.0f,131.0f,455.0f);
  if (double(view.slide) != 0.0) {
    translate_quad(quad0,0.0f,-double(view.slide),0.0f);
  }
  for (local_c = 0; local_c < i8(first.selectedSpell); local_c = local_c + 1) {
    images.quad(0,quad0,bounds0,local_a0);
    translate_quad(quad0,16.0f,0.0f,0.0f);
  }
  if (u8(view.mode) != '\x03') {
    quad0=rectangle_quad(546.0f,440.0f,562.0f,455.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,-double(view.slide),0.0f);
    }
    for (local_c = 0; local_c < i8(second.selectedSpell); local_c = local_c + 1)
    {
      images.quad(0,quad0,bounds0,local_a0);
      translate_quad(quad0,16.0f,0.0f,0.0f);
    }
  }
  bounds0={i32((2 - i8(first.combat.spellPhase)) * 0x40 + 0x340),i32(0x40),i32((2 - i8(first.combat.spellPhase)) * 0x40 + 0x380),i32(0x60)};
  quad0=rectangle_quad(223.0f,433.0f,287.0f,465.0f);
  if (double(view.slide) != 0.0) {
    translate_quad(quad0,0.0f,-double(view.slide),0.0f);
  }
  images.quad(0,quad0,bounds0,local_a0);
  if (u8(view.mode) != '\x03') {
    bounds0={i32((2 - i8(second.combat.spellPhase)) * 0x40 + 0x340),i32(0x40),i32((2 - i8(second.combat.spellPhase)) * 0x40 + 0x380),i32(0x60)};
    quad0=rectangle_quad(380.0f,433.0f,444.0f,465.0f);
    if (double(view.slide) != 0.0) {
      translate_quad(quad0,0.0f,-double(view.slide),0.0f);
    }
    images.quad(0,quad0,bounds0,local_a0);
  }
  sVar3 = i32(double(first.combat.spellExtent)/100.0);
  number(images,33.0f,439.0 - double(view.slide),local_a0,(int)sVar3,10);
  if (u8(view.mode) != '\x03') {
    sVar3 = i32(double(second.combat.spellExtent)/100.0);
    number(images,637.0f,439.0 - double(view.slide),local_a0,(int)sVar3,10);
  }
  render_combo(images,combos[0]);
  render_combo(images,combos[1]);
  return;

}
void BattleHud::render_training_menu(ImageRenderer& images,ImageRenderer& firstImages,const BattleHudView& view){const auto& first=*view.fighters[0];const auto& second=*view.fighters[1];
QuadPoints quad0{};
CollisionRectangle bounds0{};

  
  u32 local_98;
  
  int local_8;
  
  quad0={};
  images.blend(SpriteBlend(0));
  bounds0={i32(0),i32(0),i32(0x280),i32(0x1e0)};
  images.graphics.filled_rectangle(bounds0,(u32)view.pauseFade << 0x18);
  if (0x31 < view.pauseFade) {
    for (local_8 = 0; local_8 < 9; local_8 = local_8 + 1) {
      if (local_8 == i8(view.pauseChoice)) {
        local_98 = 0xffffffff;
      }
      else {
        local_98 = 0xff808080;
      }
      bounds0={i32(0),i32(local_8 * 0x18),i32(0xf0),i32(local_8 * 0x18 + 0x18)};
      quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
      if (5 < local_8) {
        translate_quad(quad0,0.0f,24.0f,0.0f);
      }
      translate_quad(quad0,10.0f,90.0f,0.0f);
      images.quad(10,quad0,bounds0,local_98);
    }
    for (local_8 = 0; local_8 < 6; local_8 = local_8 + 1) {
      if (local_8 == i8(second.training.guard)) {
        local_98 = 0xffffffff;
      }
      else {
        local_98 = 0xff808080;
      }
      bounds0={i32((local_8 % 3) * 0x48),i32((local_8 / 3) * 0x18 + 0xd8),i32((local_8 % 3) * 0x48 + 0x48),i32((local_8 / 3) * 0x18 + 0xf0)};
      quad0=rectangle_quad(0.0f,0.0f,72.0f,24.0f);
      translate_quad(quad0,(double)(local_8 * 0x3c + 0x104),90.0f,0.0f);
      images.quad(10,quad0,bounds0,local_98);
    }
    for (local_8 = 0; local_8 < 10; local_8 = local_8 + 1) {
      if (local_8 == i8(second.trainingSpell)) {
        local_98 = 0xffffffff;
      }
      else {
        local_98 = 0xff808080;
      }
      bounds0={i32(local_8 * 0x18),i32(0x108),i32(local_8 * 0x18 + 0x18),i32(0x120)};
      quad0=rectangle_quad((double)(local_8 * 0x18),0.0f,(double)(local_8 * 0x18 + 0x18),24.0f);
      translate_quad(quad0,260.0f,114.0f,0.0f);
      images.quad(10,quad0,bounds0,local_98);
    }
    if (i8(second.training.counter) == '\0') {
      local_98 = 0xffffffff;
    }
    else {
      local_98 = 0xff808080;
    }
    bounds0={i32(0),i32(0x120),i32(0x48),i32(0x138)};
    quad0=rectangle_quad(0.0f,0.0f,72.0f,24.0f);
    translate_quad(quad0,260.0f,138.0f,0.0f);
    images.quad(10,quad0,bounds0,local_98);
    if (i8(second.training.counter) == '\x01') {
      local_98 = 0xffffffff;
    }
    else {
      local_98 = 0xff808080;
    }
    bounds0={i32(0x48),i32(0x120),i32(0xa8),i32(0x138)};
    quad0=rectangle_quad(0.0f,0.0f,96.0f,24.0f);
    translate_quad(quad0,332.0f,138.0f,0.0f);
    images.quad(10,quad0,bounds0,local_98);
    if (i8(second.training.counter) == '\x02') {
      local_98 = 0xffffffff;
    }
    else {
      local_98 = 0xff808080;
    }
    bounds0={i32(0),i32(0x138),i32(0x78),i32(0x150)};
    quad0=rectangle_quad(0.0f,0.0f,120.0f,24.0f);
    translate_quad(quad0,428.0f,138.0f,0.0f);
    images.quad(10,quad0,bounds0,local_98);
    if (i8(second.training.counter) == '\x03') {
      local_98 = 0xffffffff;
    }
    else {
      local_98 = 0xff808080;
    }
    bounds0={i32(0x78),i32(0x138),i32(0xa8),i32(0x150)};
    quad0=rectangle_quad(0.0f,0.0f,48.0f,24.0f);
    translate_quad(quad0,548.0f,138.0f,0.0f);
    images.quad(10,quad0,bounds0,local_98);
    for (local_8 = 0; local_8 < 2; local_8 = local_8 + 1) {
      if (local_8 == i8(second.training.recovery)) {
        local_98 = 0xffffffff;
      }
      else {
        local_98 = 0xff808080;
      }
      bounds0={i32(local_8 * 0x48),i32(0x150),i32(local_8 * 0x48 + 0x48),i32(0x168)};
      quad0=rectangle_quad(0.0f,0.0f,72.0f,24.0f);
      translate_quad(quad0,(double)(local_8 * 0x48 + 0x104),162.0f,0.0f);
      images.quad(10,quad0,bounds0,local_98);
    }
    if (i8(second.training.direction) == '\0') {
      local_98 = 0xffffffff;
    }
    else {
      local_98 = 0xff808080;
    }
    bounds0={i32(0),i32(0x168),i32(0x48),i32(0x180)};
    quad0=rectangle_quad(0.0f,0.0f,72.0f,24.0f);
    translate_quad(quad0,260.0f,186.0f,0.0f);
    images.quad(10,quad0,bounds0,local_98);
    if (i8(second.training.direction) == '\x01') {
      local_98 = 0xffffffff;
    }
    else {
      local_98 = 0xff808080;
    }
    bounds0={i32(0x48),i32(0x168),i32(0xa8),i32(0x180)};
    quad0=rectangle_quad(0.0f,0.0f,96.0f,24.0f);
    translate_quad(quad0,332.0f,186.0f,0.0f);
    images.quad(10,quad0,bounds0,local_98);
    if (i8(second.training.direction) == '\x02') {
      local_98 = 0xffffffff;
    }
    else {
      local_98 = 0xff808080;
    }
    bounds0={i32(0),i32(0x180),i32(0x48),i32(0x198)};
    quad0=rectangle_quad(0.0f,0.0f,72.0f,24.0f);
    translate_quad(quad0,428.0f,186.0f,0.0f);
    images.quad(10,quad0,bounds0,local_98);
    if (i8(second.training.direction) == '\x03') {
      local_98 = 0xffffffff;
    }
    else {
      local_98 = 0xff808080;
    }
    bounds0={i32(0x48),i32(0x180),i32(0xa8),i32(0x198)};
    quad0=rectangle_quad(0.0f,0.0f,96.0f,24.0f);
    translate_quad(quad0,500.0f,186.0f,0.0f);
    images.quad(10,quad0,bounds0,local_98);
    if (i8(second.training.position) == '\0') {
      local_98 = 0xffffffff;
    }
    else {
      local_98 = 0xff808080;
    }
    bounds0={i32(0),i32(0x198),i32(0x48),i32(0x1b0)};
    quad0=rectangle_quad(0.0f,0.0f,72.0f,24.0f);
    translate_quad(quad0,260.0f,210.0f,0.0f);
    images.quad(10,quad0,bounds0,local_98);
    if (i8(second.training.position) == '\x01') {
      local_98 = 0xffffffff;
    }
    else {
      local_98 = 0xff808080;
    }
    bounds0={i32(0x48),i32(0x198),i32(0xa8),i32(0x1b0)};
    quad0=rectangle_quad(0.0f,0.0f,96.0f,24.0f);
    translate_quad(quad0,332.0f,210.0f,0.0f);
    images.quad(10,quad0,bounds0,local_98);
    bounds0={i32(0),i32(0),i32(0x100),i32(0x100)};
    quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
    translate_quad(quad0,340.0f,280.0f,0.0f);
    firstImages.quad(i8(first.portraitMaximum) + -1,quad0,bounds0,0xffffffff);
  }
  return;

}
void BattleHud::render_pause_menu(ImageRenderer& images,ImageRenderer& firstImages,ImageRenderer& secondImages,const BattleHudView& view){const auto& first=*view.fighters[0];const auto& second=*view.fighters[1];
QuadPoints quad0{};
CollisionRectangle bounds0{};

  
  
  
  if (u8(view.mode) == '\x05') {
    render_training_menu(images,firstImages,view);
  }
  else {
    quad0={};
    images.blend(SpriteBlend(0));
    bounds0={i32(0),i32(0),i32(0x280),i32(0x1e0)};
    images.graphics.filled_rectangle(bounds0,(u32)view.pauseFade << 0x18);
    if ((0x31 < view.pauseFade) && (i8(view.paused) != '\0')) {
      if (view.replayMode != '\x02') {
        bounds0={i32(0),i32(0),i32(0x100),i32(0x100)};
        quad0=rectangle_quad(float(bounds0.left),float(bounds0.top),float(bounds0.right),float(bounds0.bottom));
        if ((i8(first.combat.storyOpponent) == '\0') &&
           (i8(second.combat.storyOpponent) == '\0')) {
          translate_quad(quad0,32.0f,280.0f,0.0f);
          firstImages.quad(i8(first.portraitMaximum) + -1,quad0,bounds0,0xffffffff);
          translate_quad(quad0,320.0f,0.0f,0.0f);
          secondImages.quad(i8(second.portraitMaximum) + -1,quad0,bounds0,0xffffffff);
        }
        else if (i8(first.combat.storyOpponent) == '\0') {
          translate_quad(quad0,192.0f,280.0f,0.0f);
          firstImages.quad(i8(first.portraitMaximum) + -1,quad0,bounds0,0xffffffff);
        }
        else {
          translate_quad(quad0,192.0f,280.0f,0.0f);
          secondImages.quad(i8(second.portraitMaximum) + -1,quad0,bounds0,0xffffffff);
        }
      }
      if ((i8(view.phase) == '\f') && ('\0' < view.continueCredits)) {
        bounds0={i32(0x120),i32(0xc0),i32(0x200),i32(0xe0)};
        quad0=rectangle_quad(176.0f,230.0f,400.0f,262.0f);
        images.quad(0,quad0,bounds0,0xffffffff);
        bounds0={i32(0x200),i32(view.continueCredits * 0x20 + 0x60),i32(0x240),i32(view.continueCredits * 0x20 + 0x80)};
        quad0=rectangle_quad(390.0f,230.0f,454.0f,262.0f);
        images.quad(0,quad0,bounds0,0xffffffff);
      }
      if ((i8(view.phase) != '\x05') &&
         ((i8(view.phase) != '\f' || (view.continueCredits != '\0')))) {
        bounds0={i32(0x180),i32((u32)(i8(view.pauseChoice) != '\0') * 0x20 + 0x80),i32(0x200),i32((u32)(i8(view.pauseChoice) != '\0') * 0x20 + 0xa0)};
        quad0=rectangle_quad(256.0f,140.0f,384.0f,172.0f);
        images.quad(0,quad0,bounds0,0xffffffff);
      }
      bounds0={i32(0x100),i32((u32)(i8(view.pauseChoice) != '\x01') * 0x20 + 0x80),i32(0x180),i32((u32)(i8(view.pauseChoice) != '\x01') * 0x20 + 0xa0)};
      quad0=rectangle_quad(256.0f,170.0f,384.0f,202.0f);
      images.quad(0,quad0,bounds0,0xffffffff);
      if (((view.replayMode != '\x02') && (view.continued == '\0')) &&
         (i8(view.phase) != '\a')) {
        bounds0={i32(0),i32((u32)(i8(view.pauseChoice) != '\x02') * 0x20 + 0x80),i32(0x100),i32((u32)(i8(view.pauseChoice) != '\x02') * 0x20 + 0x9b)};
        quad0=rectangle_quad(192.0f,200.0f,448.0f,227.0f);
        images.quad(0,quad0,bounds0,0xffffffff);
      }
    }
  }
  return;

}
}
