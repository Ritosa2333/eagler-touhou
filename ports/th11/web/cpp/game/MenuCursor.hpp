#pragma once
#include "Types.hpp"
#include <array>
namespace th11 {
// Selection/history shared by the original title, pause and results menus.
struct MenuCursor {
    i32 selected=0,previous=0,count=0;
    std::array<i32,16> selections{},counts{};
    i32 depth=0;
    std::array<i32,16> disabled{};
    i32 wrap=0,disabled_count=0;
    i32 select(i32)noexcept;
    i32 move(i32)noexcept;
    void push()noexcept;
    void pop()noexcept;
};
static_assert(sizeof(MenuCursor)==0xd8);
}
