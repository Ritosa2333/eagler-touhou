// EX中段绘制转交连续观察流，其余服务使用共同Boss状态。
#pragma once
#include "core/stages/ExBossMiddleTimelineServices.hpp"
#include <functional>
#include <stdexcept>
namespace kaikeidou {
template<class Host> struct ExBossMiddleTimelineRuntimeServices final:ExBossMiddleTimelineServices {
    Host& host;
    std::function<void(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)> line;
    explicit ExBossMiddleTimelineRuntimeServices(Host& services):host(services){}
    std::int32_t allocate_boss(std::uint32_t kind)override{return host.allocate_boss(kind);}
    void configure_phase(std::uint32_t spell,std::uint32_t seconds,std::uint32_t hp,std::uint32_t index)override{host.configure_phase(spell,seconds,hp,index);}
    void begin_boss_effect()override{host.begin_boss_effect();}
    void retire_boss(std::uint32_t mode)override{host.retire_boss(mode);}
    std::uint32_t color(std::uint32_t red,std::uint32_t green,std::uint32_t blue)override{return host.color(red,green,blue);}
    void draw_line(std::uint32_t x1,std::uint32_t y1,std::uint32_t x2,std::uint32_t y2,std::uint32_t color_value,std::uint32_t thickness)override{
        if(!line)throw std::runtime_error("EX line drawing requires the original platform stream");
        line(x1,y1,x2,y2,color_value,thickness);
    }
};
}
