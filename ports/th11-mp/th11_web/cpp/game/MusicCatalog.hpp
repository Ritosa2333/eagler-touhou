#pragma once
#include "Types.hpp"
namespace th11 {
struct MusicTrack {i32 cue;const char* file;};
inline constexpr MusicTrack music_tracks[]={
 {0,"th11_00"},{1,"th11_01"},{2,"th11_02"},{3,"th11_03"},{4,"th11_05"},
 {5,"th11_06"},{6,"th11_07"},{7,"th11_08"},{8,"th11_10"},{9,"th11_12"},
 {10,"th11_13"},{11,"th11_14"},{12,"th11_16"},{13,"th11_15"},{14,"th11_17"},
 {15,"th11_18"},{16,"th11_19"},{17,"th10_17"}
};
// Original stage descriptor (4a3828 + stage*64), music-room unlock indices.
inline i32 stage_music(i32 stage,bool boss=false){return stage>=1&&stage<=7?(stage-1)*2+(boss?2:1):0;}
}
