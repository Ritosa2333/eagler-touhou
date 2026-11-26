// 绘图记录从输入流逐行读取，只核对恢复逻辑产生的调用；平台返回值不改游戏状态。
#pragma once
#include <array>
#include <cstdint>
#include <initializer_list>
#include <istream>
#include <stdexcept>
#include <string>
namespace kaikeidou {
struct ExDrawingWorldProbeState {
    static constexpr std::uint32_t magic=0x31445745u;
    std::istream* stream=nullptr;
    std::array<std::uint32_t,17> next{};
    std::uint32_t remaining=0,consumed=0,previous_tick=0;
    static std::uint32_t word(std::istream& input){
        std::uint32_t value=0;input.read(reinterpret_cast<char*>(&value),4);
        if(!input)throw std::runtime_error("EX drawing stream truncated");return value;
    }
    static std::uint32_t argument_count(std::uint32_t site){
        switch(site){
        case 0x5ca651:return 3;
        case 0x5fbaf6:case 0x5fbb39:case 0x5fbb7b:case 0x5fbbbe:
        case 0x5fbc11:case 0x5fbc54:case 0x5fbc97:case 0x5ed70e:return 5;
        case 0x5cfa45:case 0x5cfae4:case 0x5d81ff:
        case 0x5dba0b:case 0x5dba56:case 0x5dbaa5:
        case 0x5fbd82:case 0x5fbf5b:case 0x5f9d53:case 0x5fa56a:case 0x5fa9a0:
        case 0x5faa5e:case 0x5fb0d7:case 0x5fb195:case 0x5fb62f:return 6;
        default:throw std::runtime_error("EX drawing source site invalid");
        }
    }
    void advance(){
        if(!remaining)return;
        for(auto& value:next)value=word(*stream);
        if(next[0]<previous_tick)throw std::runtime_error("EX drawing tick order differs");
        previous_tick=next[0];
        if(next[7]!=argument_count(next[1])||next[15]!=1||next[16]>0xffffu)
            throw std::runtime_error("EX drawing record invalid");
        for(std::size_t i=8+next[7];i<14;++i)if(next[i])
            throw std::runtime_error("EX drawing unused argument differs");
    }
    void read(std::istream& input){
        if(stream)throw std::runtime_error("EX drawing stream already opened");
        if(word(input)!=magic||word(input)!=1)throw std::runtime_error("EX drawing stream version differs");
        remaining=word(input);stream=&input;advance();
    }
    std::uint32_t consume(std::uint32_t tick,std::uint32_t frame,std::uint32_t stage,
        std::uint32_t part,std::uint32_t phase,std::uint32_t character,
        std::initializer_list<std::uint32_t> arguments,std::uint32_t control_word){
        if(!stream||!remaining)throw std::runtime_error("Unexpected EX drawing call");
        const std::array<std::uint32_t,7> metadata{{tick,frame,stage,part,phase,character,static_cast<std::uint32_t>(arguments.size())}};
        for(std::size_t i=0;i<metadata.size();++i){
            const auto offset=i?i+1:i;
            if(next[offset]!=metadata[i])throw std::runtime_error("EX drawing metadata differs at call "+std::to_string(consumed+1));
        }
        std::size_t i=8;for(auto value:arguments){
            if(next[i++]!=value)throw std::runtime_error("EX drawing argument differs at call "+std::to_string(consumed+1));
        }
        if(next[16]!=control_word)throw std::runtime_error("EX drawing precision control differs");
        const auto result=next[14];--remaining;++consumed;advance();return result;
    }
    void finish_tick(std::uint32_t tick)const{
        if(remaining&&next[0]<=tick)throw std::runtime_error("EX drawing call skipped at tick "+std::to_string(tick));
    }
    void finish()const{
        if(!stream||remaining)throw std::runtime_error("EX drawing calls remain unconsumed");
        if(stream->peek()!=std::char_traits<char>::eof())throw std::runtime_error("EX drawing stream has trailing bytes");
    }
};
}
