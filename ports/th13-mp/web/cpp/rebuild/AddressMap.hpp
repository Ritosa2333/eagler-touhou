// 原版 th13.exe（v1.00c, PE 时间戳 2011-08-17）地址锚点表。
// 每完成一个子系统的行为还原，就在此登记它的原版入口与 C++ 对应物，
// 使 oracle 对照与回归脚本可以直接引用，不再散落硬编码地址。
#pragma once

#include <cstdint>

namespace th13::rebuild {

constexpr uint32_t kImageBase = 0x400000;
constexpr uint32_t kEntryPoint = 0x47BDA0;  // PE 头入口（CRT 启动）

// 还原进度登记：子系统名 → 原版入口地址（0 = 未定位/未还原）
// 已由 Unicorn oracle 验证的条目注明证据。
struct AddressAnchor {
    const char* subsystem;
    uint32_t originalEntry;
};

constexpr AddressAnchor kAnchors[] = {
    {"rng", 0x47BBA2},    // MSVC CRT _rand()；状态在 _getptd()+0x14；oracle 序列已对齐
    {"supervisor", 0},    // 主循环
    {"anm-vm", 0},        // 动画脚本虚拟机
    {"ecl-vm", 0x4772A0},  // ECL 基础解释器（线程 step）；C++ 对应物 web/cpp/game EclContext::step，
                           // 取参 0x478e60/0x478ee0 已对反汇编核对，行为规格见 docs/plans/ecl-vm.md
    {"enemy-manager", 0},
    {"bullet-manager", 0},
    {"item-manager", 0},
    {"player", 0},
    {"spirit", 0},        // th13 特有：神灵/灵界系统
    {"replay", 0},
    // S3-1（2026-10-23）：对话/关终了/覆盖层三族——行为规格与字节证据见
    // game/MsgPlayer.hpp 头注释与 docs/plans/results-overlay-anatomy.md。
    {"msg-dialog", 0x428ae0},      // 对话执行器（装载 0x424fd0、entry 定位 0x428820:128、
                                   // 文本解密 0x42a340）；C++ 对应物 game/MsgPlayer
    {"stage-clear", 0x42cd40},     // 关终了处理器（每关奖 0x42abf0、六面四式 0x42ce96-0x42ced0、
                                   // Extra 0x42cfb3）；C++ 对应物 game/PostGameFlow::on_stage_clear
    {"post-game-overlay", 0x43dbc0},  // 覆盖层 31 态分发（结算A 0x43e780/GameOver 0x43fa90/
                                      // 结算B 0x440960）；C++ 对应物 game/PostGameFlow 骨架
    // Ending 场景（2026-10-28，s6-full-20261028 Part B 反汇编规格）：
    {"ending-scene", 0x416130},   // 结局场景初始化（0x416570 建对象；结局索引
                                  // 0x41624e-0x416297/落档位 0x1b26a 区/e##.msg 名表
                                  // 0x4bba20/0x4166d0 滚屏对象）；C++ 对应物
                                  // game/EndingScene::initialize
    {"ending-update", 0x416a10},  // 结局脚本解释器（每帧 0x416610 壳；记录
                                  // [u16 行][u8 op][u8 len]，op 表 0x4171e8；
                                  // END → 0x4dcc24=0x10）；C++ 对应物
                                  // game/EndingScene::update
    {"scene-dispatch", 0x43c1c0}, // supervisor 场景调度（case0xf → 0x42c500 战斗
                                  // 拆卸 + 0x416570 Ending；case0x10 → 0x4165f0
                                  // 拆卸 → 4 + dc670=3）；C++ 对应物
                                  // sdl/ApplicationState::apply_destination
    {"score-save", 0x449e50},     // scoreth13.dat 落盘序列化（'CR' 块 0x56dc×5 +
                                  // 0x1b254 尾 + 0x4723a0 加密；0x42bfa0 首指令
                                  // 调用）；C++ 对应物 sdl/FileStore::save（子集）
};

}  // namespace th13::rebuild
