// Omniverse复用同一Boss、音乐和标题状态；平台记录只用于调用核对。
#pragma once
#include "core/stages/OmniverseTimelineState.hpp"
#include "core/stages/OmniverseBehaviorContext.hpp"
#include "core/stages/OmniverseAcosRuntime.hpp"
#include <functional>
#include <stdexcept>
namespace kaikeidou {
template<class Host,class Title> struct OmniverseWorldRuntimeServices final:OmniverseTimelineServices,OmniverseBehaviorServices {
 Host& host;Title& title;
 std::function<void(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)> line;
 std::function<void(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)> circle;
 OmniverseWorldRuntimeServices(Host& h,Title& t):host(h),title(t){}
 std::int32_t allocate_boss(std::uint32_t kind)override{return host.allocate_boss(kind);}
 void configure_phase(std::uint32_t spell,std::uint32_t seconds,std::uint32_t hp,std::uint32_t index)override{host.configure_phase(spell,seconds,hp,index);}
 void begin_boss_effect()override{host.begin_boss_effect();}
 void retire_boss(std::uint32_t mode)override{host.retire_boss(mode);}
 std::uint32_t color(std::uint32_t r,std::uint32_t g,std::uint32_t b)override{return host.color(r,g,b);}
 void dialog(std::uint32_t mode)override{host.dialog(mode);}
 void award_stage_clear()override{host.award_stage_clear();}
 void change_stage_resources(std::uint32_t stage)override{host.change_stage_resources(stage);}
 void restore_stage_music(std::uint32_t mode)override{host.restore_stage_music(mode);}
 void music(std::uint32_t track,std::uint32_t mode)override{host.music(track,mode);}
 void pause_background()override{host.pause_background();}
 void stop_music()override{title.stop_music();}
 void return_to_title(bool variant)override{title.return_to_title(variant);}
 void initialize_title_visuals()override{title.initialize_title_visuals();}
 std::uint64_t acos_bits(std::uint64_t argument)override{return checked_omniverse_acos_bits(argument,1,0x1f80,0x7f);}
 void kill_enemy(std::uint32_t index)override{host.resolve_enemy_death(index);}
 void draw_line(std::uint32_t x1,std::uint32_t y1,std::uint32_t x2,std::uint32_t y2,std::uint32_t color_value,std::uint32_t thickness)override{
  if(!line)throw std::runtime_error("Omniverse line drawing requires platform stream");line(x1,y1,x2,y2,color_value,thickness);
 }
 void draw_circle(std::uint32_t x,std::uint32_t y,std::uint32_t radius,std::uint32_t color_value,std::uint32_t fill)override{
  if(!circle)throw std::runtime_error("Omniverse circle drawing requires platform stream");circle(x,y,radius,color_value,fill);
 }
};
}
