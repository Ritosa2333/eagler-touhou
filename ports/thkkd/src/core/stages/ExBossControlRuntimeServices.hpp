// 普通189的圆形绘图连接同一有序平台观察流。
#pragma once
#include "core/stages/ExBossControlServices.hpp"
#include <functional>
#include <stdexcept>
namespace kaikeidou {
struct ExBossControlRuntimeServices final:ExBossControlServices {
    std::function<std::uint32_t(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)> circle;
    std::uint32_t draw_circle(std::uint32_t x,std::uint32_t y,std::uint32_t radius,std::uint32_t color,std::uint32_t filled)override{
        if(!circle)throw std::runtime_error("EX circle drawing requires the original platform stream");
        return circle(x,y,radius,color,filled);
    }
};
}
