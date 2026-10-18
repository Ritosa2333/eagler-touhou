#pragma once
#include "state_layout.hpp"
#include "../runtime_core/callback_owner.hpp"
namespace th20::source::sprite {struct AnimationFile;}
namespace th20::source::player_entity {
class Player;
class PlayerServices {
public:
    virtual ~PlayerServices()=default;
    virtual scheduler::State& scheduler()=0;
    virtual scheduler::Environment& scheduler_environment()=0;
    virtual bool preserve_animation_files()=0;                //474e00 session flags bit0
    virtual void unload_animation_file(int,bool preserve)=0; //44c430/4863f0
    virtual void delete_animation(std::uint32_t&)=0;           //44fcd0
    virtual void destroy_animation(sprite::Animation&)=0;     //449370
    virtual void retire_shot(Shot&)=0;                       //506af0
};
#pragma pack(push,4)
class Player final:public runtime::CallbackOwner {
public:
    std::int32_t state;
    std::uint32_t entity_flags,handle_18;
    sprite::AnimationFile* animation_file;
    std::uint32_t field_20,field_24;
    sprite::Animation animation;
    std::uint32_t handle_60c,handle_610;
    sprite::Vec3 position_614;
    Fixed2 fixed_position;
    sprite::Vec2 vectors_628[2];
    sprite::Vec3 vector_638;
    recovered::Timer timers_644[3];
    std::uint32_t fields_674[4];
    Option options[10],secondary_options[12];
    std::uint8_t focused_204c,padding_204d[3];
    recovered::Timer timers_2050[3];
    std::uint32_t fields_2080[5];
    float normal_radius,focus_radius;
    sprite::Vec3 normal_extent,focus_extent;
    std::int32_t speeds_20b4[4];
    sprite::Vec3 vector_20c4,vector_20d0;
    sprite::Vec2 vector_20dc;
    std::uint32_t fields_20e4[3];
    sprite::Vec3 vector_20f0;
    sprite::Vec2 vectors_20fc[33];
    std::uint32_t field_2204;
    void* shot_data;
    sprite::Interpolation<float> interpolation_220c;
    float collision_expansion,clock_scale;
    std::uint32_t field_2240;
    Feedback feedback;
    std::uint32_t animation_scripts[5];
    ShotController shots;
    std::uint32_t handle_1484c,field_14850;
    std::int32_t view_index;
    game_session::Context* context;
    PlayerServices* services;                                //source-only suffix
    explicit Player(PlayerServices&);                       //4f46a0
    ~Player() override;                                     //4f5320
};
#pragma pack(pop)
static_assert(offsetof(Player,animation)==0x28&&offsetof(Player,position_614)==0x614);
static_assert(offsetof(Player,options)==0x684&&offsetof(Player,secondary_options)==0x123c);
static_assert(offsetof(Player,focused_204c)==0x204c&&offsetof(Player,timers_2050)==0x2050);
static_assert(offsetof(Player,normal_radius)==0x2094&&offsetof(Player,normal_extent)==0x209c);
static_assert(offsetof(Player,shot_data)==0x2208&&offsetof(Player,feedback)==0x2244&&offsetof(Player,shots)==0x22b4);
static_assert(offsetof(Player,services)==0x1485c);
void construct_player_fields(Player&) noexcept;               //4f46a0 fields after original base construction
void clear_shots(ShotController&,PlayerServices&);            //504950
extern void* retained_shot_data;                            //actual zero-initialized BSS5c60f0
PlayerServices& player_services();
}
