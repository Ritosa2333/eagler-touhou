#pragma once
#include "AnmLayout.hpp"
namespace th08 {
class EffectSystem;struct EffectState;struct SpriteVertex;
using EffectUpdate=i32(*)(EffectState&,EffectSystem&);
using EffectDraw=void(*)(EffectState&,EffectSystem&);
// TH08 1.00d effect object. The animation script and spatial motion have
// separate positions; the renderer combines them at the selected layer.
struct EffectState:AnmVm {
    Vec3 position,parameters,velocity,acceleration,world_position,center,direction,up;
    float quaternion[4],radius,angle;u32 reserved31c;float width;
    i32 segments,slot;float height,angle_y,frequency;Timer age;u32 reserved344;
    EffectUpdate update;EffectDraw draw;
    u8 active,kind,dying;i8 fade_frames;u8 layer,alternative,geometry_dirty,ignore_pause;
    SpriteVertex* vertices;EffectState* next;
};
static_assert(sizeof(EffectState)==0x360);
static_assert(offsetof(EffectState,position)==0x2a4&&offsetof(EffectState,age)==0x338);
static_assert(offsetof(EffectState,update)==0x348&&offsetof(EffectState,vertices)==0x358);
}
