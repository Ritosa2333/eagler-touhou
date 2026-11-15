#pragma once
#include <cstdint>

namespace touhou::sdl {
// Retail46a720 software gate and46a950 vsync dispatch. Missed deadlines move
// forward without issuing extra outer update chains. Replay reruns belong to
// the update chain itself and are not capped by this outer-frame decision.
struct FrameDispatchTh14 {
    static constexpr double interval=1./60.;
    double previous=0,deadline=0;
    std::uint8_t draw_counter=0;
    struct Decision {bool update=false,sleep_one_ms=false;};
    void reset(double now=0)noexcept{previous=deadline=now;draw_counter=0;}
    Decision begin(double now,bool vsync)noexcept{
        if(vsync)return {true,false};
        if(previous>now)deadline=now;
        previous=now;
        const bool sleep=(deadline-now)*1000.>=1.5;
        if(!(now>deadline))return {false,sleep};
        do{deadline+=interval;}while(now>deadline);
        return {true,sleep};
    }
    // Call only after a successful update chain. The signed byte comparison
    // and the reset after drawing preserve the original counter's lifetime.
    bool draw_after_update(std::uint8_t skipped_frames)noexcept{
        draw_counter=std::uint8_t(unsigned(draw_counter)+1u);
        const int counter=draw_counter<128?int(draw_counter):int(draw_counter)-256;
        return counter>=int(skipped_frames)+1;
    }
    void finish_draw()noexcept{draw_counter=0;}
};
}
