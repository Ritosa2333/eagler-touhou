// 61D690分派的数值探针服务；持续状态由已恢复子模块推进。
#pragma once
#include "core/runtime/GameplayPresentationDispatchServices.hpp"
#include <functional>
namespace kaikeidou {
struct GameplayPresentationProbeServices final:GameplayPresentationDispatchServices {
    std::function<void(GameplayPresentationComponent)> component;
    std::function<std::int32_t(std::uint32_t)> input;
    EndingFadePresentationServices& ending;
    explicit GameplayPresentationProbeServices(EndingFadePresentationServices& services):ending(services){}
    void tint_color(std::uint32_t,std::uint32_t,std::uint32_t)override{}
    void present_component(GameplayPresentationComponent value)override{component(value);}
    std::int32_t input_count(std::uint32_t binding)override{return input(binding);}
    void draw_slow_marker(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,
        std::uint32_t,std::uint32_t,std::uint32_t)override{}
    void draw_fps(std::uint32_t,std::uint32_t)override{}
    void blend(std::uint32_t mode,std::uint32_t opacity)override{ending.blend(mode,opacity);}
    std::uint32_t color(std::uint32_t red,std::uint32_t green,std::uint32_t blue)override{return ending.color(red,green,blue);}
    void rectangle(std::uint32_t a,std::uint32_t b,std::uint32_t c,std::uint32_t d,std::uint32_t color_value,std::uint32_t fill)override{ending.rectangle(a,b,c,d,color_value,fill);}
};
}
