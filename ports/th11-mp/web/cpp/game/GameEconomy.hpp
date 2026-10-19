#pragma once
#include "Types.hpp"
namespace th11 {
// Gameplay counters. Score is stored in units of ten, as in the original.
struct GameEconomy {
#ifdef TH11_MULTIPLAYER
private:
    // An ordinary/session economy owns the score. Pilot economies bind that
    // same value for their entire lifetime; there is no per-seat score merge.
    i32 owned_score_units=0;
public:
    i32& score_units;
    GameEconomy() noexcept:score_units(owned_score_units){}
    explicit GameEconomy(i32& shared_score) noexcept:score_units(shared_score){}
    GameEconomy(const GameEconomy& other) noexcept:GameEconomy(){*this=other;}
    GameEconomy& operator=(const GameEconomy&) noexcept;
#else
    i32 score_units=0;
#endif
    i32 power=0,point_value=0,communication=0;
    i32 lives=2,life_fragments=0,difficulty=1,rank=0;
    i32 max_power=400,power_step=100,graze=0,max_point_value=0;
    void add_score(i32 amount) noexcept;
    void add_point_value(i32 amount) noexcept;
    void add_rank(i32 amount) noexcept;
    i32 point_item_score() const noexcept;
};
}
