#pragma once
#include "../multiplayer/native-world.hpp"
#include "multiplayer/BattleFrameState.hpp"
#include <chrono>
#include <type_traits>
using th15::multiplayer::BattleFrameState;
#include "battle-fingerprint.hpp"
using namespace th15::rollback_test;
static std::array<MultiplayerInput,3> input(unsigned frame){std::array<MultiplayerInput,3> out{};for(unsigned s=0;s<3;++s){out[s].held=1|((frame/37+s)%2?8:0)|((frame/91+s)%2?0x40:0x80);if(frame%173==s*13)out[s].held|=2;}return out;}
