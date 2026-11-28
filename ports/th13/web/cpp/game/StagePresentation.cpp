#include "StagePresentation.hpp"
#include "AnmCoordinates.hpp"
#include "AnmOverlay.hpp"
#include "StageMath.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace th13 {

using AnmRandomObserver = void (*)(const AnmVm&, i32, i32, i32, u32, u32, u32, u32, bool);
extern AnmRandomObserver anm_random_observer;

u32 StagePresentation::debug_pass_mask = 0xf;

namespace {
constexpr float kGameToScreenX = 0.0f, kGameToScreenY = 0.0f;
i32 ship_tilt(u32 held, float player_x, bool touch_active, const AnmVm* ship) {
    const i32 keyboard_tilt = ((held & 0x80u) != 0) - ((held & 0x40u) != 0);
    if ((held & 0xf0u) || !touch_active || !ship || !ship->resource) return keyboard_tilt;
    const float x = player_x + kGameToScreenX;
    const float previous_x = ship->visual.translation.x;
    return (x > previous_x) - (x < previous_x);
}

bool shot_script_has_interrupt(const AnmResource& resource, u32 script, i32 id) {
    if (script >= resource.scripts.size()) return false;
    const auto& bytes = resource.scripts[script].bytes;
    for (u32 off = 0; off + 8 <= bytes.size();) {
        i16 opcode;
        u16 length;
        std::memcpy(&opcode, bytes.data() + off, sizeof opcode);
        std::memcpy(&length, bytes.data() + off + 2, sizeof length);
        if (opcode == -1) break;
        if (length < 8 || length % 4 || off + length > bytes.size()) break;
        if (opcode == 5 && length >= 12) {
            i32 label;
            std::memcpy(&label, bytes.data() + off + 8, sizeof label);
            if (label == id) return true;
        }
        off += length;
    }
    return false;
}

i32 pause_player_effect(AnmVm& vm) {
    auto* manager = static_cast<AnmManager*>(vm.object_host);
    if (manager)
        for (auto* child : manager->registry.children(vm)) {
            child->pre_tick = pause_player_effect;
            pause_player_effect(*child);
        }
    return vm.environment && vm.environment->paused ? 1 : 0;
}
}

StagePresentation::StagePresentation(AnmManager& manager, AnmRenderer& draw, Banks banks, i32 player_shot_sprite)
    : animations(manager), renderer(draw), banks(banks), random(manager.random_source()),
      player_shot_sprite_(player_shot_sprite) {
    install_main_update_hook();
}

void StagePresentation::install_main_update_hook() {
    battle_ = nullptr;
    frame_synced_ = false;
    animations.after_main_update = [this] {
        const bool updated = post_chain_update();
        if (!updated && !error_.empty()) animations.error = error_;
        return updated;
    };
    animations.after_main_update_owner = this;
}

StagePresentation::~StagePresentation() {
    if (animations.after_main_update_owner == this) {
        animations.after_main_update = nullptr;
        animations.after_main_update_owner = nullptr;
    }
}

void StagePresentation::rebuild_bullet_lookup() {
    bullet_lookup_.clear();
    for (size_t index = 0; index < bullets_.size(); ++index)
        if (bullets_[index].active) bullet_lookup_.emplace_back(bullets_[index].generation, u32(index));
    std::sort(bullet_lookup_.begin(), bullet_lookup_.end());
}

StagePresentation::BulletSlot* StagePresentation::bullet_slot(StageBullet& bullet) {
    BulletSlot* slot = nullptr;
    const auto hint =
        std::lower_bound(bullet_lookup_.begin(), bullet_lookup_.end(), std::pair<u32, u32>{bullet.generation, 0u});
    if (hint != bullet_lookup_.end() && hint->first == bullet.generation && hint->second < bullets_.size()) {
        BulletSlot& entry = bullets_[hint->second];
        if (entry.active && entry.generation == bullet.generation) slot = &entry;
    }
    if (!slot)
        for (auto& entry : bullets_)
            if (entry.active && entry.generation == bullet.generation) {
                slot = &entry;
                break;
            }
    if (!slot) {
        for (auto& entry : bullets_)
            if (!entry.active) {
                slot = &entry;
                break;
            }
        if (!slot) {
            bullets_.emplace_back();
            slot = &bullets_.back();
        }
        if (slot->vm && !animations.release_tree(*slot->vm)) {
            fail(animations.error);
            return nullptr;
        }
        *slot = BulletSlot{};
        slot->generation = bullet.generation;
        slot->active = true;
    }
    slot->source.type = i32(bullet.sprite);
    slot->source.color = i32(bullet.subtype);
    if (!slot->vm) slot->vm = std::make_unique<AnmVm>();
    return slot;
}

void StagePresentation::bullet_animation_geometry(StageBullet& bullet, const AnmVm& vm) {
    bullet.animation_ready = vm.variables.integers[0] != 0;
    bullet.animation_geometry = vm.sprite != nullptr;
    if (vm.sprite) {
        bullet.sprite_width = vm.sprite->width;
        bullet.sprite_height = vm.sprite->height;
    }
}

bool StagePresentation::initialize_bullet(StageBullet& bullet, i32 interrupt) {
    auto* resource = animations.resource(banks.bullet);
    const auto* appearance = bullet_appearance(i32(bullet.sprite));
    if (!resource || !appearance) return fail("Bullet animation resource/type unavailable");
    auto* slot = bullet_slot(bullet);
    if (!slot) return false;
    auto& vm = *slot->vm;
    vm.environment = &animations.scene_environment();
    vm.object_host = &animations;
    if (!vm.bind(*resource, u32(appearance->script))) return fail(vm.error);
    vm.sprite_source = &slot->source;
    vm.visual.render_flags = (vm.visual.render_flags & ~0x4000u) | 0x8000u;
    if (vm.tick(random, animations.rate) < 0) return fail(vm.error);
    if ((vm.visual.render_flags & 0xc000u) == 0x8000u)
        vm.visual.render_flags = (vm.visual.render_flags & ~0x8000u) | 0x4000u;
    vm.visual.render_flags |= 0x40000;
    vm.pending_interrupt = interrupt;
    vm.visual.translation = {};
    slot->script = appearance->script;
    slot->color = i32(bullet.subtype);
    bullet_animation_geometry(bullet, vm);
    return true;
}

bool StagePresentation::tick_bullet(StageBullet& bullet, bool birth) {
    const auto* appearance = bullet_appearance(i32(bullet.sprite));
    if (!appearance) return fail("Bullet animation type unavailable");
    auto* slot = bullet_slot(bullet);
    if (!slot) return false;
    if (!slot->vm->resource || slot->script != appearance->script || slot->color != i32(bullet.subtype)) {
        if (!initialize_bullet(bullet, bullet.state == 2 ? bullet.delay : 2)) return false;
    }
    auto& vm = *slot->vm;
    if (bullet.animation_interrupt) vm.pending_interrupt = bullet.animation_interrupt;
    bullet.animation_interrupt = 0;
    if (bullet.animation_blend >= 0) {
        vm.visual.flags = (vm.visual.flags & ~0x1e0u) | (u32(bullet.animation_blend) << 5);
        bullet.animation_blend = -1;
    }
    const int result = vm.tick(random, animations.rate);
    if (result < 0) return fail("Bullet animation: " + vm.error);
    vm.variables.position = {bullet.x, bullet.y, bullet.z};
    if (vm.visual.render_flags & 0x40u) {
        vm.variables.rotation.z = wrap_angle(float(bullet.angle + 1.5707964f));
        vm.visual.flags |= 0x4u;
    }
    if (bullet.flags & 0x40u) {
        vm.visual.secondary_scale = {bullet.hit_scale, bullet.hit_scale};
        vm.visual.flags |= 0x8u;
    }
    bullet_animation_geometry(bullet, vm);
    if (result > 0) {
        bullet.alive = false;
        slot->active = false;
    }
    (void)birth;
    return true;
}

bool StagePresentation::bullet_cancel(i32 script, const Vec3& position, const Vec3& displacement, bool interpolate) {
    const u32 handle = animations.create(banks.bullet, script, -1, 0, position, 0, false);
    auto* vm = animations.registry.find(handle);
    if (!vm) return fail("Bullet cancel particle: " + animations.error);
    if (anm_random_observer) anm_random_observer(*vm, -13, script, 0, 0, 0, 0, 0, false);
    vm->visual.render_flags |= 0x40000u;
    if (interpolate)
        vm->interpolators.position.begin(30, 6, {0, 0, 0}, {displacement.x, displacement.y, displacement.z});
    bullet_cancel_handles_.push_back(handle);
    return true;
}

bool StagePresentation::beam_hit_effect(const Vec3& position, const Vec3& displacement, float angle) {
    const u32 handle = animations.create(banks.player, 15, -1, 0, position, 0, false);
    auto* vm = animations.registry.find(handle);
    if (!vm) return fail("Player beam hit effect: " + animations.error);
    vm->visual.flags |= 4u;
    vm->variables.rotation.z = angle;
    vm->interpolators.position.begin(20, 4, {0, 0, 0}, {displacement.x, displacement.y, displacement.z});
    player_effect_handles_.push_back(handle);
    return true;
}

bool StagePresentation::youmu_charge_effect(i32 action, const Vec3& position) {
    if (action == 0) {
        youmu_charge_handle_ = animations.create(banks.player, 18, -1, 0, position, 0, false);
        if (!youmu_charge_handle_) return fail("Youmu charge effect: " + animations.error);
        player_effect_handles_.push_back(youmu_charge_handle_);
    } else if (action == 1 && youmu_charge_handle_) {
        if (!animations.interrupt(youmu_charge_handle_, 2, false))
            return fail("Youmu charge interrupt: " + animations.error);
    } else if (action == 2 && youmu_charge_handle_) {
        if (!animations.retire(youmu_charge_handle_)) return fail("Youmu charge retirement: " + animations.error);
    } else if (action == 3) {
        if (auto* vm = animations.registry.find(youmu_charge_handle_))
            vm->visual.translation = position;
        else
            youmu_charge_handle_ = 0;
    }
    return true;
}

void StagePresentation::release_stage() {
    while (!fog_handles_.empty())
        retire_enemy_fog(fog_handles_.begin()->first);
    for (auto& enemy : enemies_)
        if (enemy.vm) animations.release_tree(*enemy.vm);
    enemies_.clear();
    for (auto& bullet : bullets_)
        if (bullet.vm) animations.release_tree(*bullet.vm);
    bullets_.clear();
    for (auto& vm : item_vms_)
        vm.reset();
    for (auto& vm : item_side_vms_)
        vm.reset();
    for (u32& handle : spirit_handles_) {
        if (handle) animations.retire(handle);
        handle = 0;
    }
    spirit_serials_.fill(0);
    spirit_scripts_.fill(0);
    stage_file_offsets_.clear();
    stage_offsets_source_ = nullptr;
}

void StagePresentation::release() {
    battle_ = nullptr;
    frame_synced_ = false;
    for (auto& handle : bullet_cancel_handles_)
        if (handle) animations.retire(handle);
    bullet_cancel_handles_.clear();
    if (trance_handle_) animations.retire(trance_handle_);
    trance_was_active_ = false;
    while (!fog_handles_.empty())
        retire_enemy_fog(fog_handles_.begin()->first);
    for (auto& enemy : enemies_)
        if (enemy.vm) animations.release_tree(*enemy.vm);
    enemies_.clear();
    for (auto& bullet : bullets_)
        if (bullet.vm) animations.release_tree(*bullet.vm);
    bullets_.clear();
    for (u32& handle : shot_handles_) {
        if (auto* vm = animations.registry.find(handle)) animations.release_tree(*vm);
        handle = 0;
    }
    shot_handles_.clear();
    for (auto& shot : shots_)
        if (shot && shot->resource) animations.release_tree(*shot);
    shots_.clear();
    shot_generations_.clear();
    for (auto& corpse : shot_fades_) {
        if (auto* vm = animations.registry.find(corpse.handle)) animations.release_tree(*vm);
        corpse.handle = 0;
    }
    shot_fades_.clear();
    shot_faded_generations_.clear();
    if (player_) {
        animations.release_tree(*player_);
        player_.reset();
    }
    for (u32 handle : player_effect_handles_)
        if (auto* vm = animations.registry.find(handle)) animations.release_tree(*vm);
    player_effect_handles_.clear();
    youmu_charge_handle_ = 0;
    for (u32& handle : spirit_handles_) {
        if (handle && !animations.retire(handle)) fail("Spirit animation retire failed: " + animations.error);
        handle = 0;
    }
    spirit_serials_.fill(0);
    spirit_scripts_.fill(0);
    for (auto& vm : item_vms_)
        vm.reset();
    for (auto& vm : item_side_vms_)
        vm.reset();
    for (u32& handle : youmu_slash_handles_) {
        if (handle && !animations.retire(handle)) fail("Youmu slash retire failed: " + animations.error);
        handle = 0;
    }
    for (u32 handle : youmu_slash_extra_)
        if (handle && !animations.retire(handle)) fail("Youmu slash retire failed: " + animations.error);
    youmu_slash_extra_.clear();
    focus_handle_ = 0;
    option_handles_.fill(0);
    full_option_handles_.fill(0);
    bomb_orb_handles_.fill(0);
    bomb_orb_exploding_.fill(false);
    bomb_generation_ = bomb_background_ = bomb_beam_ = 0;
    bomb_ray_handles_.fill(0);
    option_character_ = option_count_ = -1;
    full_options_ = false;
    option_rebuild_pending_ = false;
    options_hidden_ = false;
}

u32 StagePresentation::create_player_effect(i32 bank, i32 script, i32 layer, u32 ordering, const Vec3& position) {
    auto* source = animations.script_template(bank, u32(script));
    if (!source) {
        fail("Player effect template unavailable");
        return 0;
    }
    auto* vm = animations.registry.allocate();
    if (!animations.bind_template(*vm, bank, script)) {
        animations.registry.discard(*vm);
        fail("Player effect binding failed: " + animations.error);
        return 0;
    }
    if (anm_random_observer) anm_random_observer(*vm, -10, script, i32(ordering), u32(bank), 0, 0, 0, false);
    vm->instruction_offset = source->instruction_offset;
    vm->visual.render_flags |= 0x200;
    if (layer >= 0) {
        vm->visual.layer = layer;
        if (layer < 21) vm->visual.render_flags |= 0x40000;
    }
    vm->visual.translation = position;
    vm->variables.rotation.z = 0;
    vm->pre_tick = pause_player_effect;
    if (!animations.prime_instance(*vm)) {
        fail("Player effect initial tick failed: " + animations.error);
        return 0;
    }
    if (bank == banks.effect && script == 22) {
        for (auto* child : animations.registry.children(*vm))
            child->visual.set_layer(layer);
    }
    pause_player_effect(*vm);
    vm->ordering = ordering;
    const u32 handle = animations.registry.submit(*vm, ordering);
    if (!handle) {
        fail("Player effect creation failed: " + animations.error);
        return 0;
    }
    player_effect_handles_.push_back(handle);
    return handle;
}

bool StagePresentation::end_player_effect(u32& handle, bool retire) {
    if (!handle) return true;
    const bool ok = retire ? animations.retire(handle) : animations.interrupt(handle, 1, false);
    handle = 0;
    return ok || fail("Player effect release failed: " + animations.error);
}

bool StagePresentation::sync_bomb_effects(const StageBattle& battle) {
    const i32 character = battle.player_character();
    if (battle.bomb_generation != bomb_generation_) {
        bomb_generation_ = battle.bomb_generation;
        if (!end_player_effect(bomb_background_, false) || !end_player_effect(bomb_beam_, false)) return false;
        for (auto& handle : bomb_ray_handles_)
            if (!end_player_effect(handle, false)) return false;
        if (battle.bomb_timer > 0) {
            if (character == 1) {
                bomb_beam_ = create_player_effect(banks.player, 23, -1, 0, battle.bomb_center());
                if (!bomb_beam_) return false;
            }
            constexpr i32 backgrounds[] = {26, 31, 27, 25};
            bomb_background_ = create_player_effect(banks.player, backgrounds[character], -1, 0, battle.bomb_center());
            if (!bomb_background_) return false;
            if (character != 0) {
                auto* vm = animations.registry.find(bomb_background_);
                vm->visual.color = (vm->visual.color & 0xffffffu) | 0xe0000000u;
                vm->interpolators.secondary_alpha.control2[0] = 224;
            }
            if (character == 2)
                for (size_t i = 0; i < bomb_ray_handles_.size(); ++i) {
                    bomb_ray_handles_[i] = create_player_effect(banks.player, 23, -1, 0, battle.bomb_rays[i].origin);
                    if (!bomb_ray_handles_[i]) return false;
                }
        }
    }
    if (battle.bomb_timer <= 0) {
        if (!end_player_effect(bomb_background_, false) || !end_player_effect(bomb_beam_, false)) return false;
        for (auto& handle : bomb_ray_handles_)
            if (!end_player_effect(handle, false)) return false;
        return true;
    }
    if (character == 1) {
        if (auto* vm = animations.registry.find(bomb_beam_)) {
            vm->visual.translation = battle.bomb_center();
            vm->variables.rotation.z = battle.bomb_visual_angle();
            vm->visual.flags |= 4;
        }
    } else if (character == 2) {
        if (auto* vm = animations.registry.find(bomb_background_)) vm->visual.translation = battle.bomb_center();
        for (size_t i = 0; i < bomb_ray_handles_.size(); ++i)
            if (auto* vm = animations.registry.find(bomb_ray_handles_[i])) {
                vm->visual.translation = battle.bomb_rays[i].origin;
                vm->variables.rotation.z = battle.bomb_rays[i].angle;
                vm->visual.flags |= 4;
            }
    }
    return true;
}

bool StagePresentation::sync_player_effects(StageBattle& battle) {
    player_effect_handles_.erase(std::remove_if(player_effect_handles_.begin(), player_effect_handles_.end(),
                                                [&](u32 handle) { return !animations.registry.find(handle); }),
                                 player_effect_handles_.end());
    const bool visible = battle.player_visible();
    const Vec3 position{battle.player_x, battle.player_y, 0};
    if (!sync_bomb_effects(battle)) return false;

    if (visible && battle.focus) {
        if (!animations.registry.find(focus_handle_)) {
            focus_handle_ = create_player_effect(banks.effect, 22, 12, 0, position);
            if (!focus_handle_) return false;
        }
        animations.registry.find(focus_handle_)->visual.translation = position;
    } else if (!end_player_effect(focus_handle_, !visible)) {
        return false;
    }

    if (!sync_player_options(battle)) return false;

    for (size_t i = 0; i < bomb_orb_handles_.size(); ++i) {
        const auto& orb = battle.bomb_orbs[i];
        if (battle.player_character() != 0 || (!orb.active && !orb.exploding)) {
            if (!end_player_effect(bomb_orb_handles_[i], true)) return false;
            bomb_orb_exploding_[i] = false;
            continue;
        }
        if (!animations.registry.find(bomb_orb_handles_[i]) && !orb.exploding) {
            bomb_orb_handles_[i] = create_player_effect(banks.player, 18, -1, 0, {orb.x, orb.y, 0});
            if (!bomb_orb_handles_[i]) return false;
            bomb_orb_exploding_[i] = false;
        }
        if (auto* vm = animations.registry.find(bomb_orb_handles_[i])) vm->visual.translation = {orb.x, orb.y, 0};
        if (orb.exploding && !bomb_orb_exploding_[i]) {
            if (!end_player_effect(bomb_orb_handles_[i], false)) return false;
            bomb_orb_exploding_[i] = true;
        }
    }
    return true;
}

bool StagePresentation::hide_player_options() {
    options_hidden_ = true;
    for (size_t i = 0; i < option_handles_.size(); ++i) {
        for (u32* handle : {&option_handles_[i], &full_option_handles_[i]}) {
            if (!*handle) continue;
            if (!animations.interrupt(*handle, 3, false)) return fail(animations.error);
        }
    }
    return true;
}

bool StagePresentation::interrupt_player_options() {
    for (size_t i = 0; i < option_handles_.size(); ++i) {
        for (u32* handle : {&option_handles_[i], &full_option_handles_[i]}) {
            if (!*handle) continue;
            if (!animations.interrupt(*handle, 1, false)) return fail(animations.error);
            *handle = 0;
        }
    }
    option_character_ = option_count_ = -1;
    full_options_ = false;
    option_rebuild_pending_ = true;
    return true;
}

bool StagePresentation::sync_player_options(StageBattle& battle, bool power_rebuild) {
    if (option_rebuild_pending_) return true;
    if (options_hidden_) {
        const Vec3 anchor{battle.player_x, battle.player_y, 0};
        for (u32 handle : option_handles_)
            if (auto* vm = animations.registry.find(handle)) vm->visual.translation = anchor;
        for (u32 handle : full_option_handles_)
            if (auto* vm = animations.registry.find(handle)) vm->visual.translation = anchor;
        return true;
    }
    const bool visible = battle.player_visible();
    const bool defer_anchor = power_rebuild || option_anchor_deferred_;
    constexpr i32 option_scripts[4] = {16, 18, 19, 15};
    constexpr i32 full_scripts[4] = {17, 20, 21, 17};
    const i32 character = battle.player_character();
    const i32 count = battle.option_count;
    const bool full = visible && battle.full_power();
    if (character < 0 || character > 3 || count < 0 || count > 8)
        return fail("Player option state outside original bounds");
    if (!visible) {
        if (!death_options_queued_) {
            for (u32 handle : option_handles_)
                if (handle && !animations.interrupt(handle, 1, false))
                    return fail("Player option death interrupt failed: " + animations.error);
            for (u32 handle : full_option_handles_)
                if (handle && !animations.interrupt(handle, 1, false))
                    return fail("Full power option death interrupt failed: " + animations.error);
            death_options_queued_ = true;
        }
        if (battle.death_option_rebuild) {
            battle.death_option_rebuild = false;
            for (u32& handle : option_handles_)
                if (!end_player_effect(handle, true)) return false;
            for (u32& handle : full_option_handles_)
                if (!end_player_effect(handle, false)) return false;
            option_character_ = character;
            option_count_ = count;
            full_options_ = false;
            for (size_t i = 0; i < option_handles_.size(); ++i) {
                if (i >= size_t(count)) continue;
                const auto& option = battle.options[i];
                const Vec3 anchor{option.x, option.y, 0};
                option_handles_[i] = create_player_effect(banks.player, option_scripts[character], 12, 0, anchor);
                if (!option_handles_[i]) return false;
            }
        }
        option_anchor_deferred_ = power_rebuild;
        return true;
    }
    death_options_queued_ = false;
    if (character != option_character_ || count != option_count_) {
        for (u32& handle : option_handles_)
            if (!end_player_effect(handle, true)) return false;
        for (u32& handle : full_option_handles_)
            if (!end_player_effect(handle, !visible || full)) return false;
        option_character_ = character;
        option_count_ = count;
        full_options_ = false;
    }
    if (full != full_options_) {
        for (u32& handle : full_option_handles_)
            if (!end_player_effect(handle, !visible)) return false;
        full_options_ = full;
    }
    if (full)
        for (size_t i = 0; i < full_option_handles_.size(); ++i) {
            const auto& option = battle.options[i];
            if (i >= size_t(count) || !option.active) continue;
            const Vec3 anchor{option.x, option.y, 0};
            if (!animations.registry.find(full_option_handles_[i])) {
                full_option_handles_[i] =
                    create_player_effect(banks.player, full_scripts[character], 12, 2, power_rebuild ? Vec3{} : anchor);
                if (!full_option_handles_[i]) return false;
                if (power_rebuild) animations.registry.find(full_option_handles_[i])->visual.translation = {0, -32, 0};
            }
            if (!defer_anchor) animations.registry.find(full_option_handles_[i])->visual.translation = anchor;
        }
    for (size_t i = 0; i < option_handles_.size(); ++i) {
        const auto& option = battle.options[i];
        const bool active = i < size_t(count) && option.active;
        if (!active) {
            if (!end_player_effect(option_handles_[i], true) || !end_player_effect(full_option_handles_[i], true))
                return false;
            continue;
        }
        const Vec3 anchor{option.x, option.y, 0};
        if (!animations.registry.find(option_handles_[i])) {
            option_handles_[i] =
                create_player_effect(banks.player, option_scripts[character], 12, 0, power_rebuild ? Vec3{} : anchor);
            if (!option_handles_[i]) return false;
            if (power_rebuild) animations.registry.find(option_handles_[i])->visual.translation = {0, -32, 0};
        }
        if (!defer_anchor) animations.registry.find(option_handles_[i])->visual.translation = anchor;
    }
    option_anchor_deferred_ = power_rebuild;
    return true;
}

bool StagePresentation::reimu_bomb_orb_birth(const StageBattle& battle, i32 index) {
    if (index < 0 || index >= i32(bomb_orb_handles_.size())) return fail("Reimu bomb orb index outside pool");
    u32& handle = bomb_orb_handles_[size_t(index)];
    if (animations.registry.find(handle)) return fail("Reimu bomb orb slot already active");
    const auto& orb = battle.bomb_orbs[size_t(index)];
    handle = create_player_effect(banks.player, 18, -1, 0, {orb.x, orb.y, 0});
    bomb_orb_exploding_[size_t(index)] = false;
    return handle != 0;
}

bool StagePresentation::initialize_player_shot(PlayerShot& shot, u32 index) {
    if (shot.script < 0) return true;
    if (shot_handles_.size() <= index) shot_handles_.resize(index + 1, 0);
    shot_generations_.resize(shot_handles_.size());
    if (shot_generations_[index] == shot.generation && shot_handles_[index]) {
        auto* existing = animations.registry.find(shot_handles_[index]);
        if (existing && existing->source_script == shot.script) return true;
    }
    if (shot_handles_[index]) {
        auto* stale = animations.registry.find(shot_handles_[index]);
        if (stale && !animations.release_tree(*stale)) return fail("Player shot animation release failed");
        shot_handles_[index] = 0;
    }
    const auto* source = animations.script_template(banks.player, u32(shot.script));
    if (!source) return fail("Player shot animation template unavailable");
    auto* vm = animations.registry.allocate();
    if (!animations.bind_template(*vm, banks.player, shot.script)) {
        animations.registry.discard(*vm);
        return fail("Player shot animation template unavailable");
    }
    if (anm_random_observer) anm_random_observer(*vm, -11, shot.script, 0, u32(banks.player), 0, 0, 0, false);
    vm->instruction_offset = source->instruction_offset;
    vm->visual.render_flags |= 0x200u;
    vm->visual.translation = {};
    vm->variables.rotation.z = 0;
    vm->pre_tick = pause_player_effect;
    if (!animations.prime_instance(*vm)) return fail("Player shot animation initialization: " + animations.error);
    vm->ordering = 0;
    const u32 handle = animations.registry.submit(*vm, 0);
    if (!handle) {
        animations.registry.destroy_tree(*vm);
        return fail("Player shot animation registration: " + animations.error);
    }
    shot_handles_[index] = handle;
    shot_generations_[index] = shot.generation;
    if (vm->visual.render_flags & 0x40u) {
        vm->visual.flags |= 4u;
        vm->variables.rotation.z = shot.angle;
    }
    vm->visual.translation = {shot.x + kGameToScreenX, shot.y + kGameToScreenY, 0.0f};
    return true;
}

bool StagePresentation::post_chain_update() {
    append_item_draws();
    if (!battle_) return true;
    if (!frame_synced_) return true;
    if (!error_.empty()) return true;
    frame_synced_ = false;
    for (size_t i = 0; i < shot_handles_.size() && i < battle_->shots.size(); ++i) {
        auto& shot = battle_->shots[i];
        if (!shot.alive) continue;
        if (shot_generations_[i] != shot.generation) continue;
        const u32 handle = shot_handles_[i];
        if (!handle) continue;
        AnmVm* vm = animations.registry.find(handle);
        if (!vm) {
            shot.animation_finished = true;
            shot_handles_[i] = 0;
            continue;
        }
        if (shot.script < 0 || shot.type == 2) continue;
        Vec3 corners[4];
        if (!anm_quad_positions(*vm, corners)) {
            fail("Player shot geometry snapshot unavailable");
            return false;
        }
        std::array<Vec3, 4> snapshot{};
        std::copy(std::begin(corners), std::end(corners), snapshot.begin());
        battle_->set_player_shot_geometry(i32(i), shot.age, shot.script, shot.generation, snapshot);
    }
    for (size_t n = 0; n < shot_fades_.size();) {
        if (!animations.registry.find(shot_fades_[n].handle))
            shot_fades_.erase(shot_fades_.begin() + i32(n));
        else
            ++n;
    }
    return true;
}

bool StagePresentation::initialize_player_effects(StageBattle& battle) { return sync_player_effects(battle); }

bool StagePresentation::activate_player_effects(StageBattle& battle) {
    for (size_t i = 0; i < option_handles_.size(); ++i) {
        for (u32 handle : {option_handles_[i], full_option_handles_[i]}) {
            if (!animations.registry.find(handle)) continue;
            if (!animations.interrupt(handle, 2, false)) return fail(animations.error);
        }
    }
    option_rebuild_pending_ = false;
    options_hidden_ = false;
    return initialize_player_effects(battle);
}

void StagePresentation::create_color_step(AnmVm* vm) {
    if (vm) vm->interpolators.advance_color(vm->visual, animations.rate);
}

bool StagePresentation::spirit_animation_alive(u32 slot, u32 serial) const noexcept {
    return slot >= spirit_handles_.size() || spirit_serials_[slot] != serial || serial == 0 ||
           animations.registry.find(spirit_handles_[slot]) != nullptr;
}

bool StagePresentation::spirit_pickup(i32 script, const Vec3& position) {
    if (!animations.create_preserve_ip(banks.front, script, -1, 0, position, 0, false))
        return fail("Spirit pickup animation creation failed: " + animations.error);
    return true;
}

bool StagePresentation::spirit_spawn_animation(u32 slot, u32 serial, i32 kind, u32 flags, float x, float y, float z) {
    const Vec3 position{x, y, z};
    if (!animations.create(banks.effect, 0x38, -1, 0, position, 0, false))
        return fail("Spirit birth effect creation failed: " + animations.error);
    const i32 script = (flags & 1u) != 0 ? kind * 6 + 4 : kind * 6 + 1;
    const u32 handle = animations.create(banks.spirit, script, -1, 0, position, 0, false);
    if (!handle) return fail("Spirit animation creation failed: " + animations.error);
    if (slot < spirit_handles_.size()) {
        spirit_handles_[slot] = handle;
        spirit_serials_[slot] = serial;
        spirit_scripts_[slot] = script;
    }
    return true;
}

bool StagePresentation::sync_trance_effect(const StageBattle& battle) {
    const bool active = battle.sim.session && trance_active(*battle.sim.session);
    if (active && !trance_was_active_) {
        auto* effect = animations.resource(banks.effect);
        auto* astral = animations.resource(banks.spirit);
        if (!effect || !astral) return fail("Trance animation banks unavailable");
        trance_handle_ = animations.create(banks.effect, 0, 26, 0);
        auto* vm = animations.registry.find(trance_handle_);
        if (!vm) return fail("Trance effect creation failed: " + animations.error);
        vm->geometry.overlay = std::make_unique<AnmOverlay>();
        vm->geometry.allocation_bytes = 0x1c3c;
        if (!vm->geometry.overlay->initialize(*effect, *astral, *vm, random))
            return fail("Trance panels initialization failed");
        if (!animations.interrupt(trance_handle_, 9, false)) return fail(animations.error);
    } else if (!active && trance_was_active_) {
        if (!animations.interrupt(trance_handle_, 1, false)) return fail(animations.error);
    }
    trance_was_active_ = active;
    return true;
}

bool StagePresentation::sync_item_spirits(StageBattle& battle) {
    auto& spirits = battle.item_manager.spirits();
    for (size_t i = 0; i < spirits.size(); ++i) {
        Spirit& s = spirits[i];
        u32& handle = spirit_handles_[i];
        if (!s.alive) {
            if (handle && !animations.retire(handle))
                return fail("Spirit animation retire failed: " + animations.error);
            handle = 0;
            spirit_serials_[i] = 0;
            continue;
        }
        const i32 script = (s.flags & 1u) != 0 ? s.kind * 6 + 4 : s.kind * 6 + 1;
        AnmVm* vm = animations.registry.find(handle);
        const bool changed = spirit_serials_[i] != s.serial || spirit_scripts_[i] != script;
        if (changed) {
            if (handle && !animations.retire(handle))
                return fail("Spirit animation rebind retire failed: " + animations.error);
            handle = 0;
            vm = nullptr;
        }
        if (!vm && !changed && s.serial != 0) {
            continue;
        }
        if (!vm) {
            handle = animations.create(banks.spirit, script, -1, 0, {s.x, s.y, s.z}, 0, false);
            if (!handle) return fail("Spirit animation creation failed: " + animations.error);
            vm = animations.registry.find(handle);
            spirit_serials_[i] = s.serial;
            spirit_scripts_[i] = script;
        }
        if (s.fade_requested) {
            if (!animations.interrupt(handle, 1, false))
                return fail("Spirit fade interrupt failed: " + animations.error);
            s.fade_requested = false;
        }
        if (vm) vm->visual.translation = {s.x, s.y, s.z};
    }
    return true;
}

bool StagePresentation::sync_youmu_slash(StageBattle& battle) {
    for (const auto& spawn : battle.youmu_slash_spawns_) {
        const u32 handle = animations.create(banks.player, spawn.script, -1, 0, {0.0f, spawn.y, 0.0f}, 0, false);
        if (!handle) return fail("Youmu slash animation creation failed: " + animations.error);
        if (spawn.slot >= 0 && spawn.slot < YoumuBomb::kSlots) {
            youmu_slash_handles_[size_t(spawn.slot)] = handle;
        } else {
            youmu_slash_extra_.push_back(handle);
        }
    }
    for (int slot = 0; slot < YoumuBomb::kSlots; ++slot) {
        u32& handle = youmu_slash_handles_[size_t(slot)];
        AnmVm* vm = animations.registry.find(handle);
        const bool alive = vm && vm->instruction_offset != -1 && !(vm->visual.render_flags & 0x20u);
        battle.youmu_bomb_.set_vm(slot, alive, alive ? vm->variables.rotation.z : 0.0f, battle.sim.time_rate());
        if (!alive && handle) handle = 0;
    }
    return true;
}

const std::vector<i32>& StagePresentation::stage_file_offsets() {
    auto* stage_resource = animations.resource(banks.stage);
    if (stage_offsets_source_ != stage_resource || stage_file_offsets_.empty()) {
        stage_file_offsets_.assign(1, 0);
        i32 prev = -1;
        const auto& scripts = stage_resource->scripts;
        for (size_t i = 0; i < scripts.size(); ++i) {
            const i32 id = scripts[i].source_id;
            if (id <= prev && i > 0) stage_file_offsets_.push_back(i32(i));
            prev = id;
        }
        stage_offsets_source_ = stage_resource;
    }
    return stage_file_offsets_;
}

StagePresentation::ScriptTarget StagePresentation::resolve_script_target(i32 file, i32 script) {
    if (file == 0) return {banks.bullet, script};
    if (file == 1) return {banks.effect, script};
    return {banks.stage, stage_script_global(file, script)};
}

i32 StagePresentation::stage_script_global(i32 file, i32 script) {
    if (file < 2) return script;
    const auto& offsets = stage_file_offsets();
    const size_t index = size_t(file - 2);
    if (index >= offsets.size()) {
        fail("Stage ANM file index beyond merged bank boundaries: " + std::to_string(file));
        return -1;
    }
    return script + offsets[index];
}

bool StagePresentation::sync_items(StageBattle& battle) {
    auto& items = battle.item_manager.items();
    for (size_t i = 0; i < items.size(); ++i) {
        const Item& item = items[i];
        auto& vm = item_vms_[i];
        auto& side = item_side_vms_[i];
        if (!item.alive || item.type < 1 || item.type > 9) {
            vm.reset();
            side.reset();
            continue;
        }
        const i32 script = 89 + item.type;
        const i32 side_script = 98 + item.type;
        if (!vm) {
            vm = std::make_unique<AnmVm>();
            vm->environment = &animations.scene_environment();
            vm->object_host = &animations;
            if (!animations.bind_template_preserve_ip(*vm, banks.bullet, script))
                return fail("Item animation template unavailable: " + animations.error);
            vm->visual.layer = 15;
            vm->visual.render_flags |= 0x200u | 0x40000u;
            if (vm->tick(random, animations.rate) < 0) return fail("Item animation initialization: " + vm->error);
        }
        if (!side) {
            side = std::make_unique<AnmVm>();
            side->environment = &animations.scene_environment();
            side->object_host = &animations;
            if (!animations.bind_template_preserve_ip(*side, banks.bullet, side_script))
                return fail("Item side animation template unavailable: " + animations.error);
            side->visual.layer = 15;
            side->visual.render_flags |= 0x200u | 0x40000u;
            if (side->tick(random, animations.rate) < 0)
                return fail("Item side animation initialization: " + side->error);
        }
        vm->visual.translation = {item.x, item.y, item.z};
        side->visual.translation = {item.x, item.y, item.z};
        if (vm->tick(random, animations.rate) < 0) return fail("Item animation: " + vm->error);
        if (side->tick(random, animations.rate) < 0) return fail("Item side animation: " + side->error);
        const bool on_screen = item.y >= -8.0f;
        vm->visual.flags = (vm->visual.flags & ~1u) | (on_screen ? 1u : 0u);
        side->visual.flags = (side->visual.flags & ~1u) | (on_screen ? 0u : 1u);
        vm->visual.layer = 15;
        side->visual.layer = 15;
        vm->visual.translation = {item.x, item.y, item.z};
        side->visual.translation = on_screen ? Vec3{item.x, item.y, item.z} : Vec3{item.x, 8.0f, item.z};
    }
    return true;
}

void StagePresentation::append_item_draws() {
    for (auto& vm : item_vms_)
        if (vm) animations.registry.append_draw(*vm);
    for (auto& vm : item_side_vms_)
        if (vm) animations.registry.append_draw(*vm);
}

bool StagePresentation::graze_effect(const Vec3& position) {
    const u32 handle = animations.create(banks.effect, 100, -1, 0, position, 0.0f, false);
    if (!handle) return fail("Graze effect creation failed: " + animations.error);
    return true;
}

bool StagePresentation::create_ecl_effect(i32 file, i32 script, const Vec3& position) {
    const auto target = resolve_script_target(file, script);
    if (target.script < 0) return false;
    const u32 effect_handle = animations.create_preserve_ip(target.bank, target.script, -1, 2, position, 0, false);
    if (!effect_handle) return fail("Effect animation creation failed: " + animations.error);
    if (anm_random_observer)
        if (auto* vm = animations.registry.find(effect_handle))
            anm_random_observer(*vm, -12, file, script, i32(target.bank), 0, 0, 0, false);
    create_color_step(animations.registry.find(effect_handle));
    return true;
}

bool StagePresentation::flush_ecl_animations(StageBattle& battle, StageEnemy* constructing, bool slots_only) {
    std::vector<AnmEffectRequest> pending;
    if (slots_only) pending.reserve(battle.sim.anm_effects.size());
    for (const auto& request : battle.sim.anm_effects) {
        if (request.kind == 0) {
            if (slots_only) {
                pending.push_back(request);
                continue;
            }
            if (!create_ecl_effect(request.file, request.script, {request.x, request.y, request.z})) return false;
            continue;
        }
        StageEnemy* enemy = constructing && constructing->serial == request.serial ? constructing : nullptr;
        for (auto& candidate : battle.sim.enemies)
            if (candidate->serial == request.serial) {
                enemy = candidate.get();
                break;
            }
        if (!enemy) continue;
        u32& handle = enemy->anm_slot_handle[request.slot];
        if (handle) {
            if (!animations.retire(handle)) return fail("Slot animation retire failed: " + animations.error);
            handle = 0;
        }
        if (request.script < 0) continue;
        const auto target = resolve_script_target(request.file, request.script);
        if (target.script < 0) return false;
        handle =
            animations.create_preserve_ip(target.bank, target.script, enemy->mirror_script_base + 7, 2, {}, 0, false);
        if (!handle) return fail("Slot animation creation failed: " + animations.error);
        if (request.slot == 0) {
            enemy->anm_slot0_bank = target.bank;
            enemy->anm_slot0_script_offset = target.script - request.script;
        }
        create_color_step(animations.registry.find(handle));
        if (request.slot == 0 && request.opcode != 318) {
            if (AnmVm* vm = animations.registry.find(handle)) {
                const float w = float(vm->visual.sprite_size.x * vm->visual.scale.x);
                const float h = float(vm->visual.sprite_size.y * vm->visual.scale.y);
                enemy->anm_size_a[0] = h;
                enemy->anm_size_a[1] = w;
                if (request.opcode == 306) {
                    enemy->anm_size_b[0] = h;
                    enemy->anm_size_b[1] = w;
                }
            }
        }
    }
    battle.sim.anm_effects.swap(pending);
    return true;
}

bool StagePresentation::enemy_death(i32 file, i32 script, const Vec3& position, float angle) {
    const auto target = resolve_script_target(file, script);
    if (target.script < 0) return false;
    return animations.create(target.bank, target.script, 4, 0, position, angle, false) != 0 ||
           fail("Enemy death animation: " + animations.error);
}

void StagePresentation::retire_enemy_fog(u32 serial) {
    const auto found = fog_handles_.find(serial);
    if (found == fog_handles_.end()) return;
    for (auto& handle : found->second)
        if (!animations.retire(handle)) fail(animations.error);
    fog_handles_.erase(found);
}

bool StagePresentation::enemy_fog(StageEnemy& enemy, bool reset) {
    if (reset || !enemy.fog) retire_enemy_fog(enemy.serial);
    if (!enemy.fog) return error_.empty();
    auto& handles = fog_handles_[enemy.serial];
    for (size_t column = 0; column < handles.size(); ++column) {
        if (!handles[column]) {
            handles[column] = animations.create(0, 82, 29, 0);
            auto* vm = animations.registry.find(handles[column]);
            if (!vm) return fail("Enemy fog render-target animation: " + animations.error);
            vm->visual.flags = (vm->visual.flags & ~0x3e000000u) | (12u << 25);
            vm->visual.flags &= ~0x1e0u;
            vm->visual.render_flags &= ~0x40000u;
            vm->variables.integers[0] = 17;
            vm->geometry.screen_vertices.resize(34);
        }
        auto* vm = animations.registry.find(handles[column]);
        if (!vm) return fail("Enemy fog strip retired while enemy is live");
        if (!reset)
            for (size_t row = 0; row < 17; ++row) {
                vm->geometry.screen_vertices[row * 2] = enemy.fog->vertices[column * 17 + row];
                vm->geometry.screen_vertices[row * 2 + 1] = enemy.fog->vertices[(column + 1) * 17 + row];
            }
    }
    return true;
}

bool StagePresentation::sync(StageBattle& battle, u32 held) {
    battle_ = &battle;
    frame_synced_ = false;
    bullet_cancel_handles_.erase(std::remove_if(bullet_cancel_handles_.begin(), bullet_cancel_handles_.end(),
                                                [this](u32 handle) { return !animations.registry.find(handle); }),
                                 bullet_cancel_handles_.end());
    if (!sync_trance_effect(battle)) return false;
    if (!error_.empty()) return false;
    if (animations.scene_environment().paused) return true;
    auto* stage_resource = animations.resource(banks.stage);
    auto* bullet_resource = animations.resource(banks.bullet);
    auto* player_resource = animations.resource(banks.player);
    if (!stage_resource || !bullet_resource || !player_resource) return fail("Stage presentation banks unavailable");
    battle.begin_player_shot_geometry_snapshot();
    if (!sync_player_effects(battle)) return false;
    if (!battle.defer_item_side) {
        if (!sync_items(battle)) return false;
        if (!sync_item_spirits(battle)) return false;
    }
    if (!battle.youmu_bomb_update && !sync_youmu_slash(battle)) return false;

    if (!flush_ecl_animations(battle)) return false;
    if (enemies_from_ecl && !sync_ecl_enemies(battle)) return false;
    if (!enemies_from_ecl && !sync_enemy_visuals(battle, stage_resource)) return false;
    if (!sync_bullets(battle, bullet_resource)) return false;
    if (!sync_player_shots(battle, player_resource)) return false;
    if (!sync_shot_fades(battle)) return false;
    if (!sync_player_ship(battle, held, player_resource)) return false;
    frame_synced_ = true;
    return true;
}

bool StagePresentation::sync_ecl_enemies(StageBattle& battle) {
    for (auto& enemy : battle.sim.enemies) {
        const bool mirror_input_ready = enemy->mirror_ready || !enemy->threads;
        if ((enemy->flags & 0x200000u) && mirror_input_ready) {
            const float vx =
                enemy->mirror_ready ? enemy->mirror_velocity_x : float(enemy->position[0] - enemy->mirror_prev_x);
            const i32 dir = vx > 0.03f ? 1 : (vx < -0.03f ? -1 : 0);
            if (dir != enemy->mirror_dir) {
                const i32 step = dir == 0 ? (enemy->mirror_dir < 0 ? 3 : 4) : (dir < 0 ? 1 : 2);
                u32& handle = enemy->anm_slot_handle[0];
                Vec3 old_pos{};
                const AnmVm* old_vm = handle ? animations.registry.find(handle) : nullptr;
                if (old_vm) old_pos = old_vm->variables.position;
                if (handle) {
                    if (!animations.retire(handle)) return fail("Mirror retire failed: " + animations.error);
                    handle = 0;
                }
                if (enemy->anm_slot0_bank >= 0) {
                    handle = animations.create_preserve_ip(
                        enemy->anm_slot0_bank, enemy->anm_slot0_script_offset + enemy->anm_base_script + step,
                        enemy->mirror_script_base + 7, 0, old_pos, 0, false);
                } else {
                    const auto target = resolve_script_target(enemy->anm_file, enemy->anm_base_script + step);
                    if (target.script < 0) return false;
                    handle = animations.create_preserve_ip(target.bank, target.script, enemy->mirror_script_base + 7, 0,
                                                           old_pos, 0, false);
                }
                if (!handle) return fail("Mirror animation creation failed: " + animations.error);
                create_color_step(animations.registry.find(handle));
                enemy->mirror_dir = dir;
            }
            enemy->mirror_prev_x = enemy->position[0];
        }
        for (u32 slot = 0; slot < StageEnemy::kAnimationSlots; ++slot)
            if (AnmVm* vm = animations.registry.find(enemy->anm_slot_handle[slot])) {
                const Vec3 offset = enemy->flags & 0x8000000u ? Vec3{} : enemy->anm_slot_offset[slot];
                vm->visual.translation = {offset.x + enemy->position[0], offset.y + enemy->position[1],
                                          enemy->position[2] + offset.z};
                if (!(enemy->flags & 0x8000000u) && (vm->visual.render_flags & 0x40u)) {
                    vm->variables.rotation.z =
                        float(std::atan2(double(enemy->motion_delta[1]), double(enemy->motion_delta[0])));
                    vm->visual.flags |= 4u;
                }
            }
    }
    return true;
}

bool StagePresentation::sync_enemy_visuals(StageBattle& battle, AnmResource* stage_resource) {
    std::vector<u32> alive;
    for (auto& enemy : battle.sim.enemies) {
        if (enemy->dead) continue;
        alive.push_back(enemy->serial);
        EnemyVisual* slot = nullptr;
        for (auto& visual : enemies_)
            if (visual.serial == enemy->serial) {
                slot = &visual;
                break;
            }
        const i32 script = enemy->anm_script[0];
        if (script < 0) continue;
        if (!slot) {
            enemies_.push_back(EnemyVisual{enemy->serial, -1, std::make_unique<AnmVm>()});
            slot = &enemies_.back();
        }
        if (!slot->vm->resource || slot->script != script) {
            slot->vm->environment = &animations.scene_environment();
            slot->vm->object_host = &animations;
            if (!slot->vm->bind(*stage_resource, u32(script)))
                return fail("Enemy animation binding failed: " + slot->vm->error);
            slot->script = script;
        }
        slot->vm->visual.translation = {enemy->position[0] + kGameToScreenX, enemy->position[1] + kGameToScreenY,
                                        enemy->position[2]};
        if (slot->vm->tick(random, animations.rate) < 0) return fail("Enemy animation: " + slot->vm->error);
    }
    for (size_t i = 0; i < enemies_.size();) {
        bool keep = false;
        for (u32 serial : alive)
            if (enemies_[i].serial == serial) {
                keep = true;
                break;
            }
        if (keep) {
            ++i;
            continue;
        }
        if (enemies_[i].vm && !animations.release_tree(*enemies_[i].vm)) return fail("Enemy animation release failed");
        enemies_.erase(enemies_.begin() + i);
    }
    return true;
}

bool StagePresentation::sync_bullets(StageBattle& battle, AnmResource* bullet_resource) {
    if (battle.sim.bullet_animation) {
        live_bullet_generations_.clear();
        for (const auto& bullet : battle.sim.bullets)
            if (bullet.alive) live_bullet_generations_.push_back(bullet.generation);
        std::sort(live_bullet_generations_.begin(), live_bullet_generations_.end());
        for (auto& slot : bullets_)
            slot.active =
                std::binary_search(live_bullet_generations_.begin(), live_bullet_generations_.end(), slot.generation);
        rebuild_bullet_lookup();
    } else {
        if (bullets_.size() < battle.sim.bullets.size()) {
            const size_t wanted = battle.sim.bullets.size();
            while (bullets_.size() < wanted)
                bullets_.push_back(BulletSlot{});
        }
        std::vector<bool> matched(bullets_.size(), false);
        auto reset_slot = [&](BulletSlot& slot) -> bool {
            if (slot.vm && slot.vm->resource && !animations.release_tree(*slot.vm))
                return fail("Bullet animation release failed: " + animations.error);
            slot.vm.reset();
            slot.script = -1;
            slot.color = -1;
            slot.generation = 0;
            slot.active = false;
            return true;
        };
        for (const auto& bullet : battle.sim.bullets) {
            const auto* appearance = bullet_appearance(i32(bullet.sprite));
            size_t slot_index = bullets_.size();
            for (size_t i = 0; i < bullets_.size(); ++i) {
                if (!matched[i] && bullets_[i].active && bullets_[i].generation == bullet.generation) {
                    slot_index = i;
                    break;
                }
            }
            if (slot_index == bullets_.size()) {
                for (size_t i = 0; i < bullets_.size(); ++i) {
                    if (!matched[i] && !bullets_[i].active) {
                        slot_index = i;
                        break;
                    }
                }
                if (slot_index == bullets_.size()) {
                    bullets_.push_back(BulletSlot{});
                    matched.push_back(false);
                }
                if (!reset_slot(bullets_[slot_index])) return false;
            }
            matched[slot_index] = true;
            BulletSlot& slot = bullets_[slot_index];
            slot.active = true;
            slot.generation = bullet.generation;
            slot.source.type = i32(bullet.sprite);
            slot.source.color = i32(bullet.subtype);
            const i32 script = appearance ? appearance->script : -1;
            if (script < 0) continue;
            if (!slot.vm) slot.vm = std::make_unique<AnmVm>();
            if (!slot.vm->resource || slot.script != script || slot.color != i32(bullet.subtype)) {
                slot.vm->environment = &animations.scene_environment();
                slot.vm->object_host = &animations;
                if (!slot.vm->bind(*bullet_resource, u32(script)))
                    return fail("Bullet animation binding failed: " + slot.vm->error);
                slot.vm->sprite_source = &slot.source;
                if (slot.vm->tick(random, animations.rate) < 0) return fail("Bullet initialization: " + slot.vm->error);
                slot.vm->visual.render_flags |= 0x40000;
                slot.vm->pending_interrupt = bullet.state == 2 && bullet.delay ? bullet.delay : 2;
                slot.script = script;
                slot.color = i32(bullet.subtype);
            }
            slot.vm->visual.translation = {bullet.x + kGameToScreenX, bullet.y + kGameToScreenY, bullet.z};
            if (slot.vm->tick(random, animations.rate) < 0) return fail("Bullet animation: " + slot.vm->error);
        }
        for (size_t i = 0; i < bullets_.size(); ++i)
            if (!matched[i]) bullets_[i].active = false;
    }
    return true;
}

bool StagePresentation::sync_player_shots(StageBattle& battle, AnmResource* player_resource) {
    if (shot_handles_.size() < battle.shots.size()) shot_handles_.resize(battle.shots.size(), 0);
    if (shots_.size() < shot_handles_.size()) shots_.resize(shot_handles_.size());
    shot_generations_.resize(shot_handles_.size());
    for (size_t i = 0; i < shot_handles_.size(); ++i) {
        if (i < battle.shots.size() && battle.shots[i].alive) {
            auto& shot = battle.shots[i];
            bool fresh_slot = false;
            if (shot_generations_[i] != shot.generation) {
                size_t source = i + 1;
                while (source < shot_handles_.size() && shot_generations_[source] != shot.generation)
                    ++source;
                if (source < shot_handles_.size()) {
                    std::swap(shot_handles_[i], shot_handles_[source]);
                    std::swap(shots_[i], shots_[source]);
                    std::swap(shot_generations_[i], shot_generations_[source]);
                } else {
                    if (u32 stale = shot_handles_[i]) {
                        if (auto* old = animations.registry.find(stale))
                            if (!animations.release_tree(*old)) return fail("Player shot animation release failed");
                    }
                    shot_handles_[i] = 0;
                    if (shots_[i]) {
                        if (shots_[i]->resource) animations.release_tree(*shots_[i]);
                        shots_[i].reset();
                    }
                    fresh_slot = true;
                }
            }
            const i32 script = shot.script;
            AnmVm* vm = nullptr;
            if (script >= 0) {
                vm = animations.registry.find(shot_handles_[i]);
                if (!vm) {
                    if (!fresh_slot && shot_generations_[i] == shot.generation) {
                        shot.animation_finished = true;
                        shot_handles_[i] = 0;
                        shot_generations_[i] = 0;
                        continue;
                    }
                    if (!initialize_player_shot(shot, u32(i))) return false;
                    vm = animations.registry.find(shot_handles_[i]);
                } else if (vm->source_script != script) {
                    if (!initialize_player_shot(shot, u32(i))) return false;
                    vm = animations.registry.find(shot_handles_[i]);
                }
                if (!vm) return fail("Player shot animation unavailable");
            } else {
                if (!shots_[i]) shots_[i] = std::make_unique<AnmVm>();
                vm = shots_[i].get();
                if (!vm->resource || vm->source_script >= 0) {
                    vm->environment = &animations.scene_environment();
                    vm->initialize_unbound();
                    vm->resource = player_resource;
                    vm->source_script = -1;
                    if (!vm->select_sprite(player_shot_sprite_)) return fail("Player shot sprite unavailable");
                    vm->visible = true;
                    vm->visual.flags |= 1;
                    vm->visual.render_flags |= 0x200;
                    vm->visual.set_layer(11);
                }
                shot_generations_[i] = shot.generation;
            }
            if (shot.state == 2 && shot_faded_generations_.insert(shot.generation).second) {
                if (shot.script >= 0) {
                    if (!animations.interrupt(shot_handles_[i], 1, false))
                        return fail("Player shot fade interrupt failed");
                } else {
                    vm->pending_interrupt = 1;
                }
            }
            if (shot.behavior == 2 && shot.state == 1 && shot.beam_interrupt) {
                if (shot.script >= 0) {
                    if (!animations.interrupt(shot_handles_[i], shot.beam_interrupt, false))
                        return fail("Player beam interrupt failed");
                } else {
                    vm->pending_interrupt = shot.beam_interrupt;
                }
                shot.beam_interrupt = 0;
            }
            vm->visual.translation = {shot.x + kGameToScreenX, shot.y + kGameToScreenY,
                                      shot.hit_fade && !shot.sine_moving ? 0.1f : 0.0f};
            if (vm->visual.render_flags & 0x40u) {
                vm->visual.flags |= 4u;
                vm->variables.rotation.z = shot.angle;
            }
            if (shot.behavior == 2 && shot.state == 1) {
                vm->visual.sprite_size.x = shot.beam_visual_width;
                vm->visual.uv_scale.x = shot.beam_visual_width / 512.f;
                vm->visual.flags |= 0x18u;
            }
        } else {
            const i32 corpse_script = i < battle.shots.size() ? battle.shots[i].script : -1;
            AnmVm* vm = corpse_script >= 0 ? animations.registry.find(shot_handles_[i])
                                           : (shots_[i] ? shots_[i].get() : nullptr);
            bool fade = false;
            PlayerShot corpse_shot{};
            if (i < battle.shots.size() && !battle.shots[i].alive) {
                const PlayerShot& shot = battle.shots[i];
                corpse_shot = shot;
                const bool snapshotted =
                    shot_generations_[i] == shot.generation && vm && vm->source_script == shot.script && shot.type != 2;
                bool outside = false;
                if (snapshotted && shot.age - 1 > 9) {
                    Vec3 corners[4];
                    if (!anm_quad_positions(*vm, corners)) return fail("Player shot death geometry unavailable");
                    outside = true;
                    for (const auto& corner : corners)
                        outside = outside &&
                                  (corner.x <= 128.0f || corner.x >= 512.0f || corner.y <= 16.0f || corner.y >= 464.0f);
                }
                fade = !outside && vm && shot.script >= 0 &&
                       shot_faded_generations_.find(shot.generation) == shot_faded_generations_.end() &&
                       shot_script_has_interrupt(*player_resource, u32(shot.script), 1);
            }
            if (fade) {
                ShotFade corpse;
                corpse.generation = corpse_shot.generation;
                corpse.handle = shot_handles_[i];
                corpse.x = corpse_shot.x;
                corpse.y = corpse_shot.y;
                corpse.angle = corpse_shot.angle;
                corpse.drift = corpse_shot.speed * 0.125f;
                corpse.sine_moving = corpse_shot.sine_moving;
                corpse.sine_motion = corpse_shot.sine_motion;
                vm->pending_interrupt = 1;
                vm->visual.translation = {corpse.x + kGameToScreenX, corpse.y + kGameToScreenY,
                                          corpse.sine_moving ? 0.0f : 0.1f};
                vm->variables.rotation.z = corpse.angle;
                shot_faded_generations_.insert(corpse.generation);
                shot_fades_.push_back(corpse);
                shot_handles_[i] = 0;
                shot_generations_[i] = 0;
                if (shots_[i]) shots_[i].reset();
            } else {
                if (u32 handle = shot_handles_[i]) {
                    if (animations.registry.find(handle)) animations.retire(handle);
                    shot_handles_[i] = 0;
                }
                if (shots_[i]) {
                    if (shots_[i]->resource) animations.release_tree(*shots_[i]);
                    shots_[i].reset();
                }
                shot_generations_[i] = 0;
            }
        }
    }
    return true;
}

bool StagePresentation::sync_shot_fades(StageBattle& battle) {
    for (size_t n = 0; n < shot_fades_.size();) {
        ShotFade& corpse = shot_fades_[n];
        AnmVm* vm = animations.registry.find(corpse.handle);
        if (!vm) {
            shot_fades_.erase(shot_fades_.begin() + i32(n));
            continue;
        }
        if (corpse.age > 0) {
            float vx, vy;
            const float rate = battle.sim.time_rate();
            if (corpse.sine_moving) {
                corpse.sine_motion.advance(rate);
                corpse.x = corpse.sine_motion.pos[0];
                corpse.y = corpse.sine_motion.pos[1];
                corpse.angle = corpse.sine_motion.angle;
            } else {
                polar_xy(corpse.angle, float(corpse.drift * rate), vx, vy);
                corpse.x = round001(float(corpse.x + vx));
                corpse.y = round001(float(corpse.y + vy));
            }
            vm->visual.translation = {corpse.x + kGameToScreenX, corpse.y + kGameToScreenY,
                                      corpse.sine_moving ? 0.0f : 0.1f};
            vm->variables.rotation.z = corpse.angle;
        }
        ++corpse.age;
        ++n;
    }
    return true;
}

bool StagePresentation::sync_player_ship(StageBattle& battle, u32 held, AnmResource* player_resource) {
    if (battle.player_visible()) {
        const i32 tilt = ship_tilt(held, battle.player_x, battle.touch.mode != 0, player_.get());
        if (!player_) player_ = std::make_unique<AnmVm>();
        bool rebound = false;
        if (!player_->resource || tilt != player_tilt_) {
            const i32 script = tilt < 0 ? 1 : tilt > 0 ? 3 : player_tilt_ < 0 ? 2 : player_tilt_ > 0 ? 4 : 0;
            if (player_->resource) animations.release_tree(*player_);
            player_->environment = &animations.scene_environment();
            player_->object_host = &animations;
            if (!player_->bind(*player_resource, u32(script)))
                return fail("Player animation binding failed: " + player_->error);
            player_->visual.set_layer(12);
            player_tilt_ = tilt;
            player_script_ = script;
            rebound = true;
        }
        player_->visual.translation = {battle.player_x + kGameToScreenX, battle.player_y + kGameToScreenY, 0.0f};
        if (player_->tick(random, animations.rate) < 0) return fail("Player animation: " + player_->error);
        if (rebound && player_->tick(random, animations.rate) < 0) return fail("Player animation: " + player_->error);
        player_->visual.flags &= ~0x60000u;
        const bool phase_advanced = battle.player_phase_frame != battle.player_phase_frame_previous;
        if (battle.invulnerable > 0 && phase_advanced && battle.player_phase_frame_previous % 3 == 0) {
            player_->visual.secondary_color = 0xff0000ffu;
            player_->visual.flags |= 0x20000u;
        }
    } else if (player_) {
        animations.release_tree(*player_);
        player_.reset();
        player_tilt_ = 0;
        player_script_ = -1;
    }
    return true;
}

std::string StagePresentation::debug_report() const {
    std::string report;
    char line[256];
    std::snprintf(line, sizeof line, "enemies=%zu bullets=%zu shots=%zu player=%d spirits=%zu items=%zu\n",
                  enemies_.size(),
                  bullets_.empty() ? 0
                                   : std::count_if(bullets_.begin(), bullets_.end(),
                                                   [](const BulletSlot& slot) { return slot.active && slot.vm; }),
                  shot_handles_.size(), player_ ? 1 : 0,
                  size_t(std::count_if(spirit_handles_.begin(), spirit_handles_.end(),
                                       [this](u32 handle) { return handle && animations.registry.find(handle); })),
                  size_t(std::count_if(item_vms_.begin(), item_vms_.end(),
                                       [](const std::unique_ptr<AnmVm>& vm) { return vm != nullptr; })));
    report += line;
    auto describe = [&](const char* kind, const AnmVm& vm, i32 type, i32 color, u32 generation) {
        const auto* source = vm.sprite_resource ? vm.sprite_resource : vm.resource;
        std::snprintf(line, sizeof line,
                      "  %s script=%d sprite=%d size=%.0fx%.0f scale=%.2fx%.2f "
                      "pos=(%.0f,%.0f) rot=%.2f mode=%u blend=%u type=%d color=%d age=%d gen=%u\n",
                      kind, vm.source_script, vm.visual.sprite, double(vm.visual.sprite_size.x),
                      double(vm.visual.sprite_size.y), double(vm.visual.scale.x), double(vm.visual.scale.y),
                      double(vm.visual.translation.x), double(vm.visual.translation.y), double(vm.variables.rotation.z),
                      unsigned(vm.visual.draw_mode()), unsigned(vm.visual.blend_mode()), type, color, vm.age.current,
                      generation);
        report += line;
        (void)source;
    };
    for (const auto& enemy : enemies_)
        if (enemy.vm) describe("enemy", *enemy.vm, enemy.script, -1, 0);
    u32 shown = 0;
    for (const auto& slot : bullets_) {
        if (!slot.active || !slot.vm || shown >= 40) continue;
        describe("bullet", *slot.vm, slot.source.type, slot.source.color, slot.generation);
        ++shown;
    }
    if (!shot_handles_.empty() && shot_handles_[0])
        if (auto* vm = animations.registry.find(shot_handles_[0])) describe("shot", *vm, player_shot_sprite_, -1, 0);
    if (player_) describe("player", *player_, player_script_, player_tilt_, 0);
    auto describe_handle = [&](const char* kind, u32 handle, i32 index) {
        if (auto* vm = animations.registry.find(handle)) describe(kind, *vm, index, -1, 0);
    };
    describe_handle("focus", focus_handle_, 0);
    for (size_t i = 0; i < option_handles_.size(); ++i) {
        describe_handle("option", option_handles_[i], i32(i));
        describe_handle("full-option", full_option_handles_[i], i32(i));
        describe_handle("bomb-orb", bomb_orb_handles_[i], i32(i));
    }
    return report;
}

bool StagePresentation::draw(u32 pass) {
    if (!error_.empty()) return false;
    if (!(debug_pass_mask >> pass & 1u)) return true;
    const auto one = [&](AnmVm& vm) {
        if (!vm.visible) return true;
        return renderer.draw(vm) != -2 || fail(renderer.error.empty() ? "Stage draw failed" : renderer.error);
    };
    switch (pass) {
    case 0:
        for (auto& shot : shots_)
            if (shot) one(*shot);
        break;
    case 1:
        if (player_) {
            const auto visual = player_->visual;
            const auto rotation = player_->variables.rotation;
            const bool ok = one(*player_);
            player_->visual = visual;
            player_->variables.rotation = rotation;
            return ok;
        }
        break;
    case 2:
        for (auto& enemy : enemies_)
            if (enemy.vm) one(*enemy.vm);
        break;
    case 3: {
        draw_bullet_order_.clear();
        for (auto& slot : bullets_)
            if (slot.active && slot.vm) draw_bullet_order_.push_back(&slot);
        std::sort(draw_bullet_order_.begin(), draw_bullet_order_.end(),
                  [](const BulletSlot* a, const BulletSlot* b) { return a->generation > b->generation; });
        for (auto* slot : draw_bullet_order_)
            one(*slot->vm);
        if (renderer.always_hitbox && battle_) {
            if (battle_->player_visible() && battle_->player_phase == StageBattle::PlayerPhase::Active &&
                !battle_->focus) {
                const auto* source = animations.script_template(banks.effect, 22);
                if (!source) return fail("Focus marker template unavailable");
                AnmVm marker = *source;
                if (!marker.select_sprite(23)) return fail("Focus marker sprite unavailable");
                marker.visible = true;
                marker.visual.set_layer(12);
                marker.visual.scale = {1.f, 1.f};
                marker.visual.color = 0xffffffffu;
                marker.visual.translation = {battle_->player_x + kGameToScreenX, battle_->player_y + kGameToScreenY,
                                             0.f};
                if (!one(marker)) return false;
            }
        }
        break;
    }
    default:
        return fail("Stage draw pass outside schedule");
    }
    return error_.empty();
}

}
