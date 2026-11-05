// TH09 shares the ANM v3 record layout. Initial names adapted from the MIT
// TH08 reference; behavior is checked separately against TH09 1.50a.
// See cpp/licenses/th08-reference-MIT.txt. Pointers below are native C++ objects.
#pragma once
#include "Timer.hpp"
#include "Rng.hpp"
namespace th09 {
struct AnmLoaded;
struct Matrix4 {float m[4][4];void identity() noexcept {std::memset(m,0,sizeof(m));for(u32 i=0;i<4;++i)m[i][i]=1;}};
union ZunColor {i32 d3dColor;struct {u8 b,g,r,a;};};
constexpr i32 COLOR_WHITE=-1;
enum AnmVariable
{
    AnmVariable_I0 = 10000,
    AnmVariable_I1,
    AnmVariable_I2,
    AnmVariable_I3,
    AnmVariable_F0,
    AnmVariable_F1,
    AnmVariable_F2,
    AnmVariable_F3,
    AnmVariable_IC0,
    AnmVariable_IC1,
};

enum AnmInterp
{
    AnmInterp_Pos,
    AnmInterp_RGB1,
    AnmInterp_Alpha1,
    AnmInterp_Rotate,
    AnmInterp_Scale,
    AnmInterp_RGB2,
    AnmInterp_Alpha2,
    AnmInterp_Last
};

enum AnmInterpMode
{
    AnmInterpMode_Linear = 0,
    AnmInterpMode_EaseIn = 1,
    AnmInterpMode_EaseInCubic = 2,
    AnmInterpMode_EaseInQuartic = 3,
    AnmInterpMode_EaseOut = 4,
    AnmInterpMode_EaseOutCubic = 5,
    AnmInterpMode_EaseOutQuartic = 6
};

enum AnmOpcode
{
    AnmOpcode_EndOfScript = -1,
    AnmOpcode_Nop = 0,
    AnmOpcode_Delete = 1,
    AnmOpcode_Static = 2,
    AnmOpcode_Sprite = 3,
    AnmOpcode_Jmp = 4,
    AnmOpcode_JmpDec = 5,
    AnmOpcode_Pos = 6,
    AnmOpcode_Scale = 7,
    AnmOpcode_Alpha = 8,
    AnmOpcode_Color = 9,
    AnmOpcode_FlipX = 10,
    AnmOpcode_FlipY = 11,
    AnmOpcode_Rotate = 12,
    AnmOpcode_AngularVelocity = 13,
    AnmOpcode_ScaleGrowth = 14,
    AnmOpcode_AlphaTimeLinear = 15,
    AnmOpcode_AdditiveBlendMode = 16,
    AnmOpcode_PosTimeLinear = 17,
    AnmOpcode_PosTimeDecel = 18,
    AnmOpcode_PosTimeDecel2 = 19,
    AnmOpcode_Stop = 20,
    AnmOpcode_InterruptLabel = 21,
    AnmOpcode_AnchorTopLeft = 22,
    AnmOpcode_StopHide = 23,
    AnmOpcode_PosMode = 24,
    AnmOpcode_Ins25 = 25,
    AnmOpcode_AddU = 26,
    AnmOpcode_AddV = 27,
    AnmOpcode_Visible = 28,
    AnmOpcode_ScaleTimeLinear = 29,
    AnmOpcode_ZWriteDisable = 30,
    AnmOpcode_Ins31 = 31,
    AnmOpcode_PosTime = 32,
    AnmOpcode_ColorTime = 33,
    AnmOpcode_AlphaTime = 34,
    AnmOpcode_RotateTime = 35,
    AnmOpcode_ScaleTime = 36,
    AnmOpcode_ISet = 37,
    AnmOpcode_FSet = 38,
    AnmOpcode_IAdd = 39,
    AnmOpcode_FAdd = 40,
    AnmOpcode_ISub = 41,
    AnmOpcode_FSub = 42,
    AnmOpcode_IMul = 43,
    AnmOpcode_FMul = 44,
    AnmOpcode_IDiv = 45,
    AnmOpcode_FDiv = 46,
    AnmOpcode_IMod = 47,
    AnmOpcode_FMod = 48,
    AnmOpcode_ISetAdd = 49,
    AnmOpcode_FSetAdd = 50,
    AnmOpcode_ISetSub = 51,
    AnmOpcode_FSetSub = 52,
    AnmOpcode_ISetMul = 53,
    AnmOpcode_FSetMul = 54,
    AnmOpcode_ISetDiv = 55,
    AnmOpcode_FSetDiv = 56,
    AnmOpcode_ISetMod = 57,
    AnmOpcode_FSetMod = 58,
    AnmOpcode_ISetRand = 59,
    AnmOpcode_FSetRand = 60,
    AnmOpcode_FSin = 61,
    AnmOpcode_FCos = 62,
    AnmOpcode_FTan = 63,
    AnmOpcode_FAcos = 64,
    AnmOpcode_FAtan = 65,
    AnmOpcode_NormalizeAngle = 66,
    AnmOpcode_IJmpEq = 67,
    AnmOpcode_FJmpEq = 68,
    AnmOpcode_IJmpNeq = 69,
    AnmOpcode_FJmpNeq = 70,
    AnmOpcode_IJmpLess = 71,
    AnmOpcode_FJmpLess = 72,
    AnmOpcode_IJmpLessOrEq = 73,
    AnmOpcode_FJmpLessOrEq = 74,
    AnmOpcode_IJmpGreater = 75,
    AnmOpcode_FJmpGreater = 76,
    AnmOpcode_IJmpGreaterOrEq = 77,
    AnmOpcode_FJmpGreaterOrEq = 78,
    AnmOpcode_Wait = 79,
    AnmOpcode_UScroll = 80,
    AnmOpcode_VScroll = 81,
    AnmOpcode_BlendMode = 82,
    AnmOpcode_Ins83 = 83,
    AnmOpcode_Color2 = 84,
    AnmOpcode_Alpha2 = 85,
    AnmOpcode_Color2Time = 86,
    AnmOpcode_Alpha2Time = 87,
    AnmOpcode_Ins88 = 88,
    AnmOpcode_ReturnFromInterrupt = 89
};

struct AnmLoadedSprite
{
    i32 anmIdx;
    u32 texture;
    Vec2 startPixelInclusive;
    Vec2 endPixelInclusive;
    float height;
    float width;
    Vec2 uvStart;
    Vec2 uvEnd;
    float heightPx;
    float widthPx;
    Vec2 scaleFactor;
    u32 unk0x40;
};

static_assert(sizeof(AnmLoadedSprite) == 0x44);

#define ANM_MAX_ARGS 10

struct AnmRawInstr
{
    i16 opcode;
    u16 instructionSize;
    i16 time;
    u16 varMask;
};

struct AnmVmBase
{
    void Initialize()
    {
        memset(this, 0, sizeof(AnmVmBase));

        this->scale.x = 1.0f;
        this->scale.y = 1.0f;
        this->color1.d3dColor = COLOR_WHITE;
        this->matrix1.identity();
        this->flags = 7;
        this->currentTimeInScript.set(0);
    }

    bool IsVisible()
    {
        return this->visible;
    }

    void SetInvisible()
    {
        this->visible = false;
    }

    bool IsStopped()
    {
        return this->stopped;
    }

    void SetInterrupt(i16 interrupt)
    {
        this->pendingInterrupt = interrupt;
    }

    Vec3 rotation;
    Vec3 angleVel;
    Vec2 scale;
    Vec2 scaleGrowth;
    Vec2 spriteSize;
    Vec2 uvScrollPos;
    Timer currentTimeInScript;
    Timer waitTimer;
    Timer interpCurrentTimers[AnmInterp_Last];
    Timer interpEndTimers[AnmInterp_Last];
    u8 interpModes[AnmInterp_Last];
    i32 intVar0;
    i32 intVar1;
    i32 intVar2;
    i32 intVar3;
    float floatVar0;
    float floatVar1;
    float floatVar2;
    float floatVar3;
    i32 counterVar0;
    i32 counterVar1;
    Vec2 uvScrollVel;
    Matrix4 matrix1;
    Matrix4 matrix2;
    Matrix4 matrix3;
    ZunColor color1;
    ZunColor color2;
    union {
        u16 flags;
        struct
        {
            u32 visible : 1;
            u32 flag1 : 1;
            u32 updateRotation : 1;
            u32 updateScale : 1;
            u32 blendMode : 2;
            u32 flag6 : 1;
            u32 flag7 : 1;
            u32 usePosOffset : 1;
            u32 flip : 2;
            u32 anchor : 2;
            u32 zWriteDisabled : 1;
            u32 stopped : 1;
            u32 flag15 : 1;
            u32 flag16 : 1;
            u32 flag17 : 1;
            u32 flag18 : 1;
            u32 flag19 : 1;
        };
    };
    i16 type;
    i16 pendingInterrupt;
    i32 playerBulletHitAnimationType;
    AnmLoaded *anmFile;
};

static_assert(sizeof(AnmVmBase) == 0x208);

struct AnmVm : AnmVmBase
{
    Vec3 pos;
    i16 activeSpriteIndex;
    i16 anmFileIndex;
    i16 baseSpriteIndex;
    i16 scriptIndex;
    AnmRawInstr *beginningOfScript;
    AnmRawInstr *currentInstruction;
    AnmLoadedSprite *loadedSprite;
    Timer interruptReturnTime;
    AnmRawInstr *interruptReturnInstruction;

    Vec3 posInitial;
    Vec3 posFinal;
    Vec3 rotateInitial;
    Vec3 rotateFinal;
    Vec2 scaleInitial;
    Vec2 scaleFinal;
    ZunColor color1Initial;
    ZunColor color1Final;
    ZunColor color2Initial;
    ZunColor color2Final;

    Vec3 pos2;
    i32 timeOfLastSpriteSet;
    u8 fontWidth;
    u8 fontHeight;
    u8 reserved29a[10];

    AnmVm()
    {
        memset(this, 0, sizeof(AnmVm));
        this->activeSpriteIndex = -1;
    }

    double GetFloatVar(float varId,Rng& random);
    i32 GetIntVar(i32 varId);
    float *GetFloatVarPtr(float *varPtr, u16 varMask, u32 variableNumber);
    i32 *GetIntVarPtr(i32 *varPtr, u16 varMask, u32 variableNumber);
};

static_assert(sizeof(AnmVm) == 0x2a4);


}
