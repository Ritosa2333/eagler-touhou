#pragma once
#include "Types.hpp"
#include <string>
#include <vector>
namespace th15 {
struct MusicTrack {std::string filename;u32 offset=0,length=0,loop_start=0,loop_end=0,rate=0;u16 channels=0,alignment=0,bits=0;u64 frames()const noexcept{return alignment?loop_end/alignment:0;}u64 loop_frame()const noexcept{return alignment?loop_start/alignment:0;}};
class MusicLayout {
 std::vector<MusicTrack> tracks;
public:
 std::string error;bool open(const u8*,u32);u32 count()const noexcept{return tracks.size();}
 const MusicTrack* track(u32 index)const noexcept{return index<tracks.size()?&tracks[index]:nullptr;}
 // Original lookup prefers the last forward slash, then a backslash, and
 // falls back to track zero for an unknown exact WAV filename.
 i32 original_index(const std::string&)const noexcept;
};
i32 adjusted_music_volume(i32 base,i32 master)noexcept;
}
