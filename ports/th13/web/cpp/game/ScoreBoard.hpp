#pragma once

#include "Types.hpp"

namespace th13 {

constexpr i32 stage_clear_bonus(i32 stage) { return stage * 1000000; }

constexpr i32 all_clear_bonus(i32 difficulty, i32 lives, i32 bombs) {
    switch (difficulty) {
    case 0:
        return (bombs + lives * 4) * 250000;
    case 1:
        return (bombs + lives * 3) * 1000000;
    case 2:
        return lives * 5000000 + bombs * 1500000;
    case 3:
        return (bombs + lives * 5) * 2000000;
    default:
        return 0;
    }
}

constexpr i32 extra_clear_bonus(i32 lives, i32 bombs) { return (bombs + lives * 10) * 4000000; }

inline u32 add_bonus_capped(u32 score, i32 bonus) {
    const i64 next = i64(score) + bonus / 10;
    if (next < 0) return 0;
    if (next > 999999999) return 999999999;
    return u32(next);
}

}
