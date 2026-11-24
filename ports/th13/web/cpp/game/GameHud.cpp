#include "GameHud.hpp"
#include <cstdio>

namespace th13 {

namespace {
constexpr u32 kTranceGaugeDimmed = 1u;
constexpr i32 kOpeningSignatureBank = 1;
constexpr u32 kOpeningSignaturePhaseTicks = 4;
}

bool GameHud::check(bool ok) {
    if (!ok && error.empty()) error = animations.error.empty() ? "HUD animation unavailable" : animations.error;
    return ok;
}

u32 GameHud::create(i32 bank, i32 script, u32 ordering) {
    const u32 handle = animations.create(bank, script, -1, ordering);
    check(handle != 0);
    return handle;
}

u32 GameHud::widget(i32 script) {
    const u32 handle = create(front, script, 0);
    if (auto* vm = animations.registry.find(handle)) vm->visual.render_flags &= ~0xc000u;
    return handle;
}

bool GameHud::direct_interrupt(u32 handle, i32 label) {
    auto* vm = animations.registry.find(handle);
    if (!vm) return check(false);
    vm->pending_interrupt = label;
    return true;
}

GameHud::~GameHud() { release_opening_signature(); }

void GameHud::release_opening_signature() {
    if (opening_signature) animations.retire(opening_signature);
    opening_signature = 0;
    opening_signature_right = 0;
}

bool GameHud::begin_opening_signature(bool title_flow_ran) {
    if (progress.practice_mode == 0 || (progress.run_mode_flags & 0x30u) != 0) return true;
    if (title_flow_ran) return true;
    if (!animations.resource(kOpeningSignatureBank)) return true;
    release_opening_signature();
    const u32 root = create(kOpeningSignatureBank, 0, 0);
    if (!root) return false;
    opening_signature = root;
    auto* vm = animations.registry.find(root);
    if (!vm) {
        release_opening_signature();
        return check(false);
    }
    auto* right = animations.registry.find_child_script(*vm, 1, 0);
    if (!right) {
        release_opening_signature();
        error = "sig.anm script1 child VM missing";
        return false;
    }
    opening_signature_right = animations.registry.handle(*right);
    for (u32 i = 0; i < kOpeningSignaturePhaseTicks; i++) {
        if (animations.tick_instance(*vm) < 0 || animations.tick_instance(*right) < 0) {
            release_opening_signature();
            return check(false);
        }
    }
    return true;
}

bool GameHud::life(i32 lives, i32 pieces) {
    if (!life_icons[0]) return true;
    if (lives > 8 || lives < 0) {
        error = "HUD life values outside original limits";
        return false;
    }
    for (i32 i = 0; i < lives; i++)
        if (!direct_interrupt(life_icons[u32(i)], 2)) return false;
    if (lives > 7) return true;
    const i32 threshold = life_piece_threshold(progress.life_piece_threshold_index);
    if (!direct_interrupt(life_icons[u32(lives)], (pieces * 5) / threshold + 7)) return false;
    for (i32 i = lives + 1; i < 8; i++)
        if (!direct_interrupt(life_icons[u32(i)], 3)) return false;
    return true;
}

bool GameHud::bombs(i32 stock, i32 pieces) {
    if (!bomb_icons[0]) return true;
    if (stock < 0 || stock > 9) {
        error = "HUD bomb stock outside original limits";
        return false;
    }
    for (i32 i = 0; i < stock && i < 8; i++)
        if (!direct_interrupt(bomb_icons[u32(i)], 2)) return false;
    if (stock > 7) return true;
    if (!direct_interrupt(bomb_icons[u32(stock)], (pieces * 6) / 8 + 7)) return false;
    for (i32 i = stock + 1; i < 8; i++)
        if (!direct_interrupt(bomb_icons[u32(i)], 3)) return false;
    return true;
}

bool GameHud::stage_logo(i32 scene_destination) {
    if (scene_destination == 8 || (progress.run_mode_flags & 0x40u)) return true;
    if (stage_logo_bank < 0) {
        error = "Stage logo animation bank unavailable";
        return false;
    }
    return create(stage_logo_bank, 0, 0) != 0;
}

bool GameHud::stage_logo_title(i32 scene_destination) {
    if (scene_destination == 8 || (progress.run_mode_flags & 0x40u) || (progress.run_mode_flags & 0x30u) == 0x20u)
        return true;
    if (stage_logo_bank < 0) return true;
    return create(stage_logo_bank, 1, 0) != 0;
}

bool GameHud::stage_music_logo() {
    if (stage_logo_bank < 0) {
        error = "Stage logo animation bank unavailable";
        return false;
    }
    return create(stage_logo_bank, 2, 0) != 0;
}

bool GameHud::begin_stage_clear() {
    clear_banner = create(front, 115, 0);
    if (!clear_banner) return false;
    flags |= 0x100u;
    clear_age.set(0);
    return true;
}

bool GameHud::finish_stage_clear() {
    if (!check(animations.interrupt(clear_banner, 1, false))) return false;
    flags &= ~0x100u;
    clear_age.set(0);
    return true;
}

bool GameHud::initialize(const HudEntryContext& context) {
    if (!error.empty()) return false;
    if (!begin_opening_signature(context.title_flow_ran)) return false;
    if (!root && (root = widget(0)) == 0) return false;
    if (!life_icons[0]) {
        for (u32 i = 0; i < 8; i++) {
            life_icons[i] = create(front, i32(i) + 29, 0);
            auto* vm = animations.registry.find(life_icons[i]);
            if (!vm) return false;
            vm->visual.render_flags &= ~0xc000u;
        }
        for (u32 i = 0; i < 8; i++) {
            bomb_icons[i] = create(front, i32(i) + 37, 0);
            auto* vm = animations.registry.find(bomb_icons[i]);
            if (!vm) return false;
            vm->visual.render_flags &= ~0xc000u;
        }
    }
    if (!boss_hud.initialize()) {
        error = boss_hud.error;
        return false;
    }
    if (!life(player.extra_lives, player.life_pieces) || !bombs(player.bombs, player.bomb_pieces)) return false;
    if (!stage_logo_title(context.scene_destination)) return false;
    if (progress.stage == 1 && progress.practice_mode == 0 && progress.continues == 0 && !create(front, 67, 0))
        return false;
    if (progress.difficulty < 0 || progress.difficulty > 5) {
        error = "HUD difficulty outside front animation range";
        return false;
    }
    if (context.new_run) {
        difficulty_notice = widget(79 + progress.difficulty);
        if (!difficulty_notice || !animations.interrupt(difficulty_notice, 3, false)) return false;
    }
    difficulty_label = widget(85 + progress.difficulty);
    if (!difficulty_label || !animations.interrupt(difficulty_notice, 3, false)) return false;
    if (context.new_run) {
        gauge_main = widget(171);
        if (!gauge_main) return false;
        flags &= ~kTranceGaugeDimmed;
        trance_skull_label = 0;
        if (!update_trance()) return false;
    }
    if (!trance_label_refresh()) return false;
    return true;
}

bool GameHud::reset_for_retry() {
    prepare_stage_assets();
    return life(player.extra_lives, player.life_pieces) && bombs(player.bombs, player.bomb_pieces);
}

bool GameHud::update_trance(bool force_label) {
    if (!gauge_main) return true;
    auto* root = animations.registry.find(gauge_main);
    if (!root) {
        gauge_main = 0;
        trance_skull_label = 0;
        return true;
    }
    i32 value = trance_value(progress);
    if (value < 0) value = 0;
    if (value > kTranceMax) value = kTranceMax;
    if (auto* skull = animations.registry.find_child_script(*root, 172, 0)) {
        const i32 label = value >= kTranceSegment ? 2 : 3;
        if (force_label || trance_skull_label != label) {
            skull->pending_interrupt = label;
            trance_skull_label = label;
        }
    }
    if (auto* filled = animations.registry.find_child_script(*root, 173, 0)) {
        const float completed = float(value / kTranceSegment) / 3.f;
        filled->visual.scale.x = completed;
        filled->visual.uv_scale.x = completed;
        filled->visual.flags |= 0x18;
    }
    if (auto* partial = animations.registry.find_child_script(*root, 175, 0)) {
        const float fraction = float(value % kTranceSegment) / float(kTranceSegment);
        partial->visual.scale.x = fraction;
        partial->visual.uv_scale.x = fraction;
        partial->visual.translation.x = float(value / kTranceSegment) * 16.f;
        partial->visual.flags |= 0x18;
    }
    return true;
}

}
