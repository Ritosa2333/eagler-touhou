// 原版 th13.exe（v1.00c, PE 时间戳 2011-08-17）地址锚点表。
// 每完成一个子系统的行为还原，就在此登记它的原版入口与 C++ 对应物，
// 使 oracle 对照与回归脚本可以直接引用，不再散落硬编码地址。
// 逐子系统的覆盖状态见 docs/fidelity-map.md。
#pragma once

#include <cstdint>

namespace th13::rebuild {

constexpr uint32_t kImageBase = 0x400000;
constexpr uint32_t kEntryPoint = 0x47BDA0;  // PE 头入口（CRT 启动）

// 还原进度登记：子系统名 → 原版入口地址。
// 证据来源逐条写明：oracle 序列对齐，或反编译体与调用图（functions.json 的
// callers/calls）定位出的入口。
struct AddressAnchor {
    const char* subsystem;
    uint32_t originalEntry;
};

constexpr AddressAnchor kAnchors[] = {
    {"rng", 0x47BBA2},  // MSVC CRT _rand()；状态在 _getptd()+0x14；oracle 序列已对齐
    // 每帧入口（反编译体 + 调用图）。supervisor 与 enemy/item/player/spirit 四个管理器没有
    // 直接调用者，由调度器经函数指针每帧调用。
    {"supervisor", 0x439EE0},      // 每帧：0x460dd0 音乐、0x46e510 ANM 资源装载巡检、0x43c1c0 场景分发；
                                   // C++ 对应物 sdl/ApplicationState::step
    {"anm-vm", 0x462450},          // ANM 指令解释器（84 个调用者）；C++ 对应物 game/AnmInterpreter.cpp
    {"ecl-vm", 0x4772A0},          // ECL 基础解释器（线程 step）；C++ 对应物 web/cpp/game EclContext::step，
                                   // 取参 0x478e60/0x478ee0 已对反汇编核对，行为规格见 docs/plans/ecl-vm.md
    {"ecl-dispatch", 0x41B260},    // ECL 指令主分发（经 0x41b250 调用）；C++ 对应物 game/StageCommands.cpp
    {"enemy-manager", 0x418E00},   // 遍历敌机链表，逐机 0x41a8a0；C++ 对应物 game/StageSim.cpp
    {"bullet-manager", 0x40E360},  // 遍历弹表，逐发 0x40db10；C++ 对应物 game/StageSim.cpp
    {"item-manager", 0x42E380},    // 道具每帧序（槽 0..0xa57）；C++ 对应物 game/ItemManager.cpp
    {"player", 0x443240},          // 自机状态机（常规更新 0x442960）；C++ 对应物 game/StageBattle.cpp
    {"spirit", 0x438660},          // 神霊 256 槽每帧序（逐槽 0x438730）；灵界激活在 0x405a20；
                                   // C++ 对应物 game/ItemManager.cpp
    {"replay", 0x448C40},          // 录像装载（头校验/解密/stage 链），保存 0x4484d0；
                                   // C++ 对应物 game/ReplayFile.cpp
    // 对话/关终了/覆盖层三族——行为规格与字节证据见 game/MsgPlayer.hpp 头注释与
    // docs/plans/results-overlay-anatomy.md。
    {"msg-dialog", 0x428ae0},         // 对话执行器（装载 0x424fd0、entry 定位 0x428820:128、
                                      // 文本解密 0x42a340）；C++ 对应物 game/MsgPlayer
    {"stage-clear", 0x42cd40},        // 关终了处理器（每关奖 0x42abf0、六面四式 0x42ce96-0x42ced0、
                                      // Extra 0x42cfb3）；C++ 对应物 game/PostGameFlow::on_stage_clear
    {"post-game-overlay", 0x43dbc0},  // 覆盖层 31 态分发（结算A 0x43e780/GameOver 0x43fa90/
                                      // 结算B 0x440960）；C++ 对应物 game/PostGameFlow
    {"ending-scene", 0x416130},       // 结局场景初始化（0x416570 建对象；结局索引
                                      // 0x41624e-0x416297/落档位 0x1b26a 区/e##.msg 名表
                                      // 0x4bba20/0x4166d0 滚屏对象）；C++ 对应物
                                      // game/EndingScene::initialize
    {"ending-update", 0x416a10},      // 结局脚本解释器（每帧 0x416610 壳；记录
                                      // [u16 行][u8 op][u8 len]，op 表 0x4171e8；
                                      // END → 0x4dcc24=0x10）；C++ 对应物
                                      // game/EndingScene::update
    {"scene-dispatch", 0x43c1c0},     // supervisor 场景调度（case0xf → 0x42c500 战斗
                                      // 拆卸 + 0x416570 Ending；case0x10 → 0x4165f0
                                      // 拆卸 → 4 + dc670=3）；C++ 对应物
                                      // sdl/ApplicationState::apply_destination
    {"score-save", 0x449e50},         // scoreth13.dat 落盘序列化（'CR' 块 0x56dc×5 +
                                      // 0x1b254 尾 + 0x4723a0 加密；0x42bfa0 首指令
                                      // 调用）；C++ 对应物 sdl/FileStore::save
};

}  // namespace th13::rebuild
