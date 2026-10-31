// Switch host: a thbgm.dat track as a miniaudio data source.
#pragma once
#include "MiniaudioConfig.hpp"
#include "Platform.hpp"
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <thread>
#include <vector>
namespace th12host::host {
// Reads thbgm.dat on its own thread, a couple of seconds ahead of playback
// and in playback order (on from the loop start after the track's end), so
// the game thread never waits for the SD card.
class BgmReader {
public:
    explicit BgmReader(FILE* file);~BgmReader();
    // A track at byte `offset`: `frames` long, looping back to `loop_start`;
    // playback continues from `frame`.
    void start(u64 offset,u64 frames,u64 loop_start,u32 channels,u64 frame);
    void stop();
    // Up to `count` frames in playback order; waits only while the reader is
    // behind (the start of a track). Fewer after a read error.
    size_t take(i16* out,size_t count);
private:
    FILE* file;std::thread thread;std::mutex mutex;std::condition_variable wake,ready;
    std::vector<i16> ring,chunk;size_t first=0,size=0; // samples
    u64 offset=0,frames=0,loop_start=0,next=0,position=~0ull;u32 channels=2,generation=0;bool active=false,failed=false,quit=false;
    void run();
};
struct PcmStream {
    ma_data_source_base base{};BgmReader* reader=nullptr;u64 offset=0,frames=0,loop_start=0,cursor=0;u32 channels=2,rate=44100;
    std::vector<i16> scratch;
};
ma_result pcm_seek(ma_data_source*,ma_uint64 frame);
extern const ma_data_source_vtable pcm_vtable;
}
