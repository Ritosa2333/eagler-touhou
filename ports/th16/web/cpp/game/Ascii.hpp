// th16 AsciiManager——原版 DAT_004a6d98（0x19254 字节，0x407c10 / 0x407cb0 构造）：一帧寿命的屏幕文字队列。
//
// 用法与原版一致：调用方先改 `style`（颜色 / 缩放 / 字体 / 组 / 对齐），再把带坐标的字符串入队；
// 每帧更新链先把寿命耗尽的条目清掉，三条绘制链再按"组"把文字用 ascii.anm 的字形精灵逐字画出。
// 文字默认只活一帧（lifetime 0），所以 HUD 每帧都重新入队。
//
// 锚点：
//   0x407c10  默认样式：白色、缩放 1、字体 0、组 0、左对齐 / 顶对齐、默认字形宽 9
//   0x407cb0  装 ascii.anm（窗口缩放 2 / 3 倍时换 ascii_960 / ascii_1280）到槽 2；登记更新链 4 与绘制链 0x51 / 0x35 / 0x42；
//             字形 VM（+0x10）取精灵 0，第二个 VM（+0x60c）取精灵 0x62
//   0x408140  入队：抄样式、坐标乘窗口缩放；上限 0x140 条（条目区实际只有 200 条，原版从不逼近）
//   0x408260  printf 入队（vsprintf 到 260 字节）        0x4084f0  同上但字体 1（FPS 显示用）
//   0x4082b0  千分位分组的整数                             0x4083b0  分数：千分位分组后再接一位（续关数）
//   0x408fb0  更新：寿命 -1，< 0 的条目前移删除           0x408070  更新链：0x408fb0 + 帧计数 +1
//   0x408560  画一组：逐条 0x408650，最后出批并切回 UI 相机
//   0x408650  画一条：字形 VM 置可见 / 左上锚点 / 缩放模式 0，按字体定字宽行高与采样方式，按对齐预退坐标，
//             逐字选精灵（字体 0 = 字符 − 0x20；1 = 字符 + 0x42；2/3 = 字母 + 0x74 / 其它 + 0x94 与专用符号；
//             4/5 = 字符 + 0xbf 与专用符号），阴影先画半透明黑再画本体，经 0x465c40 + 0x465280 入批
//   0x408090  绘制链 0x51：组 0（UI 相机）
//   0x4080a0 → 0x408ef0  绘制链 0x35：切场地相机、出批、层组位 0x80000（坐标加 (320, 16)）画组 1，再切回 UI 相机
//   0x4080b0  绘制链 0x42：切场地相机画组 2，再切回 UI 相机
#pragma once
#include <cstdarg>

#include "AnmVm.hpp"
#include "Types.hpp"

namespace th16 {

class AnmManager;
class AnmRenderer;
struct AnmFile;
struct Camera;

class AsciiManager {
public:
    // 字体（条目 +0x120）：决定字宽 / 行高、精灵选择与采样方式（3 / 5 用最近点采样）
    enum class Font : i32 {
        Default = 0,        // 默认字形宽 × 缩放，行高 14；缩放 ≠ 1 时最近点采样
        Small = 1,          // 6 × 9
        Medium = 2,         // 7 × 10
        MediumNearest = 3,  // 7 × 10，最近点采样
        Large = 4,          // 12 × 16
        LargeNearest = 5,   // 12 × 16，最近点采样
    };
    // 绘制组（条目 +0x128）：决定哪条绘制链、哪台相机画它
    enum class Group : i32 {
        Ui = 0,         // 链 0x51，UI 相机（HUD 右栏文字）
        Field = 1,      // 链 0x35，场地相机 + 层组偏移（场地底部的季节量表数字）
        FieldLate = 2,  // 链 0x42，场地相机（符卡奖励 / Boss 计时等场上文字）
    };
    enum class HAlign : i32 { Center = 0, Left = 1, Right = 2 };
    enum class VAlign : i32 { Middle = 0, Top = 1, Bottom = 2 };

    struct Style {
        u32 color = 0xffffffff;          // +0x1920c（AARRGGBB；+0x1920f 为 alpha 字节）
        Vec2 scale{1.0f, 1.0f};          // +0x19210
        i32 reserved = 0;                // +0x19218：抄进条目 +0x11c，绘制不读
        bool shadow = false;             // +0x19220：先画半透明黑影
        Font font = Font::Default;       // +0x19224
        Group group = Group::Ui;         // +0x19228
        i32 lifetime = 0;                // +0x1922c：存活帧数（0 = 只画一帧）
        HAlign h_align = HAlign::Left;   // +0x19230
        VAlign v_align = VAlign::Top;    // +0x19234
    };

    struct Entry {
        char text[256];        // +0x000
        Vec3 pos;              // +0x100（已乘窗口缩放）
        u32 color;             // +0x10c
        Vec2 scale;            // +0x110
        i32 reserved;          // +0x11c
        Font font;             // +0x120
        bool shadow;           // +0x124
        Group group;           // +0x128
        i32 lifetime;          // +0x12c
        HAlign h_align;        // +0x130
        VAlign v_align;        // +0x134
    };

    static constexpr i32 kMaxEntries = 0x140;    // 0x408140 的上限
    static constexpr i32 kDefaultGlyphWidth = 9; // +0x19238
    static constexpr float kLargeLineHeight = 16.0f;

    Style style;                 // 当前样式：调用方直接改，入队时抄入条目
    float window_scale = 1.0f;   // DAT_004d9d34
    i32 frame = 0;               // +0x1923c

    // 0x407cb0：ascii.anm 已装入槽 2 后绑定；字形 VM 取精灵 0 / 0x62
    void create(AnmManager& animations, AnmFile& ascii);
    void set_alpha(u8 alpha) { style.color = (style.color & 0x00ffffffu) | (u32(alpha) << 24); }

    void print(const Vec3& pos, const char* text);                     // 0x408140
    void printf(const Vec3& pos, const char* format, ...);             // 0x408260
    void printf_small(const Vec3& pos, const char* format, ...);       // 0x4084f0
    void print_grouped(const Vec3& pos, u32 value);                    // 0x4082b0
    void print_score(const Vec3& pos, u32 value, u32 last_digit);      // 0x4083b0

    void update();                                                     // 0x408070 / 0x408fb0
    // 0x408560：画一组并切回 UI 相机。调用方（绘制链）负责事先切到该组的相机
    void draw_group(Group group, AnmRenderer& renderer, const Camera& ui_camera);
    // 0x408ef0 / 0x4080b0：场地相机组——切场地相机、出批、（组 1 时加层组位）画组、切回 UI 相机
    void draw_field_group(Group group, AnmRenderer& renderer, const Camera& field_camera, const Camera& ui_camera);

    i32 entry_count() const { return count_; }
    const Entry& entry(i32 index) const { return entries_[index]; }

private:
    AnmManager* animations_ = nullptr;
    AnmFile* ascii_ = nullptr;
    AnmVm glyph_vm_;         // +0x10：逐字复用的字形 VM
    AnmVm glyph_vm_alt_;     // +0x60c：精灵 0x62 起的第二个字形 VM（文字贴图用）
    Entry entries_[kMaxEntries];
    i32 count_ = 0;          // +0x19208

    void vprintf(const Vec3& pos, const char* format, va_list args);
    void draw_entry(const Entry& entry, AnmRenderer& renderer);        // 0x408650
    void select_glyph(i32 sprite);                                     // 0x409040
};

}  // namespace th16
