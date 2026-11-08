#pragma once
#include "Types.hpp"
namespace th13 {
// 0043d7e0..0043d80c stores the ratio as float before multiplying by 100.
// This is slowdown, not completion percentage; full-speed play reports 0%.
inline float result_slowdown_percent(double actual, double budget) noexcept {
    if (budget <= 0.0) return 0.0f;
    const float ratio = float(actual / budget);
    return float(100.0 - double(ratio) * 100.0);
}
// 00454050 temporarily sets stage=8 before inserting the all-clear score.
inline i32 result_record_stage(bool clear_result, i32 stage) noexcept {
    return clear_result ? 8 : stage;
}
}  // namespace th13
