// 标题 マニュアル（Manual）屏（原版 supervisor 0x44c570 case 10 → 状态机
// FUN_00455330 + help 查看器 0x42d940/0x42d740/0x42da10，th13.exe v1.00c
// 反汇编实证；已纠正「影片」旧猜测）：
//   屏值 16（0x10）。case0（substate0）：建 help 查看器对象（0x42d940：
//   malloc 0x1c0 + memset + DAT_004c2298 全局；0x42d740 初始化失败即释放
//   中止）→ 0x42d940 内部（0x42d740 起）自持页序/翻页输入/纹理装载——
//   页图名 sprintf("help_%.2d.png", page+1)（0x42da10:129 实读），页图
//   help_01.png.. 逐页装载，帧/装饰由 help.anm（bank 19）。supervisor 侧
//   **不做任何输入处理**。case1（substate1）：仅轮询查看器 +0x128 结束位
//   → 0x42d860 停止 + 0x4a7261 释放 + change_screen(1) 回主菜单。
// 移植边界：查看器（页图装载/翻页/结束判定）整体是宿主服务
// TitleManualPlatform——本类只做 supervisor 对合的 开面/轮询/收面。
// help_%.2d.png 页名格式是硬资源断言（宿主实现必须按此命名返回）。
#pragma once

#include "TitleState.hpp"

#include <functional>
#include <string>

namespace th13 {

inline constexpr i32 kTitleScreenManual = 16;  // 0x44c570 case 10

struct TitleManualPlatform {
    virtual ~TitleManualPlatform() = default;
    // 0x42d940+0x42d740：建查看器并载入首页。失败 false = 初始化中止
    //（原版 malloc/初始化失败即释放并停留在空屏语义，由调用方记录）。
    virtual bool open_manual() { return false; }
    // 查看器 +0x128 结束位（用户翻至末页退出/关闭）。
    virtual bool manual_finished() const { return false; }
    // 0x42d860 停止 + 0x4a7261 释放（页纹理/help.anm 帧回收归宿主）。
    virtual void close_manual() {}
    // 资源断言：页 page(0 基) 的页图名必须 "help_%.2d.png"（page+1）。
    virtual std::string help_page_name(i32 page) const {
        char buffer[16];
        std::snprintf(buffer, sizeof buffer, "help_%02d.png", int(page) + 1);
        return buffer;
    }
};

class TitleManualMenu {
    TitleState& state;
    TitleManualPlatform& host;
    i32 frames_ = 0;
    bool fail(const std::string& reason);

public:
    std::string error;
    TitleManualMenu(TitleState& state_, TitleManualPlatform& host_)
        : state(state_), host(host_) {}
    bool update(u32 pressed, u32 repeated);
};

}  // namespace th13
