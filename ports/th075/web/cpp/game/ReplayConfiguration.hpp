#pragma once
#include "ReplayFile.hpp"
#include "FighterActor.hpp"
#include "GameProfile.hpp"
namespace th075 {
struct ReplayConfiguration {
 static void record(ReplayFile&,ReplayRound&,const GameSelection&,const SelectionSettings&,const RoundRules&,const FighterActor&);
 static bool load(ReplayFile&,u32 round,GameSelection&,SelectionSettings&,RoundRules&,ReplayStreams&);
};
}
