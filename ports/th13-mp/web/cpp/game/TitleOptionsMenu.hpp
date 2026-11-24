#pragma once

#include "TitleAnimations.hpp"
#include "TitleMainMenu.hpp"
#include "TitleState.hpp"

#include <functional>
#include <string>

namespace th13 {

inline constexpr i32 kTitleScreenOption = 3;

struct TitleOptionVolumes {
    i32 bgm = 100, effect = 80;
};

struct TitleOptionPlatform {
    virtual ~TitleOptionPlatform() = default;
    virtual void apply_volumes(i32 bgm, i32 effect) {
        (void)bgm;
        (void)effect;
    }
    virtual bool persist_options(const TitleOptionVolumes&) { return false; }
    virtual bool open_key_config() { return false; }
};

class TitleOptionsMenu {
    TitleState& state;
    AnmManager& animations;
    TitleOptionPlatform& host;
    TitleOptionVolumes& volumes;
    TitleAnimations visuals;
    TitleSceneResources resources;
    i32 row_ = 0;
    i32 substate_frames_ = 0;
    bool key_config_ = false;
    bool pushed_ = false;
    bool fail(const std::string& reason);
    bool sound(i32 id);
    bool open();
    bool refresh_volumes();
    void select_row(i32 label_base);
    bool leave(bool to_key_config);

public:
    std::function<bool(i32)> play_sound;
    std::string error;
    TitleOptionsMenu(TitleState& state_, AnmManager& animations_, TitleOptionPlatform& host_,
                     TitleOptionVolumes& volumes_, TitleSceneResources bank)
        : state(state_), animations(animations_), host(host_), volumes(volumes_),
          visuals(state_, animations_, bank.title), resources(bank) {}
    bool update(u32 pressed, u32 repeated);
    i32 row() const { return row_; }
    const TitleOptionVolumes& volumes_ref() const { return volumes; }
};

}
