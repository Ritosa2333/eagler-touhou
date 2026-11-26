// EX后段复用Boss/音乐/背景状态，标题返回由独立尾协议绑定。
#pragma once
#include "core/stages/ExBossLateTimelineServices.hpp"
#include <functional>
#include <stdexcept>
namespace kaikeidou {
template<class Host> struct ExBossLateTimelineProbeServices final:ExBossLateTimelineServices {
    Host& host;
    std::function<void(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)> line;
    std::function<void(bool)> return_title;
    std::function<void()> title_visuals;
    explicit ExBossLateTimelineProbeServices(Host& value):host(value){}
    std::int32_t allocate_boss(std::uint32_t kind)override{return host.allocate_boss(kind);}
    void configure_phase(std::uint32_t spell,std::uint32_t seconds,std::uint32_t hp,std::uint32_t index)override{host.configure_phase(spell,seconds,hp,index);}
    void begin_boss_effect()override{host.begin_boss_effect();}
    void retire_boss(std::uint32_t mode)override{host.retire_boss(mode);}
    std::uint32_t color(std::uint32_t r,std::uint32_t g,std::uint32_t b)override{return host.color(r,g,b);}
    void dialog(std::uint32_t mode)override{host.dialog(mode);}
    void award_stage_clear()override{host.award_stage_clear();}
    void pause_background()override{host.pause_background();}
    void stop_music()override{
        auto& state=host.state;
        state.presentation.begin(state.resource_tick,host.timeline.frame,1,3,{},&state.music,sizeof(state.music));
        host.stop_sound(state.music.audio_handle);
        state.presentation.end(&state.music,sizeof(state.music));
    }
    void return_to_title(bool replay_variant)override{
        if(!return_title)throw std::runtime_error("EX return requires title initial-state protocol");
        return_title(replay_variant);
    }
    void initialize_title_visuals()override{
        if(!title_visuals)throw std::runtime_error("EX title initialization requires captured platform returns");
        title_visuals();
    }
    void draw_line(std::uint32_t x1,std::uint32_t y1,std::uint32_t x2,std::uint32_t y2,std::uint32_t color_value,std::uint32_t thickness)override{
        if(!line)throw std::runtime_error("EX late line drawing requires platform stream");
        line(x1,y1,x2,y2,color_value,thickness);
    }
};
}
