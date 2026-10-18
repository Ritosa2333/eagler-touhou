#pragma once
#include "Types.hpp"
namespace th09 {
struct MusicTrack {i32 cue,title,unlock;const char* file;};
// Cue, HUD title and Music Room slot are three distinct original identifiers.
inline constexpr MusicTrack music_tracks[]={
{0,-1,0,"th09_00"},{2,0,1,"th09_01"},{3,1,2,"th09_00b"},{4,2,3,"th09_02"},
{5,3,4,"th07_10_b"},{6,4,5,"th08_12"},{7,5,6,"th09_05"},{8,6,7,"th07_09"},
{9,7,8,"th09_07"},{10,8,9,"th09_10"},{11,9,12,"th09_13"},{12,10,10,"th09_08_2"},
{13,11,11,"th09_12"},{14,12,13,"th09_09"},{15,13,14,"th09_11"},
{1,-1,15,"th09_00c"},{18,-1,16,"th09_15"},{16,-1,17,"th09_14"},{17,-1,18,"th09_17"}};
inline const MusicTrack* music_track(i32 cue){for(const auto& track:music_tracks)if(track.cue==cue)return &track;return nullptr;}
}
