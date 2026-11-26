// 沿用音乐室的输入/文字/音频边界，并加入字体、居中文字和手柄位图。
#pragma once
#include "core/frontend/MenuMusicServices.hpp"
namespace kaikeidou {
struct MenuOptionsServices:MenuMusicServices {
    virtual std::int32_t set_font_size(std::uint32_t size)=0;
    virtual std::int32_t centered_text(std::uint64_t x_bits,std::uint64_t y_bits,
        std::uint32_t color,std::string_view text)=0;
    virtual std::uint32_t pad_state(std::uint32_t device)=0;
};
}
