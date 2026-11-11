// 标题 キー配置（Pad Config）屏：原版 supervisor 0x44c570 case 4 →
// FUN_0044fcc0（th13.exe v1.00c 反汇编实证），非自由键盘重绑。
//
// 原版结构（0044fcc0 逐 case）：
//   case0：行数 +0x30=7、光标归 0（00415030）、建 VM 根（0044bb80）、
//          substate=1（0044bb20）、把当前五键从全局 DAT_004e4c24/28/34
//          抄到 scratch +0x5a8c..+0x5a94、刷新数字（00450020(scratch[4])）。
//   case1：屏龄 >6 → substate=2；广播 immediate label3（全行熄灭）+
//          pending label 17+行（选中行入场）；00450ac0 行显隐/空标签推进。
//   case2：↑↓ 光标 ±1 环绕（00473210，音10）；
//          原始手柄 32 字节位图（00471510 写 DAT_004e52e8）扫描首个按下位
//          （bit 0..30，iVar3<0x1f）；行<5 时调用 004509d0 学习：
//            新值==旧值 → 无操作（按住不会重复写）；
//            其它行已占用新值 → 那些行改回旧值（重复冲突=交换）；
//            当前行=新值 + 数字刷新 + 音7。
//          原版无独立“等待释放”状态——等值 no-op 已使按住幂等；松开只影响
//          下一次扫描。
//          X/Esc(0x102) 仅在行6（Quit）：从当前全局恢复 scratch、音9、substate=4。
//          Z(0x80001)：行5（Default）从当前全局恢复 scratch + 音7 不离屏；
//          行6 把 scratch 写回全局/配置并音9、substate=4。
//   case4：屏龄 >9 → 0044bac0(state,3) 回 Option 屏 + 004731d0 光标栈 pop。
//
// VM（title.anm entry0 title01.png，anm-list dump 实读）：根 script2
// @(70,312) spawn 49-62 = 7 行灰(层26)/白(层25)字条；63-72 = 5 行×2 位灰数字，
// 73-82 = 同位白数字。字条 sprite 34/36/38/40/42/32/18 = atlas 行 Shot/
// Special/Trance/Slow/Pause/Default/Quit（title01.png 行15/16/17/18/19/14/9
// 位图核对），行0..4 右侧数字 = 手柄按钮号（十位/个位 sprite50+d）。
// 00450020 对灰/白两组数字都写 sprite50+d（白组被灰层覆盖，仍照原版写）。
#pragma once

#include "TitleState.hpp"
#include "TitleMainMenu.hpp"
#include "TitleAnimations.hpp"

#include <array>
#include <functional>
#include <string>

namespace th13 {

inline constexpr i32 kTitleScreenKeyConfig = 4;  // 0x44c570 case 4 -> 0x44fcc0
inline constexpr i32 kTitleKeyConfigRows = 7;
inline constexpr i32 kTitleKeyConfigBindings = 5;

// 宿主面：手柄当前原始按钮位图（bit0..31）与五键绑定持久化。
// 默认实现显式失败/空状态，不假成功；Application 在 open_key_config 时接入
// BrowserRuntime 的 raw_pad_buttons 与 FileStore::pad_bindings。
struct TitleKeyConfigPlatform {
    virtual ~TitleKeyConfigPlatform() = default;
    // 无手柄门控：false 时完全跳过学习（原版无手柄 → 位图全 0 → 不写绑定）。
    virtual bool pad_present() const { return false; }
    virtual u32 pad_buttons() const { return 0; }
    virtual std::array<i16, kTitleKeyConfigBindings> pad_bindings() const {
        return {0, 1, 2, 3, 4};  // 原版默认 pad 按钮序号
    }
    virtual bool persist_pad_bindings(const std::array<i16, kTitleKeyConfigBindings>&) {
        return false;
    }
};

// std::function 适配器（Application 接线用；未接时保持默认失败语义）。
struct TitleKeyConfigHost final : TitleKeyConfigPlatform {
    std::function<bool()> present;
    std::function<u32()> buttons;
    std::function<std::array<i16, kTitleKeyConfigBindings>()> bindings;
    std::function<bool(const std::array<i16, kTitleKeyConfigBindings>&)> persist;
    bool pad_present() const override { return present ? present() : false; }
    u32 pad_buttons() const override { return buttons ? buttons() : 0; }
    std::array<i16, kTitleKeyConfigBindings> pad_bindings() const override {
        return bindings ? bindings() : TitleKeyConfigPlatform::pad_bindings();
    }
    bool persist_pad_bindings(
        const std::array<i16, kTitleKeyConfigBindings>& values) override {
        return persist ? persist(values) : false;
    }
};

class TitleKeyConfigMenu {
    TitleState& state;
    AnmManager& animations;
    TitleKeyConfigPlatform& host;
    TitleAnimations visuals;
    TitleSceneResources resources;
    std::array<i16, kTitleKeyConfigBindings> bindings_{};  // scratch +0x5a8c..
    i32 row_ = 0;             // +0x28 光标行（0..6 环绕）
    i32 substate_frames_ = 0; // 屏龄（>6 / >9 门）
    bool fail(const std::string& reason);
    bool sound(i32 id);
    bool open();                          // case0：建根/抄绑定/刷新数字
    bool refresh_digits();                // 00450020：数字 sprite 写值
    bool learn(i32 button);               // 004509d0：学习 + 重复交换
    void select_row(i32 label_base);      // immediate label3 + pending label
    void revert_bindings();               // scratch = 宿主已保存值（行5 / 取消）
    bool leave();                         // case4：收面 + 光标栈 pop -> Option

public:
    std::function<bool(i32)> play_sound;
    std::string error;
    TitleKeyConfigMenu(TitleState& state_, AnmManager& animations_,
                       TitleKeyConfigPlatform& host_, TitleSceneResources bank)
        : state(state_), animations(animations_), host(host_),
          visuals(state_, animations_, bank.title, bank.ascii), resources(bank) {}
    bool update(u32 pressed, u32 repeated);
    i32 row() const { return row_; }
    const std::array<i16, kTitleKeyConfigBindings>& bindings() const { return bindings_; }
};

}  // namespace th13
