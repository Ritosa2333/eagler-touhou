#pragma once
#include "ReimuRecovery.hpp"
#include "ReimuCommands.hpp"
#include "ReimuFrame.hpp"
#include "StorySequence.hpp"
namespace th075 {
struct ReimuState {
    StoryCutscene story;ReimuRecoveryState recovery;ReimuCommandState commands;ReimuFrameState frame;
    std::array<i16,3> actionCounters{};i16 introWait=0,captionWait=0;
    i16 orbitAngle=0,volleyMode=0;i8 difficulty=0;
};
}
