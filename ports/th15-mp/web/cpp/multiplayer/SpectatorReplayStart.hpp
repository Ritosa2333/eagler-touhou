#pragma once
#include "SpectatorStageState.hpp"
#include "SessionSetup.hpp"
#include "../game/GameInput.hpp"
namespace th15::sdl {struct ApplicationState;}
namespace th15::multiplayer {
// Streaming Replay enters through the native per-stage Replay header.
class SpectatorReplayStart {
public:
    static bool Boundary(const sdl::ApplicationState&);
    static bool Capture(sdl::ApplicationState&,const SessionSetup&,const GameInput&,u32 frame,std::vector<u8>&,std::string&);
    static bool Restore(sdl::ApplicationState&,const SessionSetup&,GameInput&,u32 frame,const std::vector<u8>&,std::string&);
};
}
