#pragma once
#include "FighterObject.hpp"
#include "YuyukoState.hpp"
namespace th075 {struct YuyukoObject:FighterObject {i16 counter=0,timer=0,angle=0,cycle=0,variant=0,auxiliary=0;};
void advance_yuyuko_object(YuyukoObject&,FighterActor&,FighterActor&,YuyukoState&,ReplayStreams&,FighterActionEvents&);}
