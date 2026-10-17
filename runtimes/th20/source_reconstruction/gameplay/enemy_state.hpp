#pragma once
#include "enemy_frame.hpp"
#include <array>
#include <forward_list>
#include <memory>
namespace th20::source::gameplay {
// Shared72-byte motion state, constructor478530 writes all18 words to zero.
struct EnemyMotion {std::uint32_t words[18];};
struct EnemyMotionInterpolation {
    sprite::Vec3 current,start,end,tangent_start,tangent_end;
    recovered::Timer timer;                                 //3c
    std::int32_t duration,axis_modes[3],mode;                //4c,50,5c
    std::uint32_t flags;                                    //60: bit0 enables per-axis modes
}; //48b270,0x64;4a8d70 proves this differs from ANM Interpolation<Vec3>
static_assert(sizeof(EnemyMotionInterpolation)==0x64&&offsetof(EnemyMotionInterpolation,timer)==0x3c&&offsetof(EnemyMotionInterpolation,mode)==0x5c);
struct EnemyMovementRecord {
    EnemyMotion motion;
    EnemyMotionInterpolation position;
    sprite::Interpolation<float> scalar_ac,scalar_d8;
    sprite::Interpolation<sprite::Vec2> vector_104,vector_144;
    EnemyMovementRecord() noexcept;                          //489150 zero +48b550
};
static_assert(sizeof(EnemyMovementRecord)==0x184);
// State+168 is a pmr singly linked list. Nodes have stride0x50 (498020).
// Their first8-byte shared owner is destroyed by48b0a0->47c300->484570;
// remaining value bytes have no additional destructor behavior in that chain.
struct EnemyQueuedRecord {std::shared_ptr<void> owner;std::array<std::uint8_t,0x44> values{};}; //489110 zeroes all4c bytes,48b500 constructs ownership/parameters
static_assert(sizeof(EnemyQueuedRecord)==0x4c);
struct EnemyAuxiliaryRecord {std::array<std::uint32_t,0x88/4> values;};
struct EnemyAuxiliary28 {std::uint32_t words[7];};
struct EnemyPatternState {
    std::uint32_t fields_00[35];
    std::uint32_t field_8c;
    recovered::Timer timer_90;
    float field_a0,field_a4;
};
static_assert(sizeof(EnemyPatternState)==0xa8);
#pragma pack(push,4)
// Actual subobject at Enemy+88; constructor4a3060, destructor4a3ac0.
// Unknown scalar fields retain exact storage, without default game behavior.
struct EnemyState {
    std::uint32_t identifier,field_04;
    void* entity;
    std::pmr::vector<EnemyAnimationLink> animations;
    std::uint32_t fields_1c[16];
    sprite::Vec2 bounds_5c,bounds_64;
    sprite::Vec3 vector_6c;
    std::uint32_t fields_78[12];
    recovered::Timer timer_a8,timer_b8;
    EnemyMotion motion_c8,motion_110;
    std::pmr::vector<EnemyMovementRecord> movements;           //158
    std::pmr::forward_list<EnemyQueuedRecord> queued;           //168
    sprite::Vec2 vector_170;
    std::uint32_t fields_178[4],field_188;
    EnemyAuxiliary28 auxiliary_18c;
    EnemyPatternState pattern_1a8;
    std::uint32_t fields_250[14];
    recovered::Timer timer_288,timer_298,timer_2a8;
    std::pmr::vector<EnemyAuxiliaryRecord> auxiliary;           //2b8
    std::uint32_t fields_2c8[10];
    EnemyState() noexcept;                                   //4a3060
    // Default C++ member destruction has precisely4a3ac0's reverse ownership order.
    void initialize();                                       //4a7170
};
#pragma pack(pop)
static_assert(sizeof(EnemyState)==0x2f0);
static_assert(offsetof(EnemyState,animations)==0xc&&offsetof(EnemyState,movements)==0x158);
static_assert(offsetof(EnemyState,queued)==0x168&&offsetof(EnemyState,pattern_1a8)==0x1a8);
static_assert(offsetof(EnemyState,auxiliary)==0x2b8&&offsetof(EnemyState,fields_2c8)==0x2c8);
void reset_enemy_auxiliary(EnemyAuxiliary28&) noexcept;       //4a7310, preserves words3/4 and other flags
void reset_enemy_pattern(EnemyPatternState&) noexcept;        //4a7360
}
