#pragma once
#include "player_life.hpp"
#include "player_motion.hpp"
#include "player_shots.hpp"
#include "pickup_core.hpp"
#include <algorithm>

namespace soew {
struct LocalCoopConfig {
    unsigned count=2;
    int stage=0,rank=1,lives=2,bombs=3;
    std::array<int,3> shots{0,1,2};
};
struct CoopSeat {
    PlayerMotion player;
    PlayerLife life;
    PlayerShots shots;
    PickupCore pickups;
    std::size_t life_effects=0,shot_sounds=0;
};
struct CoopRescue {
    unsigned presses=0,age=0;
    int target=-1;
};
struct LocalCoopState {
    LocalCoopConfig config;
    std::array<CoopSeat,2> extra;
    std::array<std::uint16_t,3> input{},previous{};
    std::array<CoopRescue,3> rescue{};
    std::array<bool,3> half_power{},fire_held{};
    unsigned bomb_owner=0;
    bool hit_clear=false;
    static constexpr PlayerLife::Point spawn(unsigned seat){return {seat==0?176:seat==1?240:208,336};}
    static void give_life(PlayerLife &life){if(life.lives<5)++life.lives;}
    static void spawn_at(PlayerMotion &player,PlayerLife &life,unsigned seat){
        const auto p=spawn(seat);
        player=PlayerMotion(player.shot);player.x=p.x;player.y=p.y;
        life.positions={p,p};life.current=life.draw=p;life.pattern=0;life.respawn_x=p.x;
    }
    static void revive(PlayerMotion &player,PlayerLife &life,unsigned seat){
        life.game_over=life.hit=life.miss_active=life.bomb_invincible=false;
        life.bombs=1;life.miss_frame=0;life.invincibility=150;
        spawn_at(player,life,seat);
    }
};
}
