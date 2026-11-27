// th16 得分飘字——原版 DAT_004a6f10（0x449e50 创建，0x1034 字节；更新链 0x14 → 0x449ea0、绘制链 0x2f → 0x44a000）。
//
//   0x449c70  构造：各飘字计时器清 bit0、整体清零、flags |= 2
//   0x449cd0  取 ascii.anm；两条链建为禁用（关卡首帧 0x42dc50 / 0x42dee0 启用）；数字 VM 复位（0x4090f0）、
//             精灵 0x103、flags_hi 层组 0x80000（屏幕坐标 +(320,16)）与缩放模式 3
//   0x44a460  入队：10 项环；value < 0 → 单个特殊字形（数字 10）；否则十进制各位（低位在前）；上升速度 1
//   0x449ea0  每帧：y -= 速度 × 游戏速度，速度 ×= 0.95，计时 > 0x3c 结束
//   0x44a000  绘制：字宽 8（头 8 帧为 8 / 计时，首帧因此为 ∞——原版如此）；离自机 ≤ 64 像素时变淡；
//             各位从高到低按出场时序选"普通 / 放大 1 / 放大 2"三套字形，再向右推进一字宽
//   0x449d60  析构：移除两条链
//
// 入队方：道具拾取（0x42f4e0 / 0x430100 族）、擦弹（0x444cf0）、ECL 638（0x41dcb0 case 0x95）。
// 原版对象后半（+0x9bc 起 5 项 "BONUS %.1f" / "NO BONUS" 飘字，0x449ea0 淡出、0x44a000 经 AsciiManager 画出）
// 在 th16 没有任何入队方，属前作遗留的死代码，不移植。
#pragma once
#include "AnmVm.hpp"
#include "Types.hpp"

namespace th16 {

class AnmManager;
class AnmRenderer;
struct AnmFile;
class Player;

class ScorePopupManager {
public:
    static constexpr int kRingSize = 10;       // 0x44a460 的环大小
    static constexpr int kDigitPopups = 13;    // 0x44a000 / 0x449ea0 遍历的数字飘字数（环之外的 3 项从不激活）
    static constexpr int kMaxDigits = 12;
    static constexpr i32 kLifetime = 0x3c;

    struct Popup {
        u8 digits[kMaxDigits] = {};   // +0x00：十进制各位，低位在前；10 = 特殊字形
        Vec3 pos;                     // +0x0c
        float rise_speed = 0;         // +0x18
        u32 color = 0;                // +0x1c
        Timer timer;                  // +0x20
        bool active = false;          // +0x3c
        u8 digit_count = 0;           // +0x3d
    };

    // 0x449cd0：绑定 ascii.anm 并初始化数字 VM
    void create(AnmFile& ascii);
    // 0x44a460
    void push(const Vec3& pos, i32 value, u32 color);
    // 0x449ea0
    void update(float game_speed);
    // 0x44a000
    void draw(AnmRenderer& renderer);

    const Popup& digit_popup(int i) const { return digit_popups_[i]; }
    int ring_cursor() const { return ring_cursor_; }

    const Player* player = nullptr;   // DAT_004a6ef8：离自机近时变淡

private:
    void draw_digit_popup(AnmRenderer& renderer, const Popup& popup);
    u8 alpha_near_player(const Popup& popup) const;

    AnmFile* ascii_ = nullptr;     // +0x0c
    i32 ring_cursor_ = 0;          // +0x10
    AnmVm glyph_vm_;               // +0x18
    Popup digit_popups_[kDigitPopups];   // +0x614
};

}  // namespace th16
