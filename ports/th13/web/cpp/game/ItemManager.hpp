// 道具/神霊系统（原版 th13.exe 道具管理器行为子集，地址逐条登记）。
// 证据：artifacts/decomp/jp/functions/{0042f020,00417b10,0042e380,0042edc0,
// 0043d210,0043d330,0043d480,0043d2f0,0043d440,00438ec0,00438730}.c。
//
// 原版三个池：
//   - 道具池（0x42f020 写入单个"待落槽"，0x42e380 逐帧步进 2648 槽、记录步长
//     0xBC8、字段位于槽 +0xB58..+0xBAC）：类型 1..9。
//   - 神霊池（0x438ec0 管理；0x438730 步进）：类型 10..14 经 spawn_item 转入。
//   - 得分弹出池（0x44ad60，0x40 步长）——表现侧，本模块只产事件。
// RNG：掉落散布消费共享 game RNG（0x4dc324 位，StageSim.random / ApplicationState
// ::game_random 同一实例）；0x472de0 = unit() [0,1)，0x472e10 = signed_unit()。
#pragma once

#include "SessionState.hpp"
#include "StageSim.hpp"
#include "Types.hpp"

#include <array>
#include <functional>

namespace th13 {

class GameRandom;

// 道具类型 = 0x42f020 param_1（dropItems 0x417b10 产 1..14）。收集分派 =
// 0x42e380 LAB_0042e7f7 的 switch(槽+0xBA4)。
enum ItemType : i32 {
    ItemPower = 1,       // 小P：0x42edc0（be7e8 += 1；满灵力 score+1000 原值）
    ItemPoint = 2,       // 点：0x42e380 case2（高度/最大得点公式）
    ItemBigPower = 3,    // 大P：0x43d210（满灵力 popup 20000、score+20000/10）
    ItemLifePiece = 4,   // 残机碎片：0x43d330（0x4bb994 阈值表，满→+1 命、下标++）
    ItemBombPiece = 5,   // 灵击碎片：0x43d480(+2)（满 8→+1 炸弹）
    ItemExtraLife = 6,   // 1UP：0x43d2f0（直接 +1 命，上限 8）
    ItemBomb = 7,        // 灵击：0x43d440（直接 +1 炸弹，上限 9）
    ItemBigPowerAlt = 8, // 大P 变体：0x43d210 + 满灵力 popup 100（score+100/10）
    ItemSpiritDrop = 9,  // 消弹灵（state5→2 上升→追踪；收集 score+10 显示、max_point+10）
    ItemSpiritKind0 = 10, // 神霊 kind0（残机碎片灵）：0x438ec0 → 0x438730 case0
    ItemSpiritKind1 = 11, // 神霊 kind1
    ItemSpiritKind2 = 12, // 神霊 kind2（灵击碎片灵：0x43d480(+1)）
    ItemSpiritKind3 = 13, // 神霊 kind3（灵界灵：0x438730 case3 = trance 累加）
    ItemSpiritKind4 = 14, // 神霊 kind4
};

// 道具池槽（原版槽 +0xB58..+0xBAC 字段域；状态机 = 0x42e380）。
struct Item {
    i32 state = 0;  // +0xBA0：0 空 1 落下 2 上升(消弹灵) 3 追踪 4 聚焦追踪 5 待转
    i32 type = 0;   // +0xBA4
    i32 type2 = 0;  // +0xBA8（state5 路径写 9）
    float x = 0, y = 0, z = 0;   // +0xB5C/+0xB60/+0xB64
    float vx = 0, vy = 0;        // +0xB68/+0xB6C（polar(angle,speed)，0x42f540）
    float vz = 0;                // +0xB70（出生 0）
    float homing_speed = 0;      // +0xBAC
    i32 sprite = 0;              // +0xBB0（state5 每帧倒计时，<0 → state2）
    // anm 域（0x42f020 出生：+0xB78=-999999 哨兵后 -1（prev）、+0xB7C=0（frame）、
    // +0xB80=0（time）；步进 = 0x42e380 尾 [prev]=旧[frame]、[frame]+=rate）。
    // 表现层真 VM 由 Combat 侧持有；这里只保留计数域供状态机子句。
    i32 anm_frame = 0;           // +0xB7C（出生 0）
    i32 anm_frame_prev = -1;     // +0xB78（出生 -1）
    float anm_time = 0;          // +0xB80（出生 0）
    u32 flags = 0;               // +0xB88 bit0 = 已初始化
    bool alive = false;          // state != 0 的移植侧快捷判（不回写原版域）
};

// 神霊池槽（0x438ec0 槽 +0x14 起；字段 = 0x438730 读法，float 索引 ×4）。
struct Spirit {
    i32 state = 0;  // [0]：0 空 1 游离 2 追踪 4 慢速追踪(左下角) 5 场景门控
    i32 kind = 0;   // [1]（0x438ec0 param_3；==4 时落 1）
    u32 serial = 0; // [3]: original birth counter, identifies slot reuse.
    float speed = 0;        // [4]（目标速度，state2/4 每帧 +0.3/+0.05 封顶）
    float move_speed = 0;   // [0xb]（位移速度快照；state1 每帧 -0.12666667）
    float x = 0, y = 0, z = 0;  // [5]/[6]/[7]
    float angle = 0;        // [0xc]
    i32 anm_frame = 0;      // [0x1d]（出生 0；步进 = 0x438730 顶部 rate 累加）
    i32 anm_frame_prev = 0; // [0x1c]（出生 -1）
    float anm_time = 0;     // [0x1e]
    // [0x18]：第一 VM 帧计数（出生 -1，每帧 +1）——到 480（0x1e0）时清 flags
    // bit1（0x438730:142：反弹门关、此后越界即回收）。不是倒计时寿命。
    i32 frame = -1;
    Timer lifetime_clock;
    u32 flags = 0;          // [0x21] bit0 = kind 门、bit1 = 界外反弹门
    bool fade_requested = false; // 00438a7c: deferred ANM label1 at frame480.
    bool alive = false;
};

class ItemManager;

// 玩家侧只读面（DAT_004c22c4 玩家结构 + 0x4be7b8 进度结构的收集域）。
// StageBattle 每帧装配；数值域直接映射 SessionState/HudPlayerState。
struct ItemCollector {
    bool boss_present=false;
    float boss_x=0,boss_y=0;
    float x = 0, y = 0;         // 玩家 +0x5B8/+0x5BC
    i32 state = 1;              // +0x65C（1 active / 2 dying / 4 deathbomb）
    bool focus = false;         // DAT_004e4c08 & 8
    bool dialog = false;        // msgmgr+0x198 != 0（对话期 0x42e380 门）
    // 追踪初速 = *(player+0x660 表 + 8)（0x42e380 state1→3 / 4 装填）。
    float homing_speed = 4.0f;
    // 收集盒（0x42e380 LAB_0042e7f7）三组，均为 {left, top, right, bottom}：
    //   box            = +0x146EC..+0x146FC：收集判定盒（单一，无聚焦变体）。
    //   box_auto_focus = +0x14704..+0x14714：吸附盒（DAT_004e4c08&8 聚焦时选用）。
    //   box_auto       = +0x1471C..+0x1472C：吸附盒（非聚焦时选用）。
    // 吸附盒命中且 state 非 3/4 → state 4、追踪初速 = homing_speed/3
    //（asm 0x42eb00-0x42ebb9）。默认 {0,0,0,0} = 吸附关闭（未接线时行为同旧版）。
    std::array<float, 4> box{{0, 0, 0, 0}};
    std::array<float, 4> box_auto{{0, 0, 0, 0}};
    std::array<float, 4> box_auto_focus{{0, 0, 0, 0}};
    // 收集结算宿主（原版写 0x4be7b8 家族；port 经 SessionState/HudPlayerState/
    // HudScoreState：score = 0x4be7c0 原值域，max_point = 0x4be7dc）。
    // progress->character = DAT_004be7b8（选人光标 0..3，0x441dd0 按 0x4c0ba8
    // [be7b8] 取判定尺寸实证）：0x438730 case1 的慢速追踪门（==0）与 kind1
    // 收集加值门（==2）按此判；progress 为空时按 0 处理。
    SessionState* progress = nullptr;
    HudPlayerState* player = nullptr;
    HudScoreState* score = nullptr;
};

// 收集/得分事件（HUD/表现层消费；原版侧写点见各条注释）。
struct ItemEvent {
    enum class Kind : i32 {
        Collected = 0,   // 任一道具进收集盒并被结算
        ScorePopup = 1,  // 0x44ad60（value = 显示值，pos）
        PowerLevelUp = 2,// 0x42edc0/0x43d210 返回真 → 0x444df0
        FullPower = 3,   // 0x42a430（满灵力演出）
        Extend = 4,      // 0x43d330 满碎片 +1 命（sound 0x11）
        SpiritGained = 5,// 0x438730 → 0x43d530（灵界值累加）
    };
    Kind kind = Kind::Collected;
    i32 value = 0;          // 弹出值/灵力增量/灵界增量
    float x = 0, y = 0;
};

class ItemManager final : public StageItemHost {
public:
    // Production queries the actual ANM handle at 00438aca, before collection.
    // Standalone arithmetic tests have no animation host.
    std::function<bool(u32 slot, u32 serial)> spirit_animation_alive;
    // 原版槽量：0x42e380 循环上界 0xa57+1 = 2648（道具）；0x438ec0 扫描
    // 0x100 = 256 槽（步长 0x22 float）——满时静默丢弃（本函数返回不产件）。
    static constexpr i32 kItemCapacity = 2648;
    static constexpr i32 kSpiritCapacity = 256;

    explicit ItemManager(GameRandom* shared_random = nullptr)
        : random_(shared_random) {
        // 413e50 pushes ascending slot addresses onto two separate heads.
        for (i32 i = 0; i < 600; ++i) ordinary_free_.push_back(i);
        for (i32 i = 600; i < kItemCapacity; ++i) cancel_free_.push_back(i);
    }

    void reset_items(); // 413e50: ordinary/cancel pools; spirit manager is separate.

    void bind_random(GameRandom& random) { random_ = &random; }

    // 会话绑定（additive，不破坏既有签名）：神霊出生 flags bit0 =
    // DAT_004be830>>8 & 1 = trance 激活位（0x438ec0:54 的 XOR 化简结果）。
    // 未绑定时按 0（未激活）处理——常规局主路径同原版。
    void bind_session(const SessionState& session) { session_ = &session; }

    // ---- StageItemHost（StageSim 注入调用面，见 StageSim.hpp）----

    // 0x0042f020 ItemManager::spawnItem(const float pos[3], i32 type,
    //                                   float angle, float speed)。
    //   type 10..14 → 转入 spawn_value_item（0x438ec0）；
    //   type 9 → 道具池 state=5（上升灵）；
    //   其余 → state=1；x 钳 [-192,192]；vel = polar(angle, speed)。
    // 本体（0x42f020）不触 game RNG；kind 10..14 的两次抽取在
    // spawn_value_item（0x438ec0.c:94-115，勘误：此前头注释误记「无 RNG」）。
    void spawn_item(float x, float y, float z, i32 kind, float angle,
                    float speed) override;

    // 0x00438ec0 SpiritManager::spawn(mgr, kind0, kind, pos, base_speed, ~0)：
    // pos 钳 x∈[-192,192]、y∈[0,384]；每件抽 signed_unit()（serial%4 象限角
    // ±π/8 基向：0→-3π/4、1→π/4、2→3π/4、3→-π/4）与 unit()（速度 =
    // u*3.5 + base_speed），顺序先角后速（00438ec0.c:94-115）。
    void spawn_value_item(i32 kind, float x, float y, float z,
                          float base_speed) override;

    // 0x4302a0：616 etClear 转换点登记（St1 未消费，登记待证面）。
    void on_et_clear(float x, float y, float z) override;

    // 兼容旧数组入口（INTERFACE.md 登记；E 侧 StageSim 直接走三虚方法）。
    void spawn_item(const float pos[3], i32 type, float angle, float speed);

    // 0x00417b10 散布掉落（ECL 509）的 RNG 与逐件散布已由 E 在
    // StageSim::drop_table 原序实现（3 抽/件），本管理器不再重复提供该路径；
    // 敌机死亡主掉落（0x41aad0）在 Stage1 受 +0x3f64 bit1 门控恒不触发
    //（E HANDOFF 勘误），port 无调用点。

    // ---- 步进 ----

    // 0x0042e380 ItemManager::update（道具池状态机 + 收集结算）。
    // 收集盒命中 → switch(type)：1→0x42edc0、2→点值公式、3/8→0x43d210、
    // 4→0x43d330、5→0x43d480(+2)、6→0x43d2f0、7→0x43d440、9→score+10/max_point+10。
    void update(const ItemCollector& player);

    // 0x438730 SpiritManager::update（神霊状态机 + 收集）。
    void update_spirits(const ItemCollector& player);
    void set_spirit_trance(bool active); // 439280/4393a0: convert all live spirits.

    // 0x0044ad60 的调用面：收集时产生的弹出/声音/效果事件队列（每帧排空）。
    std::vector<ItemEvent>& events() { return events_; }
    void clear_events() { events_.clear(); }

    std::array<Item, kItemCapacity>& items() { return items_; }
    std::array<Spirit, kSpiritCapacity>& spirits() { return spirits_; }

    // ---- 0x4be7b8 进度域（原版道具侧写点 → SessionState/HudPlayerState）----

    // 0x0042edc0 小P收集：满灵力 → score+1000（原值）；否则 power+1，跨
    // power_step 边界时 options 重排（0x444df0）。返回是否跨档。
    bool collect_power_small();
    // 0x0043d210 大P：power += amount（未满时），满级溢出钳制 + 满灵力演出；
    // 返回是否跨档。
    bool collect_power_big(i32 amount);
    // 0x0043d330 残机碎片 +1：0x4bb994[下标] 循环扣减，满 → +1 命/下标++。
    void collect_life_piece();
    // 0x0043d480 灵击碎片 +amount：满 8 → +1 炸弹（上限 9）。
    void collect_bomb_piece(i32 amount);
    // 0x0043d2f0 1UP：+1 命（上限 8）。
    void collect_extra_life();
    // 0x0043d440 灵击：+1 炸弹（上限 9）。
    void collect_bomb();
    // 0x0043d530 灵界值累加（0x438730 收集路径；SessionState::trance_add 同式，
    // 封顶 600）。
    void collect_trance(i32 amount);
    // 点道具（type 2）得分：0x42e380 case2 公式（asm 0x42e9e5-0x42ea94）：
    // item state==3 或 y<=128 → floor(max_point/100, 10)，min 10；否则
    // round10(floor(3D/4) + floor3/4(3(A%10-A))·(ftol(y)-128)/450 + 负项补偿)。
    i32 point_value(float player_y, bool item_homing) const;

private:
    void home_step(Item& item, const ItemCollector& player);
    void collect_check(Item& item, const ItemCollector& player);
    void push_level_up(const Item& item);
    float angle_to_player(const Spirit& s, const ItemCollector& player);
    void collect_spirit(Spirit& s, const ItemCollector& player);
    void add_max_point(float displayed_increment);  // 0042f510: ftol(value * 100), cap.

    GameRandom* random_ = nullptr;
    const SessionState* session_ = nullptr;
    std::array<Item, kItemCapacity> items_{};
    std::array<Spirit, kSpiritCapacity> spirits_{};
    std::vector<ItemEvent> events_;
    ItemCollector last_player_{};
    u32 spirit_serial_ = 0;  // 神霊池 id 计数（0x438ec0 +0x8820；象限 = id&3）
    // 0x438ec0 尾溢出计数（mgr+0x8838）：kind∉{3,4} 产件后 +1，>9 时同位补
    // 一件 kind3（base 0.5，消耗两次 RNG）。**窗口语义（0x438660:31-45）**：
    // 同一非 3/4 产件同时置 mgr+0x8828 弹窗计数 60，驱动趟每帧按 rate 递减，
    // 归零后溢出计数清 0——即「60 帧内连产 >10 件才溢出补 kind3」，非单调累计
    //（r4 勘误：此前按跨阶段单调累计实现是错的）。
    i32 spirit_overflow_ = 0;
    i32 spirit_window_ = 0;  // mgr+0x8828（0x438ec0:132 置 0x3c=60）
    Timer spirit_window_clock_;
    // 0x42f020 type9 出场延迟源：0x79dcf8（累计，0x413e50 构造清零）与
    // 0x79dcfc（每帧，0x42e380:74 每步清零）。读式（0042f020.c:79-107）：
    // 波数<0x100 → total&3；<0x200 → (total&7)+4；<0x400 → (total&0xf)+8；
    // 否则 (total&0x1f)+0x10。42f020仅读；不能按生成次数自行递增。
    std::vector<i32> ordinary_free_, cancel_free_;
    void retire_item(Item& item);
    i32 cancel_total_ = 0;
    i32 cancel_wave_ = 0;
    i32 point_items_ = 0;    // 0x4be7d8 点道具收集计数
};

}  // namespace th13
