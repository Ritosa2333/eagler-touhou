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
#include <unordered_set>
#include <vector>

namespace th13 {

class StagePresentation {
public:
    struct Banks {
        i32 stage = 0, bullet = 0, player = 0;
        i32 effect = 8;  // effect.anm（mgr 槽 8；ECL 文件索引 1 的特效创建源）
        i32 spirit = 25; // 00405530: astral.anm, original resource slot25.
    };
    // player_shot_sprite：无 .sht 会话的自机弹回退精灵（plXX.anm 内）。
    StagePresentation(AnmManager& animations, AnmRenderer& renderer, Banks banks,
                      i32 player_shot_sprite);
    // 帧同步（Update pass 内调用）：敌机脚本接线、弹体/自机弹/自机姿态。
    bool sync(StageBattle& battle, u32 held);
    bool tick_bullet(StageBullet& bullet, bool birth);
    bool bullet_cancel(i32 script,const Vec3& position,const Vec3& displacement,bool interpolate=true);
    bool flush_ecl_animations(StageBattle& battle, StageEnemy* constructing = nullptr);
    bool enemy_death(i32 file,i32 script,const Vec3& position,float angle);
    bool enemy_fog(StageEnemy&,bool reset);
    void retire_enemy_fog(u32 serial);
    u32 player_effect(i32 script,const Vec3& position,i32 layer = -1) {
        return create_player_effect(banks.effect,script,layer,0,position);
    }
    bool spirit_animation_alive(u32 slot, u32 serial) const noexcept;
    // true = 敌机本体 VM 由 ECL 303/306 创建入池（0x41fe80/case3 同构），
    // sync 的敌机接线/清理段跳过。默认路径（2026-10-23 基座治理整改：
    // RunScene::initialize 置 true——敌机全走 AnmManager 句柄，th15
    // EnemyAnmHost 桥接模式的 th13 对应物）；false = 表现层手工敌机段
    // （双绘制回退路径，仅逃生开关）。
    bool enemies_from_ecl = false;
    // 直绘 pass：0=自机弹 1=自机 2=敌机 3=敌弹（调用方保证 Fullscreen 相机）。
    bool draw(u32 pass);
    void release_stage();
    void release();  // 场景卸载：销毁敌机子树（外部 VM 归还 registry 外部表）
    const std::string& error() const { return error_; }
    // 画面诊断：敌机/敌弹/自机弹/自机/神霊 VM 清单（脚本/精灵/尺寸/缩放/位置）。
    std::string debug_report() const;
    // 调试直绘过滤：bit0..3 = 自机弹/自机/敌机/敌弹；默认全开。
    static u32 debug_pass_mask;

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
    bool sync_items(StageBattle& battle);
    std::array<u32, ItemManager::kItemCapacity> item_handles_{};
    std::array<u32, ItemManager::kItemCapacity> item_side_handles_{};
    // 妖梦斩击池视觉（0x40cb50 生成支路 → pl03.anm script 19/20）：
    // slot 0..59 ↔ 句柄一一对应（ctx+0x610 anm id 区）；slot=-1（cursor
    // 越界写）= 仅视觉、无战斗槽，挂 youmu_slash_extra_。VM 生命由脚本自灭
    // （op1）→ set_vm(alive=false) 清槽；rotation.z 回填 = ins_119 抽后值。
    bool sync_youmu_slash(StageBattle& battle);
    std::array<u32, YoumuBomb::kSlots> youmu_slash_handles_{};
    std::vector<u32> youmu_slash_extra_;
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
    std::vector<std::unique_ptr<AnmVm>> shots_;
    std::vector<u32> shot_generations_;
    // 自机弹命中留尸池（0x446bf0 → 0x4464d0 state2 惰行段）：命中帧直写
    // VM+0x4a0=1（中断 label id1 = 弹型脚本后半 layer13 渐隐+自旋段），
    // 弹体转 dying：速度×0.125、z=0x3dcccccd(0.1)，VM 位姿回写死亡位；
    // 此后每帧惰行（0.01 量化）并回写 VM，直至脚本 op1 自灭（原版次帧
    // 0x446fb90 查无 VM → 槽位释放的同款时序）。出界死（0x470110）不入池。
    struct ShotFade {
        std::unique_ptr<AnmVm> vm;
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
    std::unordered_set<u32> shot_faded_generations_;
    std::unique_ptr<AnmVm> player_;
    // 00442960 / 00444df0：判定点和卫星本体是管理器中的真实 VM，
    // 不在直绘 pass 再画一次。位置只读战斗单一状态，生命周期由句柄池回收。
    u32 focus_handle_ = 0;
    std::array<u32, 8> option_handles_{}, full_option_handles_{}, bomb_orb_handles_{};
    std::array<bool, 8> bomb_orb_exploding_{};
    u32 bomb_generation_=0,bomb_background_=0,bomb_beam_=0;
    std::array<u32,5> bomb_ray_handles_{};
    u32 youmu_charge_handle_ = 0;
    bool youmu_charge_ready_ = false;
    std::vector<u32> player_effect_handles_;
    i32 option_character_ = -1, option_count_ = -1;
    bool full_options_ = false;
    bool sync_player_effects(const StageBattle&);
    bool sync_bomb_effects(const StageBattle&);
    u32 create_player_effect(i32 bank, i32 script, i32 layer, u32 ordering,
                             const Vec3& position);
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
