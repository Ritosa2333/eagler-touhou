#pragma once
#include "ScreenShake.hpp"
#include "FrameScheduler.hpp"
#include <functional>
#include <memory>
namespace th15 {
// The native motion effect owns a priority-20 update independently of its
// requesting enemy or bomb. Offsets are consumed by drawing, never gameplay.
class ScreenMotionFrame {
 FrameScheduler& scheduler;FrameCallback callback;std::unique_ptr<ScreenShake> shake;std::unique_ptr<ScreenNudge> nudge;
 std::function<ScreenShakeContext()> context;std::function<void(Vec2,Vec2)> offsets;
 i32 update();void install();
public:
 bool active=true;
 ScreenMotionFrame(FrameScheduler&,Rng&,ScreenShakeSpec,std::function<ScreenShakeContext()>,std::function<void(Vec2,Vec2)>);
 ScreenMotionFrame(FrameScheduler&,Rng&,ScreenNudgeSpec,std::function<ScreenShakeContext()>,std::function<void(Vec2,Vec2)>);
 ~ScreenMotionFrame();void retire()noexcept;
};
}
