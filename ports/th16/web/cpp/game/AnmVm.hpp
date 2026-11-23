// th16 ANM 虚拟机对象——原版 0x5fc 字节结构的逐字段镜像。
//
// 布局依据：0x45f980（指令解释）、0x4090f0（复位）、0x45edb0（设精灵）、0x463b30（插值推进）、
// 0x46f600 / 0x46eab0（池分配/回收）、0x46ed60 / 0x46eea0（子 VM 创建）。
// 字段偏移以 static_assert 固定，tests/anm-vm.test.mjs 用同一张偏移表把本实现的内存快照与
// Unicorn 直接执行原版得到的快照（tools/oracle/anm-oracle.py）逐字段比对。
//
// 计时器 Timer（0x14 字节，0x406190 推进 / 0x406490 设值 / 0x40d490 回退）：
//   +0 previous  +4 current  +8 current_f  +0xc speed_slot（0x490eb0 速度表下标，运行时恒被清零）
//   +0x10 flags（bit0 = 已初始化）
// 插值器 Interp<T>（0x406e10 / 0x463d40 / 0x464080 / 0x4643b0 / 0x464590 / 0x4171c0）：
//   initial goal bezier1 bezier2 current Timer end_time mode；mode 7 = 速度累加，0x11 = 加速度，
//   8 = Hermite（bezier1/2 为切线），其余经 0x4033f0 缓动曲线。
#pragma once
#include <cstddef>
#include "Types.hpp"

namespace th16 {

struct IVec3 { i32 x = 0, y = 0, z = 0; };

struct Timer {
    i32 previous = 0;
    i32 current = 0;
    float current_f = 0;
    i32 speed_slot = 0;
    u32 flags = 0;

    // 0x406490：设到指定时刻（previous = t-1）
    void set(i32 t) noexcept {
        if ((flags & 1) == 0) { current = 0; previous = i32(0xfff0bdc1); current_f = 0; speed_slot = 0; flags |= 1; }
        current = t; previous = t - 1; current_f = float(t);
    }
    // 0x406200 等：清零
    void reset() noexcept {
        if ((flags & 1) == 0) { current = 0; previous = i32(0xfff0bdc1); current_f = 0; speed_slot = 0; flags |= 1; }
        current = 0; current_f = 0; previous = -1;
    }
    // 0x406190：按游戏速度推进一帧
    void tick(float game_speed) noexcept;
    // 0x40d490：回退一帧（stop 指令用，与随后的 tick 抵消）
    void untick(float game_speed) noexcept;
    // 0x43ac80：回退 n 帧（Player 射击计时环绕）
    void rewind(i32 n, float game_speed) noexcept;
    // 0x405f20 内联：前进 n 帧（充能环中断 1 一次跳 300 帧）
    void advance(float n, float game_speed) noexcept;
};
static_assert(sizeof(Timer) == 0x14, "Timer 为 0x14 字节");

template <typename T>
struct Interp {
    T initial{}, goal{}, bezier1{}, bezier2{}, current{};
    Timer timer;
    i32 end_time = 0;
    i32 mode = 0;
};
static_assert(sizeof(Interp<Vec3>) == 0x58, "Interp<Vec3>");
static_assert(sizeof(Interp<IVec3>) == 0x58, "Interp<IVec3>");
static_assert(sizeof(Interp<Vec2>) == 0x44, "Interp<Vec2>");
static_assert(sizeof(Interp<float>) == 0x30, "Interp<float>");
static_assert(sizeof(Interp<i32>) == 0x30, "Interp<i32>");

// 侵入式双向链表结点（原版 0x10 字节：owner, next, prev, extra）。
struct AnmVm;
struct AnmListNode {
    AnmVm* owner = nullptr;
    AnmListNode* next = nullptr;
    AnmListNode* prev = nullptr;
    void* extra = nullptr;
};

struct AnmVm {
    Timer interrupt_return_time;      // 0x000 中断时保存的脚本时刻（ins_7 返回）
    i32 interrupt_return_offset = 0;  // 0x014
    i32 layer = 0;                    // 0x018
    i32 file_slot = 0;                // 0x01c
    i32 sprite_index = -1;            // 0x020
    i32 script_index = 0;             // 0x024
    i32 ins_offset = -1;              // 0x028（-1 = 停止执行）
    Vec3 pos;                         // 0x02c
    Vec3 rotation;                    // 0x038
    Vec3 angular_velocity;            // 0x044
    Vec2 scale{1, 1};                 // 0x050
    Vec2 scale2{1, 1};                // 0x058
    Vec2 scale_growth;                // 0x060
    Vec2 uv_scale{1, 1};              // 0x068
    Vec2 sprite_size;                 // 0x070（精灵宽高，ins_603 等可覆盖）
    Vec2 uv_scroll;                   // 0x078
    Vec2 anchor_offset;               // 0x080（ins_436）
    float unknown_088 = 0;            // 0x088
    Interp<Vec3> pos_interp;          // 0x08c
    Interp<IVec3> color_interp;       // 0x0e4
    Interp<i32> alpha_interp;         // 0x13c
    Interp<Vec3> rotation_interp;     // 0x16c
    Interp<float> angle_interp;       // 0x1c4（ins_411）
    Interp<Vec2> scale_interp;        // 0x1f4
    Interp<Vec2> scale2_interp;       // 0x238
    Interp<Vec2> uv_scale_interp;     // 0x27c
    Interp<IVec3> color2_interp;      // 0x2c0
    Interp<i32> alpha2_interp;        // 0x318
    Interp<float> u_scroll_interp;    // 0x348
    Interp<float> v_scroll_interp;    // 0x378
    Vec2 uv[4];                       // 0x3a8 四角 UV：(u0,v0) (u1,v0) (u0,v1) (u1,v1)
    Vec2 uv_scroll_velocity;          // 0x3c8
    float texture_matrix[16];         // 0x3d0 对角 (w/256, h/256, 1, 1)
    float texture_matrix_copy[16];    // 0x410
    float sprite_matrix[16];          // 0x450 对角 (w/texw*ratio, h/texh*ratio, 1, 1)
    i32 pending_interrupt = 0;        // 0x490
    i32 sprite_set_time = 0;          // 0x494
    i32 unknown_498 = 0;              // 0x498
    i16 template_script = 0;          // 0x49c
    i16 unknown_49e = 0;              // 0x49e
    i32 int_vars[4] = {};             // 0x4a0  $I0..$I3（10000..10003）
    float float_vars[4] = {};         // 0x4b0  $F0..$F3（10004..10007）
    float float_vars2[3] = {};        // 0x4c0  10033..10035
    i32 int_vars2[2] = {};            // 0x4cc  10008..10009
    float rand_scale = 1;             // 0x4d4  10027 / 随机幅度
    float rand_angle_scale = 0;       // 0x4d8  10028
    i32 rand_int_max = 0;             // 0x4dc  10029
    Vec3 pos2;                        // 0x4e0（flag 0x400 时 ins_400 写这里）
    Vec3 drawn_corners[4];            // 0x4ec 0x465280 写入的四角屏幕坐标（左上、右上、左下、右下）
    i32 creation_flags = 0;           // 0x51c（ins_500 系列的列表选择位）
    u8 color[4] = {255, 255, 255, 255};   // 0x520 b,g,r,a
    u8 color2[4] = {};                // 0x524 b,g,r,a
    u32 drawn_color = 0;              // 0x528 上次绘制实际用的顶点色（ins_315 子 VM 据此与父相乘）
    u8 unknown_52c[4] = {};           // 0x52c（0x429b20 / 0x4197c0 写 0x15 / 0x10，未见读取）
    u32 flags_lo = 0;                 // 0x530
    u32 flags_hi = 0;                 // 0x534
    u32 id = 0;                       // 0x538（池下标 | 代数<<13）
    u32 pool_index = 0;               // 0x53c
    Timer time;                       // 0x540 脚本时刻（指令 time 字段与之比较）
    Timer age;                        // 0x554 存活帧计数
    AnmListNode world_node;           // 0x568 世界/UI 链表
    AnmListNode sibling_node;         // 0x578 父 VM 的子表结点
    AnmListNode children;             // 0x588 子表头（owner = 自身，next = 首个子结点）
    AnmListNode delete_node;          // 0x598 待删除链表
    AnmVm* layer_next = nullptr;      // 0x5a8 本帧绘制链（按 layer）
    AnmVm* root = nullptr;            // 0x5ac 最顶层祖先（0x406340 沿此链取慢放系数）
    AnmVm* parent = nullptr;          // 0x5b0 直接创建者
    float slowdown = 0;               // 0x5b4
    void* vertex_buffer = nullptr;    // 0x5b8（600 系列几何）
    u32 vertex_buffer_size = 0;       // 0x5bc
    i32 time_callback = 0;            // 0x5c0 DAT_004c0f44 表下标
    i32 update_callback = 0;          // 0x5c4 DAT_004919e8 表下标
    i32 draw_callback = 0;            // 0x5c8
    i32 delete_callback = 0;          // 0x5cc DAT_00491b58 表下标
    i32 interrupt_callback = 0;       // 0x5d0 DAT_00491b0c 表下标（0x46f0b0/0x46f130 置中断前调用）
    u8 unknown_5d4[0x8] = {};         // 0x5d4
    i32 sprite_mapper = 0;            // 0x5dc DAT_00491b1c 表下标（ins_300 精灵映射钩子）
    Vec3 entity_offset;               // 0x5e0（ins_306 flag 0x8000 时每帧累加全局偏移）
    i32 unknown_5ec = 0;              // 0x5ec
    Vec3 world_rotation;              // 0x5f0（0x464dd0 结果）
    // —— 以下为移植附加字段（原版结构到此为 0x5fc）——
    // ins_300(-1) 时原版用 ascii.anm 的 0x102 号精灵填 UV（0x45edb0），但绘制（0x465280/0x467410）仍按
    // file_slot 的精灵表取纹理号——下标 0x102 对 title.anm 等已越界，读到的是堆上邻接数据。
    // 这里记录 UV 真正来源的文件槽，按作者意图取该文件的纹理，避免复现未定义读取。
    i32 sprite_source_slot = -1;
    // 原版 0x5ec 存宿主对象指针（敌弹 0x412cb0 / 激光 / 道具写入，sprite_mapper 钩子回读）；
    // 为保持 wasm32 下 0x5fc 之前的布局原样，指针放在附加区，0x5ec 保留原槽位。
    void* owner = nullptr;
    // 0x46eab0 回收时 free(vertex_buffer)：原版为 CRT 堆块；移植版由分配方给出析构函数（EffectManager 的种类缓冲等）
    void (*vertex_buffer_free)(void*) = nullptr;

    // 原版 flags_lo（0x530）位
    enum : u32 {
        kVisible = 1u << 0,
        kFlag2 = 1u << 1,
        kRotationDirty = 1u << 2,
        kScaleDirty = 1u << 3,
        kUvScaleDirty = 1u << 4,
        kBlendShift = 5, kBlendMask = 0x1e0u,
        kUsePos2 = 1u << 10,
        kFlipX = 1u << 11,
        kFlipY = 1u << 12,
        kZWriteDisable = 1u << 13,
        kStopped = 1u << 14,
        kEntityOffset = 1u << 15,
        kColorModeMask = 0x60000u,
        kPaused = 1u << 20,
        kAnchorXMask = 0x600000u, kAnchorYMask = 0x1800000u,
        kRenderModeShift = 25, kRenderModeMask = 0x3e000000u,
        kFlag312HiMask = 0xc0000000u,
    };
    // flags_hi（0x534）位
    enum : u32 {
        kHiBlend2Mask = 0x3u,
        kHi437Mask = 0x1cu,
        kHiDeleting = 1u << 6, kHiDeletingMask = 0x60u,
        kHi424 = 1u << 7,
        kHiRes431 = 1u << 8,
        kHiIgnoreSpeed = 1u << 9,
        kHiHighRes = 1u << 10,        // ins_307：1280 资源按 0.5 绘制
        kHi311 = 1u << 11,
        kHiPauseMask = 0xc000u,
        kHiScreenUv = 1u << 13,
        kHiDetached = 1u << 16,       // ins_507：不随父变换
        kHiLayerGroupMask = 0x1c0000u,
        kHiScaleModeMask = 0x700000u, // ins_313
        kHi314 = 1u << 23,
        kHiHasVelocity = 1u << 24,
        kHi315 = 1u << 25,
    };

    int blend_mode() const noexcept { return int((flags_lo & kBlendMask) >> kBlendShift); }
    int render_mode() const noexcept { return int((flags_lo & kRenderModeMask) >> kRenderModeShift); }

    // 原版 memset(vm, 0, 0x5fc)：原版字段全部清零，移植附加字段回到默认值（精灵来源槽 -1 = 跟随 file_slot；
    // 宿主指针与顶点缓冲析构函数置空——它们由生成后的宿主重新写入）
    void clear() noexcept;
};

// 0x406d80：设 layer 并按区间改 flags_hi 的层组位
void anm_set_layer(AnmVm& vm, i32 layer) noexcept;
// 0x4173f0 / 0x425f10 / 0x425e70 / 0x426020 / 0x425dd0 等插值器启动（Timer 归零；bezier 按各原函数清零与否）
inline void anm_begin_vec3_interp(Interp<Vec3>& it, i32 time, i32 mode, const Vec3& initial, const Vec3& goal) noexcept {
    it.end_time = time;
    it.bezier1 = {}; it.bezier2 = {};
    it.mode = mode;
    it.initial = initial;
    it.goal = goal;
    it.timer.reset();
}
inline void anm_begin_color_interp(Interp<IVec3>& it, i32 time, i32 mode, const u8* cur, u8 b, u8 g, u8 r) noexcept {
    it.end_time = time;
    it.bezier1 = {}; it.bezier2 = {};
    it.mode = mode;
    it.initial = {cur[0], cur[1], cur[2]};
    it.goal = {b, g, r};
    it.timer.reset();
}
inline void anm_begin_scalar_interp(Interp<i32>& it, i32 time, i32 mode, i32 initial, i32 goal) noexcept {
    it.end_time = time; it.mode = mode; it.initial = initial; it.goal = goal; it.bezier1 = 0; it.bezier2 = 0; it.timer.reset();
}
inline void anm_begin_vec2_interp(Interp<Vec2>& it, i32 time, i32 mode, const Vec2& initial, const Vec2& goal) noexcept {
    it.end_time = time; it.mode = mode; it.initial = initial; it.goal = goal; it.timer.reset();
}

static_assert(offsetof(AnmVm, interrupt_return_offset) == 0x14, "layout");
static_assert(offsetof(AnmVm, ins_offset) == 0x28, "layout");
static_assert(offsetof(AnmVm, pos) == 0x2c, "layout");
static_assert(offsetof(AnmVm, scale) == 0x50, "layout");
static_assert(offsetof(AnmVm, uv_scroll) == 0x78, "layout");
static_assert(offsetof(AnmVm, pos_interp) == 0x8c, "layout");
static_assert(offsetof(AnmVm, color_interp) == 0xe4, "layout");
static_assert(offsetof(AnmVm, alpha_interp) == 0x13c, "layout");
static_assert(offsetof(AnmVm, rotation_interp) == 0x16c, "layout");
static_assert(offsetof(AnmVm, angle_interp) == 0x1c4, "layout");
static_assert(offsetof(AnmVm, scale_interp) == 0x1f4, "layout");
static_assert(offsetof(AnmVm, scale2_interp) == 0x238, "layout");
static_assert(offsetof(AnmVm, uv_scale_interp) == 0x27c, "layout");
static_assert(offsetof(AnmVm, color2_interp) == 0x2c0, "layout");
static_assert(offsetof(AnmVm, alpha2_interp) == 0x318, "layout");
static_assert(offsetof(AnmVm, u_scroll_interp) == 0x348, "layout");
static_assert(offsetof(AnmVm, v_scroll_interp) == 0x378, "layout");
static_assert(offsetof(AnmVm, uv) == 0x3a8, "layout");
static_assert(offsetof(AnmVm, uv_scroll_velocity) == 0x3c8, "layout");
static_assert(offsetof(AnmVm, texture_matrix) == 0x3d0, "layout");
static_assert(offsetof(AnmVm, sprite_matrix) == 0x450, "layout");
static_assert(offsetof(AnmVm, pending_interrupt) == 0x490, "layout");
static_assert(offsetof(AnmVm, template_script) == 0x49c, "layout");
static_assert(offsetof(AnmVm, int_vars) == 0x4a0, "layout");
static_assert(offsetof(AnmVm, float_vars) == 0x4b0, "layout");
static_assert(offsetof(AnmVm, rand_scale) == 0x4d4, "layout");
static_assert(offsetof(AnmVm, pos2) == 0x4e0, "layout");
static_assert(offsetof(AnmVm, creation_flags) == 0x51c, "layout");
static_assert(offsetof(AnmVm, color) == 0x520, "layout");
static_assert(offsetof(AnmVm, flags_lo) == 0x530, "layout");
static_assert(offsetof(AnmVm, id) == 0x538, "layout");
static_assert(offsetof(AnmVm, time) == 0x540, "layout");
static_assert(offsetof(AnmVm, age) == 0x554, "layout");
static_assert(offsetof(AnmVm, world_node) == 0x568, "layout");
// 0x568 之后含指针字段：仅 wasm32 / 32 位目标与原版字节布局一致（本机 64 位 harness 仅校验语义）。
static_assert(sizeof(void*) != 4 || offsetof(AnmVm, children) == 0x588, "layout");
static_assert(sizeof(void*) != 4 || offsetof(AnmVm, layer_next) == 0x5a8, "layout");
static_assert(sizeof(void*) != 4 || offsetof(AnmVm, slowdown) == 0x5b4, "layout");
static_assert(sizeof(void*) != 4 || offsetof(AnmVm, update_callback) == 0x5c4, "layout");
static_assert(sizeof(void*) != 4 || offsetof(AnmVm, sprite_mapper) == 0x5dc, "layout");
static_assert(sizeof(void*) != 4 || offsetof(AnmVm, entity_offset) == 0x5e0, "layout");
static_assert(sizeof(void*) != 4 || offsetof(AnmVm, world_rotation) == 0x5f0, "layout");
static_assert(sizeof(void*) != 4 || offsetof(AnmVm, sprite_source_slot) == 0x5fc, "原版 0x5fc 字节之后才是移植附加字段（wasm32 指针 4 字节）");

// 0x464dd0：沿父链累加旋转写入 world_rotation 并归一化自身 rotation；返回 world_rotation
Vec3 anm_world_rotation(AnmVm& vm);
// 0x40e490：pos + entity_offset + pos2，未分离时随父 Z 旋转并沿父链累加（不含窗口缩放与层组偏移；
// 炸弹 / 季节解放以此取光环 VM 的场上坐标）
Vec3 anm_world_position(AnmVm& vm);
// 0x45edb0：把 file 的第 index 号精灵装进 VM（UV、尺寸、纹理 / 精灵矩阵）；资源未载入返回 -1
struct AnmFile;
int anm_set_sprite(AnmFile& file, AnmVm& vm, i32 index);
// 0x406e10：Vec3 插值推进（StageManager 相机 pos/facing/up 共用）
Vec3 step_interp_vec3(Interp<Vec3>& it, float game_speed);
// 0x4171c0：float 插值推进（Player 缩放 +0x2c798）
float step_interp_float(Interp<float>& it, float game_speed);
// 0x425570：Vec2 插值推进（敌机 radius / radius_speed）
Vec2 step_interp_vec2(Interp<Vec2>& it, float game_speed);
// 0x4033f0：缓动曲线（mode 0..）
float interp_ease(int mode, float t, float end) noexcept;
// 模式 8 的 Hermite 曲线：goal*h01 + initial*h00 + b1*h10 + b2*h11
float hermite(float initial, float goal, float b1, float b2, float r) noexcept;

}  // namespace th16
