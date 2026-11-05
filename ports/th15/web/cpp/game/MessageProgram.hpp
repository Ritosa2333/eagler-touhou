#pragma once
#include "Types.hpp"
#include <string>
#include <vector>
namespace th15 {
struct MessageInstruction {
    u32 offset=0;u16 time=0;u8 opcode=0;std::vector<u8> payload;
    template<class T> T argument(u32 index)const noexcept{T value{};if((index+1)*sizeof(T)<=payload.size())std::memcpy(&value,payload.data()+index*sizeof(T),sizeof(T));return value;}
    std::string text()const;
};
struct MessageScript {u32 flag=0;std::vector<MessageInstruction> instructions;};
class MessageProgram {
public:
    std::vector<MessageScript> scripts;std::string error;
    bool open(const u8* data,u32 size);
};
}
