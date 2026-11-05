#pragma once
#include "BulletEmission.hpp"
#include "Timer.hpp"
namespace th09 {
struct EclVm;struct EclInstruction;
struct EclEmitter {
    BulletEmission parameters;
    Vec3 offset;alignas(4) u8 repeated[44]{};i32 period=0;Timer time{0,0,0};
    bool shoot(EclVm&,const EclInstruction&,BulletEmissionActions&);
    bool command(EclVm&,const EclInstruction&,BulletEmissionActions&);
    bool update(EclVm&,const FrameTiming&,BulletEmissionActions&);
};
}
