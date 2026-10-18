#pragma once

#include "ZunColor.hpp"
#include "ZunMath.hpp"
#include "ZunResult.hpp"
#include "ZunTimer.hpp"
#include "inttypes.hpp"
#include <cstring>

struct AnmLoadedSprite
{
    i32 sourceFileIndex;
    ZunVec2 startPixelInclusive;
    ZunVec2 endPixelInclusive;
    f32 textureHeight;
    f32 textureWidth;
    ZunVec2 uvStart;
    ZunVec2 uvEnd;
    // Optional UVs into AnmManager's runtime sprite atlas. The atlas keeps a
    // one-texel extruded gutter around every static ANM sprite so GL_LINEAR
    // can sample subpixel positions without leaking neighbouring atlas cells.
    // The original UVs stay untouched for UV-scroll and other compatibility
    // paths which intentionally address the source texture outside the cell.
    ZunVec2 extrudedUvStart;
    ZunVec2 extrudedUvEnd;
    bool hasExtrudedUv;
    f32 heightPx;
    f32 widthPx;
    i32 spriteId;
};

#define AnmOpcode_Exit 0
#define AnmOpcode_SetActiveSprite 1
#define AnmOpcode_SetScale 2
#define AnmOpcode_SetAlpha 3
#define AnmOpcode_SetColor 4
#define AnmOpcode_Jump 5
#define AnmOpcode_Nop 6
#define AnmOpcode_FlipX 7
#define AnmOpcode_FlipY 8
#define AnmOpcode_SetRotation 9
#define AnmOpcode_SetAngleVel 10
#define AnmOpcode_SetScaleSpeed 11
#define AnmOpcode_Fade 12
#define AnmOpcode_SetBlendAdditive 13
#define AnmOpcode_SetBlendDefault 14
#define AnmOpcode_ExitHide 15
#define AnmOpcode_SetRandomSprite 16
#define AnmOpcode_SetPosition 17
#define AnmOpcode_PosTimeLinear 18
#define AnmOpcode_PosTimeDecel 19
#define AnmOpcode_PosTimeAccel 20
#define AnmOpcode_Stop 21
#define AnmOpcode_InterruptLabel 22
#define AnmOpcode_AnchorTopLeft 23
#define AnmOpcode_StopHide 24
#define AnmOpcode_UsePosOffset 25
#define AnmOpcode_SetAutoRotate 26
#define AnmOpcode_UVScrollX 27
#define AnmOpcode_UVScrollY 28
#define AnmOpcode_SetVisibility 29
#define AnmOpcode_ScaleTime 30
#define AnmOpcode_SetZWriteDisable 31

struct AnmRawInstr
{
    LE<i16> time;
    u8 opcode;
    u8 argsCount;
    u32 args[10];
};

enum AnmVmFlagsEnum
{
    AnmVmFlags_Visible = 1 << 0,
    AnmVmFlags_1 = 1 << 1,
    AnmVmFlags_BlendMode = 1 << 2,
    AnmVmFlags_ColorOp = 1 << 3,
    AnmVmFlags_4 = 1 << 4,
    AnmVmFlags_UsePosOffset = 1 << 5,
    AnmVmFlags_FlipX = 1 << 6,
    AnmVmFlags_FlipY = 1 << 7,
    AnmVmFlags_AnchorLeft = 1 << 8,
    AnmVmFlags_AnchorTop = 1 << 9,
    /* posTime missing because it is not really a flag */
    AnmVmFlags_ZWriteDisable = 1 << 12,
    AnmVmFlags_IsStopped = 1 << 13,
};

#define ANM_VM_INITIAL_FLAGS 0x3

enum AnmVmBlendMode
{
    AnmVmBlendMode_InvSrcAlpha,
    AnmVmBlendMode_One,
};

enum AnmVmColorOp
{
    AnmVmColorOp_Modulate,
    AnmVmColorOp_Add,
};

enum AnmVmAnchor
{
    AnmVmAnchor_Center,
    AnmVmAnchor_Left,
    AnmVmAnchor_Top,
    AnmVmAnchor_TopLeft,
};

struct AnmVmFlags
{
    u32 isVisible : 1;
    u32 flag1 : 1;
    u32 blendMode : 1;
    u32 colorOp : 1;
    u32 flag4 : 1;
    u32 usePosOffset : 1;
    u32 flip : 2;
    u32 anchor : 2;
    u32 posTime : 2;
    u32 zWriteDisable : 1;
    u32 isStopped : 1;
};

struct AnmVm
{
    void Initialize()
    {
        this->uvScrollPos.y = 0.0;
        this->uvScrollPos.x = 0.0;
        this->scaleInterpFinalX = 0.0;
        this->scaleInterpFinalY = 0.0;
        this->angleVel.z = 0.0;
        this->angleVel.y = 0.0;
        this->angleVel.x = 0.0;
        this->rotation.z = 0.0;
        this->rotation.y = 0.0;
        this->rotation.x = 0.0;
        this->scaleX = 1.0;
        this->scaleY = 1.0;
        this->scaleInterpEndTime = 0;
        this->alphaInterpEndTime = 0;
        this->color = COLOR_WHITE;
        // Match TH07 reallyportable's invariant: every property that can be
        // interpolated must start with identical previous/current endpoints.
        // Position is owned by the caller and is synchronized after scripts
        // are assigned, so do not manufacture a position here.
        this->prevRotation = this->rotation;
        this->prevScaleY = this->scaleY;
        this->prevScaleX = this->scaleX;
        this->prevUvScrollPos = this->uvScrollPos;
        this->prevColor = this->color;
        this->matrix.Identity();
        std::memset(&this->flags, 0, sizeof(this->flags));
        this->flags.isVisible = 1;
        this->flags.flag1 = 1;
        this->autoRotate = 0;
        this->pendingInterrupt = 0;
        this->posInterpEndTime = 0;
        this->currentTimeInScript.Initialize();
    }

    void UpdatePrev()
    {
        this->prevRotation = this->rotation;
        this->prevScaleY = this->scaleY;
        this->prevScaleX = this->scaleX;
        this->prevUvScrollPos = this->uvScrollPos;
        this->prevColor = this->color;
        this->prevPos = this->pos;
    }

    AnmVm()
    {
        this->activeSpriteIndex = -1;
    }

    void SetInvisible()
    {
        this->flags.isVisible = 0;
    }

    ZunVec3 rotation;
    ZunVec3 prevRotation;
    ZunVec3 angleVel;
    f32 scaleY;
    f32 scaleX;
    f32 prevScaleY;
    f32 prevScaleX;
    f32 scaleInterpFinalY;
    f32 scaleInterpFinalX;
    ZunVec2 uvScrollPos;
    ZunVec2 prevUvScrollPos;
    ZunTimer currentTimeInScript;
    ZunMatrix matrix;
    ZunColor color;
    ZunColor prevColor;
    AnmVmFlags flags;

    i16 alphaInterpEndTime;
    i16 scaleInterpEndTime;
    i16 autoRotate;
    i16 pendingInterrupt;
    i16 posInterpEndTime;
    // Two padding bytes
    ZunVec3 pos;
    ZunVec3 prevPos;
    f32 scaleInterpInitialY;
    f32 scaleInterpInitialX;
    ZunTimer scaleInterpTime;
    i16 activeSpriteIndex;
    i16 baseSpriteIndex;
    i16 anmFileIndex;
    // Two padding bytes
    const AnmRawInstr *beginingOfScript;
    const AnmRawInstr *currentInstruction;
    const AnmLoadedSprite *sprite;
    ZunColor alphaInterpInitial;
    ZunColor alphaInterpFinal;
    ZunVec3 posInterpInitial;
    ZunVec3 posInterpFinal;
    ZunVec3 posOffset;
    ZunTimer posInterpTime;
    i32 timeOfLastSpriteSet;
    ZunTimer alphaInterpTime;
    u8 fontWidth;
    u8 fontHeight;
    // Two final padding bytes
};
