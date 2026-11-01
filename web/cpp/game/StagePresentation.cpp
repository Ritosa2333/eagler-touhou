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

// 只读创建来源探针：与 AnmInterpreter.cpp 的观察器同型（同一函数指针变量，
// 由浏览器探针臂接）。哨兵 opcode -10=create_player_effect / -11=shot，
// variable=script、argument=ordering、before=bank。不消费 RNG、不改 VM。
using AnmRandomObserver = void (*)(const AnmVm&, i32, i32, i32, u32, u32, u32, u32, bool);
extern AnmRandomObserver anm_random_observer;

// 调试直绘过滤（4 pass 全开）。
u32 StagePresentation::debug_pass_mask = 0xf;

namespace {
// 游戏坐标 → ANM 屏幕坐标：+320/+16 屏幕原点已由 0x40000 位在底层
// adjust_position 统一累加（th13 .data 双精度常量 0x4ae9f0=320.0 /
// 0x4ae8e0=16.0——0x466a80/0x468670/0x46f820 族一致；th15 遗产的 224
// 是 f61 敌机行走位差 96px 的根因，2026-10-23 实测修正），此处外部平移
// 增量置 0，杜绝双重叠加。
constexpr float kGameToScreenX = 0.0f, kGameToScreenY = 0.0f;
// 弹体贴图基准角：贴图朝右时旋转 = 弹角（对屏检验后校准）。
constexpr float kBulletSpriteForward = 0.0f;

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
      random(manager.random_source()), player_shot_sprite_(player_shot_sprite) {
    // 自机弹 VM 的 tick 属主链 0x46f170 遍历（Application Update 34），晚于
    // 本层 sync（20）。四角剔除（0x446531）消费的是上一帧链遍历后的 VM 状态，
    // 故快照与自灭检出必须挂到主链 update 完成之后。钩子失败时把本层的具体
    // 原因（四角几何不可用等）转写进 AnmManager::error，避免被 update 的
    // 通用 "post-update hook failed" 覆盖（Application 只上报 animations.error）。
    install_main_update_hook();
}

void StagePresentation::install_main_update_hook() {
    // 换关时本对象被 preserve_presentation_from 移入新 RunScene，旧 battle
    // 已析构：复位 battle_/frame_synced_，由新 RunScene 的 sync 重新绑定。
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
    // 只清自己的钩子：换关时旧 RunScene（previous_run）在新 RunScene 之后
    // 析构，旧 presentation 若无条件清空会抹掉新 presentation 的钩子，
    // 使自机弹四角快照/自灭检出在 S1→S2 后整段失效（伤害注册池耗尽）。
    if (animations.after_main_update_owner == this) {
        animations.after_main_update = nullptr;
        animations.after_main_update_owner = nullptr;
    }
}

StagePresentation::BulletSlot* StagePresentation::bullet_slot(StageBullet& bullet) {
    BulletSlot* slot = nullptr;
    for (auto& entry : bullets_)
        if (entry.active && entry.generation == bullet.generation) { slot = &entry; break; }
    if (!slot) {
        for (auto& entry : bullets_) if (!entry.active) { slot = &entry; break; }
        if (!slot) { bullets_.emplace_back(); slot = &bullets_.back(); }
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
        // 0040db10 passes the sprite descriptor+38/+34, independently of
        // mutable VM sizes/scales used by the renderer.
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
    // 00405010 + 00461cb0 run immediately at birth and at EVERY etEx200,
    // even if the type/color are unchanged. The queue can then replace the
    // pending birth label before the frame's single 00462450 tail update.
    vm.environment = &animations.scene_environment();
    vm.object_host = &animations;
    if (!vm.bind(*resource, u32(appearance->script))) return fail(vm.error);
    vm.sprite_source = &slot->source;
    if (vm.tick(random, animations.rate) < 0) return fail(vm.error);
    vm.visual.render_flags |= 0x40000;
    vm.pending_interrupt = interrupt;
    vm.visual.translation = {bullet.x, bullet.y, bullet.z};
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
        // Standalone callers may enter here without StageSim's birth callback.
        if (!initialize_bullet(bullet, bullet.state == 2 ? bullet.delay : 2)) return false;
    }
    auto& vm = *slot->vm;
    if (bullet.animation_interrupt) vm.pending_interrupt = bullet.animation_interrupt;
    bullet.animation_interrupt = 0;
    if (bullet.animation_blend >= 0) {
        vm.visual.flags = (vm.visual.flags & ~0x1e0u) | (u32(bullet.animation_blend) << 5);
        bullet.animation_blend = -1;
    }
    vm.visual.translation = {bullet.x, bullet.y, bullet.z};
    vm.variables.rotation.z = bullet.angle + kBulletSpriteForward;
    const int result = vm.tick(random, animations.rate);
    if (result < 0) return fail("Bullet animation: " + vm.error);
    bullet_animation_geometry(bullet, vm);
    if (result > 0) { bullet.alive = false; slot->active = false; }
    (void)birth;
    return true;
}

bool StagePresentation::bullet_cancel(i32 script,const Vec3& position,const Vec3& displacement,bool interpolate) {
    const u32 handle=animations.create(banks.bullet,script,-1,0,position,0,false);
    auto* vm=animations.registry.find(handle);
    if(!vm)return fail("Bullet cancel particle: "+animations.error);
    if(anm_random_observer)anm_random_observer(*vm,-13,script,0,0,0,0,0,false);
    vm->visual.render_flags|=0x40000u;
    if(interpolate)vm->interpolators.position.begin(30,6,{0,0,0},
        {displacement.x,displacement.y,displacement.z}); //4125b0, no extra tick
    bullet_cancel_handles_.push_back(handle);
    return true;
}

bool StagePresentation::beam_hit_effect(const Vec3& position, const Vec3& displacement,
                                        float angle) {
    // 447230 -> 46f820 creates player ANM script15 and primes it immediately.
    // The impact rotation and 4125b0's 20-frame mode4 local displacement are
    // installed only after the initial tick, before the manager's next update.
    const u32 handle = animations.create(banks.player, 15, -1, 0, position, 0, false);
    auto* vm = animations.registry.find(handle);
    if (!vm) return fail("Player beam hit effect: " + animations.error);
    vm->visual.flags |= 4u;
    vm->variables.rotation.z = angle;
    vm->interpolators.position.begin(20, 4, {0, 0, 0},
                                     {displacement.x, displacement.y, displacement.z});
    player_effect_handles_.push_back(handle);
    return true;
}

bool StagePresentation::youmu_charge_effect(i32 action, const Vec3& position) {
    if (action == 0) {
        // 446122..446163 creates player-bank script18 via 46f820 immediately.
        youmu_charge_handle_ = animations.create(banks.player, 18, -1, 0, position, 0, false);
        if (!youmu_charge_handle_) return fail("Youmu charge effect: " + animations.error);
        player_effect_handles_.push_back(youmu_charge_handle_);
    } else if (action == 1 && youmu_charge_handle_) {
        // 4461f0 at charge60 queues label2 to this VM and its children.
        if (!animations.interrupt(youmu_charge_handle_, 2, false))
            return fail("Youmu charge interrupt: " + animations.error);
    } else if (action == 2 && youmu_charge_handle_) {
        if (!animations.retire(youmu_charge_handle_))
            return fail("Youmu charge retirement: " + animations.error);
    } else if (action == 3) {
        if (auto* vm = animations.registry.find(youmu_charge_handle_))
            vm->visual.translation = position;
        else youmu_charge_handle_ = 0;
    }
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
    for (auto& vm : item_vms_) vm.reset();
    for (auto& vm : item_side_vms_) vm.reset();
    for (u32& handle : spirit_handles_) { if (handle) animations.retire(handle); handle = 0; }
    spirit_serials_.fill(0); spirit_scripts_.fill(0);
    stage_file_offsets_.clear(); stage_offsets_source_ = nullptr;
}

void StagePresentation::release() {
    battle_ = nullptr;  // 主链 update 钩子不再读取已释放的弹体槽位
    frame_synced_ = false;
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
        if (handle && !animations.retire(handle))
            fail("Spirit animation retire failed: " + animations.error);
        handle = 0;
    }
    spirit_serials_.fill(0);
    spirit_scripts_.fill(0);
    for (auto& vm : item_vms_) vm.reset();
    for (auto& vm : item_side_vms_) vm.reset();
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
    // correctedPlan 之外的故意补充（review correction 4）：release() 是全量
    // 场景卸载复位，若同一 presentation 对象在下一场景复用，残留的
    // option_rebuild_pending_ 会让 sync_player_options 永远早退（门内待重建
    // 状态泄漏到下一场景，option 树再也不建）。同理复位隐藏位，避免
    // 0x14698 bit3 的移植对应物跨场景残留。
    option_rebuild_pending_ = false;
    options_hidden_ = false;
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
    if (anm_random_observer)
        anm_random_observer(*vm, -10, script, i32(ordering), u32(bank), 0, 0, 0, false);
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

    if (!sync_player_options(battle)) return false;

    // 0040b1b0：DAT_004c22c4 + 0x10（pl00.anm）script18；
    // 0040b2a0：label1 → 子script21/23/25与19爆炸，不是effect.anm18。
    // 伤害/运动/爆炸触发全归 battle，表现层只响应原状态边沿。
    for (size_t i = 0; i < bomb_orb_handles_.size(); ++i) {
        const auto& orb = battle.bomb_orbs[i];
        if (battle.player_character() != 0 || (!orb.active && !orb.exploding)) {
            if (!end_player_effect(bomb_orb_handles_[i], true)) return false;
            bomb_orb_exploding_[i] = false;
            continue;
        }
        if (!animations.registry.find(bomb_orb_handles_[i]) && !orb.exploding) {
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

bool StagePresentation::hide_player_options() {
    // 0x42b6d0（0x42cd40 关终调用）：capstone 0x42b6e0 or [eax+0x14698],8 置
    // 自机隐藏位；0x42b72f/0x42b753/0x42b78e/0x42b7b2 mov [vm+0x4a0],3 对
    // 8 个槽的两个根 VM（0xa368 普通树 / 0xa36c 满P树）及其直接子体写
    // pending=3；0x42b7d4 mov [edx+0x1469c],0 清隐藏计数；全程无 0x462450
    // 调用，故移植侧用 deferred interrupt(handle,3,false)（句柄保留）。
    // pl03 script15/17 无 label3、无 id=-1 回落：下一主链 tick 的
    // pending_interrupt 分支走 decrement/advance/tick 单帧，计时器不前进，
    // 本应发生的 op500 spawn 顺延一帧。
    //
    // 帧位代理（反证 correctedPlan 更正）：原版在关终帧 input 12184 执行
    // 0x42b6d0；本 port 的 0x42cd40 挂在 RunScene/PostGameFlow（非本写域），
    // 等价地在 prepare_next_stage 的 input 12185 下发。该一帧代理成立的
    // 前提是生产 S1 时间线上 pl03 script15 根在 12185 恰处 spawn 相位
    //（tick 起点 timer=4，op200 [12,0] t=4 到期，跳转目标 time=0 后 op500
    // 立即 spawn；hide 使该 tick 不执行指令、timer 停在 4，出生顺延到
    // 12186）。该相位来自 pre-B1 shadow 捕获，生产 replay 未在本轨重测
    //（wasm/浏览器验收 deferred 到主会话）。tests/ui/option_hidden_lifecycle_test.cpp
    // 的 frame0 无绘制 / frame1 首绘断言验证的是 fresh-root
    // create-then-2-frames 的合成相位（create 后固定推进两帧到 timer 4），
    // 不是生产 S1 时间线 input 12185 的相位；它只能防止模板/创建语义漂移，
    // 不能证明代理帧位与生产时间线一致。若生产相位偏移，必须由主会话的
    // wasm 对拍暴露，不得用改测试期望的方式掩盖。
    //
    // 隐藏位：0x42b6d0 同时置 player+0x14698 bit3。移植侧置 options_hidden_，
    // 使 12185..12213 的 sync_player_options 走 0x442960 隐藏支路（把槽
    // 位置钉在 player+0x5c4/+0x5c8 并跳过 per-slot 更新；见
    // sync_player_options），而不是继续按门内已过时的 battle.options[i].x/y
    // 写根 translation。
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
    // 0x442960 隐藏支路（capstone：0x442f2b test bit3；0x442f34 inc
    // [edi+0x1469c]；0x443017 cmp [edi+0x1469c],0x1e；0x44301e jl；
    // 0x44302e mov dword ptr [ecx],0 清槽 active 标志 player+0xa2b8；
    // 0x443059/0x443089/0x4430c9/0x4430f9 mov [vm+0x4a0],1 对两个根置
    // pending=1；0x443114 继续下一槽而非退出）。全程无 0x462450 调用，
    // 用 deferred interrupt(handle,1,false)；registry.interrupt 同时把
    // label1 传播到直接子体。
    // 原版时序：门内第 30 帧（计数 29，input 12214）触发；最后一次卫星
    // 出生是 input 12210 且已绘制，12214 的出生被本中断拦下，根按
    // script15 label1(t=4) → op1(t=8) 于 input 12218 由主链回收。
    // 反证更正：原版清的是槽 active 标志 player+0xa2b8（decomp 00442960.c
    // :349-350 的 *piVar8=0，piVar8=player+0xa2b8），根句柄 0xa368/0xa36c
    // 仍保留；移植侧按 correctedPlan 同时清 option_handles_/
    // full_option_handles_ 并把 option 状态复位为“未建树”，作为“槽已不
    // 活跃”的等价簿记：门内 sync 不重建，S2 的 0x444df0 公共循环重建。
    // 幂等：0x442960 每帧 counter>0x1d 都会重入；句柄清零后重复调用是空操作。
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

bool StagePresentation::sync_player_options(const StageBattle& battle, bool power_rebuild) {
    // 门内隐藏旗标期：0x442960 已把 option 槽置为不活跃（原版清
    // player+0xa2b8；移植侧等价清句柄并置 pending label1），旧树退场中；
    // 0x444df0 只在 S2 起始（0x441b00 label2 激活后）重建，门内不得按
    // missing 重建（否则会在装载门里重新长出卫星并继续抽视觉 RNG）。
    if (option_rebuild_pending_) return true;
    // 0x442960 隐藏支路（player+0x14698 bit3）：decomp 00442960.c:345-348
    // 把槽位置 piVar13[-3]/[-2] 直接写成 player+0x5c4/+0x5c8；capstone
    // 0x442f5d test bit3 / jne 0x443005，0x443005 mov edx,[edi+0x5c4]，
    // 0x44300b mov [esi-0xc],edx，0x44300e/0x443014 同式写 y。槽位置即
    // 根 VM translation 的来源。门内 StageBattle::update_options 因
    // option_movement_tick_=false（StageBattle.cpp:2070/2073 的
    // step_transition）跳过 per-slot 位置计算，battle.options[i].x/y 仍是
    // 换关前的过时值；若继续走 visible 分支，卫星会画在旧位置而不是自机
    // 位置。故隐藏期只把两组根钉到 battle.player_x/player_y（port 的
    // player+0x5c4/+0x5c8 对应字段，定点×1/128 的浮点无损表示），并跳过
    // 下面的 per-slot create/retire/anchor 更新。
    if (options_hidden_) {
        const Vec3 anchor{battle.player_x, battle.player_y, 0};
        for (u32 handle : option_handles_)
            if (auto* vm = animations.registry.find(handle))
                vm->visual.translation = anchor;
        for (u32 handle : full_option_handles_)
            if (auto* vm = animations.registry.find(handle))
                vm->visual.translation = anchor;
        return true;
    }
    const bool visible = battle.player_visible();
    const bool defer_anchor = power_rebuild || option_anchor_deferred_;
    // 00444df0 模型（反证 correctedPlan 更正，原报告把 S2 重建误写成满P分支）：
    //   * power<max（妖梦 S2 实测 power=293 < max=400）：0x444e52 对
    //     0xa36c 满P树调 0x46fc30(handle,1)（= end_player_effect(handle,false)
    //     的 deferred label1），再由公共 0x444f74 循环 retire 旧 0xa368 普通
    //     根（render_flags|=0x20）并 create 新普通根；
    //   * power>=max 才走 0x11/0x14/0x15（=17/20/21）满P树的 retire+create
    //     分支，即下方 full 首循环的 full_scripts。
    // 普通树脚本 = 0x10/0x12/0x13/0x0f（16/18/19/15）；满P树 = 0x11/0x14/
    // 0x15（17/20/21，妖梦满P同为17）。00442960 已计算完的定点卫星位置由
    // battle.options 唯一提供，此处不再维护独立跟随轨迹，也不在暂停/展示帧
    // 推进卫星物理。
    constexpr i32 option_scripts[4] = {16, 18, 19, 15};
    constexpr i32 full_scripts[4] = {17, 20, 21, 17};
    const i32 character = battle.player_character();
    const i32 count = visible ? battle.option_count : 0;
    const bool full = visible && battle.full_power();
    if (character < 0 || character > 3 || count < 0 || count > 8)
        return fail("Player option state outside original bounds");
    // power<max：满P树 0x46fc30(handle,1)（visible&&!full 时 end_player_effect
    // 的 retire=false 分支），普通根 retire 后由下方公共循环重建。
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
    // 444df0 creates the full-power trees in a separate first loop.
    if (full) for (size_t i = 0; i < full_option_handles_.size(); ++i) {
        const auto& option = battle.options[i];
        if (i >= size_t(count) || !option.active) continue;
        const Vec3 anchor{option.x, option.y, 0};
        if (!animations.registry.find(full_option_handles_[i])) {
            full_option_handles_[i] = create_player_effect(
                banks.player, full_scripts[character], 12, 2,
                power_rebuild ? Vec3{} : anchor);
            if (!full_option_handles_[i]) return false;
            if (power_rebuild)
                animations.registry.find(full_option_handles_[i])->visual.translation = {0, -32, 0};
        }
        if (!defer_anchor) animations.registry.find(full_option_handles_[i])->visual.translation = anchor;
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
                banks.player, option_scripts[character], 12, 0,
                power_rebuild ? Vec3{} : anchor);
            if (!option_handles_[i]) return false;
            if (power_rebuild)
                animations.registry.find(option_handles_[i])->visual.translation = {0, -32, 0};
        }
        if (!defer_anchor) animations.registry.find(option_handles_[i])->visual.translation = anchor;
    }
    option_anchor_deferred_ = power_rebuild;
    return true;
}

bool StagePresentation::reimu_bomb_orb_birth(const StageBattle& battle, i32 index) {
    if (index < 0 || index >= i32(bomb_orb_handles_.size()))
        return fail("Reimu bomb orb index outside pool");
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
        if (stale && !animations.release_tree(*stale))
            return fail("Player shot animation release failed");
        shot_handles_[index] = 0;
    }
    const auto* source = animations.script_template(banks.player, u32(shot.script));
    if (!source) return fail("Player shot animation template unavailable");
    // 0x445e39 0x4706c0 池分配 → 0x445e47 0x403800 模板拷贝 → 0x445e4e
    // render_flags|=0x200 → 0x445e6a 平移清零 → 0x445e7e 0x462450 首 tick →
    // 0x445e87 0x46f3f0 主链尾插（flags=0，非 0x46f4a0 头插）。
    auto* vm = animations.registry.allocate();
    if (!animations.bind_template(*vm, banks.player, shot.script)) {
        animations.registry.discard(*vm);
        return fail("Player shot animation template unavailable");
    }
    if (anm_random_observer)
        anm_random_observer(*vm, -11, shot.script, 0, u32(banks.player), 0, 0, 0, false);
    // 445e4d..445e83 copies the negative-time template and initializes at0
    // with zero translation/rotation. 4464d0 writes the live projectile pose
    // later, before the same frame's separate manager tick (46fb51).
    vm->instruction_offset = source->instruction_offset;
    vm->visual.render_flags |= 0x200u;
    vm->visual.translation = {};
    vm->variables.rotation.z = 0;
    // 暂停冻结：主链在暂停帧仍会遍历（Application Update 34 不因 pause 停），
    // 给自机弹 VM 挂与自机特效相同的 pre_tick（environment.paused → 1，整帧
    // 跳过、不推进时间/RNG），保持改动前 sync 早退时的冻结行为。
    vm->pre_tick = pause_player_effect;
    if (!animations.prime_instance(*vm))
        return fail("Player shot animation initialization: " + animations.error);
    vm->ordering = 0;
    const u32 handle = animations.registry.submit(*vm, 0);
    if (!handle) {
        animations.registry.destroy_tree(*vm);
        return fail("Player shot animation registration: " + animations.error);
    }
    shot_handles_[index] = handle;
    shot_generations_[index] = shot.generation;
    // 0x445eb2 0x46fb90 查句柄 → 0x445ec0-0x445ed0：仅当脚本 op424 置
    // render_flags bit6 时写 visual.flags|=4 与 VM+0x50=弹角；bit6 未置的
    // 脚本（pl03 script11/12 无 op424）保留 op401 的脚本旋转值。
    if (vm->visual.render_flags & 0x40u) {
        vm->visual.flags |= 4u;
        vm->variables.rotation.z = shot.angle;
    }
    // 0x445c40.c:129-131：出生尾段把弹对象当前位姿写回 VM+0x574/578/57c
    //（x/y/z）。生产路径同帧 sync 会在链 tick 前重写一次，但直接调用本函数
    // 的路径（测试/诊断）也必须看到出生位而非零平移。
    vm->visual.translation = {shot.x + kGameToScreenX, shot.y + kGameToScreenY, 0.0f};
    return true;
}

// 主链 0x46f170 遍历完成后的收尾（挂 AnmManager::after_main_update）：
//   * 自机弹四角快照：原版 0x4464d0 在写当前位姿（LAB_00446619）之前先用
//     VM 四角剔除（0x446531），即消费上一帧 0x46f170 遍历后的 VM 状态；
//     这里在链遍历之后按同一状态落快照，供下一帧 battle.step 的
//     consume_player_shot_geometry（0x4464d0 镜像）使用。
//   * 自灭检出：0x4464d0 的 0x46fb90 查无 VM → LAB_00446531 退休该弹。
//   * 留尸出池：op1 自灭后 VM 已被链回收（find 失败）。
// 暂停/覆盖层等 sync 早退的帧（RunScene 在 battle.step/sync 之前返回）：
// frame_synced_ 为 false，钩子跳过，避免用上一帧的 battle_/shot_handles_
// 做决定。转场路径仍会调用 sync（step_transition → sync），钩子正常执行。
bool StagePresentation::post_chain_update() {
    // 原版物品内嵌 VM 不在 0x46f170 链内，层桶重建（主链 update 开头清空）
    // 之后必须单独补入，才能在 AnmDrawSchedule 的层 0 绘制。
    append_item_draws();
    if (!battle_) return true;
    if (!frame_synced_) return true;
    if (!error_.empty()) return true;
    frame_synced_ = false;  // 每帧只消费一次（同一帧重复链 update 不重复落盘）
    for (size_t i = 0; i < shot_handles_.size() && i < battle_->shots.size(); ++i) {
        auto& shot = battle_->shots[i];
        if (!shot.alive) continue;
        if (shot_generations_[i] != shot.generation) continue;  // 槽位映射未对齐
        const u32 handle = shot_handles_[i];
        if (!handle) continue;
        AnmVm* vm = animations.registry.find(handle);
        if (!vm) {
            // 0x4464d0 的 0x46fb90 查无 VM：下一帧该弹退休（伤害登记同帧退休）。
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
        battle_->set_player_shot_geometry(i32(i), shot.age, shot.script, shot.generation,
                                          snapshot);
    }
    for (size_t n = 0; n < shot_fades_.size();) {
        if (!animations.registry.find(shot_fades_[n].handle))
            shot_fades_.erase(shot_fades_.begin() + i32(n));
        else ++n;
    }
    return true;
}

bool StagePresentation::initialize_player_effects(const StageBattle& battle) {
    // Keep this narrowly scoped to the player VM family. Projectile and enemy
    // presentation is still created by sync() after the first simulation step.
    return sync_player_effects(battle);
}

bool StagePresentation::activate_player_effects(const StageBattle& battle) {
    // 00441b00 broadcasts label2 to every option root and child. In particular
    // pl03 script15's script16 child consumes the four RNG reads at this
    // deferred manager tick; do not call tick_instance directly (that bypasses
    // the native interrupt path and gives other characters a false extra tick).
    for (size_t i = 0; i < option_handles_.size(); ++i) {
        for (u32 handle : {option_handles_[i], full_option_handles_[i]}) {
            if (!animations.registry.find(handle)) continue;
            if (!animations.interrupt(handle, 2, false)) return fail(animations.error);
        }
    }
    // 424d0 broadcasts to the existing tree before 444df0 rebuilds it.
    // 门内第 29 帧 interrupt_player_options 已清空两组句柄：旧树（根与
    // 直接子体）不再收到 0x441b00 的 label2，基线里旧卫星被 label2 重放
    // 抽取的 +24 因此消失。0x441b00 先于 0x444df0，旗标必须在 label2
    // 广播之后、重建（initialize_player_effects → sync_player_options）
    // 之前清除，使重建树不接收本帧 label2。
    option_rebuild_pending_ = false;
    // 0x441b00 同时 and [player+0x14698],~8 清隐藏位；重建树必须回到
    // visible 分支（否则 S2 的 sync 仍走隐藏支路，只钉位置不建树）。
    options_hidden_ = false;
    return initialize_player_effects(battle);
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

// 0x438730 收集瞬间的 front.anm 拾取动画。原版在 0x46f820 调用点用
// flags=0（0x46f3f0 主链尾插）、layer=-1、anmArg=0、位置=神霊位，并丢弃
// 返回句柄（ESI=0）；脚本自灭，链扫描回收。创建发生在神霊管理器更新内
//（port：battle.step → update_item_side，优先级 20），早于主链 walk。
bool StagePresentation::spirit_pickup(i32 script, const Vec3& position) {
    if (!animations.create(banks.front, script, -1, 0, position, 0, false))
        return fail("Spirit pickup animation creation failed: " + animations.error);
    return true;
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
            // 0x438ec0:81/85 建神霊 VM 走 0x46f820 flags=0 → 0x46f3f0 主链
            // 尾插（非 0x46f4a0 头插）。神霊高频尾插使取消弹等链尾父 VM 之后
            // 始终有后继节点，0x46f170 保存 next 的遍历才会到达本帧尾插的子
            // VM（Reimu S1 2406 首差的链形态依据）。
            handle = animations.create(banks.spirit, script, -1, 0,
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
            banks.player, spawn.script, -1, 0, {0.0f, spawn.y, 0.0f}, 0, false);
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
                                  alive ? vm->variables.rotation.z : 0.0f,
                                  battle.sim.time_rate());
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
    // 原版证据（phase3 定案）：
    //  * 物品视觉 = 物品结构体内嵌两枚 AnmVm（item+0x10 / item+0x5b4）。
    //    出生 0x42f020:60-63 连续两次 0x403800 模板拷贝（bullet.anm，
    //    DAT_004c2174+0x9752b4）+ 0x462450 首拍；此后每帧由物品管理器
    //    0x42e380 直接 0x462450 步进（:87 出生分支、:318 主项、:324 副项）。
    //  * 这两枚 VM **从未链入 0x46f170**：主链链掘（native chain-dump
    //    10202/10219/10222）中 bank5 属 front.anm，89-107 家族仅 10-11 个
    //    且在飞（timer 负值）= 0x438730 收集瞬间创建的 front 拾取动画
    //    （0x60/0x61/0x66/0x67 = 96/97/102/103），不是道具本体。
    //  * 因此 port 不得把道具 VM 注册进 registry 链（旧实现每件 2 个
    //    registry VM，10213 一帧新增 642 个主链成员，改变取消弹父节点的
    //    链尾可达性 → 妖梦 10222 / 早苗 11277 首差）。
    // 本实现：自持 VM（不入链）、每帧在 sync_items 手动首拍/步进，位置
    // 跟随 item.x/y/z；主链 update 完成后 append_item_draws() 把它们放进
    // 绘制层桶（层 0）。脚本无 RNG 操作（89-107 实测 0 条 117/118/119/
    // 120/121/122/131/301），故 tick 相位不影响 RNG。
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
            if (!animations.bind_template(*vm, banks.bullet, script))
                return fail("Item animation template unavailable: " + animations.error);
            vm->visual.layer = 0;
            vm->visual.render_flags |= 0x200u | 0x40000u;
            if (vm->tick(random, animations.rate) < 0)
                return fail("Item animation initialization: " + vm->error);
        }
        if (!side) {
            side = std::make_unique<AnmVm>();
            side->environment = &animations.scene_environment();
            side->object_host = &animations;
            if (!animations.bind_template(*side, banks.bullet, side_script))
                return fail("Item side animation template unavailable: " +
                            animations.error);
            side->visual.layer = 0;
            side->visual.render_flags |= 0x200u | 0x40000u;
            if (side->tick(random, animations.rate) < 0)
                return fail("Item side animation initialization: " + side->error);
        }
        vm->visual.translation = {item.x, item.y, item.z};
        side->visual.translation = {item.x, item.y, item.z};
        if (vm->tick(random, animations.rate) < 0)
            return fail("Item animation: " + vm->error);
        if (side->tick(random, animations.rate) < 0)
            return fail("Item side animation: " + side->error);
    }
    return true;
}

// 主链 update 重建层桶之后调用（post_chain_update 顶部）：把自持道具 VM
// 放入绘制层桶，仅参与绘制，不参与任何链遍历（与原版内嵌 VM 同构）。
void StagePresentation::append_item_draws() {
    for (auto& vm : item_vms_)
        if (vm) animations.registry.append_draw(*vm);
    for (auto& vm : item_side_vms_)
        if (vm) animations.registry.append_draw(*vm);
}

bool StagePresentation::graze_effect(const Vec3& position) {
    // 0x445900（caller 0x445958）: FUN_0046f820(out, 0x64, pos, 0.0, -1, 0)
    // → flags=0 → 0x46f3f0 主链尾插；effect.anm script 100；layer=-1（脚本
    // 自定）、anmArg=0。0x460c50 音效由 StageBattle 侧保留。
    const u32 handle = animations.create(banks.effect, 100, -1, 0, position,
                                         0.0f, false);
    if (!handle) return fail("Graze effect creation failed: " + animations.error);
    return true;
}

bool StagePresentation::create_ecl_effect(i32 file, i32 script, const Vec3& position) {
    // kind=0 特效创建体（flush_ecl_animations 同款）：layer=-1（脚本自定
    // 层）、anmArg=0；0x4243e0 语义为模板拷贝→立即 tick，AnmManager::create
    // 自带首 tick。
    const auto target = resolve_script_target(file, script);
    if (target.script < 0) return false;
    const u32 effect_handle =
        animations.create(target.bank, target.script, -1, 2, position, 0, false);
    if (!effect_handle)
        return fail("Effect animation creation failed: " + animations.error);
    if (anm_random_observer)
        if (auto* vm = animations.registry.find(effect_handle))
            anm_random_observer(*vm, -12, file, script, i32(target.bank), 0, 0, 0, false);
    create_color_step(animations.registry.find(effect_handle));
    return true;
}

bool StagePresentation::flush_ecl_animations(StageBattle& battle, StageEnemy* constructing,
                                             bool slots_only) {
    // ---- ECL 30x ANM 创建（0x4243e0：模板拷贝→立即 tick→主池头入链。
    // kind=0 特效路径 layer=-1（脚本自定层）、anmArg=0、位置清零（307 的
    // 敌机位仅特效路径用）；kind=1 槽位路径见下方 [+0x278]+7 注。
    // 文件索引：0=bullet、1=effect、2..=stage 合并表（脚本号文件内局部——
    // 文件 3+ 的合并偏移为已知局限，窗口内敌机均用文件 2）。
    // 交互模式 307/308 已在指令处经 ecl_effect_create 同步创建（round-s15），
    // 此处 kind=0 仅服务无宿主入队与激光/StageBattle 直入队请求。
    // slots_only：kind=0 原样留在队列（顺序保持），kind=1 立即建 VM。
    std::vector<AnmEffectRequest> pending;
    if (slots_only) pending.reserve(battle.sim.anm_effects.size());
    for (const auto& request : battle.sim.anm_effects) {
        if (request.kind == 0) {
            if (slots_only) { pending.push_back(request); continue; }
            if (!create_ecl_effect(request.file, request.script,
                                   {request.x, request.y, request.z}))
                return false;
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
    // 失败路径保持队列原样（错误已是终态，重试会重复建 VM）。
    battle.sim.anm_effects.swap(pending);
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
            // mode12 strip and screen coordinates. 0x43cac0 replaces only the
            // draw-mode field (mask 0xd9ffffff | 0x18000000); 4159c0 then
            // clears VM+0x18 blend bits 0x1e0 (script82 op303 wrote 3 =
            // One/Zero) and render_flags 0x40000, leaving blend 0.
            handles[column]=animations.create(0,82,29,0);
            auto* vm=animations.registry.find(handles[column]);
            if(!vm)return fail("Enemy fog render-target animation: "+animations.error);
            vm->visual.flags=(vm->visual.flags&~0x3e000000u)|(12u<<25);
            vm->visual.flags&=~0x1e0u;
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
    // 主链 update 后的快照钩子（post_chain_update）读取本帧 battle 的弹体
    // 槽位；battle 生命周期长于本对象（RunScene 成员序），release 时置空。
    // frame_synced_ 只在完整走完本帧后置位：暂停/覆盖层等早退帧让钩子
    // 跳过，避免用上一帧的 battle_/shot_handles_ 做决定。
    battle_ = &battle;
    frame_synced_ = false;
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
        slot.vm->variables.rotation.z = bullet.angle + kBulletSpriteForward;
        if (slot.vm->tick(random, animations.rate) < 0) return fail("Bullet animation: " + slot.vm->error);
    }
    for (size_t i = 0; i < bullets_.size(); ++i)
        if (!matched[i]) bullets_[i].active = false;

    }

    // ---- 自机弹：.sht 弹型脚本（plXX.anm，旋转模式 1 随弹向）；无 .sht
    // 会话回退精灵直贴（player_shot_sprite_） ----
    if (shot_handles_.size() < battle.shots.size())
        shot_handles_.resize(battle.shots.size(), 0);
    if (shots_.size() < shot_handles_.size()) shots_.resize(shot_handles_.size());
    shot_generations_.resize(shot_handles_.size());
    for (size_t i = 0; i < shot_handles_.size(); ++i) {
        if (i < battle.shots.size() && battle.shots[i].alive) {
            auto& shot = battle.shots[i];
            // Compaction changes slots, not ANM identity. Preserve a survivor's
            // VM; a new shot binds fresh even if its script matches the old one.
            bool fresh_slot = false;
            if (shot_generations_[i] != shot.generation) {
                size_t source = i + 1;
                while (source < shot_handles_.size() &&
                       shot_generations_[source] != shot.generation) ++source;
                if (source < shot_handles_.size()) {
                    std::swap(shot_handles_[i], shot_handles_[source]);
                    std::swap(shots_[i], shots_[source]);
                    std::swap(shot_generations_[i], shot_generations_[source]);
                } else {
                    if (u32 stale = shot_handles_[i]) {
                        if (auto* old = animations.registry.find(stale))
                            if (!animations.release_tree(*old))
                                return fail("Player shot animation release failed");
                    }
                    shot_handles_[i] = 0;
                    if (shots_[i]) {
                        if (shots_[i]->resource) animations.release_tree(*shots_[i]);
                        shots_[i].reset();
                    }
                    fresh_slot = true;  // generation written after the VM exists
                }
            }
            const i32 script = shot.script;
            AnmVm* vm = nullptr;
            if (script >= 0) {
                vm = animations.registry.find(shot_handles_[i]);
                if (!vm) {
                    if (!fresh_slot && shot_generations_[i] == shot.generation) {
                        // 本代 VM 已被主链遍历回收（脚本 op1 自灭）：原版
                        // 0x4464d0 下一趟 0x46fb90 查无 VM 即退休该弹
                        //（LAB_00446531）；不得复活。
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
                    vm->source_script = -1;  // 回退精灵无脚本身份（重绑判定用）
                    if (!vm->select_sprite(player_shot_sprite_))
                        return fail("Player shot sprite unavailable");
                    vm->visible = true;
                    vm->visual.flags |= 1;
                    vm->visual.render_flags |= 0x200;
                    vm->visual.set_layer(11);
                }
                shot_generations_[i] = shot.generation;
            }
            if (shot.state == 2 && shot_faded_generations_.insert(shot.generation).second)
                vm->pending_interrupt = 1; // 446bf0, once per hit generation.
            if (shot.behavior == 2 && shot.state == 1 && shot.beam_interrupt) {
                vm->pending_interrupt = shot.beam_interrupt;
                shot.beam_interrupt = 0;
            }
            vm->visual.translation = {shot.x + kGameToScreenX, shot.y + kGameToScreenY,
                                      shot.state == 2 && !shot.sine_moving ? 0.1f : 0.0f};
            // 0x446670：仅当脚本 op424 置 render_flags bit6 时写角度与
            // visual.flags|=4；pl03 script11/12（无 op424）保留脚本 op401
            // 的旋转值（raw pl03.anm：script11/12 op401 z=-π/2）。
            if (vm->visual.render_flags & 0x40u) {
                vm->visual.flags |= 4u;
                vm->variables.rotation.z = shot.angle;
            }
            if (shot.behavior == 2 && shot.state == 1) {
                // 446f20 writes VM+80 length and +78 scale each frame.
                vm->visual.scale.x = shot.beam_visual_width / 512.f;
                vm->visual.secondary_scale.x = shot.beam_visual_width;
                vm->visual.flags |= 0x18u;
            }
            // 0x4464d0 只写位姿（LAB_00446619）；VM tick 由同一帧稍后的
            // 0x46f170 主链遍历统一执行（AnmManager::update(false)，Update 34）。
            // 四角快照取 post-tick 状态，在 post_chain_update 落盘。
        } else {
            // 0x4464d0 只有两种死法：出界（上一帧四角全离开 [128,512]×
            // [16,464] 且快照龄>9、类型≠2 —— consume_player_shot_geometry
            // 已在 step_shots 内镜像同一谓词先行剔除）与命中（sim.step_frame
            // 的 0x446870 → 伤害记录回调 0x446bf0）。此处用同一 VM 四角复算
            // 同一谓词区分死因：VM 自上帧快照后未再推进（tick 属主链遍历，
            // 快照取 post-tick 状态），四角与快照逐位相等，判定与 battle 侧
            // consume 完全同数据同条件，无启发式。
            const i32 corpse_script = i < battle.shots.size() ? battle.shots[i].script : -1;
            AnmVm* vm = corpse_script >= 0
                            ? animations.registry.find(shot_handles_[i])
                            : (shots_[i] ? shots_[i].get() : nullptr);
            bool fade = false;
            PlayerShot corpse_shot{};
            if (i < battle.shots.size() && !battle.shots[i].alive) {
                const PlayerShot& shot = battle.shots[i];
                corpse_shot = shot;
                const bool snapshotted = shot_generations_[i] == shot.generation && vm &&
                                         vm->source_script == shot.script && shot.type != 2;
                bool outside = false;
                if (snapshotted && shot.age - 1 > 9) {  // consume 的快照龄门
                    Vec3 corners[4];
                    if (!anm_quad_positions(*vm, corners))
                        return fail("Player shot death geometry unavailable");
                    outside = true;
                    for (const auto& corner : corners)
                        outside = outside && (corner.x <= 128.0f || corner.x >= 512.0f ||
                                              corner.y <= 16.0f || corner.y >= 464.0f);
                }
                fade = !outside && vm && shot.script >= 0 &&
                       shot_faded_generations_.find(shot.generation) ==
                           shot_faded_generations_.end() &&
                       shot_script_has_interrupt(*player_resource, u32(shot.script), 1);
            }
            if (fade) {
                // 0x446bf0：VM+0x4a0=1（中断段=脚本后半 layer13 渐隐）；弹转
                // dying：速度×0.125、z=0x3dcccccd(0.1)、VM 位姿回写死亡位
                //（battle.step_shots 的本帧整速位移已在命中前完成，不再补动）。
                // VM 保持主链成员身份：中断段由本帧稍后的链遍历 tick，脚本
                // op1 自灭后由 registry 回收（句柄失效即出留尸池）。
                ShotFade corpse;
                corpse.generation = corpse_shot.generation;
                corpse.handle = shot_handles_[i];
                corpse.x = corpse_shot.x;
                corpse.y = corpse_shot.y;
                corpse.angle = corpse_shot.angle;
                corpse.drift = corpse_shot.speed * 0.125f;
                corpse.sine_moving = corpse_shot.sine_moving;
                corpse.sine_motion = corpse_shot.sine_motion;
                vm->pending_interrupt = 1;  // 0x446bf0: VM+0x4a0 = 1
                vm->visual.translation = {corpse.x + kGameToScreenX,
                                          corpse.y + kGameToScreenY,
                                          corpse.sine_moving ? 0.0f : 0.1f};
                vm->variables.rotation.z = corpse.angle;
                shot_faded_generations_.insert(corpse.generation);
                shot_fades_.push_back(corpse);
                shot_handles_[i] = 0;
                shot_generations_[i] = 0;
                if (shots_[i]) shots_[i].reset();
            } else {
                // 出界死（0x470110：render_flags|=0x20 交本帧链遍历回收）或
                // 脚本已自灭（句柄失效）：清槽。
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

    // ---- 命中留尸推进（0x4464d0 dying 惰行 + VM 跟随 → op1 自灭回收）----
    // 原版 dying 弹仍走 0x4464d0：位移（减速率 0.125 后的 speed）、0x473780
    // 0.01 量化、VM+0x574 位姿回写、flags&0x40 时角度回写；随后由 0x46f170
    // 主链遍历推进。脚本 op1 自灭后 VM 被链回收——对应原版次帧 0x46fb90
    // 查无 VM 的槽位释放路径（视觉自灭帧即不可见，回收无观察差）。
    for (size_t n = 0; n < shot_fades_.size();) {
        ShotFade& corpse = shot_fades_[n];
        AnmVm* vm = animations.registry.find(corpse.handle);
        if (!vm) {  // op1 自灭（链遍历已回收）：出池
            shot_fades_.erase(shot_fades_.begin() + i32(n));
            continue;
        }
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
            vm->visual.translation = {corpse.x + kGameToScreenX,
                                      corpse.y + kGameToScreenY,
                                      corpse.sine_moving ? 0.0f : 0.1f};
            vm->variables.rotation.z = corpse.angle;
        }
        ++corpse.age;
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
        // 原生 00443240.c:242-243 在 phase_frame 递增前比较
        // (phase_frame != prev && phase_frame % 3 == 0)；port 的 sync 在
        // battle.step 之后，player_phase_frame 已比原生多 1，故用
        // pre-increment 快照（phase_frame_previous）判定，蓝帧恢复
        // 0,3,6,9,12,15（web 旧值 2,5,8,11,14 早一帧）。
        player_->visual.flags &= ~0x60000u;
        const bool phase_advanced =
            battle.player_phase_frame != battle.player_phase_frame_previous;
        if (battle.invulnerable > 0 && phase_advanced &&
            battle.player_phase_frame_previous % 3 == 0) {
            player_->visual.secondary_color = 0xff0000ffu;
            player_->visual.flags |= 0x20000u;
        }
    } else if (player_) {
        animations.release_tree(*player_);
        player_.reset();
        player_tilt_ = 0;
        player_script_ = -1;
    }
    // 本帧 sync 完整走完：主链 update(false) 之后的收尾钩子可以消费本帧
    // 的 battle_/shot_handles_（暂停/覆盖层/转场早退帧保持 false）。
    frame_synced_ = true;
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
                  shot_handles_.size(), player_ ? 1 : 0,
                  size_t(std::count_if(spirit_handles_.begin(), spirit_handles_.end(),
                                       [this](u32 handle) {
                                           return handle && animations.registry.find(handle);
                                       })),
                  size_t(std::count_if(item_vms_.begin(), item_vms_.end(),
                                       [](const std::unique_ptr<AnmVm>& vm) {
                                           return vm != nullptr;
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
    if (!shot_handles_.empty() && shot_handles_[0])
        if (auto* vm = animations.registry.find(shot_handles_[0]))
            describe("shot", *vm, player_shot_sprite_, -1, 0);
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
        // 仅无 .sht 会话的回退精灵直贴 VM（script<0，不入 registry）。
        // 已注册的自机弹/留尸 VM 由 AnmDrawSchedule 层 11（优先级 23）与
        // 层 13（优先级 27）生产绘制——此处再画会重复提交。
        for (auto& shot : shots_)
            if (shot) one(*shot);
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
