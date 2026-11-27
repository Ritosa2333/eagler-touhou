// th16 EnemyManager（DAT_004a6dc0，0x190）+ Enemy（0x574c）：
//   0x41b340 创建 / 0x41ae70 装 ECL、登记更新链 0x1a（0x41b4f0→0x41b3d0）
//   0x41aa70 生成（含立即 0x41d1e0） / 0x41d1e0→0x41d2e0 每帧（运动 0x41bb50 → ECL 0x473bc0 → 碰撞 0x41c330）
//   敌方 ECL 操作码 ≥300：0x41dcb0（本阶段覆盖生成/运动/判定/标志等 st01 前段所需）
#pragma once
#include "Bullet.hpp"
#include "Distortion.hpp"
#include "Ecl.hpp"
#include "Motion.hpp"
#include "Sound.hpp"
#include "Types.hpp"
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace th16 {

class AnmManager;
struct AnmFile;
struct AnmVm;
class Archive;
class EffectManager;
class Gui;
class StageManager;
class ItemManager;
class LaserManager;
struct GameGlobals;
class Player;
struct PlayerWorld;
struct Spellcard;

// Boss 阶段中断表项（Enemy+0x5284 起 ×8，步长 0x88；op 514 / 521 写，0x424f00 / 0x425010 查）
struct EnemyInterrupt {
    i32 life_threshold = -1;   // +0x00：血量降到该值以下时切到 sub（-1 = 空槽）
    i32 time_threshold = -1;   // +0x04：life_timer 到达该帧时切到 timeout_sub（op 514 arg2）
    std::string sub;           // +0x08（0x40 字节）
    std::string timeout_sub;   // +0x48（0x40 字节；op 521 可单独改写）
};

// 0x425870 / 0x4258b0：敌机位置插值器（0x68 字节；与 AnmVm 的 Interp<Vec3> 布局不同——current 在前，
// 另有 flags bit0 = 逐轴模式（op 434/435：x / y 各自取 axis_modes）；结束时非 7 / 0x11 模式返回 goal，否则 initial）
struct EnemyPosInterp {
    Vec3 current;         // +0x00
    Vec3 initial;         // +0x0c
    Vec3 goal;            // +0x18
    Vec3 bezier1;         // +0x24
    Vec3 bezier2;         // +0x30
    Timer timer;          // +0x3c
    i32 end_time = 0;     // +0x50
    i32 axis_modes[3]{};  // +0x54
    i32 mode = 0;         // +0x60
    u32 flags = 0;        // +0x64
};
static_assert(sizeof(EnemyPosInterp) == 0x68, "EnemyPosInterp 为 0x68 字节");

struct EnemySpawnArgs {
    Vec3 pos;
    float life = 0;          // +0x14；≥1000 → flags |= 0x40000000
    i32 score = 0;
    i32 item = 0;
    u32 mirror = 0;          // → flags bit 19
    // +0x20..+0x4c：生成者的 I0..I3 / F0..F3 / F4..F7（0x423050 抄自 EnemyInf+0x28c..0x2b8，0x41aa70 写入子机同位）
    i32 int_regs[4]{};
    float float_regs[4]{};
    float temp_regs[4]{};
};

class Enemy;

class Enemy final : public EclVariables, public EclThreadHost, public EclCommands {
public:
    Enemy* self = this;
    EclContext primary;
    std::list<std::unique_ptr<EclContext>> async_threads;
    EclProgram* program = nullptr;
    class EnemyManager* manager = nullptr;
    GameGlobals* globals = nullptr;
    Player* player = nullptr;
    Rng* rng = nullptr;

    u32 id = 0;                 // +0x5740
    u32 flags = 0;              // EnemyInf+0x4060 / Enemy+0x526c
    Vec3 pos;                   // EnemyInf+0x44
    MotionState motion1;        // EnemyInf+0x88
    MotionState motion2;        // EnemyInf+0xcc
    Vec3 hitbox;                // EnemyInf+0x110（op 500）
    Vec3 hurtbox;               // EnemyInf+0x118（op 501；x × 0.5 = 体碰撞半径）
    Timer invuln;               // EnemyInf+0x3ffc（生成时 = 2）
    Timer life_timer;           // 年龄计时（生成时 current=0x3c）
    i32 life = 0;               // EnemyInf+0x3f74 / Enemy+0x5180 / ECL -9954
    i32 life_max = 0;           // +0x3f78 / +0x5184
    i32 life_display = 0;       // +0x3f7c / +0x5188：距下一阶段阈值的血量（Boss 血条 / 低血闪烁阈值）
    i32 life_budget = 0;        // +0x3f80 / +0x518c（符卡 7 倍血量池）
    i32 phase_life = 0;         // +0x3f84 / +0x5190
    i32 damage_accum = 0;       // +0x3f88 / +0x5194：累计承受伤害（0x41a6d0 +0x14，无敌分支同样累加；op 1001 按此放出）
    u32 life_flags = 0;         // +0x3f8c / +0x5198（bit0 = 符卡 7 倍减伤；bit1 = 立即死亡）
    float damage_rate = 1.0f;   // +0x4038 / +0x5244（op 565；Bomb 中按比例减伤，0 时音效 0x24）
    i32 season_item_step = 0;   // +0x4058（op 1001 arg0：每累计该伤害放一个季节道具）
    i32 season_item_next = 0;   // +0x405c（下一次放出的累计伤害阈值）
    Timer hit_timer;            // +0x4024：受击后置 30 帧并逐帧回退（0x41c330 尾）
    Timer no_hit_timer;         // +0x4010（op 541）：> 0 时不判体碰撞，逐帧回退
    i32 score_bonus = 0;        // +0x452c / +0x5738（op 569）
    i32 score_bonus_spell = 0;  // +0x5734（生成时取 DAT_004a5798，op 524 改写：奖励归属的 spell_id）
    i32 bomb_script = 0;        // +0x4068（op 546 arg1：Bomb / 季节解放期间的主 VM 脚本）
    i32 bomb_script_restore = 0;// +0x406c（op 546 时的 main_script）
    // op 632 / 639 登记、op 637 立即调用的每帧钩子（DAT_004917b8 函数表：0 空 / 1 0x4252d0 / 2 0x4253b0）。
    // 0x41d2e0 在 ECL 执行后、碰撞判定前调用，返回非 0 则敌机移出列表（表内两项都返回 0）。
    enum class FrameHook : u8 {
        None = 0,
        TriggerBulletsNearPlayer = 1,  // 0x4252d0：F0 半径内 Ex 0x8000 参数 == 1 的子弹跳到 Ex 第 8 步（st06bs BossCard4）
        DrainPowerAndSeason = 2,       // 0x4253b0：火力与季节量表每帧归零 + 重建子机（st06bs BossCard6..9）
    };
    FrameHook frame_hook = FrameHook::None;  // +0x4518
    i32 frame_hook_flag = 0;                 // +0x451c（632 置 0 / 639 置 1；原版无读取方）
    static constexpr int kInterruptSlots = 8;
    EnemyInterrupt interrupts[kInterruptSlots];  // Enemy+0x5284
    // —— ANM VM 绑定（0x41dcb0 op 302..340 / 0x4233a0 / 0x423260）——
    static constexpr int kVmSlots = 16;        // ids 16 槽；偏移/父槽表只有前 14 槽参与每帧同步
    static constexpr int kVmSyncSlots = 14;
    u32 vm_ids[kVmSlots]{};                    // EnemyInf+0x124
    Vec3 vm_offsets[kVmSyncSlots]{};           // EnemyInf+0x164（op 320）
    i32 vm_parent_slot[kVmSyncSlots];          // EnemyInf+0x224（op 322；-1 = 直接跟随敌机）
    i32 anm_slot = 0;           // +0x264（op 302：EnemyManager anm 表下标）
    i32 anm_slot_main = 0;      // +0x268：主 VM 来源的 anm 表下标（朝向切换重建用）
    i32 main_script = 0;        // +0x26c：主 VM 脚本号
    i32 main_script_base = 0;   // +0x270：朝向脚本族基号（+1..+4 = 左右倾/回正）
    i32 sprite_direction = 0;   // +0x274：-1 左 / 0 正 / 1 右
    i32 anm_layer = 0;          // +0x278（op 550）
    i32 anm_layer_offset = 1;   // +0x27c（op 552；0x41b580 初值 1）：spawn 层 = 该值 + 7
    Vec3 hit_pos;               // EnemyInf+0x280：最后一次命中的伤害源坐标（0x445a30 出参）
    Vec3 delta;                 // EnemyInf+0x78：0x41c1f0 本帧位移（朝向判定 / 0x80 位 VM 的朝向角）
    float vm_rotation = 0;      // EnemyInf+0x120：上次写给 0x80 位 VM 的 rotation.z
    i32 death_anm_slot = 0;     // +0x3fec（op 323 arg0；生成首帧后默认 1 = effect.anm）
    i32 death_script = 0;       // +0x3fe8（op 323 arg1；≥0 时 0x41d520 生成死亡特效；0 → 0x41aa70 按主脚本取默认）
    EffectManager* effects = nullptr;          // DAT_004a6db8
    Timer season_drop_timer;    // EnemyInf+0x403c / Enemy+0x5248（op 1000 限时季节掉落计时）
    i32 season_drop_duration = 0x3c; // +0x4050 / +0x525c（op 1000 arg0）
    i32 season_drop_max = 0;    // +0x3fd0 / +0x51dc（op 1000 arg1；0x41d700 掉落表第 15 项 = 季节道具数）
    i32 season_drop_min = 1;    // +0x4054 / +0x5260（op 1000 arg2）
    i32 drop_item_type = 0;     // +0x3f90 / +0x519c（op 510：死亡/op 562 时先掉的单个道具）
    i32 drop_counts[15]{};      // +0x3f94..+0x3fcc（掉落表：道具种类 1..15 的数量）
    Vec2 drop_radius{32.0f, 32.0f};  // +0x3fd8 / +0x51e4（op 508；0x41b580 初值 32）
    std::string death_sub;      // +0x44d8（op 556：死亡时切到该子程序立刻跑一帧）
    // ECL 629（0x41dcb0 case 0x8c）Boss 畸变光环：17 × 17 网格把 RT1 场地画面扭曲后贴进 RT2（Distortion.hpp）
    std::unique_ptr<DistortionGrid> distortion;   // +0x44bc
    float distortion_radius_target = 0; // +0x44c4（arg0；<= 0 时不建网格）
    float distortion_radius = 0;        // +0x44c8（每帧 += 2 × 速度，直到目标半径；初值 16）
    u32 distortion_color = 0;           // +0x44cc（arg1：圈内顶点色，随强度向白过渡）
    float distortion_angle_a = 0;       // +0x44d0（逐顶点 += π/32 的横向抖动相位；整帧 += π/16 × 速度）
    float distortion_angle_b = 0;       // +0x44d4（逐顶点 −= π/64 的纵向抖动相位；整帧 += π/32 × 速度）
    i32 hit_flash_timer = 0;    // +0x3ff0（受击闪烁/音效节流 4 帧）
    i32 hit_sfx = -1;           // +0x3ff8
    i32 death_sfx = -1;         // +0x3fe4 / +0x51f0（(id & 1) + 3）
    i32 boss_slot = -1;
    float rate = 0;             // +0x14f0
    Vec2 sprite_size{};         // EnemyInf+0x3f58..0x3f5c：主 VM 的 |sprite_size.y×scale.y| / |sprite_size.x×scale.x|
    Vec2 bound_center{};        // EnemyInf+0x3f60..0x3f64（op 504）
    Vec2 bound_size{};          // EnemyInf+0x3f68..0x3f6c（op 504）
    i32 int_regs[4]{};          // EnemyInf+0x28c..0x298（-9985..-9982；生成时承自生成者）
    float float_regs[4]{};      // EnemyInf+0x29c..0x2a8（-9981..-9978）
    float temp_regs[4]{};       // EnemyInf+0x14b8..0x14c4（-9935..-9932）
    // 运动插值器（下标 0 = motion1 / 1 = motion2；0x41bb50 每帧推进）
    EnemyPosInterp pos_interp[2]{};     // EnemyInf+0x2f8 / +0x360（op 401/403/425/426/434–439）
    Interp<float> angle_interp[2]{};    // EnemyInf+0x3c8 / +0x428（op 405/407/412/413/441/443）
    Interp<float> speed_interp[2]{};    // EnemyInf+0x3f8 / +0x458（op 405/407/409/411/412/413/421/423/445/447）
    Interp<Vec2> radius_interp[2]{};    // EnemyInf+0x488 / +0x4cc：(radius, radius_speed)（op 409/411/421/423）
    Interp<Vec2> ellipse_interp[2]{};   // EnemyInf+0x510 / +0x554：(angle2, f2c)（op 421/423 写入；原版无处推进）

    // 弹幕发射器（ECL 600..641 / 526；0x41b580 整块清零）
    static constexpr int kShooterSlots = 16;
    EtShooter shooters[kShooterSlots];        // EnemyInf+0x598（0x380 × 16）
    Vec3 shooter_offset[kShooterSlots];       // EnemyInf+0x3dd8（ECL 603 / 626 相对偏移）
    Vec3 shooter_pos2[kShooterSlots];         // EnemyInf+0x3e98（ECL 628 绝对坐标；z > 0.9 表示启用）
    i32 shooter_ex_counter[kShooterSlots]{};  // EnemyInf+0x3d98（ECL 611 / 612 递增下标）
    float safe_radius_sq = 0;                 // EnemyInf+0x4074（ECL 526：发射时自机近身安全半径²）
    BulletManager* bullets = nullptr;         // DAT_004a6dac
    ItemManager* items = nullptr;             // DAT_004a6ddc
    std::string error;
    bool alive = true;

    const char* routine_name() const noexcept;

    // EnemyManager+0x164 anm 表：0 → bullet.anm（槽 7）、1 → effect.anm（槽 8）、i ≥ 2 → ECL ANIM i-2（槽 8+i）
    AnmFile* anm_file(i32 index) const noexcept;
    // 0x46efa0 + 失效清零：找不到时 vm_ids[slot] = 0
    AnmVm* vm_at(int slot) noexcept;
    // op 303 / 306 / 313 / 316 / 318 公共体：杀旧 VM，用 anm_file(source) 的 script 在 anm_layer_offset+7 层
    // 生成（0x426160 链表头）到 vm_ids[slot]；full 时 slot 0 同步 sprite_size、flags & 0x20 隐藏整棵子树
    AnmVm* rebind_vm(int slot, i32 source, i32 script, bool full);
    // 0x41ba10：移出列表时先注销 Boss 槽，再杀掉全部绑定 VM
    void destroy();
    // 0x41d700：按掉落表散布道具（visual_rng 调用序与原版一致），随后清表
    void scatter_items();
    // 0x41a720：先掉 drop_item_type（op 510），再 scatter_items()
    void release_items();
    // op 509 / 0x41d520 共用：按限时计时折算季节道具数到 season_drop_max
    void settle_season_drop();
    // 全局速度 DAT_004a5788 的实时值（见 Enemy::update）；没有管理器时用调用方给的值
    float live_speed(float fallback) const noexcept;
    const float* live_speed_ptr(float& fallback) const noexcept;
    // 0x41d520：死亡结算（音效、死亡特效、掉落、死亡子程序）；返回后敌机移出列表
    void die(float game_speed);
    // 0x41c330：受击 / 阶段中断 / 死亡 / 体碰撞 / 闪烁；返回 0 继续，非 0 → 敌机移出列表
    int resolve_hits(float game_speed);
    // 0x41cbd0：每帧重建畸变网格并按到 Boss 的距离位移 / 着色顶点（在 resolve_hits 之后、sync_vm_offsets 之前）
    void update_distortion(float game_speed);
    // 0x424f00：按 interrupts 的血量阈值切阶段；返回要切入的子程序（空 = 无）并刷新 life_display / phase_life
    const std::string* life_interrupt();
    // 0x425010：按 interrupts 的时间阈值切阶段（并结算 Spellcard 超时）；返回要切入的子程序
    const std::string* time_interrupt();
    // 0x424f00 / 0x425010 / 0x41c330 共用：清线程、复位主上下文到 sub 并立即执行一帧；返回 false = ECL 结束
    bool switch_routine(const std::string& sub, float game_speed);
    // 0x424e40：登记阶段中断（life 阈值 / 时间阈值 / 子程序名，timeout_sub 同名）
    void set_interrupt(int slot, i32 life_threshold, i32 time_threshold, const char* sub);
    // 0x41bb50 尾：flags & 0x100000 时按 delta.x 切换朝向脚本，并从主 VM 回读 sprite_size
    void sync_sprite_direction();
    // 0x41c1f0：pos = motion1.pos + motion2.pos 的本帧位移积分（delta 记录、厘米取整、0x20000 场地夹紧）
    void apply_motion_position();
    void apply_default_death_effect();
    // 0x41d2e0 后段：按 vm_offsets / 父槽同步各 VM 的 entity_offset（0x80 位 VM 带朝向角）
    void sync_vm_offsets();
    // 0x41c330 尾：主 VM 的受击 / 低血闪烁（color2 + flags_lo 0x20000 位）与受击音效
    void update_hit_flash();
    // DAT_004917b8 函数表项；返回非 0 → 敌机移出列表
    int run_frame_hook(FrameHook hook);
    // ECL 502 / 503：对全部 VM 槽的子树应用 hide_tree / show_tree
    void for_each_vm_tree(void (*apply)(AnmVm&));

    // ECL 601 / 702 / 703 / 711 / 713 共用：发射器实际发射坐标（ECL 628 绝对坐标优先，否则敌机坐标 + 偏移）
    Vec3 shooter_origin(int slot) const noexcept;
    // EclVariables：0x423810 / 0x423f80（先覆盖 st01 前段用到的）
    float angle_to_player(const Vec3& from) const noexcept;
    Enemy* boss(int slot) const;
    bool integer(i32 id, i32& value) override;
    bool floating(i32 id, float& value) override;
    i32* integer_destination(i32 id) override;
    float* float_destination(i32 id) override;

    bool spawn(EclContext& source, i32 id, u32 skipped_arguments) override;
    EclContext* find(i32 id) override;
    void terminate_children() override;
    int execute(EclContext& context, u16 opcode) override;

    void reset_context(EclContext& ctx, i32 routine);
    bool init(EnemyManager& mgr, EclProgram& program, const char* sub, const EnemySpawnArgs& args,
              GameGlobals& globals, Player* player, Rng& rng, u8 difficulty);
    // 0x41d1e0 / 0x41d2e0：返回 false → 应销毁
    bool update(float game_speed);
};

class EnemyManager final : public EclResourceProvider {
public:
    EclProgram program;
    std::list<std::unique_ptr<Enemy>> enemies;
    u32 next_id = 1;            // +0x90
    u32 last_id = 0;            // +0x94：最近一次生成的敌机 id（ECL -9931）
    i32 enemy_count = 0;        // +0x18c
    Timer timer;
    i32 enemy_control = 99999;  // +0x8c
    i32 global_ints[4]{};       // +0x0c..0x18（-9926..-9923）
    float global_floats[8]{};   // +0x1c..0x38（-9922..-9915）
    i32 counters[3]{};          // +0x3c..0x44（-9949..-9947）
    u32 boss_ids[3]{};          // +0x48..0x50（0x41a910 写 / 0x417580 查）
    bool hud_hidden = false;    // +0x88 bit0（ECL 642 / 0x41a950）：Gui 不显示 Boss 血条 / 计时
    Spellcard* spellcard = nullptr;    // DAT_004a6db0（GameSession 持有）
    EffectManager* effects = nullptr;  // DAT_004a6db8
    i32 damage_dealt = 0;       // +0xb0：自机对敌总伤害
    i32 damage_dealt_bomb = 0;  // +0xac：炸弹伤害（命中 flags&4 的伤害源时按血量折算）
    bool bomb_active = false;   // DAT_004a6da8+0x30 == 1（Bomb 接入前恒 false）
    bool release_active = false;// DAT_004a6da4+0x30 == 1（季节解放接入前恒 false）
    const PlayerWorld* world = nullptr;  // Gui+0x1c8 != 0 的镜像（对话中伤害为 0、不判体碰撞；ECL 518 当帧即生效）
    Gui* gui = nullptr;                  // DAT_004a6dcc：ECL 518 / 519 / 554
    class ScorePopupManager* score_popups = nullptr;   // DAT_004a6f10：ECL 638 得分飘字（0x44a460）
    StageManager* stage = nullptr;       // DAT_004a6da0：ECL 630 背景脚本标签跳转（换关后由 GameSession 重新绑定）
    const bool* render_targets_ready = nullptr;   // DAT_004c127c：ECL 629 畸变网格只在渲染目标可用时建 VM
    bool dialogue_active() const noexcept;
    // Enemy 触发的 0x45e1f0 音效（受击 0x22/0x23、死亡 3/4、ECL 516）。原版与 ECL 601 等经 BulletManager 登记的音效
    // 同在敌机链内按调用序写入声音队列；GameSession 把 sfx_sink 指向帧级汇总表以保持这一顺序
    std::vector<i32> sfx_queue;
    SoundRequests* sfx_sink = nullptr;
    void play_sfx(i32 id, float x = 0.0f) { if (sfx_sink != nullptr) sfx_sink->push(id, x); else sfx_queue.push_back(id); }
    // ECL 517 → 0x45d150(1, duration, from, to)：宿主在敌机链之后登记为链 0x13 的 ScreenShake（下一帧起生效）
    struct ShakeRequest { i32 duration; i32 amp_from; i32 amp_to; };
    std::vector<ShakeRequest> shake_requests;
    // 0x41c890 执行到尚未还原的 ECL 操作码时记录（去重）；harness game-digest 末尾打印，用于按关卡清点缺口
    std::vector<i32> unknown_ops;
    float* global_game_speed = nullptr;  // DAT_004a5788（ECL 547 写入）
    BulletManager* bullets = nullptr;  // DAT_004a6dac（GameSession 在 BulletManager 创建后绑定）
    ItemManager* items = nullptr;      // DAT_004a6ddc（同上；道具掉落 0x41a720 / 0x41d700 / op 1001 直接生成）
    LaserManager* lasers = nullptr;    // DAT_004a6ee0（ECL 700..714 / 545 / 518 / 613 / 615 / 616 / 635 / 636）
    AnmManager* animations = nullptr;
    GameGlobals* globals = nullptr;
    Player* player = nullptr;
    // EnemyManager+0x164 ANM 表：0 → bullet.anm（slot 7）、1 → effect.anm（slot 8）、i≥2 → ECL ANIM i-2（slot 8+i）
    AnmFile* anm_file(i32 index) const noexcept;
    Rng* rng = nullptr;
    Archive* archive = nullptr;  // 前向：用函数注入读档
    std::string error;
    using ReadFn = bool (*)(void*, const std::string&, std::vector<u8>&);
    void* read_user = nullptr;
    ReadFn read_fn = nullptr;
    using AnmFn = bool (*)(void*, u32, const std::string&);
    void* anm_user = nullptr;
    AnmFn anm_fn = nullptr;

    bool read(const std::string& name, std::vector<u8>& data) override {
        return read_fn && read_fn(read_user, name, data);
    }
    bool animation(u32 slot, const std::string& name) override {
        return !anm_fn || anm_fn(anm_user, slot, name);
    }

    // 0x41b340 / 0x41ae70
    bool create(const char* ecl_name, GameGlobals& globals, Player* player, Rng& rng,
                AnmManager& animations, u8 difficulty);
    // 0x42ca00：同一关重开时 0x42cb60 代替 0x41b340 调用——只把管理器计时（+0x98）重置为开局态；
    // id 计数 / ECL 全局变量 / HUD 隐藏位 / 敌机控制值都沿用
    void restart_timer() { timer.reset(); }
    // 0x4185b0：ECL ANIM 槽（10..15）的 VM 全部标删、敌机全部析构（0x41ba10）、Boss 槽清零
    void reset();
    // 0x41b1a0：reset 后卸载 ECL ANIM 槽的 ANM 文件（0x46d770）；换关时 0x42d200 走此路，随后 0x41b340 重建
    void destroy();
    // 0x42dc50 段：生成 "main"
    bool spawn_main();
    // 0x41b3d0
    void update(float game_speed);
    Enemy* spawn_enemy(const char* sub, const EnemySpawnArgs& args);
    Enemy* find_id(u32 id) noexcept;
    // 0x41b540：按 id 找敌机；原版遍历到表尾仍未命中时返回的是最后一只敌机而非空（灵梦阴阳玉追踪 0x410550 依赖此行为）
    Enemy* find_id_or_last(u32 id) noexcept;
    // 0x417580：Boss 槽查找（id 为 0 → 空；未命中 → 链表最后一只，沿用原版口径）
    Enemy* boss_slot(int slot) noexcept { return find_id_or_last(boss_ids[slot]); }
    // 0x425240：pos 周围 range 内、flags & 0xc000021 == 0 的最近敌机 id（没有则 0）——自机追踪弹 / 灵梦阴阳玉索敌
    u32 nearest_targetable_id(const Vec3& pos, float range) const noexcept;
    // 0x41d900 / 0x41da30 / 0x41db70（ECL 525 / 551 / 571）：清掉落表后让非 Boss 族敌机全部死亡（下一帧移出）
    // mode 1 只杀 anm_layer == *filter 的敌机；mode 2 另清死亡子程序
    void kill_all(int mode, const i32* filter);
};

}  // namespace th16
