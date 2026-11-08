#pragma once
#include "Types.hpp"
#include <string>
#include <vector>
namespace th13 {
struct MusicTrack {
    std::string filename;
    u32 offset = 0, length = 0, loop_start = 0, loop_end = 0, rate = 0;
    u16 channels = 0, alignment = 0, bits = 0;
    u32 frames() const { return loop_end / alignment; }
    u32 loop_frame() const { return loop_start / alignment; }
};
// thbgm.fmt is a zero-terminated array of original 52-byte records.
class MusicLayout {
    std::vector<MusicTrack> tracks;
public:
    std::string error;
    bool open(const u8*, u32);
    u32 count() const { return u32(tracks.size()); }
    const MusicTrack* track(i32 index) const {
        return index >= 0 && u32(index) < tracks.size() ? &tracks[index] : nullptr;
    }
    i32 index(const std::string&) const;
};
}
