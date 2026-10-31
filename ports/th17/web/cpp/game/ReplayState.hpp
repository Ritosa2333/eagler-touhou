#pragma once
#include "RoaringSystem.hpp"
#include <cstddef>
namespace th17 {
// Serialized run state in a t17r v2 stage header. The game itself keeps these
// values in their named owners; this layout is only the replay file boundary.
struct ReplayRunState {
    i32 stage=1,initial_stage=1,chapter=0;
    std::array<i32,3> counters{};
    i32 character=0,animal=0,score=0,difficulty=1,continues=0,rank=0;
    i32 grazes=0,chapter_grazes=0,spell_id=-1,deaths=0,reserved_40=0;
    i32 point_items=0,point_value=0,initial_point_value=0,max_point_value=0;
    i32 power=0,max_power=0,power_step=0,reserved_60=0;
    i32 lives=0,life_pieces=0,life_piece_tier=0,bombs=0,bomb_pieces=0,initial_bombs=0;
    i32 chapter_total=0,chapter_defeated=0;
    RoaringState roaring;
    // Native assignment restores timer values through Timer::set. Copying the
    // serialized timer flags/fractional values would change stage-entry state.
    void assign(const ReplayRunState&)noexcept;
};
static_assert(sizeof(ReplayRunState)==0xec);
static_assert(offsetof(ReplayRunState,roaring)==0x84);
struct ReplayStageState {
    u16 stage=0,seed=0;u32 frames=0,payload_size=0;
    PlayerFixedPosition player;
    ReplayRunState run;
    i32 focus=0;std::array<u32,20> reserved{};u32 frame_gate=0;
    void seed_random(Rng& game,Rng& visual)const noexcept{game.seed=seed;game.calls=0;visual.seed=seed;}
    void restore_player(PlayerMotion&)const noexcept;
};
static_assert(sizeof(ReplayStageState)==0x158);
static_assert(offsetof(ReplayStageState,run)==0x14);
static_assert(offsetof(ReplayStageState,focus)==0x100);
}
