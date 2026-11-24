#include "StagePresentation.hpp"
#include "AnmCoordinates.hpp"
#include "AnmOverlay.hpp"
#include "StageMath.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <unordered_map>

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

bool StagePresentation::beam_hit_effect(const Vec3& position, const Vec3& displacement, float angle, i32 seat) {
    const i32 bank = seat ? banks.teammate_player[size_t(seat - 1)] : banks.player;
    const u8 owner = battle_ && battle_->seat_count >= 2 ? u8(seat + 1) : 0;
    const u32 handle = animations.create(bank, 15, -1, 0, position, 0, false, owner);
    auto* vm = animations.registry.find(handle);
    if (!vm) return fail("Player beam hit effect: " + animations.error);
    vm->visual.flags |= 4u;
    vm->variables.rotation.z = angle;
    vm->interpolators.position.begin(20, 4, {0, 0, 0}, {displacement.x, displacement.y, displacement.z});
    player_effect_handles_.push_back(handle);
    return true;
}

bool StagePresentation::youmu_charge_effect(i32 action, const Vec3& position, i32 seat) {
    auto& handle = youmu_charge_handles_[size_t(seat)];
    const i32 bank = seat ? banks.teammate_player[size_t(seat - 1)] : banks.player;
    const u8 owner = battle_ && battle_->seat_count >= 2 ? u8(seat + 1) : 0;
    if (action == 0) {
        handle = animations.create(bank, 18, -1, 0, position, 0, false, owner);
        if (!handle) return fail("Youmu charge effect: " + animations.error);
        player_effect_handles_.push_back(handle);
    } else if (action == 1 && handle) {
        if (!animations.interrupt(handle, 2, false)) return fail("Youmu charge interrupt: " + animations.error);
    } else if (action == 2 && handle) {
        if (!animations.retire(handle)) return fail("Youmu charge retirement: " + animations.error);
    } else if (action == 3) {
        if (auto* vm = animations.registry.find(handle))
            vm->visual.translation = position;
        else
            handle = 0;
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

void StagePresentation::poison_dead_slots_for_test() {
    for (size_t i = 0; i < bullets_.size(); ++i) {
        auto& slot = bullets_[i];
        const bool needed =
            slot.vm != nullptr && (slot.active || animations.registry.is_embedded_referenced(slot.vm.get()));
        if (needed || !slot.vm) continue;
        AnmVm& vm = *slot.vm;
        vm.visual.translation = {float(50000 + i), float(-40000 - int(i)), 111.f};
        vm.variables.position = {-7777.f, 6666.f, -222.f};
        for (i32& v : vm.variables.integers)
            v = 0x24681357;
        vm.visual.color = 0xfeedfaceu;
        vm.visual.flags = 0xaa55aa55u;
        vm.timer.set(4321);
        vm.age.set(8765);
    }
}

bool StagePresentation::capture_presentation(SnapshotData& out) const {
    out.version = SnapshotData::kContractVersion;
    const auto& effects = player_effects_[0];
    for (size_t i = 0; i < out.teammate_effects.size(); ++i)
        out.teammate_effects[i] = player_effects_[i + 1];
    out.embedded_identities.clear();
    out.embedded_identities.reserve(enemies_.size() + bullets_.size() + shots_.size() + 3 + item_vms_.size() +
                                    item_side_vms_.size());
    const auto remember = [&](const auto& vm) { out.embedded_identities.push_back(vm.get()); };
    for (const auto& enemy : enemies_)
        remember(enemy.vm);
    for (const auto& bullet : bullets_) {
        const bool keep_vm =
            bullet.vm != nullptr && (bullet.active || animations.registry.is_embedded_referenced(bullet.vm.get()));
        out.embedded_identities.push_back(keep_vm ? bullet.vm.get() : nullptr);
    }
    for (const auto& shot : shots_)
        remember(shot);
    remember(player_);
    for (const auto& vm : teammate_players_)
        remember(vm);
    for (const auto& vm : item_vms_)
        remember(vm);
    for (const auto& vm : item_side_vms_)
        remember(vm);
    out.enemies.resize(enemies_.size());
    for (size_t i = 0; i < enemies_.size(); ++i) {
        const auto& enemy = enemies_[i];
        auto& data = out.enemies[i];
        data.serial = enemy.serial;
        data.script = enemy.script;
        data.has_vm = enemy.vm != nullptr;
        if (enemy.vm)
            data.vm = *enemy.vm;
        else if (data.vm.resource || !data.vm.geometry.empty() || !data.vm.error.empty())
            data.vm = AnmVm{};
    }
    out.bullets.resize(bullets_.size());
    size_t live_bullet_vms = 0;
    for (size_t i = 0; i < bullets_.size(); ++i) {
        const auto& slot = bullets_[i];
        auto& data = out.bullets[i];
        const bool keep_vm =
            slot.vm != nullptr && (slot.active || animations.registry.is_embedded_referenced(slot.vm.get()));
        data.has_vm = keep_vm;
        live_bullet_vms += keep_vm;
        data.script = slot.script;
        data.color = slot.color;
        data.source_type = slot.source.type;
        data.source_color = slot.source.color;
        data.generation = slot.generation;
        data.active = slot.active;
    }
    out.bullet_vms.resize(live_bullet_vms);
    size_t bullet_vm_out = 0;
    for (size_t i = 0; i < bullets_.size(); ++i) {
        if (out.bullets[i].has_vm) out.bullet_vms[bullet_vm_out++] = *bullets_[i].vm;
    }
    out.shot_handles.assign(shot_handles_.begin(), shot_handles_.end());
    out.shots.resize(shots_.size());
    for (size_t i = 0; i < shots_.size(); ++i) {
        if (shots_[i])
            out.shots[i] = *shots_[i];
        else
            out.shots[i] = AnmVm{};
    }
    out.shot_generations.assign(shot_generations_.begin(), shot_generations_.end());
    out.shot_fades.resize(shot_fades_.size());
    for (size_t i = 0; i < shot_fades_.size(); ++i) {
        const auto& fade = shot_fades_[i];
        auto& data = out.shot_fades[i];
        data.handle = fade.handle;
        data.generation = fade.generation;
        data.x = fade.x;
        data.y = fade.y;
        data.angle = fade.angle;
        data.drift = fade.drift;
        data.sine_moving = fade.sine_moving;
        data.sine_motion = fade.sine_motion;
        data.age = fade.age;
    }
    out.shot_faded_generations.clear();
    for (const auto& bucket : shot_faded_generations_)
        for (u32 generation : bucket.second)
            out.shot_faded_generations.push_back((u64(bucket.first) << 32) | generation);
    out.has_player = player_ != nullptr;
    if (player_)
        out.player = *player_;
    else
        out.player = AnmVm{};
    out.player_script = player_script_;
    out.player_tilt = player_tilt_;
    for (size_t i = 0; i < teammate_players_.size(); ++i) {
        if (teammate_players_[i])
            out.teammate_players[i] = *teammate_players_[i];
        else
            out.teammate_players[i].reset();
    }
    out.teammate_tilts = teammate_tilts_;
    out.spirit_handles = spirit_handles_;
    out.spirit_serials = spirit_serials_;
    out.spirit_scripts = spirit_scripts_;
    size_t body_count = 0, side_count = 0;
    for (size_t i = 0; i < item_vms_.size(); ++i) {
        const bool present = item_vms_[i] != nullptr;
        out.item_vm_present[i] = present;
        body_count += present;
    }
    out.item_vms.resize(body_count);
    size_t body_out = 0;
    for (size_t i = 0; i < item_vms_.size(); ++i) {
        if (item_vms_[i]) out.item_vms[body_out++] = *item_vms_[i];
    }
    for (size_t i = 0; i < item_side_vms_.size(); ++i) {
        const bool present = item_side_vms_[i] != nullptr;
        out.item_side_vm_present[i] = present;
        side_count += present;
    }
    out.item_side_vms.resize(side_count);
    size_t side_out = 0;
    for (size_t i = 0; i < item_side_vms_.size(); ++i) {
        if (item_side_vms_[i]) out.item_side_vms[side_out++] = *item_side_vms_[i];
    }
    out.youmu_slash_handles = effects.youmu_slash_handles;
    out.youmu_slash_extra.assign(effects.youmu_slash_extra.begin(), effects.youmu_slash_extra.end());
    out.option_handles = effects.option_handles;
    out.full_option_handles = effects.full_option_handles;
    out.bomb_orb_handles = effects.bomb_orb_handles;
    out.bomb_orb_exploding = effects.bomb_orb_exploding;
    out.bomb_ray_handles = effects.bomb_ray_handles;
    out.trance_handle = trance_handle_;
    out.trance_was_active = trance_was_active_;
    out.bullet_cancel_handles.assign(bullet_cancel_handles_.begin(), bullet_cancel_handles_.end());
    out.focus_handle = effects.focus_handle;
    out.youmu_charge_handles = youmu_charge_handles_;
    if (!fog_handles_.empty() || !out.fog_handles.empty()) out.fog_handles = fog_handles_;
    out.option_character = effects.option_character;
    out.option_count = effects.option_count;
    out.option_anchor_deferred = effects.option_anchor_deferred;
    out.option_rebuild_pending = effects.option_rebuild_pending;
    out.options_hidden = effects.options_hidden;
    out.death_options_queued = effects.death_options_queued;
    out.full_options = effects.full_options;
    out.frame_synced = frame_synced_;
    out.bomb_generation = effects.bomb_generation;
    out.bomb_background = effects.bomb_background;
    out.bomb_beam = effects.bomb_beam;
    out.player_effect_handles.assign(player_effect_handles_.begin(), player_effect_handles_.end());
    return true;
}

bool StagePresentation::restore_presentation(const SnapshotData& source,
                                             std::unordered_map<const AnmVm*, AnmVm*>* remapped,
                                             std::vector<AnmVm*>* embedded_out) {
    if (source.version != SnapshotData::kContractVersion) {
        return fail("Presentation snapshot contract version mismatch");
    }
    auto& effects = player_effects_[0];
    for (size_t i = 0; i < source.teammate_effects.size(); ++i)
        player_effects_[i + 1] = source.teammate_effects[i];
    enemies_.resize(source.enemies.size());
    for (size_t i = 0; i < source.enemies.size(); ++i) {
        const auto& data = source.enemies[i];
        auto& visual = enemies_[i];
        visual.serial = data.serial;
        visual.script = data.script;
        if (data.has_vm) {
            if (!visual.vm)
                visual.vm = std::make_unique<AnmVm>(data.vm);
            else
                *visual.vm = data.vm;
            visual.vm->environment = &animations.scene_environment();
            visual.vm->object_host = &animations;
        } else {
            visual.vm.reset();
        }
    }
    bullets_.resize(source.bullets.size());
    size_t bullet_vm_cursor = 0;
    for (size_t i = 0; i < source.bullets.size(); ++i) {
        const auto& data = source.bullets[i];
        auto& slot = bullets_[i];
        slot.script = data.script;
        slot.color = data.color;
        slot.source.type = data.source_type;
        slot.source.color = data.source_color;
        slot.generation = data.generation;
        slot.active = data.active;
        if (data.has_vm) {
            if (bullet_vm_cursor >= source.bullet_vms.size()) return fail("Presentation snapshot bullet VM missing");
            const AnmVm& src_vm = source.bullet_vms[bullet_vm_cursor++];
            if (!slot.vm)
                slot.vm = std::make_unique<AnmVm>(src_vm);
            else
                *slot.vm = src_vm;
            slot.vm->environment = &animations.scene_environment();
            slot.vm->object_host = &animations;
            slot.vm->sprite_source = &slot.source;
        } else {
            slot.vm.reset();
        }
    }
    rebuild_bullet_lookup();
    shot_handles_.assign(source.shot_handles.begin(), source.shot_handles.end());
    shots_.resize(source.shots.size());
    for (size_t i = 0; i < source.shots.size(); ++i) {
        const auto& shot = source.shots[i];
        if (!shot.resource) {
            shots_[i].reset();
            continue;
        }
        if (!shots_[i])
            shots_[i] = std::make_unique<AnmVm>(shot);
        else
            *shots_[i] = shot;
        shots_[i]->environment = &animations.scene_environment();
        shots_[i]->object_host = &animations;
    }
    shot_generations_.assign(source.shot_generations.begin(), source.shot_generations.end());
    shot_fades_.resize(source.shot_fades.size());
    for (size_t i = 0; i < source.shot_fades.size(); ++i) {
        const auto& data = source.shot_fades[i];
        auto& fade = shot_fades_[i];
        fade.handle = data.handle;
        fade.generation = data.generation;
        fade.x = data.x;
        fade.y = data.y;
        fade.angle = data.angle;
        fade.drift = data.drift;
        fade.sine_moving = data.sine_moving;
        fade.sine_motion = data.sine_motion;
        fade.age = data.age;
    }
    for (auto& bucket : shot_faded_generations_)
        bucket.second.clear();
    for (u64 key : source.shot_faded_generations)
        shot_faded_generations_[u32(key >> 32)].push_back(u32(key));
    for (auto it = shot_faded_generations_.begin(); it != shot_faded_generations_.end();) {
        auto& keys = it->second;
        if (keys.empty()) {
            it = shot_faded_generations_.erase(it);
            continue;
        }
        if (!std::is_sorted(keys.begin(), keys.end())) std::sort(keys.begin(), keys.end());
        keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
        ++it;
    }
    if (source.has_player) {
        if (!player_)
            player_ = std::make_unique<AnmVm>(source.player);
        else
            *player_ = source.player;
        player_->environment = &animations.scene_environment();
        player_->object_host = &animations;
    } else {
        player_.reset();
    }
    player_script_ = source.player_script;
    player_tilt_ = source.player_tilt;
    for (size_t i = 0; i < teammate_players_.size(); ++i) {
        if (!source.teammate_players[i]) {
            teammate_players_[i].reset();
            continue;
        }
        if (!teammate_players_[i])
            teammate_players_[i] = std::make_unique<AnmVm>(*source.teammate_players[i]);
        else
            *teammate_players_[i] = *source.teammate_players[i];
        teammate_players_[i]->environment = &animations.scene_environment();
        teammate_players_[i]->object_host = &animations;
    }
    teammate_tilts_ = source.teammate_tilts;
    spirit_handles_ = source.spirit_handles;
    spirit_serials_ = source.spirit_serials;
    spirit_scripts_ = source.spirit_scripts;
    size_t body_cursor = 0, side_cursor = 0;
    for (size_t i = 0; i < item_vms_.size(); ++i) {
        if (source.item_vm_present[i]) {
            const auto& src = source.item_vms[body_cursor++];
            if (!item_vms_[i])
                item_vms_[i] = std::make_unique<AnmVm>(src);
            else
                *item_vms_[i] = src;
            item_vms_[i]->environment = &animations.scene_environment();
            item_vms_[i]->object_host = &animations;
        } else if (item_vms_[i]) {
            item_vms_[i].reset();
        }
        if (source.item_side_vm_present[i]) {
            const auto& src = source.item_side_vms[side_cursor++];
            if (!item_side_vms_[i])
                item_side_vms_[i] = std::make_unique<AnmVm>(src);
            else
                *item_side_vms_[i] = src;
            item_side_vms_[i]->environment = &animations.scene_environment();
            item_side_vms_[i]->object_host = &animations;
        } else if (item_side_vms_[i]) {
            item_side_vms_[i].reset();
        }
    }
    effects.youmu_slash_handles = source.youmu_slash_handles;
    effects.youmu_slash_extra.assign(source.youmu_slash_extra.begin(), source.youmu_slash_extra.end());
    effects.option_handles = source.option_handles;
    effects.full_option_handles = source.full_option_handles;
    effects.bomb_orb_handles = source.bomb_orb_handles;
    effects.bomb_orb_exploding = source.bomb_orb_exploding;
    effects.bomb_ray_handles = source.bomb_ray_handles;
    trance_handle_ = source.trance_handle;
    trance_was_active_ = source.trance_was_active;
    bullet_cancel_handles_.assign(source.bullet_cancel_handles.begin(), source.bullet_cancel_handles.end());
    effects.focus_handle = source.focus_handle;
    youmu_charge_handles_ = source.youmu_charge_handles;
    if (!source.fog_handles.empty() || !fog_handles_.empty()) fog_handles_ = source.fog_handles;
    effects.option_character = source.option_character;
    effects.option_count = source.option_count;
    effects.option_anchor_deferred = source.option_anchor_deferred;
    effects.option_rebuild_pending = source.option_rebuild_pending;
    effects.options_hidden = source.options_hidden;
    effects.death_options_queued = source.death_options_queued;
    effects.full_options = source.full_options;
    frame_synced_ = source.frame_synced;
    effects.bomb_generation = source.bomb_generation;
    effects.bomb_background = source.bomb_background;
    effects.bomb_beam = source.bomb_beam;
    player_effect_handles_.assign(source.player_effect_handles.begin(), source.player_effect_handles.end());
    if (remapped || embedded_out) {
        if (embedded_out) {
            embedded_out->clear();
            embedded_out->reserve(source.embedded_identities.size());
        }
        size_t i = 0;
        const auto restore_identity = [&](const auto& vm) {
            const auto* old = source.embedded_identities[i++];
            if (embedded_out) embedded_out->push_back(vm.get());
            if (remapped && old && (!embedded_out || old != vm.get())) {
                remapped->emplace(old, vm.get());
            }
        };
        for (const auto& enemy : enemies_)
            restore_identity(enemy.vm);
        for (const auto& bullet : bullets_)
            restore_identity(bullet.vm);
        for (const auto& shot : shots_)
            restore_identity(shot);
        restore_identity(player_);
        for (const auto& vm : teammate_players_)
            restore_identity(vm);
        for (const auto& vm : item_vms_)
            restore_identity(vm);
        for (const auto& vm : item_side_vms_)
            restore_identity(vm);
    }
    return true;
}

void StagePresentation::reset_snapshot_caches() {
    for (auto& enemy : enemies_)
        if (enemy.vm) enemy.vm.reset();
    enemies_.clear();
    for (auto& bullet : bullets_)
        if (bullet.vm) bullet.vm.reset();
    bullets_.clear();
    shot_handles_.clear();
    shot_generations_.clear();
    for (auto& shot : shots_)
        shot.reset();
    shots_.clear();
    shot_fades_.clear();
    shot_faded_generations_.clear();
    for (auto& vm : item_vms_)
        vm.reset();
    for (auto& vm : item_side_vms_)
        vm.reset();
    for (u32& handle : spirit_handles_)
        handle = 0;
    spirit_serials_.fill(0);
    spirit_scripts_.fill(0);
    for (auto& effects : player_effects_) {
        for (u32& handle : effects.youmu_slash_handles)
            handle = 0;
        effects.youmu_slash_extra.clear();
        for (u32& handle : effects.bomb_orb_handles)
            handle = 0;
        effects.bomb_orb_exploding.fill(false);
        for (u32& handle : effects.bomb_ray_handles)
            handle = 0;
        for (u32& handle : effects.option_handles)
            handle = 0;
        for (u32& handle : effects.full_option_handles)
            handle = 0;
        effects.option_character = -1;
        effects.option_count = -1;
        effects.focus_handle = 0;
    }
    youmu_charge_handles_.fill(0);
    bullet_cancel_handles_.clear();
    frame_synced_ = false;
}

void StagePresentation::release_player_shots() {
    for (u32 handle : shot_handles_) {
        if (auto* vm = animations.registry.find(handle)) animations.release_tree(*vm);
    }
    shot_handles_.clear();
    for (auto& shot : shots_)
        if (shot && shot->resource) animations.release_tree(*shot);
    shots_.clear();
    shot_generations_.clear();
    for (auto& corpse : shot_fades_) {
        if (auto* vm = animations.registry.find(corpse.handle)) animations.release_tree(*vm);
    }
    shot_fades_.clear();
    shot_faded_generations_.clear();
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
    release_player_shots();
    if (player_) {
        animations.release_tree(*player_);
        player_.reset();
    }
    for (auto& vm : teammate_players_) {
        if (vm) animations.release_tree(*vm);
        vm.reset();
    }
    teammate_tilts_.fill(0);
    for (u32 handle : player_effect_handles_)
        if (auto* vm = animations.registry.find(handle)) animations.release_tree(*vm);
    player_effect_handles_.clear();
    youmu_charge_handles_.fill(0);
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
    for (auto& effects : player_effects_) {
        for (u32 handle : effects.youmu_slash_handles)
            if (handle && !animations.retire(handle)) fail("Youmu slash retire failed: " + animations.error);
        for (u32 handle : effects.youmu_slash_extra)
            if (handle && !animations.retire(handle)) fail("Youmu slash retire failed: " + animations.error);
        effects = PlayerEffectsState{};
    }
}

u32 StagePresentation::create_player_effect(i32 bank, i32 script, i32 layer, u32 ordering, const Vec3& position) {
    return create_player_effect(bank, script, layer, ordering, position, 0);
}

u32 StagePresentation::create_player_effect(i32 bank, i32 script, i32 layer, u32 ordering, const Vec3& position,
                                            u8 owner, EffectOpacity opacity) {
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
    vm->shot_effect_owner = owner;
    vm->player_support_effect = opacity == EffectOpacity::Support;
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

bool StagePresentation::sync_bomb_effects(const StageBattle& battle, i32 seat) {
    const i32 index = battle.seat_count >= 2 ? (seat < 0 ? battle.active_seat : seat) : 0;
    auto& effects = player_effects_[size_t(index)];
    const auto& player = battle.seat_count >= 2 && seat >= 0 ? battle.seats[size_t(index)] : battle.player_state();
    const i32 bank = player_bank(index);
    const u8 owner = battle.seat_count >= 2 ? u8(index + 1) : 0;

    const i32 character = player.character;
    if (player.bomb_generation != effects.bomb_generation) {
        effects.bomb_generation = player.bomb_generation;
        if (!end_player_effect(effects.bomb_background, false) || !end_player_effect(effects.bomb_beam, false))
            return false;
        for (auto& handle : effects.bomb_ray_handles)
            if (!end_player_effect(handle, false)) return false;
        if (player.bomb_timer > 0) {
            if (character == 1) {
                effects.bomb_beam =
                    create_player_effect(bank, 23, -1, 0, Vec3{player.bomb_center_x, player.bomb_center_y, 0}, owner);
                if (!effects.bomb_beam) return false;
            }
            constexpr i32 backgrounds[] = {26, 31, 27, 25};
            effects.bomb_background = create_player_effect(bank, backgrounds[character], -1, 0,
                                                           Vec3{player.bomb_center_x, player.bomb_center_y, 0}, owner);
            if (!effects.bomb_background) return false;
            if (character != 0) {
                auto* vm = animations.registry.find(effects.bomb_background);
                vm->visual.color = (vm->visual.color & 0xffffffu) | 0xe0000000u;
                vm->interpolators.secondary_alpha.control2[0] = 224;
            }
            if (character == 2)
                for (size_t i = 0; i < effects.bomb_ray_handles.size(); ++i) {
                    effects.bomb_ray_handles[i] =
                        create_player_effect(bank, 23, -1, 0, player.bomb_rays[i].origin, owner);
                    if (!effects.bomb_ray_handles[i]) return false;
                }
        }
    }
    if (player.bomb_timer <= 0) {
        if (!end_player_effect(effects.bomb_background, false) || !end_player_effect(effects.bomb_beam, false))
            return false;
        for (auto& handle : effects.bomb_ray_handles)
            if (!end_player_effect(handle, false)) return false;
        return true;
    }
    if (character == 1) {
        if (auto* vm = animations.registry.find(effects.bomb_beam)) {
            vm->visual.translation = Vec3{player.bomb_center_x, player.bomb_center_y, 0};
            vm->variables.rotation.z = player.bomb_visual_angle;
            vm->visual.flags |= 4;
        }
    } else if (character == 2) {
        if (auto* vm = animations.registry.find(effects.bomb_background))
            vm->visual.translation = Vec3{player.bomb_center_x, player.bomb_center_y, 0};
        for (size_t i = 0; i < effects.bomb_ray_handles.size(); ++i)
            if (auto* vm = animations.registry.find(effects.bomb_ray_handles[i])) {
                vm->visual.translation = player.bomb_rays[i].origin;
                vm->variables.rotation.z = player.bomb_rays[i].angle;
                vm->visual.flags |= 4;
            }
    }
    return true;
}

bool StagePresentation::sync_player_effects(StageBattle& battle, i32 seat) {
    const i32 index = battle.seat_count >= 2 ? (seat < 0 ? battle.active_seat : seat) : 0;
    auto& effects = player_effects_[size_t(index)];
    const auto& player = battle.seat_count >= 2 && seat >= 0 ? battle.seats[size_t(index)] : battle.player_state();
    const i32 bank = player_bank(index);
    const u8 owner = battle.seat_count >= 2 ? u8(index + 1) : 0;

    player_effect_handles_.erase(std::remove_if(player_effect_handles_.begin(), player_effect_handles_.end(),
                                                [&](u32 handle) { return !animations.registry.find(handle); }),
                                 player_effect_handles_.end());
    const bool visible = (!player.dead && player.phase != StageBattle::PlayerPhase::Dying);
    const Vec3 position{player.x, player.y, 0};
    if (!sync_bomb_effects(battle, seat)) return false;

    if (visible && player.focus) {
        if (!animations.registry.find(effects.focus_handle)) {
            effects.focus_handle =
                create_player_effect(banks.effect, 22, 12, 0, position, owner, EffectOpacity::Support);
            if (!effects.focus_handle) return false;
        }
        animations.registry.find(effects.focus_handle)->visual.translation = position;
    } else if (!end_player_effect(effects.focus_handle, !visible)) {
        return false;
    }

    if (!sync_player_options(battle, false, seat)) return false;

    for (size_t i = 0; i < effects.bomb_orb_handles.size(); ++i) {
        const auto& orb = player.bomb_orbs[i];
        if (player.character != 0 || (!orb.active && !orb.exploding)) {
            if (!end_player_effect(effects.bomb_orb_handles[i], true)) return false;
            effects.bomb_orb_exploding[i] = false;
            continue;
        }
        if (!animations.registry.find(effects.bomb_orb_handles[i]) && !orb.exploding) {
            effects.bomb_orb_handles[i] = create_player_effect(bank, 18, -1, 0, {orb.x, orb.y, 0}, owner);
            if (!effects.bomb_orb_handles[i]) return false;
            effects.bomb_orb_exploding[i] = false;
        }
        if (auto* vm = animations.registry.find(effects.bomb_orb_handles[i]))
            vm->visual.translation = {orb.x, orb.y, 0};
        if (orb.exploding && !effects.bomb_orb_exploding[i]) {
            if (!end_player_effect(effects.bomb_orb_handles[i], false)) return false;
            effects.bomb_orb_exploding[i] = true;
        }
    }
    return true;
}

bool StagePresentation::hide_player_options() {
    for (auto& effects : player_effects_) {
        effects.options_hidden = true;
        for (size_t i = 0; i < effects.option_handles.size(); ++i) {
            for (u32* handle : {&effects.option_handles[i], &effects.full_option_handles[i]}) {
                if (!*handle) continue;
                if (!animations.interrupt(*handle, 3, false)) return fail(animations.error);
            }
        }
    }
    return true;
}

bool StagePresentation::interrupt_player_options() {
    for (auto& effects : player_effects_) {
        for (size_t i = 0; i < effects.option_handles.size(); ++i) {
            for (u32* handle : {&effects.option_handles[i], &effects.full_option_handles[i]}) {
                if (!*handle) continue;
                if (!animations.interrupt(*handle, 1, false)) return fail(animations.error);
                *handle = 0;
            }
        }
        effects.option_character = effects.option_count = -1;
        effects.full_options = false;
        effects.option_rebuild_pending = true;
    }
    return true;
}

bool StagePresentation::sync_player_options(StageBattle& battle, bool power_rebuild) {
    return sync_player_options(battle, power_rebuild, -1);
}

bool StagePresentation::sync_player_options(StageBattle& battle, bool power_rebuild, i32 seat) {
    const i32 index = battle.seat_count >= 2 ? (seat < 0 ? battle.active_seat : seat) : 0;
    auto& effects = player_effects_[size_t(index)];
    const auto& player = battle.seat_count >= 2 && seat >= 0 ? battle.seats[size_t(index)] : battle.player_state();
    const i32 bank = player_bank(index);
    const u8 owner = battle.seat_count >= 2 ? u8(index + 1) : 0;

    if (effects.option_rebuild_pending) return true;
    if (effects.options_hidden) {
        const Vec3 anchor{player.x, player.y, 0};
        for (u32 handle : effects.option_handles)
            if (auto* vm = animations.registry.find(handle)) vm->visual.translation = anchor;
        for (u32 handle : effects.full_option_handles)
            if (auto* vm = animations.registry.find(handle)) vm->visual.translation = anchor;
        return true;
    }
    const bool visible = (!player.dead && player.phase != StageBattle::PlayerPhase::Dying);
    const bool defer_anchor = power_rebuild || effects.option_anchor_deferred;
    constexpr i32 option_scripts[4] = {16, 18, 19, 15};
    constexpr i32 full_scripts[4] = {17, 20, 21, 17};
    const i32 character = player.character;
    const i32 count = player.option_count;
    const bool full = visible && (battle.seat_count >= 2 ? battle.seat_huds[size_t(index)].power >=
                                                               battle.seat_huds[size_t(index)].max_power
                                                         : battle.full_power());
    if (character < 0 || character > 3 || count < 0 || count > 8)
        return fail("Player option state outside original bounds");
    if (!visible) {
        if (!effects.death_options_queued) {
            for (u32 handle : effects.option_handles)
                if (handle && !animations.interrupt(handle, 1, false))
                    return fail("Player option death interrupt failed: " + animations.error);
            for (u32 handle : effects.full_option_handles)
                if (handle && !animations.interrupt(handle, 1, false))
                    return fail("Full power option death interrupt failed: " + animations.error);
            effects.death_options_queued = true;
        }
        if (player.death_option_rebuild) {
            if (battle.seat_count >= 2 && seat >= 0) {
                battle.seats[size_t(index)].death_option_rebuild = false;
                if (index == battle.active_seat) battle.death_option_rebuild = false;
            } else
                battle.death_option_rebuild = false;
            for (u32& handle : effects.option_handles)
                if (!end_player_effect(handle, true)) return false;
            for (u32& handle : effects.full_option_handles)
                if (!end_player_effect(handle, false)) return false;
            effects.option_character = character;
            effects.option_count = count;
            effects.full_options = false;
            for (size_t i = 0; i < effects.option_handles.size(); ++i) {
                if (i >= size_t(count)) continue;
                const auto& option = player.options[i];
                const Vec3 anchor{option.x, option.y, 0};
                effects.option_handles[i] =
                    create_player_effect(bank, option_scripts[character], 12, 0, anchor, owner, EffectOpacity::Support);
                if (!effects.option_handles[i]) return false;
            }
        }
        effects.option_anchor_deferred = power_rebuild;
        return true;
    }
    effects.death_options_queued = false;
    if (character != effects.option_character || count != effects.option_count) {
        for (u32& handle : effects.option_handles)
            if (!end_player_effect(handle, true)) return false;
        for (u32& handle : effects.full_option_handles)
            if (!end_player_effect(handle, !visible || full)) return false;
        effects.option_character = character;
        effects.option_count = count;
        effects.full_options = false;
    }
    if (full != effects.full_options) {
        for (u32& handle : effects.full_option_handles)
            if (!end_player_effect(handle, !visible)) return false;
        effects.full_options = full;
    }
    if (full)
        for (size_t i = 0; i < effects.full_option_handles.size(); ++i) {
            const auto& option = player.options[i];
            if (i >= size_t(count) || !option.active) continue;
            const Vec3 anchor{option.x, option.y, 0};
            if (!animations.registry.find(effects.full_option_handles[i])) {
                effects.full_option_handles[i] =
                    create_player_effect(bank, full_scripts[character], 12, 2, power_rebuild ? Vec3{} : anchor, owner,
                                         EffectOpacity::Support);
                if (!effects.full_option_handles[i]) return false;
                if (power_rebuild)
                    animations.registry.find(effects.full_option_handles[i])->visual.translation = {0, -32, 0};
            }
            if (!defer_anchor) animations.registry.find(effects.full_option_handles[i])->visual.translation = anchor;
        }
    for (size_t i = 0; i < effects.option_handles.size(); ++i) {
        const auto& option = player.options[i];
        const bool active = i < size_t(count) && option.active;
        if (!active) {
            if (!end_player_effect(effects.option_handles[i], true) ||
                !end_player_effect(effects.full_option_handles[i], true))
                return false;
            continue;
        }
        const Vec3 anchor{option.x, option.y, 0};
        if (!animations.registry.find(effects.option_handles[i])) {
            effects.option_handles[i] = create_player_effect(
                bank, option_scripts[character], 12, 0, power_rebuild ? Vec3{} : anchor, owner, EffectOpacity::Support);
            if (!effects.option_handles[i]) return false;
            if (power_rebuild) animations.registry.find(effects.option_handles[i])->visual.translation = {0, -32, 0};
        }
        if (!defer_anchor) animations.registry.find(effects.option_handles[i])->visual.translation = anchor;
    }
    effects.option_anchor_deferred = power_rebuild;
    return true;
}

bool StagePresentation::reimu_bomb_orb_birth(const StageBattle& battle, i32 index) {
    const i32 seat = battle.seat_count >= 2 ? battle.active_seat : 0;
    auto& effects = player_effects_[size_t(seat)];

    if (index < 0 || index >= i32(effects.bomb_orb_handles.size())) return fail("Reimu bomb orb index outside pool");
    u32& handle = effects.bomb_orb_handles[size_t(index)];
    if (animations.registry.find(handle)) return fail("Reimu bomb orb slot already active");
    const auto& orb = battle.bomb_orbs[size_t(index)];
    handle = create_player_effect(player_bank(seat), 18, -1, 0, {orb.x, orb.y, 0},
                                  battle.seat_count >= 2 ? u8(seat + 1) : 0);
    effects.bomb_orb_exploding[size_t(index)] = false;
    return handle != 0;
}

bool StagePresentation::initialize_player_shot(PlayerShot& shot, u32 index, i32 seat) {
    if (shot.script < 0) return true;
    const i32 bank = seat > 0 ? banks.teammate_player[size_t(seat - 1)] : banks.player;
    if (battle_ && battle_->seat_count >= 2) index = index * 3 + u32(seat);
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
    const auto* source = animations.script_template(bank, u32(shot.script));
    if (!source) return fail("Player shot animation template unavailable");
    auto* vm = animations.registry.allocate();
    if (!animations.bind_template(*vm, bank, shot.script)) {
        animations.registry.discard(*vm);
        return fail("Player shot animation template unavailable");
    }
    if (anm_random_observer) anm_random_observer(*vm, -11, shot.script, 0, u32(bank), 0, 0, 0, false);
    vm->instruction_offset = source->instruction_offset;
    vm->visual.render_flags |= 0x200u;
    vm->visual.translation = {};
    vm->variables.rotation.z = 0;
    vm->pre_tick = pause_player_effect;
    vm->shot_effect_owner = battle_ && battle_->seat_count >= 2 ? u8(seat + 1) : 0;
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
    for (size_t i = 0; i < shot_handles_.size(); ++i) {
        const bool multi = battle_->seat_count >= 2;
        const size_t seat = multi ? i % 3 : 0, slot = multi ? i / 3 : i;
        if (multi && seat >= size_t(battle_->seat_count)) continue;
        auto& pool = multi ? battle_->seats[seat].shots : battle_->shots;
        if (slot >= pool.size()) continue;
        auto& shot = pool[slot];
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
        battle_->set_player_shot_geometry(i32(slot), shot.age, shot.script, shot.generation, snapshot, i32(seat));
    }
    for (size_t n = 0; n < shot_fades_.size();) {
        if (!animations.registry.find(shot_fades_[n].handle))
            shot_fades_.erase(shot_fades_.begin() + i32(n));
        else
            ++n;
    }
    return true;
}

bool StagePresentation::sync_all_player_effects(StageBattle& battle) {
    if (battle.seat_count < 2) return sync_player_effects(battle);
    for (i32 seat = 0; seat < battle.seat_count; ++seat)
        if (!sync_player_effects(battle, seat)) return false;
    return true;
}

bool StagePresentation::initialize_player_effects(StageBattle& battle) {
    battle_ = &battle;
    return sync_all_player_effects(battle);
}

bool StagePresentation::activate_player_effects(StageBattle& battle) {
    for (auto& effects : player_effects_) {
        for (size_t i = 0; i < effects.option_handles.size(); ++i) {
            for (u32 handle : {effects.option_handles[i], effects.full_option_handles[i]}) {
                if (!animations.registry.find(handle)) continue;
                if (!animations.interrupt(handle, 2, false)) return fail(animations.error);
            }
        }
        effects.option_rebuild_pending = false;
        effects.options_hidden = false;
    }
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
    const i32 seat = battle.seat_count >= 2 ? battle.active_seat : 0;
    auto& effects = player_effects_[size_t(seat)];
    const u8 owner = battle.seat_count >= 2 ? u8(seat + 1) : 0;

    for (const auto& spawn : battle.youmu_slash_spawns_) {
        const u32 handle =
            animations.create(player_bank(seat), spawn.script, -1, 0, {0.0f, spawn.y, 0.0f}, 0, false, owner);
        if (!handle) return fail("Youmu slash animation creation failed: " + animations.error);
        if (spawn.slot >= 0 && spawn.slot < YoumuBomb::kSlots) {
            effects.youmu_slash_handles[size_t(spawn.slot)] = handle;
        } else {
            effects.youmu_slash_extra.push_back(handle);
        }
    }
    for (int slot = 0; slot < YoumuBomb::kSlots; ++slot) {
        u32& handle = effects.youmu_slash_handles[size_t(slot)];
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
    if (target.script < 0)
        return fail("Effect animation target unresolved: file=" + std::to_string(file) +
                    " script=" + std::to_string(script));
    const u32 effect_handle = animations.create_preserve_ip(target.bank, target.script, -1, 2, position, 0, false);
    if (!effect_handle)
        return fail("Effect animation creation failed: file=" + std::to_string(file) + " script=" +
                    std::to_string(script) + " bank=" + std::to_string(target.bank) + " manager=" + animations.error);
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
    if (battle.seat_count >= 2) {
        for (i32 seat = 0; seat < battle.seat_count; ++seat)
            battle.begin_player_shot_geometry_snapshot(seat);
    } else
        battle.begin_player_shot_geometry_snapshot();
    if (!sync_all_player_effects(battle)) return false;
    if (!battle.defer_item_side) {
        if (!sync_items(battle)) return false;
        if (!sync_item_spirits(battle)) return false;
    }
    if (!battle.youmu_bomb_update && !sync_youmu_slash(battle)) return false;

    if (!flush_ecl_animations(battle)) return false;
    if (enemies_from_ecl) {
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
                        handle = animations.create_preserve_ip(target.bank, target.script,
                                                               enemy->mirror_script_base + 7, 0, old_pos, 0, false);
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
    }

    std::vector<u32> alive;
    if (!enemies_from_ecl) {
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
            if (enemies_[i].vm && !animations.release_tree(*enemies_[i].vm))
                return fail("Enemy animation release failed");
            enemies_.erase(enemies_.begin() + i);
        }
    }

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

    const bool multi_shots = battle.seat_count >= 2;
    size_t shot_slots = battle.shots.size();
    if (multi_shots) {
        shot_slots = 0;
        for (i32 seat = 0; seat < battle.seat_count; ++seat)
            shot_slots = std::max(shot_slots, battle.seats[size_t(seat)].shots.size() * 3);
    }
    const auto shot_at = [&](size_t slot) -> PlayerShot* {
        if (!multi_shots) return slot < battle.shots.size() ? &battle.shots[slot] : nullptr;
        const size_t seat = slot % 3, index = slot / 3;
        if (seat >= size_t(battle.seat_count)) return nullptr;
        auto& pool = battle.seats[seat].shots;
        return index < pool.size() ? &pool[index] : nullptr;
    };
    if (shot_handles_.size() < shot_slots) shot_handles_.resize(shot_slots, 0);
    if (shots_.size() < shot_handles_.size()) shots_.resize(shot_handles_.size());
    shot_generations_.resize(shot_handles_.size());
    for (size_t i = 0; i < shot_handles_.size(); ++i) {
        const i32 seat = multi_shots ? i32(i % 3) : 0;
        const i32 bank = seat > 0 && seat < battle.seat_count ? banks.teammate_player[size_t(seat - 1)] : banks.player;
        auto* player_resource = animations.resource(bank);
        const auto fade_key = [seat](u32 generation) { return (u64(u32(seat)) << 32) | generation; };
        const auto insert_fade_key = [this](u64 key) {
            auto& keys = shot_faded_generations_[u32(key >> 32)];
            const u32 generation = u32(key);
            if (keys.empty() || keys.back() < generation) {
                keys.push_back(generation);
                return true;
            }
            const auto it = std::lower_bound(keys.begin(), keys.end(), generation);
            if (it != keys.end() && *it == generation) return false;
            keys.insert(it, generation);
            return true;
        };
        const auto contains_fade_key = [this](u64 key) {
            const auto bucket = shot_faded_generations_.find(u32(key >> 32));
            return bucket != shot_faded_generations_.end() &&
                   std::binary_search(bucket->second.begin(), bucket->second.end(), u32(key));
        };
        if (shot_at(i) != nullptr && (*shot_at(i)).alive) {
            auto& shot = (*shot_at(i));
            bool fresh_slot = false;
            if (shot_generations_[i] != shot.generation) {
                size_t source = i + 1;
                while (source < shot_handles_.size() &&
                       (shot_generations_[source] != shot.generation || (multi_shots && source % 3 != i % 3)))
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
                    if (!initialize_player_shot(shot, u32(multi_shots ? i / 3 : i), seat)) return false;
                    vm = animations.registry.find(shot_handles_[i]);
                } else if (vm->source_script != script) {
                    if (!initialize_player_shot(shot, u32(multi_shots ? i / 3 : i), seat)) return false;
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
                    vm->shot_effect_owner = multi_shots ? u8(seat + 1) : 0;
                }
                shot_generations_[i] = shot.generation;
            }
            if (shot.state == 2 && insert_fade_key(fade_key(shot.generation))) {
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
            const i32 corpse_script = shot_at(i) != nullptr ? (*shot_at(i)).script : -1;
            AnmVm* vm = corpse_script >= 0 ? animations.registry.find(shot_handles_[i])
                                           : (shots_[i] ? shots_[i].get() : nullptr);
            bool fade = false;
            PlayerShot corpse_shot{};
            if (shot_at(i) != nullptr && !(*shot_at(i)).alive) {
                const PlayerShot& shot = (*shot_at(i));
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
                fade = !outside && vm && shot.script >= 0 && !contains_fade_key(fade_key(shot.generation)) &&
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
                insert_fade_key(fade_key(corpse.generation));
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

    const bool first_ghost = battle.seat_count >= 2 && battle.seats[0].ghost;
    if (battle.player_visible() || first_ghost) {
        const i32 tilt = first_ghost ? 0 : ship_tilt(held, battle.player_x, battle.touch.mode != 0, player_.get());
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
    for (i32 seat = 1; seat < battle.seat_count && seat < 3; ++seat) {
        auto& vm = teammate_players_[seat - 1];
        auto& old_tilt = teammate_tilts_[seat - 1];
        const auto& state = battle.seats[seat];
        if (!state.ghost && (state.dead || state.phase == StagePlayerState::Phase::Dying)) {
            if (vm) animations.release_tree(*vm);
            vm.reset();
            old_tilt = 0;
            continue;
        }
        const auto buttons = battle.seat_inputs[seat].held;
        const i32 tilt = state.ghost ? 0 : ship_tilt(buttons, state.x, state.touch.mode != 0, vm.get());
        if (!vm) vm = std::make_unique<AnmVm>();
        const bool rebound = !vm->resource || tilt != old_tilt;
        if (rebound) {
            const i32 script = tilt < 0 ? 1 : tilt > 0 ? 3 : old_tilt < 0 ? 2 : old_tilt > 0 ? 4 : 0;
            if (vm->resource) animations.release_tree(*vm);
            const auto bank = banks.teammate_player[seat - 1];
            auto* resource = animations.resource(bank ? bank : banks.player);
            vm->environment = &animations.scene_environment();
            vm->object_host = &animations;
            if (!resource || !vm->bind(*resource, u32(script))) return fail("Teammate animation binding failed");
            vm->visual.set_layer(12);
            old_tilt = tilt;
        }
        vm->visual.translation = {state.x + kGameToScreenX, state.y + kGameToScreenY, 0};
        if (vm->tick(random, animations.rate) < 0 || (rebound && vm->tick(random, animations.rate) < 0))
            return fail("Teammate animation: " + vm->error);
    }
    frame_synced_ = true;
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
    for (size_t i = 0; i < teammate_players_.size(); ++i)
        if (teammate_players_[i]) describe("player", *teammate_players_[i], i32(i + 1), teammate_tilts_[i], 0);
    auto describe_handle = [&](const char* kind, u32 handle, i32 index) {
        if (auto* vm = animations.registry.find(handle)) describe(kind, *vm, index, -1, 0);
    };
    for (size_t seat = 0; seat < player_effects_.size(); ++seat) {
        const auto& effects = player_effects_[seat];
        describe_handle("focus", effects.focus_handle, 0);
        for (size_t i = 0; i < effects.option_handles.size(); ++i) {
            describe_handle("option", effects.option_handles[i], i32(i));
            describe_handle("full-option", effects.full_option_handles[i], i32(i));
            describe_handle("bomb-orb", effects.bomb_orb_handles[i], i32(i));
        }
    }
    return report;
}

void StagePresentation::prepare_player_opacity() {
    renderer.player_opacity.fill(255);
    if (!battle_ || battle_->seat_count < 2) return;
    const i32 local = battle_->mp_local_seat;
    const auto& viewer = battle_->seats[size_t(local)];
    for (i32 seat = 0; seat < battle_->seat_count; ++seat) {
        const auto& player = battle_->seats[size_t(seat)];
        if (player.ghost) {
            renderer.player_opacity[size_t(seat + 1)] = 140;
            continue;
        }
        if (seat == local) continue;
        const float x = player.x - viewer.x, y = player.y - viewer.y;
        renderer.player_opacity[size_t(seat + 1)] = x * x + y * y < 48.f * 48.f ? 72 : 160;
    }
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
    case 1: {
        const auto draw_ship = [&](AnmVm& vm, u8 opacity) {
            const auto visual = vm.visual;
            const auto rotation = vm.variables.rotation;
            const int result = renderer.draw_with_opacity(vm, opacity);
            vm.visual = visual;
            vm.variables.rotation = rotation;
            return result != -2 || fail(renderer.error.empty() ? "Stage draw failed" : renderer.error);
        };
        if (!battle_ || battle_->seat_count < 2) return !player_ || !player_->visible || draw_ship(*player_, 255);
        const i32 local = battle_->mp_local_seat;
        const auto ship = [&](i32 seat) -> AnmVm* {
            return seat == 0 ? player_.get() : teammate_players_[size_t(seat - 1)].get();
        };
        for (i32 seat = 0; seat < battle_->seat_count; ++seat) {
            if (seat == local) continue;
            auto* vm = ship(seat);
            if (!vm || !vm->visible) continue;
            const u8 opacity = renderer.player_opacity[size_t(seat + 1)];
            if (!draw_ship(*vm, opacity)) return false;
        }
        auto* vm = ship(local);
        return !vm || !vm->visible || draw_ship(*vm, renderer.player_opacity[size_t(local + 1)]);
    }
    case 2:
        for (auto& enemy : enemies_)
            if (enemy.vm) one(*enemy.vm);
        break;
    case 3: {
        if (renderer.logical_frame()) {
            for (auto& slot : bullets_)
                if (slot.active && slot.vm) one(*slot.vm);
            break;
        }
        draw_bullet_order_.clear();
        for (auto& slot : bullets_)
            if (slot.active && slot.vm) draw_bullet_order_.push_back(&slot);
        std::sort(draw_bullet_order_.begin(), draw_bullet_order_.end(),
                  [](const BulletSlot* a, const BulletSlot* b) { return a->generation > b->generation; });
        for (auto* slot : draw_bullet_order_)
            one(*slot->vm);
        if (renderer.always_hitbox && battle_) {
            const auto& local =
                battle_->seat_count >= 2 ? battle_->seats[size_t(battle_->mp_local_seat)] : battle_->player_state();
            if (!local.dead && !local.ghost && local.phase == StagePlayerState::Phase::Active && !local.focus) {
                const auto* source = animations.script_template(banks.effect, 22);
                if (!source) return fail("Focus marker template unavailable");
                AnmVm marker = *source;
                if (!marker.select_sprite(23)) return fail("Focus marker sprite unavailable");
                marker.visible = true;
                marker.visual.set_layer(12);
                marker.visual.scale = {1.f, 1.f};
                marker.visual.color = 0xffffffffu;
                marker.visual.translation = {local.x + kGameToScreenX, local.y + kGameToScreenY, 0.f};
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
