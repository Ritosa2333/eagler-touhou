#pragma once

#include "AnmText.hpp"
#include "TitleMusicRoomMenu.hpp"

#include <array>
#include <functional>
#include <string>
#include <vector>

namespace th13 {

class TitleMusicRoomView {
    AnmManager& animations;
    i32 title_bank, text_bank, ascii_bank;
    u32 badge = 0, particles = 0;
    std::vector<u32> rows;
    std::array<u32, 8> comments{};
    i32 previous_row = -1, previous_comment = -1, revealed = 0, top = 0;
    bool opened = false, closing = false;
    bool fail(const std::string&);
public:
    std::string error;
    using Rasterizer = std::function<bool(AnmVm&, u32, const std::string&, AnmTextAlignment)>;
    TitleMusicRoomView(AnmManager& manager, i32 title = 16, i32 text = 0, i32 ascii = 2)
        : animations(manager), title_bank(title), text_bank(text), ascii_bank(ascii) {}
    bool update(const TitleState&, const TitleMusicRoomMenu&, TitleMusicRoomPlatform&,
                const Rasterizer&);
    void release();
};

} // namespace th13
