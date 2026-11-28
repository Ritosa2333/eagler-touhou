#pragma once
#include <array>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace th128::hud {
struct PercentGlyph {int sprite=0;float x=0,y=0;};
#define TH128_ORIGINAL_SCORE_HUD 1
struct ScoreLine {std::string text;std::uint32_t color=0xffffffffu;std::vector<PercentGlyph> glyphs;};
struct OriginalScoreDisplay {
    int value=0,step=0;
    void update(int target){
        if(target<0||target>999999999)return;
        if(value==target){step=0;return;}
        const int difference=target-value;
        if(difference<0){value=target;step=0;return;}
        int increment=difference/32;if(increment<1)increment=1;if(increment>578910)increment=578910;
        step=std::max(step,increment);step=std::min(step,difference);value+=step;if(value>=target)step=0;
    }
};
inline std::optional<std::array<ScoreLine,2>> originalScoreHUD(int score,int highScore,int continues,int highContinues){
    if(score<0||score>999999999||highScore<0||highScore>999999999||continues<0||continues>9||highContinues<0||highContinues>9)return std::nullopt;
    std::array<ScoreLine,2> lines;
    const std::array<std::int64_t,2> values{std::int64_t(highScore)*10+highContinues,std::int64_t(score)*10+continues};
    for(unsigned line=0;line<2;++line){
        auto text=std::to_string(values[line]);
        for(int position=int(text.size())-3;position>0;position-=3)text.insert(std::size_t(position),1,',');
        auto& out=lines[line];out.text=text;out.color=line?0xffffffc0u:0xffc0ffc0u;
        float x=line?512.f-14.f*float(text.size()):128.f;
        for(char code:text){out.glyphs.push_back({code==','?253:243+code-'0',x,16});x+=14;}
    }
    return lines;
}
struct PercentLine {int value=0;float x=576,y=0;std::string text;std::vector<PercentGlyph> glyphs;};
inline std::optional<std::array<PercentLine,4>> originalPercentHUD(int motivationRaw,int powerRaw,int chargeRaw,float area){
    if(powerRaw<0||powerRaw>10000||chargeRaw<0||chargeRaw>30000||!std::isfinite(area)||area<0||area>1000000000.f)
        return std::nullopt;
    std::array<PercentLine,4> result;
    const std::array values{motivationRaw>0?motivationRaw/100:0,powerRaw/100,chargeRaw/100,int(area)};
    const std::array<float,4> vertical{70,118,166,358};
    for(std::size_t line=0;line<result.size();++line){
        auto& output=result[line];output.value=values[line];output.y=vertical[line];
        output.text=std::to_string(output.value)+"%";
        float horizontal=576.f-float(output.text.size())*7.f;
        for(const unsigned char character:output.text){
            const int sprite=character=='%'?255:int(character)+195;
            output.glyphs.push_back({sprite,horizontal,output.y});horizontal+=14.f;
        }
    }
    return result;
}
}
