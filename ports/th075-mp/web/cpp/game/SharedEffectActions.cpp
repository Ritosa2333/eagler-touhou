#include "SharedEffects.hpp"
#include <cmath>
namespace th075 {
namespace {
// Keep each policy group out of line: Safari needs a bounded native Wasm
// call frame when a hit effect immediately creates and updates child particles.
// Child update order and RNG consumption remain synchronous and unchanged.
[[gnu::noinline]] void advance_group_0(SharedEffect& e,ReplayRandom& random,SharedEffectEvents& events){
    i16 shortBits=0,shortValue=0;i32 randomValue=0;double angleRadians=0;float launchSpeed=0;
    const auto emit=[&](i16 id,float x,float y,i8 facing,u8 layer){events.spawn(id,x,y,facing,layer,e.parameters);};
  switch(e.cursor.action) {
  case 1:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 0.5f;
      e.scale[1] = 0.5f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x2d);
      e.rotation[0] = (float)randomValue;
      randomValue = random.range(0,0x2d);
      e.rotation[1] = (float)randomValue;
      randomValue = random.range(0,0x2d);
      e.rotation[2] = (float)randomValue;
      e.age = 0;
    }
    e.age = e.age + 1;
    e.scale[0] =
         (double(0.3f) - (float)(int)e.age * double(0.035f)) + double(e.scale[0]);
    e.scale[1] =
         (double(0.3f) - (float)(int)e.age * double(0.035f)) + double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x1f) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x1e;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 2:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 0.5f;
      e.scale[1] = 0.5f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x2d);
      e.rotation[0] = (float)randomValue;
      randomValue = random.range(0,0x2d);
      e.rotation[1] = (float)randomValue;
      randomValue = random.range(0,0x2d);
      e.rotation[2] = (float)randomValue;
      e.age = 0;
    }
    if ((e.cursor.index == 1) && (e.cursor.elapsed == 0)) {
      emit(0x65,e.motion.x,e.motion.y,1,1);
      emit(0x66,e.motion.x,e.motion.y,1,1);
      emit(0x66,e.motion.x,e.motion.y,1,1);
      emit(0x66,e.motion.x,e.motion.y,1,1);
      emit(0x66,e.motion.x,e.motion.y,1,1);
      emit(0x66,e.motion.x,e.motion.y,1,1);
      emit(0x66,e.motion.x,e.motion.y,1,1);
      emit(0x66,e.motion.x,e.motion.y,1,1);
      emit(0x66,e.motion.x,e.motion.y,1,1);
      emit(0x66,e.motion.x,e.motion.y,1,1);
      emit(0x66,e.motion.x,e.motion.y,1,1);
      emit(0x66,e.motion.x,e.motion.y,1,1);
    }
    e.age = e.age + 1;
    e.scale[0] =
         (double(0.3f) - (float)(int)e.age * double(0.035f)) + double(e.scale[0]);
    e.scale[1] =
         (double(0.3f) - (float)(int)e.age * double(0.035f)) + double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x1f) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 3:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 0.5f;
      e.scale[1] = 0.5f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x2d);
      e.rotation[0] = (float)randomValue;
      randomValue = random.range(0,0x2d);
      e.rotation[1] = (float)randomValue;
      randomValue = random.range(0,0x2d);
      e.rotation[2] = (float)randomValue;
      e.age = 0;
    }
    if ((e.cursor.index == 1) && (e.cursor.elapsed == 0)) {
      emit(0x67,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
    }
    e.age = e.age + 1;
    e.scale[0] =
         (double(0.3f) - (float)(int)e.age * double(0.035f)) + double(e.scale[0]);
    e.scale[1] =
         (double(0.3f) - (float)(int)e.age * double(0.035f)) + double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x1f) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 5:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 4.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x14);
      e.rotation[2] = (float)(100 - randomValue);
      e.age = 0;
    }
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
    }
    e.scale[0] =
         (double(0.5f) - (float)(int)e.age * double(0.035f)) + double(e.scale[0]);
    e.scale[1] = double(e.scale[1]) * double(0.75f);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x3d) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x3c;
      }
      if (0x3c < e.red) {
        e.red = i8(e.red) + -0x3c;
      }
      if (0x3c < e.green) {
        e.green = i8(e.green) + -0x3c;
      }
    }
    e.age = e.age + 1;
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 6:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 4.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x14);
      e.rotation[2] = (float)(-0x23 - randomValue);
      e.age = 0;
    }
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
    }
    e.scale[0] =
         (double(0.5f) - (float)(int)e.age * double(0.035f)) + double(e.scale[0]);
    e.scale[1] = double(e.scale[1]) * double(0.75f);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x3d) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x3c;
      }
      if (0x3c < e.red) {
        e.red = i8(e.red) + -0x3c;
      }
      if (0x3c < e.green) {
        e.green = i8(e.green) + -0x3c;
      }
    }
    e.age = e.age + 1;
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 7:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 4.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x14);
      e.rotation[2] = (float)(10 - randomValue);
      e.age = 0;
    }
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
    }
    e.scale[0] =
         (double(0.5f) - (float)(int)e.age * double(0.035f)) + double(e.scale[0]);
    e.scale[1] = double(e.scale[1]) * double(0.75f);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x3d) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x3c;
      }
      if (0x3c < e.red) {
        e.red = i8(e.red) + -0x3c;
      }
      if (0x3c < e.green) {
        e.green = i8(e.green) + -0x3c;
      }
    }
    e.age = e.age + 1;
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 8:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 4.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x14);
      e.rotation[2] = (float)(0x37 - randomValue);
      e.age = 0;
    }
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
    }
    e.scale[0] =
         (double(0.5f) - (float)(int)e.age * double(0.035f)) + double(e.scale[0]);
    e.scale[1] = double(e.scale[1]) * double(0.75f);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x3d) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x3c;
      }
      if (0x3c < e.red) {
        e.red = i8(e.red) + -0x3c;
      }
      if (0x3c < e.green) {
        e.green = i8(e.green) + -0x3c;
      }
    }
    e.age = e.age + 1;
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0xd:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      randomValue = random.range(0,10);
      e.scale[0] = (float)randomValue * double(0.1f) + double(3.0f);
      e.scale[1] = e.scale[0];
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
    }
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      emit(0x8a,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
    }
    e.age = e.age + 1;
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  }
}
[[gnu::noinline]] void advance_group_1(SharedEffect& e,ReplayRandom& random,SharedEffectEvents& events){
    i16 shortBits=0,shortValue=0;i32 randomValue=0;double angleRadians=0;float launchSpeed=0;
    u32 growthEnabled0=0;
    u32 growthEnabled1=0;
    u32 growthEnabled2=0;
    u32 growthEnabled3=0;
    u32 growthEnabled4=0;
    u32 growthEnabled5=0;
    u32 growthEnabled6=0;
    u32 growthEnabled7=0;
    const auto emit=[&](i16 id,float x,float y,i8 facing,u8 layer){events.spawn(id,x,y,facing,layer,e.parameters);};
  switch(e.cursor.action) {
  case 0xe:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 0.5f;
      e.scale[1] = 0.5f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
    }
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      emit(0x69,double(e.motion.x) - (float)(i8(e.motion.facing) * 0x3c),
                   e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
    }
    e.age = e.age + 1;
    e.scale[0] =
         (double(0.3f) - (float)(int)e.age * double(0.02f)) + double(e.scale[0]);
    e.scale[1] =
         (double(0.3f) - (float)(int)e.age * double(0.02f)) + double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x1f) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0xf:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 0.5f;
      e.scale[1] = e.scale[0];
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
    }
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      emit(0x8b,e.motion.x,e.motion.y,1,1);
      emit(0x8c,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
    }
    growthEnabled0 = (u32)(double(0.0f) < double(0.2f) - (float)(int)e.age * double(0.02f));
    e.scale[0] =
         (double(0.2f) - (float)(int)e.age * double(0.02f)) * (float)growthEnabled0 +
         double(e.scale[0]);
    growthEnabled1 = (u32)(double(0.0f) < double(0.2f) - (float)(int)e.age * double(0.02f));
    e.scale[1] =
         (double(0.2f) - (float)(int)e.age * double(0.02f)) * (float)growthEnabled1 +
         double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x1a) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x19;
      }
      if (0x19 < e.red) {
        e.red = i8(e.red) + -0x19;
      }
      if (0x19 < e.green) {
        e.green = i8(e.green) + -0x19;
      }
    }
    e.age = e.age + 1;
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x32:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 0.25f;
      e.scale[1] = 0.25f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.rotation[1] = 60.0f;
      e.age = 0;
    }
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
    }
    e.age = e.age + 1;
    growthEnabled2 = (u32)(double(0.0f) < double(0.2f) - (float)(int)e.age * double(0.02f));
    e.scale[0] =
         (double(0.2f) - (float)(int)e.age * double(0.02f)) * (float)growthEnabled2 +
         double(e.scale[0]);
    growthEnabled3 = (u32)(double(0.0f) < double(0.2f) - (float)(int)e.age * double(0.02f));
    e.scale[1] =
         (double(0.2f) - (float)(int)e.age * double(0.02f)) * (float)growthEnabled3 +
         double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x1a) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x19;
      }
      if (0x19 < e.red) {
        e.red = i8(e.red) + -0x19;
      }
      if (0x19 < e.green) {
        e.green = i8(e.green) + -0x19;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x33:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 0.25f;
      e.scale[1] = 0.25f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.rotation[1] = 60.0f;
      e.age = 0;
    }
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      emit(0x69,e.motion.x,e.motion.y,1,1);
      emit(0x69,e.motion.x,e.motion.y,1,1);
      emit(0x69,e.motion.x,e.motion.y,1,1);
      emit(0x69,e.motion.x,e.motion.y,1,1);
      emit(0x69,e.motion.x,e.motion.y,1,1);
      emit(0x69,e.motion.x,e.motion.y,1,1);
      emit(0x69,e.motion.x,e.motion.y,1,1);
      emit(0x69,e.motion.x,e.motion.y,1,1);
    }
    e.age = e.age + 1;
    growthEnabled4 = (u32)(double(0.0f) < double(0.2f) - (float)(int)e.age * double(0.02f));
    e.scale[0] =
         (double(0.2f) - (float)(int)e.age * double(0.02f)) * (float)growthEnabled4 +
         double(e.scale[0]);
    growthEnabled5 = (u32)(double(0.0f) < double(0.2f) - (float)(int)e.age * double(0.02f));
    e.scale[1] =
         (double(0.2f) - (float)(int)e.age * double(0.02f)) * (float)growthEnabled5 +
         double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x1a) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x19;
      }
      if (0x19 < e.red) {
        e.blue = i8(e.blue) + -0x19;
      }
      if (0x19 < e.green) {
        e.green = i8(e.green) + -0x19;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x34:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      emit(0x6b,e.motion.x,e.motion.y,1,1);
      emit(0x6b,e.motion.x,e.motion.y,1,1);
      emit(0x6b,e.motion.x,e.motion.y,1,1);
      emit(0x6b,e.motion.x,e.motion.y,1,1);
      emit(0x6b,e.motion.x,e.motion.y,1,1);
      emit(0x6b,e.motion.x,e.motion.y,1,1);
      emit(0x6b,e.motion.x,e.motion.y,1,1);
      emit(0x6b,e.motion.x,e.motion.y,1,1);
      emit(0x6b,e.motion.x,e.motion.y,1,1);
      emit(0x6b,e.motion.x,e.motion.y,1,1);
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x35:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      emit(0x6c,e.motion.x,e.motion.y,1,1);
      emit(0x6d,e.motion.x,e.motion.y,1,1);
      emit(0x6e,e.motion.x,e.motion.y,1,1);
      emit(0x6e,e.motion.x,e.motion.y,1,1);
      emit(0x6e,e.motion.x,e.motion.y,1,1);
      emit(0x6e,e.motion.x,e.motion.y,1,1);
      emit(0x6e,e.motion.x,e.motion.y,1,1);
      emit(0x6e,e.motion.x,e.motion.y,1,1);
      emit(0x6e,e.motion.x,e.motion.y,1,1);
      emit(0x6e,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
      emit(0x68,e.motion.x,e.motion.y,1,1);
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x36:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      emit(0x89,e.motion.x,e.motion.y,1,1);
      emit(0x6f,e.motion.x,e.motion.y,1,1);
      emit(0x6f,e.motion.x,e.motion.y,1,1);
      emit(0x6f,e.motion.x,e.motion.y,1,1);
      emit(0x6f,e.motion.x,e.motion.y,1,1);
      emit(0x6f,e.motion.x,e.motion.y,1,1);
      emit(0x6f,e.motion.x,e.motion.y,1,1);
      emit(0x6f,e.motion.x,e.motion.y,1,1);
      emit(0x6f,e.motion.x,e.motion.y,1,1);
      emit(0x6f,e.motion.x,e.motion.y,1,1);
      emit(0x6f,e.motion.x,e.motion.y,1,1);
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x37:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
    }
    if ((e.cursor.index == 1) && (e.cursor.elapsed == 0)) {
      emit(0x70,e.motion.x,e.motion.y,1,1);
      emit(0x71,e.motion.x,e.motion.y,1,1);
      emit(0x72,e.motion.x,e.motion.y,1,1);
      emit(0x72,e.motion.x,e.motion.y,1,1);
      emit(0x72,e.motion.x,e.motion.y,1,1);
      emit(0x72,e.motion.x,e.motion.y,1,1);
      emit(0x72,e.motion.x,e.motion.y,1,1);
      emit(0x72,e.motion.x,e.motion.y,1,1);
      emit(0x72,e.motion.x,e.motion.y,1,1);
      emit(0x72,e.motion.x,e.motion.y,1,1);
      emit(0x72,e.motion.x,e.motion.y,1,1);
      emit(0x72,e.motion.x,e.motion.y,1,1);
      emit(0x72,e.motion.x,e.motion.y,1,1);
      emit(0x72,e.motion.x,e.motion.y,1,1);
      emit(0x72,e.motion.x,e.motion.y,1,1);
      emit(0x72,e.motion.x,e.motion.y,1,1);
    }
    e.age = e.age + 1;
    growthEnabled6 = (u32)(double(0.0f) < double(0.5f) - (float)(int)e.age * double(0.035f));
    e.scale[0] =
         (double(0.5f) - (float)(int)e.age * double(0.035f)) * (float)growthEnabled6 +
         double(e.scale[0]);
    growthEnabled7 = (u32)(double(0.0f) < double(0.5f) - (float)(int)e.age * double(0.035f));
    e.scale[1] =
         (double(0.5f) - (float)(int)e.age * double(0.035f)) * (float)growthEnabled7 +
         double(e.scale[1]);
    if ((0 < e.cursor.index) && (3 < e.cursor.elapsed)) {
      if (e.alpha < 0x1f) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  }
}
[[gnu::noinline]] void advance_group_2(SharedEffect& e,ReplayRandom& random,SharedEffectEvents& events){
    i16 shortBits=0,shortValue=0;i32 randomValue=0;double angleRadians=0;float launchSpeed=0;
    u32 growthEnabled8=0;
    u32 growthEnabled9=0;
    const auto emit=[&](i16 id,float x,float y,i8 facing,u8 layer){events.spawn(id,x,y,facing,layer,e.parameters);};
  switch(e.cursor.action) {
  case 0x38:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
      emit(0x39,e.motion.x,e.motion.y,1,1);
      emit(0x39,e.motion.x,e.motion.y,1,1);
      emit(0x39,e.motion.x,e.motion.y,1,1);
      emit(0x39,e.motion.x,e.motion.y,1,1);
      emit(0x39,e.motion.x,e.motion.y,1,1);
      emit(0x39,e.motion.x,e.motion.y,1,1);
      emit(0x39,e.motion.x,e.motion.y,1,1);
    }
    e.age = e.age + 1;
    growthEnabled8 = (u32)(double(0.0f) < double(0.5f) - (float)(int)e.age * double(0.05f));
    e.scale[0] =
         (double(0.5f) - (float)(int)e.age * double(0.05f)) * (float)growthEnabled8 + double(0.05f) +
         double(e.scale[0]);
    growthEnabled9 = (u32)(double(0.0f) < double(0.5f) - (float)(int)e.age * double(0.05f));
    e.scale[1] =
         (double(0.5f) - (float)(int)e.age * double(0.05f)) * (float)growthEnabled9 + double(0.05f) +
         double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x1a) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x19;
      }
      if (0x19 < e.red) {
        e.red = i8(e.red) + -0x19;
      }
      if (0x19 < e.green) {
        e.green = i8(e.green) + -0x19;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x39:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      randomValue = random.range(5,0x14);
      launchSpeed = (float)randomValue;
      randomValue = random.range(0,0x168);
      e.polar((float)randomValue,launchSpeed);
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      angleRadians = (double)std::atan2(double(e.motion.velocityY),double(e.motion.velocityX))
      ;
      e.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
      e.age = 0;
    }
    e.motion.x = double(e.motion.x) + double(e.motion.velocityX);
    e.motion.y = double(e.motion.y) - double(e.motion.velocityY);
    e.age = e.age + 1;
    if (0 < e.cursor.index) {
      e.scale[0] = double(e.scale[0]) + double(0.1f);
      e.scale[1] = double(e.scale[1]) * double(0.9f);
      if (e.alpha < 8) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -8;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x3c:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 0.25f;
      e.scale[1] = 0.25f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.rotation[0] = 75.0f;
      e.projection[2] = i8(e.motion.facing) * 0x15e;
      e.age = 0;
    }
    e.age = e.age + 1;
    e.scale[0] =
         (double(0.2f) - (float)(int)e.age * double(0.015f)) + double(e.scale[0]);
    e.scale[1] =
         (double(0.2f) - (float)(int)e.age * double(0.015f)) + double(e.scale[1]);
    if (-1 < e.cursor.index) {
      if (e.alpha < 0x1f) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x3e:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      shortBits = random.range(0,0x167);
      e.age = shortBits;
      e.parameters[0] = (float)(e.age + 0xb4);
      e.polar((float)(int)e.age,500.0f);
      emit(0x3f,double(e.motion.x) + double(e.motion.velocityX),
                   double(e.motion.y) + double(e.motion.velocityY),1,1);
    }
    e.age = e.age + 1;
    e.scale[0] = double(e.scale[0]) - double(0.03f);
    e.scale[1] = double(e.scale[1]) - double(0.03f);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x1a) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x19;
      }
      if (0x19 < e.red) {
        e.red = i8(e.red) + -0x19;
      }
      if (0x19 < e.green) {
        e.green = i8(e.green) + -0x19;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x3f:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
      e.polar(e.parameters[0],15.0f);
    }
    if (e.age == 0) {
      if (e.alpha < 0xd7) {
        e.alpha = i8(e.alpha) + '(';
      }
      else {
        e.alpha = 0xff;
        e.age = 1;
      }
    }
    if (e.age == 0) {
      if (e.alpha < 0xb) {
        e.alpha = 0;
        e.age = 2;
      }
      else {
        e.alpha = i8(e.alpha) + -10;
      }
    }
    e.rotation[2] = double(e.rotation[2]) + double(5.0f);
    e.motion.x = double(e.motion.x) + double(e.motion.velocityX);
    e.motion.y = double(e.motion.y) + double(e.motion.velocityY);
    e.scale[0] = double(e.scale[0]) - double(0.033f);
    e.scale[1] = double(e.scale[1]) - double(0.033f);
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x40:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      shortBits = random.range(0,0x167);
      e.age = shortBits;
      e.parameters[0] = (float)(e.age + 0xb4);
      e.polar((float)(int)e.age,500.0f);
      emit(0x41,double(e.motion.x) + double(e.motion.velocityX),
                   double(e.motion.y) + double(e.motion.velocityY),1,1);
    }
    e.age = e.age + 1;
    e.scale[0] = double(e.scale[0]) - double(0.03f);
    e.scale[1] = double(e.scale[1]) - double(0.03f);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x1a) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x19;
      }
      if (0x19 < e.red) {
        e.red = i8(e.red) + -0x19;
      }
      if (0x19 < e.green) {
        e.green = i8(e.green) + -0x19;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x41:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
      e.polar(e.parameters[0],15.0f);
    }
    if (e.age == 0) {
      if (e.alpha < 0xd7) {
        e.alpha = i8(e.alpha) + '(';
      }
      else {
        e.alpha = 0xff;
        e.age = 1;
      }
    }
    if (e.age == 0) {
      if (e.alpha < 0xb) {
        e.alpha = 0;
        e.age = 2;
      }
      else {
        e.alpha = i8(e.alpha) + -10;
      }
    }
    e.rotation[2] = double(e.rotation[2]) + double(5.0f);
    e.motion.x = double(e.motion.x) + double(e.motion.velocityX);
    e.motion.y = double(e.motion.y) + double(e.motion.velocityY);
    e.scale[0] = double(e.scale[0]) - double(0.033f);
    e.scale[1] = double(e.scale[1]) - double(0.033f);
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 100:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      randomValue = random.next();
      launchSpeed = (float)randomValue * 0.0005;
      randomValue = random.next();
      e.polar((float)randomValue,launchSpeed);
      e.begin_pose(0,0,0);
      e.scale[0] = 1.5f;
      e.scale[1] = 1.5f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
    }
    e.motion.x = double(e.motion.x) + double(e.motion.velocityX);
    e.motion.y = double(e.motion.y) + double(e.motion.velocityY);
    e.age = e.age + 1;
    e.scale[0] = double(e.scale[0]) * double(0.85f);
    e.scale[1] = double(e.scale[1]) * double(0.85f);
    if (e.alpha < 0xf) {
      e.alpha = 0;
    }
    else {
      e.alpha = i8(e.alpha) + -0xf;
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  }
}
[[gnu::noinline]] void advance_group_3(SharedEffect& e,ReplayRandom& random,SharedEffectEvents& events){
    i16 shortBits=0,shortValue=0;i32 randomValue=0;double angleRadians=0;float launchSpeed=0;
  switch(e.cursor.action) {
  case 0x65:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 0.699999988079071f;
      e.scale[1] = 0.699999988079071f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x168);
      e.rotation[2] = (float)randomValue;
      e.age = 0;
    }
    e.age = e.age + 1;
    e.scale[0] =
         (double(0.2f) - (float)(int)e.age * double(0.02f)) + double(e.scale[0]);
    e.scale[1] =
         (double(0.2f) - (float)(int)e.age * double(0.02f)) + double(e.scale[1]);
    if (e.alpha < 0x1f) {
      e.alpha = 0;
    }
    else {
      e.alpha = i8(e.alpha) + -0xf;
    }
    if (0x14 < e.red) {
      e.red = i8(e.red) + -0xf;
    }
    if (0x14 < e.green) {
      e.green = i8(e.green) + -0xf;
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x66:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x168);
      e.rotation[2] = (float)randomValue;
      shortValue = random.range(0,0x2ee);
      e.age = shortValue + 1000;
      shortValue = random.range(0,0x168);
      e.speedAngle = shortValue + -0xb4;
    }
    shortBits = (i16)i32(double(e.age)*double(0.95f));
    e.age = shortBits;
    e.polar((float)(int)e.speedAngle,
                 (float)(int)e.age * double(0.01f));
    e.motion.x = double(e.motion.x) + double(e.motion.velocityX);
    e.motion.y = double(e.motion.y) + double(e.motion.velocityY);
    e.rotation[2] = double(e.rotation[2]) + double(5.0f);
    if (5 < e.cursor.elapsed) {
      if (e.alpha < 0xb) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -10;
      }
      if (10 < e.red) {
        e.red = i8(e.red) + -10;
      }
      if (10 < e.green) {
        e.green = i8(e.green) + -10;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x67:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 0.699999988079071f;
      e.scale[1] = 0.699999988079071f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x168);
      e.rotation[2] = (float)randomValue;
      e.age = 0;
    }
    e.age = e.age + 1;
    e.scale[0] =
         (double(0.3f) - (float)(int)e.age * double(0.02f)) + double(e.scale[0]);
    e.scale[1] =
         (double(0.3f) - (float)(int)e.age * double(0.02f)) + double(e.scale[1]);
    if (e.alpha < 0x1f) {
      e.alpha = 0;
    }
    else {
      e.alpha = i8(e.alpha) + -0xf;
    }
    if (0x14 < e.red) {
      e.red = i8(e.red) + -0xf;
    }
    if (0x14 < e.green) {
      e.green = i8(e.green) + -0xf;
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x68:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      randomValue = random.range(5,0x14);
      launchSpeed = (float)randomValue;
      randomValue = random.range(0,0x168);
      e.polar((float)randomValue,launchSpeed);
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      angleRadians = (double)std::atan2(double(e.motion.velocityY),double(e.motion.velocityX))
      ;
      e.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
      e.age = 0;
    }
    e.motion.x = double(e.motion.x) + double(e.motion.velocityX);
    e.motion.y = double(e.motion.y) - double(e.motion.velocityY);
    e.age = e.age + 1;
    if (0 < e.cursor.index) {
      e.scale[1] = double(e.scale[1]) * double(0.85f);
      if (e.alpha < 0xf) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0xf;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x69:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      randomValue = random.range(5,0x14);
      launchSpeed = (float)randomValue;
      randomValue = random.range(0,0x168);
      e.polar((float)randomValue,launchSpeed);
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      angleRadians = (double)std::atan2(double(e.motion.velocityY),double(e.motion.velocityX))
      ;
      e.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
      e.age = 0;
    }
    e.motion.x = double(e.motion.x) + double(e.motion.velocityX);
    e.motion.y = double(e.motion.y) - double(e.motion.velocityY);
    e.age = e.age + 1;
    if (0 < e.cursor.index) {
      e.scale[1] = double(e.scale[1]) * double(0.85f);
      if (e.alpha < 0xf) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0xf;
      }
      if (0x14 < e.red) {
        e.blue = i8(e.blue) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x6a:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 0.25f;
      e.scale[1] = 0.25f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.rotation[1] = 65.0f;
      e.age = 0;
    }
    e.age = e.age + 1;
    e.scale[0] =
         (double(0.2f) - (float)(int)e.age * double(0.015f)) + double(e.scale[0]);
    e.scale[1] =
         (double(0.2f) - (float)(int)e.age * double(0.015f)) + double(e.scale[1]);
    if (-1 < e.cursor.index) {
      if (e.alpha < 0x1f) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x6b:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      shortBits = random.range(0x32,0x6e);
      e.lifetime = shortBits;
      e.begin_pose(0,0,0);
      e.scale[0] = (float)(int)e.lifetime * double(0.01f);
      e.scale[1] = (float)(int)e.lifetime * double(0.01f);
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      shortValue = random.range(0,0x2ee);
      e.age = shortValue + 1000;
      shortValue = random.range(0,0x168);
      e.speedAngle = shortValue + -0xb4;
    }
    shortBits = (i16)i32(double(e.age)*double(0.96f));
    e.age = shortBits;
    e.polar((float)(int)e.speedAngle,
                 (float)(int)e.age * double(0.01f));
    e.motion.x = double(e.motion.x) + double(e.motion.velocityX);
    e.motion.y = double(e.motion.y) + double(e.motion.velocityY);
    if (2 < e.cursor.elapsed) {
      e.scale[0] = double(e.scale[0]) * double(0.85f);
      e.scale[1] = double(e.scale[1]) * double(0.85f);
      if (e.alpha < 0xf) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0xf;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0xf;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0xf;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x6c:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 0.25f;
      e.scale[1] = 0.25f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.rotation[2] = 0;
      e.age = 0;
    }
    e.rotation[2] = double(e.rotation[2]) + double(5.0f);
    e.age = e.age + 1;
    e.scale[0] =
         (double(0.4f) - (float)(int)e.age * double(0.01f)) + double(e.scale[0]);
    e.scale[1] =
         (double(0.4f) - (float)(int)e.age * double(0.01f)) + double(e.scale[1]);
    if (0x1d < e.cursor.elapsed) {
      if (e.alpha < 0x1f) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  }
}
[[gnu::noinline]] void advance_group_4(SharedEffect& e,ReplayRandom& random,SharedEffectEvents& events){
    i16 shortBits=0,shortValue=0;i32 randomValue=0;double angleRadians=0;float launchSpeed=0;
    u32 growthEnabled10=0;
    u32 growthEnabled11=0;
    u32 growthEnabled12=0;
    const auto emit=[&](i16 id,float x,float y,i8 facing,u8 layer){events.spawn(id,x,y,facing,layer,e.parameters);};
  switch(e.cursor.action) {
  case 0x6d:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 0.25f;
      e.scale[1] = 0.25f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.rotation[2] = 45.0f;
      e.age = 0;
    }
    e.rotation[2] = double(e.rotation[2]) + double(5.0f);
    e.age = e.age + 1;
    e.scale[0] =
         (double(0.4f) - (float)(int)e.age * double(0.01f)) + double(e.scale[0]);
    e.scale[1] =
         (double(0.4f) - (float)(int)e.age * double(0.01f)) + double(e.scale[1]);
    if (0x1d < e.cursor.elapsed) {
      if (e.alpha < 0x1f) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x6e:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      shortBits = random.range(0x32,0x6e);
      e.lifetime = shortBits;
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      shortValue = random.range(0,500);
      e.age = shortValue + 0x5dc;
      shortBits = random.range(0,0x168);
      e.speedAngle = shortBits;
      e.rotation[2] = (float)(e.speedAngle + -0x5a);
    }
    shortBits = (i16)i32(double(e.age)*double(0.97f));
    e.age = shortBits;
    e.polar((float)(e.speedAngle + -0xb4),
                 (float)(int)e.age * double(0.01f));
    e.motion.x = double(e.motion.x) + double(e.motion.velocityX);
    e.motion.y = double(e.motion.y) + double(e.motion.velocityY);
    e.scale[1] = double(e.scale[1]) + double(0.05f);
    if (0xf < e.cursor.elapsed) {
      if (e.alpha < 10) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -10;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -5;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -5;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x6f:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      randomValue = random.range(0x14,100);
      e.scale[0] = (float)randomValue * double(0.01f);
      e.scale[1] = e.scale[0];
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(100,0xfa);
      launchSpeed = (float)randomValue * double(0.1f);
      randomValue = random.range(-155,-25);
      e.polar((float)randomValue,launchSpeed);
      randomValue = random.range(0,0x168);
      e.rotation[2] = (float)randomValue;
    }
    e.motion.velocityY = double(e.motion.velocityY) - double(0.75f);
    e.motion.x = double(e.motion.x) + double(e.motion.velocityX);
    e.motion.y = double(e.motion.y) + double(e.motion.velocityY);
    e.rotation[2] = double(e.rotation[2]) + double(5.0f);
    if (((double(e.motion.y) < double(40.0f)) && (double(e.motion.velocityY) < double(0.0f))) &&
       (randomValue = random.range(0,100), randomValue < 10)) {
      e.motion.velocityY = -double(e.motion.velocityY);
    }
    if (0 < e.cursor.index) {
      if (e.alpha < 10) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -10;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -5;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -5;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x70:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 0.5f;
      e.scale[1] = 0.5f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.rotation[0] = 65.0f;
      e.age = 0;
    }
    e.age = e.age + 1;
    growthEnabled10 = (u32)(double(0.0f) < double(0.5f) - (float)(int)e.age * double(0.04f));
    e.scale[0] =
         (double(0.5f) - (float)(int)e.age * double(0.04f)) * (float)growthEnabled10 + double(0.02f) +
         double(e.scale[0]);
    growthEnabled11 = (u32)(double(0.0f) < double(0.5f) - (float)(int)e.age * double(0.04f));
    e.scale[1] =
         (double(0.5f) - (float)(int)e.age * double(0.04f)) * (float)growthEnabled11 + double(0.02f) +
         double(e.scale[1]);
    if (9 < e.cursor.elapsed) {
      if (e.alpha < 0x1f) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x71:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 0.699999988079071f;
      e.scale[1] = 0.699999988079071f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x168);
      e.rotation[2] = (float)randomValue;
      e.age = 0;
    }
    e.age = e.age + 1;
    e.scale[0] =
         (double(0.3f) - (float)(int)e.age * double(0.02f)) + double(e.scale[0]);
    e.scale[1] =
         (double(0.3f) - (float)(int)e.age * double(0.02f)) + double(e.scale[1]);
    if (e.alpha < 0x1f) {
      e.alpha = 0;
    }
    else {
      e.alpha = i8(e.alpha) + -0xf;
    }
    if (0x14 < e.red) {
      e.red = i8(e.red) + -0xf;
    }
    if (0x14 < e.green) {
      e.green = i8(e.green) + -0xf;
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x72:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      randomValue = random.range(0x32,0x78);
      e.scale[0] = (float)randomValue * double(1.0f) * double(0.01f);
      e.scale[1] = e.scale[0];
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      shortValue = random.range(0,1000);
      e.age = shortValue + 4000;
      shortBits = random.range(0,0x168);
      e.speedAngle = shortBits;
    }
    shortBits = (i16)i32(double(e.age)*double(0.9f));
    e.age = shortBits;
    e.polar((float)(e.speedAngle + -0xb4),
                 (float)(int)e.age * double(0.01f));
    e.motion.x = double(e.motion.x) + double(e.motion.velocityX);
    e.motion.y = double(e.motion.y) + double(e.motion.velocityY);
    if (0x14 < e.cursor.elapsed) {
      e.scale[0] = double(e.scale[0]) * double(0.9f);
      e.scale[1] = double(e.scale[1]) * double(0.9f);
      if (e.alpha < 0x14) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x73:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 2.0f;
      e.scale[1] = 2.0f;
      e.alpha = 0;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
      for (growthEnabled12 = 0; growthEnabled12 < 0x19; growthEnabled12 = growthEnabled12 + 1) {
        emit(0x7a,e.motion.x,e.motion.y,1,1);
      }
      emit(0x8d,e.motion.x,e.motion.y,1,1);
      emit(0x8e,e.motion.x,e.motion.y,1,1);
      emit(0x8f,e.motion.x,e.motion.y,1,1);
      emit(0x90,e.motion.x,e.motion.y,1,1);
      emit(0x91,e.motion.x,e.motion.y,1,1);
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x74:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 2.0f;
      e.scale[1] = 2.0f;
      e.alpha = 0;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
      emit(0x83,e.motion.x,e.motion.y,1,1);
      emit(0x84,e.motion.x,e.motion.y,1,1);
      emit(0x85,e.motion.x,e.motion.y,1,1);
      emit(0x86,e.motion.x,e.motion.y,1,1);
      emit(0x86,e.motion.x,e.motion.y,1,1);
      emit(0x86,e.motion.x,e.motion.y,1,1);
      emit(0x86,e.motion.x,e.motion.y,1,1);
      emit(0x86,e.motion.x,e.motion.y,1,1);
      emit(0x86,e.motion.x,e.motion.y,1,1);
      emit(0x86,e.motion.x,e.motion.y,1,1);
      emit(0x86,e.motion.x,e.motion.y,1,1);
      emit(0x86,e.motion.x,e.motion.y,1,1);
      emit(0x86,e.motion.x,e.motion.y,1,1);
      emit(0x86,e.motion.x,e.motion.y,1,1);
      emit(0x86,e.motion.x,e.motion.y,1,1);
      emit(0x87,e.motion.x,e.motion.y,1,1);
      emit(0x87,e.motion.x,e.motion.y,1,1);
      emit(0x88,e.motion.x,e.motion.y,1,1);
      emit(0x88,e.motion.x,e.motion.y,1,1);
      emit(0x88,e.motion.x,e.motion.y,1,1);
      emit(0x88,e.motion.x,e.motion.y,1,1);
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  }
}
[[gnu::noinline]] void advance_group_5(SharedEffect& e,ReplayRandom& random,SharedEffectEvents& events){
    i16 shortBits=0,shortValue=0;i32 randomValue=0;double angleRadians=0;float launchSpeed=0;
    u32 growthEnabled13=0;
    u32 growthEnabled14=0;
    u32 growthEnabled15=0;
    u32 growthEnabled16=0;
    u32 growthEnabled17=0;
    u32 growthEnabled18=0;
  switch(e.cursor.action) {
  case 0x75:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.2000000476837158f;
      e.scale[1] = 1.2000000476837158f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.rotation[0] = -80.0f;
      e.age = 0;
    }
    e.projection[2] = 0x5dc;
    e.projection[0] = 0;
    e.rotation[2] = double(e.rotation[2]) + double(3.0f);
    e.scale[0] = double(e.scale[0]) + double(0.02f);
    e.scale[1] = double(e.scale[1]) + double(0.02f);
    if (0 < e.cursor.index) {
      e.alpha = i8(e.alpha) + -6;
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x76:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.399999976158142f;
      e.scale[1] = 1.399999976158142f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.rotation[0] = -80.0f;
      e.age = 0;
    }
    e.projection[2] = 0x5dc;
    e.projection[0] = 0;
    e.rotation[2] = double(e.rotation[2]) + double(3.0f);
    e.scale[0] = double(e.scale[0]) + double(0.02f);
    e.scale[1] = double(e.scale[1]) + double(0.02f);
    if (0 < e.cursor.index) {
      e.alpha = i8(e.alpha) + -6;
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x77:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
    }
    e.age = e.age + 1;
    growthEnabled13 = (u32)(double(0.0f) < double(0.1f) - (float)(int)e.age * double(0.01f));
    e.scale[0] =
         (double(0.1f) - (float)(int)e.age * double(0.01f)) * (float)growthEnabled13 + double(0.01f) +
         double(e.scale[0]);
    growthEnabled14 = (u32)(double(0.0f) < double(0.1f) - (float)(int)e.age * double(0.01f));
    e.scale[1] =
         (double(0.1f) - (float)(int)e.age * double(0.01f)) * (float)growthEnabled14 + double(0.01f) +
         double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x14) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x78:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
    }
    e.age = e.age + 1;
    growthEnabled15 = (u32)(double(0.0f) < double(0.3f) - (float)(int)e.age * double(0.03f));
    e.scale[0] =
         (double(0.3f) - (float)(int)e.age * double(0.03f)) * (float)growthEnabled15 + double(0.03f) +
         double(e.scale[0]);
    growthEnabled16 = (u32)(double(0.0f) < double(0.3f) - (float)(int)e.age * double(0.03f));
    e.scale[1] =
         (double(0.3f) - (float)(int)e.age * double(0.03f)) * (float)growthEnabled16 + double(0.03f) +
         double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x14) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x79:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
    }
    e.age = e.age + 1;
    growthEnabled17 = (u32)(double(0.0f) < double(0.5f) - (float)(int)e.age * double(0.05f));
    e.scale[0] =
         (double(0.5f) - (float)(int)e.age * double(0.05f)) * (float)growthEnabled17 + double(0.05f) +
         double(e.scale[0]);
    growthEnabled18 = (u32)(double(0.0f) < double(0.5f) - (float)(int)e.age * double(0.05f));
    e.scale[1] =
         (double(0.5f) - (float)(int)e.age * double(0.05f)) * (float)growthEnabled18 + double(0.05f) +
         double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x14) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x7a:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      randomValue = random.range(5,0x14);
      launchSpeed = (float)randomValue;
      randomValue = random.range(0,0x168);
      e.polar((float)randomValue,launchSpeed);
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      angleRadians = (double)std::atan2(double(e.motion.velocityY),double(e.motion.velocityX))
      ;
      e.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
      e.age = 0;
    }
    e.motion.x = double(e.motion.x) + double(e.motion.velocityX);
    e.motion.y = double(e.motion.y) - double(e.motion.velocityY);
    e.age = e.age + 1;
    if (0 < e.cursor.index) {
      e.scale[0] = double(e.scale[0]) + double(0.3f);
      e.scale[1] = double(e.scale[1]) * double(0.9f);
      if (e.alpha < 8) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -8;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x7b:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 3.5f;
      e.scale[1] = 10.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x168);
      e.rotation[2] = (float)randomValue;
      e.age = 0;
    }
    e.rotation[2] = double(e.rotation[2]) + double(1.0f);
    e.scale[0] = double(e.scale[0]) + double(0.01f);
    e.scale[1] = double(e.scale[1]) * double(0.9f);
    if (1 < e.age) {
      if (e.alpha < 0xf) {
        e.alpha = 0;
        e.destroy();
        return;
      }
      e.alpha = i8(e.alpha) + -0xf;
      if (10 < e.red) {
        e.red = i8(e.red) + -10;
      }
      if (10 < e.green) {
        e.green = i8(e.green) + -10;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.frame(1);
      e.age = e.age + 1;
    }
    break;
  case 0x7c:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 3.5f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x168);
      e.rotation[2] = (float)randomValue;
      e.age = 0;
    }
    e.rotation[2] = double(e.rotation[2]) - double(1.0f);
    e.scale[0] = double(e.scale[0]) + double(0.01f);
    e.scale[1] = double(e.scale[1]) * double(0.95f);
    if (1 < e.age) {
      if (e.alpha < 0x14) {
        e.alpha = 0;
        e.destroy();
        return;
      }
      e.alpha = i8(e.alpha) + -0x14;
      if (10 < e.red) {
        e.red = i8(e.red) + -10;
      }
      if (10 < e.green) {
        e.green = i8(e.green) + -10;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.frame(1);
      e.age = e.age + 1;
    }
    break;
  }
}
[[gnu::noinline]] void advance_group_6(SharedEffect& e,ReplayRandom& random,SharedEffectEvents& events){
    i16 shortBits=0,shortValue=0;i32 randomValue=0;double angleRadians=0;float launchSpeed=0;
    u32 growthEnabled19=0;
    u32 growthEnabled20=0;
    u32 growthEnabled21=0;
    u32 growthEnabled22=0;
  switch(e.cursor.action) {
  case 0x7d:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 0.25f;
      e.scale[1] = 0.25f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.rotation[1] = 65.0f;
      e.projection[2] = i8(e.motion.facing) * 0x15e;
      e.age = 0;
    }
    e.age = e.age + 1;
    e.scale[0] =
         (double(0.2f) - (float)(int)e.age * double(0.015f)) + double(e.scale[0]);
    e.scale[1] =
         (double(0.2f) - (float)(int)e.age * double(0.015f)) + double(e.scale[1]);
    if (-1 < e.cursor.index) {
      if (e.alpha < 0x1f) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x7e:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x7f:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x80:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x81:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x82:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.5f;
      e.scale[1] = 1.5f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x83:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
    }
    e.age = e.age + 1;
    growthEnabled19 = (u32)(double(0.0f) < double(0.1f) - (float)(int)e.age * double(0.01f));
    e.scale[0] =
         (double(0.1f) - (float)(int)e.age * double(0.01f)) * (float)growthEnabled19 + double(0.01f) +
         double(e.scale[0]);
    growthEnabled20 = (u32)(double(0.0f) < double(0.1f) - (float)(int)e.age * double(0.01f));
    e.scale[1] =
         (double(0.1f) - (float)(int)e.age * double(0.01f)) * (float)growthEnabled20 + double(0.01f) +
         double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x14) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x84:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
    }
    e.age = e.age + 1;
    growthEnabled21 = (u32)(double(0.0f) < double(0.3f) - (float)(int)e.age * double(0.03f));
    e.scale[0] =
         (double(0.3f) - (float)(int)e.age * double(0.03f)) * (float)growthEnabled21 + double(0.03f) +
         double(e.scale[0]);
    growthEnabled22 = (u32)(double(0.0f) < double(0.3f) - (float)(int)e.age * double(0.03f));
    e.scale[1] =
         (double(0.3f) - (float)(int)e.age * double(0.03f)) * (float)growthEnabled22 + double(0.03f) +
         double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x14) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  }
}
[[gnu::noinline]] void advance_group_7(SharedEffect& e,ReplayRandom& random,SharedEffectEvents& events){
    i16 shortBits=0,shortValue=0;i32 randomValue=0;double angleRadians=0;float launchSpeed=0;
    u32 growthEnabled23=0;
    u32 growthEnabled24=0;
    u32 growthEnabled25=0;
    u32 growthEnabled26=0;
  switch(e.cursor.action) {
  case 0x85:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
    }
    e.age = e.age + 1;
    growthEnabled23 = (u32)(double(0.0f) < double(0.5f) - (float)(int)e.age * double(0.05f));
    e.scale[0] =
         (double(0.5f) - (float)(int)e.age * double(0.05f)) * (float)growthEnabled23 + double(0.05f) +
         double(e.scale[0]);
    growthEnabled24 = (u32)(double(0.0f) < double(0.5f) - (float)(int)e.age * double(0.05f));
    e.scale[1] =
         (double(0.5f) - (float)(int)e.age * double(0.05f)) * (float)growthEnabled24 + double(0.05f) +
         double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x14) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x86:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      randomValue = random.range(5,0x14);
      launchSpeed = (float)randomValue;
      randomValue = random.range(0,0x168);
      e.polar((float)randomValue,launchSpeed);
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      angleRadians = (double)std::atan2(double(e.motion.velocityY),double(e.motion.velocityX))
      ;
      e.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
      e.age = 0;
    }
    e.motion.x = double(e.motion.x) + double(e.motion.velocityX);
    e.motion.y = double(e.motion.y) - double(e.motion.velocityY);
    e.age = e.age + 1;
    if (0 < e.cursor.index) {
      e.scale[0] = double(e.scale[0]) + double(0.1f);
      e.scale[1] = double(e.scale[1]) * double(0.9f);
      if (e.alpha < 8) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -8;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x87:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 3.5f;
      e.scale[1] = 10.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x168);
      e.rotation[2] = (float)randomValue;
      e.age = 0;
    }
    e.rotation[2] = double(e.rotation[2]) + double(1.0f);
    e.scale[0] = double(e.scale[0]) + double(0.01f);
    e.scale[1] = double(e.scale[1]) * double(0.9f);
    if (1 < e.age) {
      if (e.alpha < 0xf) {
        e.alpha = 0;
        e.destroy();
        return;
      }
      e.alpha = i8(e.alpha) + -0xf;
      if (10 < e.red) {
        e.red = i8(e.red) + -10;
      }
      if (10 < e.green) {
        e.green = i8(e.green) + -10;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.frame(1);
      e.age = e.age + 1;
    }
    break;
  case 0x88:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 3.5f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x168);
      e.rotation[2] = (float)randomValue;
      e.age = 0;
    }
    e.rotation[2] = double(e.rotation[2]) - double(1.0f);
    e.scale[0] = double(e.scale[0]) + double(0.01f);
    e.scale[1] = double(e.scale[1]) * double(0.95f);
    if (1 < e.age) {
      if (e.alpha < 0x14) {
        e.alpha = 0;
        e.destroy();
        return;
      }
      e.alpha = i8(e.alpha) + -0x14;
      if (10 < e.red) {
        e.red = i8(e.red) + -10;
      }
      if (10 < e.green) {
        e.green = i8(e.green) + -10;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.frame(1);
      e.age = e.age + 1;
    }
    break;
  case 0x89:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x168);
      e.rotation[2] = (float)randomValue;
    }
    e.scale[0] = double(e.scale[0]) + double(0.05f);
    e.scale[1] = double(e.scale[1]) + double(0.05f);
    if (e.alpha < 0x10) {
      e.destroy();
    }
    else {
      e.alpha = i8(e.alpha) + -0xf;
      shortValue = e.advance();
      if (shortValue == e.cursor.frameCount) {
        e.destroy();
      }
    }
    break;
  case 0x8a:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.rotation[1] = -40.0f;
      e.projection[2] = i8(e.motion.facing) * 0x15e;
      e.age = 0;
    }
    e.age = e.age + 1;
    e.scale[0] =
         (double(0.4f) - (float)(int)e.age * double(0.015f)) + double(e.scale[0]);
    e.scale[1] =
         (double(0.4f) - (float)(int)e.age * double(0.015f)) + double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x1f) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x1e;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x8b:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 2.0f;
      e.scale[1] = 2.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.age = 0;
    }
    e.age = e.age + 1;
    growthEnabled25 = (u32)(double(0.0f) < double(0.1f) - (float)(int)e.age * double(0.01f));
    e.scale[0] =
         (double(0.1f) - (float)(int)e.age * double(0.01f)) * (float)growthEnabled25 + double(0.01f) +
         double(e.scale[0]);
    growthEnabled26 = (u32)(double(0.0f) < double(0.1f) - (float)(int)e.age * double(0.01f));
    e.scale[1] =
         (double(0.1f) - (float)(int)e.age * double(0.01f)) * (float)growthEnabled26 + double(0.01f) +
         double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x14) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  case 0x8c:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      e.rotation[1] = (float)(i8(e.motion.facing) * -0x3c);
      randomValue = random.range(0,0xb4);
      e.rotation[0] = (float)randomValue;
      e.age = 0;
    }
    e.age = e.age + 1;
    e.rotation[2] = double(e.rotation[2]) + double(5.0f);
    e.scale[0] =
         (double(0.6f) - (float)(int)e.age * double(0.02f)) + double(e.scale[0]);
    e.scale[1] =
         (double(0.6f) - (float)(int)e.age * double(0.02f)) + double(e.scale[1]);
    if (0 < e.cursor.index) {
      if (e.alpha < 0x15) {
        e.alpha = 0;
      }
      else {
        e.alpha = i8(e.alpha) + -0x14;
      }
      if (0x14 < e.red) {
        e.red = i8(e.red) + -0x14;
      }
      if (0x14 < e.green) {
        e.green = i8(e.green) + -0x14;
      }
    }
    shortValue = e.advance();
    if (shortValue == e.cursor.frameCount) {
      e.destroy();
    }
    break;
  }
}
[[gnu::noinline]] void advance_group_8(SharedEffect& e,ReplayRandom& random,SharedEffectEvents& events){
    i16 shortBits=0,shortValue=0;i32 randomValue=0;double angleRadians=0;float launchSpeed=0;
  switch(e.cursor.action) {
  case 0x8d:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x168);
      e.rotation[2] = (float)randomValue;
    }
    e.scale[0] = double(e.scale[0]) + double(0.45f);
    e.scale[1] = double(e.scale[1]) + double(0.45f);
    if (e.alpha < 0xb) {
      e.alpha = 0;
      e.destroy();
    }
    else {
      e.alpha = i8(e.alpha) + -10;
      if (10 < e.red) {
        e.red = i8(e.red) + -10;
      }
      if (10 < e.green) {
        e.green = i8(e.green) + -10;
      }
      shortValue = e.advance();
      if (shortValue == e.cursor.frameCount) {
        e.destroy();
      }
    }
    break;
  case 0x8e:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x168);
      e.rotation[2] = (float)randomValue;
    }
    e.scale[0] = double(e.scale[0]) + double(0.25f);
    e.scale[1] = double(e.scale[1]) + double(0.25f);
    if (e.alpha < 0xb) {
      e.alpha = 0;
      e.destroy();
    }
    else {
      e.alpha = i8(e.alpha) + -10;
      if (10 < e.red) {
        e.red = i8(e.red) + -10;
      }
      if (10 < e.green) {
        e.green = i8(e.green) + -10;
      }
      shortValue = e.advance();
      if (shortValue == e.cursor.frameCount) {
        e.destroy();
      }
    }
    break;
  case 0x8f:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x168);
      e.rotation[2] = (float)randomValue;
      randomValue = random.range(0,0x3c);
      e.rotation[0] = (float)(randomValue + -0x1e);
      randomValue = random.range(0,0x3c);
      e.rotation[1] = (float)(randomValue + -0x1e);
    }
    e.rotation[2] = double(e.rotation[2]) + double(15.0f);
    e.rotation[0] = double(e.rotation[0]) + double(4.0f);
    e.rotation[1] = double(e.rotation[1]) + double(4.0f);
    e.scale[0] = double(e.scale[0]) + double(0.14f);
    e.scale[1] = double(e.scale[1]) + double(0.14f);
    if (e.alpha < 0xb) {
      e.alpha = 0;
      e.destroy();
    }
    else {
      e.alpha = i8(e.alpha) + -10;
      if (10 < e.red) {
        e.red = i8(e.red) + -10;
      }
      if (10 < e.green) {
        e.green = i8(e.green) + -10;
      }
      shortValue = e.advance();
      if (shortValue == e.cursor.frameCount) {
        e.destroy();
      }
    }
    break;
  case 0x90:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x168);
      e.rotation[2] = (float)randomValue;
      randomValue = random.range(0,0x3c);
      e.rotation[0] = (float)(randomValue + -0x1e);
      randomValue = random.range(0,0x3c);
      e.rotation[1] = (float)(randomValue + -0x1e);
    }
    e.rotation[2] = double(e.rotation[2]) - double(12.0f);
    e.scale[0] = double(e.scale[0]) + double(0.12f);
    e.scale[1] = double(e.scale[1]) + double(0.12f);
    e.rotation[0] = double(e.rotation[0]) - double(4.0f);
    e.rotation[1] = double(e.rotation[1]) - double(4.0f);
    if (e.alpha < 0xb) {
      e.alpha = 0;
      e.destroy();
    }
    else {
      e.alpha = i8(e.alpha) + -10;
      if (10 < e.red) {
        e.red = i8(e.red) + -10;
      }
      if (10 < e.green) {
        e.green = i8(e.green) + -10;
      }
      shortValue = e.advance();
      if (shortValue == e.cursor.frameCount) {
        e.destroy();
      }
    }
    break;
  case 0x91:
    if ((e.cursor.index == 0) && (e.cursor.elapsed == 0)) {
      e.begin_pose(0,0,0);
      e.scale[0] = 1.0f;
      e.scale[1] = 1.0f;
      e.alpha = 0xff;
      e.red = 0xff;
      e.green = 0xff;
      e.blue = 0xff;
      randomValue = random.range(0,0x168);
      e.rotation[2] = (float)randomValue;
      randomValue = random.range(0,0x3c);
      e.rotation[0] = (float)(randomValue + -0x1e);
      randomValue = random.range(0,0x3c);
      e.rotation[1] = (float)(randomValue + -0x1e);
    }
    e.rotation[2] = double(e.rotation[2]) - double(10.0f);
    e.scale[0] = double(e.scale[0]) + double(0.1f);
    e.scale[1] = double(e.scale[1]) + double(0.1f);
    if (e.alpha < 0xb) {
      e.alpha = 0;
      e.destroy();
    }
    else {
      e.alpha = i8(e.alpha) + -10;
      if (10 < e.red) {
        e.red = i8(e.red) + -10;
      }
      if (10 < e.green) {
        e.green = i8(e.green) + -10;
      }
      shortValue = e.advance();
      if (shortValue == e.cursor.frameCount) {
        e.destroy();
      }
    }
  }
}
}
void advance_shared_effect(SharedEffect& e,ReplayRandom& random,SharedEffectEvents& events){
    using Policy=void(*)(SharedEffect&,ReplayRandom&,SharedEffectEvents&);
    // Binaryen otherwise inlines all single-caller groups despite noinline.
    // Keep the dispatch indirect so child updates use the smaller activations.
    static Policy volatile policies[]{advance_group_0,advance_group_1,advance_group_2,
        advance_group_3,advance_group_4,advance_group_5,advance_group_6,advance_group_7,advance_group_8};
    const auto action=e.cursor.action;
    if(action<=0xd){policies[0](e,random,events);return;}
    if(action<=0x37){policies[1](e,random,events);return;}
    if(action<=100){policies[2](e,random,events);return;}
    if(action<=0x6c){policies[3](e,random,events);return;}
    if(action<=0x74){policies[4](e,random,events);return;}
    if(action<=0x7c){policies[5](e,random,events);return;}
    if(action<=0x84){policies[6](e,random,events);return;}
    if(action<=0x8c){policies[7](e,random,events);return;}
    if(action<=0x91)policies[8](e,random,events);
}
}
