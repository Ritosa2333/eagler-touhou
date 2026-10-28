// 标题 音楽室（Music Room）屏（原版 supervisor 0x44c570 case d → 状态机
// FUN_004554f0，th13.exe v1.00c 反汇编实证）：
//   屏值 13（0xd）。case0（age==1）：光标归 0 + 建文本/VM 队列对象（0x706c0
//   家族）+ 读 th13.dat 的 musiccmt.txt（0x4724c0：'@' 分块 = 曲目，每块曲名
//   + 8 行注释，0x42/0x40 步长表）→ 曲数 +0x30；age≥10 进 case1。
//   case1：隔帧逐行显（+0x74c<8）；age>4 → case2。
//   case2：↑↓ FUN_00473210 环绕（音10）；Z：选中曲未听过（score 数据块
//   +0x1b27a+track 字节 == 0，0x4554f0:193/237 实读）→ 播锁定响应
//   0x461830(3/4,0)（DAT_004dc8a8&0x10 分支），已听过 → 0x43adc0(0,曲名)
//   播曲 + 0x461830(2,0) + 落档 score[0x1b27a]=1（任意播曲解锁 0 号曲）；
//   X（0x102）：音9 → case3。
//   case3（>9 帧）：收面（+0x470/+0x690 VM 树 label1）→ change_screen(1)
//   回主菜单 + 恢复标题曲 th13_00（0x43adc0）+ 0x43ae40(0,0) + 光标栈 pop。
//   行显示：10 行窗口（0x4554f0:190 页顶+10 显隐门），行距 20.0（fadd 步长
//   实读），选中行 x+4（±4 poke 对），未选中 pending label3 / 选中 label2；
//   锁定曲名走 DAT_004ac930/PTR_004c0da0 占位（？？？）色 0x8080ff，已听
//   曲名 0xffffff。绝对 origin 无本函数证据 → 沿用队列 (58,80) 起点待对拍。
//   注释 8 行 = 选中曲块内文本，隔帧逐行显（原版同拍）；Z 后 +0x750=选中
//   +0x74c=0 重置显影（注释查看路径）。
// 持久化：曲目听过位 = 原版 score+0x1b27a 字节族 ↔ FileStore v1 保留字
// reserved[0] 位掩（bit=曲号，无新存档框架；宿主 TitleMusicRoomPlatform
// 转发）。锁定响应/播曲/恢复标题曲均宿主服务，默认实现显式失败不假成功。
#pragma once

#include "TitleState.hpp"
#include "TitleMainMenu.hpp"
#include "HudDrawData.hpp"

#include <functional>
#include <string>

namespace th13 {

inline constexpr i32 kTitleScreenMusicRoom = 13;  // 0x44c570 case d

// 宿主接口：musiccmt.txt 曲目表（cp932 解码由宿主）、真实档听过位、
// 播放与退出恢复。默认实现返回空/失败——接线前菜单可见但操作如实报错。
struct TitleMusicRoomPlatform {
    virtual ~TitleMusicRoomPlatform() = default;
    virtual i32 track_count() const { return 0; }
    virtual std::string track_title(i32 track) { (void)track; return {}; }
    virtual std::string track_comment(i32 track, i32 line) { (void)track; (void)line; return {}; }
    virtual bool track_heard(i32 track) const { (void)track; return false; }  // 新档全 false
    virtual void mark_heard(i32 track) { (void)track; }  // 0x4554f0 Z 路径落档
    // 已听曲播放（0x43adc0(0,曲名)+0x461830(2,0)）；宿主负责实际曲目映射。
    virtual bool play_track(i32 track) { (void)track; return false; }
    // 未听曲响应（0x461830(3/4,0)，DAT_004dc8a8&0x10 分支留给宿主）。
    virtual bool play_locked_response() { return false; }
    // 退出收面恢复 th13_00（0x43adc0(0,"th13_00")）。
    virtual bool resume_title_music() { return false; }
    virtual std::string locked_title() const { return "???"; }  // DAT_004ac930 占位族
};

class TitleMusicRoomMenu {
    TitleState& state;
    TitleMusicRoomPlatform& host;
    i32 row_ = 0;               // +0x28 选中曲（0..count-1 环绕）
    i32 reveal_lines_ = 0;      // +0x74c 注释/行显影计数（隔帧 +1，封 8）
    i32 frames_ = 0;            // 子态屏龄（case0 age≥10 / case3 >9 门）
    bool fail(const std::string& reason);
    bool sound(i32 id);
    bool leave();               // case3：收面 + 恢复标题曲 + 回 Main

public:
    std::function<bool(i32)> play_sound;
    std::string error;
    TitleMusicRoomMenu(TitleState& state_, TitleMusicRoomPlatform& host_)
        : state(state_), host(host_) {}
    bool update(u32 pressed, u32 repeated);
    // 绘制侧逐帧入队（TitleFrame Draw 回调 → TitleScene → 此处；仅 substate
    // 2 出列表+注释，对应 0x4554f0 case1/case2 文本队列值域）。
    bool draw_texts(HudDrawServices& draw);
    i32 row() const { return row_; }
};

}  // namespace th13
