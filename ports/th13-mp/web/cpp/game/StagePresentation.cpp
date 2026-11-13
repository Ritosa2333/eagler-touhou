// StagePresentation 实现。锚点注释对应 docs/plans/anm-pipeline.md 与
// BulletAppearance.hpp（0x4120b0 / 0x4bbe30 表）。
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
    // 0x461cb0: render_flags = (flags & ~0x4000) | 0x8000 before the first
    // tick, then if (flags & 0xc000) == 0x8000 convert to
    // (flags & ~0x8000) | 0x4000. 0x40e840:194/0x40f030:502 both use this
    // sequence (0x405010 initialises 0x4000, 0x461cb0 re-runs it per rebind).
    // Bit14 is the 0x462450:301-308 pause gate for manually ticked VMs.
    vm.visual.render_flags = (vm.visual.render_flags & ~0x4000u) | 0x8000u;
    if (vm.tick(random, animations.rate) < 0) return fail(vm.error);
    if ((vm.visual.render_flags & 0xc000u) == 0x8000u)
        vm.visual.render_flags = (vm.visual.render_flags & ~0x8000u) | 0x4000u;
    vm.visual.render_flags |= 0x40000;  // 0x40e840:189 bullet+0x44 layer bit
    vm.pending_interrupt = interrupt;
    // 0x405010/0x40e840 leave the embedded VM translation (+0x574) at zero;
    // the live position is written to vm+0x3c/0x40/0x44 by 0x40e4d0 at draw
    // time (see tick_bullet). A reused slot must not keep an old translation.
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
    // 原版 0x40e840/0x40db10 出生与逐帧更新都不写 VM 位置/旋转：位置与
    // op424 朝向由绘制函数 0x40e4d0 在 tick 之后写入。此处紧随 tick 写，
    // 对渲染与下一帧脚本读取（10013-10015）等价（同帧内无第二次 tick）。
    const int result = vm.tick(random, animations.rate);
    if (result < 0) return fail("Bullet animation: " + vm.error);
    // 0x40e4d0:8-17：把子弹游戏位置写到 VM+0x3c/0x40/0x44（变量 10013-
    // 10015，覆盖脚本 op400/407 的位置），+0x574 平移保持 0。此前 port 写
    // translation 而让脚本位置叠加，渲染位置 = 弹位 + 脚本偏移（命中框
    // 与贴图错位）。
    vm.variables.position = {bullet.x, bullet.y, bullet.z};
    // 0x40e4d0:18-23 / 0x40e630:16-21：render_flags bit6（0x40，op424 写）
    // 置位时，绘制把 rotation.z 覆盖为 0x472e90(弹体+0xb8c, π/2) =
    // wrap(angle + π/2)，并置 render_flags bit2（0x4，旋转序位）。
    // bullet.anm 主体脚本 38/43..50 用 op424(1)；缺此写入时 30x30/32x32
    // 弹体贴图恒为轴对齐（原生 draw 实测 edge_deg = move_deg + 90°：
    // artifacts/round-19/gate-death-final2/native/battle-draws.jsonl
    // f1762-1766，bullet2.png）。
    if (vm.visual.render_flags & 0x40u) {
        vm.variables.rotation.z = wrap_angle(float(bullet.angle + 1.5707964f));
        // 0x40e544 `orl $0x4,0x5e4(%esi)`：0x5e4 = 内嵌 VM+0x18 =
        // visual.flags（旋转脏位，0x462450 case0x192/0x1ad 同义），
        // 不是 render_flags（+0x1c=0x5e8，bit6 是 op424 的旋转使能）。
        vm.visual.flags |= 0x4u;
    }
    // 0x40e4d0:24-29：bullet+0x20 bit6（etEx op 0x400000 置位）时绘制把
    // VM 次级缩放（+0x90/+0x94）钉到成长尺寸 +0x1324 并置 render_flags
    // bit3。StageSim 尚未实现 op 0x400000（见交付报告“未解决”），此处
    // 保留绘制契约，位一旦置位即生效。
    if (bullet.flags & 0x40u) {
        vm.visual.secondary_scale = {bullet.hit_scale, bullet.hit_scale};
        // 0x40e565 `orl $0x8,0x5e4(%esi)`：visual.flags bit3（缩放脏位），
        // 与 render_flags 无关。
        vm.visual.flags |= 0x8u;
    }
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
                                        float angle, i32 seat) {
    // 447230 -> 46f820 creates player ANM script15 and primes it immediately.
    // The impact rotation and 4125b0's 20-frame mode4 local displacement are
    // installed only after the initial tick, before the manager's next update.
    const i32 bank = seat ? banks.teammate_player[size_t(seat - 1)] : banks.player;
    const u8 owner = battle_ && battle_->seat_count >= 2 ? u8(seat + 1) : 0;
    const u32 handle = animations.create(bank, 15, -1, 0, position, 0, false, owner);
    auto* vm = animations.registry.find(handle);
    if (!vm) return fail("Player beam hit effect: " + animations.error);
    vm->visual.flags |= 4u;
    vm->variables.rotation.z = angle;
    vm->interpolators.position.begin(20, 4, {0, 0, 0},
                                     {displacement.x, displacement.y, displacement.z});
    player_effect_handles_.push_back(handle);
    return true;
}

bool StagePresentation::youmu_charge_effect(i32 action, const Vec3& position, i32 seat) {
    auto& handle = youmu_charge_handles_[size_t(seat)];
    const i32 bank = seat ? banks.teammate_player[size_t(seat - 1)] : banks.player;
    const u8 owner = battle_ && battle_->seat_count >= 2 ? u8(seat + 1) : 0;
    if (action == 0) {
        // 446122..446163 creates player-bank script18 via 46f820 immediately.
        handle = animations.create(bank, 18, -1, 0, position, 0, false, owner);
        if (!handle) return fail("Youmu charge effect: " + animations.error);
        player_effect_handles_.push_back(handle);
    } else if (action == 1 && handle) {
        // 4461f0 at charge60 queues label2 to this VM and its children.
        if (!animations.interrupt(handle, 2, false))
            return fail("Youmu charge interrupt: " + animations.error);
    } else if (action == 2 && handle) {
        if (!animations.retire(handle))
            return fail("Youmu charge retirement: " + animations.error);
    } else if (action == 3) {
        if (auto* vm = animations.registry.find(handle))
            vm->visual.translation = position;
        else handle = 0;
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

// M6 快照：手账 capture（头文件 SnapshotData 注释即契约）。自有 VM 按
// 值拷贝（AnmVm 值语义，registry 快照同款）；有序集合（shot_faded_
// generations_）按升序复制为 vector（同状态同字节序）。
bool StagePresentation::capture_presentation(SnapshotData& out) const {
    const auto& effects = player_effects_[0];
    for (size_t i = 0; i < out.teammate_effects.size(); ++i) out.teammate_effects[i] = player_effects_[i + 1];
    out.embedded_identities.clear();
    const auto remember = [&](const auto& vm) { out.embedded_identities.push_back(vm.get()); };
    for (const auto& enemy : enemies_) remember(enemy.vm);
    for (const auto& bullet : bullets_) remember(bullet.vm);
    for (const auto& shot : shots_) remember(shot);
    remember(player_);
    for (const auto& vm : teammate_players_) remember(vm);
    for (const auto& vm : item_vms_) remember(vm);
    for (const auto& vm : item_side_vms_) remember(vm);
    out.enemies.clear();
    out.enemies.reserve(enemies_.size());
    for (const auto& enemy : enemies_) {
        SnapshotData::EnemyVisualData data;
        data.serial = enemy.serial;
        data.script = enemy.script;
        data.has_vm = enemy.vm != nullptr;
        if (enemy.vm) data.vm = *enemy.vm;
        out.enemies.push_back(std::move(data));
    }
    out.bullets.clear();
    for (const auto& slot : bullets_) {
        SnapshotData::BulletSlotData data;
        data.has_vm = slot.vm != nullptr;
        if (slot.vm) data.vm = *slot.vm;
        data.script = slot.script;
        data.color = slot.color;
        data.source_type = slot.source.type;
        data.source_color = slot.source.color;
        data.generation = slot.generation;
        data.active = slot.active;
        out.bullets.push_back(std::move(data));
    }
    out.shot_handles = shot_handles_;
    out.shots.clear();
    out.shots.reserve(shots_.size());
    for (const auto& shot : shots_) out.shots.push_back(shot ? *shot : AnmVm{});
    out.shot_generations = shot_generations_;
    out.shot_fades.clear();
    out.shot_fades.reserve(shot_fades_.size());
    for (const auto& fade : shot_fades_) {
        SnapshotData::ShotFadeData data;
        data.handle = fade.handle;
        data.generation = fade.generation;
        data.x = fade.x; data.y = fade.y; data.angle = fade.angle;
        data.drift = fade.drift; data.sine_moving = fade.sine_moving;
        data.sine_motion = fade.sine_motion;
        data.age = fade.age;
        out.shot_fades.push_back(data);
    }
    out.shot_faded_generations.clear();
    for (const auto& bucket : shot_faded_generations_)
        for (u32 generation : bucket.second)
            out.shot_faded_generations.push_back((u64(bucket.first) << 32) | generation);
    out.has_player = player_ != nullptr;
    // Reused snapshots can cross a confirmed scene boundary. An empty new
    // scene must not retain the previous player's retired ANM bindings.
    out.player = player_ ? *player_ : AnmVm{};
    out.player_script = player_script_;
    out.player_tilt = player_tilt_;
    for (size_t i = 0; i < teammate_players_.size(); ++i) {
        if (teammate_players_[i]) out.teammate_players[i] = *teammate_players_[i];
        else out.teammate_players[i].reset();
    }
    out.teammate_tilts = teammate_tilts_;
    out.spirit_handles = spirit_handles_;
    out.spirit_serials = spirit_serials_;
    out.spirit_scripts = spirit_scripts_;
    out.item_vms.clear();
    out.item_side_vms.clear();
    for (size_t i = 0; i < item_vms_.size(); ++i) {
        out.item_vm_present[i] = item_vms_[i] != nullptr;
        if (item_vms_[i]) out.item_vms.push_back(*item_vms_[i]);
    }
    for (size_t i = 0; i < item_side_vms_.size(); ++i) {
        out.item_side_vm_present[i] = item_side_vms_[i] != nullptr;
        if (item_side_vms_[i]) out.item_side_vms.push_back(*item_side_vms_[i]);
    }
    out.youmu_slash_handles = effects.youmu_slash_handles;
    out.youmu_slash_extra = effects.youmu_slash_extra;
    out.option_handles = effects.option_handles;
    out.full_option_handles = effects.full_option_handles;
    out.bomb_orb_handles = effects.bomb_orb_handles;
    out.bomb_orb_exploding = effects.bomb_orb_exploding;
    out.bomb_ray_handles = effects.bomb_ray_handles;
    out.trance_handle = trance_handle_;
    out.trance_was_active = trance_was_active_;
    out.bullet_cancel_handles = bullet_cancel_handles_;
    out.focus_handle = effects.focus_handle;
    out.youmu_charge_handles = youmu_charge_handles_;
    out.fog_handles = fog_handles_;
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
    out.player_effect_handles = player_effect_handles_;
    return true;
}

// M6 快照：手账 restore。自有 VM 重建后重绑宿主指针：environment/
// object_host 指向 animations（快照存活期身份不变），bullets_ 槽 VM 的
// sprite_source 指回**槽自身**的 Source（值拷贝后原指针悬垂——槽在
// deque 尾部追加可能整体搬移）。registry 句柄原值写回（registry 已按
// 同 generation 恢复，句柄值语义有效）。
bool StagePresentation::restore_presentation(const SnapshotData& source,
    std::unordered_map<const AnmVm*, AnmVm*>* remapped) {
    auto& effects = player_effects_[0];
    for (size_t i = 0; i < source.teammate_effects.size(); ++i) player_effects_[i + 1] = source.teammate_effects[i];
    enemies_.clear();
    enemies_.reserve(source.enemies.size());
    for (const auto& data : source.enemies) {
        EnemyVisual visual;
        visual.serial = data.serial;
        visual.script = data.script;
        if (data.has_vm) {
            visual.vm = std::make_unique<AnmVm>(data.vm);
            visual.vm->environment = &animations.scene_environment();
            visual.vm->object_host = &animations;
        }
        enemies_.push_back(std::move(visual));
    }
    bullets_.clear();
    for (const auto& data : source.bullets) {
        BulletSlot slot;
        if (data.has_vm) {
            slot.vm = std::make_unique<AnmVm>(data.vm);
            slot.vm->environment = &animations.scene_environment();
            slot.vm->object_host = &animations;
        }
        slot.script = data.script;
        slot.color = data.color;
        slot.source.type = data.source_type;
        slot.source.color = data.source_color;
        slot.generation = data.generation;
        slot.active = data.active;
        bullets_.push_back(std::move(slot));
        auto& restored = bullets_.back();
        if (restored.vm) restored.vm->sprite_source = &restored.source;
    }
    shot_handles_ = source.shot_handles;
    shots_.clear();
    shots_.reserve(source.shots.size());
    for (const auto& shot : source.shots) {
        if (!shot.resource) {
            shots_.push_back(nullptr);
            continue;
        }
        auto vm = std::make_unique<AnmVm>(shot);
        vm->environment = &animations.scene_environment();
        vm->object_host = &animations;
        shots_.push_back(std::move(vm));
    }
    shot_generations_ = source.shot_generations;
    shot_fades_.clear();
    shot_fades_.reserve(source.shot_fades.size());
    for (const auto& data : source.shot_fades) {
        ShotFade fade;
        fade.handle = data.handle;
        fade.generation = data.generation;
        fade.x = data.x; fade.y = data.y; fade.angle = data.angle;
        fade.drift = data.drift; fade.sine_moving = data.sine_moving;
        fade.sine_motion = data.sine_motion;
        fade.age = data.age;
        shot_fades_.push_back(fade);
    }
    shot_faded_generations_.clear();
    for (u64 key : source.shot_faded_generations)
        shot_faded_generations_[u32(key >> 32)].push_back(u32(key));
    for (auto& bucket : shot_faded_generations_) {
        auto& keys = bucket.second;
        std::sort(keys.begin(), keys.end());
        keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
    }
    player_.reset();
    if (source.has_player) {
        player_ = std::make_unique<AnmVm>(source.player);
        player_->environment = &animations.scene_environment();
        player_->object_host = &animations;
    }
    player_script_ = source.player_script;
    player_tilt_ = source.player_tilt;
    for (size_t i = 0; i < teammate_players_.size(); ++i) {
        teammate_players_[i].reset();
        if (!source.teammate_players[i]) continue;
        auto vm = std::make_unique<AnmVm>(*source.teammate_players[i]);
        vm->environment = &animations.scene_environment();
        vm->object_host = &animations;
        teammate_players_[i] = std::move(vm);
    }
    teammate_tilts_ = source.teammate_tilts;
    spirit_handles_ = source.spirit_handles;
    spirit_serials_ = source.spirit_serials;
    spirit_scripts_ = source.spirit_scripts;
    for (size_t i = 0; i < item_vms_.size(); ++i) {
        item_vms_[i].reset();
        item_side_vms_[i].reset();
    }
    size_t body_cursor = 0, side_cursor = 0;
    for (size_t i = 0; i < item_vms_.size(); ++i) {
        if (source.item_vm_present[i]) {
            item_vms_[i] = std::make_unique<AnmVm>(source.item_vms[body_cursor++]);
            item_vms_[i]->environment = &animations.scene_environment();
            item_vms_[i]->object_host = &animations;
        }
        if (source.item_side_vm_present[i]) {
            item_side_vms_[i] = std::make_unique<AnmVm>(source.item_side_vms[side_cursor++]);
            item_side_vms_[i]->environment = &animations.scene_environment();
            item_side_vms_[i]->object_host = &animations;
        }
    }
    effects.youmu_slash_handles = source.youmu_slash_handles;
    effects.youmu_slash_extra = source.youmu_slash_extra;
    effects.option_handles = source.option_handles;
    effects.full_option_handles = source.full_option_handles;
    effects.bomb_orb_handles = source.bomb_orb_handles;
    effects.bomb_orb_exploding = source.bomb_orb_exploding;
    effects.bomb_ray_handles = source.bomb_ray_handles;
    trance_handle_ = source.trance_handle;
    trance_was_active_ = source.trance_was_active;
    bullet_cancel_handles_ = source.bullet_cancel_handles;
    effects.focus_handle = source.focus_handle;
    youmu_charge_handles_ = source.youmu_charge_handles;
    fog_handles_ = source.fog_handles;
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
    player_effect_handles_ = source.player_effect_handles;
    if (remapped) {
        size_t i = 0;
        const auto restore_identity = [&](const auto& vm) {
            const auto* old = source.embedded_identities[i++];
            if (old) remapped->emplace(old, vm.get());
        };
        for (const auto& enemy : enemies_) restore_identity(enemy.vm);
        for (const auto& bullet : bullets_) restore_identity(bullet.vm);
        for (const auto& shot : shots_) restore_identity(shot);
        restore_identity(player_);
        for (const auto& vm : teammate_players_) restore_identity(vm);
        for (const auto& vm : item_vms_) restore_identity(vm);
        for (const auto& vm : item_side_vms_) restore_identity(vm);
    }
    return true;
}

// M6 快照恢复的手账重置（头文件注释即契约）。只清「权威态 → 表现层」
// 的派生缓存；registry 的 VM 池由 WorldState::restore 恢复（句柄 generation
// 一致），重置只丢弃指向发散期的悬空映射——下一帧 sync 全量重建。
void StagePresentation::reset_snapshot_caches() {
    for (auto& enemy : enemies_)
        if (enemy.vm) enemy.vm.reset();
    enemies_.clear();
    for (auto& bullet : bullets_)
        if (bullet.vm) bullet.vm.reset();
    bullets_.clear();
    // 自机弹句柄池：registry 已恢复，句柄本身在恢复后的池里仍有效——
    // 但 generation 匹配面（shot_generations_）与权威弹的对应在发散期
    // 错位，保留会误退休权威弹（M6 实证）。清空后 sync 重建。
    shot_handles_.clear();
    shot_generations_.clear();
    for (auto& shot : shots_) shot.reset();
    shots_.clear();
    shot_fades_.clear();
    shot_faded_generations_.clear();
    for (auto& vm : item_vms_) vm.reset();
    for (auto& vm : item_side_vms_) vm.reset();
    for (u32& handle : spirit_handles_) handle = 0;
    spirit_serials_.fill(0);
    spirit_scripts_.fill(0);
    for (auto& effects : player_effects_) {
        for (u32& handle : effects.youmu_slash_handles) handle = 0;
        effects.youmu_slash_extra.clear();
        for (u32& handle : effects.bomb_orb_handles) handle = 0;
        effects.bomb_orb_exploding.fill(false);
        for (u32& handle : effects.bomb_ray_handles) handle = 0;
        for (u32& handle : effects.option_handles) handle = 0;
        for (u32& handle : effects.full_option_handles) handle = 0;
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
        if (handle && !animations.retire(handle))
            fail("Spirit animation retire failed: " + animations.error);
        handle = 0;
    }
    spirit_serials_.fill(0);
    spirit_scripts_.fill(0);
    for (auto& vm : item_vms_) vm.reset();
    for (auto& vm : item_side_vms_) vm.reset();
    for (auto& effects : player_effects_) {
        for (u32 handle : effects.youmu_slash_handles)
            if (handle && !animations.retire(handle))
                fail("Youmu slash retire failed: " + animations.error);
        for (u32 handle : effects.youmu_slash_extra)
            if (handle && !animations.retire(handle))
                fail("Youmu slash retire failed: " + animations.error);
        effects = PlayerEffectsState{};
    }
}

u32 StagePresentation::create_player_effect(i32 bank, i32 script, i32 layer,
                                            u32 ordering, const Vec3& position) {
    return create_player_effect(bank, script, layer, ordering, position, 0);
}

u32 StagePresentation::create_player_effect(i32 bank, i32 script, i32 layer,
                                            u32 ordering, const Vec3& position, u8 owner) {
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
    vm->shot_effect_owner = owner;
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

bool StagePresentation::sync_bomb_effects(const StageBattle& battle, i32 seat) {
    const i32 index = battle.seat_count >= 2 ? (seat < 0 ? battle.active_seat : seat) : 0;
    auto& effects = player_effects_[size_t(index)];
    const auto& player = battle.seat_count >= 2 && seat >= 0
                             ? battle.seats[size_t(index)] : battle.player_state();
    const i32 bank = player_bank(index);
    const u8 owner = battle.seat_count >= 2 ? u8(index + 1) : 0;

    const i32 character=player.character;
    if(player.bomb_generation!=effects.bomb_generation) {
        effects.bomb_generation=player.bomb_generation;
        // 40b4b0 / 40a650 / 40be80 / 40c910: original plXX background
        // scripts, not effect.anm or a synthetic fullscreen flash.
        if(!end_player_effect(effects.bomb_background,false)||
           !end_player_effect(effects.bomb_beam,false))return false;
        for(auto& handle:effects.bomb_ray_handles)
            if(!end_player_effect(handle,false))return false;
        if(player.bomb_timer>0) {
            if(character==1) {
                effects.bomb_beam=create_player_effect(bank,23,-1,0,Vec3{player.bomb_center_x, player.bomb_center_y, 0},owner);
                if(!effects.bomb_beam)return false;
            }
            constexpr i32 backgrounds[]={26,31,27,25};
            effects.bomb_background=create_player_effect(bank,backgrounds[character],
                                                   -1,0,Vec3{player.bomb_center_x, player.bomb_center_y, 0},owner);
            if(!effects.bomb_background)return false;
            if(character!=0) {
                auto* vm=animations.registry.find(effects.bomb_background);
                vm->visual.color=(vm->visual.color&0xffffffu)|0xe0000000u;
                vm->interpolators.secondary_alpha.control2[0]=224; // native vm+334
            }
            if(character==2)for(size_t i=0;i<effects.bomb_ray_handles.size();++i) {
                // 40be80 copies pl02 template +81bc = script23 * 5a4.
                effects.bomb_ray_handles[i]=create_player_effect(bank,23,-1,0,
                                                         player.bomb_rays[i].origin,owner);
                if(!effects.bomb_ray_handles[i])return false;
            }
        }
    }
    if(player.bomb_timer<=0) {
        if(!end_player_effect(effects.bomb_background,false)||
           !end_player_effect(effects.bomb_beam,false))return false;
        for(auto& handle:effects.bomb_ray_handles)
            if(!end_player_effect(handle,false))return false;
        return true;
    }
    if(character==1) {
        if(auto* vm=animations.registry.find(effects.bomb_beam)) {
            vm->visual.translation=Vec3{player.bomb_center_x, player.bomb_center_y, 0};
            vm->variables.rotation.z=player.bomb_visual_angle;
            vm->visual.flags|=4;
        }
    } else if(character==2) {
        if(auto* vm=animations.registry.find(effects.bomb_background))
            vm->visual.translation=Vec3{player.bomb_center_x, player.bomb_center_y, 0};
        for(size_t i=0;i<effects.bomb_ray_handles.size();++i)
            if(auto* vm=animations.registry.find(effects.bomb_ray_handles[i])) {
                vm->visual.translation=player.bomb_rays[i].origin;
                vm->variables.rotation.z=player.bomb_rays[i].angle;
                vm->visual.flags|=4;
            }
    }
    return true;
}

bool StagePresentation::sync_player_effects(StageBattle& battle, i32 seat) {
    const i32 index = battle.seat_count >= 2 ? (seat < 0 ? battle.active_seat : seat) : 0;
    auto& effects = player_effects_[size_t(index)];
    const auto& player = battle.seat_count >= 2 && seat >= 0
                             ? battle.seats[size_t(index)] : battle.player_state();
    const i32 bank = player_bank(index);
    const u8 owner = battle.seat_count >= 2 ? u8(index + 1) : 0;

    player_effect_handles_.erase(
        std::remove_if(player_effect_handles_.begin(), player_effect_handles_.end(),
                       [&](u32 handle) { return !animations.registry.find(handle); }),
        player_effect_handles_.end());
    const bool visible = (!player.dead && player.phase != StageBattle::PlayerPhase::Dying);
    const Vec3 position{player.x, player.y, 0};
    if(!sync_bomb_effects(battle,seat))return false;

    // 00442a8f：DAT_004c2180 + 0x10 = effect.anm，不是自机 plXX.anm。
    // script22 自带 sprite23、12帧缩放、18帧旋转及子script21；松Shift
    // label1 的8帧退场也由原版脚本完成，不画近似圆、不手工改scale。
    if (visible && player.focus) {
        if (!animations.registry.find(effects.focus_handle)) {
            effects.focus_handle = create_player_effect(banks.effect, 22, 12, 0, position, owner);
            if (!effects.focus_handle) return false;
        }
        animations.registry.find(effects.focus_handle)->visual.translation = position;
    } else if (!end_player_effect(effects.focus_handle, !visible)) {
        return false;
    }

    if (!sync_player_options(battle,false,seat)) return false;

    // 0040b1b0：DAT_004c22c4 + 0x10（pl00.anm）script18；
    // 0040b2a0：label1 → 子script21/23/25与19爆炸，不是effect.anm18。
    // 伤害/运动/爆炸触发全归 battle，表现层只响应原状态边沿。
    for (size_t i = 0; i < effects.bomb_orb_handles.size(); ++i) {
        const auto& orb = player.bomb_orbs[i];
        if (player.character != 0 || (!orb.active && !orb.exploding)) {
            if (!end_player_effect(effects.bomb_orb_handles[i], true)) return false;
            effects.bomb_orb_exploding[i] = false;
            continue;
        }
        if (!animations.registry.find(effects.bomb_orb_handles[i]) && !orb.exploding) {
            effects.bomb_orb_handles[i] = create_player_effect(
                bank, 18, -1, 0, {orb.x, orb.y, 0}, owner);
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
    // 隐藏位：0x42b6d0 同时置 player+0x14698 bit3。移植侧置 effects.options_hidden，
    // 使 12185..12213 的 sync_player_options 走 0x442960 隐藏支路（把槽
    // 位置钉在 player+0x5c4/+0x5c8 并跳过 per-slot 更新；见
    // sync_player_options），而不是继续按门内已过时的 battle.options[i].x/y
    // 写根 translation。
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
    // 仍保留；移植侧按 correctedPlan 同时清 effects.option_handles/
    // effects.full_option_handles 并把 option 状态复位为“未建树”，作为“槽已不
    // 活跃”的等价簿记：门内 sync 不重建，S2 的 0x444df0 公共循环重建。
    // 幂等：0x442960 每帧 counter>0x1d 都会重入；句柄清零后重复调用是空操作。
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
    const auto& player = battle.seat_count >= 2 && seat >= 0
                             ? battle.seats[size_t(index)] : battle.player_state();
    const i32 bank = player_bank(index);
    const u8 owner = battle.seat_count >= 2 ? u8(index + 1) : 0;

    // 门内隐藏旗标期：0x442960 已把 option 槽置为不活跃（原版清
    // player+0xa2b8；移植侧等价清句柄并置 pending label1），旧树退场中；
    // 0x444df0 只在 S2 起始（0x441b00 label2 激活后）重建，门内不得按
    // missing 重建（否则会在装载门里重新长出卫星并继续抽视觉 RNG）。
    if (effects.option_rebuild_pending) return true;
    // 0x442960 隐藏支路（player+0x14698 bit3）：decomp 00442960.c:345-348
    // 把槽位置 piVar13[-3]/[-2] 直接写成 player+0x5c4/+0x5c8；capstone
    // 0x442f5d test bit3 / jne 0x443005，0x443005 mov edx,[edi+0x5c4]，
    // 0x44300b mov [esi-0xc],edx，0x44300e/0x443014 同式写 y。槽位置即
    // 根 VM translation 的来源。门内 StageBattle::update_options 因
    // option_movement_tick_=false（StageBattle.cpp:2070/2073 的
    // step_transition）跳过 per-slot 位置计算，player.options[i].x/y 仍是
    // 换关前的过时值；若继续走 visible 分支，卫星会画在旧位置而不是自机
    // 位置。故隐藏期只把两组根钉到 player.x/player_y（port 的
    // player+0x5c4/+0x5c8 对应字段，定点×1/128 的浮点无损表示），并跳过
    // 下面的 per-slot create/retire/anchor 更新。
    if (effects.options_hidden) {
        const Vec3 anchor{player.x, player.y, 0};
        for (u32 handle : effects.option_handles)
            if (auto* vm = animations.registry.find(handle))
                vm->visual.translation = anchor;
        for (u32 handle : effects.full_option_handles)
            if (auto* vm = animations.registry.find(handle))
                vm->visual.translation = anchor;
        return true;
    }
    const bool visible = (!player.dead && player.phase != StageBattle::PlayerPhase::Dying);
    const bool defer_anchor = power_rebuild || effects.option_anchor_deferred;
    // 00444df0 模型（反证 correctedPlan 更正，原报告把 S2 重建误写成满P分支）：
    //   * power<max（妖梦 S2 实测 power=293 < max=400）：0x444e52 对
    //     0xa36c 满P树调 0x46fc30(handle,1)（= end_player_effect(handle,false)
    //     的 deferred label1），再由公共 0x444f74 循环 retire 旧 0xa368 普通
    //     根（render_flags|=0x20）并 create 新普通根；
    //   * power>=max 才走 0x11/0x14/0x15（=17/20/21）满P树的 retire+create
    //     分支，即下方 full 首循环的 full_scripts。
    // 普通树脚本 = 0x10/0x12/0x13/0x0f（16/18/19/15）；满P树 = 0x11/0x14/
    // 0x15（17/20/21，妖梦满P同为17）。00442960 已计算完的定点卫星位置由
    // player.options 唯一提供，此处不再维护独立跟随轨迹，也不在暂停/展示帧
    // 推进卫星物理。
    constexpr i32 option_scripts[4] = {16, 18, 19, 15};
    constexpr i32 full_scripts[4] = {17, 20, 21, 17};
    const i32 character = player.character;
    // 原版 0x146c4 的语义源 = power/power_step，与 Dying 无关：死亡演出
    // 第 3 帧的 0x444df0 重建必须看到新 power 的 count。
    const i32 count = player.option_count;
    const bool full = visible && (battle.seat_count >= 2 ? battle.seat_huds[size_t(index)].power >= battle.seat_huds[size_t(index)].max_power : battle.full_power());
    if (character < 0 || character > 3 || count < 0 || count > 8)
        return fail("Player option state outside original bounds");
    // 0x444a10（死亡演出）对 8 组普通/满P option 根及子体只排队 label1
    // （vm+0x4a0=1，0x46fc30 的 deferred 语义），**不 retire**；树在死亡
    // 等待/重生期继续 tick，label1 脚本的视觉 RNG 抽取必须保留。
    // round-23 S4 f6363 实证：原生 visual_index 每 3-4 帧多 12，首差帧
    // 正是死亡后第 3 帧。
    if (!visible) {
        if (!effects.death_options_queued) {
            for (u32 handle : effects.option_handles)
                if (handle && !animations.interrupt(handle, 1, false))
                    return fail("Player option death interrupt failed: " +
                                animations.error);
            for (u32 handle : effects.full_option_handles)
                if (handle && !animations.interrupt(handle, 1, false))
                    return fail("Full power option death interrupt failed: " +
                                animations.error);
            effects.death_options_queued = true;
        }
        // 0x443240:130（0x668==3，死亡演出第 3 帧）调 0x444df0：0x146c4
        // 已被 0x444a10 置 0，按扣 power 后的 count 重建普通树；满P树只
        // 0x46fc30 排队 label1（不 retire）。新建根的初始 tick 在同帧抽
        // 视觉 RNG（S4 f6363 原生 +12 即 3 根 × 2 抽）。
        if (player.death_option_rebuild) {
            if (battle.seat_count >= 2 && seat >= 0) {
                battle.seats[size_t(index)].death_option_rebuild = false;
                if (index == battle.active_seat) battle.death_option_rebuild = false;
            } else battle.death_option_rebuild = false;
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
                effects.option_handles[i] = create_player_effect(
                    bank, option_scripts[character], 12, 0, anchor, owner);
                if (!effects.option_handles[i]) return false;
            }
        }
        effects.option_anchor_deferred = power_rebuild;
        return true;
    }
    effects.death_options_queued = false;
    // power<max：满P树 0x46fc30(handle,1)（visible&&!full 时 end_player_effect
    // 的 retire=false 分支），普通根 retire 后由下方公共循环重建。
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
    // 444df0 creates the full-power trees in a separate first loop.
    if (full) for (size_t i = 0; i < effects.full_option_handles.size(); ++i) {
        const auto& option = player.options[i];
        if (i >= size_t(count) || !option.active) continue;
        const Vec3 anchor{option.x, option.y, 0};
        if (!animations.registry.find(effects.full_option_handles[i])) {
            effects.full_option_handles[i] = create_player_effect(
                bank, full_scripts[character], 12, 2,
                power_rebuild ? Vec3{} : anchor, owner);
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
                !end_player_effect(effects.full_option_handles[i], true)) return false;
            continue;
        }
        const Vec3 anchor{option.x, option.y, 0};
        if (!animations.registry.find(effects.option_handles[i])) {
            effects.option_handles[i] = create_player_effect(
                bank, option_scripts[character], 12, 0,
                power_rebuild ? Vec3{} : anchor, owner);
            if (!effects.option_handles[i]) return false;
            if (power_rebuild)
                animations.registry.find(effects.option_handles[i])->visual.translation = {0, -32, 0};
        }
        if (!defer_anchor) animations.registry.find(effects.option_handles[i])->visual.translation = anchor;
    }
    effects.option_anchor_deferred = power_rebuild;
    return true;
}

bool StagePresentation::reimu_bomb_orb_birth(const StageBattle& battle, i32 index) {
    const i32 seat = battle.seat_count >= 2 ? battle.active_seat : 0;
    auto& effects = player_effects_[size_t(seat)];

    if (index < 0 || index >= i32(effects.bomb_orb_handles.size()))
        return fail("Reimu bomb orb index outside pool");
    u32& handle = effects.bomb_orb_handles[size_t(index)];
    if (animations.registry.find(handle)) return fail("Reimu bomb orb slot already active");
    const auto& orb = battle.bomb_orbs[size_t(index)];
    handle = create_player_effect(player_bank(seat), 18, -1, 0, {orb.x, orb.y, 0}, battle.seat_count >= 2 ? u8(seat + 1) : 0);
    effects.bomb_orb_exploding[size_t(index)] = false;
    return handle != 0;
}

bool StagePresentation::initialize_player_shot(PlayerShot& shot, u32 index, i32 seat) {
    if (shot.script < 0) return true;
    // Keep each seat's compacted slots and character resources independent.
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
        if (stale && !animations.release_tree(*stale))
            return fail("Player shot animation release failed");
        shot_handles_[index] = 0;
    }
    const auto* source = animations.script_template(bank, u32(shot.script));
    if (!source) return fail("Player shot animation template unavailable");
    // 0x445e39 0x4706c0 池分配 → 0x445e47 0x403800 模板拷贝 → 0x445e4e
    // render_flags|=0x200 → 0x445e6a 平移清零 → 0x445e7e 0x462450 首 tick →
    // 0x445e87 0x46f3f0 主链尾插（flags=0，非 0x46f4a0 头插）。
    auto* vm = animations.registry.allocate();
    if (!animations.bind_template(*vm, bank, shot.script)) {
        animations.registry.discard(*vm);
        return fail("Player shot animation template unavailable");
    }
    if (anm_random_observer)
        anm_random_observer(*vm, -11, shot.script, 0, u32(bank), 0, 0, 0, false);
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
    vm->shot_effect_owner = battle_ && battle_->seat_count >= 2 ? u8(seat + 1) : 0;
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
    for (size_t i = 0; i < shot_handles_.size(); ++i) {
        const bool multi = battle_->seat_count >= 2;
        const size_t seat = multi ? i % 3 : 0, slot = multi ? i / 3 : i;
        if (multi && seat >= size_t(battle_->seat_count)) continue;
        auto& pool = multi ? battle_->seats[seat].shots : battle_->shots;
        if (slot >= pool.size()) continue;
        auto& shot = pool[slot];
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
        battle_->set_player_shot_geometry(i32(slot), shot.age, shot.script, shot.generation,
                                          snapshot, i32(seat));
    }
    for (size_t n = 0; n < shot_fades_.size();) {
        if (!animations.registry.find(shot_fades_[n].handle))
            shot_fades_.erase(shot_fades_.begin() + i32(n));
        else ++n;
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
    // Keep this narrowly scoped to the player VM family. Projectile and enemy
    // presentation is still created by sync() after the first simulation step.
    battle_ = &battle;
    return sync_all_player_effects(battle);
}

bool StagePresentation::activate_player_effects(StageBattle& battle) {
    // 00441b00 broadcasts label2 to every option root and child. In particular
    // pl03 script15's script16 child consumes the four RNG reads at this
    // deferred manager tick; do not call tick_instance directly (that bypasses
    // the native interrupt path and gives other characters a false extra tick).
    for (auto& effects : player_effects_) {
        for (size_t i = 0; i < effects.option_handles.size(); ++i) {
            for (u32 handle : {effects.option_handles[i], effects.full_option_handles[i]}) {
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
        effects.option_rebuild_pending = false;
        // 0x441b00 同时 and [player+0x14698],~8 清隐藏位；重建树必须回到
        // visible 分支（否则 S2 的 sync 仍走隐藏支路，只钉位置不建树）。
        effects.options_hidden = false;
    }
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
// 必须走 create_preserve_ip：0x46f820 的 0x403800 模板拷贝保留 IP 与
// 插值通道相位（只重置 VM 计时器），脚本 96-110 的模板预跑已把 op412
// 生长插值推进一拍；从指令 0 重放会丢掉该相位（原生 5560 拾取
// scale.y=0.75 vs 端口 0.5，且后续逐帧落后一拍）。
bool StagePresentation::spirit_pickup(i32 script, const Vec3& position) {
    if (!animations.create_preserve_ip(banks.front, script, -1, 0, position, 0, false))
        return fail("Spirit pickup animation creation failed: " + animations.error);
    return true;
}

// 0x438ec0 出生体内两处 0x46f820（反汇编 0x438ff9 / 0x43909d / 0x4390be）：
//  1) 先建 effect.anm script 0x38（0x438ff1 push 0x38；层 -1、flags 0、
//     位置=神霊出生位），0x46f3f0 尾插，句柄不写回（脚本自灭）；
//  2) 再建 astral.anm kind*6+1（0x439088-0x43909d，flags bit0=0）或
//     kind*6+4（0x4390aa-0x4390be，灵界位），层 -1、flags 0、尾插，句柄写
//     神霊槽 +0x58（0x4390a5/0x4390c6 mov [ebx+0x58]）。
// 时机 = 出生当帧（0x438ec0 体内），早于本帧 0x46f170 walk；此前 port 在
// Update 25 sync_item_spirits 才建 astral VM，且从不建 0x38，使神霊链节点
// 落在本帧子弹相位创建物之后（Pn16 s05 f11697 s174 子 VM 多 tick 一帧）。
bool StagePresentation::spirit_spawn_animation(u32 slot, u32 serial, i32 kind,
                                               u32 flags, float x, float y,
                                               float z) {
    const Vec3 position{x, y, z};
    if (!animations.create(banks.effect, 0x38, -1, 0, position, 0, false))
        return fail("Spirit birth effect creation failed: " + animations.error);
    const i32 script = (flags & 1u) != 0 ? kind * 6 + 4 : kind * 6 + 1;
    const u32 handle = animations.create(banks.spirit, script, -1, 0, position, 0,
                                         false);
    if (!handle)
        return fail("Spirit animation creation failed: " + animations.error);
    if (slot < spirit_handles_.size()) {
        spirit_handles_[slot] = handle;
        spirit_serials_[slot] = serial;
        spirit_scripts_[slot] = script;
    }
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
    const i32 seat = battle.seat_count >= 2 ? battle.active_seat : 0;
    auto& effects = player_effects_[size_t(seat)];
    const u8 owner = battle.seat_count >= 2 ? u8(seat + 1) : 0;

    for (const auto& spawn : battle.youmu_slash_spawns_) {
        const u32 handle = animations.create(
            player_bank(seat), spawn.script, -1, 0, {0.0f, spawn.y, 0.0f}, 0, false, owner);
        if (!handle)
            return fail("Youmu slash animation creation failed: " +
                        animations.error);
        if (spawn.slot >= 0 && spawn.slot < YoumuBomb::kSlots) {
            effects.youmu_slash_handles[size_t(spawn.slot)] = handle;
        } else {
            effects.youmu_slash_extra.push_back(handle);  // cursor 越界：仅视觉
        }
    }
    for (int slot = 0; slot < YoumuBomb::kSlots; ++slot) {
        u32& handle = effects.youmu_slash_handles[size_t(slot)];
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
            if (!animations.bind_template_preserve_ip(*vm, banks.bullet, script))
                return fail("Item animation template unavailable: " + animations.error);
            vm->visual.layer = 15;
            vm->visual.render_flags |= 0x200u | 0x40000u;
            if (vm->tick(random, animations.rate) < 0)
                return fail("Item animation initialization: " + vm->error);
        }
        if (!side) {
            side = std::make_unique<AnmVm>();
            side->environment = &animations.scene_environment();
            side->object_host = &animations;
            if (!animations.bind_template_preserve_ip(*side, banks.bullet, side_script))
                return fail("Item side animation template unavailable: " +
                            animations.error);
            side->visual.layer = 15;
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
        // 0042ee90:17-49：每件道具二选一绘制——item.y >= -8 只画本体 VM1；
        // 否则画 VM2 并把锚点钉到屏幕顶 y=8（离屏指示器）。port 旧实现无条件
        // 两枚都画，激光窗因此每帧多出 10 个 bullet3 sprite（u=0.5 列）。
        // 层号 15：原生该 op 的 layer watch=15（layer15 之后、16 之前）。
        const bool on_screen = item.y >= -8.0f;
        vm->visual.flags = (vm->visual.flags & ~1u) | (on_screen ? 1u : 0u);
        side->visual.flags = (side->visual.flags & ~1u) | (on_screen ? 0u : 1u);
        vm->visual.layer = 15;
        side->visual.layer = 15;
        vm->visual.translation = {item.x, item.y, item.z};
        side->visual.translation = on_screen
            ? Vec3{item.x, item.y, item.z}
            : Vec3{item.x, 8.0f, item.z};
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
    // 层）、anmArg=0；0x4243e0 语义为模板拷贝（FUN_00403800 整块复制含
    // instruction_offset/timer，t=-1 立即块只在装载预热时执行一次）→立即
    // tick，AnmManager::create 自带首 tick。故必须 preserve_ip：通用 create
    // 的 IP 回卷会让 t=-1 块与 t=0 指令各重跑一次——st07enm 脚本 8（猯藏
    // 动物镜像变体）的 op308 是 toggle 翻转，二次执行抵消镜像 = EX 动物
    // 弹幕贴图方向反（0x46f820 类创建才有时间重置语义）。
    const auto target = resolve_script_target(file, script);
    if (target.script < 0)
        return fail("Effect animation target unresolved: file=" +
                    std::to_string(file) + " script=" + std::to_string(script));
    const u32 effect_handle =
        animations.create_preserve_ip(target.bank, target.script, -1, 2, position, 0, false);
    if (!effect_handle)
        return fail("Effect animation creation failed: file=" + std::to_string(file) +
                    " script=" + std::to_string(script) + " bank=" +
                    std::to_string(target.bank) + " manager=" + animations.error);
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
        // 0x41fe80:38 / 0x41b260:161 走 0x4243e0（模板拷贝保留 IP），同
        // create_ecl_effect 注：不回卷，t=-1 立即块不重跑（op308 镜像）。
        handle = animations.create_preserve_ip(target.bank, target.script,
                                               enemy->mirror_script_base + 7, 2, {}, 0, false);
        if (!handle)
            return fail("Slot animation creation failed: " + animations.error);
        // 0x419731 镜像重挂基准：记录本次 slot0 创建的 bank 与本地→全局
        // 偏移（stage 合并表内脚本号按文件偏移；bullet/effect 恒 0）。
        if (request.slot == 0) {
            enemy->anm_slot0_bank = target.bank;
            enemy->anm_slot0_script_offset = target.script - request.script;
        }
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
    if (battle.seat_count >= 2) {
        for (i32 seat = 0; seat < battle.seat_count; ++seat)
            battle.begin_player_shot_geometry_snapshot(seat);
    } else battle.begin_player_shot_geometry_snapshot();
    if (!sync_all_player_effects(battle)) return false;
    // 生产调度（defer_item_side）：道具/神霊的 VM 面与更新一起移到
    // Update 24/25（原版 0x42efb0/0x438e70），sync 不再内联同步。
    if (!battle.defer_item_side) {
        if (!sync_items(battle)) return false;
        if (!sync_item_spirits(battle)) return false;
    }
    // 无外部 youmu_bomb_update（RunScene.cpp 生产接线）时，表现层 sync
    // 自持斩击池更新：消费 youmu_slash_spawns_ 并逐帧回填槽。生产路径已在
    // StageBattle::step_bomb 注册矩形前经回调执行；不能重复调用
    //（set_vm 每调一次会推进槽计时器 scale += rate）。
    if (!battle.youmu_bomb_update && !sync_youmu_slash(battle)) return false;

    if (!flush_ecl_animations(battle)) return false;
    // 槽位 VM 位置跟随（0x41a8a0:84-93：敌机管理器每帧把敌机 ECL 位置
    // （enemy+0x44/48/4c）写入各槽 VM 的偏移 +0x574/578/57c——0x466a80 的
    // 位置和项；+320/+16 由 0x40000 位在 adjust 阶段补，此处只写 ECL 原值）。
    if (enemies_from_ecl) {
        for (auto& enemy : battle.sim.enemies) {
            // 镜像翻转重挂（0x4196d3-0x419827 对齐反汇编全解）：
            // 门 = flags&0x200000 且 x 速度符号（inner+0x78，±0.03f 死区）
            // 变化。0x419694 flds 0x78(%edi) / fcompl 0x4aee20(-0.03f 的
            // double 视图 -0.029999999329447746) / fcompl 0x4aee18(+0.03f
            // 视图)：v < -0.03f → -1；-0.03f ≤ v ≤ 0.03f → 0；v > 0.03f →
            // +1（故此处 float 0.03f 比较与原版逐位同域）。+0x78 由 0x419950
            // 在 0x419450 内先算
            // （速度子位置累加后、round001/界限钳制前），StageSim::tick_enemy
            // 在 integrate() 后、ECL 线程体前把该值快照进 mirror_velocity_x
            // （0x41a8a0 调用序 = 0x419450 → ECL）。
            // 行走 VM 脚本号 = 槽 0 脚本号 + 步号；步号表见 0x4196d3-0x419711
            // 的逐分支（旧 0/±1 × 新 -1/0/+1，相等即不重挂）：dir<0→1、
            // dir>0→2；dir==0 时旧 -1→3、旧 +1→4。layer = [+0x278]+7；
            // 位置 = 旧槽 VM 脚本位置；anmArg=0；尾入池句柄存回槽 0。
            // 生产敌机恒有 ECL 线程块（0x4183f0:28），mirror_ready 随
            // tick_enemy 的 integrate 相位推进；threads 为空的裸敌机只存在
            // 于合成夹具（见 StageSim::tick_enemy 注），它们不走 0x41a8a0，
            // 沿用旧位置差分回退以保持既有单测语义。
            const bool mirror_input_ready = enemy->mirror_ready || !enemy->threads;
            if ((enemy->flags & 0x200000u) && mirror_input_ready) {
                // 主路径：0x419694 读 inner+0x78（mirror_velocity_x）。
                // 过渡帧（306/构造当帧、原版 0x80000 自门空转、不比较朝向）
                // 保留旧位置差分回退，供既有单测夹具使用。
                const float vx = enemy->mirror_ready
                    ? enemy->mirror_velocity_x
                    : float(enemy->position[0] - enemy->mirror_prev_x);
                const i32 dir = vx > 0.03f ? 1 : (vx < -0.03f ? -1 : 0);
                if (dir != enemy->mirror_dir) {
                    // 0x4196e1-0x41970e：
                    //   old -1: new +1 → 0x419708 neg/sbb/add 3 → 2；new 0 → 3
                    //   old +1: new -1 → 0x4196ef neg/sbb/and ~2/add 4 → 1；
                    //           new 0 → 4
                    //   old  0: 0x4196fd setne/inc → new -1 → 1，new +1 → 2
                    // 相等分支已在 0x4196db je 0x41982f 跳过，故 old==dir 不可达。
                    const i32 step = dir == 0
                        ? (enemy->mirror_dir < 0 ? 3 : 4)
                        : (dir < 0 ? 1 : 2);
                    u32& handle = enemy->anm_slot_handle[0];
                    // 0x41974a-0x41975b：继承旧槽 VM 的脚本位置
                    // （vm+0x3c/0x40/0x44 = variables.position，不是 ECL 位），
                    // 0x4197db-0x4197e9 写入新 VM 的 +0x574/578/57c。
                    Vec3 old_pos{};
                    const AnmVm* old_vm = handle ? animations.registry.find(handle) : nullptr;
                    if (old_vm) old_pos = old_vm->variables.position;
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
                    // 0x419731 `mov eax,[eax+0x30]`：重挂脚本解析自**当前
                    // slot0 VM 的 resource**，脚本号 = (+0x26c + step) 的该
                    // 文件本地号。port 用 slot0 创建记录 (bank, 本地→全局
                    // 偏移) 复刻：offset + anm_base_script + step——303/313/
                    // 316 重建 slot0 后 26c 仍是 306 值（敌机 +0x264..0x270
                    // 仅 306 写 / 318 清），而文件随最近创建变化。无记录
                    // （合成夹具直接造敌机不建槽）回退 anm_file 解析。
                    // 0x419450 镜像重挂 = 0x403800 模板拷贝 + 0x462450 tick
                    //（与 0x4243e0 同构，保留 IP），同上述 preserve_ip 依据。
                    if (enemy->anm_slot0_bank >= 0) {
                        handle = animations.create_preserve_ip(
                            enemy->anm_slot0_bank,
                            enemy->anm_slot0_script_offset +
                                enemy->anm_base_script + step,
                            enemy->mirror_script_base + 7, 0, old_pos, 0, false);
                    } else {
                        const auto target = resolve_script_target(
                            enemy->anm_file, enemy->anm_base_script + step);
                        if (target.script < 0) return false;
                        handle = animations.create_preserve_ip(target.bank, target.script,
                                                   enemy->mirror_script_base + 7,
                                                   0, old_pos, 0, false);
                    }
                    if (!handle)
                        return fail("Mirror animation creation failed: " +
                                    animations.error);
                    create_color_step(animations.registry.find(handle));
                    enemy->mirror_dir = dir;  // 0x419827 mov +0x270 = new dir
                }
                enemy->mirror_prev_x = enemy->position[0];  // 过渡帧回退基线
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
                    // 41a9d1..41aa0c: op424 follows inner+78/+7c motion,
                    // without the bullet draw path's pi/2 offset. The direct
                    // position branch (08000000) does not write rotation.
                    if (!(enemy->flags & 0x8000000u) && (vm->visual.render_flags & 0x40u)) {
                        vm->variables.rotation.z = float(std::atan2(double(enemy->motion_delta[1]),
                                                                   double(enemy->motion_delta[0])));
                        vm->visual.flags |= 4u;
                    }
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
        // 同上：原版不写 VM rotation，脚本 op401 是唯一来源。
        if (slot.vm->tick(random, animations.rate) < 0) return fail("Bullet animation: " + slot.vm->error);
    }
    for (size_t i = 0; i < bullets_.size(); ++i)
        if (!matched[i]) bullets_[i].active = false;

    }

    // ---- 自机弹：.sht 弹型脚本（plXX.anm，旋转模式 1 随弹向）；无 .sht
    // 会话回退精灵直贴（player_shot_sprite_） ----
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
        const i32 bank = seat > 0 && seat < battle.seat_count
            ? banks.teammate_player[size_t(seat - 1)] : banks.player;
        auto* player_resource = animations.resource(bank);
        const auto fade_key = [seat](u32 generation) { return (u64(u32(seat)) << 32) | generation; };
        const auto insert_fade_key = [this](u64 key) {
            auto& keys = shot_faded_generations_[u32(key >> 32)];
            const u32 generation = u32(key);
            if (keys.empty() || keys.back() < generation) { keys.push_back(generation); return true; }
            const auto it = std::lower_bound(keys.begin(), keys.end(), generation);
            if (it != keys.end() && *it == generation) return false;
            keys.insert(it, generation); return true;
        };
        const auto contains_fade_key = [this](u64 key) {
            const auto bucket = shot_faded_generations_.find(u32(key >> 32));
            return bucket != shot_faded_generations_.end() &&
                std::binary_search(bucket->second.begin(), bucket->second.end(), u32(key));
        };
        if (shot_at(i) != nullptr && (*shot_at(i)).alive) {
            auto& shot = (*shot_at(i));
            // Compaction changes slots, not ANM identity. Preserve a survivor's
            // VM; a new shot binds fresh even if its script matches the old one.
            bool fresh_slot = false;
            if (shot_generations_[i] != shot.generation) {
                size_t source = i + 1;
                while (source < shot_handles_.size() &&
                       (shot_generations_[source] != shot.generation || (multi_shots && source % 3 != i % 3))) ++source;
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
                    vm->source_script = -1;  // 回退精灵无脚本身份（重绑判定用）
                    if (!vm->select_sprite(player_shot_sprite_))
                        return fail("Player shot sprite unavailable");
                    vm->visible = true;
                    vm->visual.flags |= 1;
                    vm->visual.render_flags |= 0x200;
                    vm->visual.set_layer(11);
                    vm->shot_effect_owner = multi_shots ? u8(seat + 1) : 0;
                }
                shot_generations_[i] = shot.generation;
            }
            // 0x446bf0 命中回调 / 0x4471c5·0x447213 光束中断都经 vm+0x59c
            // （0x46fca0/0x46fc30）置 +0x4a0 并沿 +0x560 子链把 label 逐个
            // 写到每个子体；tick 仍由主链 walk 消费（0x46fc30 不立即 tick）。
            // 只写父 VM 的 pending 时，script13 光束经 op500 生成的 script14
            // 光斑子体收不到 label1：父走 252（op412 收缩 8 帧）→op1 自灭，
            // 子体却在 op200(80,0) 循环里回绕继续脉冲，直到父树销毁才被清
            // （th13_udMy25 5521-5530 链 dump：原生子 5521 起收缩 t16 自灭，
            // port 子 5522 回绕 t=1）。脚本无 label 的普通弹净效果不变
            // （interrupt 扫描失败即清除 pending），旧回退路径保持直写。
            // MP hides the spent projectile at impact. Its ANM still ticks
            // for normal cleanup and separate effects. Beams use state 2 for
            // their shutdown animation; piercing shots stay in state 1.
            vm->hide_hit_projectile = multi_shots && shot.state == 2 && shot.behavior != 2;
            if (shot.state == 2 && insert_fade_key(fade_key(shot.generation))) {
                // 446bf0, once per hit generation.
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
            // 0x446670：仅当脚本 op424 置 render_flags bit6 时写角度与
            // visual.flags|=4；pl03 script11/12（无 op424）保留脚本 op401
            // 的旋转值（raw pl03.anm：script11/12 op401 z=-π/2）。
            if (vm->visual.render_flags & 0x40u) {
                vm->visual.flags |= 4u;
                vm->variables.rotation.z = shot.angle;
            }
            if (shot.behavior == 2 && shot.state == 1) {
                // 446f20 writes VM+80 (sprite/anchor size) and +78 (UV scale)
                // each frame; 0x467b50 用 +0x80 做几何锚点尺寸，0x467350 用
                // +0x78 做 UV 缩放。port 的对应字段是 sprite_size（+0x80）与
                // uv_scale（+0x78）——旧实现误写 scale/secondary_scale，几何
                // 宽变成 sprite_size*(w/512)*w，产生上万像素顶点。
                vm->visual.sprite_size.x = shot.beam_visual_width;
                vm->visual.uv_scale.x = shot.beam_visual_width / 512.f;
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
            const i32 corpse_script = shot_at(i) != nullptr ? (*shot_at(i)).script : -1;
            AnmVm* vm = corpse_script >= 0
                            ? animations.registry.find(shot_handles_[i])
                            : (shots_[i] ? shots_[i].get() : nullptr);
            bool fade = false;
            PlayerShot corpse_shot{};
            if (shot_at(i) != nullptr && !(*shot_at(i)).alive) {
                const PlayerShot& shot = (*shot_at(i));
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
                       !contains_fade_key(fade_key(shot.generation)) &&
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
                insert_fade_key(fade_key(corpse.generation));
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
    const bool first_ghost = battle.seat_count >= 2 && battle.seats[0].ghost;
    if (battle.player_visible() || first_ghost) {
        const i32 tilt = first_ghost ? 0 : ((held & 0x80) ? 1 : 0) - ((held & 0x40) ? 1 : 0);
        if (!player_) player_ = std::make_unique<AnmVm>();
        bool rebound = false;
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
            rebound = true;
        }
        player_->visual.translation = {battle.player_x + kGameToScreenX,
                                       battle.player_y + kGameToScreenY, 0.0f};
        if (player_->tick(random, animations.rate) < 0) return fail("Player animation: " + player_->error);
        // 原生 0x442960 经 0x46f820 创建时 initial tick 一次，同帧主链
        // 0x46f170 再 tick 一次（出生/换向重绑帧双拍）；port 的 player_ 不入链，
        // 旧实现只 tick 一次，pl01 换帧边界整体晚 1 帧（f5/f10/f15 vs 原生
        // f4/f9/f14）。pl01 script0-4 无 RNG 指令，补拍不影响 core。
        if (rebound && player_->tick(random, animations.rate) < 0)
            return fail("Player animation: " + player_->error);
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
        const i32 tilt = state.ghost ? 0 : ((buttons & 0x80) ? 1 : 0) - ((buttons & 0x40) ? 1 : 0);
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
        if (vm->tick(random, animations.rate) < 0 ||
            (rebound && vm->tick(random, animations.rate) < 0))
            return fail("Teammate animation: " + vm->error);
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
    case 1: {
        // Ship transforms and inherited colors are draw scratch; preserve the
        // update snapshot across first and repeated presentation alike.
        const auto draw_ship = [&](AnmVm& vm, u8 opacity) {
            const auto visual = vm.visual;
            const auto rotation = vm.variables.rotation;
            const int result = renderer.draw_with_opacity(vm, opacity);
            vm.visual = visual;
            vm.variables.rotation = rotation;
            return result != -2 || fail(renderer.error.empty() ? "Stage draw failed" : renderer.error);
        };
        if (!battle_ || battle_->seat_count < 2)
            return !player_ || !player_->visible || draw_ship(*player_, 255);
        const i32 local = battle_->mp_local_seat;
        const auto ship = [&](i32 seat) -> AnmVm* {
            return seat == 0 ? player_.get() : teammate_players_[size_t(seat - 1)].get();
        };
        // Match TH15: ships within 48 game pixels fade further. Draw the
        // local ship last so a teammate can never obscure it at overlap.
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
        // 原版敌弹按池/链序（新生在前 = generation 降序）绘制；port 旧实现
        // 按 bullets_ 槽位序，同一 UP 批内两精灵顺序反转（f0-47 48 帧）。
        std::vector<BulletSlot*> order;
        order.reserve(bullets_.size());
        for (auto& slot : bullets_)
            if (slot.active && slot.vm) order.push_back(&slot);
        std::stable_sort(order.begin(), order.end(),
            [](const BulletSlot* a, const BulletSlot* b) {
                return a->generation > b->generation;
            });
        for (auto* slot : order) one(*slot->vm);
        break;
    }
    default:
        return fail("Stage draw pass outside schedule");
    }
    return error_.empty();
}

}  // namespace th13
