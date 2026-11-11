// 帧计时器：ANM/ECL 共用的整数帧 + 浮点分数双轨（对齐 th15 Timer 行为；
// th13 世代共享同族实现，rate_index 字段保留原版 autosave 布局位）。
#pragma once

#include "Types.hpp"

namespace th13 {

struct Timer {
    i32 previous = -999999, current = 0;
    float fractional = 0;
    u32 rate_index = 0, flags = 0;

    void set(i32 frame) noexcept {
        if (!(flags & 1)) {
            current = 0;
            previous = -999999;
            fractional = 0;
            rate_index = 0;
            flags |= 1;
        }
        current = frame;
        previous = wrapping_add(frame, -1);
        fractional = static_cast<float>(frame);
    }

    void tick(const float* rate) noexcept {
        rate_index = 0;
        previous = current;
        const float speed = rate ? *rate : 1.f;
        if (speed > .99f && speed < 1.01f) {
            current = wrapping_add(current, 1);
            fractional = static_cast<float>(fractional + 1.f);
        } else {
            fractional = static_cast<float>(fractional + speed);
            current = truncate_int(fractional);
        }
    }

    void decrement(const float* rate) noexcept {
        rate_index = 0;
        previous = current;
        const float speed = rate ? *rate : 1.f;
        fractional =
            static_cast<float>(fractional - (speed > .99f && speed < 1.01f ? 1.f : speed));
        current = truncate_int(fractional);
    }
};

static_assert(sizeof(Timer) == 20);

}  // namespace th13
