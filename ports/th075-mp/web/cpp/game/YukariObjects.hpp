#pragma once
#include "FighterObject.hpp"
#include "YukariState.hpp"
namespace th075 {struct YukariObject:FighterObject {i16 counter=0,timer=0,angle=0,cycle=0,variant=0,auxiliary=0;};
void advance_yukari_object(YukariObject&,FighterActor&,FighterActor&,YukariState&,ReplayStreams&,FighterActionEvents&);}
