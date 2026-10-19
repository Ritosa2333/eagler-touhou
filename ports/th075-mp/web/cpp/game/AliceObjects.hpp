#pragma once
#include "FighterObject.hpp"
#include "AliceState.hpp"
namespace th075 {struct AliceObject:FighterObject {i16 counter=0,timer=0,angle=0,cycle=0,variant=0,auxiliary=0;};
void advance_alice_object(AliceObject&,FighterActor&,FighterActor&,AliceState&,ReplayStreams&,FighterActionEvents&);
}
