#include "input.hpp"
#include <algorithm>
namespace th20::source::replay {
void reset_input(input::ButtonState& s) noexcept {
    std::fill_n(s.retained_118,32,0u);std::fill_n(s.retained_218,32,0u);std::fill_n(s.retained_298,6,0u);s.retained_2b4=0;
}
void update_input(input::ButtonState& s) noexcept {
    auto held=s.retained_298[1];unsigned bit=1;s.retained_298[3]=0;s.retained_2b4=0;
    for(unsigned i=0;i<32;++i,held>>=1,bit<<=1){
        if(!(held&1)){s.retained_118[i]=s.retained_218[i]=0;}
        else{++s.retained_118[i];++s.retained_218[i];if(s.retained_118[i]>7)s.retained_2b4|=bit;if(s.retained_118[i]>25){s.retained_298[3]|=bit;s.retained_118[i]-=8;}}
    }
    s.retained_298[4]=(s.retained_298[1]^s.retained_298[2])&s.retained_298[1];s.retained_298[5]=(s.retained_298[1]^s.retained_298[2])&~s.retained_298[1];
}
}
