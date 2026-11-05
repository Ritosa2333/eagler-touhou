#pragma once

#include "AnmVm.hpp"
#include "ZunMath.hpp"
#include "ZunTimer.hpp"
#include "inttypes.hpp"
#ifdef TH_ENABLE_NETPLAY
#include "netplay/NetplaySideEffects.hpp"
#endif

enum EffectCallbackResult
{
    EFFECT_CALLBACK_RESULT_STOP = 0,
    EFFECT_CALLBACK_RESULT_DONE = 1
};

struct Effect;

typedef i32 (*EffectUpdateCallback)(Effect *);
struct Effect
{
    AnmVm vm;
    ZunVec3 pos1;
    ZunVec3 prevPos;
    ZunVec3 unk_11c;
    ZunVec3 unk_128;
    ZunVec3 position;
    ZunVec3 pos2;
    ZunVec4 quaternion;
    f32 unk_15c;
    f32 angleRelated;
    ZunTimer timer;
    i32 unk_170;
    EffectUpdateCallback updateCallback;
    i8 inUseFlag;
    i8 effectId;
    i8 unk_17a;
    i8 unk_17b;
#ifdef TH_ENABLE_NETPLAY
    Netplay::SideEffects::MissFrameTag missConfirmation;
#endif
};

struct EffectInfo
{
    i32 anmIdx;
    EffectUpdateCallback updateCallback;
};
