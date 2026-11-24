#pragma once

#include "TitleAnimations.hpp"
#include "TitleMainMenu.hpp"
#include "TitleState.hpp"

#include <array>
#include <functional>
#include <string>

namespace th13 {

inline constexpr i32 kTitleScreenKeyConfig = 4;
inline constexpr i32 kTitleKeyConfigRows = 7;
inline constexpr i32 kTitleKeyConfigBindings = 5;

struct TitleKeyConfigPlatform {
    virtual ~TitleKeyConfigPlatform() = default;
    virtual bool pad_present() const { return false; }
    virtual u32 pad_buttons() const { return 0; }
    virtual std::array<i16, kTitleKeyConfigBindings> pad_bindings() const { return {0, 1, 2, 3, 4}; }
    virtual bool persist_pad_bindings(const std::array<i16, kTitleKeyConfigBindings>&) { return false; }
};

struct TitleKeyConfigHost final : TitleKeyConfigPlatform {
    std::function<bool()> present;
    std::function<u32()> buttons;
    std::function<std::array<i16, kTitleKeyConfigBindings>()> bindings;
    std::function<bool(const std::array<i16, kTitleKeyConfigBindings>&)> persist;
    bool pad_present() const override { return present ? present() : false; }
    u32 pad_buttons() const override { return buttons ? buttons() : 0; }
    std::array<i16, kTitleKeyConfigBindings> pad_bindings() const override {
        return bindings ? bindings() : TitleKeyConfigPlatform::pad_bindings();
    }
    bool persist_pad_bindings(const std::array<i16, kTitleKeyConfigBindings>& values) override {
        return persist ? persist(values) : false;
    }
};

class TitleKeyConfigMenu {
    TitleState& state;
    AnmManager& animations;
    TitleKeyConfigPlatform& host;
    TitleAnimations visuals;
    TitleSceneResources resources;
    std::array<i16, kTitleKeyConfigBindings> bindings_{};
    i32 row_ = 0;
    i32 substate_frames_ = 0;
    bool fail(const std::string& reason);
    bool sound(i32 id);
    bool open();
    bool refresh_digits();
    bool learn(i32 button);
    void select_row(i32 label_base);
    void revert_bindings();
    bool leave();

public:
    std::function<bool(i32)> play_sound;
    std::string error;
    TitleKeyConfigMenu(TitleState& state_, AnmManager& animations_, TitleKeyConfigPlatform& host_,
                       TitleSceneResources bank)
        : state(state_), animations(animations_), host(host_), visuals(state_, animations_, bank.title),
          resources(bank) {}
    bool update(u32 pressed, u32 repeated);
    i32 row() const { return row_; }
    const std::array<i16, kTitleKeyConfigBindings>& bindings() const { return bindings_; }
};

}
