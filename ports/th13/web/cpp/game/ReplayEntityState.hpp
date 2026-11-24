#pragma once

#include "ItemManager.hpp"
#include "StageBattle.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace th13 {

struct ReplayEntityState {
    struct Enemy {
        u32 serial = 0, trace_id = 0;
        i32 script_id = 0;
        u32 x_bits = 0, y_bits = 0;
        u32 life_bits = 0;
        bool dead = false;
        i32 hit_anim_timer = 0;
        u32 spirit_accum_bits = 0, spirit_threshold_bits = 0;
        u32 vel_pos_bits[3] = {}, pos_pos_bits[3] = {};
        u32 vel_aux_bits[3] = {}, pos_aux_bits[3] = {};
        u32 vel_angle_bits = 0, vel_speed_bits = 0, vel_mode = 0;
        u32 pos_angle_bits = 0, pos_speed_bits = 0, pos_mode = 0;
    };
    struct Bullet {
        u32 index = 0, generation = 0, source_enemy = 0;
        u16 state = 0;
        u32 x_bits = 0, y_bits = 0, angle_bits = 0;
        bool alive = false;
        u32 z_bits = 0, velocity_x_bits = 0, velocity_y_bits = 0, velocity_z_bits = 0;
        u32 speed_bits = 0, sprite = 0, subtype = 0, behavior_flags = 0;
        i32 pool_slot = -1;
    };
    struct Laser {
        u32 index = 0, generation = 0;
        i32 id = 0, phase = 0, time = 0;
        u32 origin_x_bits = 0, origin_y_bits = 0, length_bits = 0;
        bool active = false;
    };
    struct Shot {
        u32 index = 0, generation = 0;
        i32 age = 0, state = 0;
        u32 x_bits = 0, y_bits = 0, damage_bits = 0;
        bool alive = false;
        u32 angle_bits = 0, speed_bits = 0, width_bits = 0, height_bits = 0;
        u32 damage_x_bits = 0, damage_y_bits = 0;
        i32 behavior = 0, type = 0, anchor = 0;
    };
    struct Item {
        u32 slot = 0, serial = 0;
        i32 state = 0, kind = 0;
        u32 x_bits = 0, y_bits = 0;
        bool alive = false;
    };
    struct Registration {
        u32 slot = 0, generation = 0, flags = 0;
        u32 owner = 0, radius_bits = 0, radius_growth_bits = 0;
        u32 angle_bits = 0, width_bits = 0, height_bits = 0;
        u32 center_x_bits = 0, center_y_bits = 0, center_z_bits = 0;
        i32 previous = 0, current = 0;
        u32 current_f_bits = 0, damage_bits = 0;
        i32 accumulated_damage = 0, threshold = 0, interval = 0;
    };
    u32 frame = 0;
    i32 player_phase = 0, player_phase_frame = 0;
    u32 player_x_bits = 0, player_y_bits = 0;
    i32 bomb_timer = 0, boss_timer_seconds = -1, boss_timer_centiseconds = 0;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    std::vector<Laser> lasers;
    std::vector<Shot> shots;
    std::vector<Item> items;
    std::vector<Item> spirits;
    std::vector<Registration> damage_registrations;
    std::string json() const;
};

ReplayEntityState sample_replay_entities(const StageBattle& battle);

}
