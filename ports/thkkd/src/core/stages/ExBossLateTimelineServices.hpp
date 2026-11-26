#pragma once
#include "core/entities/BossStageContext.hpp"
#include "core/player/PlayerShotFireContext.hpp"
namespace kaikeidou {
struct ExBossLateTimelineServices:BossStageServices{
    virtual void draw_line(std::uint32_t x1,std::uint32_t y1,std::uint32_t x2,std::uint32_t y2,std::uint32_t color,std::uint32_t thickness)=0;
    virtual void dialog(std::uint32_t mode)=0;
    virtual void award_stage_clear()=0;
    virtual void pause_background()=0;
    virtual void stop_music()=0;
    virtual void return_to_title(bool replay_variant)=0;
    virtual void initialize_title_visuals()=0;
};
inline std::uint32_t ex_boss_late_timeline_input(std::uint32_t binding,const PlayerShotFireContext& fire,PlayerShotFireInput& input){
    if(fire.replay_variant&&fire.replay_mode){const auto bit=binding==11?7u:binding;return bit<32?((fire.replay_buttons>>bit)&1u):0u;}return input.read_counter(binding);
}
}
