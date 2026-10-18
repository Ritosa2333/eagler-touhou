#pragma once
#include "../game/GameProgression.hpp"
namespace th10::browser {
struct MenuData {
    const char *alphabet,*locked_title,*unknown_spell;
    using TextTable=const char* const*;
    TextTable locked_comments,demo_files,characters,difficulties,long_difficulties,stage_names,replay_stage_names;
    const u32* unlock_sequence;
    const StageConfiguration* stages;
};
const MenuData& menu_data(bool chinese);
}
