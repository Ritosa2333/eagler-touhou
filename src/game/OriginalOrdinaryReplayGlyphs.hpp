#pragma once
#include "NativeReplaySlotPreview.hpp"
#include "OriginalMenuFont.hpp"
#include <algorithm>
#include <bit>
#include <cstdio>
#include <cmath>
#include <ctime>
namespace th128::postrun {
// Exact ordinary replay draw44e4d0..44e886; constants479, labels481, shadow flag483/484.
inline std::string ordinaryReplayLine(const record::NativeReplaySlotPreview& row,unsigned slot,bool blankName=false){
    if(slot>=25)throw std::runtime_error("ordinary replay slot draw exceeds25");char line[192]{};
    if(!row.occupied){std::snprintf(line,sizeof(line),"No.%.2d -------- --/--/-- --:-- --------- ------- ------ ---%%",int(slot+1));return line;}
    if(!row.known)throw std::runtime_error("ordinary occupied replay draw lacks full metadata");
    constexpr std::array<const char*,5> ranks{"Easy   ","Normal ","Hard   ","Lunatic","Extra  "};
    constexpr std::array<const char*,24> stages{"test  ","A1-1  ","A1-2  ","A1-3  ","A2-2  ","A2-3  ","B1-1  ","B1-2  ","B1-3  ","B2-2  ","B2-3  ","C1-1  ","C1-2  ","C1-3  ","C2-2  ","C2-3  ","Extra ","A1 All","A2 All","B1 All","B2 All","C1 All","C2 All","Ex All"};
    const auto view=std::span<const std::uint8_t>(row.metadata);char name[9]{};std::copy_n(row.metadata.begin(),8,name);
    if(blankName)std::fill_n(name,8,' ');const auto word=[&](unsigned p){return record::NativeReplaySlotPreview::word(view,p);};
    const auto slowdown=std::bit_cast<float>(word(84));if(!std::isfinite(slowdown))throw std::runtime_error("ordinary replay slowdown draw rejected");
    std::snprintf(line,sizeof(line),"No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %8d%d %s %s %2.1f%%",int(slot+1),name,
        int(row.calendar[0]%100),int(row.calendar[1]),int(row.calendar[2]),int(row.calendar[3]),int(row.calendar[4]),int(word(20)),int(word(108)),ranks[word(100)],stages[word(104)],double(slowdown));return line;
}
inline std::vector<pause::OriginalMenuGlyph> ordinaryReplayGlyphs(unsigned subphase,unsigned timer,unsigned selected,unsigned slot,
    const std::array<record::NativeReplaySlotPreview,25>& rows,std::span<const std::uint8_t> raw,const std::array<std::uint8_t,9>& name,
    unsigned length,unsigned character,const char* characters){
    std::vector<pause::OriginalMenuGlyph> result;const auto append=[&](const std::string& text,float x,float y,std::uint32_t color,bool shadow){
        auto glyphs=pause::originalMenuGlyphs(text,x,y,color);for(const auto& glyph:glyphs)if(shadow||!glyph.shadow)result.push_back(glyph);};
    if(subphase==2){for(unsigned row=0;row<25;++row)append(ordinaryReplayLine(rows[row],row),58,80+15*float(row),row==selected?0xffffff00u:0xff808080u,true);}
    else if(subphase==3){
        if(slot>=25||raw.size()<148||length>8||character>=91)throw std::runtime_error("ordinary replay name draw identity rejected");
        record::NativeReplaySlotPreview heading;heading.occupied=heading.known=true;std::copy_n(raw.begin()+36,112,heading.metadata.begin());
        const auto seconds=(std::uint64_t(record::NativeReplaySlotPreview::word(raw,52))<<32)|record::NativeReplaySlotPreview::word(raw,48);
        const auto timestamp=std::time_t(seconds);const auto* calendar=std::localtime(&timestamp);if(!calendar)throw std::runtime_error("ordinary replay localtime unavailable");
        heading.calendar={unsigned(calendar->tm_year+1900),unsigned(calendar->tm_mon+1),unsigned(calendar->tm_mday),unsigned(calendar->tm_hour),unsigned(calendar->tm_min)};
        const float start=80+15*float(slot),y=start+(240-start)*float(std::min(timer,10u))/10;
        append(ordinaryReplayLine(heading,slot,true),58,y,0xffffffffu,false);if(timer<10)return result;
        append(std::string(name.begin(),name.begin()+8),112,240,0xffffffffu,false);append("_",112+9*float(std::min(length,7u)),240,0xffffff00u,false);
        for(unsigned i=0;i<91;++i)append(std::string(1,char(i<88?unsigned(characters[i]):i==88?129:i==89?127:128)),212+18*float(i%13),360+16*float(i/13),i==character?0xffffff00u:0xff808080u,false);
    }
    return result;
}
}
