#pragma once

#include "MenuCursor.hpp"
#include "Timer.hpp"

namespace th13 {

enum class TitleScreen : i32 {
    Initialize = 0,
    Main = 1,
    Quit = 2,
    StartGame = 2,
    Option = 3,
    KeyConfig = 4,
    Difficulty = 6,
    Character = 7,
    Stage = 8,
    PlayerData = 10,
    Replay = 11,
    MusicRoom = 13,
    ResultName = 14,
    ResultReplaySlot = 15,
    Manual = 16,
    SpellPractice = 17,
};

struct TitleSelectionSettings {
    i32 configured_difficulty = 1, saved_difficulty = 0, preferred_character = 0;
    i32 practice_stage = 0;
};

struct TitleState {
    MenuCursor menu;
    TitleScreen screen = TitleScreen::Main, previous_screen = TitleScreen::Initialize;
    i32 substate = 0;
    Timer age{0, 0, 0, 0, 0};
    u32 flags = 2;
    std::array<u32, 248> handles{};
    u32 version_bar = 0;
    i32 return_reason = 1;
    i32 confirmed_entry = -1;
    u32 start_effect = 0;
    u32 particle_clock = 0;
    i32 replay_index = 0;
    i32 replay_import_hint = -1;
    u32 replay_ascii_root = 0;
    std::string result_name;
    i32 result_name_cursor = 90;
    i32 result_slot = 0;
    std::string result_slot_name;
    i32 result_character = 0;
    i32 result_difficulty = 0;
    i32 result_rank = 0;
    u32 result_background = 0;

    TitleState() { age.set(0); }

    void change_screen(TitleScreen next) noexcept {
        previous_screen = screen;
        screen = next;
        substate = 0;
        age.set(0);
    }

    void change_substate(i32 value) noexcept {
        substate = value;
        age.set(0);
    }
};

}
