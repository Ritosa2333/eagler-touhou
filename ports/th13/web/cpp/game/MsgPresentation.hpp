#pragma once

#include "AnmManager.hpp"

#include <functional>
#include <string>
#include <vector>

namespace th13 {

class MsgPresentation {
public:
    struct Banks {
        i32 player = 0, stage = 0;
    };
    static constexpr i32 kPlayerPortraitScripts[4] = {29, 34, 30, 28};
    struct PortraitRow {
        i32 nameplate_a_bank, nameplate_a_script;
        i32 nameplate_b_bank, nameplate_b_script;
        i32 boss_bank, boss_script;
        i32 midboss_bank, midboss_script;
    };
    static const PortraitRow kStageRows[7];

    using ReadFn = std::function<bool(const std::string&, std::vector<u8>&)>;

    MsgPresentation(AnmManager& animations, Banks banks) : animations(animations), banks(banks) {}
    std::string error;
    struct RollbackState {
        u32 player_;
        u32 side_;
        u32 nameplate_;
    };
    RollbackState capture_rollback() const { return {player_, side_, nameplate_}; }
    void restore_rollback(const RollbackState& state) {
        player_ = state.player_;
        side_ = state.side_;
        nameplate_ = state.nameplate_;
    }

    bool bind_stage_scripts(i32 stage, const std::vector<std::string>& anim_list, const ReadFn& read);
    bool player_portrait(i32 character);
    bool side_portrait(i32 entry);
    bool boss_side_portrait();
    bool nameplate(i32 chapter);
    bool interrupt_player(i32 label);
    bool interrupt_side(i32 label);
    bool fade_player(i32 label);
    bool fade_side(i32 label);
    bool set_visible(bool visible);
    void release_player();
    void release_side();
    void release_nameplates();
    void release_all();

private:
    bool create(bool player_bank, i32 manager_bank, i32 script, u32& handle);
    i32 stage_script(i32 manager_bank, i32 script);
    bool fail(const std::string& why) {
        if (error.empty()) error = why;
        return false;
    }
    AnmManager& animations;
    Banks banks;
    i32 stage_ = 0;
    std::vector<i32> stage_script_base_;
    u32 player_ = 0, side_ = 0, nameplate_ = 0;
};

}
