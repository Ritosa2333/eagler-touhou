#pragma once
#include "FighterObject.hpp"
#include "PatchouliState.hpp"
namespace th075 {struct PatchouliObject:FighterObject {i16 counter=0,timer=0,angle=0,cycle=0,variant=0,auxiliary=0;};
void advance_patchouli_object(PatchouliObject&,FighterActor&,FighterActor&,PatchouliState&,ReplayStreams&,FighterActionEvents&);}
