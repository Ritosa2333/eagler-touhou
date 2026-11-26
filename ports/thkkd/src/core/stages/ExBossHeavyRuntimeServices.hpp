#pragma once
#include "core/stages/ExBossHeavyServices.hpp"
#include <functional>
#include <stdexcept>
namespace kaikeidou {
template<class Host> struct ExBossHeavyRuntimeServices final:ExBossHeavyServices {
    Host& host;
    std::function<std::uint32_t(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)> circle;
    std::function<std::uint32_t(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)> line;
    explicit ExBossHeavyRuntimeServices(Host& value):host(value){}
    std::uint32_t color(std::uint32_t r,std::uint32_t g,std::uint32_t b)override{return host.color(r,g,b);}
    std::uint32_t draw_circle(std::uint32_t x,std::uint32_t y,std::uint32_t radius,std::uint32_t ink,std::uint32_t filled)override{
        if(!circle)throw std::runtime_error("EX heavy circle stream missing");return circle(x,y,radius,ink,filled);
    }
    std::uint32_t draw_line(std::uint32_t x1,std::uint32_t y1,std::uint32_t x2,std::uint32_t y2,std::uint32_t ink,std::uint32_t width)override{
        if(!line)throw std::runtime_error("EX heavy line stream missing");return line(x1,y1,x2,y2,ink,width);
    }
};
}
