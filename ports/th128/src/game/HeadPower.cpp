#include "HeadPower.hpp"
#include <cmath>
#include <string>
namespace th128::hud {
std::optional<HeadPower> headPower(int power,int held,float x,float y){
    if(power<0||power>10000||held< -1000000||held>1000000||!std::isfinite(x)||!std::isfinite(y)
        ||std::abs(x)>65536||std::abs(y)>65536)return std::nullopt;
    HeadPower result;result.percent=power/100;result.x=float(double(x)+320);result.y=float(double(y)-8);
    result.color=power<3000?0x80808080u:power<10000?0xa080d0e0u:0xa080f0f0u;
    result.scale=power<3000?1:held<45?1.5f:2;
    const auto text=std::to_string(result.percent)+"%";
    float horizontal=float(double(result.x)-double(text.size())*9.0*result.scale*.5);
    const float vertical=float(double(result.y)-10.0*result.scale);
    for(const char character:text){
        const int sprite=character=='%'?269:int(static_cast<unsigned char>(character))+210;
        result.glyphs.push_back({sprite,horizontal,vertical});horizontal=float(double(horizontal)+9.0*result.scale);
    }
    return result;
}
}
