// th16 LaserManager（DAT_004a6ee0，0x610 字节）：敌方激光的生成、每帧推进、消弹切分与自机判定。
//
// 生命周期与链优先级：
//   0x4314a0 / 0x431330  进游戏时创建（0x42cb60），挂更新链 0x1b（0x4316b0 → 0x431510，在敌机链 0x1a 之后、
//                        敌弹链 0x1c 之前）与绘制链 0x23（0x431720）；0x42dc50 / 0x42dee0 首帧调 0x42cb00 清表。
//   精灵映射钩子：AnmManager::sprite_mappers[2] = 0x431fa0（按激光弹种与颜色查 bullet_type_info 重映射）、
//                 sprite_mappers[3] = 0x43a840（曲线激光：颜色 + 0x20c）。
//
// 四种激光子类（0x431760 按 kind 创建，挂入 +0x14 双向链表尾，分配递增句柄 >= 0x10000）：
//   kind 0  LaserLine（0x431130，ECL 702 / 弹幕 Ex 0x8000000 a=0）：沿角度伸长到目标长度后整段匀速前进的直线激光。
//   kind 1  LaserInfinite（0x431860，ECL 703 / 弹幕 Ex 0x8000000 a=1）：预警线 → 展开 → 维持 → 收束的照射激光。
//   kind 2  LaserCurve（0x431900，ECL 711）：由多段运动律驱动的采样点链曲线激光。
//   kind 3  LaserBeam（0x4318c0，ECL 713）：只占句柄、不绘制不判定的占位激光（本作 ECL 未使用）。
//
// 与相邻管理器的交互：
//   - BulletManager：消弹道具转化（0x416a00 convert_cancel_item）、擦弹结算（0x444cf0 graze）、
//                    Ex 0x2000 激光转弹幕（0x414da0 shoot）、Ex 0x8000000 弹幕派生激光（spawn_from_bullet）。
//   - Player：激光线段与自机判定（0x443af0：外框 16 倍判定半径算擦弹、内框 1 倍算被弹 → 0x443f10）。
//   - EffectManager：矩形消弹（炸弹）生成的消散特效 VM 经 0x41aa00 登记到特效槽。
#pragma once

#include "AnmManager.hpp"
#include "Bullet.hpp"
#include "RenderDevice.hpp"
#include "Sound.hpp"
#include "Types.hpp"
#include <memory>
#include <vector>

namespace th16 {

class AnmRenderer;
class EffectManager;
class EnemyManager;
struct GameGlobals;
class Player;
struct PlayerWorld;

enum class LaserKind : i32 {
    Line = 0,
    Infinite = 1,
    Curve = 2,
    Beam = 3,
};

enum class LaserState : i32 {
    Dead = 1,       // 待链表摘除（0x431510 开头回收）
    Active = 2,     // 正常活动中（可判定）
    Telegraph = 3,  // 照射激光预警期（不判定被弹）
    Expand = 4,     // 照射激光展开期（宽度线性增长，已可判定）
    Shrink = 5,     // 照射激光收束期（宽度线性缩小，不判定被弹）
};

// 生命周期位（Laser+0x0c）
enum LaserLifecycle : u32 {
    kLaserRanOnce = 0x1u,         // 已跑过至少一帧（炸弹矩形消弹只作用于此类激光）
    kLaserDestroyCountMask = 0x6u, // 两位延迟销毁计数：消弹置 2，下一帧 0x431510 自增到 4 后摘除
    kLaserPaused = 0x8u,          // 暂停推进，只做强制擦弹判定
    kLaserCurveFrozen = 0x10u,    // 曲线激光：采样点不再按时间重算（Ex 0x10000000）
};

// 直线激光创建参数（0x358 字节；ECL 702 / 弹幕 Ex 0x8000000 a=0 / 反弹与消弹切分派生时复用）
struct LineLaserArgs {
    Vec3 origin;                 // +0x00 发射起点
    float angle = 0;             // +0x0c 朝向角（弧度，已 wrap）
    float target_length = 0;     // +0x10 伸长到此长度后整段开始前进
    float initial_length = 0;    // +0x14 初始长度
    float max_extent = 0;        // +0x18 起点可越过的最大距离（> 0 时越过后尾部缩短）
    float width = 0;             // +0x1c 全宽（像素）
    float speed = 0;             // +0x20 伸长 / 前进速度
    i32 type = 0;                // +0x24 外观弹种
    i32 color = 0;               // +0x28 外观颜色
    float start_offset = 0;      // +0x2c 起点沿朝向前移的距离
    i32 start_ex = 0;            // +0x30 起始 Ex 下标
    u32 flags = 0;               // +0x34 bit1 = 全长参与被弹判定（否则跳过前 1/10 并只取 4/5 长）
    BulletExStep ex[18]{};       // +0x38 Ex 指令序列（激光自备副本，Ex 0x40 反弹次数就地递减）
    i32 spawn_sfx = -1;          // +0x350 生成音效
    i32 step_sfx = -1;           // +0x354 Ex 变换 / 反弹音效
};

// 照射激光创建参数（0x378 字节；0x411860 构造：清零后 speed = 8）
struct InfiniteLaserArgs {
    Vec3 origin;                 // +0x00 发射起点
    Vec3 velocity;               // +0x0c 起点平移速度（ECL 705）
    float angle = 0;             // +0x18 朝向角
    float angular_vel = 0;       // +0x1c 角速度（ECL 709）
    float target_length = 0;     // +0x20 伸长到的全长
    float initial_length = 0;    // +0x24 初始长度
    float width = 0;             // +0x28 展开后的全宽
    float speed = 8.0f;          // +0x2c 伸长速度
    i32 telegraph_time = 0;      // +0x30 预警帧数
    i32 expand_time = 0;         // +0x34 展开帧数
    i32 active_time = 0;         // +0x38 维持帧数
    i32 shrink_time = 0;         // +0x3c 收束帧数
    i32 spawn_sfx = -1;          // +0x40 生成音效
    i32 step_sfx = -1;           // +0x44 Ex 变换音效
    i32 handle = 0;              // +0x48 ECL 703 指定的句柄（覆盖管理器分配值；弹幕派生为 0）
    float start_offset = 0;      // +0x4c 起点沿朝向前移的距离
    i32 start_ex = 0;            // +0x50 起始 Ex 下标
    i32 type = 0;                // +0x54 外观弹种
    i32 color = 0;               // +0x58 外观颜色
    u32 flags = 0;               // +0x5c bit0 跟随 Boss 0；bit1 → 切分出的直线激光 flags bit0
    BulletExStep ex[18]{};       // +0x60 Ex 指令序列
};

// 曲线激光的轨迹段（0x431190 分配，0x3c 字节）：在时刻区间 [start_time, end_time) 内生效的运动律
struct CurveSegment {
    enum Mode : i32 {
        kUniform = 0,            // 匀速直线
        kAccelerate = 1,         // 加速度（accel_angle < -990 为沿切向，否则为固定方向）
        kAngularStep = 2,        // 每帧转角 accel_angle、速度增量 accel
    };
    float start_time = 0;        // +0x08
    float end_time = 0;          // +0x0c
    i32 mode = kUniform;         // +0x10
    Vec3 direction;              // +0x14 polar(start_angle, 1)
    Vec3 start_pos;              // +0x20 本段起点
    float start_angle = 0;       // +0x2c
    float start_speed = 0;       // +0x30
    float accel = 0;             // +0x34 加速度（Ex 4 / 8 的 r）
    float accel_angle = 0;       // +0x38 加速方向角或每帧转角（Ex 4 / 8 的 s）
};

// 曲线激光创建参数（0x358 字节；ECL 711 / 矩形消弹切分派生）
struct CurveLaserArgs {
    Vec3 origin;                 // +0x00 发射起点
    float angle = 0;             // +0x0c 朝向角
    float width = 0;             // +0x10 全宽
    float speed = 0;             // +0x14 初速
    i32 type = 0;                // +0x18 外观弹种（1 = 专用脚本 0x142）
    i32 color = 0;               // +0x1c 外观颜色
    i32 node_count = 0;          // +0x20 采样点个数（最多 0x40）
    float start_offset = 0;      // +0x24 起点沿朝向前移的距离（0x4370a0 用后清零）
    u32 flags = 0;               // +0x28 ECL 711 置 bit0（原版仅写不读）
    BulletExStep ex[18]{};       // +0x2c Ex 指令序列
    i32 spawn_sfx = -1;          // +0x344 生成音效
    i32 step_sfx = -1;           // +0x348 Ex 变换音效
    i32 start_ex = 0;            // +0x34c 起始 Ex 下标
    // +0x350：切分派生时继承母激光的轨迹段链（空 = 从 angle / speed 新建匀速段）
    const std::vector<CurveSegment>* inherited_segments = nullptr;
    float initial_time = 0;      // +0x354 采样时刻初值（派生片段 = 母激光时刻 - 片段起始下标）
};

// 占位激光创建参数（0x354 字节；ECL 713）
struct BeamLaserArgs {
    Vec3 origin;                 // +0x00
    float angle = 0;             // +0x18
    float length = 0;            // +0x20
    float width = 0;             // +0x24
    i32 handle = 0;              // +0x28 ECL 713 第 2 参数
    i32 color = 0;               // +0x2c
    float start_offset = 0;      // +0x30
    i32 node_count = 0;          // +0x34
    u32 flags = 0;               // +0x38 ECL 714 写 bit0（0x431310）
    BulletExStep ex[18]{};       // +0x3c
};

// 曲线激光单个采样点（0x20 字节，+0x1524 数组）
struct CurveNode {
    Vec3 pos;                    // +0x00
    Vec3 velocity;               // +0x0c
    float angle = 0;             // +0x18
    float speed = 0;             // +0x1c
};

// Ex 指令状态机工作区（0x431fe0 / 0x438cb0 解释，偏移与 Bullet 的同名区段一致）
struct LaserExState {
    Timer boost_timer;           // +0x84  Ex 0x1：直线激光只置位不处理
    Timer accel_timer;           // +0xcc  Ex 0x4
    float accel = 0;             // +0xe0
    float accel_angle = 0;       // +0xe4
    Vec3 accel_vec;              // +0xf4
    i32 accel_duration = 0;      // +0x100
    Timer ang_vel_timer;         // +0x114 Ex 0x8（仅曲线激光处理）
    float ang_vel_accel = 0;     // +0x128
    float ang_vel = 0;           // +0x12c
    i32 ang_vel_duration = 0;    // +0x148
    Timer turn_timer;            // +0x15c Ex 0x10：减速到停再折返
    float turn_speed = 0;        // +0x170
    float turn_angle = 0;        // +0x174
    i32 turn_duration = 0;       // +0x190
    i32 turn_max = 0;            // +0x194
    i32 turn_count = 0;          // +0x198
    i32 turn_mode = 0;           // +0x19c（仅模式 0 由激光处理）
    float bounce_speed = 0;      // +0x1b8 Ex 0x40：反弹派生激光的速度
    i32 bounce_count = 0;        // +0x1d8
    i32 bounce_max = 0;          // +0x1dc
    u32 bounce_walls = 0;        // +0x1e0 bit0 上 / bit1 下 / bit2 左 / bit3 右 / bit4 只触发不派生
    Timer wait_timer;            // +0x1ec Ex 0x80000000
    Timer wrap_timer;            // +0x234 Ex 0x1000（直线激光只计时不处理）
    Timer curve_hold_timer;      // +0x39c 曲线激光 Ex 0x100：倒数期间不做出界判定
    i32 curve_hold_param = 0;    // +0x3d0
};

// 0x430fc0：四类激光的公共数据 + 各子类扩展字段（按 kind 只使用对应的参数包与 VM）
struct Laser {
    LaserKind kind = LaserKind::Line;      // +0x14
    u32 lifecycle = 0;                     // +0x0c LaserLifecycle 位
    LaserState state = LaserState::Active; // +0x10
    Timer age_timer;                       // +0x18 存活帧计时（照射激光各阶段计时）
    Timer graze_timer;                     // +0x2c 擦弹节流计时（每 3 帧掉一枚得点道具）
    Timer frame_timer;                     // +0x40 直线激光存活帧 / 曲线激光采样时刻
    Vec3 pos;                              // +0x54 当前起点坐标
    Vec3 velocity;                         // +0x60 当前速度向量
    float angle = 0;                       // +0x6c 当前朝向角
    float length = 0;                      // +0x70 当前长度
    float width = 0;                       // +0x74 当前全宽
    float speed = 0;                       // +0x78 当前标量速度
    float start_drawn_len = 0;             // +0x7c 起点已越过的距离（> 0 时不画根部光球）
    i32 id = 0;                            // +0x80 句柄（0x41aa40 按此查找；ECL 710 清零）

    LaserExState ex;                       // +0x84..+0x3d0
    i32 ex_index = 0;                      // +0x594 当前 Ex 步骤下标
    u32 ex_flags = 0;                      // +0x598 当前活动 Ex 位掩码
    Timer spawn_grace_timer;               // +0x5a0 生成后 30 帧内不做出界销毁
    Timer invuln_timer;                    // +0x5b4 生成后 3 帧（或 Ex 0x100 指定帧）内同样豁免出界
    i32 no_cancel = 0;                     // +0x5c8 非零时免疫「尊重免疫」的消弹（Ex 0x80，逐帧递减）
    i32 bullet_type = 0;                   // +0x5cc 外观弹种（sprite_mapper 2 读取）
    i32 bullet_color = 0;                  // +0x5d0 外观颜色（sprite_mapper 2 / 3 读取）

    // —— 创建参数备份（+0x5d4；反弹派生与消弹切分时就地修改后复用） ——
    LineLaserArgs line;
    InfiniteLaserArgs infinite;
    CurveLaserArgs curve;
    BeamLaserArgs beam;

    AnmVm vm_beam;                         // +0x92c（Inf +0x950 / Beam +0x928）激光柱主 VM
    AnmVm vm_origin;                       // +0xf28（Inf +0xf4c）根部光球 VM
    AnmVm vm_tip;                          // +0x1524（仅 Line）前端光球 VM

    // —— 曲线激光专属 ——
    std::vector<CurveNode> curve_nodes;    // +0x1524 采样点数组
    std::vector<ScreenColorUvVertex> curve_strip;  // +0x1528 绘制条带顶点（每采样点左右各一个，0x438750 每帧重算）
    std::vector<CurveSegment> curve_segments; // +0x152c 起的轨迹段链（按时间顺序追加）
    float curve_full_width = 0;            // +0x94c（Inf 复用同偏移：原版仅清零）
};

// 0x4314a0：激光管理器（DAT_004a6ee0）
class LaserManager {
public:
    static constexpr int kMaxLasers = 0x200;  // 512
    static constexpr i32 kFirstHandle = 0x10000;
    static constexpr int kBeamSpriteMapper = 2;
    static constexpr int kCurveSpriteMapper = 3;

    // —— 活动激光与句柄 ——
    std::vector<std::unique_ptr<Laser>> lasers; // +0x14 活动链表（保持插入次序；更新中追加的子激光同帧可见）
    i32 next_handle = kFirstHandle;             // +0x5e8 递增句柄计数器
    Vec3 last_cancel_center;                    // +0x5ec 最近一次圆形 / 矩形消弹的中心
    Vec3 last_cancel_size;                      // +0x5f8 最近一次矩形消弹的尺寸
    std::vector<i32> sfx_queue;                 // 本帧触发的音效（生成 / Ex 折返 / 反弹）
    SoundRequests* sfx_sink = nullptr;

    // —— 依赖 ——
    AnmManager* animations = nullptr;           // DAT_004c0f48
    AnmFile* bullet_anm = nullptr;              // +0x604 bullet.anm（槽 7）
    AnmFile* effect_anm = nullptr;              // effect.anm（槽 8，自机被弹特效）
    BulletManager* bullets = nullptr;           // DAT_004a6dac
    Player* player = nullptr;                   // DAT_004a6ef8
    EnemyManager* enemies = nullptr;            // DAT_004a6dc0（照射激光跟随 Boss 0）
    EffectManager* effects = nullptr;           // DAT_004a6db8
    const PlayerWorld* world = nullptr;         // 对话中标志
    float game_speed = 1.0f;                    // DAT_004a5788 镜像（update 入口抄入）

    // 0x4314a0 + 0x431330：绑定依赖并注册 sprite_mapper 2 / 3；0x42cb00：清空全部活动激光
    void create(AnmManager& anm_manager, AnmFile& bullet_file, AnmFile* effect_file, BulletManager& bullet_mgr,
                Player* pl, EnemyManager* enm_mgr, EffectManager* eff_mgr, const PlayerWorld* w);
    void reset();

    // 0x4316b0 → 0x431510：更新链 0x1b 每帧入口
    void update(float game_speed);
    // 0x431720：绘制链 0x23——跳过待摘除（state 1）的激光，其余按种类绘制：
    //   直线 0x433720  激光柱 VM 同步起点 / 朝向后直绘，再画前端光球，起点未越过时画根部光球
    //   照射 0x4357a0  激光柱 VM 同步后直绘，起点未越过时画根部光球
    //   曲线 0x438750  按采样点生成屏幕空间条带（draw_curve_strip）经 0x4681f0 直绘，采样已满时在末节点画根部光球
    //   占位 0x43ac10  不绘制
    void draw(AnmRenderer& renderer);

    // 0x431760：创建一条激光并跑初始化，返回分配的句柄（槽满返回 0 且不创建）
    i32 spawn_line(const LineLaserArgs& args);
    i32 spawn_infinite(const InfiniteLaserArgs& args);
    i32 spawn_curve(const CurveLaserArgs& args);
    i32 spawn_beam(const BeamLaserArgs& args);
    // 0x413860 Ex 0x8000000 内联段：弹幕派生激光（BulletManager::on_laser_request 当场回调）
    void spawn_from_bullet(const BulletLaserRequest& req);

    // 0x41aa40：按句柄查找活动激光（id == 0 或未命中返回 nullptr）
    Laser* find_by_id(i32 id) noexcept;

    // 0x431af0(item_mode)：全屏激光消弹（对话开始 / ECL 613 / 决死第 15 帧）
    void cancel_all(i32 item_mode);
    // 0x4319e0：ECL 545——全部激光出界豁免计时清零、免疫清零后带道具消弹
    void cancel_all_ignoring_immunity();
    // ECL 710（0x41dcb0 case 0xa3）：终止全部同句柄激光（不转道具）并清句柄
    void cancel_handle(i32 id);
    // 0x431a70(center, radius, item_mode, respect_immunity)：圆形范围激光消弹，返回消去的采样段数
    i32 cancel_radius(const Vec3& center, float radius, i32 item_mode, bool respect_immunity);
    // 0x431950(center, size, angle)：炸弹矩形消弹（只作用于已跑过首帧的激光；mode 5、尊重免疫）
    i32 cancel_rect(const Vec3& center, const Vec3& size, float rect_angle);

    // 0x431fa0 / 0x43a840：sprite_mapper 2 / 3（ins_300 钩子；vm.owner 为 Laser*）
    static i32 map_sprite_beam(AnmVm& vm, i32 index);
    static i32 map_sprite_curve(AnmVm& vm, i32 index);

private:
    Laser* allocate(LaserKind kind);  // 0x431760 前半：槽满返回 nullptr，否则分配句柄并挂到链尾

    void init_line(Laser& l);         // 0x431b30
    void init_infinite(Laser& l);     // 0x435050
    void init_curve(Laser& l);        // 0x4370a0
    void init_beam(Laser& l);         // 0x43a860

    bool step(Laser& l);              // vtable 槽 4：返回 true 表示本帧销毁
    bool step_line(Laser& l);         // 0x432f40
    bool step_infinite(Laser& l);     // 0x4352f0
    bool step_curve(Laser& l);        // 0x4377d0

    void run_ex_line(Laser& l);       // 0x431fe0
    void run_ex_infinite(Laser& l);   // 0x436fd0
    void run_ex_curve(Laser& l);      // 0x438cb0
    void ex_reshoot(Laser& l, const BulletExStep* steps);  // Ex 0x2000：以激光末端为发射点再发射

    int ex_accelerate(Laser& l);      // 0x432dc0 / 0x4395b0
    int ex_angular_velocity(Laser& l);// 0x439460
    int ex_turn(Laser& l, i32 step_sfx); // 0x432c20 / 0x4392c0
    int ex_bounce_line(Laser& l);     // 0x432620
    int ex_curve_hold(Laser& l);      // 0x439730
    void run_ex_wait_and_immunity(Laser& l, int* retry_count);  // 各 update 共用：等待计时与免疫倒数

    void bind_beam_vm(Laser& l, int script, bool curve_mapper);  // 激光柱 VM 绑脚本并置绘制标志
    void bind_glow_vm(AnmVm& vm, int script, bool extra_flags);  // 根部 / 前端光球 VM
    void sync_beam_scale(Laser& l);   // 柱 VM 按当前宽 / 长缩放

    void collide_player(Laser& l, bool force_graze);           // vtable 槽 13
    void collide_player_line(Laser& l, bool force_graze);      // 0x433510
    void collide_player_infinite(Laser& l, bool force_graze);  // 0x435610
    void collide_player_curve(Laser& l, bool force_graze);     // 0x437cf0
    void graze_at_player_foot(const Vec3& seg_origin, float seg_angle, float graze_z);  // 擦弹点 = 自机到激光轴的垂足
    int test_player_segment(const Vec3& seg_origin, float seg_angle, float seg_length, float seg_width,
                            bool force_graze);  // 0x443af0

    // vtable 槽 10 / 9 / 8：整条消弹 / 圆形切分 / 矩形切分
    i32 cancel_single(Laser& l, i32 item_mode, bool respect_immunity);
    i32 cancel_circle(Laser& l, const Vec3& center, float radius, i32 item_mode, bool respect_immunity);
    i32 cancel_rect_single(Laser& l, const Vec3& center, const Vec3& size, float rect_angle, i32 item_mode,
                           bool respect_immunity);
    i32 cancel_line_single(Laser& l, i32 item_mode, bool respect_immunity);      // 0x434cd0
    i32 cancel_line_circle(Laser& l, const Vec3& center, float radius, i32 item_mode, bool respect_immunity); // 0x434730
    i32 cancel_line_rect(Laser& l, const Vec3& center, const Vec3& size, float rect_angle, i32 item_mode,
                         bool respect_immunity);                                   // 0x433860
    i32 cancel_infinite_single(Laser& l, i32 item_mode, bool respect_immunity);  // 0x436c70
    i32 cancel_infinite_circle(Laser& l, const Vec3& center, float radius, i32 item_mode, bool respect_immunity); // 0x436670
    i32 cancel_infinite_rect(Laser& l, const Vec3& center, const Vec3& size, float rect_angle, i32 item_mode,
                             bool respect_immunity);                               // 0x435880
    i32 cancel_curve_single(Laser& l, i32 item_mode, bool respect_immunity);     // 0x43a620
    i32 cancel_curve_circle(Laser& l, const Vec3& center, float radius, i32 item_mode, bool respect_immunity); // 0x43a2f0
    i32 cancel_curve_rect(Laser& l, const Vec3& center, const Vec3& size, float rect_angle, i32 item_mode,
                          bool respect_immunity);                                  // 0x4397d0
    void split_line_after_hits(Laser& l, const bool* hit, int sample_count, const Vec3& step16);  // 0x434730 / 0x433860 共用
    void spawn_infinite_fragments(Laser& l, const bool* hit, int sample_count, const Vec3& step16, bool rect_variant);
    void shift_curve_nodes(Laser& l, int drop_count);

    // 消弹特效：直线 / 照射激光按弹种分三档脚本；register_in_effects = 矩形消弹（炸弹）的登记变体
    void spawn_cancel_effect(i32 bullet_type, i32 bullet_color, const Vec3& pos, bool register_in_effects,
                             bool rect_thresholds);

    // 曲线激光轨迹递推（0x4370a0 / 0x4377d0 / 0x438cb0 共用）
    CurveSegment& append_curve_segment(Laser& l, float time);  // 0x431190
    const CurveSegment* find_curve_segment(const Laser& l, float time) const;
    static void eval_curve_head(const CurveSegment& seg, float time, Vec3& pos, float& speed, float& angle);
    static void eval_curve_follow(const CurveSegment& seg, float time, const CurveNode& prev, Vec3& pos,
                                  float& speed, float& angle);
    void recompute_curve_nodes(Laser& l);
    bool curve_in_bounds(const Laser& l) const;

    // 0x433720 / 0x4357a0 / 0x438750：三类激光的绘制回调（Beam 0x43ac10 为空）
    void draw_line(Laser& l, AnmRenderer& renderer);
    void draw_infinite(Laser& l, AnmRenderer& renderer);
    void draw_curve(Laser& l, AnmRenderer& renderer);
    // 0x438750 前半：把采样点展开成屏幕空间三角条带（每点沿相邻方向角平分线左右各半宽，U 沿条带 0 → 1，
    // V 取激光柱 VM 精灵的上 / 下沿），写入 l.curve_strip
    void build_curve_strip(Laser& l, float field_offset_x) const;

    void push_sfx(i32 sfx, float x = 0.0f);
};

}  // namespace th16
