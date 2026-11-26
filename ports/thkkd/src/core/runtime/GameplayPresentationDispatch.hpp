// 原61D690..61D8BD完整分派；显示子函数保持服务边界，结局白幕复用已恢复实现。
#pragma once
#include "core/runtime/GameplayPresentationDispatchContext.hpp"
#include "core/runtime/GameplayPresentationDispatchServices.hpp"
namespace kaikeidou {
namespace gameplay_presentation_dispatch_detail {
inline std::uint64_t load_store_position(std::uint64_t bits){
    if((bits&0x7fffffffffffffffull)>0x7ff0000000000000ull)bits|=0x0008000000000000ull;
    return bits;
}
}
inline void dispatch_gameplay_presentation(GameplayPresentationDispatchContext& context,
    ObjectRecord<420>& ending,GameplayPresentationDispatchServices& services){
    using Component=GameplayPresentationComponent;
    if(context.tint!=255)services.tint_color(context.tint,context.tint,context.tint);
    services.present_component(Component::routine_611db0);
    if(context.scene!=1||context.practice!=0){
        if(context.tint!=255)services.tint_color(context.tint,context.tint,context.tint);
        services.present_component(Component::routine_6128f0);
        services.present_component(Component::routine_618c30);
        services.present_component(Component::routine_61d0f0);
        services.present_component(Component::routine_619b80);
        if(context.tint!=255)services.tint_color(254,254,254);
        services.present_component(Component::routine_65cd00);
        services.present_component(Component::routine_619a80);
        if(context.tint!=255)services.tint_color(context.tint,context.tint,context.tint);
        services.present_component(Component::routine_61d450);
        services.present_component(Component::routine_612480);
        if(context.tint!=255)services.tint_color(254,254,254);
        if(services.input_count(context.slow_binding)>0){
            const auto graph=context.slow_graph;
            const auto y=gameplay_presentation_dispatch_detail::load_store_position(context.player_y_bits);
            const auto x=gameplay_presentation_dispatch_detail::load_store_position(context.player_x_bits);
            services.draw_slow_marker(x,y,0x3ff0000000000000ull,0,graph,1,0);
        }
    }
    if(context.tint!=255)services.tint_color(254,254,254);
    services.present_component(Component::routine_644bb0);
    services.present_component(Component::routine_613560);
    services.present_component(Component::routine_65cc50);
    services.present_component(Component::routine_613250);
    services.present_component(Component::routine_61d180);
    services.present_component(Component::routine_611ff0);
    services.draw_fps(572,465);
    present_ending_fade(ending,services);
}
}
