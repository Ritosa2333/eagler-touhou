// th16 BulletManager（DAT_004a6dac，0x1403b28 字节）与敌弹 Bullet（0x1478 字节，2000 槽）：
//   0x411dd0 / 0x411880 / 0x411a30 / 0x411b70  构造、bullet.anm（槽 7）、2000 槽 LIFO 空闲链、更新链 0x1c / 绘制链 0x25
//   0x414da0 / 0x412cb0                        EtShooter 弹幕生成（way × stack、13 种瞄准模式、近身安全半径、VM 与消弹精灵表）
//   0x412860 / 0x411e70 / 0x412670             每帧推进（状态 1/2/3/4/5、Ex 步骤、移动、出界回收、VM tick、图层挂链）
//   0x413860 / 0x414ec0..0x4162d0              18 槽 Ex 弹幕变换指令集（加速、角速度、转向停止、反弹、穿墙、缓动、再发射、改外观等）
//   0x4124b0 / 0x4438c0 / 0x4439e0 / 0x444cf0  自机被弹与擦弹判定
//   0x416840 / 0x416a00 / 0x416c20 / 0x416d20 / 0x416e20 / 0x416f40  消弹与季节/得点道具转化
//   0x417140                                   sprite_mapper 1：按弹种/颜色重映射 ins_300 精灵号
//
// 链表语义：原版空闲链（+0x5c 哨兵）与活动链（+0x6c 哨兵）都是头插的双向链表——新弹总在活动链头，
// 0x412860 从头到尾遍历（最新的先更新），释放的槽回到空闲链头被下一次生成复用。这里用侵入式指针逐字还原，
// 保证槽下标分配顺序与遍历次序和原版一致（oracle 的 bullets 快照按活动链顺序导出）。
#pragma once

#include "AnmVm.hpp"
#include "Sound.hpp"
#include "Types.hpp"
#include <functional>
#include <vector>

namespace th16 {

class AnmManager;
class AnmRenderer;
struct AnmFile;
struct AnmRuntimeSprite;
struct GameGlobals;
class ItemManager;
class Player;
struct PlayerWorld;
struct Rng;

// 单个 Ex 变换步骤（0x2c 字节 = 11 × 4 字节；ECL 609..612 写入，0x413860 解释）
struct BulletExStep {
    float r = 0;           // +0x00 float 参数 0
    float s = 0;           // +0x04 float 参数 1
    float m = 0;           // +0x08 float 参数 2
    float n = 0;           // +0x0c float 参数 3
    i32 a = 0;             // +0x10 int 参数 0
    i32 b = 0;             // +0x14 int 参数 1
    i32 c = 0;             // +0x18 int 参数 2
    i32 d = 0;             // +0x1c int 参数 3
    u32 type = 0;          // +0x20 步骤类型（单一位；0 = 空槽）
    i32 async = 0;         // +0x24 1 = 不等待前序 Ex 结束即生效
    const char* str = nullptr; // +0x28 ECL 640 写入的子程序名（Ex 0x1000000 生成敌机用）
};

// Ex 步骤类型（type 字段），按 0x413860 的分派
enum BulletEx : u32 {
    kExInitialBoost = 0x1u,      // 0x414ec0：生成后 16 帧内速度额外 +5→0 衰减
    kExSpawnFog = 0x2u,          // 中断 a+7、状态 2（雾化生成）、位置回退 4 帧
    kExAccel = 0x4u,             // 0x414fb0：沿角度 s 以加速度 r 推 a 帧
    kExAccelTurn = 0x8u,         // 0x4153e0：加速度 r + 角速度 s，a 帧
    kExStopTurn = 0x10u,         // 0x415570：a 帧内减速到停，再按模式 c 转向、速度 s，重复 b 次
    kExBounce = 0x40u,           // 0x415bb0：版边反弹（掩码 b，最多 a 次，新速度 r）
    kExImmunity = 0x80u,         // +0x24 = a 帧禁消弹
    kExBoundsGuard = 0x100u,     // 0x4162d0：a 帧内屏蔽出界回收（b≠0 时附带射线判断）
    kExChangeType = 0x200u,      // 改弹种 a / 颜色 b（重建 VM、判定、消弹脚本）
    kExCancel = 0x400u,          // 消弹（a==1 时不出消弹特效）
    kExSfx = 0x800u,             // 音效 a
    kExWrap = 0x1000u,           // 0x415d80：穿墙环绕（掩码 b，最多 a 次）
    kExReshoot = 0x2000u,        // 以本弹为发射点再发射（参数取本步与下一步）
    kExParam = 0x8000u,          // +0xc4c = a
    kExLoop = 0x10000u,          // 跳到 Ex 下标 a，循环 b 次
    kExMoveTo = 0x20000u,        // 0x415f90：a 帧插值移动到 (r,s)（b&0x100 相对），模式 b
    kExSetAngle = 0x40000u,      // 角度 r（≥990 自机狙 + m）、速度 s
    kExExtraVelocity = 0x80000u, // 0x4161f0：附加速度 polar(r,s) 持续 a 帧
    kExBlend = 0x100000u,        // 主 VM 混合模式 = a
    kExSpeedEase = 0x200000u,    // 0x4151e0：a 帧内加速到速率 r（朝角度 s / 自机狙 + m）
    kExScale = 0x400000u,        // 缩放 r→s，a 帧，模式 b
    kExSavePose = 0x800000u,     // 保存坐标/角度/速度
    kExSpawnEnemy = 0x1000000u,  // 0x41aa70 生成敌机（名字取 str）
    kExLayer = 0x2000000u,       // 绘制桶 = a
    kExHide = 0x4000000u,        // 隐身暂停 a 帧
    kExLaser = 0x8000000u,       // 生成激光（a==0 直线 0x431130 / a==1 照射 0x431860）
    kExHitbox = 0x20000000u,     // 判定 = r（<0 取弹种表）
    kExWait = 0x80000000u,       // 等待 a 帧
};

// EnemyInf+0x598：发射器槽（0x380 字节，共 16 槽；ECL 609..612 的槽号按 0..17 裁剪）
struct EtShooter {
    i16 type = 0;          // +0x00 弹种（0..43）
    i16 pad02 = 0;
    i16 color = 0;         // +0x04 颜色（0..15）
    i16 pad06 = 0;
    Vec3 pos;              // +0x08 发射坐标（ECL 601 解析后写入）
    float angle1 = 0;      // +0x14 主角度
    float angle2 = 0;      // +0x18 副角度 / 扇形间隔
    float speed1 = 2.0f;   // +0x1c 主速度
    float speed2 = 0;      // +0x20 副速度
    float distance = 0;    // +0x24 发射点沿角度的偏移距离（ECL 627）
    BulletExStep ex[18]{}; // +0x28..0x33f
    u8 extra[0x24]{};      // +0x340..0x363（激光参数区，LaserManager 使用）
    i16 way = 1;           // +0x364 列数
    i16 stack = 1;         // +0x366 层数
    i16 aim_mode = 0;      // +0x368 瞄准模式（0..12）
    i16 pad36a = 0;
    u32 flags = 0x23u;     // +0x36c 弹初始 ex_flags（bit 5 = 发射时播放音效）
    i32 shoot_sfx = 0x15;  // +0x370 发射音效
    i32 step_sfx = 0x26;   // +0x374 Ex 变换音效
    i32 start_ex = 0;      // +0x378 起始 Ex 下标
    i32 pad37c = 0;

    // ECL 600：清零后 angle1=0、speed1=2、way=stack=1、flags=0x23、音效 0x15/0x26
    void reset() noexcept;
};

// 0x49f2e0：弹种属性表项（0x114 字节，44 个有效弹种）
struct BulletTypeInfo {
    i32 script = 0;        // +0x00 bullet.anm 脚本号
    i32 sprite_map[64]{};  // +0x04..0x103：16 色 × 4 精灵重映射（[0] < 0 时不映射）
    float hitbox = 0;      // +0x104 判定直径
    i32 default_layer = 0; // +0x108 绘制桶（0..5）
    i32 cancel_kind = 0;   // +0x10c 消弹特效脚本选择方式（0..10）
    i32 extra_script = 0;  // +0x110 附加 VM 脚本号（0 = 无）
};

const BulletTypeInfo& bullet_type_info(i32 type) noexcept;

// 0x411940：单颗敌弹（原版 0x1478 字节）
struct Bullet {
    enum : u32 {
        kFlagActive = 0x1u,        // bit0 在用
        kFlagCollidable = 0x2u,    // bit1 可与自机判定
        kFlagGrazed = 0x4u,        // bit2 已擦弹（每弹一次）
        kFlagReleased = 0x8u,      // bit3 待回收
        kFlagCircle = 0x10u,       // bit4 圆形判定（消弹脚本为 -1 的弹种）
        kFlagScaled = 0x40u,       // bit6 缩放中（Ex 0x400000）
        kFlagForceGraze = 0x100u,  // bit8 强制擦弹（不判被弹）
        kFlagHidden = 0x200u,      // bit9 隐身（不绘制、不移动、不挂图层）
    };

    Bullet* free_next = nullptr;   // +0x04/+0x08 空闲链结点
    Bullet* active_next = nullptr; // +0x14/+0x18 活动链结点
    Bullet* active_prev = nullptr;
    u32 flags = 0;              // +0x20
    i32 cancel_immunity = 0;    // +0x24 禁消弹倒计时（Ex 0x80）
    AnmVm vm_main;              // +0x28 主精灵 VM（sprite_mapper=1）
    AnmVm vm_extra;             // +0x624 附加光环 VM
    u32 extra_vm_flags = 0;     // +0xb54（bit0 = 拥有附加 VM）
    Vec3 pos;                   // +0xc20
    Vec3 velocity;              // +0xc2c
    float speed = 0;            // +0xc38
    float angle = 0;            // +0xc3c
    Vec2 hitbox;                // +0xc40 判定宽高 / 直径
    i32 index = 0;              // +0xc48 池槽下标（0..1999）
    i32 ex_param = 0;           // +0xc4c Ex 0x8000 参数
    i32 spawn_grace = 0;        // +0xc58 出界豁免帧（初值 5）
    i32 cancel_script = -1;     // +0xc5c 消弹特效脚本号（bullet.anm）
    i32 ex_index = 0;           // +0xc60 当前 Ex 步骤下标
    i32 ex_loop_count = 0;      // +0xc64 Ex 0x10000 循环计数
    u32 ex_flags = 0;           // +0xc68 当前活动 Ex 标志位
    u32 initial_ex_flags = 0;   // +0xc6c 发射器 flags 备份
    u16 state = 0;              // +0xc72 0 空闲 / 1 正常 / 2 雾化生成 / 3 被弹消散 / 4 消弹 / 5 特殊消散
    Bullet* layer_next = nullptr; // +0xc78 本帧绘制桶链表后继
    i32 unknown_c7c = 0;        // +0xc7c
    i32 step_sfx = -1;          // +0xc80 Ex 变换音效
    i32 layer_bucket = 0;       // +0xc84 所属绘制桶（0..5）
    BulletExStep ex[18]{};      // +0xc88..0xf9f

    // —— 各 Ex 状态机工作区 ——
    Timer ex1_timer;            // +0xfa0
    i32 ex1_extra = 0;          // +0xfd0（Ex 0x1 置 0；0x414ec0 不读）

    Timer ex4_timer;            // +0xfe8
    float ex4_acc = 0;          // +0xffc
    float ex4_angle = 0;        // +0x1000
    Vec3 ex4_acc_vec;           // +0x1010
    i32 ex4_duration = 0;       // +0x101c

    Timer ex8_timer;            // +0x1030
    float ex8_acc = 0;          // +0x1044
    float ex8_ang_vel = 0;      // +0x1048
    i32 ex8_duration = 0;       // +0x1064

    Timer ex10_timer;           // +0x1078
    float ex10_target_speed = 0;// +0x108c
    float ex10_target_angle = 0;// +0x1090
    i32 ex10_duration = 0;      // +0x10ac
    i32 ex10_repeats = 0;       // +0x10b0
    i32 ex10_count = 0;         // +0x10b4
    i32 ex10_mode = 0;          // +0x10b8
    i32 ex10_param = 0;         // +0x10bc

    float ex40_speed = 0;       // +0x10d4
    float ex40_width = 0;       // +0x10dc
    float ex40_height = 0;      // +0x10e0
    i32 ex40_count = 0;         // +0x10f4
    i32 ex40_max_bounces = 0;   // +0x10f8
    u32 ex40_wall_mask = 0;     // +0x1100

    Timer ex_wait_timer;        // +0x1108

    i32 ex1000_count = 0;       // +0x1184
    i32 ex1000_max = 0;         // +0x1188
    u32 ex1000_mask = 0;        // +0x118c

    Timer ex20000_timer;        // +0x11e0
    float ex20000_saved_speed = 0; // +0x11f4
    Vec3 ex20000_target;        // +0x1208
    i32 ex20000_duration = 0;   // +0x1214
    u32 ex20000_mode = 0;       // +0x1218

    Timer ex80000_timer;        // +0x1228
    float ex80000_speed = 0;    // +0x123c
    float ex80000_angle = 0;    // +0x1240
    Vec3 ex80000_vel;           // +0x1250
    i32 ex80000_duration = 0;   // +0x125c

    Timer ex200000_timer;       // +0x1270
    float ex200000_acc = 0;     // +0x1284
    float ex200000_angle = 0;   // +0x1288
    Vec3 ex200000_acc_vec;      // +0x1298
    i32 ex200000_duration = 0;  // +0x12a4

    Timer ex100_timer;          // +0x12b8
    i32 ex100_ray_check = 0;    // +0x12ec

    float saved_speed = 0;      // +0x1314
    float saved_angle = 0;      // +0x1318
    Vec3 saved_pos;             // +0x131c

    Timer ex4000000_timer;      // +0x1348

    Interp<Vec3> ex20000_interp;// +0x1394
    Interp<float> scale_interp; // +0x13ec
    float scale = 1.0f;         // +0x141c

    Timer timer1;               // +0x1420
    Timer timer2;               // +0x1434
    i32 life_default = 0x3c;    // +0x1448
    Timer timer;                // +0x144c 主存活计时（0x412860 尾推进）
    Timer tick_timer;           // +0x1460 步进计时（0x411e70 首推进）
    i16 type = 0;               // +0x1474
    i16 color = 0;              // +0x1476
};

// Ex 0x8000000 激光生成请求（a==0 直线 0x431130 / a==1 照射 0x431860）。0x413860 在弹幕更新内当场建激光，
// 其生成音效排在随后的消弹音效之前，故 BulletManager 通过 on_laser_request 即时回调 LaserManager::spawn_from_bullet；
// 原版在槽满（0x200）时同样跳过生成，只推进 Ex 下标。
struct BulletLaserRequest {
    i32 kind = 0;              // 0 直线 / 1 照射
    Vec3 pos;                  // 弹当前坐标
    float angle = 0;           // 已解析角度（r ≥ 999990 自机狙）
    float speed = 0;           // 已解析速度（m ≤ -999990 取弹速）
    BulletExStep step;         // 本步（r,s,m,n,a,b,c,d）
    BulletExStep next;         // 下一步（弹种/颜色/消弹标志等）
    BulletExStep ex[18]{};     // 弹的 Ex 列表副本
};

// 0x411dd0：弹幕管理器（DAT_004a6dac）
class BulletManager {
public:
    // game_speed 引用全局游戏速度 DAT_004a5788（GameSession 传入 Supervisor::game_speed）：原版 0x416840 族消弹的末步直接读它，
    // ECL 在敌机链里刚改成慢动作时，同一帧里的消弹就按新速度推进
    explicit BulletManager(float& game_speed) : game_speed(game_speed) {}

    static constexpr int kMaxBullets = 2000;
    static constexpr int kLayerBuckets = 6;
    static constexpr int kBulletAnmSlot = 7;

    std::vector<Bullet> bullets;        // +0x9c 起 2000 槽
    Bullet* free_head = nullptr;        // +0x60 空闲链头（初始化后槽 1999 在头）
    Bullet* active_head = nullptr;      // +0x70 活动链头（最新生成的弹）
    Bullet* layer_heads[kLayerBuckets]{}; // +0x10..0x24
    Bullet* layer_tails[kLayerBuckets]{}; // +0x28..0x3c
    i32 active_count = 0;               // +0x40
    float safe_radius_sq = 0;           // +0x44（ECL 601 期间临时写入 EnemyInf+0x4074）
    float bounce_width = 0;             // +0x48 全局反弹宽（0 = 用弹自身的 Ex 参数）
    float bounce_height = 0;            // +0x4c
    i32 cancel_item_counter = 0;        // +0x58（消弹模式 5 每 3 颗出一个道具）
    float& game_speed;                 // DAT_004a5788
    i32 total_canceled = 0;             // +0x1403b14
    std::vector<u32> cancel_effect_ids; // +0x13ffc8c：按槽记录消弹特效 VM id
    std::vector<i32> sfx_queue;         // 0x45e1f0 音效登记（发射 / Ex 变换 / 擦弹 0x2a / 消弹 0x47）
    SoundRequests* sfx_sink = nullptr;
    void play_sfx(i32 id, float x = 0.0f) { if (sfx_sink != nullptr) sfx_sink->push(id, x); else sfx_queue.push_back(id); }
    std::function<void(const BulletLaserRequest&)> on_laser_request;  // Ex 0x8000000 → LaserManager::spawn_from_bullet
    bool spellcard_active = false;      // DAT_004a6db0->0x78 bit0（Spellcard 管理器接入前由宿主同步）

    AnmManager* animations = nullptr;   // DAT_004c0f48
    AnmFile* anm = nullptr;             // +0x1403b24 bullet.anm（槽 7）
    AnmFile* effect_anm = nullptr;      // DAT_004a6db8->0xc effect.anm（槽 8，擦弹特效脚本 0x18）
    GameGlobals* globals = nullptr;
    Player* player = nullptr;           // DAT_004a6ef8
    const PlayerWorld* world = nullptr; // DAT_004a6dcc->0x1c8 对话中
    Rng* rng = nullptr;                 // DAT_004a6d88
    ItemManager* items = nullptr;       // DAT_004a6ddc（擦弹得点 / 消弹转化道具 0x430960）

    // 0x411dd0 + 0x411a30：构造（分配池、绑定依赖）；0x411b70：首帧复位（空闲链重建、计数清零）
    void create(AnmManager& anm_manager, AnmFile& bullet_anm, AnmFile* effect, GameGlobals& g, Player* pl,
                const PlayerWorld* w, Rng& game_rng);
    void reset();

    // 0x414da0 / 0x412cb0：按发射器配置发射整组弹幕；spawn_single 返回 1 = 池已空，-1 = 近身安全半径内丢弃
    void shoot(const EtShooter& shooter);
    int spawn_single(const EtShooter& shooter, i32 way_idx, i32 stack_idx, float aim_angle);

    // 0x412860：更新链 0x1c；frozen = GameManager+0x88 & 0x400（对话/过场冻结，只重挂图层）
    void update(float game_speed, bool frozen);
    // 0x412a60：绘制链 0x25。弹 VM 不挂图层：按 6 个图层桶顺序，先画附加 VM 再画主 VM（同步坐标 / 朝向 / 缩放后 0x468490）
    void draw(AnmRenderer& renderer);

    // 0x416840：单弹消弹（item_mode 见 0x416a00）
    void cancel_bullet(Bullet& b, i32 item_mode);
    // 0x416c20 / 0x416d20：圆形范围消弹（check_immunity = 0x416d20 变体，跳过 +0x24 非零的弹）
    void cancel_radius(const Vec3& center, float radius, i32 item_mode, bool check_immunity);
    // 0x416e20：旋转矩形（中心 center、宽高 size、绕中心转 angle）范围消弹——还要求弹同时落在场地矩形
    //   (0, 224) ± (384, 448)/2 内；跳过 +0x24 禁消弹的弹。炸弹（琪露诺 / 文的横扫、魔理沙的光束）用
    void cancel_rect(const Vec3& center, const Vec2& size, float angle, i32 item_mode);
    // 0x444cf0：Player::graze（计数、特效、音效）后生成得点道具 0x10（敌机体擦弹 0x41c330 亦经此）
    void graze(const Vec3& pos);
    // 0x4252d0（敌机每帧钩子 1）：center 半径 radius 内、Ex 0x8000 参数 == 1 的弹改参数为 2 并跳到 Ex 第 8 步
    void trigger_ex_param_near(const Vec3& center, float radius);
    // 0x430960：经 items 生成道具（ItemManager 未绑定时忽略；0x430960 不消耗 RNG）
    void spawn_item(i32 type, const Vec3& pos, float angle, float speed, i32 flag);
    // 0x416f40：全屏消弹（状态非 0/3 的全部槽）
    void cancel_all(i32 item_mode);

    // 0x416a00：消弹点的道具转化（item_mode：0 无 / 1 得点 / 4 季节 / 5 炸弹每 3 颗一个……）；激光消弹亦经此
    void convert_cancel_item(const Vec3& pos, i32 item_mode);
    // 0x443840：from → 自机的角度（重合时 π/2）
    float angle_to_player(const Vec3& from) const noexcept;
    // 0x417140：sprite_mapper 1（ins_300 钩子；vm.owner 为 Bullet*）
    static i32 map_sprite(AnmVm& vm, i32 index);

private:
    Bullet* iterate_current_ = nullptr; // +0x1403b1c
    Bullet* iterate_next_ = nullptr;    // +0x1403b20

    void release_bullet(Bullet& b);                     // 0x412670
    int step_bullet(Bullet& b, float game_speed);       // 0x411e70
    void run_ex(Bullet& b, float game_speed);           // 0x413860
    int check_player_collision(Bullet& b, bool force_graze, float game_speed); // 0x4124b0
    void start_hit_dissolve(Bullet& b, float game_speed);                     // 0x4124b0 / 0x411e70 状态 5 共用：中断 1 + 消弹特效飞散
    void bind_appearance(Bullet& b, i32 type, i32 color);  // 0x412cb0 / Ex 0x200 共用：VM 重建 + 消弹脚本 + 判定/桶
    const AnmRuntimeSprite* main_sprite(const Bullet& b) const;  // vm_main 当前精灵（出界 / 反弹 / 穿墙尺寸）
    void push_front_active(Bullet& b);
    void unlink_active(Bullet& b);

    int ex_initial_boost(Bullet& b, float gs);   // 0x414ec0
    int ex_accel(Bullet& b, float gs);           // 0x414fb0
    int ex_speed_ease(Bullet& b, float gs);      // 0x4151e0
    int ex_accel_turn(Bullet& b, float gs);      // 0x4153e0
    int ex_stop_turn(Bullet& b, float gs);       // 0x415570
    int ex_bounce(Bullet& b, float gs);          // 0x415bb0
    int ex_wrap(Bullet& b);                      // 0x415d80
    int ex_move_to(Bullet& b, float gs);         // 0x415f90
    int ex_extra_velocity(Bullet& b, float gs);  // 0x4161f0
    int ex_bounds_guard(Bullet& b, float gs);    // 0x4162d0
};

}  // namespace th16
