#pragma once

#include "../game/MsgPlayer.hpp"
#include "GraphicsDevice.hpp"
#include <eagler/text/RuntimeFontRaster.hpp>

#include <string>
#include <vector>

namespace th13::sdl {

class FontDevice {
public:
    std::string error;
    FontDevice() = default;
    bool initialize();
    bool ready() const noexcept { return ready_; }
    i32 extent(const std::string& cp932_text, i32 profile = 0);
    bool draw_text(TextureImage& target, i32 x, i32 y, i32 profile, u32 color, const std::string& cp932_text);
    bool draw_dialogue(TextureImage& target, i32 x, i32 y, i32 profile, u32 color, const std::string& cp932_text);
    bool render_dialogue_line(TextureImage& target, i32 x, i32 y, const MsgLine& line, u32 color = 0xffffffff);

private:
    eagler::text::RuntimeFontRaster raster{
        {32.0f, 400}, {40.0f, 400}, {32.0f, 600}, {40.0f, 600}, {15.0f, 700},
    };
    std::vector<u8> mapping_;
    bool ready_ = false;
};

}
