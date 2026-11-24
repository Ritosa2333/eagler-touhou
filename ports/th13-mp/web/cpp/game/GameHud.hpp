#pragma once

#include "AnmManager.hpp"
#include "BossPresentation.hpp"
#include "HudDrawData.hpp"
#include "HudFrameData.hpp"
#include "SessionState.hpp"
#include "SpellPresentation.hpp"

namespace th13 {

struct HudEntryContext {
    i32 scene_destination = 0;
    bool new_run = true;
    bool title_flow_ran = false;
};

class GameHud {
    AnmManager& animations;
    SessionState& progress;
    HudPlayerState& player;
    HudScoreState& score;
    i32 front, ascii;

    bool check(bool);
    u32 create(i32 bank, i32 script, u32 ordering = 0, const Vec3& translation = {});
    u32 widget(i32 script);
    bool direct_interrupt(u32, i32);

public:
    bool update_boss(BossFrameInput& view, HudFrameServices& services);
    std::array<u32, 8> life_icons{}, bomb_icons{};
    i32 mp_seat_count = 0;
    std::array<std::array<u32, 8>, 2> mp_life_icons{}, mp_bomb_icons{};
    std::array<std::array<u32, 5>, 2> mp_resource_labels{};
    std::array<u32, 3> mp_shared_labels{};
    static constexpr float kMpRowPitch = 80.f;
    bool challenge = false;
    bool enable_multiplayer(i32 seat_count);
    bool life_seat(i32 seat, i32 lives, i32 pieces, i32 threshold_index);
    bool bombs_seat(i32 seat, i32 stock, i32 pieces);
    u32 root = 0, difficulty_notice = 0, difficulty_label = 0;
    u32 flags = 0;
    Timer intro_age{0, 0, 0, 0, 0};
    Timer frame_age{0, 0, 0, 0, 0};
    Timer clear_age{0, 0, 0, 0, 0};
    u32 clear_banner = 0;
    i32 displayed_score = 0, score_increment = 0;
    std::string error;
    u32 gauge_main = 0;
    i32 trance_skull_label = 0;
    BossPresentation boss_hud;
    SpellPresentation spell_hud;

    GameHud(AnmManager& a, SessionState& p, HudPlayerState& s, HudScoreState& v, i32 front_bank, i32 ascii_bank)
        : animations(a), progress(p), player(s), score(v), front(front_bank), ascii(ascii_bank),
          boss_hud(a, {front_bank, ascii_bank, 8}), spell_hud(a, {4}) {
        frame_age.set(0);
    }
    struct RollbackState {
        std::array<u32, 8> life_icons;
        std::array<u32, 8> bomb_icons;
        i32 mp_seat_count;
        std::array<std::array<u32, 8>, 2> mp_life_icons;
        std::array<std::array<u32, 8>, 2> mp_bomb_icons;
        std::array<std::array<u32, 5>, 2> mp_resource_labels;
        std::array<u32, 3> mp_shared_labels;
        u32 root;
        u32 difficulty_notice;
        u32 difficulty_label;
        u32 flags;
        Timer intro_age;
        Timer frame_age;
        Timer clear_age;
        u32 clear_banner;
        i32 displayed_score;
        i32 score_increment;
        u32 gauge_main;
        i32 trance_skull_label;
        u32 opening_signature;
        u32 opening_signature_right;
        i32 stage_logo_bank;
        BossPresentation::RollbackState boss;
        SpellPresentation::RollbackState spell;
    };
    RollbackState capture_rollback() const {
        return {life_icons,
                bomb_icons,
                mp_seat_count,
                mp_life_icons,
                mp_bomb_icons,
                mp_resource_labels,
                mp_shared_labels,
                root,
                difficulty_notice,
                difficulty_label,
                flags,
                intro_age,
                frame_age,
                clear_age,
                clear_banner,
                displayed_score,
                score_increment,
                gauge_main,
                trance_skull_label,
                opening_signature,
                opening_signature_right,
                stage_logo_bank,
                boss_hud.capture_rollback(),
                spell_hud.capture_rollback()};
    }
    void restore_rollback(const RollbackState& state) {
        life_icons = state.life_icons;
        bomb_icons = state.bomb_icons;
        mp_seat_count = state.mp_seat_count;
        mp_life_icons = state.mp_life_icons;
        mp_bomb_icons = state.mp_bomb_icons;
        mp_resource_labels = state.mp_resource_labels;
        for (u32 i = 0; i < 3; ++i)
            mp_shared_labels[i] = state.mp_shared_labels[i];
        root = state.root;
        difficulty_notice = state.difficulty_notice;
        difficulty_label = state.difficulty_label;
        flags = state.flags;
        intro_age = state.intro_age;
        frame_age = state.frame_age;
        clear_age = state.clear_age;
        clear_banner = state.clear_banner;
        displayed_score = state.displayed_score;
        score_increment = state.score_increment;
        gauge_main = state.gauge_main;
        trance_skull_label = state.trance_skull_label;
        opening_signature = state.opening_signature;
        opening_signature_right = state.opening_signature_right;
        stage_logo_bank = state.stage_logo_bank;
        boss_hud.restore_rollback(state.boss);
        spell_hud.restore_rollback(state.spell);
    }

    ~GameHud();

    bool initialize(const HudEntryContext&);
    bool life(i32 lives, i32 pieces);
    bool bombs(i32 bombs, i32 pieces);
    u32 opening_signature = 0, opening_signature_right = 0;
    bool begin_opening_signature(bool title_flow_ran);
    void release_opening_signature();
    i32 stage_logo_bank = -1;
    bool stage_logo(i32 scene_destination);
    bool stage_logo_title(i32 scene_destination);
    bool stage_music_logo();
    bool update(const HudFrameContext&, HudFrameServices&);
    void update_score();
    bool draw(const HudDrawContext&, HudDrawServices&);
    bool draw_trance_countdown(HudDrawServices&);
    bool reset_for_retry();
    void prepare_stage_assets() noexcept {
        frame_age.set(0);
        displayed_score = score.score;
        boss_hud.prepare_stage_assets();
    }
    bool update_trance(bool force_label = false);
    bool trance_label_refresh() { return update_trance(true); }
    bool begin_stage_clear();
    bool finish_stage_clear();
    void tick_stage_clear() noexcept {
        if (flags & 0x100u) clear_age.tick(&progress.rate);
    }
};

}
