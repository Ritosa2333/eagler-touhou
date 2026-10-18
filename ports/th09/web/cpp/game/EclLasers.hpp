#pragma once
#include "BulletEmission.hpp"
#include "LaserManager.hpp"
namespace th09 {
struct EclVm;struct EclInstruction;
struct LaserEmissionActions {
    virtual ~LaserEmissionActions()=default;
    virtual Laser* emit_laser(const BulletEmission&)=0;
};
struct EclLasers {
    BulletEmission parameters;Laser* references[32]{};i32 selected=0;
    bool command(EclVm&,const EclInstruction&,LaserEmissionActions&);
};
}
