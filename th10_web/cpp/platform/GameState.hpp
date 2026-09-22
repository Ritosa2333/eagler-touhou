#pragma once
#include "Input.hpp"
#include "MenuData.hpp"
#include "../game/ApplicationConfig.hpp"
#include "../game/Replay.hpp"
#include "../game/ApplicationState.hpp"
#include "../game/PracticeConfig.hpp"
#include "../../../portable/input/MotionTrack.hpp"
namespace th10::browser {
// Persistent application data shared by menus, gameplay and result screens.
struct GameState {
    GameEconomy game{};PracticeState practice;ApplicationConfig configuration{};
    ApplicationState application{};
    u32& engine_flags=application.engine_flags;
    i32& pending_screen=application.pending_screen;
    u32& background_color=application.background_color;
    u32 quitting=0;
    i32 return_screen=0,inactive_frames=0,demo_index=0;
    i32 remembered_stage=0,practice_shortcut=0,remembered_replay=0;
    u32 unlock_cursor=0; i32 unlock_elapsed=0;
    u8 keyboard[256]{},previous_keyboard[256]{},pressed_keyboard[256]{};
    char replay_filename[256]{};
    const StageConfiguration* current_stage;
    Replay* replay=nullptr;
    touhou::input::MotionTrack motion;
    double active_time=0,total_time=0;
    bool chinese;
    GameState(Input& input,bool chinese);
};
}
