#pragma once
#include "FighterObject.hpp"
#include "ReimuState.hpp"
#include "ReplayFile.hpp"
namespace th075 {
struct ReimuObject:FighterObject {i16 counter=0,timer=0,angle=0,cycle=0,variant=0;};
bool advance_reimu_object_basic(ReimuObject&,FighterActor& owner,FighterActor& opponent,ReimuState&,ReplayStreams&,FighterActionEvents&);
bool advance_reimu_object_story(ReimuObject&,FighterActor& owner,FighterActor& opponent,ReimuState&,ReplayStreams&,FighterActionEvents&);
void advance_reimu_object(ReimuObject&,FighterActor& owner,FighterActor& opponent,ReimuState&,ReplayStreams&,FighterActionEvents&);
}
