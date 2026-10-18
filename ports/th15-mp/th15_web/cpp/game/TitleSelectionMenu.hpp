#pragma once
#include "TitleAnimations.hpp"
#include "SessionState.hpp"
#include <functional>
namespace th15 {
class TitleModeMenu {
    TitleState& state;SessionState& progress;PlayerLifeSession& player;TitleSelectionSettings& settings;TitleAnimations visuals;
    bool check(bool);bool sound(i32);
public:
    std::function<bool(i32)> play_sound;std::string error;
    TitleModeMenu(TitleState& s,SessionState& p,PlayerLifeSession& player,TitleSelectionSettings& settings,AnmManager& a,i32 title=16,i32 ascii=5):state(s),progress(p),player(player),settings(settings),visuals(s,a,title,ascii){}
    bool update(u32 pressed,u32 repeated);
};
class TitleDifficultyMenu {
    TitleState& state;SessionState& progress;PlayerLifeSession& player;TitleSelectionSettings& settings;TitleAnimations visuals;
    bool check(bool);bool sound(i32);void choose_character()noexcept;void return_main();
public:
    std::function<bool(i32)> play_sound;std::string error;
    TitleDifficultyMenu(TitleState& s,SessionState& p,PlayerLifeSession& player,TitleSelectionSettings& settings,AnmManager& a,i32 title=16,i32 ascii=5):state(s),progress(p),player(player),settings(settings),visuals(s,a,title,ascii){}
    bool update(u32 pressed,u32 repeated,const std::array<bool,5>& all_characters_cleared);
};
}
