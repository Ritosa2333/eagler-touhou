#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../sprite_renderer/sprite.hpp"
namespace th20::source::player_entity {struct Feedback;}
namespace th20::source::hud {
struct Dialogue;
struct Pair {std::uint32_t first=0,second=0;}; //4aece0
struct PanelState { //4aeb40,54 bytes
    std::uint32_t fields_00[4]{};
    Pair pairs[4];
    std::uint32_t handles[7]{};
    std::uint32_t field_4c=0,field_50=0;
};
static_assert(sizeof(PanelState)==0x54);
struct FrontInf final:runtime::CallbackOwner { //4aef90, vtable57062c
    std::uint32_t life_handles[7],bomb_handles[7],number_handles[2]; //10,2c,48
    sprite::Animation* life_animations[7]; //50
    sprite::Animation* bomb_animations[7]; //6c
    sprite::Animation* number_animations[2]; //88
    std::uint32_t handle_90,score_handles[10],notice_handles[3]; //90,94,bc
    std::uint32_t handle_c8,handle_cc,handles_d0[10];
    std::uint32_t handles_f8[9]; //f8..118
    std::uint32_t fields_11c[8]; //11c..138
    std::uint32_t background_handle; //13c
    recovered::Timer age; //140
    scheduler::Node* secondary_draw; //150
    std::uint32_t field_154; //not touched by constructor
    std::uint32_t field_158,field_15c;
    std::uint64_t score; //160
    std::uint32_t field_168,field_16c;
    sprite::AnimationFile* stage_file; //170
    std::uint32_t fields_174[4];
    Pair pairs[4]; //184
    std::uint32_t flags; //1a4; constructor clears low14 bits only
    recovered::Timer secondary_age; //1a8
    std::uint32_t field_1b8;
    Dialogue* collecting; //1bc; active dialogue also triggers Item collection
    std::uint8_t* message_data; //1c0
    std::int32_t message_index; //1c4
    std::uint32_t field_1c8;
    std::int32_t field_1cc;
    PanelState panels[3]; //1d0
    sprite::AnimationFile* front_file; //2cc, not touched by constructor
    std::uint32_t field_2d0,field_2d4;
    FrontInf();
    ~FrontInf() override; //4afaa0
    void enable_callbacks() override; //4b5bb0
};
static_assert(sizeof(FrontInf)==0x2d8&&offsetof(FrontInf,life_animations)==0x50&&offsetof(FrontInf,age)==0x140&&offsetof(FrontInf,flags)==0x1a4&&offsetof(FrontInf,panels)==0x1d0&&offsetof(FrontInf,front_file)==0x2cc);
extern FrontInf* controller; //5c06a4
void set_lives(FrontInf&,int,int,int); //4b8bf0
void set_bombs(FrontInf&,int,int,int); //4b8650
void notify(FrontInf&,int,int); //4b90e0
int update(FrontInf&); //4b2c90
int draw(FrontInf&); //4b4730
int draw_player(); //4b5790
void draw_feedback(player_entity::Feedback&); //4f8840
int initialize(FrontInf&); //4b5820
int initialize_stage(FrontInf&); //4b5960
void clear(FrontInf&); //4b6220
FrontInf* create(); //4b9090
bool load_shared(); //4b5920
void unload_shared(); //4b6560
namespace environment {
sprite::Controller& sprites();
sprite::AnimationFile& notice_file(); //TextRenderer +1a244 (437950)
}
}
