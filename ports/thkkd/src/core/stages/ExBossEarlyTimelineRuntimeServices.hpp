// EX关底时间轴到既有Boss服务的适配，像素绘制需绑定独立平台观察流。
#pragma once
#include "core/stages/ExBossEarlyTimelineServices.hpp"
#include <functional>
#include <stdexcept>
namespace kaikeidou {
template<class Host> struct ExBossEarlyTimelineRuntimeServices final:ExBossEarlyTimelineServices {
    Host& host;
    std::function<void(std::uint32_t,std::uint32_t,std::uint32_t)> pixel;
    explicit ExBossEarlyTimelineRuntimeServices(Host& services):host(services){}
    std::int32_t allocate_boss(std::uint32_t kind)override{return host.allocate_boss(kind);}
    void configure_phase(std::uint32_t spell,std::uint32_t seconds,std::uint32_t hp,std::uint32_t index)override{host.configure_phase(spell,seconds,hp,index);}
    void begin_boss_effect()override{host.begin_boss_effect();}
    void retire_boss(std::uint32_t mode)override{host.retire_boss(mode);}
    std::uint32_t color(std::uint32_t red,std::uint32_t green,std::uint32_t blue)override{return host.color(red,green,blue);}
    void dialog(std::uint32_t mode)override{host.dialog(mode);}
    void draw_pixel(std::uint32_t x,std::uint32_t y,std::uint32_t color_value)override{
        if(!pixel)throw std::runtime_error("EX pixel drawing requires the original platform stream");
        pixel(x,y,color_value);
    }
};
}
