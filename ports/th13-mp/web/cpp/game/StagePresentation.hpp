#include <optional>
// Stage1 战斗表现层：把 StageSim/StageBattle 的状态同步为 ANM VM 并直绘。
// 敌机 = ECL 302/303/306 记录的脚本（stage ANM 合并表全局序号）；
// 敌弹 = bullet.anm 脚本 + 0x4120b0 精灵解析（BulletAppearance 表）；
// 自机/自机弹 = plXX.anm（脚本 0..4 姿态；弹为精灵直贴）。
// 敌机本体 VM 默认经 ECL 303/306 入 registry 句柄池（enemies_from_ecl，
// 2026-10-23 整改扶正）；弹体/自机弹/自机 VM 仍为表现层自备直绘
//（th15 PlayerDraw/GameDraw 同式——宿主自备 VM 直调 renderer 是基座允许
// 的模式），由 RunScene 注册分层回调。
#pragma once

#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "BulletAppearance.hpp"
#include "GameRandom.hpp"
#include "StageBattle.hpp"

#include <memory>
#include <deque>
#include <map>
#include <vector>

namespace th13 {

class StagePresentation {
public:
    struct Banks {
        i32 stage = 0, bullet = 0, player = 0;
        i32 effect = 8;  // effect.anm（mgr 槽 8；ECL 文件索引 1 的特效创建源）
        i32 spirit = 25; // 00405530: astral.anm, original resource slot25.
        i32 front = 3;   // front.anm：0x438730 神霊拾取动画（0x4c2190+0x2a8）
        std::array<i32, 2> teammate_player{};
    };
    // player_shot_sprite：无 .sht 会话的自机弹回退精灵（plXX.anm 内）。
    StagePresentation(AnmManager& animations, AnmRenderer& renderer, Banks banks,
                      i32 player_shot_sprite);
    ~StagePresentation();
    // 帧同步（Update pass 内调用）：敌机脚本接线、弹体/自机弹/自机姿态。
    bool sync(StageBattle& battle, u32 held);
    // 004424d0 invokes 00444df0 during player construction, before the first
    // replay update. This primes the player/options VM trees once; later sync
    // reuses the handles and only updates their positions.
    bool initialize_player_effects(StageBattle& battle);
    bool activate_player_effects(StageBattle& battle);
    // 0x445900 擦弹/得分特效（effect.anm script100，主链尾插 ordering=0）。
    bool graze_effect(const Vec3& position);
    // 0x42b6d0（0x42cd40 关终调用）：置 player+0x14698 bit3（自机隐藏位），
    // 对每个 option 槽的两个根 VM（0xa368 普通树 / 0xa36c 满P树）及其直接
    // 子体置 pending=3（label3），槽句柄保留；无立即 0x462450 tick。
    // pl03 script15/17 无 label3、无 -1 回落 → 下一主链 tick 只走
    // decrement/advance/tick，计时器不前进，卫星出生推迟一帧（原版关终帧
    // input 12184；本 port 在门首 input 12185 下发，代理成立条件见实现
    // 注释）。隐藏位同时使 sync_player_options 走 0x442960 隐藏支路。
    bool hide_player_options();
    // 0x42b6d0 隐藏位（player+0x14698 bit3 的移植等价）：clear 帧已下发时
    // 门（prepare_next_stage）不再重复 interrupt（原生 0x42cd40 只在 clear
    // 调 0x42b6d0，门 0x42bfa0 不调）。
    bool options_hidden() const noexcept { return player_effects_[0].options_hidden; }
    // 0x442960 隐藏支路（player+0x14698 bit3 且 +0x1469c>0x1d）：对每个
    // option 槽的两个根 VM 及其直接子体置 pending=1（label1）；原版清槽
    // active 标志 player+0xa2b8（不是根句柄），移植侧按 correctedPlan 等价
    // 清句柄并把 option 状态复位为“未建树”，使门内 sync 不重建、S2 的
    // 0x444df0 公共循环重建；幂等（计数每帧 >0x1d 都会重入）。原版门内
    // 第 30 帧 input 12214 触发。隐藏位保持到 0x441b00（activate_player_effects）
    // 清除。
    bool interrupt_player_options();
    bool reimu_bomb_orb_birth(const StageBattle& battle, i32 index);
    bool initialize_player_shot(PlayerShot& shot, u32 index, i32 seat = 0);
    bool tick_bullet(StageBullet& bullet, bool birth);
    bool initialize_bullet(StageBullet& bullet, i32 interrupt);
    bool bullet_cancel(i32 script,const Vec3& position,const Vec3& displacement,bool interpolate=true);
    // ECL 307/308 特效的指令级同步创建（0x41b510→0x4243e0：敌机 ECL 相位内
    // 模板拷贝+立即 tick。round-s15 帧序实证推翻"stage_script 相位后置"旧
    // 模型——推迟创建会使同帧多消费者的视觉 RNG 帧内顺序颠倒）。
    bool create_ecl_effect(i32 file, i32 script, const Vec3& position);
    // slots_only=true：只排空 kind=1 槽位请求（enemy_animation/中断回调的
    // 0x41fe80 指令级同步语义）；kind=0 请求保留入队。交互模式下 307/308
    // 已在指令处经 ecl_effect_create 同步创建，此处 kind=0 仅覆盖无宿主
    // 与激光/StageBattle 直入队请求。
    bool flush_ecl_animations(StageBattle& battle, StageEnemy* constructing = nullptr,
                              bool slots_only = false);
    bool enemy_death(i32 file,i32 script,const Vec3& position,float angle);
    bool enemy_fog(StageEnemy&,bool reset);
    void retire_enemy_fog(u32 serial);
    u32 player_effect(i32 script,const Vec3& position,i32 layer = -1) {
        return create_player_effect(banks.effect,script,layer,0,position);
    }
    bool beam_hit_effect(const Vec3& position, const Vec3& displacement, float angle, i32 seat = 0);
    // 非 const：死亡演出第 3 帧消费 StageBattle::death_option_rebuild（一次性
    // 边沿，见 StageBattle.hpp 注释），与 0x444df0 的“重建后清状态”同构。
    bool sync_player_options(StageBattle& battle, bool power_rebuild = false);
    // 0x42e380/0x438730 的 VM 面同步：生产在 Update 24/25 由 RunScene 调用
    // （与 StageBattle 的同名 phase 配对）；defer_item_side=false 时仍由
    // sync 内联，harness/单测行为不变。
    bool sync_items_phase(StageBattle& battle) { return sync_items(battle); }
    bool sync_item_spirits_phase(StageBattle& battle) {
        return sync_item_spirits(battle);
    }
    bool sync_youmu_slash(StageBattle& battle);
    bool youmu_charge_effect(i32 action, const Vec3& position, i32 seat = 0);
    bool spirit_animation_alive(u32 slot, u32 serial) const noexcept;
    // 0x438730 收集瞬间的 front.anm 拾取动画（0x46f820 flags=0 尾插、layer -1、
    // 位置=神霊位、anmArg=0；句柄原版丢弃）。失败置 error_ 并返回 false。
    bool spirit_pickup(i32 script, const Vec3& position);
    // 0x438ec0 出生瞬间建 VM（0x438ff9 effect.anm 0x38 先、0x43909d/
    // 0x4390be astral kind*6+1/4 后；均 layer -1、flags 0 → 0x46f3f0 尾插）。
    // astral 句柄写回 spirit_handles_[slot] 供 sync_item_spirits 复用。
    bool spirit_spawn_animation(u32 slot, u32 serial, i32 kind, u32 flags,
                                float x, float y, float z);
    // true = 敌机本体 VM 由 ECL 303/306 创建入池（0x41fe80/case3 同构），
    // sync 的敌机接线/清理段跳过。默认路径（2026-10-23 基座治理整改：
    // RunScene::initialize 置 true——敌机全走 AnmManager 句柄，th15
    // EnemyAnmHost 桥接模式的 th13 对应物）；false = 表现层手工敌机段
    // （双绘制回退路径，仅逃生开关）。
    bool enemies_from_ecl = false;
    // 直绘 pass：0=自机弹 1=自机 2=敌机 3=敌弹（调用方保证 Fullscreen 相机）。
    bool draw(u32 pass);
    void prepare_player_opacity();
    void release_stage();
    void release();  // 场景卸载：销毁敌机子树（外部 VM 归还 registry 外部表）
    void release_player_shots();
    // M6 快照：表现层手账的值快照（与 AnmRegistry::SnapshotData 同批次
    // 消费）。手账是「权威态 → 表现层」的句柄映射与自有 VM 值：
    //   * registry 句柄（shot_handles_/spirit_handles_/option 树/特效/
    //     fog_handles_ 等）——restore 后 registry 已按同句柄 generation
    //     重建池，句柄值语义不变；
    //   * 自有 VM（enemies_/bullets_/item_vms_/player_/shots_ 的
    //     unique_ptr<AnmVm>）——值拷贝，restore 后重绑槽内指针
    //     （sprite_source 指向槽自身的 Source；environment/object_host
    //     指向 animations 宿主——快照存活期对象身份不变，红线）；
    //   * 派生标量（script/color/generation/active/age 等手账字段）。
    // 快照不覆盖：error_（终态诊断）、stage_file_offsets_（资源派生，
    // stage_offsets_source_ 判定后惰性重建）。
    // 不快照手账的替代路线（reset_snapshot_caches 全清重建）已实证不可
    // 行：sync 重建走 fresh allocate 路径，restore 恢复的 registry VM 成
    // 孤儿滞留池（digest 含 registry → restore 后首帧即分歧），且新建
    // VM 的 age/RNG 消耗与连续基线不同源（M6 浏览器探针：302 vs 253）。
    // Every player owns an independent ANM handle family. Team overlays and
    // the registry retirement ledger remain shared by the world.
    struct PlayerEffectsState {
        std::array<u32, YoumuBomb::kSlots> youmu_slash_handles{};
        std::vector<u32> youmu_slash_extra;
        std::array<u32, 8> option_handles{}, full_option_handles{}, bomb_orb_handles{};
        std::array<bool, 8> bomb_orb_exploding{};
        std::array<u32, 5> bomb_ray_handles{};
        u32 focus_handle = 0;
        i32 option_character = -1, option_count = -1;
        bool option_anchor_deferred = false, option_rebuild_pending = false;
        bool options_hidden = false, death_options_queued = false, full_options = false;
        u32 bomb_generation = 0, bomb_background = 0, bomb_beam = 0;
    };
    struct SnapshotData {
        struct EnemyVisualData { u32 serial = 0; i32 script = -1; bool has_vm = false; AnmVm vm; };
        struct BulletSlotData { bool has_vm = false; AnmVm vm; i32 script = -1, color = -1, source_type = 0, source_color = 0; u32 generation = 0; bool active = false; };
        struct ShotFadeData { u32 handle = 0, generation = 0; float x = 0, y = 0, angle = 0, drift = 0; bool sine_moving = false; i32 age = 0; StageMover sine_motion; };
        std::vector<const AnmVm*> embedded_identities;
        std::vector<EnemyVisualData> enemies;
        std::deque<BulletSlotData> bullets;
        std::vector<u32> shot_handles;
        std::vector<AnmVm> shots;          // 回退精灵直贴 VM（script<0）
        std::vector<u32> shot_generations;
        std::vector<ShotFadeData> shot_fades;
        std::vector<u64> shot_faded_generations;  // 递增、去重的稳定快照
        bool has_player = false;
        AnmVm player;
        i32 player_script = -1, player_tilt = 0;
        std::array<std::optional<AnmVm>, 2> teammate_players{};
        std::array<i32, 2> teammate_tilts{};
        std::array<u32, ItemManager::kSpiritCapacity> spirit_handles{}, spirit_serials{};
        std::array<i32, ItemManager::kSpiritCapacity> spirit_scripts{};
        std::array<bool, ItemManager::kItemCapacity> item_vm_present{}, item_side_vm_present{};
        std::vector<AnmVm> item_vms, item_side_vms;   // 按 present 下标序
        std::array<u32, YoumuBomb::kSlots> youmu_slash_handles{};
        std::vector<u32> youmu_slash_extra;
        std::array<u32, 8> option_handles{}, full_option_handles{}, bomb_orb_handles{};
        std::array<bool, 8> bomb_orb_exploding{};
        std::array<u32, 5> bomb_ray_handles{};
        u32 trance_handle = 0;
        bool trance_was_active = false;
        std::vector<u32> bullet_cancel_handles;
        u32 focus_handle = 0;
        std::array<u32, 3> youmu_charge_handles{};
        std::unordered_map<u32, std::array<u32, 16>> fog_handles;
        i32 option_character = -1, option_count = -1;
        bool option_anchor_deferred = false, option_rebuild_pending = false;
        bool options_hidden = false, death_options_queued = false;
        bool full_options = false, frame_synced = false;
        u32 bomb_generation = 0, bomb_background = 0, bomb_beam = 0;
        std::vector<u32> player_effect_handles;
        std::array<PlayerEffectsState, 2> teammate_effects{};
    };
    bool capture_presentation(SnapshotData& out) const;
    // Recreate embedded VMs and return the old-to-new identity map so the
    // registry can restore their child chains afterward.
    bool restore_presentation(const SnapshotData& source,
                              std::unordered_map<const AnmVm*, AnmVm*>* remapped = nullptr);
    // M6 快照恢复：手账缓存重置（WorldState::restore 后由宿主调用）。
    // 敌机视觉缓存/弹体槽/自机弹句柄池/留尸池是「权威态 → 表现层」的
    // 派生缓存：restore 恢复权威态与 registry VM 池后，这些句柄映射手账
    // 可能指向发散期的悬空句柄（registry 已整池重建），继续沿用会令
    // sync 误判「VM 已回收」→ 权威弹被误退休（M6 实证：shots 计数错位
    // 且不收敛）。重置后下一帧 sync 按恢复后的权威态全量重建（与换关
    // 重建同语义）。option 树/自机 VM/玩家特效句柄不重置——restore 恢复
    // 同句柄 generation，registry find 仍解析到恢复后的 VM。
    void reset_snapshot_caches();
    // 安装主链收尾钩子并复位跨 RunScene 的 battle_/frame_synced_。构造时调用；
    // 换关时 preserve_presentation_from 把本对象移到新 RunScene 后必须重调，
    // 否则新 RunScene 构造时创建的临时 presentation 析构会清掉钩子，且
    // battle_ 会悬空指向已析构的旧 RunScene。
    void install_main_update_hook();
    const std::string& error() const { return error_; }
    // 画面诊断：敌机/敌弹/自机弹/自机/神霊 VM 清单（脚本/精灵/尺寸/缩放/位置）。
    std::string debug_report() const;
    // 调试直绘过滤：bit0..3 = 自机弹/自机/敌机/敌弹；默认全开。
    static u32 debug_pass_mask;
    // 只读测试访问：自持道具 VM（0x42f020 物品结构体内嵌、不入 0x46f170
    // 主链——契约见 sync_items 注释与 e34 四路 coreEqual 认证）。
    const AnmVm* item_body_vm(u32 slot) const { return item_vms_[slot].get(); }
    const AnmVm* item_side_vm(u32 slot) const { return item_side_vms_[slot].get(); }

private:
    std::unordered_map<u32,std::array<u32,16>> fog_handles_;
    struct EnemyVisual {
        u32 serial = 0;
        i32 script = -1;
        std::unique_ptr<AnmVm> vm;
    };
    // 神霊池视觉：槽位 ↔ registry 句柄一一对应（0x438730 槽 [0x16] 句柄域）。
    // 脚本 = kind*6+1（出生 bit0==0）/ kind*6+4（bit0==1，trance 期产件），
    // 合并 stage bank（enemy.anm 局部序＝合并表序，镜像对结构即原版双形态）；
    // 层 -1（脚本 ins_304 置 8）；出生位 = 钳制后 pos；逐帧同步 +0x574 语义
    //（0x438730 位移尾）；收集/回收即 retire（0x418330/0x422b20 的 0x20 隐藏
    // 等价呈现）。
    bool sync_item_spirits(StageBattle& battle);
    bool sync_trance_effect(const StageBattle&);
    u32 trance_handle_ = 0;
    std::vector<u32> bullet_cancel_handles_;
    bool trance_was_active_ = false;
    std::array<u32, ItemManager::kSpiritCapacity> spirit_handles_{};
    std::array<u32, ItemManager::kSpiritCapacity> spirit_serials_{};
    std::array<i32, ItemManager::kSpiritCapacity> spirit_scripts_{};
    // 道具池视觉（0x42f020 两次模板拷贝的运行时实证，r4/items
    // item_vm_observer：模板池按 bullet.anm 局部脚本号索引）：
    // type N(1..9) → VM1 = bullet.anm script 89+N（sprite 459+N 道具本体，
    // 脉冲族），VM2 = script 98+N（sprite 468+N 静态副体）；层 = 模板层 0。
    // 链归属（phase3 实证）：原版这两枚 VM 是**物品结构体内嵌 VM**
    // （0x42f020:60-63 连续两次 0x403800 模板拷贝 + 0x462450 首拍，落
    // item+0x10 / item+0x5b4；0x42e380:87/318/324 每帧直接 0x462450 步进），
    // **从不进入 0x46f170 主链**。故 port 也不得 registry.submit；改为
    // 自持 VM、sync_items 内手动 tick、主链 update 后 append_draw 入层桶。
    bool sync_items(StageBattle& battle);
    void append_item_draws();
    std::array<std::unique_ptr<AnmVm>, ItemManager::kItemCapacity> item_vms_{};
    std::array<std::unique_ptr<AnmVm>, ItemManager::kItemCapacity> item_side_vms_{};
    // 妖梦斩击池视觉（0x40cb50 生成支路 → pl03.anm script 19/20）：
    // slot 0..59 ↔ 句柄一一对应（ctx+0x610 anm id 区）；slot=-1（cursor
    // 越界写）= 仅视觉、无战斗槽，挂 youmu_slash_extra_。VM 生命由脚本自灭
    // （op1）→ set_vm(alive=false) 清槽；rotation.z 回填 = ins_119 抽后值。
    // file≥2 合并 stage bank 的文件内局部脚本号 → 合并全局号偏移
    //（0x4243e0 用 file_table[e.anm_file] 文件对象 + 局部脚本；append 保
    // 留文件局部 source_id，表内升序 → 回绕即文件边界）。
    const std::vector<i32>& stage_file_offsets();
    struct ScriptTarget { i32 bank = 0; i32 script = 0; };
    ScriptTarget resolve_script_target(i32 file, i32 script);
    i32 stage_script_global(i32 file, i32 script);
    std::vector<i32> stage_file_offsets_;
    const AnmResource* stage_offsets_source_ = nullptr;
    struct BulletSlot {
        std::unique_ptr<AnmVm> vm;
        i32 script = -1;
        i32 color = -1;  // 槽位当前弹色（变色即重绑 → 出生中断重放）
        u32 generation = 0;  // StageBullet birth identity; survives compaction
        bool active = false;
        // 0x4120b0 精灵解析源：参数 → 表[类型].色[颜色]
        struct Source final : AnmSpriteSource {
            i32 type = 0, color = 0;
            i32 sprite_index(i32 parameter) const noexcept override {
                return bullet_animation_parameter(type, color, parameter);
            }
        } source;
    };
    BulletSlot* bullet_slot(StageBullet& bullet);
    static void bullet_animation_geometry(StageBullet& bullet, const AnmVm& vm);
    AnmManager& animations;
    AnmRenderer& renderer;
    Banks banks;
    void create_color_step(AnmVm* vm);
    // 与管理器（=ECL sim）同一条 0x472c90 游戏侧 RNG：原版敌机/弹体/自机弹
    // VM 全在全局态 0x4dc31c 上抽签，独立种子会移动共享流（对拍实证）。
    GameRandom& random;
    std::vector<EnemyVisual> enemies_;
    // VMs retain a pointer to Slot::source; growing the pool must not move it.
    std::deque<BulletSlot> bullets_;
    // 自机弹 VM = registry 主链正式成员（0x445c40）：池分配 → 0x403800 模板
    // 拷贝 → 0x462450 首 tick → 0x46f3f0 主链尾插（flags=0）。此后每帧只写
    // 位姿（0x4464d0 LAB_00446619），tick 由 0x46f170 主链遍历完成
    //（AnmManager::update(false)，Application Update 34，晚于 sync 20）。
    // 句柄域与 battle.shots 下标一一对应；compaction 只搬句柄，VM 身份不变。
    std::vector<u32> shot_handles_;
    // 无 .sht 会话的回退精灵直贴 VM（script<0，无脚本、不入链、不 tick）。
    std::vector<std::unique_ptr<AnmVm>> shots_;
    std::vector<u32> shot_generations_;
    // 自机弹命中留尸池（0x446bf0 → 0x4464d0 state2 惰行段）：命中帧直写
    // VM+0x4a0=1（中断 label id1 = 弹型脚本后半 layer13 渐隐+自旋段），
    // 弹体转 dying：速度×0.125、z=0x3dcccccd(0.1)，VM 位姿回写死亡位；
    // 此后每帧惰行（0.01 量化）并回写 VM，直至脚本 op1 自灭（原版次帧
    // 0x46fb90 查无 VM → 槽位释放的同款时序）。出界死（0x470110）不入池。
    // 留尸 VM 仍是主链成员：中断/自灭都由链遍历推进，句柄失效即出池。
    struct ShotFade {
        u32 handle = 0;      // registry 句柄（主链成员，不归表现层所有）
        u32 generation = 0;  // 死亡弹代（shot_faded_generations_ 去重键）
        float x = 0, y = 0;  // 游戏坐标惰行位（0x4464d0 move + 0x473780 量化）
        float angle = 0;     // 惰行方向 = 死亡时弹向（oracle f5990-6010 恒定直线）
        float drift = 0;     // speed×0.125（0x446bf0:44 f32 乘）
        bool sine_moving = false;
        StageMover sine_motion;
        i32 age = 0;         // 留尸帧计数；0 = 命中当帧（不惰行，仅消费中断）
    };
    std::vector<ShotFade> shot_fades_;
    // 已入留尸池的弹代（防同一死亡被重复消费：battle 向量里的死弹要到下一次
    // step_shots 才被压实，若调用方在两次 step 之间重复 sync 仍会看到它）。
    // Keep every consumed generation. Group by the entire high word so
    // snapshot restore also retains keys outside the currently active seats.
    std::map<u32, std::vector<u32>> shot_faded_generations_;
    std::unique_ptr<AnmVm> player_;
    std::array<std::unique_ptr<AnmVm>, 2> teammate_players_{};
    std::array<i32, 2> teammate_tilts_{};
    // 00442960 / 00444df0：判定点和卫星本体是管理器中的真实 VM，
    // 不在直绘 pass 再画一次。位置只读战斗单一状态，生命周期由句柄池回收。
    std::array<PlayerEffectsState, 3> player_effects_{};
    std::array<u32, 3> youmu_charge_handles_{};
    std::vector<u32> player_effect_handles_;
    bool sync_all_player_effects(StageBattle&);
    bool sync_player_options(StageBattle&, bool power_rebuild, i32 seat);
    i32 player_bank(i32 seat) const { return seat ? banks.teammate_player[size_t(seat - 1)] : banks.player; }
    bool sync_player_effects(StageBattle&, i32 seat = -1);
    bool sync_bomb_effects(const StageBattle&, i32 seat);
    // 主链 update 之后（0x46f170 遍历完成）的收尾：四角几何快照（供下一
    // battle.step 的 0x446531 剔除用，须取 post-tick 的 VM 状态）、脚本自灭
    // 检出（0x4464d0 查无 VM 语义）与留尸出池。挂 AnmManager::after_main_update。
    bool post_chain_update();
    StageBattle* battle_ = nullptr;  // sync 绑定；仅主链 update 钩子读取
    bool frame_synced_ = false;      // sync 完成且钩子尚未消费（暂停/覆盖层跳过）
    u32 create_player_effect(i32 bank, i32 script, i32 layer, u32 ordering,
                             const Vec3& position);
    u32 create_player_effect(i32 bank, i32 script, i32 layer, u32 ordering,
                             const Vec3& position, u8 owner);
    bool end_player_effect(u32& handle, bool retire);
    i32 player_script_ = -1, player_tilt_ = 0;
    i32 player_shot_sprite_;
    std::string error_;
    bool fail(const std::string& why) {
        if (error_.empty()) error_ = why;
        return false;
    }
};

}  // namespace th13
