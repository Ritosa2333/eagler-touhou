#pragma once
#include "OriginalMenuFont.hpp"
#include "NativeScoreProfile.hpp"
#include <array>
#include <algorithm>
#include <cmath>
#include <string>
#include <bit>
#include <cstdio>
#include <ctime>
#include <span>
namespace th128::postrun {
inline unsigned scoreNameWord(std::span<const std::uint8_t> bytes,unsigned at){return unsigned(bytes[at])|(unsigned(bytes[at+1])<<8)|(unsigned(bytes[at+2])<<16)|(unsigned(bytes[at+3])<<24);}
inline std::string originalScoreNameRow(std::span<const std::uint8_t> profile,unsigned route,unsigned rank,unsigned row){
    static constexpr std::array<const char*,24> stages{"init Stage","Stage A1-1","Stage A1-2","Stage A1-3","Stage A2-2","Stage A2-3","Stage B1-1","Stage B1-2","Stage B1-3","Stage B2-2","Stage B2-3","Stage C1-1","Stage C1-2","Stage C1-3","Stage C1-2","Stage C1-3","ExtraStage"," A1 Clear "," A2 Clear "," B1 Clear "," B2 Clear "," C1 Clear "," C2 Clear ","ExtraClear"};
    if(profile.size()!=52232||route>=8||rank>=5||row>=10)throw std::runtime_error("original score-name row owner domain rejected");
    const unsigned at=route*1644+16+rank*320+row*32;const auto score=scoreNameWord(profile,at);const unsigned stage=profile[at+4],continued=profile[at+5];
    std::string name;bool terminated=false;for(unsigned i=0;i<9;++i){const auto code=profile[at+6+i];if(!code){terminated=true;break;}if(code<32||code>129)throw std::runtime_error("original score row glyph unavailable");name+=char(code);}
    const auto time=std::uint64_t(scoreNameWord(profile,at+16))|(std::uint64_t(scoreNameWord(profile,at+20))<<32);
    const auto slowdown=std::bit_cast<float>(scoreNameWord(profile,at+24));
    if(!terminated||score>999999999||stage>=stages.size()||continued>9||!std::isfinite(slowdown)||time>0x793406fffull)throw std::runtime_error("original score-name row metadata rejected");
    char text[160]{};int count=0;
    if(time){const std::time_t timestamp=static_cast<std::time_t>(time);if(std::uint64_t(timestamp)!=time)throw std::runtime_error("native score row time64 conversion unavailable");
        const auto* observed=std::localtime(&timestamp);if(!observed)throw std::runtime_error("native score row actual local calendar unavailable");const auto calendar=*observed;
        count=std::snprintf(text,sizeof(text),"%2d  %s  %9ld%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %2.1f%%",int(row+1),name.c_str(),long(score),int(continued),
            calendar.tm_year+1900,calendar.tm_mon+1,calendar.tm_mday,calendar.tm_hour,calendar.tm_min,stages[stage],double(slowdown));
    }else count=std::snprintf(text,sizeof(text),"%2d  %s  %9ld%d  ----/--/-- --:--  Stage -     ---%%",int(row+1),name.c_str(),long(score),int(continued));
    if(count<1||std::size_t(count)>=sizeof(text))throw std::runtime_error("native score row text budget rejected");return text;
}
inline std::vector<th128::pause::OriginalMenuGlyph> originalScoreNameGlyphs(std::span<const std::uint8_t> profile,unsigned route,unsigned rank,
        int position,const std::array<std::uint8_t,9>& name,unsigned length,unsigned selected,std::string_view characters){
    if(position< -1||position>9||length>8||selected>=91||characters.size()!=88||name[8]!=0)throw std::runtime_error("ordinary score-name display identity rejected");
    std::vector<th128::pause::OriginalMenuGlyph> result;
    const auto emit=[&](const std::string& text,float x,float y,std::uint32_t color){const auto glyphs=th128::pause::originalMenuGlyphs(text,x,y,color);result.insert(result.end(),glyphs.begin(),glyphs.end());};
    for(unsigned row=0;row<10;++row){const auto shade=255-row*16;const auto color=position<0?0xff0000ffu|(shade<<16)|(shade<<8):int(row)==position?0xffffffffu:0xff404040u;
        emit(originalScoreNameRow(profile,route,rank,row),48,160+18*float(row),color);}
    if(position<0)return result;
    const float nameY=160+18*float(position);emit(std::string(name.begin(),name.begin()+8),84,nameY,0xffffffffu);
    emit("_",84+9*float(std::min(length,7u)),nameY,0xffffff00u);
    for(unsigned index=0;index<91;++index){const unsigned character=index<88?unsigned(characters[index]):index==88?129:index==89?127:128;
        emit(std::string(1,char(character)),212+18*float(index%13),360+16*float(index/13),index==selected?0xffffff00u:0xff808080u);}
    return result;
}
}
