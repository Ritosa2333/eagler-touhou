#include "game/HeadPower.hpp"
#include <bit>
#include <cassert>
#include <iostream>
int main(){
    int power=0,held=0;float horizontal=0,vertical=0;
    while(std::cin>>power>>held>>horizontal>>vertical){
        const auto result=th128::hud::headPower(power,held,horizontal,vertical);assert(result);
        std::cout<<result->percent<<' '<<result->color<<' '<<std::bit_cast<std::uint32_t>(result->scale)<<' '
            <<std::bit_cast<std::uint32_t>(result->x)<<' '<<std::bit_cast<std::uint32_t>(result->y)<<'\n';
        assert(result->glyphs.size()==std::to_string(power/100).size()+1);
        assert(result->glyphs.back().sprite==269);
        assert(result->glyphs.front().x==float(double(result->x)-double(result->glyphs.size())*9.0*result->scale*.5));
        for(const auto& glyph:result->glyphs)assert(glyph.sprite==269||(glyph.sprite>=258&&glyph.sprite<=267));
    }
    assert(!th128::hud::headPower(-1,0,0,0));assert(!th128::hud::headPower(10001,0,0,0));
}
