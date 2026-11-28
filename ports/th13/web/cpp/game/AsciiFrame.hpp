#pragma once
#include "AnmDrawSchedule.hpp"
#include "AsciiText.hpp"
namespace th13 {
inline constexpr i32 kAsciiPassPriority[3]{78, 47, 63};
class AsciiFrame {
    FrameScheduler& scheduler;
    AsciiText& text;
    AnmRenderer& renderer;
    AnmDrawServices& views;
    std::array<FrameCallback, 4> callbacks;
    bool draw(i32);

public:
    std::string error;
    i32 frames = 0;
    AsciiFrame(FrameScheduler&, AsciiText&, AnmRenderer&, AnmDrawServices&);
    ~AsciiFrame();
    AsciiFrame(const AsciiFrame&) = delete;
    AsciiFrame& operator=(const AsciiFrame&) = delete;
};
}
