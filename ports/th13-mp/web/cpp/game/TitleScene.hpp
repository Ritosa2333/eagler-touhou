#pragma once

#include "AnmText.hpp"
#include "HudDrawData.hpp"
#include "TitleFrame.hpp"
#include "TitleKeyConfigMenu.hpp"
#include "TitleMainMenu.hpp"
#include "TitleManualMenu.hpp"
#include "TitleMusicRoomMenu.hpp"
#include "TitleMusicRoomView.hpp"
#include "TitleOptionsMenu.hpp"
#include "TitlePlayerDataMenu.hpp"
#include "TitlePlayerDataView.hpp"
#include "TitleReplayMenu.hpp"
#include "TitleResultEntry.hpp"
#include "TitleResultView.hpp"
#include "TitleSelectionMenu.hpp"
#include "TitleSpellPracticeMenu.hpp"
#include "TitleSpellPracticeView.hpp"

#include <memory>
#include <utility>

namespace th13 {

struct TitleScenePlatform : TitleFramePlatform,
                            TitleReplayPlatform,
                            TitleOptionPlatform,
                            TitleMusicRoomPlatform,
                            TitleManualPlatform,
                            TitleMenuServices,
                            TitlePlayerDataPlatform,
                            TitleSpellPracticePlatform,
                            HudDrawServices {
    virtual bool sound(i32) = 0;
    virtual bool start_spell_practice(const SpellPracticeCommand&) { return false; }
    virtual bool can_start_spell_practice(const SpellPracticeCommand&) const { return false; }
    virtual bool record_menu_views() const { return false; }
    virtual bool rasterize_title_text(AnmVm&, u32, const std::string&, AnmTextAlignment = AnmTextAlignment::Left) {
        return false;
    }
    virtual bool save_replay_slot(i32, const std::string&) { return false; }
    virtual bool can_save_replay() const { return false; }
    virtual bool record_result_name(const std::string&) { return false; }
    virtual bool record_clear_result_name(const std::string&) { return false; }
    virtual std::string last_result_name() const { return {}; }
    virtual bool result_ranking_available() const { return true; }
    virtual bool result_replay_save_available() const { return false; }
    virtual bool result_screen_data(TitleResultScreenData&) const { return false; }
};

class TitleScene final : private TitleFrameServices, private TitleMenuServices {
public:
    TitleState state;
    std::string error;

private:
    SessionState& progress;
    TitleSelectionSettings& settings;
    AnmManager& animations;
    TitleScenePlatform& host;
    FrameScheduler& scheduler;
    TitleSceneResources resources;
    TitleFrameInput input;
    TitleMainMenu main;
    TitleDifficultyMenu difficulty;
    TitleCharacterMenu character;
    TitleStageMenu stage;
    FrameCallback stage_text_draw{};
    TitleReplayMenu replay;
    TitleOptionVolumes option_volumes;
    TitleOptionsMenu option;
    TitleKeyConfigHost key_config_host;
    TitleKeyConfigMenu key_config;
    TitleMusicRoomMenu music_room;
    std::unique_ptr<TitleMusicRoomView> music_room_view;
    TitleManualMenu manual;
    TitleResultEntry result;
    std::unique_ptr<TitlePlayerDataMenu> player_data;
    std::unique_ptr<TitlePlayerDataView> player_data_view;
    std::unique_ptr<TitleSpellPracticeMenu> spell_practice;
    std::unique_ptr<TitleSpellPracticeView> spell_practice_view;
    i32 spell_menu_delay_ = 0;
    SpellPracticeInput delayed_spell_input_;
    std::string spell_practice_launch_status_;
    SpellPracticeCommand pending_spell_;
    SpellPracticeCommand spell_restore_{};
    bool spell_restore_pending_ = false;
    i32 spell_launch_age_ = -1;
    bool check(bool, const std::string&);
    bool clear_title_overlay() override;
    bool queue_title_music(i32 code, i32 value, const std::string&) override;
    bool clear_current_wave() override;
    bool title_exit(i32) override;
    bool fade_out_title_music() override;
    bool update_title_menu(TitleScreen, const TitleFrameInput&) override;
    bool update_player_data(const TitleFrameInput&);
    bool update_spell_practice(const TitleFrameInput&);
    bool restore_practice_selection() override;
    bool restore_spell_practice_selection() override;
    bool draw_title_replay_texts() override;
    bool draw_title_music_room_texts() override;
    bool sound(i32) override;
    bool begin_transition(u32&) override;
    bool start_game(i32) override;
    bool character_cleared(i32 character, i32 difficulty) override;
    bool extra_unlocked(i32 character) override;
    bool practice_stage_played(i32 character, i32 difficulty, i32 stage) override;

public:
    const TitleOptionsMenu& option_menu() const noexcept { return option; }
    TitleOptionsMenu& option_menu() noexcept { return option; }
    const TitleKeyConfigMenu& key_config_menu() const noexcept { return key_config; }
    void connect_key_config(std::function<bool()> present, std::function<u32()> buttons,
                            std::function<std::array<i16, 5>()> bindings,
                            std::function<bool(const std::array<i16, 5>&)> persist) {
        key_config_host.present = std::move(present);
        key_config_host.buttons = std::move(buttons);
        key_config_host.bindings = std::move(bindings);
        key_config_host.persist = std::move(persist);
    }
    const TitleMusicRoomMenu& music_room_menu() const noexcept { return music_room; }
    const TitleManualMenu& manual_menu() const noexcept { return manual; }
    const TitlePlayerDataMenu* player_data_menu() const noexcept { return player_data.get(); }
    const TitleSpellPracticeMenu* spell_practice_menu() const noexcept { return spell_practice.get(); }
    bool spell_practice_launch_pending() const noexcept { return !spell_practice_launch_status_.empty(); }
    const std::string& spell_practice_launch_status() const noexcept { return spell_practice_launch_status_; }
    TitleFrame frame;
    TitleScene(SessionState&, TitleSelectionSettings&, AnmManager&, TitleScenePlatform&, FrameScheduler&,
               TitleSceneResources = {});
    ~TitleScene();
    TitleScene(const TitleScene&) = delete;
    TitleScene& operator=(const TitleScene&) = delete;
    void controls(const TitleFrameInput& value) noexcept {
        input = value;
        frame.controls(value);
    }
};

}
