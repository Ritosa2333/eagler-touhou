#pragma once
#include "Types.hpp"
namespace th075 {
struct ControlCounts {
    i32 horizontal=0,vertical=0;std::array<i32,4> buttons{};
    void from_held(u8);u8 held()const;u8 edges(i8 facing)const;
    void clear(){horizontal=vertical=0;buttons.fill(0);}
};
class InputHistory {
public:
    std::array<u8,90> frames{};
    void reset(u8 value){frames.fill(value);}
    void push(u8 value);
    u8 match(const char* pattern,u32 length,u32 window,i8 facing)const;
};
struct FighterControls {
    ControlCounts raw,current,buffered;
    i8 bufferTimer=0;
    InputHistory history;
    u8 player_frame(const ControlCounts&,i8 facing,i16 action);
    u8 replay_frame(u8 held,i8 facing,i16 action){raw.from_held(held);return player_frame(raw,facing,action);}
};
}
