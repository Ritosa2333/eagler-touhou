#pragma once
#include "FighterObject.hpp"
#include "SakuyaState.hpp"
namespace th075 {struct SakuyaObject:FighterObject {i16 counter=0,timer=0,angle=0,cycle=0,variant=0,auxiliary=0;};
void advance_sakuya_object(SakuyaObject&,FighterActor&,FighterActor&,SakuyaState&,ReplayStreams&,FighterActionEvents&);}
