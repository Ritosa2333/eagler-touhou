#pragma once
#include "Types.hpp"
namespace th13 {
inline float result_slowdown_percent(double actual, double budget) noexcept {
    if (budget <= 0.0) return 0.0f;
    const float ratio = float(actual / budget);
    return float(100.0 - double(ratio) * 100.0);
}
inline i32 result_record_stage(bool clear_result, i32 stage) noexcept { return clear_result ? 8 : stage; }
inline i32 result_record_stage(bool clear_result, bool extra_clear, i32 stage) noexcept {
    if (extra_clear && stage == 7) return 9;
    return result_record_stage(clear_result, stage);
}
}
