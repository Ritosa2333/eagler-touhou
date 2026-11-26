#pragma once
#include "core/entities/BossStageContext.hpp"
#include "core/player/PlayerShotFireContext.hpp"
namespace kaikeidou {
struct ExBossEarlyTimelineServices:BossStageServices{
    virtual void dialog(std::uint32_t mode)=0;
    virtual void draw_pixel(std::uint32_t x_bits,std::uint32_t y_bits,std::uint32_t color)=0;
};
inline std::uint32_t ex_boss_early_timeline_input(std::uint32_t binding,const PlayerShotFireContext& fire,PlayerShotFireInput& input){
    if(fire.replay_variant&&fire.replay_mode){const auto bit=binding==11?7u:binding;return bit<32?((fire.replay_buttons>>bit)&1u):0u;}
    return input.read_counter(binding);
}
}
