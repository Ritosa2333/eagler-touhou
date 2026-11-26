// 依赖入口保持原次序，具体RuntimeAdapter调用已有恢复实现。
#pragma once
#include <cstdint>
#include <string_view>
namespace kaikeidou {
struct PauseGameoverServices {
 virtual ~PauseGameoverServices()=default;
 virtual bool enter_scene_zero()=0;
 virtual std::int32_t physical_input_count(std::uint32_t binding)=0;
 virtual void pause_background()=0;
 virtual void resume_background(std::uint32_t mode)=0;
 virtual void resume_music(std::uint32_t track,std::uint32_t mode)=0;
 virtual void present_gameplay()=0;
 virtual std::uint32_t color(std::uint32_t red,std::uint32_t green,std::uint32_t blue)=0;
 virtual void blend(std::uint32_t mode,std::uint32_t opacity)=0;
 virtual void rectangle(std::int32_t left,std::int32_t top,std::int32_t right,std::int32_t bottom,std::uint32_t ink,std::uint32_t filled)=0;
 virtual void draw_font_text(std::int32_t x,std::int32_t y,std::string_view text,std::uint32_t ink,std::uint32_t font,std::uint32_t edge,std::uint32_t vertical)=0;
 virtual std::uint32_t initialize_game(std::uint32_t parameter)=0;
 virtual void return_title()=0;
 virtual void initialize_title()=0;
 virtual void store_scores_settings()=0;
};
}
