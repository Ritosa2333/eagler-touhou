// th16 ItemManager（DAT_004a6ddc，0x1c972ec 字节）与道具 Item（0xc78 字节，0x1258 槽）：
//   0x42f440 / 0x42f0b0 / 0x42f260 / 0x4184a0  构造（两段池各 0x1258 项；只用第一段）、更新链 0x1d（0x4308f0→0x42f4e0）、
//                                            绘制链 0x21 / 0x13（0x430920 / 0x430940 → 0x4307a0）、首帧复位（空闲链重建）
//   0x430960 / 0x430d10 / 0x430df0            生成：普通道具（1–8、0xf→6）取普通池；得点 / 季节道具（9–0xe、0x10）取得点池并
//                                            以生成计数决定出现延迟
//   0x42f4e0                                  每帧：状态 1 下落 / 2 上抛后归位 / 3 季节道具减速 / 4 吸向自机 / 5 收点圈吸引 /
//                                            6 延迟出现；拾取判定、吸引圈判定、VM 推进、计时
//   0x430100 / 0x430620 / 0x4304a0 / 0x4303a0  火力 / 得点 / 大火力 / 满火力拾取结算；0x43df70 残机、0x43dff0 / 0x43dfb0 炸弹、
//   0x430dc0 / 0x43de50                       得点值累加、季节量表推进
//   0x430c90 / 0x430d90                       得点池道具出现（绑定脚本）/ 回收到空闲链头
//
// 链表语义：两个空闲链（普通 +0xe4b954、得点 +0xe4b964 哨兵）都是头插的双向链表，0x4184a0 依下标顺序头插，
// 故最先分配的是下标最大的槽；释放的槽回到链头被下一次生成复用。更新与绘制按数组下标顺序遍历。
// 这里用侵入式指针逐字还原，保证槽分配次序、遍历次序与 oracle 的 items 快照一致。
#pragma once

#include "AnmVm.hpp"
#include "Sound.hpp"
#include "Types.hpp"
#include <vector>

namespace th16 {

class AnmManager;
class AnmRenderer;
struct AnmFile;
struct GameGlobals;
class Player;
struct Rng;

// 道具种类（+0xc54）
enum ItemType : i32 {
    kItemPower = 1,        // 火力（0x430100）
    kItemPoint = 2,        // 得点（0x430620）
    kItemBigPower = 3,     // 大火力（0x4304a0）
    kItemLifeFragment = 4, // 无结算（仅音效 0x25）
    kItemLife = 5,         // 残机（0x43df70）
    kItemBombFragment = 6, // 炸弹碎片（0x43dff0）
    kItemBomb = 7,         // 炸弹（0x43dfb0）
    kItemFullPower = 8,    // 满火力（0x4303a0）
    kItemSeasonValue0 = 9, // 9–0xe：季节得点（值表 DAT_004917bc：5/10/20/30/40/50）
    kItemSeasonValue5 = 0xe,
    kItemBombFragmentAlt = 0xf,  // 生成时转为 6 并计数 DAT_004a587c
    kItemSeason = 0x10,    // 季节量表（0x43de50）
};

struct Item {
    // +0x00 链表结点：{哨兵, next, prev}
    Item* next = nullptr;          // +0x04
    Item* prev = nullptr;          // +0x08
    AnmVm vm;                      // +0x10 主 VM（bullet.anm 脚本 DAT_004917f8[type*2]）
    AnmVm vm2;                     // +0x60c 副 VM（DAT_004917f8[type*2+1]；屏幕上方指示用）
    Vec3 pos;                      // +0xc08
    Vec3 velocity;                 // +0xc14
    float speed = 0;               // +0xc20 状态 3 的减速速度
    float angle = 0;               // +0xc24 状态 3 的方向
    Timer timer;                   // +0xc28
    Timer timer2;                  // +0xc3c（未用）
    i32 state = 0;                 // +0xc50 0 空 / 1 下落 / 2 上抛 / 3 季节减速 / 4 吸向自机 / 5 收点圈吸引 / 6 延迟出现
    i32 type = 0;                  // +0xc54
    i32 type_copy = 0;             // +0xc58（得点池 = type；普通池 0）
    float attract_speed = 0;       // +0xc5c
    i32 counter = 0;               // +0xc60 得点池出现延迟（按生成计数取模）
    i32 serial = 0;                // +0xc64 = 管理器 +0x1c972e8
    i32 flag = 0;                  // +0xc68 0x430960 末参数（1 = 解放 / 炸弹消弹所生，状态 3 结束时直接吸引）
    i32 above_screen = 0;          // +0xc6c 0x4307a0：1 = 画屏幕上方指示（副 VM）
    i32 index = 0;                 // 池下标（移植附加；oracle 快照以下标标识）
};

class ItemManager {
public:
    static constexpr int kNormalSlots = 600;       // 普通池（+0x14 起）
    static constexpr int kPointSlots = 0x1000;     // 得点池（普通池之后）
    static constexpr int kSlots = kNormalSlots + kPointSlots;  // 0x1258
    static constexpr float kGravity = 0.03f;
    static constexpr float kMaxFallSpeed = 2.0f;
    static constexpr float kMaxAttractSpeed = 12.0f;
    static constexpr float kAttractAccel = 0.2f;
    static constexpr float kBottomLimit = 472.0f;  // 0x4946b8
    static constexpr float kSideLimit = 200.0f;

    std::vector<Item> items;           // +0x14 起 0x1258 项
    Item* normal_free = nullptr;       // +0xe4b958 普通池空闲链头
    Item* point_free = nullptr;        // +0xe4b968 得点池空闲链头
    float attract_scale = 1.0f;        // +0xe4b974（th16 无写入路径，恒 1.0；< 1 时按 0.1/帧回升）
    i32 active_count = 0;              // +0x1c972dc 本帧活动道具数
    i32 spawn_count = 0;               // +0x1c972e0 0x430960 调用计数
    i32 point_active = 0;              // +0x1c972e4（0x4184a0 清零，更新里每帧清零；决定延迟模式）
    i32 serial = 0;                    // +0x1c972e8

    // 宿主同步的外部状态（原版直接读其它管理器）
    bool bomb_attract = false;         // DAT_004a6da8->0x30 == 1 && +0x38 < 0x3c（炸弹开始 60 帧内全收）
    bool release_attract = false;      // DAT_004a6da4->0x30 == 1 && +0x38 < 10000（季节解放中全收）
    bool dialogue = false;             // DAT_004a6dcc->0x1c8 != 0（对话中全收）
    bool focus_held = false;           // DAT_004a52c8 & 8（低速时收点圈取擦弹框）
    float* release_bonus = nullptr;    // DAT_004a6da4->0xdc：拾取季节得点时无条件累加到解放的奖励（飘字 "+%d"）
    std::vector<i32> sfx_queue;        // 0x45e150 / 0x45e1f0 音效登记（宿主每帧取走）
    SoundRequests* sfx_sink = nullptr;
    void play_sfx(i32 id, float x = 0.0f) { if (sfx_sink != nullptr) sfx_sink->push(id, x); else sfx_queue.push_back(id); }
    class ScorePopupManager* score_popups = nullptr;   // DAT_004a6f10：拾取得分飘字（0x44a460）
    class Gui* gui = nullptr;          // DAT_004a6dcc：满火力 / extend 弹出（0x42bcf0）、残机 / 炸弹格 / 季节量表显示刷新

    AnmManager* animations = nullptr;  // DAT_004c0f48
    AnmFile* anm = nullptr;            // DAT_004a6dac->0x1403b24 bullet.anm（槽 7）
    AnmFile* effect_anm = nullptr;     // DAT_004a6db8->0xc effect.anm（槽 8，出现特效脚本 0x65）
    GameGlobals* globals = nullptr;
    Player* player = nullptr;          // DAT_004a6ef8
    Rng* rng = nullptr;                // DAT_004a6d80（attract_scale < 1 时 VM 抖动用）

    // 0x42f440 → 0x42f0b0 + 0x42f260（含 0x4184a0 首次复位）
    void create(AnmManager& anm_manager, AnmFile* bullet_anm, AnmFile* effect, GameGlobals& g, Player* pl, Rng& game_rng);
    // 0x4184a0：清池、重建两条空闲链、attract_scale = 1
    void reset();
    // 0x430960：生成；返回槽（池空时 nullptr）。type 0xf 记为 6 并 DAT_004a587c++
    Item* spawn(i32 type, const Vec3& pos, float angle, float speed, i32 flag);
    // 0x42f4e0：更新链 0x1d（0x4308f0：GameManager 装载 / 结束 / 0x400 冻结时跳过——由宿主判断）
    void update(float game_speed);
    // 0x4307a0：绘制链 0x21（season_pass = false：除 0x10 外）/ 0x13（season_pass = true：仅 0x10）。道具 VM 不挂图层：
    //   entity_offset 同步后直接画主 VM；飘在场地上方时改画屏幕上沿的指示副 VM（y 钳到 8、按距离淡入）
    void draw(AnmRenderer& renderer, bool season_pass);

private:
    // 0x430d10：类型 4/5/6/7（含 0xf）出现特效 0x65 + 音效 0x4a（4/5）/ 0x30
    void spawn_effect(Item& it);
    // 0x430c90：得点池道具延迟结束——绑定脚本（0x10 按季节 DAT_004a57ac + 0x81、状态 3；其余 DAT_004917f8、状态 2）
    void appear(Item& it);
    // 0x430d90：回收到所属空闲链头
    void release(Item& it);
    void push_front(Item*& head, Item& it);
    void unlink(Item*& head, Item& it);                  // 0x405610
    void bind_script(AnmVm& vm, int script);           // 0x40d460：模板复制 + 回调清零 + 首帧
    void collect(Item& it);                             // 0x42f4e0 拾取分支（按种类结算 + 音效 0x25）
    void collect_power(Item& it);                       // 0x430100
    void collect_point(Item& it);                       // 0x430620
    void collect_big_power(Item& it);                   // 0x4304a0
    void collect_full_power(Item& it);                  // 0x4303a0
    bool add_life();                                    // 0x43df70
    void add_bomb_fragment();                           // 0x43dff0
    void add_bomb();                                    // 0x43dfb0
    void notify_full_power();                           // 0x42bcf0(0, 2)：火力到顶时的弹出
    void add_point_value(float value);                  // 0x430dc0
    bool add_season_gauge();                            // 0x43de50
    void add_score(i32 value);                          // 0x43e080（extend 时 Gui::celebrate_extend）
    void popup(const Vec3& pos, i32 value, u32 color);  // 0x44a460（value < 0 → 特殊字形）
    void record_collect(i32 value);                     // DAT_004a5860/5868/5870 自动收点统计
    i32 point_line() const;                             // 128（魔理沙 148）：自机 y 低于此线自动收点
    i32 scaled_point_value(float player_y, i32 line) const;  // 0x430100 / 0x430620 的按高度折算
    i32 full_point_value() const;                       // (pv/100 − pv/100%10)/10*10，下限 10
    float angle_to_player(const Vec3& from) const;      // 0x443840：重合时 π/2
};

}  // namespace th16
