#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../sprite_renderer/sprite.hpp"
#include "../game_session/session.hpp"
namespace th20::source::stage_clear {
struct StageClearInf final : runtime::CallbackOwner {
    sprite::AnimationFile* file;                   //10
    std::uint32_t panel_handle,secondary_handle;   //14,18
    std::uint32_t field_1c,kind,state,bonus;        //1c..28
    recovered::Timer age,secondary_age;            //2c,3c
    std::uint32_t field_4c,fields_50[16],fields_90[2];
    StageClearInf();                              //510980
    ~StageClearInf() override;                    //510a80
};
static_assert(sizeof(StageClearInf)==0x98 && offsetof(StageClearInf,age)==0x2c && offsetof(StageClearInf,fields_90)==0x90);
StageClearInf* controller();                      //actual5c6114 shared with startup
StageClearInf* create(int kind);                  //5115c0
int initialize(StageClearInf&,int kind);          //511270
int update(StageClearInf&);                       //510b60
int draw(StageClearInf&);                         //510e80
int clamped_level(game_session::Player&,unsigned slot); //5113c0/511320/511410/511370
int clamped_phase(game_session::Player&,unsigned slot); //5114f0/511460/511530/5114a0
std::uint32_t calculate_bonus(game_session::Player&,int stage); //510b60 arithmetic and mutating getters
}
