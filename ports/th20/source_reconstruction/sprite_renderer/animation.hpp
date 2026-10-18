#pragma once
#include <cstddef>
#include <cstdint>
#include "../../native_recovered/native_core.hpp"
namespace th20::source::sprite {
struct Vec2 { float x,y; };
struct Vec3 { float x,y,z; };
struct Vec3i { std::int32_t x,y,z; };
struct Matrix4 { float elements[16]; };
template<class T> struct Interpolation {
    T start,end,tangent_start,tangent_end,current;
    th20::recovered::Timer timer;
    std::int32_t duration,mode;
};
#pragma pack(push,1)
struct AnimationBase {                          // complete 0x4c0-byte base storage
    th20::recovered::Timer timer;
    std::uint32_t fields_10_28[7];
    Vec3 vector_2c,vector_38,vector_44;
    Vec2 vector_50,vector_58,vector_60,vector_68,vector_70;
    std::uint32_t field_78,field_7c;
    Vec3 vector_80;
    Interpolation<Vec3> interpolation_8c;
    Interpolation<Vec3i> interpolation_e0;
    Interpolation<std::int32_t> interpolation_134;
    Interpolation<Vec3> interpolation_160;
    Interpolation<float> interpolation_1b4;
    Interpolation<Vec2> interpolation_1e0,interpolation_220,interpolation_260;
    Interpolation<Vec3i> interpolation_2a0;
    Interpolation<std::int32_t> interpolation_2f4;
    Interpolation<float> interpolation_320,interpolation_34c;
    Vec2 vectors_378[4],vector_398;
    std::uint32_t fields_3a0[6];
    Matrix4 matrix_3b8,matrix_3f8;
    std::uint32_t field_438,field_43c;
    std::uint16_t field_440;
    std::uint8_t padding_442[2];
    std::uint32_t fields_444[16];
    Vec3 vector_484;
    std::uint32_t field_490,field_494,flags[8],field_4b8,field_4bc;
};
struct Animation;
struct AnimationLink {
    Animation* value;
    AnimationLink* next;
    AnimationLink* previous;
    void* owner;
    void* iterator;
};
struct Animation {                              // complete 0x5e4-byte VM storage
    AnimationBase base;
    std::uint32_t handle,index;
    th20::recovered::Timer timer_4c8,timer_4d8;
    std::uint32_t field_4e8;
    AnimationLink links[5];
    std::uint32_t fields_550[10];
    std::uint8_t field_578,field_579,padding_57a[2];
    Matrix4 matrix_57c;
    Vec3 vector_5bc;
    std::uint32_t field_5c8,field_5cc;
    Vec3 vector_5d0;
    std::uint32_t field_5dc,field_5e0;
};
struct PooledAnimation {
    Animation animation;
    AnimationLink free_link;
    std::uint8_t active,padding_5f9[3];
    std::uint32_t index;
};
#pragma pack(pop)
static_assert(sizeof(Interpolation<float>)==44);
static_assert(sizeof(Interpolation<Vec2>)==64);
static_assert(sizeof(Interpolation<Vec3>)==84);
static_assert(offsetof(AnimationBase,matrix_3b8)==0x3b8);
static_assert(offsetof(AnimationBase,field_440)==0x440);
static_assert(offsetof(AnimationBase,flags)==0x498);
static_assert(sizeof(AnimationBase)==0x4c0);
static_assert(offsetof(Animation,links)==0x4ec);
static_assert(offsetof(Animation,fields_550)==0x550);
static_assert(offsetof(Animation,matrix_57c)==0x57c);
static_assert(sizeof(Animation)==0x5e4 && sizeof(PooledAnimation)==0x600);
void construct_animation_base(AnimationBase&) noexcept;     // 0x448d70
void construct_animation(Animation&) noexcept;              // 0x448b40
void construct_pooled_animation(PooledAnimation&) noexcept; // 0x4489f0
void reset_animation_state(Animation&) noexcept;             // 0x4299d0
void clear_animation_suffix(Animation&) noexcept;            // 0x429e30 -> 0x438a50/0x438a30
}
