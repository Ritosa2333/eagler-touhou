#pragma once
#include <array>
#include <cstdint>
namespace th128::pause {
class OriginalMenuInputFilter {
    std::array<unsigned,32> heldTicks_{};
public:
    void reset(){heldTicks_.fill(0);}
    std::uint32_t advance(std::uint32_t holding){
        std::uint32_t repeated=0;
        for(unsigned index=0;index<32;++index){
            const auto bit=std::uint32_t(1)<<index;
            auto& ticks=heldTicks_[index];
            if(!(holding&bit)){ticks=0;continue;}
            if(++ticks>=26){repeated|=bit;ticks-=8;}
        }
        return repeated;
    }
};
}
