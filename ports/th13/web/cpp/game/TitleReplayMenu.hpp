// 标题 Replay 菜单（原版 0x44c570 case b → 0x4525b0 状态机 + 0x452c30 绘制，
// p58 反编译证据：artifacts/verification/p58-replay-menu/）：
//   25 行/页共 3 页（0x4525f0 +0x30=0x19、+0x1e0=3），文件行 origin (58,80)、
//   行距 15（x=0x4aeb54=58f、y=0x4aeb28=80f、0x45312e fadd QWORD
//   0x4aeb20=15.0——25 行末行 y=440 收进 480）；stage 列表 origin (220,128)、
//   行距 18（0x4aeb84=220f、0x4ae9e8=128f、QWORD 0x4aeb30=18.0）；
//   选中色 0xffffff00、未选 0xff808080（0x452f95 掩码算式实读）。
//   case0 建 ascii.anm s19 粒子根 + title.anm s104 徽记（自带 (-256,8)→(32,8)
//   20f 滑入/label1 滑出，dump L7193）；case1 >6 帧进列表；case2 上下移行、
//   左右换页、Z 确认合法录像进选关（7 槽缺关禁用 0x4417c0）、X 经 case5 收面；
//   符卡练习录像（顶层头 +0xA bit1）跳过选关直进 case3；case4 >14 帧收输入、
//   Z 确认关卡（音 50）、X 回列表；case3 >31 帧排队 destination 0xd（经宿主
//   start_replay，菜单 update 内不销毁自身）。
// 动态文字全部走原版 ASCII 队列等价通道（HudDrawServices → AsciiText），
// 格式串逐字节取自原版 .data：0x4c0d8c[0..3] + 0x4c0d9c（No./User/日期/角色/
// 难度/路线/慢速），stage 行 "%s  %.8d%d"/"%s  ---------"（0x4ac818/0x4ac7e4）。
#pragma once

#include "TitleState.hpp"
#include "TitleMainMenu.hpp"
#include "ReplayFile.hpp"
#include "HudDrawData.hpp"
#include "MenuCursor.hpp"
#include "AnmManager.hpp"

#include <functional>
#include <string>

namespace th13 {

// p58 拟定接口（已定稿）：宿主提供 replay 目录与导入服务。默认实现 =
// 空 catalog（现有宿主/测试不接 replay 仍编译、运行不变）。
// replay_entry 借用宿主已解码的不可变对象（不建立另一套 Replay 状态）；
// start_replay 只排队 destination，由宿主在帧尾消费（避免菜单 update 中
// 销毁自身）。replay_count = 槽位总数（原版 0x452300：75 = 25 编号 + 50
// 用户；网页侧末槽为「导入本地 .rpy」入口行，见宿主实现）。
struct TitleReplayPlatform {
    virtual ~TitleReplayPlatform() = default;
    virtual i32 replay_count() const { return 0; }
    virtual const ReplayFile* replay_entry(i32 index) const { (void)index; return nullptr; }
    virtual std::string replay_filename(i32 index) const { (void)index; return {}; }
    virtual bool request_replay_file() { return false; }
    virtual std::string replay_import_error() const { return {}; }
    virtual bool start_replay(i32 file_index, i32 stage_index) {
        (void)file_index;
        (void)stage_index;
        return false;
    }
};

class TitleReplayMenu {
    TitleState& state;
    AnmManager& animations;
    TitleReplayPlatform& replay_host;
    TitleSceneResources resources;
    MenuCursor page;             // 页光标（+0x1d8/+0x1e0：count = 3）
    i32 selected_index = 0;      // +0x5aa0：扁平选中（页*25+行）
    i32 selected_stage_row = 0;  // +0x5aa4：选关行（0 基，关卡 = 行+1）
    i32 pages = 1;
    bool launch_queued = false;
    bool fail(const std::string& reason);
    bool sound(i32 id);
    bool present(u32 handle) const;
    bool create_badge();
    bool open();                 // case0：建面 + 光标恢复
    bool confirm_file();         // case2 Z
    void consume_import_hint();
    const ReplayFile* selected_file() const;
    const ReplayStage* stage_block(const ReplayFile& file, i32 number) const;
    static bool spell_practice(const ReplayFile& file);  // 顶层头 +0xA bit1
    i32 stage_chain_index(const ReplayFile& file) const;
    // 0x452c30 绘制侧：行文本组装（槽位 → 原版格式串）。
    bool file_row_text(i32 index, std::string& out);
    bool stage_row_text(const ReplayFile& file, i32 row, std::string& out);

public:
    std::function<bool(i32)> play_sound;
    std::string error;
    TitleReplayMenu(TitleState& state_, AnmManager& animations_, TitleReplayPlatform& host,
                    TitleSceneResources bank)
        : state(state_), animations(animations_), replay_host(host), resources(bank) {}
    bool update(u32 pressed, u32 repeated);
    // 绘制侧逐帧入队（TitleFrame Draw 回调 → TitleScene → 此处；仅 substate
    // 2/4 出字，与原版 0x452c30 的 screen 值域一致）。
    bool draw_texts(HudDrawServices& host);
};

}  // namespace th13
