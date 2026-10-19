#pragma once
#include "player.hpp"
#include "../core_scheduler/scheduler.hpp"
#include "../runtime_state/motion.hpp"
namespace th20::source::player_entity {
struct ShotController;
struct Fixed2 {std::int32_t x,y;};
struct Option { //4f49c0, Player+684 ten records, +123c twelve records
    std::uint32_t state;
    sprite::Vec3 position,previous_position,vectors_1c[7];
    Fixed2 vector_70,vector_78,offsets_80[7];
    sprite::Vec2 vector_b8,vector_c0;
    sprite::Vec3 vector_c8;
    std::uint32_t field_d4,field_d8,handle_dc,handle_e0;
    recovered::Timer timer_e4;
    std::uint32_t fields_f4[14];
};
struct Shot { //4f4cb0, 256 records in Player+22b4
    scheduler::Link link;
    std::uint32_t flags,handle_18;
    recovered::Timer timer_1c,timer_2c;
    std::uint32_t fields_3c[5];
    state::Motion motion;
    std::uint32_t fields_98[6];
    sprite::Vec2 vector_b0;
    std::uint32_t fields_b8[18];
    sprite::Vec3 vector_100;
    std::uint32_t field_10c,field_110;
    std::uint8_t byte_114,padding_115[3];
    std::int32_t view_index;
    ShotController* owner;
    game_session::Context* context;
};
struct ShotController { //4f4b80, source of PlayerInf's fixed shot pool
    Shot pool[256];
    recovered::Timer timer_12400,timer_12410,timer_12420;
    scheduler::List active,free;
    std::uint32_t field_12460,field_12464,counters_12468[30],counters_124e0[30];
    std::uint32_t field_12558,field_1255c,handle_12560,field_12564;
    recovered::Timer timer_12568;
    std::uint32_t field_12578;
    std::uint8_t byte_1257c,padding_1257d[3];
    recovered::Timer timer_12580;
    std::int32_t view_index;
    game_session::Context* context;
};
struct Feedback { //4f4580, Player+2244, owns one ANM handle at+4c
    recovered::Timer timers[3];
    std::uint32_t fields_30[4];
    sprite::Vec3 vector_40;
    std::uint32_t handle_4c;
    std::uint8_t enabled,padding_51[3];
    std::int32_t view_index;
    game_session::Context* context;
};
static_assert(sizeof(Option)==0x12c&&offsetof(Option,handle_dc)==0xdc&&offsetof(Option,fields_f4)==0xf4);
static_assert(sizeof(Shot)==0x124&&offsetof(Shot,motion)==0x50&&offsetof(Shot,byte_114)==0x114);
static_assert(sizeof(ShotController)==0x12598&&offsetof(ShotController,active)==0x12430&&offsetof(ShotController,view_index)==0x12590);
static_assert(sizeof(Feedback)==0x5c&&offsetof(Feedback,enabled)==0x50);
void construct_option(Option&) noexcept;                     //4f49c0
void construct_shot(Shot&) noexcept;                         //4f4cb0
void construct_shot_controller(ShotController&) noexcept;     //4f4b80/4f41d0
void construct_feedback(Feedback&) noexcept;                 //4f4580
}
