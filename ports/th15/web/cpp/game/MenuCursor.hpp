#pragma once
#include "Types.hpp"
#include <array>
#include <string>
namespace th15 {
// Shared menu navigation: history and disabled entries belong to the menu,
// while held/repeated input and animation selection belong to its controller.
class MenuCursor {
    bool enabled(i32)const noexcept;
    bool any_enabled()const noexcept;
public:
    i32 cursor=0,previous=0,count=0;
    std::array<i32,16> history_cursor{},history_count{},disabled{};
    i32 depth=0;bool wrapping=true;i32 disabled_count=0;std::string error;
    i32 select(i32);
    i32 move(i32);
    bool disable(i32);
    void push()noexcept;
    void pop()noexcept;
};
}
