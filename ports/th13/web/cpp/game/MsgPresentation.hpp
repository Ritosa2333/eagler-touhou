// 对话立绘表现（原版 MSG 可视指令的 ANM VM 面）。
//
// 原版证据（0x428ae0 执行器 jump table 0x429c7c + 00424fd0/0042b990）：
//   opcode 1（0x429285）：自机立绘根 VM——玩家 ANM 管理器（0x4c22c4）+
//     脚本表 0x4c0aa0[be7b8]（EXE .data 实读 = 29,34,30,28）；位置清零、
//     flags |= 0x200，句柄存对话对象 +0x40。
//   opcode 2（0x429335）：右立绘 VM——敌机 ANM 管理器（0x4c2188）bank
//     [stage+0x6c] 脚本 [stage+0x70]（entry 号 <2 或 +0x64==-1 时），否则
//     bank [stage+0x64] 脚本 [stage+0x68]；句柄 +0x44。Stage1 = st01enm
//     script 10（p58 实证）。
//   opcode 20（0x429a94）：名牌 VM——be824<5 且 [stage+0x54]!=-1 取
//     [stage+0x54]/[stage+0x58]，否则 [stage+0x5c]/[stage+0x60]；句柄 +0x5c；
//     随后 0x42b1f0 在管理器 0x4c2190 建 Boss 背景 VM（该管理器与脚本身位
//     未在 port 落位，本轮不建，见 HANDOFF）。
//   opcode 31（0x42948c）：右立绘（boss 对，无条件），句柄 +0x44。
//   opcode 7/8（0x429641/0x4296ee）：说话方切换 = 0x46fca0 立即中断——
//     7：右 label3（暗）、左 label2（亮）；8：左 label3、右 label2。
//   opcode 13/14（0x4298d4/0x4298e8）：表情切换 = 0x46fca0 立即中断，
//     label = *(i32*)(指令 arg) + 17；13 打左（+0x40）、14 打右（+0x44）。
//     （旧 MsgPlayer 注释「13/14 隐立绘」为误读，p58 已纠正。）
//   opcode 4/5/23/24（0x4295cf/0x4295e9/0x4295a9/0x4295bc）：0x46fc30 释放
//     左/右/名牌槽（mode 1/7 退出风格差异未校准，port 统一走 retire）。
//   opcode 6/18（0x42960c/0x429250）：释放名牌四槽 + 正文/ruby 文本区。
//
// bank 号 → 合并表全局脚本号：原版敌机管理器 bank 数组 +0x80+idx*4 与 ECL
// 文件槽同序（ECL 302 文件槽 = 2 + ANIM 名单序）；port 把名单序 ANM 追加成
// 单一 bank（AnmResource::append 全局序号语义），故全局脚本号 = 名单序在
// 目标文件之前各文件脚本数之和 + 文件内脚本号。
#pragma once

#include "AnmManager.hpp"

#include <functional>
#include <string>
#include <vector>

namespace th13 {

class MsgPresentation {
public:
    struct Banks {
        i32 player = 0, stage = 0;  // ApplicationState bank 槽（kPlayerBank/kStageBank）
    };
    // 0x4c0aa0 自机立绘根脚本表（EXE .data 实读）。
    static constexpr i32 kPlayerPortraitScripts[4] = {29, 34, 30, 28};
    // 原版 stage 表 0x4bb4f0 行（0x7c 步进；本任务 EXE 实读 dump，games/th13
    // 只读核对）：名牌对 A=+0x54/+0x58、B=+0x5c/+0x60；Boss 对 = +0x64/+0x68、
    // 中Boss 对 = +0x6c/+0x70（bank 号 = ECL 文件槽，脚本号 = 文件内序）。
    struct PortraitRow {
        i32 nameplate_a_bank, nameplate_a_script;
        i32 nameplate_b_bank, nameplate_b_script;
        i32 boss_bank, boss_script;
        i32 midboss_bank, midboss_script;
    };
    static const PortraitRow kStageRows[7];

    using ReadFn = std::function<bool(const std::string&, std::vector<u8>&)>;

    MsgPresentation(AnmManager& animations, Banks banks) : animations(animations), banks(banks) {}
    std::string error;
    // 舞台行（1..7）+ ANM 名单（ECL ANIM 序）+ 读通道：算各文件的全局脚本
    // 基号。缺文件/解析失败置 error 并返回 false（显式，不静默降级）。
    bool bind_stage_scripts(i32 stage, const std::vector<std::string>& anim_list,
                            const ReadFn& read);
    bool player_portrait(i32 character);  // opcode 1
    // opcode 2：entry 号决定 Boss/中Boss 对（entry<2 或无 boss 对 = 中Boss）。
    bool side_portrait(i32 entry);
    bool boss_side_portrait();  // opcode 31
    bool nameplate();           // opcode 20
    bool interrupt_player(i32 label);  // opcode 13 / opcode 7 的左亮
    bool interrupt_side(i32 label);    // opcode 14 / opcode 8 的右亮
    void release_player();      // opcode 4/23
    void release_side();        // opcode 5/24（含名牌槽 +0x5c）
    void release_nameplates();  // opcode 6/18 的名牌四槽语义（port 单名牌槽）
    void release_all();

private:
    bool create(bool player_bank, i32 manager_bank, i32 script, u32& handle);
    i32 stage_script(i32 manager_bank, i32 script);  // → 合并表全局号；失败置 error 返回 -1
    bool fail(const std::string& why) {
        if (error.empty()) error = why;
        return false;
    }
    AnmManager& animations;
    Banks banks;
    i32 stage_ = 0;
    std::vector<i32> stage_script_base_;  // ANIM 名单序 i 的全局脚本基号
    u32 player_ = 0, side_ = 0, nameplate_ = 0;
};

}  // namespace th13
