#pragma once
#include "FighterObject.hpp"
#include "SuikaState.hpp"
namespace th075 {struct SuikaObject:FighterObject {i16 counter=0,timer=0,angle=0,cycle=0,variant=0,auxiliary=0;};
void advance_suika_object(SuikaObject&,FighterActor&,FighterActor&,SuikaState&,ReplayStreams&,FighterActionEvents&);}
