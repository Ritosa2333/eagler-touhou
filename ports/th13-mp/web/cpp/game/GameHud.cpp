#include "GameHud.hpp"
#include "AnmDrawSchedule.hpp"
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

u32 GameHud::create(i32 bank, i32 script, u32 ordering, const Vec3& translation) {
    const u32 handle = animations.create(bank, script, -1, ordering, translation);
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

GameHud::~GameHud() {
    AnmDrawSchedule::set_hud_ghost_icons(animations, {}, 0);
    release_opening_signature();
}

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

namespace {
bool interrupt_life_set(GameHud& hud, AnmManager& animations, const std::array<u32, 8>& icons, i32 lives, i32 pieces,
                        i32 threshold) {
    if (!icons[0]) return true;
    if (lives > 8 || lives < 0 || threshold <= 0) {
        hud.error = "HUD life values outside original limits";
        return false;
    }
    auto interrupt = [&](u32 handle, i32 label) {
        auto* vm = animations.registry.find(handle);
        if (!vm) {
            if (hud.error.empty()) hud.error = "HUD animation unavailable";
            return false;
        }
        vm->pending_interrupt = label;
        return true;
    };
    for (i32 i = 0; i < lives; i++)
        if (!interrupt(icons[u32(i)], 2)) return false;
    if (lives > 7) return true;
    if (!interrupt(icons[u32(lives)], (pieces * 5) / threshold + 7)) return false;
    for (i32 i = lives + 1; i < 8; i++)
        if (!interrupt(icons[u32(i)], 3)) return false;
    return true;
}
bool interrupt_bomb_set(GameHud& hud, AnmManager& animations, const std::array<u32, 8>& icons, i32 stock, i32 pieces) {
    if (!icons[0]) return true;
    if (stock < 0 || stock > 9) {
        hud.error = "HUD bomb stock outside original limits";
        return false;
    }
    auto interrupt = [&](u32 handle, i32 label) {
        auto* vm = animations.registry.find(handle);
        if (!vm) {
            if (hud.error.empty()) hud.error = "HUD animation unavailable";
            return false;
        }
        vm->pending_interrupt = label;
        return true;
    };
    for (i32 i = 0; i < stock && i < 8; i++)
        if (!interrupt(icons[u32(i)], 2)) return false;
    if (stock > 7) return true;
    if (!interrupt(icons[u32(stock)], (pieces * 6) / 8 + 7)) return false;
    for (i32 i = stock + 1; i < 8; i++)
        if (!interrupt(icons[u32(i)], 3)) return false;
    return true;
}
}

bool GameHud::life(i32 lives, i32 pieces) {
    return interrupt_life_set(*this, animations, life_icons, lives, pieces,
                              life_piece_threshold(progress.life_piece_threshold_index));
}

bool GameHud::bombs(i32 stock, i32 pieces) { return interrupt_bomb_set(*this, animations, bomb_icons, stock, pieces); }

bool GameHud::life_seat(i32 seat, i32 lives, i32 pieces, i32 threshold_index) {
    if (seat < 0 || seat >= mp_seat_count) return life(lives, pieces);
    const auto& icons = seat == 0 ? life_icons : mp_life_icons[size_t(seat) - 1];
    if (challenge) {
        if (!icons[0]) return true;
        for (u32 handle : icons) {
            auto* vm = animations.registry.find(handle);
            if (!vm) {
                error = "HUD animation unavailable";
                return false;
            }
            vm->pending_interrupt = 3;
        }
        return true;
    }
    return interrupt_life_set(*this, animations, icons, lives, pieces, life_piece_threshold(threshold_index));
}

bool GameHud::bombs_seat(i32 seat, i32 stock, i32 pieces) {
    if (seat <= 0 || seat >= mp_seat_count) return bombs(stock, pieces);
    return interrupt_bomb_set(*this, animations, mp_bomb_icons[size_t(seat) - 1], stock, pieces);
}

bool GameHud::enable_multiplayer(i32 seat_count) {
    if (seat_count < 2 || seat_count > 3) {
        error = "HUD multiplayer seat count outside 2..3";
        return false;
    }
    if (!life_icons[0]) {
        error = "HUD multiplayer enable before initialize";
        return false;
    }
    if (mp_seat_count == seat_count) return true;
    for (u32 i = 0; i < 8; ++i) {
        for (u32 handle : {life_icons[i], bomb_icons[i]}) {
            auto* vm = animations.registry.find(handle);
            if (!vm) return check(false);
            vm->visual.translation.x = 0.f;
            vm->visual.scale = {.8f, .8f};
        }
        animations.registry.find(bomb_icons[i])->visual.translation.y = -10.f;
    }
    for (i32 seat = 1; seat < seat_count; ++seat) {
        auto& life_set = mp_life_icons[size_t(seat) - 1];
        auto& bomb_set = mp_bomb_icons[size_t(seat) - 1];
        if (!life_set[0] && !bomb_set[0]) {
            for (u32 i = 0; i < 8; i++) {
                life_set[i] = create(front, i32(i) + 29, 0, Vec3{0.f, float(seat) * kMpRowPitch, 0.f});
                auto* vm = animations.registry.find(life_set[i]);
                if (!vm) return false;
                vm->visual.render_flags &= ~0xc000u;
                vm->visual.scale = {.8f, .8f};
            }
            for (u32 i = 0; i < 8; i++) {
                bomb_set[i] = create(front, i32(i) + 37, 0, Vec3{0.f, float(seat) * kMpRowPitch - 10.f, 0.f});
                auto* vm = animations.registry.find(bomb_set[i]);
                if (!vm) return false;
                vm->visual.render_flags &= ~0xc000u;
                vm->visual.scale = {.8f, .8f};
            }
        }
        constexpr i32 scripts[] = {8, 9, 10, 11, 12};
        constexpr float dx[] = {0.f, 16.f, 0.f, 16.f, 0.f};
        constexpr float offsets[] = {0.f, 0.f, -10.f, -10.f, -66.f};
        for (size_t i = 0; i < 5; ++i) {
            if (!mp_resource_labels[size_t(seat) - 1][i])
                mp_resource_labels[size_t(seat) - 1][i] =
                    create(front, scripts[i], 0, Vec3{dx[i], float(seat) * kMpRowPitch + offsets[i], 0.f});
            if (!mp_resource_labels[size_t(seat) - 1][i]) return false;
            auto* label = animations.registry.find(mp_resource_labels[size_t(seat) - 1][i]);
            if (!label) return false;
            label->visual.render_flags &= ~0xc000u;
            label->visual.scale = (i == 1 || i == 3) ? Vec2{.65f, .65f} : Vec2{1.f, 1.f};
        }
    }
    if (mp_seat_count != seat_count) {
        auto* root_vm = animations.registry.find(root);
        const float shared_y = 100.f + kMpRowPitch * float(seat_count) + 8.f;
        if (root_vm) {
            for (i32 script = 19; script <= 24; ++script) {
                if (auto* child = animations.registry.find_child_script(*root_vm, script, 0)) {
                    const i32 seat = (script - 19) / 2;
                    const float original_y = script <= 20 ? 132.f : script <= 22 ? 170.f : 234.f;
                    child->visual.translation = {script <= 22 ? 43.f : 27.f,
                                                 176.f + kMpRowPitch * float(seat) - original_y, 0.f};
                    child->visual.sprite_size.x = seat < seat_count ? 52.8f : 0.f;
                }
            }
            for (i32 script = 25; script <= 28; ++script) {
                if (auto* child = animations.registry.find_child_script(*root_vm, script, 0)) {
                    child->visual.translation = {-16.f, shared_y - 235.f, 0.f};
                    if (script >= 26) mp_shared_labels[size_t(script - 26)] = animations.registry.handle(*child);
                }
            }
        }
        if (root_vm) {
            for (i32 script : {13, 14}) {
                if (auto* child = animations.registry.find_child_script(*root_vm, script, 0)) {
                    child->visual.translation = {-16.f, shared_y - 240.f, 0.f};
                    child->visual.scale = {1.f, 1.f};
                }
            }
            constexpr i32 scripts[] = {8, 9, 10, 11, 12};
            constexpr float dx[] = {0.f, 16.f, 0.f, 16.f, 0.f};
            const float offsets[] = {0.f, 0.f, -10.f, -10.f, -66.f};
            for (size_t i = 0; i < 5; ++i) {
                if (auto* child = animations.registry.find_child_script(*root_vm, scripts[i], 0)) {
                    child->visual.translation.x = dx[i];
                    child->visual.translation.y = offsets[i];
                    child->visual.scale = (i == 1 || i == 3) ? Vec2{.65f, .65f} : Vec2{1.f, 1.f};
                }
            }
        }
    }
    mp_seat_count = seat_count;
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
