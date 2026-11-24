#pragma once

#include "AnmManager.hpp"
#include "PauseState.hpp"

namespace th13 {

constexpr i32 kPauseMenuScript = 132;
constexpr i32 kPracticePauseMenuScript = 133;
constexpr i32 kPracticeEndMenuScript = 134;
constexpr i32 kGameOverMenuScript = 142;
constexpr i32 kResultsMenuScript = 147;
constexpr i32 kPauseBackdropScript = 75;

struct PauseMenuServices {
    virtual ~PauseMenuServices() = default;
    virtual bool sound(i32) = 0;
    virtual bool finish_selection(i32 entry) = 0;
    virtual bool save_replay_slot(i32, const std::string&) { return false; }
    virtual bool change_option(i32, i32) { return false; }
    virtual bool record_result_name(const std::string&) { return false; }
    virtual std::string last_result_name() const { return {}; }
};

class PauseMenu {
    PauseState& state;
    AnmManager& animations;
    PauseMenuServices& host;
    bool check(bool, const char*);
    bool interrupt(u32, i32, bool immediate = false);
    bool child(i32, i32);
    bool navigate(u32, u32, i32);
    bool close();
    bool open_replay_slots();
    bool configure_entries();
    i32 entry_script(i32) const;

public:
    std::string error;
    PauseMenu(PauseState& s, AnmManager& a, PauseMenuServices& h) : state(s), animations(a), host(h) {}
    bool update(u32 pressed, u32 repeated);
};

}
