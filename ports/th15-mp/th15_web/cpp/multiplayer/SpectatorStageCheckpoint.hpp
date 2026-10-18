#pragma once
#include "SpectatorStageState.hpp"
namespace th15::sdl {struct ApplicationState;}
namespace th15::multiplayer {
// Sparse stage checkpoints use the existing portable object codecs. They do
// not invoke the Pointdevice retry path, award resources or change the host.
class SpectatorStageCheckpoint {
public:
    static bool Boundary(const sdl::ApplicationState&);
    static bool Capture(sdl::ApplicationState&,const SessionSetup&,const GameInput&,u32 frame,std::vector<u8>&,std::string&);
    static bool Restore(sdl::ApplicationState&,const SessionSetup&,GameInput&,u32 frame,const std::vector<u8>&,std::string&);
};
}
