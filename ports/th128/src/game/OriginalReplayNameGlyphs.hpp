#pragma once
#include "NativeReplaySaveMenu.hpp"
#include "game/OriginalMenuFont.hpp"
#include <string>
#include <vector>
namespace th128::record {
inline std::vector<th128::pause::OriginalMenuGlyph> originalReplayNameGlyphs(const NativeReplaySaveMenu& menu){
    if(menu.phase()!=2||menu.slot()>=25||menu.length()>8||menu.selected()>=91)
        throw std::runtime_error("original replay name presentation requires the active name owner");
    std::vector<th128::pause::OriginalMenuGlyph> result;
    const auto append=[&](const std::string& text,float x,float y,std::uint32_t color){
        const auto glyphs=th128::pause::originalMenuGlyphs(text,x,y,color);
        result.insert(result.end(),glyphs.begin(),glyphs.end());
    };
    const float start=64+15*float(menu.slot());
    const float y=start+(224-start)*float(std::min(menu.nameAge(),10u))/10;
    append(std::string(menu.name().begin(),menu.name().end()),198,y,0xffffffffu);
    append("_",198+9*float(std::min(menu.length(),7u)),y,0xffffff00u);
    for(unsigned index=0;index<91;++index)
        append(std::string(1,char(NativeReplaySaveMenu::character(index))),208+18*float(index%13),320+16*float(index/13),
               index==menu.selected()?0xffffff00u:0xff808080u);
    return result;
}
}
