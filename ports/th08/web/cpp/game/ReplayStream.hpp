#pragma once
#include "Types.hpp"
#include <vector>
namespace th08 {
struct ReplayInputState {
    u16 physical=0,current=0,previous=0,repeat=0;
    i32 held_frames=0;
    u32 flags=0;
    bool slow_mode=false,speedhack=false;
    u8 timing_level=0;
    i32 timing_forced=0;
};
class ReplayStream {
public:
    i32 frame=0,transition_frames=0;
    u32 input_cursor=0,timing_cursor=0,input_end=0,timing_end=0;
    std::vector<u8> input,timing;
    void begin_record(u16 seed);
    bool begin_playback(const u8* keys,u32 key_size,const u8* timing_bytes,u32 timing_size,bool debug_records);
    void record(ReplayInputState& state);
    bool play(ReplayInputState& state);
    void stop_recording();
private:
    u32 stride=2;
};
}
