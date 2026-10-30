#pragma once
#include "Types.hpp"
#include <array>
#include <string>
namespace th17 {
struct StageVisualSet {
    i32 background_bank=-1,background_script=-1,independent=0,overlay_bank=-1,overlay_script=-1;
    i32 name_bank=-1,name_script=-1,portrait_bank=-1,portrait_script=-1,boss_name=-1;
};
static_assert(sizeof(StageVisualSet)==40);
struct StageConfiguration {
    i32 stage=0;std::string background,enemy_script,stage_music,boss_music;
    std::array<std::string,9> messages;std::string logo;
    i32 stage_music_index=0,boss_music_index=0;u32 flags=0;
    std::array<StageVisualSet,4> visuals{};
    const std::string* message(i32 character,i32 animal)const noexcept;
    static const StageConfiguration* get(i32 stage)noexcept;
};
}
