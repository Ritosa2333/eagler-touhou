// 绘制参数保留原格式串和文本字节；返回值由平台提供。
#pragma once
#include "core/shared/MusicPresentationServices.hpp"
namespace kaikeidou {
struct MenuMusicServices:MusicPresentationServices {
    virtual std::int32_t input_count(std::uint32_t binding)=0;
    virtual std::uint32_t color(std::uint32_t red,std::uint32_t green,std::uint32_t blue)=0;
    virtual std::int32_t draw_text(std::int32_t x,std::int32_t y,std::uint32_t color,
        std::string_view format,std::uint32_t number,std::string_view text)=0;
    virtual void change_stage_resources(std::int32_t stage)=0;
};
}
