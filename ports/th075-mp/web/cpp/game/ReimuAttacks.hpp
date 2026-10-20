#pragma once
#include "ReimuRecovery.hpp"
namespace th075 {
// Action identifiers select original character animation data. Dispatch uses
// ordinary named C++ owners; no native function address appears in this layer.
bool advance_reimu_melee(FighterActor&,FighterActor& opponent,ReimuRecoveryState&,FighterActionEvents&);
bool advance_reimu_shooting(FighterActor&,const FighterActor& opponent,ReimuRecoveryState&,FighterActionEvents&);
}
