// 通关后标题结算屏（原版标题 supervisor 0x44c570 case 0xe/0xf）：
//   0xe → 0x454050（成绩名前录入/结算）：
//     case0：读榜位、建面/名字默认值；case1：>6 帧进编辑；
//     case2：91 字符网格名字编辑（88=SPACE/89=DEL/90=END，最多 8 字）；
//     case3：收面——DAT_004be7c8（续关数）==0 → FUN_0044bac0(...,0xf)
//     = 0x4549d0（回放保存），否则 FUN_00448080 + 流程 1（主菜单）。
//     入口链：Ending END → 0x43c1c0 case0x10 → 0x44c570 case0 的
//     DAT_004dc670==3 分支（+0x30=10、0x415030(0)、0x473190、
//     0x44bac0(...,0xe)）。
//   0xf → 0x4549d0（回放槽保存）：
//     case0 逐槽 sprintf("th13_%.2d.rpy", 1..25) + 0x447fc0 查已存录像；
//     case2 选槽（+0x28）；原版 Z 后先进本屏自己的 8 字名字编辑（case3），
//     END 才走 0x4484d0 写文件。移植侧按本轮工单语义：前屏 0x454050 已
//     编好名字，本屏 Z 直接用该名字落盘，不二次编辑。
//     窄化登记：0x4549d0 的已存录像列表显示（0x447fc0 逐槽查文件）未接，
//     宿主暂无该查询接口；本屏只画槽号选择行，不伪造列表内容。
#pragma once

#include "TitleState.hpp"
#include "HudDrawData.hpp"

#include <string>

namespace th13 {

struct TitleScenePlatform;

// 名字字符表：原样复制自 game/PauseState.hpp 的 kResultNameCharacters
// （0x454050 case2 字面量 0x4ab984，91 项；尾部三空格被光标语义复用为
// 0x58=SPACE / 0x59=DEL / 0x5a=END）。复制而非包含 PauseState.hpp，避免
// 标题结算组件与暂停状态机耦合。
inline constexpr char kTitleResultNameCharacters[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   ";
// 0x454050 case2 的字面量长度 91（0x58/0x59/0x5a 光标值直接索引最后三格）。
static_assert(sizeof(kTitleResultNameCharacters) == 92,
              "result name table must be 91 entries + NUL");

class TitleResultEntry {
    TitleState& state;
    TitleScenePlatform& host;
    bool fail(const std::string& reason);
    bool name_editor(u32 pressed, u32 repeated);   // 0x454050 case2
    bool slot_selector(u32 pressed, u32 repeated); // 0x4549d0 case2
    void return_main();                            // 0x454050 case3 流程 1
public:
    std::string error;
    TitleResultEntry(TitleState& state_, TitleScenePlatform& host_)
        : state(state_), host(host_) {}
    bool update(u32 pressed, u32 repeated);
    // 屏内文本走标题现有 Draw 通道（TitleScene stage_text_draw → HudDrawServices
    // → AsciiText 队列），不新建渲染路径。
    bool draw_texts(HudDrawServices&);
};

}  // namespace th13
