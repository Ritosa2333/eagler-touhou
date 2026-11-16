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
// 0043f961..0043f96a: the Extra results overlay (+0x1f4 = 1, set only by
// 0043f720) inserts an Extra score as stage 9, "ExClear".
inline i32 result_record_stage(bool clear_result, bool extra_clear, i32 stage) noexcept {
    if (extra_clear && stage == 7) return 9;
    return result_record_stage(clear_result, stage);
}
}  // namespace th13
