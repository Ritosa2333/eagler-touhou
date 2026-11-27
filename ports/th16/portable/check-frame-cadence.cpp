// th16 帧钟自检（launcher/tests/test-presentation-contract-coverage.mjs 的 runtime 车道）：
// FrameClock 以 rAF 墙钟折算逻辑步数——任意刷新率下 10 秒内都收敛到 60 逻辑帧/秒（首帧固定 1 步、不欠账），
// 单帧最多追 4 步（原版靠跳帧 DAT_004c12c4），超过半秒的挂起直接对齐不补帧，暂停（started=false）后恢复不追帧。
#include "../web/cpp/game/FrameClock.hpp"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <initializer_list>

int main() {
    for (unsigned hz : {15u, 20u, 30u, 50u, 59u, 60u, 75u, 90u, 120u, 144u, 165u, 240u}) {
        th16::FrameClock clock;
        unsigned steps = 0;
        const unsigned frames = hz * 10;
        for (unsigned i = 0; i < frames; ++i) steps += clock.begin_frame(double(i) / hz);
        // 首帧 1 步 + 其后 (frames-1)/hz 秒 × 60；四舍五入误差不超过 1 步
        const double expected = 1.0 + double(frames - 1) / hz * 60.0;
        assert(std::fabs(double(steps) - expected) <= 1.0);
    }
    th16::FrameClock clock;
    assert(clock.begin_frame(0.0) == 1);
    assert(clock.begin_frame(0.1) == th16::FrameClock::kMaxSteps);   // 追帧封顶，余量丢弃
    assert(clock.begin_frame(0.1 + 1.0 / 60.0) == 1);                // 封顶后不再欠账
    assert(clock.begin_frame(1.0) == 1);                             // > 半秒：对齐，不补帧
    clock.started = false;                                           // 暂停恢复：单步、无欠账
    assert(clock.begin_frame(5.0) == 1);
    assert(clock.begin_frame(5.0 + 1.0 / 60.0) == 1);
    clock.catch_up = false;                                          // DAT_004d9d1c & 0x40 关：恒 1 步
    assert(clock.begin_frame(9.0) == 1 && clock.begin_frame(9.5) == 1);
    std::puts("th16 FrameClock: ok");
    return 0;
}
