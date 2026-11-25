#pragma once
#include <array>
#include <span>
#include <string_view>

namespace soew::english_story {
using DialogueBox=std::array<std::string_view,2>;
#include "english_dialogue.inc"
#include "english_endings.inc"

inline constexpr std::array<std::span<const DialogueBox>,6> stages={stage0,stage1,stage2,stage3,stage4,stage5};
static_assert(stage0.size()==24&&stage1.size()==35&&stage2.size()==11&&stage3.size()==52&&stage4.size()==57&&stage5.size()==31);
static_assert(bad.size()==51&&good.size()==94);
static_assert([]{
    constexpr std::string_view marker="{continues}";
    for(const auto stage:stages)for(const auto &box:stage){
        if(box[0].empty())return false;
        for(const auto row:box)
            if(row.size()-(row.contains(marker)?marker.size()-2:0)>36)return false;
    }
    for(const auto ending:{std::span<const std::string_view>(bad),std::span<const std::string_view>(good)})
        for(const auto row:ending)if(row.empty()||row.size()>44)return false;
    return true;
}(),"TH02 English story must fit the original text cells");
}
