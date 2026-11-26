// 仅平台时钟、绘图和既有菜单主体跨越服务边界。
#pragma once
#include "core/frontend/MenuStartServices.hpp"
#include "core/frontend/MenuPresentationContext.hpp"
namespace kaikeidou {
struct MenuPresentationServices:MenuStartServices {
 virtual void brightness(std::uint32_t red,std::uint32_t green,std::uint32_t blue)=0;
 virtual void draw_graph(std::uint32_t x,std::uint32_t y,std::uint32_t graph,std::uint32_t transparent)=0;
 virtual std::uint32_t platform_random(std::uint32_t maximum)=0;
 virtual std::uint32_t text_width(std::uint32_t font,std::string_view text)=0;
 virtual MenuPresentationRegisters draw_formatted(std::uint32_t x,std::uint32_t y,std::uint32_t ink,std::string_view format,bool has_number,std::uint64_t number)=0;
 virtual std::uint32_t now_count(std::uint32_t use_performance_counter)=0;
 virtual void sleep_ms(std::uint32_t milliseconds)=0;
 virtual void menu_body(std::uint32_t entry)=0;
 // B41260至跳回5E9FC4，主体由Replay UI模块提供。
 virtual void replay_ui_route()=0;
};
}
