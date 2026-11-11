#pragma once
#include "AsciiText.hpp"
#include "AnmDrawSchedule.hpp"
namespace th13 {
// 原版 ASCII 三 pass 消费优先级（objdump 实读 0x403a00/0x403a54）：pass0
// thunk 0x403d80=0x4a(74)、pass1 thunk 0x403d90=0x2f(47)。端口 pass0 保持
// 78：所有 space0 生产者（含 fps 叠加 0x424a90 端口侧@77）都在 78 之前，
// 74/78 之间无其它端口回调，行为等价；pass2 无端口生产者，沿用 63。
inline constexpr i32 kAsciiPassPriority[3]{78, 47, 63};
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
