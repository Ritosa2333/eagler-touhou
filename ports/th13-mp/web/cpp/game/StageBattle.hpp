// 对局战斗核心（回放对拍 + 可玩化）：自机模型 + StageSim 交互步进 + 碰撞/结算。
// ECL/敌机/弹幕推进全部经 StageSim（oracle 契约路径不变，交互模式见
// StageSim::begin）；本模块做「自机/自机弹/碰撞/奖惩」。
//
// 数值口径（event-contract.md §7.2，与 oracle python 孪生同式）：
//   移动 = 原版 0x442960 定点模型——pos(1/128) += speed128 × dir(单位向量表
//   0x4abc30，斜向不归一)，speed128 四选一（focus × 斜向），出自 pl02a.sht
//   （+0x10..+0x1c 浮点 ×128 取整：4.5/2.0/3.1819805/1.4142135 →
//   576/256/407/181），钳制 x∈[-23552,23552]、y∈[4096,55296]，初态 (0,51200)。
//   自机弹出 .sht 弹型域（0x446000/0x445c40 原版语义：帧%间隔==相位出膛、
//   伤害=记录+2、弹速=记录+0x18、动画=记录+0x22 再加 10；
//   块=功率/100+聚焦×5；回放口径功率恒初值 100）。判定半径
//   为定标值【待校准】，AABB 判定对齐 0x446870。全部整数/整值浮点运算，
//   两侧无舍入差（与 oracle/replay_model.py 孪生逐字段同式）。
#pragma once

#include "GameInput.hpp"
#include "ItemManager.hpp"
#include "SessionState.hpp"
#include "StageSim.hpp"
#include "YoumuBomb.hpp"

#include <array>

namespace th13 {

class AnmRegistry;

namespace multiplayer { class WorldState; struct StageEntry; }

// 诊断开关（不参与逻辑）：由 th13_debug_damage_probe() 打开，
// 打印 0x446870/0x419a90 的每次注册项伤害贡献。打印窗口默认 7970..7985
// （round-16 S5 判例帧），可由 th13_debug_damage_probe_window() 覆盖
// （round-19 调试脚手架扩展，不改任何比较语义）。
extern bool damage_probe_enabled;
extern int damage_probe_first_frame;
extern int damage_probe_last_frame;

struct StageBattleServices {
    virtual ~StageBattleServices() = default;
    virtual bool life_hud(i32 lives, i32 pieces) = 0;
    virtual bool bomb_hud(i32 bombs, i32 pieces) = 0;
    // M5：席位感知 HUD 刷新（MP 每席图标集；seat 0 /单席回落单套面）。
    // 默认实现忽略席位，与旧宿主/测试 stub 完全兼容。
    virtual bool life_hud_seat(i32 seat, i32 lives, i32 pieces) {
        (void)seat;
        return life_hud(lives, pieces);
    }
    virtual bool bomb_hud_seat(i32 seat, i32 bombs, i32 pieces) {
        (void)seat;
        return bomb_hud(bombs, pieces);
    }
    virtual bool request_game_over() = 0;
    // 443240 distinguishes Replay death from ordinary Game Over. The host
    // owns Replay mode; other callers of request_game_over keep their route.
    virtual bool request_player_game_over() { return request_game_over(); }
    virtual bool hud_sound(i32 id) = 0;
    virtual bool hud_sound(i32 id, float x) { (void)x; return hud_sound(id); }
    virtual bool stop_sound(i32 id) { (void)id; return true; }
    virtual bool set_voice_pan(i32 id, float x) { (void)id; (void)x; return true; }
    virtual bool trance_music(bool active) { (void)active; return true; }
    // 0x42b030 → HUD script172 标签无条件重写（Update pass 内调用）。
    // 默认空实现：无头/无 HUD 测试路径不失败。
    virtual bool trance_gauge_refresh() { return true; }
    // 0x42a439 = jmp dword ptr [eax*4+0x42abd4]（入口 0x42a430，jmp 在 +9）。
    // case2（0x42ee28 小P满灵力 / 0x43d23e 大P满灵力，mov eax,2）→ front61；
    // case4（0x43d3a6，FUN_0043d330 碎片达标内，mov eax,4）→ front63(EXTENDED)。
    // 全 EXE 可达 case 仅 0/1/2/4；case3(front62) 在 0x42a41c 后的不可达块、
    // case5 是共享 epilogue、case6(front64) 无调用方，故不在此列。
    // case0/1 由符卡结算面处理；1UP（0x43d2f0）只 +1 命、无 0x11/横幅。
    // 参数 = front.anm 脚本号。
    // 默认返回 true：所有无头/单测夹具行为不变（纯改善前提）。
    virtual bool hud_notice(i32 front_script) { (void)front_script; return true; }
    // 0x43d530：灵界值 be808 每跨 200 一整槽，level = be808/200 ∈ 1..3 →
    // FUN_0046f820(..., level+0x6b, ...) 建 front 108/109/110（×1/×2/×MAX）。
    // x/y/z = 自机世界坐标（0043d530.c:24-30 取 DAT_004c22c4+0x5b8/+0x5bc/
    // +0x5c0 并 y-32，作为 FUN_0046f820 的 param_3）；front108-110 无
    // ins_400 绝对坐标，位置必须由宿主传入。
    // 默认返回 true：无头/单测夹具行为不变（纯改善前提）。
    virtual bool hud_spirit_notice(i32 level, float x, float y, float z) {
        (void)level; (void)x; (void)y; (void)z; return true;
    }
    // 0x44ad60：道具得分弹字入池（value/位置/颜色）。默认空实现。
    virtual bool score_popup(i32 value, float x, float y, u32 color) {
        (void)value; (void)x; (void)y; (void)color;
        return true;
    }
    // 敌机死亡回收钩（0x418e00 销毁支路的 ANM 侧）：宿主回收该敌机全部
    // 槽位 VM（enemies_from_ecl 路径句柄归零防泄漏）。默认空实现——
    // 无头契约路径（stage1/replay 事件窗）无 ANM 面。
    virtual void enemy_retire(u32 serial) { (void)serial; }
};

// 自机弹：出生时补偿初速度，含 age0 的每帧经回调、速度重算、移动和 0.01 量化。
struct PlayerShot {
    float x = 0, y = 0, damage = 0;
    float angle = -1.5707964f;  // 弹向（渲染旋转用；主弹恒 -π/2 直上）
    i32 script = -1;            // plXX.anm 弹型脚本（.sht 记录；-1 = 精灵直贴回退）
    bool alive = false;
    // 446bf0 generic hit sets +0x34=0.1f; 4474b0/4475b0/4471f0 do not.
    bool hit_fade = false;
    i32 state = 1;              // 445c40 +70: 1 active, 2 hit fade, 0 retired.
    bool animation_finished = false; // VM termination observed by presentation.
    float speed = 12.0f;       // 无 SHT 的旧回退；生产弹从 SHT +0x18 取值。
    float width = 8.0f, height = 8.0f;
    float beam_visual_width = 0; // 446f20 writes VM before enemy hit shrinks width.
    i32 age = 0, behavior = 0, type = 0;
    i32 anchor = 0, anchor_power = 0;
    float target_angle = 0, damage_x = 0, damage_y = 0;
    Timer age_clock;
    //4457e0 registration+70..80;443240 decrements before firing new shots.
    Timer damage_clock{9999998, 9999999, 9999999.f, 0, 1};
    u32 target_serial = 0;
    u32 generation = 0;
    i32 hit_behavior = 0;
    bool sine_moving = false;
    StageMover sine_motion; // 447420 initializer; remains active after 4474b0 hit
    // 445c40 installs one independent 4457e0 damage registration for every
    // live shot.  The slot is an implementation handle only; the pool owns
    // the geometry and timer, so vector compaction cannot retarget a hit.
    i32 registration_slot = -1;
    // 446f20/447230 latch beam contact across frames to interrupt its VM
    // only on transitions into or out of enemy contact.
    bool beam_contact = false, beam_contact_seen = false;
    i32 beam_interrupt = 0;
    // M2：发射席位（射击事件归属；单席 = 0）。发射时取当前活动席。
    i32 owner_seat = 0;
};

// 4457e0/446870 registration record.  The retail player object reserves 256
// records at +0xa9e4, each 0x9c bytes.  Only the fields consumed by the
// collision path are represented here; presentation handles remain on the
// PlayerShot/ANM side.
struct DamageRegistration {
    enum class Owner { Shot, Area };
    bool active = false;
    u32 flags = 0; // bit0 = enabled; bit1 = circular (unused by shot pool)
    Owner owner = Owner::Shot;
    i32 orb = -1;
    float radius = 0, radius_growth = 0;
    float center_x = 0, center_y = 0, center_z = 0;
    // 4475b0 copies its projectile mover into the new circular area after
    // setting speed2. Other4456e0/4457e0 allocations start stationary.
    float travel_speed = 0, travel_angle = 0;
    float width = 0, height = 0, angle = 0;
    Timer clock{9999998, 9999999, 9999999.f, 0, 1};
    i32 interval = 1;
    float damage = 0;
    i32 accumulated_damage = 0;
    i32 threshold = 999999;
    u32 generation = 0;
    // M2：owner 席位（伤害/表现事件归属；单席 = 0）。注册时取当前
    // 活动席，席位交换随 StagePlayerState 整体携带。
    i32 owner_seat = 0;
};

// Presentation-owned ANM geometry snapshot. StagePresentation computes these
// with the same anm_quad_positions() routine used by AnmRenderer; StageBattle
// consumes the previous frame's snapshot before enemy update/collision.
struct PlayerShotGeometrySnapshot {
    bool valid = false;
    i32 slot = -1;
    i32 age = 0;
    i32 script = -1;
    u32 generation = 0;
    std::array<Vec3, 4> quad{};
};

// 生产战斗和表现层共用的卫星/梦想封印状态，坐标均为游戏坐标。
struct PlayerOption {
    float x = 0, y = 0, angle = -1.5707964f;
    bool active = false;
};
struct PlayerBombOrb {
    float x = 0, y = 0, angle = 0;
    bool active = false, exploding = false;
    i32 explosion_frame = 0;
    Timer explosion_damage_clock{10, 11, 11.f, 0, 1}; // 4456e0 registration lifetime
    float radius = 0, speed = 0;
    float velocity_x = 0, velocity_y = 0;
    float accumulated_damage = 0;
    i32 registration_slot = -1;
    // 40ae30 owns the trajectory; caller supplies the current target, if any.
    void step(i32 age,i32 index,float player_x,float player_y,const StageEnemy* target);
};

// 命中扫描用的敌机快照（每帧 sim.step_frame 之前采集）：原版 0x419a90 的
// 自机弹扫描（0x446870）发生在敌机 0x419450 积分之前，故 G 帧命中判定用
// G-1 帧末位置/受击尺寸（G 状态流 th13_01 实证：旧序提前一帧命中）。
struct EnemyScanState {
    u32 serial = 0;
    float x = 0, y = 0;
    float hurtbox[2] = {0, 0};
};

// .sht 弹型记录（0x443e40 装载解析：+0xc8 指针表 → +0xf4 基准块，记录
// 步长 0x38、首字节 <0 终止）。字段序 = 文件布局（0x446000/0x445c40 直读）：
// +0 u8 间隔（帧 % 间隔 == 相位才出膛，0x446000:24；pl00a 主弹=3、卫星=8）、
// +1 u8 相位、+2 i16 伤害、+4/+8 f32 {x,y} 出膛偏移（anchor≠0 时相对卫星，
// 卫星模型分期）、+0x14 f32 角度、+0x18 f32 弹速、+0x20 i8 卫星锚位、
// +0x22 i16 动画编号（实际 ANM 脚本 = 编号 + 10，00445e0c–00445e47）。
struct ShtShotRecord {
    i32 interval = 0;  // 发射间隔（= 首字节，兼类别：3 主弹 / 8 卫星弹）
    i32 phase = 0;     // 发射相位（帧 % interval == phase 出膛）
    i32 script = 0;    // plXX.anm 脚本号
    i32 anchor = 0;    // 卫星锚位（0 = 自机；≠0 = Option 卫星，分期）
    float x = 0, y = 0;
    float angle = 0;
    float damage = 0;
    float speed = 0;
    float width = 0, height = 0;
    i32 behavior = 0, type = 0;
    i32 sound = -1; // SHT record +0x24, 445c40 positional shot sound.
    i32 initializer = 0, hit_behavior = 0;
    float motion_phase = 0;
};

// DAT_004c22c4 survives destination0xc (0042bfa0). Stage-local ECL,
// bullet and item storage can be replaced without constructing a new player.
//
// M2（一套世界、多席自机）：本结构是**每席玩家的完整战斗面**。原 StageBattle
// 的散装玩家域（Bomb/respawn/YoumuBomb/输入/touch/SHT 参数/角色/灵界个人
// 侧标志）全部迁入此处，使席位切换 = 一次 StagePlayerState 交换；StageBattle
// 的引用别名（player_x/shots/damage_registrations/…）始终绑定本结构的同一
// 存储实例，交换内容即切换席位，单席路径字节级不变（M2 门：单席等价）。
// 共享世界（StageSim/ItemManager/SpellState/双 RNG/团队灵界槽）不在本结构。
struct StagePlayerState {
    enum class Phase : i32 { Entering = 0, Active = 1, Dying = 2, Deathbomb = 4 };
    bool initialized = false;
    float x = 0, y = 400;
    i32 fixed_x = 0, fixed_y = 51200;
    bool focus = false, dead = false;
    // M5/R-I8：幽灵位（死亡后待救援席）。幽灵不操作/射击/擦弹/拾取/
    // 被瞄准；可获得团队账面奖励但不因此复活。救援（送残 5 点按）/
    // 过关复活清除本位。单席恒 false（SP 无幽灵语义）。
    bool ghost = false;
    // MP final-death latch prevents later shared rewards from causing a respawn.
    bool final_death = false;
    bool ghost_just_entered = false;
    u32 ghost_random = 1, ghost_clock = 0;
    i32 ghost_dx = 0, ghost_dy = 0;
    // M5/R-I9：送残/送 P 计数（AdvanceLifeTransfer 状态字节：taps*32+剩余窗）。
    u8 life_transfer_state = 0;
    u8 power_transfer_state = 0;
    Phase phase = Phase::Active;
    i32 phase_frame = 0, invulnerable = 0;
    // 上一帧的 phase_frame（0x443240 蓝帧判据的 pre-increment 快照）：
    // 原生 00443240.c:242-243 在 :283-300 存 prev/递增之前比较
    // phase_frame != prev && phase_frame % 3 == 0；port 的 sync 在
    // battle.step 之后读到的已是递增后的值（比原生早一帧），故表现层
    // 用本快照还原原生相位。
    i32 phase_frame_previous = 0;
    Timer phase_clock, fire_clock, startup_clock, charge_clock;
    Vec3 charge_origin{};
    std::vector<PlayerShot> shots;
    std::array<DamageRegistration, 256> damage_registrations{};
    i32 damage_registration_cursor = 0;
    std::vector<PlayerShotGeometrySnapshot> shot_geometry;
    u32 shot_generation = 0;
    std::array<PlayerOption, 8> options{};
    i32 option_count = 0;
    std::array<std::array<i32, 2>, 8> option_fixed{};
    std::array<std::array<i32, 2>, 33> movement_history{};
    i32 option_power = -1;
    bool option_reset_pending = false, option_movement_tick = false;
    // ---- M2 迁入的每席域（原 StageBattle 成员，语义/注释原样保留）----
    // 0x443240 case2 死亡重建标志：表现层 sync 消费的一次性边沿。
    bool death_option_rebuild = false;
    // 复活伤害区（player+644/0x4457e0 复活注册的运行时镜像）。
    struct RespawnDamage {
        Vec3 center{};
        float radius = 0;
        Timer remaining;
        bool active = false;
        i32 registration_slot = -1;
    } respawn_damage;
    Vec3 respawn_origin{};  // player+644：入场激光清除用的死亡位置快照
    // Bomb 域（0x40a270 上下文；四角色共享布局，逐席独立库存/相位）。
    i32 bomb_timer = 0;          // 炸弹作用剩余帧
    // Remaining protection supplied by this seat's Bomb, including its tail.
    i32 bomb_protection = 0;
    i32 bomb_age = -1;           // ctx+0x1c；未施放=-1
    // 0040a270:292 `add esi,0x18` → 00401400 的 +0x20 浮点累加器。
    float bomb_age_accumulator = 0;
    u32 bomb_generation = 0;
    struct BombRay {
        Vec3 origin{}, center{};
        float angle = 0;
    };
    std::array<BombRay, 5> bomb_rays{};   // 0040c2c0: ANM 原点与伤害中心
    std::array<PlayerBombOrb, 8> bomb_orbs{};
    float bomb_angle = -1.5707964f;
    float bomb_center_x = 0, bomb_center_y = 0;
    float bomb_rotation_speed = 0;
    float bomb_visual_angle = 0;
    i32 bomb_duration = 0;
    // 妖梦斩击池（每席独立 cursor/槽位；VM 回填经 StageBattle 回调）。
    YoumuBomb youmu_bomb;
    // 当帧输入快照（player 事件/移动判定用；每席独立）。
    u32 input_state = 0;
    // Browser touch target（每席独立触控面；M7 设备席位隔离的权威侧）。
    struct TouchMotion {
        int mode = 0;
        float x = 0, y = 0;
    } touch;
    // SHT 定标（移速/卫星偏移/弹型域；prepare_player 按角色装载，逐席）。
    i32 speed_orth = 576, speed_diag = 407;          // 非聚焦（pl02a.sht ×128）
    i32 speed_orth_focus = 256, speed_diag_focus = 181;
    std::array<std::array<std::array<i32, 2>, 10>, 2> option_offsets{};
    std::array<std::vector<ShtShotRecord>, 11> sht_shots;
    bool sht_shots_loaded = false;
    // 每席角色（0x441dd0 按 be7b8 表取判定尺寸；MP 允许重复角色）。
    i32 character = 0;
    // 0x660+4：0x441dd0 按 kPlayerHitSize[character] 写入的判定尺寸。
    float hit_size = 2.0f;
    // 灵界个人侧：死亡保护标志（团队槽/mode/owner 在 SessionState/团队态，
    // M4 落地；本字段随席位交换，保护期归属该席 owner）。
    bool trance_protection = false;
};

class StageBattle {
    std::unique_ptr<StagePlayerState> owned_player_state_;
    StagePlayerState& player_state_;
    void activate_stage_player();
public:
    StageBattle() : StageBattle(nullptr) {}
    // 共享 game RNG 注入（R 模块 0x4dc324 位接线）：传入时 sim（ECL/弹幕/
    // etEx）与自机弹出膛共用同一实例；缺省 = 自持（tests 无头路径不破）。
    explicit StageBattle(GameRandom* shared_random, StagePlayerState* shared_player = nullptr)
        : owned_player_state_(shared_player ? nullptr : std::make_unique<StagePlayerState>()),
          player_state_(shared_player ? *shared_player : *owned_player_state_),
          sim(shared_random), item_manager(shared_random) {}
    StageSim sim;
    // 当帧敌机总受伤害统计（原版 0x418e00 mgr+0x28 每帧清零并累加，供 harness 对照）
    float frame_damage_total = 0.0f;
    // 道具/神霊管理器（0x42f020/0x438ec0/0x42e380/0x438730）；load 时注入
    // sim.items（StageItemHost），每帧经 update_item_side 步进。
    ItemManager item_manager;
    // 本帧斩击生成事件（slot/script/y；slot=-1 仅视觉）：step_bomb 产出，
    // 表现层（Items/pl03 VM）读后建 VM；每帧 step 开头清空。跨席位聚合
    //（StepBattle::step_mp 在每席 step_bomb 后收集，同帧按席位序表现）。
    std::vector<YoumuBomb::Spawn> youmu_slash_spawns_;
    // 自机对外浮点位置（= 定点 × 0.0078125，无损）；game 坐标 x∈[-192,192]。
    // M2：以下别名全部绑定 player_state_（每席 StagePlayerState 的同一存储
    // 实例）；席位切换经 swap_player_context 交换内容，别名地址不变。
    float& player_x = player_state_.x;
    float& player_y = player_state_.y;
    bool& focus = player_state_.focus;
    bool& player_dead = player_state_.dead;
    // 原版 player+0x65c / +0x668；碰撞不等于已经扣命。
    using PlayerPhase = StagePlayerState::Phase;
    PlayerPhase& player_phase = player_state_.phase;
    i32& player_phase_frame = player_state_.phase_frame;
    i32& player_phase_frame_previous = player_state_.phase_frame_previous;
    bool player_visible() const noexcept {
        return !player_dead && player_phase != PlayerPhase::Dying;
    }
    i32& invulnerable = player_state_.invulnerable;
    // M2：Bomb/respawn/妖梦斩击池/输入/touch/SHT 域随席位交换（原 StageBattle
    // 成员迁移至 StagePlayerState，引用别名保持全部既有访问面不变）。
    StagePlayerState::RespawnDamage& respawn_damage = player_state_.respawn_damage;
    Vec3& respawn_origin = player_state_.respawn_origin;
    i32& bomb_timer = player_state_.bomb_timer;
    i32& bomb_age = player_state_.bomb_age;
    float& bomb_age_accumulator_ = player_state_.bomb_age_accumulator;
    u32& bomb_generation = player_state_.bomb_generation;
    using BombRay = StagePlayerState::BombRay;
    std::array<BombRay, 5>& bomb_rays = player_state_.bomb_rays;
    Vec3 bomb_center() const noexcept { return {bomb_center_x_, bomb_center_y_, 0}; }
    float bomb_visual_angle() const noexcept { return bomb_visual_angle_; }
    std::array<PlayerBombOrb, 8>& bomb_orbs = player_state_.bomb_orbs;
    std::array<PlayerOption, 8>& options = player_state_.options;
    i32& option_count = player_state_.option_count;
    i32 player_character() const noexcept { return character_; }
    i32 player_power() const noexcept { return player_ ? player_->power : 0; }
    i32 player_power_max() const noexcept { return player_ ? player_->max_power : 0; }
    i32 power_level() const noexcept {
        return player_ && player_->power_step > 0 ? player_->power / player_->power_step : 0;
    }
    bool full_power() const noexcept {
        return player_ && player_->power >= player_->max_power;
    }
    std::vector<PlayerShot>& shots = player_state_.shots;
    // 447230 creates the hit effect synchronously during enemy collision,
    // before later enemies/ANM instructions consume visual RNG.
    std::function<bool(const Vec3&, const Vec3&, float, i32)> beam_hit_effect;
    std::function<bool(StageBattle&)> option_power_changed;
    // 0x443240 case2：0x668==3（死亡演出第 3 帧）先 0x445a20 扣 power，
    // 再调 0x444df0 重建 option 树；0x444a10 已把 0x146c4 置 0。表现层
    // 在 battle.step 之后的 sync 消费此标志（此时 option_count 已按新
    // power 更新），重建语义见 sync_player_options 的 Dying 分支。
    bool& death_option_rebuild = player_state_.death_option_rebuild;
    // 445c40 initializes the shot VM before its first4464d0 update; the
    // later ANM manager pass is a separate tick, including on the birth frame.
    std::function<bool(PlayerShot&, u32)> player_shot_birth;
    // M6：ANM registry 注入点（Web 生产由 RunScene 接 animations.registry；
    // 无头测试不注入 = WorldState 的 ANM 面跳过 capture/restore，其余域
    // 照常——敌机槽位句柄在无头路径恒 0，恢复空 registry 与快照一致）。
    AnmRegistry* mp_registry_hook = nullptr;
    AnmRegistry* mp_registry() { return mp_registry_hook; }
    // 40cb50 creates/reads slash VMs before registering their hitboxes.
    std::function<bool(StageBattle&)> youmu_bomb_update;
    // 4460d0's charge VM is created/retired synchronously during player update.
    // action 0=create, 1=ready interrupt, 2=retire, 3=update position.
    std::function<bool(i32, const Vec3&)> youmu_charge_effect;
    // 45f8c0: independent priority-16 bomb shake, driven by game RNG.
    std::function<bool(i32, i32, i32, i32)> bomb_screen_shake;
    std::function<bool()> reimu_explosion_shake;
    // 40b590 calls 40b1b0 per orb before the enemy/item update passes.
    std::function<bool(StageBattle&, i32)> reimu_bomb_orb_birth;
    std::array<DamageRegistration, 256>& damage_registrations =
        player_state_.damage_registrations;
    i32& damage_registration_cursor = player_state_.damage_registration_cursor;

    // 定点模型状态（§7.2）。
    i32& pos_fx = player_state_.fixed_x;
    i32& pos_fy = player_state_.fixed_y;
    void set_player_pos(float x, float y) {
        player_x = x;
        player_y = y;
        pos_fx = i32(x * 128.0f);
        pos_fy = i32(y * 128.0f);
        // 与 restore_replay_player 同式同步：ECL 瞄准/碰撞回调直接读
        // sim.player_*（契约常量默认 192,420），仅写 battle 侧会留脏值。
        sim.player_x = x;
        sim.player_y = y;
    }
    // 448ec0 ->447850: recorded fixed position and initial focus, before input0.
    void restore_replay_player(i32 x,i32 y,bool initial_focus);
    i32& speed_orth = player_state_.speed_orth;
    i32& speed_diag = player_state_.speed_diag;
    i32& speed_orth_focus = player_state_.speed_orth_focus;
    i32& speed_diag_focus = player_state_.speed_diag_focus;
    // Browser touch target in player coordinates; mode 0 disables it, 1 limits
    // movement to the current speed, 2 moves directly to the clamped target.
    StagePlayerState::TouchMotion& touch = player_state_.touch;
    u32& input_state = player_state_.input_state;  // 当帧回放输入（player 事件用）
    // 妖梦 Bomb 斩击池（0x40cb50 战斗侧）：每席独立 cursor/槽位；set_vm 由
    // 表现层（pl03 VM 持有者）在 ANM tick 后回填——每帧序 = battle 生成支路
    //（1 次 signed_unit 抽）→ VM tick（ins_119 抽）→ set_vm → 命中/消弹查询
    //（原版帧序）。
    YoumuBomb& youmu_bomb_ = player_state_.youmu_bomb;

    // 定标常量（.sht 弹型域以外的域：碰撞/无敌等，§7.2【待校准】）。
    // kFireInterval/kShotDamage/kShotOffset* 仅无 .sht 回退路径使用；
    // 弹型/伤害/弹速/间隔/相位全部出自 .sht 记录（0x446000/0x445c40）。
    static constexpr i32 kFireInterval = 4;      // 回退：发射间隔帧（相位制）
    static constexpr float kShotDamage = 12.0f;  // 回退：单发伤害（整值）
    static constexpr float kShotSpeed = 12.0f;   // 仅无 SHT 的旧回退；生产用记录 +0x18。
    static constexpr float kShotHalf = 4.0f;     // 弹体半宽高
    static constexpr float kShotOffsetX = 8.0f;  // 无 .sht 回退：三发 x 偏移
    static constexpr float kShotOffsetY = 12.0f; // 无 .sht 回退：出膛 y 偏移
    // 自机判定尺寸（0x441dd0:65 + .data 0x4c0ba8[be7b8]）：DAT_004be7b8 =
    // 选人光标 0..3；常规路径 be7bc=0，故下标 = character。值 = 2/3/3/3，
    // 原版存“全尺寸”：0x444140 弹幕圆形判定以 尺寸² 参与（port collide_player
    // 的 p2 = p*p 同式）；0x441dd0 以 ×0.5 写入玩家矩形半宽 0x620/0x624
    //（0x443fd0 AABB 分支用半宽 = 尺寸/2，collide_player 同式）。
    // 敌机本体碰撞（0x419a90 系）的旋转矩形域（+0x118/+0x11c 与 flags
    // 0x8000）仍待 StageSim 侧字段落地；kPlayerHitRadius/kEnemyBodyRadius
    // 仅作本作敌机本体近似保留，不再宣称校准。
    static constexpr float kPlayerHitSize[4] = {2.0f, 3.0f, 3.0f, 3.0f};
    static constexpr float kPlayerHitRadius = 3.0f;
    static constexpr float kBulletHitRadius = 4.0f;
    static constexpr float kEnemyBodyRadius = 20.0f;
    static constexpr i32 kInvulnFrames = 240;
    static constexpr i32 kBombFrames = 180;
    static constexpr float kBombDamagePerFrame = 3.0f;
    static constexpr i32 kKillScore = 5000;
    static constexpr i32 kKillPower = 8;
    // 灵界（trance）：
    static constexpr i32 kTranceActivateFrame = 30;   // 0x4057f0：任务计时 ≥0x1e
    static constexpr float kTranceDrainPerFrame = 3.0f;  // 0x4aeca8=3.0【drain 点待 asm 复核】

    // sht_name：移速 + 弹型来源（回放对拍 = plXXa/b.sht；缺文件时失败）。
    bool load(EclResourceProvider& provider, const char* stage_file, i32 difficulty_rank,
              StageBattleServices& host, SessionState& progress, HudPlayerState& player,
              HudScoreState& score, const char* event_path = nullptr,
              const char* sht_name = nullptr);
    // 441dd0/444df0 occur before 447900 restores a cold replay.
    bool prepare_player(EclResourceProvider& provider, SessionState& progress,
                        HudPlayerState& player, const char* sht_name, bool cold_entry);
    // 一帧逻辑：输入 → 自机/自机弹 → ECL 步进 → 碰撞 → 奖惩/HUD。
    // 事件落盘开启时：帧号自 1 起（0 = start() 装载趟），player 事件为本帧
    // 首条（§7.3）。
    bool step(const GameInput& input);
    bool step_transition(const GameInput& input);
    // 原版把道具/神霊管理器注册为独立调度任务：0x42efb0 @ Update 0x18=24、
    // 0x438e70 @ Update 0x19=25（objdump 0x42e0e0/0x438400：mov ebx,0x18/
    // 0x19 后 call 0x470990；激光 0x42fa30 为 0x16=22、子弹 0x40e780 为
    // 0x17=23）。生产 RunScene 置 defer_item_side=true 并在 24/25 两个 pass
    // 分别调用两个 phase；无宿主调度的 harness/单测保持 step 内合并路径。
    // 0x42e380/0x438730 的帧内相位因此与原生一致：激光擦弹（0x431a1f →
    // 0x445900）的 max_point+100 先于 kind3 收集（S5 f2725 首差根因）。
    bool defer_item_side = false;
    void update_items_phase();    // 0x42e380 道具更新 + 道具事件
    void update_spirits_phase();  // 0x438730 神霊更新 + 神霊事件
    // M3：多席收集面（R-I7：每件/每只按最近合格席；单席 = 单元素）。
    std::array<ItemCollector, 3> make_seat_collectors();
    bool continue_player();
    void on_bomb_key();  // X 边沿（经 step 内部处理，这里供测试直调）
    bool end_trance(); // 405b80: depletion or428820 dialogue entry.
    // 观察接口（r4/harness TH13_RUN_SCENE=1 路径）：装载后补挂事件落盘。
    // 与 battle.load(event_path) 的观察面等价（events_ 置位 → 逐帧
    // sink.set_frame + player 事件），只写 sink，不改变任何战斗机制/相位。
    bool open_event_sink(const char* path);
    const std::string& error() const { return error_; }

    void begin_player_shot_geometry_snapshot(i32 seat = -1);
    void set_player_shot_geometry(i32 slot, i32 age, i32 script, u32 generation,
                                  const std::array<Vec3, 4>& quad, i32 seat = -1);

    // .sht 弹型域：5 能量档非聚焦（块 0-4）+ 5 能量档聚焦（块 5-9）+
    // 特殊块 10（类别 2，未接入）。M2：逐席域（每角色独立弹型表），经
    // 引用别名跟随当前活动席。
    static constexpr u32 kShtBlocks = 11;
    std::array<std::vector<ShtShotRecord>, kShtBlocks>& sht_shots =
        player_state_.sht_shots;
    bool& sht_shots_loaded = player_state_.sht_shots_loaded;

    // ---- M2：多席自机（一套共享世界 + 2..3 席独立玩家面） ----
    // 席位数（RULES R-X：2–3 席）。0 = 单席（SP 兼容，全部既有路径不变）。
    // 启用后 step_mp 按固定席位序 0..seat_count-1 推进各席玩家面，世界面
    // （StageSim/ItemManager/SpellState/团队灵界）每逻辑帧只推进一次。
    i32 seat_count = 0;
    // 当前活动席（别名/回调归属）。单席恒 0。
    i32 active_seat = 0;
    // 每席玩家面存储（M2 权威槽位）。enable_seats 时把当前 player_state_
    // 窗口内容移入 seats[0]（SP 兼容席），此后窗口只是活动席暂存。
    std::array<StagePlayerState, 3> seats{};
    // 每席 HUD 资源（残机/Bomb/Power/deaths；RULES R-A「每席独立」）。
    // 单席模式 seat 0 由宿主注入的 player_ 提供（load/prepare_player 绑定）。
    std::array<HudPlayerState, 3> seat_huds{};
    // 每席角色（RULES R-X：允许重复角色）。单席 = progress.character。
    std::array<i32, 3> seat_characters{};
    // MP 输入沿：每席独立 GameInput（held/previous/pressed/repeat/duration）。
    std::array<GameInput, 3> seat_inputs{};
    // ---- M4：团队灵界状态机（RULES R-T1..T10） ----
    // backing 仍是 SessionState.run_mode_flags bit0x100 + ending_points
    //（共享槽，全场一份）。本状态机只记录 MP 侧归属：mode/owner/generation。
    // 单席（seat_count==0）不改变任何 SP 语义——查询三分类：
    //   team_trance_active()  团队全场查询（神霊/BGM/HUD/槽显示/槽消耗）；
    //   seat_trance_active()  可操作席位的灵界射击/伤害/三类免疫查询；
    //   bomb_allowed()        团队灵界对新 Bomb 的限制（R-T2）。
    enum class TranceMode : i32 { Idle = 0, Manual = 1, DeathProtection = 2 };
    TranceMode trance_mode = TranceMode::Idle;
    i32 trance_owner_seat = 0;   // 发动者（仲裁序与死亡责任归属）
    u32 trance_generation = 0;   // 代数（R-T9：owner 死亡重生只清当前代）
    // 进入死亡演出时记录的灵界代数：重生第 30 帧清槽（0x443240:158）
    // 仅当期间未开启新代灵界——owner 保护灵界耗尽死亡的演出期间队友
    // 开启的新代不得被旧重生路径清掉（R-T9 的 generation 守卫）。
    u32 death_trance_generation = 0;
    // 团队全场查询（= SP trance_active；MP 下同式）。
    bool team_trance_active() const {
        return progress_ && trance_active(*progress_);
    }
    // 可操作席位的灵界射击查询：灵界期间全体可操作席（R-T2）。
    bool seat_trance_active() const { return team_trance_active(); }
    // R-T2：团队灵界激活期间，所有席位禁止发动新 Bomb。
    bool bomb_allowed() const {
        return !team_trance_active();
    }
    // 把 seats[seat] 的内容换入 player_state_ 窗口（当前窗口内容先写回
    // 其权威槽位）。引用别名自动跟随；席位切换不触碰共享世界/RNG。
    void swap_player_context(i32 seat);
    // 席位循环收尾：窗口写回 seats[active_seat]，恢复调用方活动席。
    void restore_seat_context(i32 restore_seat);
    // 启用多席（2..3；RULES R-X）。调用点 = RunScene/宿主在 battle.load
    // 与 prepare_player 之后：把当前单席玩家面收编为 seat 0，逐席绑定
    // seat_huds/seat_characters/输入沿。characters 为每席角色（允许重复）。
    // 每席按角色装载 plXXa.sht 定标域（移速/卫星偏移/弹型表——provider
    // 供 .sht 读取；缺文件即失败，不静默降级）。hud 模板 = SP 起飞资源
    //（R-B：每席备用残机 2、Bomb 2、碎片 0、power 按开局档位）。
    bool enable_seats(EclResourceProvider& provider, i32 count,
                      const std::array<i32, 3>& characters);
    // M7：本地席位号（宿主接线——联机会话的 LocalSeat）。step 的
    // effective_input（playback_input/物理输入接缝）写入本席的输入沿；
    // 默认 0 = 单席/本地多席测试零变化。远端席位由 Session::CanSimulate
    // 逐帧注入。双端对称性依赖此值与 SessionSetup.local_player 一致。
    i32 mp_local_seat = 0;
    // MP 一帧：固定席位序推进所有席位的玩家面（每席 update_player/
    // options/firing/shots/bomb），世界面（sim.step_frame）在席位循环后
    // 只推进一次。输入取 seat_inputs；单席路径（seat_count==0）不经过本入口。
    bool step_mp();
    // M4/R-T8：MP 灵界团队帧（Manual 请求逐席仲裁 + 槽消耗一次）。
    bool update_trance_mp();
    // ---- M5：幽灵与团队生命周期 ----
    // 幽灵席数（R-I8）。0 = 全员可操作。
    i32 ghost_count() const {
        i32 count = 0;
        for (i32 seat = 0; seat < seat_count; ++seat)
            if (seats[size_t(seat)].ghost) ++count;
        return count;
    }
    // 团队全灭（全员幽灵；R-G 180 逻辑帧全灭等待由宿主层计时）。
    bool team_wiped() const { return seat_count >= 2 && ghost_count() == seat_count; }
    // 救援（R-I9 定向事务）：donor 扣 1 命、recipient 清幽灵位并按 SP
    // 重生语义复位（复活 Bomb = SP 初始 2）。跨席事务：不走拾取广播。
    bool rescue_seat(i32 donor, i32 recipient) {
        if (seat_count < 2 || donor == recipient) return false;
        if (donor < 0 || donor >= seat_count || recipient < 0 || recipient >= seat_count)
            return false;
        auto& donor_hud = seat_huds[size_t(donor)];
        auto& recipient_hud = seat_huds[size_t(recipient)];
        if (!seats[size_t(recipient)].ghost || seats[size_t(recipient)].ghost_just_entered)
            return false;   // 非幽灵或刚进入幽灵的同一帧不可救
        if (seats[size_t(donor)].ghost || seats[size_t(donor)].dead) return false;
        if (donor_hud.extra_lives <= 0) return false;         // 无命可送
        --donor_hud.extra_lives;
        seats[size_t(recipient)].ghost = false;
        seats[size_t(recipient)].dead = false;
        seats[size_t(recipient)].final_death = false;
        // SP 重生资源（R-G：救援数值用本作 SP 上限与初始 Bomb）。
        recipient_hud.bombs = 2;
        recipient_hud.bomb_pieces = 0;
        recipient_hud.extra_lives = std::max(0, recipient_hud.extra_lives);
        recipient_hud.power = recipient_hud.max_power / 2;
        seats[size_t(recipient)].life_transfer_state = 0;
        seats[size_t(recipient)].power_transfer_state = 0;
        // 位置/相位复位（Entering 重生态由 update_player 的重生序接管）。
        seats[size_t(recipient)].phase = StagePlayerState::Phase::Active;
        seats[size_t(recipient)].phase_frame = 0;
        seats[size_t(recipient)].invulnerable = 240;
        return true;
    }
    // 过关复活（R-G）：stage 切换时清全部幽灵位（过关复活=全员可操作）。
    void revive_all_on_stage_clear() {
        for (i32 seat = 0; seat < seat_count; ++seat) {
            if (!seats[size_t(seat)].ghost) continue;
            seats[size_t(seat)].ghost = false;
            seats[size_t(seat)].dead = false;
            seats[size_t(seat)].final_death = false;
            seat_huds[size_t(seat)].bombs = 2;
            seat_huds[size_t(seat)].bomb_pieces = 0;
            seat_huds[size_t(seat)].extra_lives = std::max(0, seat_huds[size_t(seat)].extra_lives);
            seat_huds[size_t(seat)].power = seat_huds[size_t(seat)].max_power / 2;
            seats[size_t(seat)].life_transfer_state = 0;
            seats[size_t(seat)].power_transfer_state = 0;
            seats[size_t(seat)].phase = StagePlayerState::Phase::Active;
            seats[size_t(seat)].phase_frame = 0;
            seats[size_t(seat)].invulnerable = 120;
        }
    }
    // 单席观察面（seat_count==0 时 seats[] 未收编，宿主读窗口玩家态）。
    const StagePlayerState& player_state() const { return player_state_; }
    const HudPlayerState* seat_hud(i32 seat) const {
        return seat >= 0 && seat < i32(seat_huds.size()) ? &seat_huds[size_t(seat)] : nullptr;
    }

private:
    bool team_bomb_protected() const;
    void advance_mp_protection_and_ghosts();
    void share_mp_bomb_protection();
    bool retire_final_death_combat();
    void update_transfers();
    bool player_prepared_ = false;
    void rebuild_options_for_power();
    // M3：单席弹-玩家碰撞体（collide_player_bullet 的 MP 分派按席位循环
    // 调用；seat = 判定席号，擦弹 mask 用）。
    void collide_player_bullet_seat(StageBullet& bullet, i32 seat);
    // M2：按角色装载 .sht 定标域到当前窗口席（enable_seats 逐席调用；
    // 与 prepare_player 的 SHT 段同源语义：移速/卫星偏移/弹型表，缺文件
    // 或布局异常即失败，不静默降级）。
    bool load_seat_sht(EclResourceProvider& provider, i32 character);
    // bomb_slow = 本帧开始前 Bomb 已在运行（0x40a860 写的 0xa2b4 乘数），
    // 不含本帧按键刚启动的 Bomb：00443240 在 00442960 之后才复位 1.0。
    void move_player(u32 held, bool bomb_slow);
    void update_options(bool reset = false);
    void update_firing(bool held);
    void step_bomb();
    float query_enemy_damage(StageEnemy& enemy, float width, float height);
    bool query_enemy_bomb_hit(const StageEnemy& enemy) const;
    void explode_orb(PlayerBombOrb& orb);
    void fire_shots(i32 frame);
    void step_shots();
    i32 reserve_damage_registration();
    i32 register_circle(Vec3 center, float radius, float growth, i32 lifetime,
                        i32 damage, i32 interval = 1, i32 threshold = 999999,
                        i32 orb = -1);
    i32 register_box(Vec3 center, float width, float height, float angle, i32 damage);
    i32 allocate_damage_registration(PlayerShot& shot);
    void sync_damage_registrations();
    void retire_damage_registration(PlayerShot& shot);
    void collide_shots(StageEnemy& enemy);
    void collide_enemy_body(StageEnemy& enemy);
    void consume_player_shot_geometry();
    void collide_player_bullet(StageBullet& bullet);
    i32 collide_laser(const StageLaser&);
    void reward_kill(StageEnemy& enemy);
    void update_item_side();    // 0x42e380/0x438730 玩家只读面装配 + 步进
    ItemCollector make_item_collector();  // 0x42e380/0x438730 共用只读面
    void emit_item_events();              // 道具/神霊事件 → HUD/音/弹字
    bool update_trance(const GameInput& input);  // 0x4057f0/0x405a20/0x405b80
    bool kill_player();
    bool commit_death();
    bool commit_death_normal();
    bool prepare_expired_deathbombs(std::array<bool, 3>&);
    bool update_player(const GameInput&, bool deathbomb_prepared = false);
    StageBattleServices* host_ = nullptr;
    SessionState* progress_ = nullptr;
    HudPlayerState* player_ = nullptr;
    HudScoreState* score_ = nullptr;
    Timer& player_phase_clock_ = player_state_.phase_clock;
    Timer& fire_clock_ = player_state_.fire_clock;
    std::array<std::array<std::array<i32, 2>, 10>, 2>& option_offsets_ =
        player_state_.option_offsets;
    std::array<std::array<i32, 2>, 8>& option_fixed_ = player_state_.option_fixed;
    std::array<std::array<i32, 2>, 33>& movement_history_ = player_state_.movement_history;
    i32& option_power_ = player_state_.option_power;
    bool& option_reset_pending_ = player_state_.option_reset_pending;
    bool& option_movement_tick_ = player_state_.option_movement_tick;
    float& bomb_angle_ = player_state_.bomb_angle;
    float& bomb_center_x_ = player_state_.bomb_center_x;
    float& bomb_center_y_ = player_state_.bomb_center_y;
    float& bomb_rotation_speed_ = player_state_.bomb_rotation_speed;
    float& bomb_visual_angle_ = player_state_.bomb_visual_angle;
    i32& bomb_duration_ = player_state_.bomb_duration;
    i32& character_ = player_state_.character;
    bool life_probe_ = false;  // TH13_LIFE_PROBE=1：敌机生命轨迹对拍探针
    bool events_ = false;  // 事件落盘（回放对拍）
    bool sink_open_ = false;  // open_event_sink 已启（load 不清 events_ 的幂等位）
    float& player_hit_size_ = player_state_.hit_size;  // 0x660+4：0x441dd0 写入
    // 灵界（trance）状态：团队槽在 SessionState.ending_points（共享）；
    // 个人死亡保护标志逐席（StagePlayerState.trance_protection 引用别名）。
    bool& trance_protection_ = player_state_.trance_protection;
    i32 trance_task_frame_ = 0;  // 任务计时（+0x20，rate 1.0 每帧 +1；共享）
    std::string error_;
    // M6：结构化快照访问面（web/cpp/multiplayer/WorldState 专用——
    // 灵界任务计时是权威模拟量，随 WorldState 快照走）。
    friend class multiplayer::WorldState;
    // M9：关头延续快照（StageEntry 的 Capture/Restore 同用本访问面）。
    friend struct multiplayer::StageEntry;
    std::vector<PlayerShotGeometrySnapshot>& shot_geometry_ = player_state_.shot_geometry;
    u32& shot_generation_ = player_state_.shot_generation;
};

}  // namespace th13
