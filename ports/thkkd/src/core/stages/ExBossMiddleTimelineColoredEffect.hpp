#pragma once
#include "core/stages/Stage6BossTimelineColoredEffect.hpp"
namespace kaikeidou {
inline std::int32_t ex_boss_middle_timeline_colored_effect(EffectPool& effects,Effect& prefix,std::array<std::uint32_t,100>& sounds,
    std::uint32_t kind,std::uint32_t color,std::uint32_t hp,std::uint32_t behavior,std::uint64_t x,std::uint64_t y,BossStageServices& services){
    struct Adapter final:StageBossServices{
        BossStageServices& source;explicit Adapter(BossStageServices& s):source(s){}
        std::uint32_t color(std::uint32_t r,std::uint32_t g,std::uint32_t b)override{return source.color(r,g,b);}
        std::int32_t allocate_boss(std::uint32_t)override{throw std::logic_error("unused colored effect service");}
        void configure_phase(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)override{throw std::logic_error("unused colored effect service");}
        void begin_boss_effect()override{throw std::logic_error("unused colored effect service");}
        void retire_boss(std::uint32_t)override{throw std::logic_error("unused colored effect service");}
        void dialog(std::uint32_t)override{throw std::logic_error("unused colored effect service");}
        void award_stage_clear()override{throw std::logic_error("unused colored effect service");}
        void change_stage_resources(std::uint32_t)override{throw std::logic_error("unused colored effect service");}
        void restore_stage_music(std::uint32_t)override{throw std::logic_error("unused colored effect service");}
    } adapter(services);
    return stage6_boss_colored_effect(effects,prefix,sounds,kind,color,hp,behavior,x,y,adapter);
}
}
