#pragma once

#include "FrameScheduler.hpp"
#include "TitleMainMenu.hpp"
#include "TitleState.hpp"

namespace th13 {

struct TitleFrameInput {
    u32 held = 0, pressed = 0, repeated = 0, system_flags = 0;
};

struct TitleFramePlatform {
    virtual ~TitleFramePlatform() = default;
    virtual bool clear_title_overlay() = 0;
    virtual bool queue_title_music(i32 code, i32 value, const std::string&) = 0;
    virtual bool clear_current_wave() = 0;
    virtual bool title_exit(i32 destination) = 0;
    virtual bool fade_out_title_music() = 0;
};

struct TitleFrameServices : TitleFramePlatform {
    virtual bool update_title_menu(TitleScreen, const TitleFrameInput&) = 0;
    virtual bool restore_practice_selection() { return false; }
    virtual bool restore_spell_practice_selection() { return false; }
    virtual bool draw_title_replay_texts() { return true; }
    virtual bool draw_title_music_room_texts() { return true; }
};

void emit_title_side_strips(AnmManager& animations);

class TitleFrame {
    TitleState& state;
    SessionState& progress;
    TitleFrameServices& host;
    FrameScheduler& scheduler;
    AnmManager& animations;
    std::array<FrameCallback, 2> callbacks;
    TitleFrameInput input;
    bool check(bool, const char*);
    bool menu();
    bool initialize();
    void submit_side_strips();

public:
    i32 music_age = 0;
    std::string error;
    TitleFrame(TitleState&, SessionState&, TitleFrameServices&, FrameScheduler&, AnmManager&);
    ~TitleFrame();
    void controls(const TitleFrameInput& value) noexcept { input = value; }
    bool update(const TitleFrameInput&);
    bool drawing() const noexcept { return callbacks[1].enabled; }
};

}
