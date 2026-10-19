#pragma once
#include "FighterObject.hpp"
#include "YoumuState.hpp"
namespace th075 {struct YoumuObject:FighterObject {i16 counter=0,timer=0,angle=0,cycle=0,variant=0,auxiliary=0;};
void advance_youmu_object(YoumuObject&,FighterActor&,FighterActor&,YoumuState&,ReplayStreams&,FighterActionEvents&);}
