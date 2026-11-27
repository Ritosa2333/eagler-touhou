// th16 炸弹（Bomb，DAT_004a6da8）与季节解放（Release，DAT_004a6da4）：两只 0x108 字节的对象，0x40d890 在装载
// 关卡时按机体 / 子季节各建一只，整局常驻，0x40da90 在离开游戏（0x42d200）时释放。
//
// 生命周期与链优先级：
//   0x40d600  初始化：更新链 0x18（炸弹）/ 0x19（解放）→ 0x40dd00；绘制链 0x28 / 0x29 → 0x40da70（奖励飘字）。
//   0x40dda0  可用判定（Player 0x442560 每帧查询：炸弹看残弹，解放看季节等级与冷却；两者都要求双方未在发动、
//             无对话、场上有敌机）。
//   0x40db20  发动：残弹 -1 / 季节量表结算、音效、各机体 on_start，再由 EnemyManager 计数器登记。
//   0x40dd00  每帧：计时为负时（解放结束后的 45 帧冷却）先推进，再调各机体 on_update，返回非零即失活。
//   0x40e040  解放结束：飘字计时 0xb4、炸弹计时 -45（冷却）。
//
// 机体实现（ZUN 的类名见各处注释，oracle 的 class 键沿用）：
//   灵梦  ReimuBomb   0x410d10 / 0x410de0 / 0x411280 / 0x411320   八枚阴阳玉绕自机扩散后索敌直飞、到点引爆
//   琪露诺 CirnoBomb  0x40f070 / 0x40f240 / 0x40f4c0              以自机为心的扩散圆（两段半径增长）
//   文    AyaBomb     0x40e780 / 0x40e9d0 / 0x40ec10              横扫全屏的一阵风（640×118 旋转矩形伤害 + 消弹）
//   魔理沙 MarisaBomb 0x40f930 / 0x40fb00 / 0x40fe80              随左右输入摆动的巨型光束（三段矩形伤害）
//   季节解放 SeasonRelease：0x40d890 按子季节选类——春 ReimuSub 0x411460 / 夏 CirnoSub 0x40f590 / 秋 AyaSub 0x40ec70 /
//             冬 MarisaSub 0x410150 / 土用 AllSub 0x40e0f0（类名随 RTTI；与 pl0Nsub.anm 的机体编号一致），
//             以子机 ANM 的两层光环为判定，参数查表。
//
// 与相邻系统的交互：
//   - Player：伤害源池（0x444b20 矩形 / 0x4449b0 圆形，flags & 4 = 炸弹伤害）、无敌计时、移动倍率 / 伤害倍率；
//   - BulletManager / LaserManager：范围消弹（圆 0x416d20 / 0x431a70，旋转矩形 0x416e20 / 0x431950）；
//   - EffectManager：爆点粒子（种类 3）；ScreenEffect：震动（kind 1 线性 / kind 8 包络）经宿主登记链 0x13；
//   - Spellcard：符卡中发动置 kBombUsed（取得失败），符卡开始 60 帧内发动则直接清取得标志；
//   - ItemManager / EnemyManager / BulletManager 读「是否在发动」与解放等级：由 GameSession 每帧镜像。
#pragma once

#include "AnmVm.hpp"
#include "Motion.hpp"
#include "Sound.hpp"
#include "Types.hpp"

#include <array>
#include <memory>
#include <vector>

namespace th16 {

class AnmManager;
class AsciiManager;
class BulletManager;
class EffectManager;
class EnemyManager;
struct GameGlobals;
class LaserManager;
class Player;
struct PlayerWorld;
struct Spellcard;

// 0x45d150(kind, ...) 画面震动请求：宿主在炸弹链之后登记为链 0x13 的 ScreenShake / ScreenShakeEnvelope
struct BombShakeRequest {
    enum Kind : i32 { kLinear = 1, kEnvelope = 8 };
    Kind kind;
    // kLinear：duration / amp_from / amp_to；kEnvelope：amplitude / attack / sustain / decay
    i32 a, b, c, d;
};

// 炸弹与解放共用的宿主对象与输出队列（原版经全局变量访问）
struct BombContext {
    AnmManager* animations = nullptr;    // DAT_004c0f48
    Player* player = nullptr;            // DAT_004a6ef8
    GameGlobals* globals = nullptr;
    EnemyManager* enemies = nullptr;     // DAT_004a6dc0
    BulletManager* bullets = nullptr;    // DAT_004a6dac
    LaserManager* lasers = nullptr;      // DAT_004a6ee0
    EffectManager* effects = nullptr;    // DAT_004a6db8
    Spellcard* spellcard = nullptr;      // DAT_004a6db0
    const PlayerWorld* world = nullptr;  // Gui+0x1c8 对话中
    class Gui* gui = nullptr;            // DAT_004a6dcc：残弹显示 0x42c390、季节量表显示 0x42c600
    class Bomb* main = nullptr;          // DAT_004a6da8
    class Bomb* release = nullptr;       // DAT_004a6da4

    std::vector<i32> sfx_queue;                   // 0x45e150 / 0x45e1f0
    SoundRequests* sfx_sink = nullptr;
    void play_sfx(i32 id, float x = 0.0f) { if (sfx_sink != nullptr) sfx_sink->push(id, x); else sfx_queue.push_back(id); }
    std::vector<BombShakeRequest> shake_requests;  // 0x45d150(1 / 8)
};

// BombInf 基类（vftable 0x491d40）：+0x08 / +0x0c 两条链、+0x14 位置、+0x30 活动、+0x34 计时……
class Bomb {
public:
    enum class Role : i32 { Main = 0, Release = 1 };  // +0xd4

    explicit Bomb(BombContext& context) : ctx(context) {}
    virtual ~Bomb() = default;

    // 0x40d600 第二段：计时复位、种类、飘字计时、显示奖励 -1
    void init(Role role);
    // 0x40dda0
    bool usable() const;
    // 0x40db20
    void start();
    // 0x40dd00（更新链 0x18 / 0x19）
    void update(float game_speed);
    // 0x40da70 → 0x40de30（绘制链 0x28 / 0x29）：解放奖励飘字 "+N" 入 AsciiManager 队列，其计时只在绘制链里推进
    void draw_popup(AsciiManager& text, float game_speed);
    // 0x42e150 过关：炸弹仍在发动时调 vtable+0x14（只有灵梦有实质动作）
    void on_stage_clear() { if (active) on_stage_end(); }

    virtual const char* class_name() const = 0;   // oracle BOMB_VTABLES 键
    // 无 RTTI 构建下给快照 / 对拍用：只有灵梦炸弹返回自身（阴阳玉数组）
    virtual const class ReimuBomb* as_reimu() const { return nullptr; }

    Role role = Role::Main;      // +0xd4
    Vec3 pos;                    // +0x14
    float sweep_speed = 0;       // +0x20：文的一阵风每帧横移量
    float angle = 0;             // +0x2c
    i32 active = 0;              // +0x30
    Timer timer;                 // +0x34
    u32 primary_vm = 0;          // +0x5c
    u32 secondary_vm = 0;        // +0x64
    i32 started_in_spell = 0;    // +0x68：符卡计时 > 0x3b 时发动（取得失败）
    i32 release_level = 0;       // +0xd8：解放发动时的季节等级（1..6；BulletManager 消弹附加道具 = 等级 + 8）
    float release_bonus = 0;     // +0xdc：解放期间拾取的季节得点累计（ItemManager 直接累加）
    float shown_bonus = -1.0f;   // +0xe0：飘字显示的奖励（-1 = 未定）
    Timer popup_timer;           // +0xe4
    i32 popup_level = 0;         // +0xf8
    Vec3 popup_pos;              // +0xfc

protected:
    // 各机体 vtable：+0x00 发动 / +0x04 每帧（非零 → 失活）/ +0x10 每帧消弹 / +0x14 过关收尾
    virtual void on_start() = 0;
    virtual i32 on_update() = 0;
    virtual void on_cancel() = 0;
    virtual void on_stage_end() {}

    // 0x40e040：解放结束的飘字与冷却
    void finish_release();
    // 主炸弹发动时对符卡的处理（0x410d10 / 0x40e780 / 0x40f070 / 0x40f930 共有段）
    void mark_spellcard_bomb_use();
    void grant_invincibility(i32 frames);
    void request_linear_shake(i32 duration, i32 amp_from, i32 amp_to);
    void request_envelope_shake(i32 amplitude, i32 attack, i32 sustain, i32 decay);
    void play_sfx(i32 id, float x = 0.0f) { ctx.play_sfx(id, x); }
    // 0x40e730(3, pos) + flags_lo 改混合模式 0x20：琪露诺 / 文的随机爆点
    void spawn_burst_particle(const Vec3& at);
    AnmVm* find_vm(u32& id) const;   // 0x46efa0；找不到时把 id 清零

    BombContext& ctx;
    float game_speed_ = 1.0f;        // DAT_004a5788（update 入口抄入，供阴阳玉计时 / 运动用）
};

// ---------------------------------------------------------------------------------------------
// 主炸弹

// BombReimuAInf（vftable 0x491e3c）：梦符「夢想封印」——八枚阴阳玉
class ReimuBomb final : public Bomb {
public:
    using Bomb::Bomb;
    const char* class_name() const override { return "ReimuA"; }
    const ReimuBomb* as_reimu() const override { return this; }

    // 0xd8 字节一枚（+0x70 指向的 8 枚数组，发动时分配、结束时释放）
    struct Orb {
        u32 vm = 0;                  // +0x00（player.anm 脚本 0xf）
        MotionState motion;          // +0x04（mode 2 绕自机；到点后 mode 0 直飞）
        i32 active = 0;              // +0xa0
        Timer timer;                 // +0xa4
        Vec3 delta;                  // +0xb8：上一帧位移（转直飞时据此定初速与方向）
        u32 target_id = 0;           // +0xc4：索敌到的敌机 id
        i32 index = 0;               // +0xcc
        i32 damage_source = 0;       // +0xd0：自机伤害源下标（1 起）
    };
    std::vector<Orb> orbs;           // +0x70（空 = 未分配）

protected:
    void on_start() override;        // 0x410d10
    i32 on_update() override;        // 0x410de0
    void on_cancel() override;       // 0x411280
    void on_stage_end() override;    // 0x411320

private:
    void init_orb(Orb& orb, i32 index);      // 0x4109d0
    void step_orb(Orb& orb);                 // 0x410550
    void steer_orb(Orb& orb);                // 0x410550 直飞段的索敌转向
    void detonate_orb(Orb& orb);             // 0x410ae0
    void release_orbs();                     // 0x410bb0
};

// BombCirnoAInf（vftable 0x491e04）：以自机为心的扩散冰圆
class CirnoBomb final : public Bomb {
public:
    using Bomb::Bomb;
    const char* class_name() const override { return "CirnoA"; }

protected:
    void on_start() override;        // 0x40f070
    i32 on_update() override;        // 0x40f240
    void on_cancel() override;       // 0x40f4c0
};

// BombAyaAInf（vftable 0x491de8）：横扫全屏的一阵风
class AyaBomb final : public Bomb {
public:
    using Bomb::Bomb;
    const char* class_name() const override { return "AyaA"; }

protected:
    void on_start() override;        // 0x40e780
    i32 on_update() override;        // 0x40e9d0
    void on_cancel() override;       // 0x40ec10
};

// BombMarisaAInf（vftable 0x491e20）：恋符「マスタースパーク」——随输入摆动的巨型光束
class MarisaBomb final : public Bomb {
public:
    using Bomb::Bomb;
    const char* class_name() const override { return "MarisaA"; }

protected:
    void on_start() override;        // 0x40f930
    i32 on_update() override;        // 0x40fb00
    void on_cancel() override;       // 0x40fe80
};

// ---------------------------------------------------------------------------------------------
// 季节解放：以子机 ANM（player.sub_anm）两层光环为判定；五个子季节只差参数表与少数附加效果

struct SeasonReleaseProfile {
    const char* class_name;          // ZUN 类名（oracle 键）
    i32 inner_script;                // 内层光环脚本（+0x5c）
    i32 outer_script;                // 外层光环脚本（+0x64）
    const float* radius_table;       // 按等级（0..6）写入光环 $F0 的半径
    const float* radius2_table;      // 写入 $F1 的半径（多数与 radius_table 同表）
    const i32* duration_table;       // 写入 $I3 的持续帧数
    float outer_growth;              // 外层半径 = 内层 + 该值（冬改为 × 1.2，见 outer_scaled）
    bool outer_scaled;
    i32 start_invincible;            // 发动时的无敌帧数
    i32 damage;                      // 每帧圆形伤害源的伤害
    bool keep_invincible;            // 春：每帧把无敌续到 50 帧
    bool follows_player;             // 秋：光环每帧贴到自机坐标，且移动倍率 1.5
    bool damage_boost;               // 冬：自机伤害倍率 1.5（EnemyManager 0x41b3d0 读 scale2 后清回 1）
    bool cancel_first_16_only;       // 春：只在前 16 帧消弹
};

class SeasonRelease final : public Bomb {
public:
    SeasonRelease(BombContext& context, const SeasonReleaseProfile& profile) : Bomb(context), profile_(profile) {}
    const char* class_name() const override { return profile_.class_name; }

    // 0x40d890 switch(DAT_004a57ac)：0 春 / 1 夏 / 2 秋 / 3 冬 / 4 土用（其它值按春）
    static const SeasonReleaseProfile& profile_for_season(i32 season);

protected:
    void on_start() override;        // 春 0x411460 / 夏 0x40f590 / 秋 0x40ec70 / 冬 0x410150 / 土用 0x40e0f0
    i32 on_update() override;        // 春 0x4116a0 / 夏 0x40f7d0 / 秋 0x40eeb0 / 冬 0x4103e0 / 土用 0x40e330
    void on_cancel() override;       // 春 0x411790 / 夏 0x40f860 / 秋 0x40efa0 / 冬 0x410480 / 土用 0x40e3c0

private:
    u32 spawn_aura(i32 script, float radius, float radius2, i32 duration);
    const SeasonReleaseProfile& profile_;
};

// ---------------------------------------------------------------------------------------------
// 0x40d890 / 0x40da90：两只对象的创建与释放；GameSession 把链 0x18 / 0x19 / 0x28 / 0x29 接到 update_* / draw_*
class BombManager {
public:
    // 0x40d890：按 globals.shot + shot_aux 建主炸弹、按 globals.season 建解放；原版失败路径（new 失败）不会发生
    void create(BombContext context);
    // 0x40da90
    void destroy();

    Bomb* main() const { return main_.get(); }
    Bomb* release() const { return release_.get(); }
    BombContext& context() { return ctx_; }

    void update_main(float game_speed) { if (main_) main_->update(game_speed); }
    void update_release(float game_speed) { if (release_) release_->update(game_speed); }
    void draw_main(AsciiManager& text, float game_speed) { if (main_) main_->draw_popup(text, game_speed); }
    void draw_release(AsciiManager& text, float game_speed) { if (release_) release_->draw_popup(text, game_speed); }

    bool main_active() const { return main_ && main_->active == 1; }
    bool release_active() const { return release_ && release_->active == 1; }

private:
    BombContext ctx_;
    std::unique_ptr<Bomb> main_;
    std::unique_ptr<Bomb> release_;
};

}  // namespace th16
