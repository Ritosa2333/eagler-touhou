// StagePresentation 实现。锚点注释对应 docs/plans/anm-pipeline.md 与
// BulletAppearance.hpp（0x4120b0 / 0x4bbe30 表）。
#include "StagePresentation.hpp"
#include "AnmCoordinates.hpp"
#include "AnmOverlay.hpp"
#include "StageMath.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace th13 {

// 调试直绘过滤（4 pass 全开）。
u32 StagePresentation::debug_pass_mask = 0xf;

namespace {
// 游戏坐标 → ANM 屏幕坐标：+320/+16 屏幕原点已由 0x40000 位在底层
// adjust_position 统一累加（th13 .data 双精度常量 0x4ae9f0=320.0 /
// 0x4ae8e0=16.0——0x466a80/0x468670/0x46f820 族一致；th15 遗产的 224
// 是 f61 敌机行走位差 96px 的根因，2026-10-23 实测修正），此处外部平移
// 增量置 0，杜绝双重叠加。
constexpr float kGameToScreenX = 0.0f, kGameToScreenY = 0.0f;
// 0040e4d0 writes the bullet heading after ANM update, immediately before
// drawing. ins424 (VM+1c bit6) opts in; other scripts retain their own rotation.
void sync_bullet_heading(AnmVm& vm, float angle) {
    if (!(vm.visual.render_flags & 0x40u)) return;
    vm.variables.rotation.z = wrap_angle(float(angle + 1.5707963705062866f));
    vm.visual.flags |= 4u;
}

// 00445c40 / 004464d0: player shots use their heading without the enemy
// bullet's quarter-turn, and only when the script opts in via ins424.
void sync_shot_heading(AnmVm& vm, float angle) {
    if (!(vm.visual.render_flags & 0x40u)) return;
    vm.variables.rotation.z = angle;
    vm.visual.flags |= 4u;
}

// 自机弹脚本是否带 id 中断标签（op5 [len12: time,mask,id32]）。0x446bf0
// 无条件写 VM+0x4a0=1；仅当脚本确有 id1 标签（pl00.anm script 10..17 的
// 命中渐隐段）才留尸，无标签脚本不进留尸池（中断无落点，弹型脚本会按
// 原飞行段死循环，永不自灭）。
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

// 自机所有表现 VM（含脚本创建的子体）随游戏暂停。原版游戏侧回调停机，
// 不能因为此处改用池 VM 就在 Pause 背后继续旋转/生成 Youmu 残像。
i32 pause_player_effect(AnmVm& vm) {
    auto* manager = static_cast<AnmManager*>(vm.object_host);
    if (manager)
        for (auto* child : manager->registry.children(vm)) {
            child->pre_tick = pause_player_effect;
            pause_player_effect(*child);
        }
    return vm.environment && vm.environment->paused ? 1 : 0;
}
}  // namespace

StagePresentation::StagePresentation(AnmManager& manager, AnmRenderer& draw, Banks banks,
                                     i32 player_shot_sprite)
    : animations(manager), renderer(draw), banks(banks),
      random(manager.random_source()), player_shot_sprite_(player_shot_sprite) {}

bool StagePresentation::tick_bullet(StageBullet& bullet, bool birth) {
    auto* resource = animations.resource(banks.bullet);
    const auto* appearance = bullet_appearance(i32(bullet.sprite));
    if (!resource || !appearance) return fail("Bullet animation resource/type unavailable");
    BulletSlot* slot = nullptr;
    for (auto& entry : bullets_)
        if (entry.active && entry.generation == bullet.generation) { slot = &entry; break; }
    if (!slot) {
        for (auto& entry : bullets_) if (!entry.active) { slot = &entry; break; }
        if (!slot) { bullets_.emplace_back(); slot = &bullets_.back(); }
        if (slot->vm && !animations.release_tree(*slot->vm)) return fail(animations.error);
        *slot = BulletSlot{};
        slot->generation = bullet.generation;
        slot->active = true;
    }
    slot->source.type = i32(bullet.sprite);
    slot->source.color = i32(bullet.subtype);
    if (!slot->vm) slot->vm = std::make_unique<AnmVm>();
    auto& vm = *slot->vm;
    if (!vm.resource || slot->script != appearance->script || slot->color != i32(bullet.subtype)) {
        vm.environment = &animations.scene_environment();
        vm.object_host = &animations;
        if (!vm.bind(*resource, u32(appearance->script))) return fail(vm.error);
        vm.sprite_source = &slot->source;
        if (vm.tick(random, animations.rate) < 0) return fail(vm.error); // 461cb0 initialization
        vm.visual.render_flags |= 0x40000;
        vm.pending_interrupt = bullet.state == 2 ? bullet.delay : 2;
        slot->script = appearance->script;
        slot->color = i32(bullet.subtype);
    } else if (bullet.animation_interrupt) {
        vm.pending_interrupt = bullet.animation_interrupt;
    }
    bullet.animation_interrupt = 0;
    if (bullet.animation_blend >= 0) {
        // 40e840 initializes ANM, executes the etEx queue, then ticks ANM.
        // This is a one-time command: a later ANM instruction owns its state.
        vm.visual.flags = (vm.visual.flags & ~0x1e0u) | (u32(bullet.animation_blend) << 5);
        bullet.animation_blend = -1;
    }
    vm.visual.translation = {bullet.x, bullet.y, bullet.z};
    const int result = vm.tick(random, animations.rate);
    if (result < 0) return fail("Bullet animation: " + vm.error);
    sync_bullet_heading(vm, bullet.angle);
    bullet.animation_ready = vm.variables.integers[0] != 0; // bullet+4d4 = vm+4ac
    bullet.animation_geometry = vm.sprite != nullptr;
    if (vm.sprite) {
        bullet.sprite_width = vm.visual.sprite_size.x;
        bullet.sprite_height = vm.visual.sprite_size.y;
    }
    if (result > 0) { bullet.alive = false; slot->active = false; }
    (void)birth;
    return true;
}

bool StagePresentation::bullet_cancel(i32 script,const Vec3& position,const Vec3& displacement,bool interpolate) {
    const u32 handle=animations.create(banks.bullet,script,-1,0,position,0,false);
    auto* vm=animations.registry.find(handle);
    if(!vm)return fail("Bullet cancel particle: "+animations.error);
    vm->visual.render_flags|=0x40000u;
    if(interpolate)vm->interpolators.position.begin(30,6,{0,0,0},
        {displacement.x,displacement.y,displacement.z}); //4125b0, no extra tick
    bullet_cancel_handles_.push_back(handle);
    return true;
}

void StagePresentation::release_stage() {
    while (!fog_handles_.empty()) retire_enemy_fog(fog_handles_.begin()->first);
    for (auto& enemy : enemies_)
        if (enemy.vm) animations.release_tree(*enemy.vm);
    enemies_.clear();
    for (auto& bullet : bullets_)
        if (bullet.vm) animations.release_tree(*bullet.vm);
    bullets_.clear();
    for (auto* handles : {&item_handles_, &item_side_handles_})
        for (u32& handle : *handles) { if (handle) animations.retire(handle); handle = 0; }
    for (u32& handle : spirit_handles_) { if (handle) animations.retire(handle); handle = 0; }
    spirit_serials_.fill(0); spirit_scripts_.fill(0);
    stage_file_offsets_.clear(); stage_offsets_source_ = nullptr;
}

void StagePresentation::release() {
    for(auto& handle:bullet_cancel_handles_)if(handle)animations.retire(handle);
    bullet_cancel_handles_.clear();
    if(trance_handle_)animations.retire(trance_handle_);
    trance_was_active_=false;
    while(!fog_handles_.empty())retire_enemy_fog(fog_handles_.begin()->first);
    for (auto& enemy : enemies_)
        if (enemy.vm) animations.release_tree(*enemy.vm);
    enemies_.clear();
    for (auto& bullet : bullets_)
        if (bullet.vm) animations.release_tree(*bullet.vm);
    bullets_.clear();
    for (auto& shot : shots_)
        if (shot && shot->resource) animations.release_tree(*shot);
    shots_.clear();
    shot_generations_.clear();
    for (auto& corpse : shot_fades_)
        if (corpse.vm && corpse.vm->resource) animations.release_tree(*corpse.vm);
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
    youmu_charge_ready_ = false;
    for (u32& handle : spirit_handles_) {
        if (handle && !animations.retire(handle))
            fail("Spirit animation retire failed: " + animations.error);
        handle = 0;
    }
    spirit_serials_.fill(0);
    spirit_scripts_.fill(0);
    for (u32& handle : item_handles_) {
        if (handle && !animations.retire(handle))
            fail("Item animation retire failed: " + animations.error);
        handle = 0;
    }
    for (u32& handle : item_side_handles_) {
        if (handle && !animations.retire(handle))
            fail("Item side animation retire failed: " + animations.error);
        handle = 0;
    }
    for (u32& handle : youmu_slash_handles_) {
        if (handle && !animations.retire(handle))
            fail("Youmu slash retire failed: " + animations.error);
        handle = 0;
    }
    for (u32 handle : youmu_slash_extra_)
        if (handle && !animations.retire(handle))
            fail("Youmu slash retire failed: " + animations.error);
    youmu_slash_extra_.clear();
    focus_handle_ = 0;
    option_handles_.fill(0);
    full_option_handles_.fill(0);
    bomb_orb_handles_.fill(0);
    bomb_orb_exploding_.fill(false);
    bomb_generation_=bomb_background_=bomb_beam_=0;
    bomb_ray_handles_.fill(0);
    option_character_ = option_count_ = -1;
    full_options_ = false;
}

u32 StagePresentation::create_player_effect(i32 bank, i32 script, i32 layer,
                                            u32 ordering, const Vec3& position) {
    // 00403800 复制已跑完负时间初始化的模板，保留模板的当前指令指针。
    // 自机模块在写 layer12 后只跑 time0；不可重放 -1 的 ins304 把它改回16。
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
        // 0046f990 子体在模板初始化后继承父层；script21 的 -1 ins304
        // 不应在 spawn 时重新覆盖继承值（现公共 spawn 尚从IP0开始）。
        for (auto* child : animations.registry.children(*vm))
            child->visual.set_layer(layer);
    }
    // time0 可立即 spawn：在首个暂停帧前给这批子体安装同一个停机回调。
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
    // 0046fc30 只排队中断、不立即再 tick；其子树接收同一 label。
    const bool ok = retire ? animations.retire(handle)
                           : animations.interrupt(handle, 1, false);
    handle = 0;
    return ok || fail("Player effect release failed: " + animations.error);
}

bool StagePresentation::sync_bomb_effects(const StageBattle& battle) {
    const i32 character=battle.player_character();
    if(battle.bomb_generation!=bomb_generation_) {
        bomb_generation_=battle.bomb_generation;
        // 40b4b0 / 40a650 / 40be80 / 40c910: original plXX background
        // scripts, not effect.anm or a synthetic fullscreen flash.
        if(!end_player_effect(bomb_background_,false)||
           !end_player_effect(bomb_beam_,false))return false;
        for(auto& handle:bomb_ray_handles_)
            if(!end_player_effect(handle,false))return false;
        if(battle.bomb_timer>0) {
            if(character==1) {
                bomb_beam_=create_player_effect(banks.player,23,-1,0,battle.bomb_center());
                if(!bomb_beam_)return false;
            }
            constexpr i32 backgrounds[]={26,31,27,25};
            bomb_background_=create_player_effect(banks.player,backgrounds[character],
                                                   -1,0,battle.bomb_center());
            if(!bomb_background_)return false;
            if(character!=0) {
                auto* vm=animations.registry.find(bomb_background_);
                vm->visual.color=(vm->visual.color&0xffffffu)|0xe0000000u;
                vm->interpolators.secondary_alpha.control2[0]=224; // native vm+334
            }
            if(character==2)for(size_t i=0;i<bomb_ray_handles_.size();++i) {
                // 40be80 copies pl02 template +81bc = script23 * 5a4.
                bomb_ray_handles_[i]=create_player_effect(banks.player,23,-1,0,
                                                         battle.bomb_rays[i].origin);
                if(!bomb_ray_handles_[i])return false;
            }
        }
    }
    if(battle.bomb_timer<=0) {
        if(!end_player_effect(bomb_background_,false)||
           !end_player_effect(bomb_beam_,false))return false;
        for(auto& handle:bomb_ray_handles_)
            if(!end_player_effect(handle,false))return false;
        return true;
    }
    if(character==1) {
        if(auto* vm=animations.registry.find(bomb_beam_)) {
            vm->visual.translation=battle.bomb_center();
            vm->variables.rotation.z=battle.bomb_visual_angle();
            vm->visual.flags|=4;
        }
    } else if(character==2) {
        if(auto* vm=animations.registry.find(bomb_background_))
            vm->visual.translation=battle.bomb_center();
        for(size_t i=0;i<bomb_ray_handles_.size();++i)
            if(auto* vm=animations.registry.find(bomb_ray_handles_[i])) {
                vm->visual.translation=battle.bomb_rays[i].origin;
                vm->variables.rotation.z=battle.bomb_rays[i].angle;
                vm->visual.flags|=4;
            }
    }
    return true;
}

bool StagePresentation::sync_player_effects(const StageBattle& battle) {
    player_effect_handles_.erase(
        std::remove_if(player_effect_handles_.begin(), player_effect_handles_.end(),
                       [&](u32 handle) { return !animations.registry.find(handle); }),
        player_effect_handles_.end());
    const bool visible = battle.player_visible();
    const Vec3 position{battle.player_x, battle.player_y, 0};
    if(!sync_bomb_effects(battle))return false;
    // 004460d0 / 00446790: pl03 script18 follows the player during charge;
    // reaching60 sends label2, release destroys the feedback VM.
    if (visible && battle.player_character() == 3 && battle.youmu_charge() > 0) {
        if (!animations.registry.find(youmu_charge_handle_)) {
            youmu_charge_handle_ = create_player_effect(banks.player, 18, -1, 0, position);
            if (!youmu_charge_handle_) return false;
            youmu_charge_ready_ = false;
        }
        animations.registry.find(youmu_charge_handle_)->visual.translation = position;
        if (battle.youmu_charge() >= 60 && !youmu_charge_ready_) {
            if (!animations.interrupt(youmu_charge_handle_, 2, false)) return fail(animations.error);
            youmu_charge_ready_ = true;
        }
    } else {
        if (!end_player_effect(youmu_charge_handle_, true)) return false;
        youmu_charge_ready_ = false;
    }

    // 00442a8f：DAT_004c2180 + 0x10 = effect.anm，不是自机 plXX.anm。
    // script22 自带 sprite23、12帧缩放、18帧旋转及子script21；松Shift
    // label1 的8帧退场也由原版脚本完成，不画近似圆、不手工改scale。
    if (visible && battle.focus) {
        if (!animations.registry.find(focus_handle_)) {
            focus_handle_ = create_player_effect(banks.effect, 22, 12, 0, position);
            if (!focus_handle_) return false;
        }
        animations.registry.find(focus_handle_)->visual.translation = position;
    } else if (!end_player_effect(focus_handle_, !visible)) {
        return false;
    }

    // 00444df0：四角色本体脚本16/18/19/15，满P光环17/20/21/17。
    // 00442960 已计算完的定点卫星位置由 battle.options 唯一提供，
    // 此处不再维护独立跟随轨迹，也不在暂停/展示帧推进卫星物理。
    constexpr i32 option_scripts[4] = {16, 18, 19, 15};
    constexpr i32 full_scripts[4] = {17, 20, 21, 17};
    const i32 character = battle.player_character();
    const i32 count = visible ? battle.option_count : 0;
    const bool full = visible && battle.full_power();
    if (character < 0 || character > 3 || count < 0 || count > 8)
        return fail("Player option state outside original bounds");
    if (character != option_character_ || count != option_count_) {
        for (u32& handle : option_handles_)
            if (!end_player_effect(handle, true)) return false;
        for (u32& handle : full_option_handles_)
            if (!end_player_effect(handle, true)) return false;
        option_character_ = character;
        option_count_ = count;
        full_options_ = false;
    }
    if (full != full_options_) {
        for (u32& handle : full_option_handles_)
            if (!end_player_effect(handle, !visible)) return false;
        full_options_ = full;
    }
    for (size_t i = 0; i < option_handles_.size(); ++i) {
        const auto& option = battle.options[i];
        const bool active = i < size_t(count) && option.active;
        if (!active) {
            if (!end_player_effect(option_handles_[i], true) ||
                !end_player_effect(full_option_handles_[i], true)) return false;
            continue;
        }
        const Vec3 anchor{option.x, option.y, 0};
        if (!animations.registry.find(option_handles_[i])) {
            option_handles_[i] = create_player_effect(
                banks.player, option_scripts[character], 12, 0, anchor);
            if (!option_handles_[i]) return false;
        }
        animations.registry.find(option_handles_[i])->visual.translation = anchor;
        if (full) {
            if (!animations.registry.find(full_option_handles_[i])) {
                full_option_handles_[i] = create_player_effect(
                    banks.player, full_scripts[character], 12, 2, anchor);
                if (!full_option_handles_[i]) return false;
            }
            animations.registry.find(full_option_handles_[i])->visual.translation = anchor;
        }
    }

    // 0040b1b0：DAT_004c22c4 + 0x10（pl00.anm）script18；
    // 0040b2a0：label1 → 子script21/23/25与19爆炸，不是effect.anm18。
    // 伤害/运动/爆炸触发全归 battle，表现层只响应原状态边沿。
    for (size_t i = 0; i < bomb_orb_handles_.size(); ++i) {
        const auto& orb = battle.bomb_orbs[i];
        if (character != 0 || (!orb.active && !orb.exploding)) {
            if (!end_player_effect(bomb_orb_handles_[i], true)) return false;
            bomb_orb_exploding_[i] = false;
            continue;
        }
        if (!animations.registry.find(bomb_orb_handles_[i]) &&
            !orb.exploding) {
            bomb_orb_handles_[i] = create_player_effect(
                banks.player, 18, -1, 0, {orb.x, orb.y, 0});
            if (!bomb_orb_handles_[i]) return false;
            bomb_orb_exploding_[i] = false;
        }
        if (auto* vm = animations.registry.find(bomb_orb_handles_[i]))
            vm->visual.translation = {orb.x, orb.y, 0};
        if (orb.exploding && !bomb_orb_exploding_[i]) {
            if (!end_player_effect(bomb_orb_handles_[i], false)) return false;
            bomb_orb_exploding_[i] = true;
        }
    }
    return true;
}

// 30x/镜像创建点色通道补步：原版这些 VM 在敌机更新趟内创建（0x41fe80
// case3 与 0x419450 镜像块，均由 0x41a8a0 趟直调），创建趟本身多推进一次
// 色/α 插值——oracle 实测 f0 VM+0x15c α=6/timer(+0x160)=2 而 port 4/1
//（f9 第二个环境精灵同症——非装载趟特例，是每次创建的普遍行为）。
// 仅色/α 两通道：整池 update 会推 world 矩阵（实测破坏 f0 对齐），脚本
// 与其余通道不动（见 AnmInterpolators::advance_color）。
void StagePresentation::create_color_step(AnmVm* vm) {
    if (vm) vm->interpolators.advance_color(vm->visual, animations.rate);
}

// 神霊池视觉同步（原版 0x438ec0 的 kind VM 创建 + 0x438730 位移尾的
// +0x574 位置回写 + 收集/回收出链）。脚本号证据：0x439088-0x4390b9
// （lea edx,[kind*3*2+1] / bit0 分支 +4），文件上下文为独立 anm file ctx
//（0x4c2164+0x10）。00405530 loads astral.anm into original resource slot25;
// these script numbers refer to that file, including its label1 fade lifetime.
bool StagePresentation::spirit_animation_alive(u32 slot, u32 serial) const noexcept {
    // A newly born spirit has not reached sync/create yet. Once bound, its
    // actual registry handle is the lifetime authority, including script op1.
    return slot >= spirit_handles_.size() || spirit_serials_[slot] != serial ||
           serial == 0 || animations.registry.find(spirit_handles_[slot]) != nullptr;
}

bool StagePresentation::sync_trance_effect(const StageBattle& battle) {
    const bool active=battle.sim.session&&trance_active(*battle.sim.session);
    if(active&&!trance_was_active_){
        auto* effect=animations.resource(banks.effect);auto* astral=animations.resource(banks.spirit);
        if(!effect||!astral)return fail("Trance animation banks unavailable");
        trance_handle_=animations.create(banks.effect,0,26,0);
        auto* vm=animations.registry.find(trance_handle_);
        if(!vm)return fail("Trance effect creation failed: "+animations.error);
        vm->geometry.overlay=std::make_unique<AnmOverlay>();
        vm->geometry.allocation_bytes=0x1c3c;
        if(!vm->geometry.overlay->initialize(*effect,*astral,*vm,random))return fail("Trance panels initialization failed");
        if(!animations.interrupt(trance_handle_,9,false))return fail(animations.error);
    }else if(!active&&trance_was_active_){
        if(!animations.interrupt(trance_handle_,1,false))return fail(animations.error);
    }
    trance_was_active_=active;return true;
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
            // 槽位复用换型：回收重建（正常路径经死亡回收，此为防御）。
            if (handle && !animations.retire(handle))
                return fail("Spirit animation rebind retire failed: " + animations.error);
            handle = 0;
            vm = nullptr;
        }
        if (!vm && !changed && s.serial != 0) {
            // Script completion is not a new birth. Ordinary spirits are
            // removed by ItemManager's handle check; trance skips that gate.
            continue;
        }
        if (!vm) {
            handle = animations.create(banks.spirit, script, -1, 2,
                                       {s.x, s.y, s.z}, 0, false);
            if (!handle)
                return fail("Spirit animation creation failed: " + animations.error);
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

// 妖梦斩击池视觉 + set_vm 回填（原版 0x40cb50 生成支路 + r4/bomb 接线）。
// 生成：script 19（cursor 偶）/ 20（奇），pl03.anm（= character 3 的
// banks.player），pos (0, lane+jitter y)，层 -1（脚本 ins_304 置 11）；
// create 的 initial tick 即执行 ins_119（角抽）与 ins_401（写 rotation.z）
// ——帧序 = battle lane 抽 → 本 create 角抽 → set_vm 回填。
// 槽生命：脚本 op1 自灭 / registry 清出 → set_vm(slot,false)（清 anm id，
// scale 冻结）；rotation 取回填当帧 VM 值。
bool StagePresentation::sync_youmu_slash(StageBattle& battle) {
    for (const auto& spawn : battle.youmu_slash_spawns_) {
        const u32 handle = animations.create(
            banks.player, spawn.script, -1, 2, {0.0f, spawn.y, 0.0f}, 0, false);
        if (!handle)
            return fail("Youmu slash animation creation failed: " +
                        animations.error);
        if (spawn.slot >= 0 && spawn.slot < YoumuBomb::kSlots) {
            youmu_slash_handles_[size_t(spawn.slot)] = handle;
        } else {
            youmu_slash_extra_.push_back(handle);  // cursor 越界：仅视觉
        }
    }
    for (int slot = 0; slot < YoumuBomb::kSlots; ++slot) {
        u32& handle = youmu_slash_handles_[size_t(slot)];
        AnmVm* vm = animations.registry.find(handle);
        // 0x46fb90 语义（r4/bomb 接线）：句柄有效即活；另加脚本终止判
        //（op1 自灭 → instruction_offset=-1/待回收 0x20 → 清槽）。
        const bool alive = vm && vm->instruction_offset != -1 &&
                           !(vm->visual.render_flags & 0x20u);
        battle.youmu_bomb_.set_vm(slot, alive,
                                  alive ? vm->variables.rotation.z : 0.0f);
        if (alive) {
            // 40cb50 invokes4300f0 immediately after obtaining this VM's
            // rotation and its pre-tick rectangle scale.
            const auto rect = battle.youmu_bomb_.rect(slot);
            battle.sim.cancel_lasers_box({rect.cx, rect.cy, 0}, {rect.w, rect.h, 0},
                                         rect.angle, true);
        }
        if (!alive && handle) handle = 0;  // 死槽：不再追踪（scale 冻结在战斗侧）
    }
    return true;
}

// file≥2 的合并 bank 偏移（r4/effects/integration §1）：append 保留各文件
// 局部 source_id（ANM {id,offset} 表内升序）→ 合并序列里 source_id 回绕处
// 即新文件起点。file 2 = 合并首文件（局部=全局，offset 0）。
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

// 真实 ECL 文件号路由（0x41fe80 / 0x4243e0 对齐原版 DAT_004c2188+0x80 槽位表）：
// 0 = bullet.anm（banks.bullet）
// 1 = effect.anm（banks.effect）
// 2.. = stage ECL ANIM 合并表（banks.stage）
StagePresentation::ScriptTarget StagePresentation::resolve_script_target(i32 file, i32 script) {
    if (file == 0) return {banks.bullet, script};
    if (file == 1) return {banks.effect, script};
    return {banks.stage, stage_script_global(file, script)};
}

// 局部脚本号 → 合并全局号；file 超出已知文件数 = 阻塞（不猜）。
i32 StagePresentation::stage_script_global(i32 file, i32 script) {
    if (file < 2) return script;
    const auto& offsets = stage_file_offsets();
    const size_t index = size_t(file - 2);
    if (index >= offsets.size()) {
        fail("Stage ANM file index beyond merged bank boundaries: " +
             std::to_string(file));
        return -1;
    }
    return script + offsets[index];
}

// 道具池视觉同步（0x42f020：两次 0x403800 模板拷贝 + 0x462450 首拍）。
// 脚本号 = Unicorn 运行时实证（r4/items/item_vm_observer.py，th13_01）：
// 模板索引 type+0x59 / type+0x62 即 bullet.anm 局部脚本号 → VM1 = 89+N、
// VM2 = 98+N（sprite 459+N / 468+N）；模板层字段 +0x24 = 0。
// 每帧位置跟随（0x42e380 尾 vm+0x574 写）；死亡（收集/底回收）→ retire。
bool StagePresentation::sync_items(StageBattle& battle) {
    auto& items = battle.item_manager.items();
    for (size_t i = 0; i < items.size(); ++i) {
        const Item& item = items[i];
        u32& handle = item_handles_[i];
        u32& side = item_side_handles_[i];
        if (!item.alive) {
            if (handle && !animations.retire(handle))
                return fail("Item animation retire failed: " + animations.error);
            if (side && !animations.retire(side))
                return fail("Item side animation retire failed: " + animations.error);
            handle = 0;
            side = 0;
            continue;
        }
        if (item.type < 1 || item.type > 9) continue;  // 10..14 走神霊路径
        if (!animations.registry.find(handle)) {
            handle = animations.create(banks.bullet, 89 + item.type, 0, 2,
                                       {item.x, item.y, item.z}, 0, false);
            if (!handle)
                return fail("Item animation creation failed: " +
                            animations.error);
        }
        if (!animations.registry.find(side)) {
            side = animations.create(banks.bullet, 98 + item.type, 0, 2,
                                     {item.x, item.y, item.z}, 0, false);
            if (!side)
                return fail("Item side animation creation failed: " +
                            animations.error);
        }
        if (AnmVm* vm = animations.registry.find(handle))
            vm->visual.translation = {item.x, item.y, item.z};
        if (AnmVm* vm = animations.registry.find(side))
            vm->visual.translation = {item.x, item.y, item.z};
    }
    return true;
}

bool StagePresentation::flush_ecl_animations(StageBattle& battle, StageEnemy* constructing) {
    // ---- ECL 30x ANM 创建（0x4243e0：模板拷贝→立即 tick→主池头入链。
    // kind=0 特效路径 layer=-1（脚本自定层）、anmArg=0、位置清零（307 的
    // 敌机位仅特效路径用）；kind=1 槽位路径见下方 [+0x278]+7 注。
    // 文件索引：0=bullet、1=effect、2..=stage 合并表（脚本号文件内局部——
    // 文件 3+ 的合并偏移为已知局限，窗口内敌机均用文件 2）----
    for (const auto& request : battle.sim.anm_effects) {
        if (request.kind == 0) {
            const auto target = resolve_script_target(request.file, request.script);
            if (target.script < 0) return false;
            const u32 effect_handle =
                animations.create(target.bank, target.script, -1, 2,
                                  {request.x, request.y, request.z}, 0, false);
            if (!effect_handle)
                return fail("Effect animation creation failed: " + animations.error);
            create_color_step(animations.registry.find(effect_handle));
            continue;
        }
        // kind=1：303/306 槽位 VM（0x41fe80 / 0x41b260 case3）——重挂先回收
        // 旧槽（0x470110 语义：retire+子树 0x20 位），script<0 仅清槽。
        StageEnemy* enemy = constructing && constructing->serial == request.serial ? constructing : nullptr;
        for (auto& candidate : battle.sim.enemies)
            if (candidate->serial == request.serial) {
                enemy = candidate.get();
                break;
            }
        if (!enemy) continue;
        u32& handle = enemy->anm_slot_handle[request.slot];
        if (handle) {
            if (!animations.retire(handle))
                return fail("Slot animation retire failed: " + animations.error);
            handle = 0;
        }
        if (request.script < 0) continue;
        // 槽位 VM 创建层 = 敌机内嵌 VM+0x278 + 7（0x41fe80:0x41ff24 `mov
        // 0x278(%esi),%eax; add $0x7,%eax`——脚本模板不自带层，此前传 -1
        // （脚本自定层）使 aura 挂错层）。该字段构造默认 1（StageEnemy
        // mirror_script_base，ECL 552 可覆盖）→ 层 8（与 oracle 敌机
        // aura up 层 4/8 对拍）。
        const auto target = resolve_script_target(request.file, request.script);
        if (target.script < 0) return false;
        handle = animations.create(target.bank, target.script,
                                   enemy->mirror_script_base + 7, 2, {}, 0, false);
        if (!handle)
            return fail("Slot animation creation failed: " + animations.error);
        create_color_step(animations.registry.find(handle));
        if (request.slot == 0 && request.opcode != 318) {
            if (AnmVm* vm = animations.registry.find(handle)) {
                const float w = float(vm->visual.sprite_size.x *
                                      vm->visual.scale.x);
                const float h = float(vm->visual.sprite_size.y *
                                      vm->visual.scale.y);
                // 0x41fe80: +0x3f30=(vm+0x84)×(vm+0x64)、+0x3f34=(vm+0x80)×(vm+0x60)
                enemy->anm_size_a[0] = h;
                enemy->anm_size_a[1] = w;
                if (request.opcode == 306) {
                    enemy->anm_size_b[0] = h;
                    enemy->anm_size_b[1] = w;
                }
            }
        }
    }
    battle.sim.anm_effects.clear();
    return true;
}

bool StagePresentation::enemy_death(i32 file,i32 script,const Vec3& position,float angle) {
    const auto target=resolve_script_target(file,script);
    if(target.script<0)return false;
    // 41ab7f ->46f820(bank,script,pos,angle,layer4,flags0).
    return animations.create(target.bank,target.script,4,0,position,angle,false)!=0 ||
           fail("Enemy death animation: "+animations.error);
}

void StagePresentation::retire_enemy_fog(u32 serial) {
    const auto found=fog_handles_.find(serial);if(found==fog_handles_.end())return;
    for(auto& handle:found->second)if(!animations.retire(handle))fail(animations.error);
    fog_handles_.erase(found);
}

bool StagePresentation::enemy_fog(StageEnemy& enemy,bool reset) {
    if(reset||!enemy.fog)retire_enemy_fog(enemy.serial);
    if(!enemy.fog)return error_.empty();
    auto& handles=fog_handles_[enemy.serial];
    for(size_t column=0;column<handles.size();++column) {
        if(!handles[column]) {
            // 4159c0 ->43cac0: text82 samples @R, layer29, tail insertion,
            // mode12 strip and screen coordinates. 0x43cac0 only replaces the
            // draw-mode field (mask 0xd9ffffff | 0x18000000); script82's op303
            // blend field (3 = One/Zero opaque strip) is preserved.
            handles[column]=animations.create(0,82,29,0);
            auto* vm=animations.registry.find(handles[column]);
            if(!vm)return fail("Enemy fog render-target animation: "+animations.error);
            vm->visual.flags=(vm->visual.flags&~0x3e000000u)|(12u<<25);
            vm->visual.render_flags&=~0x40000u;
            vm->variables.integers[0]=17;
            vm->geometry.screen_vertices.resize(34);
        }
        auto* vm=animations.registry.find(handles[column]);
        if(!vm)return fail("Enemy fog strip retired while enemy is live");
        if(!reset)for(size_t row=0;row<17;++row) {
            vm->geometry.screen_vertices[row*2]=enemy.fog->vertices[column*17+row];
            vm->geometry.screen_vertices[row*2+1]=enemy.fog->vertices[(column+1)*17+row];
        }
    }
    return true;
}

bool StagePresentation::sync(StageBattle& battle, u32 held) {
    bullet_cancel_handles_.erase(std::remove_if(bullet_cancel_handles_.begin(),bullet_cancel_handles_.end(),
        [this](u32 handle){return !animations.registry.find(handle);}),bullet_cancel_handles_.end());
    if(!sync_trance_effect(battle))return false;
    if (!error_.empty()) return false;
    if (animations.scene_environment().paused) return true;
    auto* stage_resource = animations.resource(banks.stage);
    auto* bullet_resource = animations.resource(banks.bullet);
    auto* player_resource = animations.resource(banks.player);
    if (!stage_resource || !bullet_resource || !player_resource)
        return fail("Stage presentation banks unavailable");
    // The next battle step consumes this snapshot before sim.step_frame().
    // It is deliberately refreshed only after the previous frame's VM tick.
    battle.begin_player_shot_geometry_snapshot();
    if (!sync_player_effects(battle)) return false;
    if (!sync_items(battle)) return false;
    if (!sync_item_spirits(battle)) return false;
    if (!sync_youmu_slash(battle)) return false;

    if (!flush_ecl_animations(battle)) return false;
    // 槽位 VM 位置跟随（0x41a8a0:84-93：敌机管理器每帧把敌机 ECL 位置
    // （enemy+0x44/48/4c）写入各槽 VM 的偏移 +0x574/578/57c——0x466a80 的
    // 位置和项；+320/+16 由 0x40000 位在 adjust 阶段补，此处只写 ECL 原值）。
    if (enemies_from_ecl) {
        for (auto& enemy : battle.sim.enemies) {
            // 镜像翻转重挂（0x4196d3-0x419827 对齐反汇编全解）：
            // 门 = flags&0x200000 且 x 速度符号（+0x78 语义，位置差分近似）
            // 变化；行走 VM 脚本号 = 槽 0 脚本号 + 朝向增量查表（旧 0：
            // 新 -1→1 / 新 0,+1→2；旧 +1：恒 4；旧 -1：恒 3）；layer 恒 8；
            // 位置 = 旧槽 VM 位置（继承，非零）；anmArg=0；尾入池句柄存回槽 0。
            if (enemy->flags & 0x200000u) {
                const float dx = enemy->position[0] - enemy->mirror_prev_x;
                const i32 dir = dx > 0.03f ? 1 : (dx < -0.03f ? -1 : 0);
                if (dir != enemy->mirror_dir) {
                    i32 step;
                    if (enemy->mirror_dir == 0)
                        step = dir == -1 ? 1 : 2;
                    else if (enemy->mirror_dir == 1)
                        step = 4;
                    else
                        step = 3;
                    u32& handle = enemy->anm_slot_handle[0];
                    // 位置继承 = 翻转帧的敌机 ECL 位（原版读旧槽 VM+0x574，
                    // 其值即当帧跟随位——本 sync 尚未写跟随，故直接取敌机位）
                    const Vec3 old_pos{enemy->position[0], enemy->position[1],
                                       enemy->position[2]};
                    if (handle) {
                        if (!animations.retire(handle))
                            return fail("Mirror retire failed: " + animations.error);
                        handle = 0;
                    }
                    // 槽 0 脚本号 = 306 记录（enemy->anm_base_script）；
                    // layer = 敌机内嵌 VM+0x278 + 7（0x419450:0x419764 实证，
                    // mirror_script_base 构造默认 1 → 层 8，ECL 552 可覆盖）；
                    // 插入方向 = 主链尾插（0x419764 块直调 0x46f3f0——f92
                    // Unicorn 实测 F3F0-tail，重挂 VM 落链尾、层桶序排旧 VM
                    // 之后。此前沿用 create 默认 ordering=2（0x46f4a0 头插
                    // 语义；303/306 建池 0x4243e0 才是头插）→ f92 op[12] 的
                    // 层内序 Blue 先于 Red，与 oracle 相反（draws 首分歧）。
                    const auto target = resolve_script_target(enemy->anm_file,
                                                              enemy->anm_base_script + step);
                    if (target.script < 0) return false;
                    handle = animations.create(target.bank, target.script,
                                               enemy->mirror_script_base + 7,
                                               0, old_pos, 0, false);
                    if (!handle)
                        return fail("Mirror animation creation failed: " +
                                    animations.error);
                    create_color_step(animations.registry.find(handle));
                    enemy->mirror_dir = dir;
                }
                enemy->mirror_prev_x = enemy->position[0];
            }
            for (u32 slot = 0; slot < StageEnemy::kAnimationSlots; ++slot)
                if (AnmVm* vm = animations.registry.find(enemy->anm_slot_handle[slot])) {
                    // 41a8a0 adds inner+160 offsets unless flag08000000
                    // selects direct enemy-position following.
                    const Vec3 offset = enemy->flags & 0x8000000u
                        ? Vec3{} : enemy->anm_slot_offset[slot];
                    vm->visual.translation = {offset.x + enemy->position[0],
                                              offset.y + enemy->position[1],
                                              enemy->position[2] + offset.z};
                }
        }
    }

    // ---- 敌机：ECL 记录的槽 0 脚本（全局序号），位置逐帧跟随 ----
    //（enemies_from_ecl 时整段跳过：敌机本体 VM 由 ECL 303/306 创建入池，
    // 与 oracle 极小环境同构——对拍窗语义）
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
        if (script < 0) continue;  // 无本体脚本（纯发射器敌机）
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
        slot->vm->visual.translation = {enemy->position[0] + kGameToScreenX,
                                        enemy->position[1] + kGameToScreenY, enemy->position[2]};
        if (slot->vm->tick(random, animations.rate) < 0) return fail("Enemy animation: " + slot->vm->error);
    }
    // 消失的敌机：销毁其 ANM 子树（子体可能仍挂 registry，release_tree 归还）。
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
    }  // !enemies_from_ecl

    // Production bullet ANM is advanced by StageSim at birth/update time.
    // Standalone presentation clients retain the existing sync entry point.
    if (battle.sim.bullet_animation) {
        for (auto& slot : bullets_) {
            bool live = false;
            for (const auto& bullet : battle.sim.bullets)
                if (bullet.alive && slot.generation == bullet.generation) { live = true; break; }
            slot.active = live;
        }
    } else {
    // ---- 敌弹：bullet.anm 脚本 + 外观表精灵源（出生中断 2，0x40e840 尾）----
    if (bullets_.size() < battle.sim.bullets.size()) {
        const size_t wanted = battle.sim.bullets.size();
        while (bullets_.size() < wanted) bullets_.push_back(BulletSlot{});
    }
    // StageSim compacts its bullet vector after every update.  Match existing
    // VMs by birth identity rather than by the compacted index; the original
    // bullet manager keeps a distinct ANM root per live bullet.
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
            if (!matched[i] && bullets_[i].active &&
                bullets_[i].generation == bullet.generation) {
                slot_index = i;
                break;
            }
        }
        if (slot_index == bullets_.size()) {
            // Reuse an inactive storage slot only after retiring its old VM;
            // otherwise a same-script/color bullet inherits its old timer.
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
        if (script < 0) continue;  // 未入表的弹型：无视觉（碰撞仍生效）
        if (!slot.vm) slot.vm = std::make_unique<AnmVm>();
        // 槽位复用（池按下标分配）：弹色变化 = 不同弹体，重绑并重放出生中断。
        if (!slot.vm->resource || slot.script != script || slot.color != i32(bullet.subtype)) {
            slot.vm->environment = &animations.scene_environment();
            slot.vm->object_host = &animations;
            if (!slot.vm->bind(*bullet_resource, u32(script)))
                return fail("Bullet animation binding failed: " + slot.vm->error);
            slot.vm->sprite_source = &slot.source;
            // 461cb0 executes the script's initialization before40e840 sends
            // the birth interrupt. Skipping it loses sprite/draw-mode setup.
            if (slot.vm->tick(random, animations.rate) < 0) return fail("Bullet initialization: " + slot.vm->error);
            slot.vm->visual.render_flags |= 0x40000;
            slot.vm->pending_interrupt = bullet.state == 2 && bullet.delay ? bullet.delay : 2;
            slot.script = script;
            slot.color = i32(bullet.subtype);
        }
        slot.vm->visual.translation = {bullet.x + kGameToScreenX, bullet.y + kGameToScreenY,
                                       bullet.z};
        if (slot.vm->tick(random, animations.rate) < 0) return fail("Bullet animation: " + slot.vm->error);
        sync_bullet_heading(*slot.vm, bullet.angle);
    }
    for (size_t i = 0; i < bullets_.size(); ++i)
        if (!matched[i]) bullets_[i].active = false;

    }

    // ---- 自机弹：.sht 弹型脚本（plXX.anm，旋转模式 1 随弹向）；无 .sht
    // 会话回退精灵直贴（player_shot_sprite_） ----
    if (shots_.size() < battle.shots.size())
        while (shots_.size() < battle.shots.size()) shots_.push_back(std::make_unique<AnmVm>());
    shot_generations_.resize(shots_.size());
    for (size_t i = 0; i < shots_.size(); ++i) {
        if (i < battle.shots.size() && battle.shots[i].alive) {
            const auto& shot = battle.shots[i];
            // Compaction changes slots, not ANM identity. Preserve a survivor's
            // VM; a new shot binds fresh even if its script matches the old one.
            if (shot_generations_[i] != shot.generation) {
                size_t source = i + 1;
                while (source < shots_.size() &&
                       shot_generations_[source] != shot.generation) ++source;
                if (source < shots_.size()) {
                    std::swap(shots_[i], shots_[source]);
                    std::swap(shot_generations_[i], shot_generations_[source]);
                } else {
                    if (shots_[i]->resource && !animations.release_tree(*shots_[i]))
                        return fail("Player shot animation release failed");
                    *shots_[i] = AnmVm{};
                    shot_generations_[i] = shot.generation;
                }
            }
            AnmVm& vm = *shots_[i];
            const i32 script = shot.script;
            const bool binding_shot = script >= 0 && vm.source_script != script;
            if (binding_shot) {
                if (vm.resource) animations.release_tree(vm);
                vm.environment = &animations.scene_environment();
                vm.object_host = &animations;  // 弹型脚本可 spawn 附属弹子 VM
                if (!vm.bind(*player_resource, u32(script)))
                    return fail("Player shot animation binding failed: " + vm.error);
            } else if (script < 0 && (!vm.resource || vm.source_script >= 0)) {
                vm.environment = &animations.scene_environment();
                vm.initialize_unbound();
                vm.resource = player_resource;
                if (!vm.select_sprite(player_shot_sprite_))
                    return fail("Player shot sprite unavailable");
                vm.visible = true;
                vm.visual.flags |= 1;
                vm.visual.render_flags |= 0x200;
                vm.visual.set_layer(11);
            }
            vm.visual.translation = {shot.x + kGameToScreenX, shot.y + kGameToScreenY, 0.0f};
            sync_shot_heading(vm, shot.angle);
            if (script >= 0 && vm.resource) {
                const i32 result = vm.tick(random, animations.rate);
                if (result < 0) return fail("Player shot animation: " + vm.error);
                // 004464d0 retires the damage registration when its ANM
                // disappears. Youmu's piercing slash scripts terminate at16.
                if (result > 0 && shot.hit_behavior == 4) {
                    battle.shots[i].alive = false;
                    if (!animations.release_tree(vm)) return fail(animations.error);
                    vm = AnmVm{};
                    shot_generations_[i] = 0;
                    continue;
                }
            }
            // Newly bound scripts establish ins424 during this first tick.
            if (binding_shot) sync_shot_heading(vm, shot.angle);
            if (script >= 0 && shot.type != 2) {
                Vec3 corners[4];
                if (!anm_quad_positions(vm, corners))
                    return fail("Player shot geometry snapshot unavailable");
                std::array<Vec3, 4> snapshot{};
                std::copy(std::begin(corners), std::end(corners), snapshot.begin());
                battle.set_player_shot_geometry(i32(i), shot.age, script, shot.generation,
                                                snapshot);
            }
        } else {
            // 0x4464d0 只有两种死法：出界（上一帧四角全离开 [128,512]×
            // [16,464] 且快照龄>9、类型≠2 —— consume_player_shot_geometry
            // 已在 step_shots 内镜像同一谓词先行剔除）与命中（sim.step_frame
            // 的 0x446870 → 伤害记录回调 0x446bf0）。此处用同一 VM 四角复算
            // 同一谓词区分死因：VM 自上帧快照后未再推进，四角与快照逐位相等，
            // 判定与 battle 侧 consume 完全同数据同条件，无启发式。
            AnmVm& vm = *shots_[i];
            bool fade = false;
            PlayerShot corpse_shot{};
            if (i < battle.shots.size() && !battle.shots[i].alive) {
                const PlayerShot& shot = battle.shots[i];
                corpse_shot = shot;
                const bool snapshotted = shot_generations_[i] == shot.generation &&
                                         vm.resource &&
                                         vm.source_script == shot.script && shot.type != 2;
                bool outside = false;
                if (snapshotted && shot.age - 1 > 9) {  // consume 的快照龄门
                    Vec3 corners[4];
                    if (!anm_quad_positions(vm, corners))
                        return fail("Player shot death geometry unavailable");
                    outside = true;
                    for (const auto& corner : corners)
                        outside = outside && (corner.x <= 128.0f || corner.x >= 512.0f ||
                                              corner.y <= 16.0f || corner.y >= 464.0f);
                }
                fade = !outside && shot.script >= 0 &&
                       shot_faded_generations_.find(shot.generation) ==
                           shot_faded_generations_.end() &&
                       shot_script_has_interrupt(*player_resource, u32(shot.script), 1);
            }
            if (fade) {
                // 0x446bf0：VM+0x4a0=1（中断段=脚本后半 layer13 渐隐）；弹转
                // dying：速度×0.125、z=0x3dcccccd(0.1)、VM 位姿回写死亡位
                //（battle.step_shots 的本帧整速位移已在命中前完成，不再补动）。
                if (!vm.resource || vm.source_script != corpse_shot.script) {
                    // 出生帧即命中、从未同步绑定的弹：补绑后直接进中断段。
                    if (vm.resource && !animations.release_tree(vm))
                        return fail("Player shot animation release failed");
                    vm.environment = &animations.scene_environment();
                    vm.object_host = &animations;  // 与存活路径同一宿主接线
                    if (!vm.bind(*player_resource, u32(corpse_shot.script)))
                        return fail("Player shot animation binding failed: " + vm.error);
                }
                ShotFade corpse;
                corpse.generation = corpse_shot.generation;
                corpse.vm = std::move(shots_[i]);
                corpse.x = corpse_shot.x;
                corpse.y = corpse_shot.y;
                corpse.angle = corpse_shot.angle;
                corpse.drift = corpse_shot.speed * 0.125f;
                corpse.sine_moving = corpse_shot.sine_moving;
                corpse.sine_motion = corpse_shot.sine_motion;
                corpse.vm->pending_interrupt = 1;  // 0x446bf0: VM+0x4a0 = 1
                corpse.vm->visual.translation = {corpse.x + kGameToScreenX,
                                                 corpse.y + kGameToScreenY,
                                                 corpse.sine_moving ? 0.0f : 0.1f};
                sync_shot_heading(*corpse.vm, corpse.angle);
                shot_faded_generations_.insert(corpse.generation);
                shot_fades_.push_back(std::move(corpse));
                shots_[i] = std::make_unique<AnmVm>();
                shot_generations_[i] = 0;
            } else {
                // 出界死（0x470110 语义）或无中断段脚本：立即回收。
                if (vm.resource) {
                    animations.release_tree(vm);
                    vm = AnmVm{};
                }
                vm.visible = false;
                shot_generations_[i] = 0;
            }
        }
    }

    // ---- 命中留尸推进（0x4464d0 dying 惰行 + VM 跟随 → op1 自灭回收）----
    // 原版 dying 弹仍走 0x4464d0：位移（减速率 0.125 后的 speed）、0x473780
    // 0.01 量化、VM+0x574 位姿回写、flags&0x40 时角度回写；随后 anm 推进。
    // 脚本 op1/-1 自灭（tick 返回 1）后回收——对应原版次帧 0x46fb90 查无
    // VM 的槽位释放路径（视觉自灭帧即不可见，回收无观察差）。
    for (size_t n = 0; n < shot_fades_.size();) {
        ShotFade& corpse = shot_fades_[n];
        AnmVm& vm = *corpse.vm;
        if (corpse.age > 0) {  // 命中当帧不惰行（整速位移已计入死亡位）
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
            vm.visual.translation = {corpse.x + kGameToScreenX,
                                     corpse.y + kGameToScreenY,
                                     corpse.sine_moving ? 0.0f : 0.1f};
            sync_shot_heading(vm, corpse.angle);
        }
        ++corpse.age;
        const i32 result = vm.tick(random, animations.rate);
        if (result < 0) return fail("Player shot fade animation: " + vm.error);
        if (result == 1) {
            animations.release_tree(vm);
            corpse.vm.reset();
            shot_fades_.erase(shot_fades_.begin() + i32(n));
            continue;
        }
        ++n;
    }

    // ---- 自机：00442960，0直立、1左倾、2左倾回正、3右倾、4右倾回正 ----
    if (battle.player_visible()) {
        const i32 tilt = ((held & 0x80) ? 1 : 0) - ((held & 0x40) ? 1 : 0);
        if (!player_) player_ = std::make_unique<AnmVm>();
        if (!player_->resource || tilt != player_tilt_) {
            const i32 script = tilt < 0 ? 1 : tilt > 0 ? 3
                                : player_tilt_ < 0 ? 2 : player_tilt_ > 0 ? 4 : 0;
            if (player_->resource) animations.release_tree(*player_);
            player_->environment = &animations.scene_environment();
            player_->object_host = &animations;
            if (!player_->bind(*player_resource, u32(script)))
                return fail("Player animation binding failed: " + player_->error);
            player_->visual.set_layer(12);
            player_tilt_ = tilt;
            player_script_ = script;
        }
        player_->visual.translation = {battle.player_x + kGameToScreenX,
                                       battle.player_y + kGameToScreenY, 0.0f};
        if (player_->tick(random, animations.rate) < 0) return fail("Player animation: " + player_->error);
        // 0044378x：无敌时每3个状态帧使用蓝色secondary通道；不是隐藏精灵。
        player_->visual.flags &= ~0x60000u;
        if (battle.invulnerable > 0 && battle.player_phase_frame % 3 == 0) {
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
    std::snprintf(line, sizeof line,
                  "enemies=%zu bullets=%zu shots=%zu player=%d spirits=%zu items=%zu\n",
                  enemies_.size(),
                  bullets_.empty() ? 0 : std::count_if(bullets_.begin(), bullets_.end(),
                                                       [](const BulletSlot& slot) {
                                                           return slot.active && slot.vm;
                                                       }),
                  shots_.size(), player_ ? 1 : 0,
                  size_t(std::count_if(spirit_handles_.begin(), spirit_handles_.end(),
                                       [this](u32 handle) {
                                           return handle && animations.registry.find(handle);
                                       })),
                  size_t(std::count_if(item_handles_.begin(), item_handles_.end(),
                                       [this](u32 handle) {
                                           return handle && animations.registry.find(handle);
                                       })));
    report += line;
    auto describe = [&](const char* kind, const AnmVm& vm, i32 type, i32 color,
                        u32 generation) {
        const auto* source = vm.sprite_resource ? vm.sprite_resource : vm.resource;
        std::snprintf(line, sizeof line,
                      "  %s script=%d sprite=%d size=%.0fx%.0f scale=%.2fx%.2f "
                      "pos=(%.0f,%.0f) rot=%.2f mode=%u blend=%u type=%d color=%d age=%d gen=%u\n",
                      kind, vm.source_script, vm.visual.sprite,
                      double(vm.visual.sprite_size.x), double(vm.visual.sprite_size.y),
                      double(vm.visual.scale.x), double(vm.visual.scale.y),
                      double(vm.visual.translation.x), double(vm.visual.translation.y),
                      double(vm.variables.rotation.z), unsigned(vm.visual.draw_mode()),
                      unsigned(vm.visual.blend_mode()), type, color, vm.age.current,
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
    if (!shots_.empty() && shots_[0])
        describe("shot", *shots_[0], player_shot_sprite_, -1, 0);
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
    if (!(debug_pass_mask >> pass & 1u)) return true;  // 调试过滤
    // [2026-10-24 拔除 -96 平移 hack] 旧注释称「原版 stage 上下文视口原点
    // X=+96、表现层渲染位置 = ECL+224（图像实证 f901 自机 ~178）」并据此对
    // 四 pass 直绘统一 renderer.offset -= 96——该「实证」的参照系是**裁剪
    // 错误的 golden**（速通视频缺游戏左侧 ~97px，2026-10-24 勘误，见
    // scripts/video_golden_extract.py 头注）。oracle 数据反证：st01-draws
    // -replay.jsonl f124 敌机 up 批 verts 屏幕坐标 x∈[411.5,443.5]（layer
    // 56，ECL≈+107 → ECL+320 直出），[32,416] 视口假设下该敌机出界。原版
    // 表现层 = ECL+320 直绘于 playfield [128,512]×[16,464]，无 -96 平移。
    // 同步修正：ScreenViews 游戏相机视口 32→128。
    const auto one = [&](AnmVm& vm) {
        if (!vm.visible) return true;
        return renderer.draw(vm) != -2 || fail(renderer.error.empty() ? "Stage draw failed"
                                                                      : renderer.error);
    };
    switch (pass) {
    case 0:
        for (auto& shot : shots_)
            if (shot) one(*shot);
        // 命中留尸 VM 同层直绘（原版 layer13 渐隐条与在飞亮条同 pass）。
        for (auto& corpse : shot_fades_)
            if (corpse.vm) one(*corpse.vm);
        break;
    case 1:
        if (player_) one(*player_);
        break;
    case 2:
        for (auto& enemy : enemies_)
            if (enemy.vm) one(*enemy.vm);
        break;
    case 3:
        for (auto& slot : bullets_)
            if (slot.active && slot.vm) one(*slot.vm);
        break;
    default:
        return fail("Stage draw pass outside schedule");
    }
    return error_.empty();
}

}  // namespace th13
