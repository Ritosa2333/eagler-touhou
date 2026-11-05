#pragma once
#include "AsciiManager.hpp"
namespace th08 {
struct FrameClock {
    virtual ~FrameClock()=default;
    virtual u32 milliseconds()=0;
    virtual u64 performance_counter()=0;
};
struct FrameStatisticsState {
    u32 timer_started=0,milliseconds_origin=0,frames=0,performance_samples=0;
    u64 performance_origin=0;
    float rendered=0,total=0;i16 replay_fps=0;u16 padding=0;
    char fps_text[256]{},replay_text[256]{};
};
struct FrameStatisticsContext {
    u32 frequency=0,game_flags=0;bool paused=false;u8 frameskip=0;bool suppress_text=false,software_texturing=false,replay_warning=false;
};
// Original CalculateFps: display measurements and quantized replay slowdown
// records. This does not change the game's fixed update rate.
class FrameStatistics {
    AsciiManager& ascii;FrameClock& clock;
    u32 presentation_origin=0,presentation_frames=0;bool presentation_started=false;
public:
    FrameStatisticsState state;FrameStatisticsContext context;
    FrameStatistics(AsciiManager& ascii,FrameClock& clock):ascii(ascii),clock(clock){}
    void calculate(bool draw);
    void draw_text();
    void presentation_frame();
};
}
