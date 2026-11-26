// 白幕分支的ECX由61D8AC..61D8BC产生；观察值只在调用边界比较。
#pragma once
#include "ReplayFileRuntime.hpp"
#include <array>
#include <cstring>
#include <istream>
#include <ostream>
#include <stdexcept>
namespace kaikeidou {
struct ExReplayTailWorldProbeState {
    std::vector<std::uint8_t> buffer;
    std::array<std::uint32_t,12> expected{};
    std::array<std::uint32_t,9> result{};
    bool advanced=false;
    static std::uint32_t read_word(std::istream& stream){std::uint32_t value;stream.read(reinterpret_cast<char*>(&value),4);if(!stream)throw std::runtime_error("EX replay tail input truncated");return value;}
    void read(std::istream& stream){
        if(read_word(stream)!=0x31545245u||read_word(stream)!=1||read_word(stream)!=1)throw std::runtime_error("EX replay tail version differs");
        const auto count=read_word(stream);if(count==0||count>0x100000u)throw std::runtime_error("EX replay buffer size invalid");
        buffer.resize(count);stream.read(reinterpret_cast<char*>(buffer.data()),count);if(!stream)throw std::runtime_error("EX replay buffer truncated");
        for(auto& value:expected)value=read_word(stream);
    }
    struct Memory {
        std::vector<std::uint8_t>& buffer;
        std::uint32_t& accumulator;
        std::uint32_t cursor,mode,buttons;
        std::uint32_t word(std::uint32_t address)const{
            switch(address){case 0xb43100:return 0;case 0xb43108:return buttons;case 0xb4310c:return mode;case 0xb43110:return cursor;case 0xb43c04:return accumulator;default:throw std::out_of_range("EX replay tail word");}
        }
        void word(std::uint32_t address,std::uint32_t value){if(address==0xb43110)cursor=value;else if(address==0xb43c04)accumulator=value;else throw std::out_of_range("EX replay tail word write");}
        std::uint8_t byte(std::uint32_t address)const{if(address==0xb43108)return static_cast<std::uint8_t>(buttons);return buffer.at(address);}
        void byte(std::uint32_t address,std::uint8_t value){buffer.at(address)=value;}
    };
    void advance(std::uint32_t tick,std::uint32_t frame,std::uint32_t presentation_ecx,std::uint32_t cursor,
                 std::uint32_t mode,std::uint32_t buttons,std::uint32_t& accumulator){
        if(advanced)throw std::runtime_error("EX replay tail called twice");
        const std::array<std::uint32_t,7> before{{tick,frame,presentation_ecx,accumulator,cursor,mode,buttons}};
        for(unsigned i=0;i<7;++i)if(before[i]!=expected[i])throw std::runtime_error("EX replay tail entry differs");
        Memory memory{buffer,accumulator,cursor,mode,buttons};advance_replay_after_presentation(memory,presentation_ecx);
        const auto exit_ecx=mode==0?(presentation_ecx&0xffffff00u)|(buttons&255u):presentation_ecx;
        const std::array<std::uint32_t,5> after{{exit_ecx,accumulator,memory.cursor,mode,buttons}};
        for(unsigned i=0;i<5;++i)if(after[i]!=expected[i+7])throw std::runtime_error("EX replay tail return differs");
        result={{tick,frame,presentation_ecx,exit_ecx,memory.cursor,mode,buttons,accumulator,static_cast<std::uint32_t>(buffer.size())}};advanced=true;
    }
    void write(std::ostream& stream)const{
        if(!advanced)throw std::runtime_error("EX replay tail was not executed");
        const std::uint32_t marker=0x31524f45u;stream.write(reinterpret_cast<const char*>(&marker),4);
        stream.write(reinterpret_cast<const char*>(result.data()),sizeof(result));stream.write(reinterpret_cast<const char*>(buffer.data()),buffer.size());
    }
};
}
