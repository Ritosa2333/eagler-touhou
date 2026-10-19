#pragma once
#include "ReimuRecovery.hpp"
namespace th075 {
struct ReimuCommandState {i16 activeSeals=0;};
void select_reimu_commands(FighterActor&,const FighterActor& opponent,const ReimuCommandState&);
}
