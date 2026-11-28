#pragma once

#include "TitleFailure.hpp"

#include "AnmManager.hpp"
#include "SessionState.hpp"
#include "TitleState.hpp"

#include <functional>

namespace th13 {

enum class TitleMainEntry : i32 {
    StartGame = 0,
    ExtraStart = 1,
    PracticeStart = 2,
    SpellPractice = 3,
    Replay = 4,
    PlayerData = 5,
    MusicRoom = 6,
    Option = 7,
    Manual = 8,
    Quit = 9,
};
constexpr i32 kTitleMainEntries = 10;

constexpr i32 kTitleBootIntroFrames = 180;
constexpr i32 kTitleMenuReadyDelay = 11;

struct TitleSceneResources {
    i32 title = 16;
    i32 version = 17;
    i32 ascii = 2;
    i32 help = 19;
    i32 main_root = 0;
    i32 difficulty_root = 1;
    i32 character_root = 2;
    i32 char_menu_root = 129;
    i32 char_menu_root_ex = 130;
    i32 difficulty_tree = 123;
    i32 difficulty_tree_extra = 124;
    i32 difficulty_badge = 101;
    i32 difficulty_cursor = 119;
    i32 difficulty_particle = 18;
    i32 background_root = 89;
    i32 logo_root = 94;
    i32 logo_restore_root = 95;
    i32 version_bar = 0;
    i32 portrait_base = 125;
    i32 weapon_base = 135;
    i32 weapon_base_ex = 139;
    i32 clear_tag_base = 131;
    i32 replay_badge = 104;
    i32 replay_particle_root = 19;
};

class TitleMainMenu : public TitleFailure {
    TitleState& state;
    SessionState& progress;
    TitleSelectionSettings& settings;
    AnmManager& animations;
    TitleSceneResources resources;
    bool create(i32);
    bool create_at(i32, const Vec3& translation);
    bool present(u32&);
    bool interrupt(i32, i32, bool);
    bool child_interrupt(i32, i32, bool);
    bool sound(i32);
    i32 menu_age = -1;
    bool open_menu();
    bool select_entry();
    bool show_menu();
    void enter(i32 entry);
    bool extra_gate();

public:
    void apply_draw_layers();
    std::function<bool(i32)> play_sound;
    std::function<bool(i32)> extra_unlocked;
    TitleMainMenu(TitleState& state, SessionState& progress, TitleSelectionSettings& settings, AnmManager& animations,
                  TitleSceneResources resources)
        : state(state), progress(progress), settings(settings), animations(animations), resources(resources) {}
    bool update(u32 pressed, u32 repeated);
};

}
