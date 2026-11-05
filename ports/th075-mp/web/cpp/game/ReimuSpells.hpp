#pragma once
#include "ReimuRecovery.hpp"
#include "ReplayFile.hpp"
namespace th075 {
bool advance_reimu_spells(FighterActor&,FighterActor& opponent,ReimuRecoveryState&,GameMode,i8 difficulty,u8 replayMode,ReplayStreams&,FighterCoreEvents&,FighterActionEvents&);
}
