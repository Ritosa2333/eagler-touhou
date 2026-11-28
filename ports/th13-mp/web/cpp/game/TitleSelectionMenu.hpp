#pragma once

#include "HudDrawData.hpp"
#include "SessionState.hpp"
#include "TitleAnimations.hpp"
#include "TitleMainMenu.hpp"

#include <functional>

namespace th13 {

struct TitleMenuServices {
    virtual ~TitleMenuServices() = default;
    virtual bool sound(i32) = 0;
    virtual bool begin_transition(u32& animation) = 0;
    virtual bool start_game(i32 stage) = 0;
    virtual bool practice_stage_played(i32 character, i32 difficulty, i32 stage) {
        (void)character;
        (void)difficulty;
        (void)stage;
        return false;
    }
    virtual bool character_cleared(i32 character, i32 difficulty) {
        (void)character;
        (void)difficulty;
        return false;
    }
    virtual bool extra_unlocked(i32 character) {
        (void)character;
        return false;
    }
};

class TitleDifficultyMenu {
    TitleState& state;
    SessionState& progress;
    TitleSelectionSettings& settings;
    TitleAnimations visuals;
    TitleSceneResources resources;
    AnmManager& animations;
    i32 frames = -1;
    bool check(bool);
    bool sound(i32);
    bool build_difficulty();
    void finish_menu_hides();
    void advance_choreography();
    void spawn_particles(i32 count);
    void reorder_difficulty_draws();
    void retire_difficulty();
    void choose_character() noexcept;
    void return_main();

public:
    std::function<bool(i32)> play_sound;
    std::string error;
    TitleDifficultyMenu(TitleState& s, SessionState& p, TitleSelectionSettings& settings, AnmManager& a,
                        TitleSceneResources resources)
        : state(s), progress(p), settings(settings), visuals(s, a, resources.title), resources(resources),
          animations(a) {}
    bool update(u32 pressed, u32 repeated);
};

class TitleCharacterMenu {
    TitleState& state;
    SessionState& progress;
    TitleSelectionSettings& settings;
    AnmManager& animations;
    TitleAnimations visuals;
    TitleSceneResources resources;
    TitleMenuServices& host;
    bool check(bool, const char*);
    bool visual(bool);
    bool character_interrupt(i32 character, i32 label, bool immediate);
    bool exit_portrait(i32 character, i32 label);
    bool enter_portrait(i32 character, i32 label);
    void return_difficulty();

public:
    std::string error;
    TitleCharacterMenu(TitleState& s, SessionState& p, TitleSelectionSettings& settings, AnmManager& a,
                       TitleMenuServices& host, TitleSceneResources resources)
        : state(s), progress(p), settings(settings), animations(a), visuals(s, a, resources.title),
          resources(resources), host(host) {}
    bool update(u32 pressed, u32 repeated);
};

class TitleStageMenu {
    TitleState& state;
    SessionState& progress;
    TitleSelectionSettings& settings;
    AnmManager& animations;
    TitleAnimations visuals;
    TitleMenuServices& host;
    bool check(bool, const char*);
    bool available(i32 stage);

public:
    std::string error;
    TitleStageMenu(TitleState& s, SessionState& p, TitleSelectionSettings& choice, AnmManager& a, TitleMenuServices& h,
                   TitleSceneResources resources)
        : state(s), progress(p), settings(choice), animations(a), visuals(s, a, resources.title), host(h) {}
    bool update(u32 pressed, u32 repeated);
    bool draw_texts(HudDrawServices&);
};

constexpr i32 kTitleCharacterRows = 4;
constexpr i32 kTitleCharacterFirst = 0;
constexpr i32 kTitleCharacterLast = 3;
inline i32 title_character_at(i32 row) { return row - kTitleCharacterFirst; }

}
