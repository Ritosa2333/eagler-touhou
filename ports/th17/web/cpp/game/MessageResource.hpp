#pragma once
#include "Types.hpp"
#include <string>
#include <vector>
namespace th17 {
struct MessageInstruction {
    u16 time=0;u8 opcode=0;u32 offset=0;std::vector<u8> data;std::string text;
    template<class T=i32>T argument(u32 index=0)const noexcept{T value{};if(u64(index)*4+sizeof(T)<=data.size())std::memcpy(&value,data.data()+index*4,sizeof(T));return value;}
};
struct MessageScript {u32 offset=0,flags=0;std::vector<MessageInstruction> instructions;};
class MessageResource {
public:
    std::vector<MessageScript> scripts;std::string error;
    bool open(const u8*,u32,bool ending=false);
    // Text stays in the original Shift-JIS byte encoding until the font owner
    // rasterizes it. A trail byte 0x5f must never become an ASCII space.
    static bool decode(const u8*,u32,std::string&);
};
}
