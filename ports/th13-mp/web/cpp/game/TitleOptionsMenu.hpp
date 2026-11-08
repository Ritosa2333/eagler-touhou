// 标题 Option（オプション）屏（原版 supervisor 0x44c570 case 3 → 状态机
// FUN_0044dd00 + 音量 VM 刷新 FUN_0044ed70，th13.exe v1.00c 反汇编实证）：
//   屏值 3；5 行（0=BGM音量 1=SE音量 2=キー配置 3=デフォルト 4=やめる）。
//   case0：光标归 0（FUN_00415030(0)）+ 音量 VM 刷新（0044ed70）→ substate1。
//   case1：屏龄 >6 → substate2；immediate label3（全行熄灭）+ pending label
//   17+行（入场选中态）。case2：↑↓ 光标 ±1 环绕（FUN_00473210）音10，
//   redraw = immediate label3 + pending label7+行；X/Esc(0x102) 行≠4 → 音9
//   + 光标跳行4；行0/1 ←→(0x40/0x80) ±5 钳 0..100（无音效）+ 0044ed70；
//   Z(0x80001) 行2 音7→substate4（>9 帧切键配置屏 = case4→screen4）、
//   行3 音7 + 音量复位 100/80（DAT_004dc894=0 同拍，v1.00c 无读者）+
//   0044ed70、行4 音9→substate4（>9 帧回主菜单 + 光标栈 pop 004731d0）。
//   VM：title.anm root1 @(70,312) spawn 行条 23-27/28-32、BGM 数字 33-35
//   (s50+d) + 后缀 36、BGM 大数字 37-39 (s61+d) + 40、SE 41-43/44、45-47/48；
//   数字 sprite = 值+50（小）/+61（大）（FUN_00461a60），前导零隐藏 =
//   百位仅 v≥100 显示、十位仅 v≥10 显示（VM+0x18 bit1，0044ed70 全分支
//   实读）。原版 root1 由标题初始化常驻；移植按 build_difficulty 重入惯例
//   开屏建、离屏 label1 收（视觉等价，生命周期差异登记 integration.md）。
// 音量语义（原版曲线）：BGM = DAT_004dc892 → DAT_004e476c（0x476234 门 +
// (1-(v/100-1)²) 曲线，AudioDevice::music_volume 已实现）；SE = DAT_004dc893
// → DAT_004e4770（0x4613f3 每 voice：se==0 静音 -10000，否则
// round((1-(se/100-1)²)*(def+5000))-5000）。DAT_004e4774（0044ed70 内 u⁴
// 变体）在 v1.00c 全 EXE 无读者，不移植。
#pragma once

#include "TitleState.hpp"
#include "TitleMainMenu.hpp"
#include "TitleAnimations.hpp"

#include <functional>
#include <string>

namespace th13 {

// TitleScreen 暂无 Option 成员（TitleState.hpp 归 Title 接线所有者）；
// 原版数值 = 3（0x44c570 case 表）。所有者补 enum 后以常量对齐。
inline constexpr i32 kTitleScreenOption = 3;

// 音量对（原版存档字节 DAT_004dc892/893；默认 100/80 = 0x5064）。
struct TitleOptionVolumes {
    i32 bgm = 100, effect = 80;
};

// 宿主服务：音量应用与持久化。默认实现显式失败（返回 false/无操作），
// 不假成功；Application/集成方接线 FileStore 与 AudioDevice。
struct TitleOptionPlatform {
    virtual ~TitleOptionPlatform() = default;
    // 0044ed70 侧 DAT_004e476c/70 写：每次音量变化与开屏时调用。
    virtual void apply_volumes(i32 bgm, i32 effect) { (void)bgm; (void)effect; }
    // 原版离开 Option 时随 scoreth13.dat 落盘；网页 = FileStore.save()。
    // 默认 false = 未接持久化（真实失败，菜单继续可用并记录 error）。
    virtual bool persist_options(const TitleOptionVolumes&) { return false; }
    // 行2 キー配置（case4 → screen 4 = FUN_0044fcc0，独立屏后接）。
    // 默认 false = 键配置屏未接线。
    virtual bool open_key_config() { return false; }
};

class TitleOptionsMenu {
    TitleState& state;
    AnmManager& animations;
    TitleOptionPlatform& host;
    TitleOptionVolumes& volumes;  // 宿主共享真值（FileStore 装载/本屏编辑）
    TitleAnimations visuals;
    TitleSceneResources resources;
    i32 row_ = 0;             // +0x28 光标行（0..4，环绕，无禁用行）
    i32 substate_frames_ = 0; // 子态屏龄（原版 +0x2b8 差值；>6 / >9 门）
    bool key_config_ = false; // substate4 出口：true=行2 键配置，false=行4 回主菜单
    bool pushed_ = false;     // 光标栈 push（开屏 0x73190 / 离屏 0x731d0 对合）
    bool fail(const std::string& reason);
    bool sound(i32 id);
    bool open();           // case0：建面 + 音量刷新 + 光标归 0
    bool refresh_volumes();  // 0044ed70：数字 sprite/可见位 + host.apply
    void select_row(i32 label_base);  // immediate label3 + pending label(基+行)
    bool leave(bool to_key_config);   // case4 出口（>9 帧）：收面/持久化/切屏请求

public:
    std::function<bool(i32)> play_sound;
    std::string error;
    TitleOptionsMenu(TitleState& state_, AnmManager& animations_, TitleOptionPlatform& host_,
                     TitleOptionVolumes& volumes_, TitleSceneResources bank)
        : state(state_), animations(animations_), host(host_), volumes(volumes_),
          visuals(state_, animations_, bank.title, bank.ascii), resources(bank) {}
    // 每帧驱动（TitleFrame 屏值 3 分支接线；pressed/repeated 与他屏同义）。
    bool update(u32 pressed, u32 repeated);
    i32 row() const { return row_; }             // 测试/采样点
    const TitleOptionVolumes& volumes_ref() const { return volumes; }
};

}  // namespace th13
