#pragma once
#include "ReplayRecording.hpp"
namespace th08 {
// Calendar strings and frame measurements are supplied by the application.
struct ReplayExportContext {
    char player_name[9]{},date[6]{},timestamp[20]{};
    float rendered_frames=1,total_frames=1;
    i32 human_frames=0,active_frames=1;
    bool cheat_movement_used=false;
};
// Original SaveReplay metadata, stage packing, USER information and obfuscation.
// Storage and the decision to dispose of the recording remain application I/O.
std::vector<u8> export_replay(ReplayRecording&,GameplaySession&,const EclGlobals&,const ReplayExportContext&);
}
