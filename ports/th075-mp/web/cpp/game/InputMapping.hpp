#pragma once
#include "Types.hpp"
namespace th075 {
struct JoystickState { i32 horizontal=0,vertical=0; std::array<u8,128> buttons{}; };
// Signed held-frame counts are part of the original command recognition,
// not a replacement with movement vectors or shooter-style analog motion.
class InputMapping {
public:
    i8 joystick=-1;
    std::array<u32,9> bindings{};
    i32 horizontal=0,vertical=0;
    std::array<i32,5> buttons{};
    void update(const std::array<u8,256>& keyboard,const JoystickState* pad=nullptr);
    void clear(){horizontal=vertical=0;buttons.fill(0);}
};
}
