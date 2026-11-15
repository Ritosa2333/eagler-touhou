#pragma once
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <vector>
namespace th128::pause {
struct OriginalMenuGlyph {
    unsigned sprite=0;
    float x=0,y=0;
    std::uint32_t color=0;
    bool shadow=false;
    unsigned priority=74;
};
inline std::vector<OriginalMenuGlyph> originalMenuGlyphs(std::string_view text,float x,float y,std::uint32_t color){
    if(!std::isfinite(x)||!std::isfinite(y)||std::abs(x)>1048576||std::abs(y)>1048576||text.size()>256)
        throw std::runtime_error("original menu font position or text budget rejected");
    std::vector<OriginalMenuGlyph> result;result.reserve(text.size()*2);
    for(const unsigned char code:text){
        if(code<32||code>129)throw std::runtime_error("original style0 menu glyph unavailable");
        if(code!=32){
            const unsigned sprite=unsigned(code)-32;
            result.push_back({sprite,x+2,y+2,(color>>25)<<24,true,74});
            result.push_back({sprite,x,y,color,false,74});
        }
        x+=9;
    }
    return result;
}
}
