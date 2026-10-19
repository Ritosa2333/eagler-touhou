#pragma once
#include "Player.hpp"
#include "AnmCheckpoint.hpp"
namespace th15 {
// Only the original chapter-owned player progress is rewound. Root pose,
// collision bounds, external services and shot callbacks stay live.
class PlayerCheckpoint {
    struct Option {i32 active,index,snap;PlayerFixedPosition target,position,normal_offset,focus_offset;};
    struct State {
        Vec3 position,velocity,last_direction,external_velocity;PlayerFixedPosition fixed,step;
        i32 normal_speed,focus_speed,normal_diagonal,focus_diagonal,focus,option_count,follow,collapse,state,power_level,damage_cursor;
        u32 behavior_flags;float movement_scale;
        Timer life_age,invulnerability,input_age,ready_age,shot_age,continuous_age,barrier_age;
        std::array<Option,8> options;std::array<float,8> option_angles;std::array<i32,5> laser_power;
        std::array<DamageSource,256> damage;std::array<PlayerShot,256> shots;
        std::array<std::array<u32,2>,8> option_handles;u32 focus_handle,barrier_handle;
    } saved;
    Player& player;AnmRegistry& registry;AnmCheckpoint& animations;bool available=false;
    bool retire(u32);u32 save_animation(u32);u32 restore_animation(u32);
public:
    std::string error;
    PlayerCheckpoint(Player& p,AnmRegistry& r,AnmCheckpoint& a):player(p),registry(r),animations(a){}
    bool capture();bool restore();bool ready()const noexcept{return available;}
    bool write_file(std::vector<u8>&,AnmManager&);
    bool read_file(const u8*,u32,u32& consumed,AnmManager&);
    const PlayerShot& shot(u32 i)const noexcept{return saved.shots[i];}
    u32 option_handle(u32 i,u32 k)const noexcept{return saved.option_handles[i][k];}
    u32 focus_handle()const noexcept{return saved.focus_handle;}u32 barrier_handle()const noexcept{return saved.barrier_handle;}
};
}
