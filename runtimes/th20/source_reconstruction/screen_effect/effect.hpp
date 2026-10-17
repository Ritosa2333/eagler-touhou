#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../sprite_renderer/sprite.hpp"
#include "../../native_recovered/native_core.hpp"
namespace th20::source::screen {
#pragma pack(push,4)
class Effect final : public runtime::CallbackOwner {
public:
    std::int32_t mode,field_14,alpha,duration;
    std::uint32_t argument_20,argument_24,argument_28;
    std::int32_t phase;
    recovered::Timer timer;
    std::int32_t view_index;
    Effect(); //4233b0
    ~Effect() override; //4234a0
    void initialize(int mode,int duration,std::uint32_t a,std::uint32_t b,std::uint32_t c,int priority); //423bf0
};
#pragma pack(pop)
static_assert(sizeof(Effect)==0x44&&offsetof(Effect,timer)==0x30);
Effect* create_effect(int mode,int duration,std::uint32_t a,std::uint32_t b,std::uint32_t c,int priority,int view_index=0); //4250f0/4250c0
int update_fade_out(Effect&); //4240c0 modes0/3
int update_fade_in(Effect&); //424260 modes2/5
int update_hold(Effect&); //424180 modes6/7
int update_flashes(Effect&); //424350 mode4
int update_solid(Effect&); //424460 mode9
int update_linear_shake(Effect&); //4249f0 mode1
int update_envelope_shake(Effect&); //4244c0 mode8
int draw_display(Effect&); //424e00 modes0/5
int draw_playfield(Effect&); //424e80 mode7
int draw_current_view(Effect&); //424f30 modes2/3/6/9
int draw_offset_playfield(Effect&); //424fa0 mode4
void draw_rectangle(sprite::Controller&,Device&,const float (&bounds)[4],std::uint32_t color); //423650
namespace environment {
bool cancelled(); //5ba518
const std::uint32_t* game_flags(); //nullable5ba828+e8
const float* timer_rate(); //5aefe0
std::uint32_t random_direction(); //423ea0(RNG5ba4c4,3)
void apply_shake(int mode,int view_index,unsigned axis,std::uint32_t direction,float amplitude); //original four-camera writes
}
}
