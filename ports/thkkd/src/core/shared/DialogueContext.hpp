// 对话读取的共享状态和平台调用边界；句柄表保留原排列。
#pragma once
#include <array>
#include <cstdint>
#include <string_view>
namespace kaikeidou {
struct DialogueContext {
    std::uint32_t character=0, stage=0, scene=0, subphase=0, game_frame=0;
    std::array<std::uint32_t,24> player_portraits{};
    std::array<std::uint32_t,40> boss_portraits{};
    std::array<std::uint32_t,8> titles{};
    std::uint32_t panel=0, font=0, confirm_binding=5, skip_binding=6;
    std::uint32_t replay_mode=0, replay_buttons=0;
    bool replay_variant=false;
};
struct DialogueServices {
    virtual ~DialogueServices()=default;
    virtual std::uint32_t color(std::uint32_t red,std::uint32_t green,std::uint32_t blue)=0;
    virtual void brightness(std::uint32_t red,std::uint32_t green,std::uint32_t blue)=0;
    virtual void blend(std::uint32_t mode,std::uint32_t opacity)=0;
    virtual void sprite(std::uint64_t x,std::uint64_t y,std::uint64_t scale,std::uint64_t angle,
                        std::uint32_t handle,std::uint32_t transparent,std::uint32_t reverse)=0;
    virtual void text(std::uint32_t x,std::uint32_t y,std::string_view value,std::uint32_t color,
                      std::uint32_t font,std::uint32_t edge,std::uint32_t vertical)=0;
    virtual std::int32_t allocate_boss(std::uint32_t kind)=0;
    virtual void music(std::uint32_t track,std::uint32_t mode)=0;
    virtual std::uint32_t read_counter(std::uint32_t binding)=0;
};
}
