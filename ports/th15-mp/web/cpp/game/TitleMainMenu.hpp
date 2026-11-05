#pragma once
#include "TitleState.hpp"
#include "SessionState.hpp"
#include "AnmManager.hpp"
#include <functional>
namespace th15 {
// The original title scripts own text, transitions and backgrounds. This
// controller makes only semantic animation calls, with no legacy graphics API.
class TitleMainMenu {
    TitleState& state;SessionState& progress;PlayerLifeSession& player;
    TitleSelectionSettings& settings;AnmManager& animations;i32 title_bank,portrait_bank;
    bool fail(const std::string&);bool create(i32);bool present(u32&);
    bool interrupt(i32,i32,bool);bool child_interrupt(i32,i32,bool);
    bool paint(bool extra_available);
    bool select_animation(bool extra_available);
    bool sound(i32);void reset_mode(i32)noexcept;
public:
    std::function<bool(i32)> play_sound;std::string error;
    TitleMainMenu(TitleState& state,SessionState& progress,PlayerLifeSession& player,TitleSelectionSettings& settings,AnmManager& animations,i32 title_bank=16,i32 portrait_bank=17):state(state),progress(progress),player(player),settings(settings),animations(animations),title_bank(title_bank),portrait_bank(portrait_bank){}
    bool update(u32 pressed,u32 repeated,bool extra_available);
};
}
