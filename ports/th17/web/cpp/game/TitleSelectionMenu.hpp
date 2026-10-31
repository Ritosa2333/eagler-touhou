#pragma once
#include "TitleAnimations.hpp"
#include "TitleRecords.hpp"
#include <functional>
namespace th17 {
class TitleDifficultyMenu {
    TitleState& state;GameSelection& selection;TitleSelectionSettings& settings;TitleAnimations visuals;
    bool visual(bool);bool sound(i32);void choose_character();
public:
    std::function<bool(i32)> play_sound;std::string error;
    TitleDifficultyMenu(TitleState& s,GameSelection& p,TitleSelectionSettings& settings,AnmManager& a):state(s),selection(p),settings(settings),visuals(s,a){}
    bool update(u32 pressed,u32 repeated,const TitleRecords&);
};
class TitleCharacterMenu {
    TitleState& state;GameSelection& selection;TitleSelectionSettings& settings;TitleAnimations visuals;
    bool visual(bool);bool sound(i32);bool selected_children(i32 root,bool recovering=false);bool choose_animal(i32 root);
public:
    std::function<bool(i32)> play_sound;std::string error;
    TitleCharacterMenu(TitleState& s,GameSelection& p,TitleSelectionSettings& settings,AnmManager& a):state(s),selection(p),settings(settings),visuals(s,a){}
    bool update(u32 pressed,u32 repeated,const TitleRecords&);
};
}
