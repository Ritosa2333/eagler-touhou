#pragma once
#include "AsciiText.hpp"
#include "ScreenViews.hpp"
namespace th17 {
class AsciiDrawSchedule {
    struct Callback {FrameCallback frame;AsciiDrawSchedule* owner=nullptr;u32 index=0;};std::array<Callback,4> callbacks;
    FrameScheduler& scheduler;AsciiText& text;AnmRenderer& renderer;ScreenViews& views;bool pass(u32);
public:
    std::string error;
    AsciiDrawSchedule(FrameScheduler&,AsciiText&,AnmRenderer&,ScreenViews&);~AsciiDrawSchedule();
};
}
