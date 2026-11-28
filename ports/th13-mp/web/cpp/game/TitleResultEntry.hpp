#pragma once

#include "HudDrawData.hpp"
#include "TitleResultView.hpp"
#include "TitleState.hpp"

#include <string>

namespace th13 {

class AnmManager;
struct TitleScenePlatform;

class TitleResultEntry {
    TitleState& state;
    TitleScenePlatform& host;
    AnmManager& animations;
    i32 title_bank;
    bool data_loaded_ = false;
    TitleResultScreenData data_;
    bool fail(const std::string& reason);
    bool create_visual(i32 script, u32& handle);
    bool open_name_screen();
    bool open_slot_screen();
    void retire_name_visuals();
    void retire_slot_visuals();
    bool return_main();
    bool name_editor(u32 pressed, u32 repeated);
    bool slot_selector(u32 pressed, u32 repeated);
    bool slot_name_editor(u32 pressed, u32 repeated);
    bool edit_name(u32 pressed, u32 repeated, bool save_to_replay);
    bool draw_score_rows(HudDrawServices&, const TitleResultScreenData&);
    bool draw_name_editor(HudDrawServices&, float name_x, float name_y);
    bool draw_slot_list(HudDrawServices&);
    bool draw_slot_selected(HudDrawServices&);
    std::string slot_row_text(i32 index) const;
    static std::string score_row_text(const TitlePlayerDataScore&, i32 ordinal, const std::string& fallback_name);

public:
    std::string error;
    TitleResultEntry(TitleState& state_, TitleScenePlatform& host_, AnmManager& animations_, i32 title_bank_ = 16)
        : state(state_), host(host_), animations(animations_), title_bank(title_bank_) {}
    bool update(u32 pressed, u32 repeated);
    bool draw_texts(HudDrawServices&);
};

}
