#pragma once

#include "AnmManager.hpp"
#include "HudDrawData.hpp"
#include "MenuCursor.hpp"
#include "ReplayFile.hpp"
#include "TitleMainMenu.hpp"
#include "TitleState.hpp"

#include <functional>
#include <string>

namespace th13 {

struct TitleReplayPlatform {
    virtual ~TitleReplayPlatform() = default;
    virtual i32 replay_count() const { return 0; }
    virtual const ReplayFile* replay_entry(i32 index) const {
        (void)index;
        return nullptr;
    }
    virtual std::string replay_filename(i32 index) const {
        (void)index;
        return {};
    }
    virtual bool request_replay_file() { return false; }
    virtual std::string replay_import_error() const { return {}; }
    virtual bool start_replay(i32 file_index, i32 stage_index) {
        (void)file_index;
        (void)stage_index;
        return false;
    }
};

class TitleReplayMenu {
    TitleState& state;
    AnmManager& animations;
    TitleReplayPlatform& replay_host;
    TitleSceneResources resources;
    MenuCursor page;
    i32 selected_index = 0;
    i32 selected_stage_row = 0;
    i32 pages = 1;
    bool launch_queued = false;
    u32 launch_prayer = 0;
    bool start_launch_prayer();
    void stop_launch_prayer();
    bool fail(const std::string& reason);
    bool sound(i32 id);
    bool present(u32 handle) const;
    bool create_badge();
    bool open();
    bool confirm_file();
    void consume_import_hint();
    const ReplayFile* selected_file() const;
    const ReplayStage* stage_block(const ReplayFile& file, i32 number) const;
    static bool spell_practice(const ReplayFile& file);
    i32 stage_chain_index(const ReplayFile& file) const;
    bool file_row_text(i32 index, std::string& out);
    bool stage_row_text(const ReplayFile& file, i32 row, std::string& out);

public:
    std::function<bool(i32)> play_sound;
    std::string error;
    TitleReplayMenu(TitleState& state_, AnmManager& animations_, TitleReplayPlatform& host, TitleSceneResources bank)
        : state(state_), animations(animations_), replay_host(host), resources(bank) {}
    ~TitleReplayMenu();
    bool update(u32 pressed, u32 repeated);
    bool draw_texts(HudDrawServices& host);
};

}
