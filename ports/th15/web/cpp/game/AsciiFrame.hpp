#pragma once
#include "AsciiText.hpp"
#include "AnmDrawSchedule.hpp"
namespace th15 {
// Each caption space has its own original callback priority and camera lifetime.
class AsciiFrame {
 FrameScheduler& scheduler;AsciiText& text;AnmRenderer& renderer;AnmDrawServices& views;std::array<FrameCallback,4> callbacks;
 bool draw(i32);
public:
 std::string error;i32 frames=0;
 AsciiFrame(FrameScheduler&,AsciiText&,AnmRenderer&,AnmDrawServices&);~AsciiFrame();
 AsciiFrame(const AsciiFrame&)=delete;AsciiFrame& operator=(const AsciiFrame&)=delete;
};
}
