// th13 MSG13 对话系统移植（S3 第一阶段：Stage 终了→对话→奖励→换关链路）。
//
// 二进制格式（从 0x428ae0 家族反汇编 + 实际文件字节解出，dump 证据留各行注释）：
//   文件头：u32 entry_count；随后 entry_count 个 8 字节表项
//     { u32 data_offset（文件内绝对偏移）; u32 id（th13 全部 0x100） }
//     ——运行时定位 = 原始文件镜像 + 表内偏移：0x428820:128
//       `*(u32*)(T + 4 + entry*8) + T`，T = 0x424fd0:51 存入 stagectx+0x19c
//       的整文件装载指针；字节核对 st01a{2 项: 0x14/0x334}、st07a{3 项:
//       0x1c/0x59c/0xd40}、e01/staff{1 项: 0x0c}，指令流起点 = 4+count*8。
//   指令：{ u16 time; u8 opcode; u8 arg_bytes; u8 args[arg_bytes] }
//     ——opcode = 指针+2 的字节（0x428ae0:151）；推进 = +4+arg_bytes
//       （0x428ae0:759）；时间门 = 对话钟 >= time 才执行（0x428ae0:143/761）。
//   entry 终止 = opcode 0 指令（case 0x0 返回 -1），其后 4 零字节填充
//     （staff.msg 全 12 条指令与 thmsg dump 逐条核对全中）。
//   文本参数（opcode 17）加密：FUN_0042a340 反编译直读——XOR 流
//     key=0x77，每字节后 key+=inc、inc+=0x10（u8 环绕），解到 NUL 止；
//     st01a entry0 全 14 句解出通顺 cp932 日文实证
//     （「さーて、幽霊の親玉を倒さないとね！」= 0xF5 0xCD… 解出 0x82 0xB3…）。
//
// 脚本钟（对话 +0x1c）仅在解释器遇到未来指令的时间门时 +1；opcode 11
// 阻塞不推进。调用者 0x4265a0 另增 +8 的对话年龄，不能当作脚本钟。
// 终止指令当帧销毁对话（stagectx+0x198=0），ECL 519 下一帧放行。
//
// opcode 语义（dump 十进制 = 案值；st0N[a-d] 全 28 文件用例普查 + 0x429c7c
// jump table 逐 handler 反汇编核验——旧「2/19/20/31 BGM 族」「13/14 隐立绘」
// 注释为误读，p58-dialogue 已纠正）：
//   0 终止 / 1 自机立绘根（0x4c0aa0[角色]）/ 2 右立绘（中Boss/Boss 对按
//   entry 分流）/ 4/5/6 槽位与名牌释放 / 7 左侧说话人（左2 亮/右3 暗）/
//   8 右侧（左3/右2）/ 9 双立绘变体 / 10 自动推进位翻转
//   11 等输入（Z/Enter 按下、arg 倒计时、auto+快进位+键持续 ≥0x14 三路放行）/
//   12 设置 +0x174=1，继续执行 / 13/14 左/右立绘表情切换（arg+17 立即中断）/
//   17 文本行（解密后支持 `|a,b,` 读音注记前缀）/ 18 文本区+名牌槽释放
//   19 Boss 曲播放（0x43ae40(1,·) 预载槽 1）/ 20 名牌 VM（stage 表 +0x54..）
//   21 ★关终了触发（case 0x15 → 0x42cd40，0x428ae0:673-675）
//   22/27 Boss 战设置（0x43af70 音乐淡出：常数 2.0s/关6 8.0s/27 取 arg）
//   23/24 释放槽位 / 25 名牌缩放 / 26 显示位 / 28 对话框 x,y / 29 文字属性位
//   30 原版 switch 无此案（空转）/ 31 右立绘（Boss 对无条件）
#pragma once

#include "Types.hpp"

#include <string>
#include <vector>

namespace th13 {

struct MsgEntry {
    u32 offset = 0;  // 指令流文件内绝对偏移
    u32 id = 0;      // 表项第二 u32（th13 恒 0x100，语义待 oracle 核）
};

// 整文件镜像 + entry 表（不做指令级预解析：执行器按 4+arg_bytes 游走，
// 与原版 0x428ae0 同构）。
class MsgFile {
public:
    // 校验头与表项并保留字节镜像；失败时填 error 并返回 false。
    bool open(const u8* data, u32 size, std::string& error);
    u32 entry_count() const { return u32(entries_.size()); }
    const MsgEntry& entry(u32 index) const { return entries_[index]; }
    const u8* bytes() const { return bytes_.data(); }
    u32 size() const { return u32(bytes_.size()); }

private:
    std::vector<u8> bytes_;
    std::vector<MsgEntry> entries_;
};

// 0x42a340 文本解密：解到 NUL（含），返回解密后字节串。
std::string msg_decrypt_text(const u8* args, u32 arg_bytes);
// cp932 → UTF-8（表见 MsgPlayerCp932.hpp；半角片假名/ASCII 算法处理）。
// 非法序列按 U+FFFD 替换（对话文本为标准 cp932，预期不触发）。
std::string cp932_to_utf8(const u8* data, u32 size);
// 字体栅格 cp932→Unicode 映射（th15 prepare-fonts.mjs 同语义）：65536 项
// u16 小端，恰 131072 字节。n<0x100 = 单字节（ASCII 直通、0xA1..0xDF 半角
// 片假名 U+FF61..、其余 0x30FB）；n>=0x100 = (lead<<8|trail) 双字节查
// MsgPlayerCp932 表，无效组合 0x30FB。供 sdl/FontDevice 经
// eagler::text::RuntimeFontRaster::initialize(mapping, 131072) 自举——
// 映射在仓内推导，不再要求 /fonts/cp932.bin 启动资产。
bool build_cp932_font_mapping(std::vector<u8>& out);

// 单行对话文本与元数据（原版 0x428ae0 case 0x11 实证：保留 CP932 原文字节与 ruby 注音）
struct MsgLine {
    std::string text_cp932;       // 原始 CP932 文本
    std::string text_utf8;        // 派生 UTF-8 文本（用于展示/观察）
    bool has_ruby = false;        // 是否有 ruby 读音注记（0x428ae0:455-467）
    i32 ruby_x = 0;               // ruby 第一数值参数：x 偏移（0x428ae0:464/529 →
                                  // 0x46e8f0 param_4 → 0x45aef0 param_4，scratch x*2）
    i32 ruby_spacing = 0;         // ruby 第二数值参数：逐字步进（0x46e8f0 param_5 →
                                  // 0x45aef0 param_9；0 = 连续排版，非 0 = 每字
                                  // TextOut 后前进 param_9*2 scratch 像素）
    std::string ruby_cp932;       // ruby CP932 读音注记
    std::string ruby_utf8;        // 派生 UTF-8 读音注记
    float bubble_width = 0.0f;    // 0x428ae0 原版气泡排版宽度 ((len-1)*8)&~0xf - 28.0
    u32 block = 0;                // 行块序号（0x428ae0 +0x5f/+0x60 配对周期）
    i32 side = 0;                 // +0x184：0=左（文字 x=box_x）、1=右
                                  // （文字 x=box_x-(bubble_width+30)，0x428ae0:499）
    u32 text_attr = 0;            // 0x428ae0 +0x178 文字属性位（opcode 29）；
                                  // 正文字体样式 = ((text_attr>>1)&1)+4
};

// 对话可视状态（渲染侧消费；字段语义对应 0x1a8 对话对象）。
struct MsgDialogState {
    bool active = false;
    bool done = false;                  // entry 终止指令已命中
    i32 entry = 0;                      // 对话对象 +0（0x428820 存 entry 号）
    i32 side = 0;                       // +0x184：0=左 1=右（opcode 7/8）
    float box_x = 192.0f, box_y = 320.0f;  // +0x194/+0x198（0x4282b0 尾默认）
    bool auto_advance = false;          // +0x178 bit0（opcode 10）
    u32 text_attr = 0;                  // +0x178：bit0 自动、bit1 状态、bit2-5 opcode29
    bool skip_request = false;          // +0x178 bit 0x40（Ctrl/Z 按下置位，0x428ae0:125-127）
    float bubble_width = 0.0f;          // 当前对话框气泡宽度（0x428ae0: unaff_EDI[0x68]）
    u32 block = 0;                      // 行块序号：首行（+0x5f/+0x60 双零）起新块
    // 当前行块（≤2 行/块）。opcode 11 只清 +0x5f/+0x60，不擦文字；下一块首行
    // 才替换本块内容（0x428ae0:378-379/426-452）。
    std::vector<std::string> lines;
    // 结构化行数据（保留原始 CP932、ruby 及度量）。
    std::vector<MsgLine> lines_meta;
};

// 对话推进输入（0x428ae0:122-129 + 0x4298fc-0x429988 的键位面；port 位值 =
// GameKeyboard 表：0x1 Z、0x200 Ctrl、0x80000 Enter）。
struct MsgAdvance {
    bool z_pressed = false;  // Z 边沿（既有路径）
    u32 held = 0;            // 本帧持续位（0x4e4c08）
    u32 z_frames = 0;        // Z 持续帧数（0x42b3f0(0) 对应）
    u32 ctrl_frames = 0;     // Ctrl 持续帧数（0x42b3f0(9) 对应）
    u32 pressed = 0;         // 本帧按下边沿（0x4e4c14）
};

class MsgHost {
public:
    virtual ~MsgHost() = default;
    virtual void msg_bgm(i32 opcode) = 0;        // 2/19/20/31（案值透传；宿主按
                                                 // 0x429c7c jump table 分派真实语义）
    virtual void msg_portrait_load() = 0;        // 1（DAT_004c0aa0+角色*4）
    // 22/27 only call 0x43af70 (music fade); there is no separate battle setup.
    virtual void msg_boss_battle() {}             // legacy compatibility hook
    virtual void msg_stage_clear() = 0;          // 21 → 0x42cd40 ★
    // ——可视化通知钩子（本阶段追加；默认空实现，既有宿主不受影响）——
    // 13/14：表情切换（label = arg+0x11，0x46fca0 立即中断；13=左 14=右）。
    virtual void msg_portrait_interrupt(i32 opcode, i32 label) { (void)opcode; (void)label; }
    // 7/8：说话方切换（宿主按 0x429641/0x4296ee 做左2/右3、左3/右2 中断）。
    virtual void msg_speaker_switch(i32 opcode) { (void)opcode; }
    // 4/5/6/18/23/24：槽位释放（0x46fc30；宿主映射到立绘/名牌/文本槽）。
    virtual void msg_release(i32 opcode) { (void)opcode; }
    // 22/27 的音乐淡出半边（0x43af70 → 曲命令 5；常数 2.0s/关6 8.0s 宿主定）。
    virtual void msg_music_fade(i32 opcode) { (void)opcode; }
    virtual void msg_music_fade(i32 opcode, float seconds) { (void)seconds; msg_music_fade(opcode); }
    // opcode 25: signed i32 copied as float to the four text VM +0x4e4 fields.
    virtual void msg_text_anchor_y(float value) { (void)value; }
    // opcode 11 计声放行（Z/Enter 按下或超时路径的 0x460bc0(0) 提示音）。
    virtual void msg_advance_blip() {}
    // entry 终止指令命中（0x4265a0 当帧销毁）——宿主释放全部对话 VM。
    virtual void msg_dialog_end() {}
};

// 单 entry 播放器（0x428ae0 执行器与脚本钟）。
// opcode 11 放行语义（r4 定稿，msg-interrupt.asm 4298fc-429969 逐指令核）：
//   a) Z/Enter 按下位（0x4e4c14 & 0x80001）或 arg 倒计时耗尽 → 计声放行
//     （0x460bc0(0) 提示音；Z 边沿与 a) 首帧重合）；
//   b) 自动位 bit0 + 快进请求位 bit0x40（0x428ae0:125-127：Ctrl 0x200/Z
//     按下置位）+ Ctrl(0x42b3f0(9)) 或 Z(0x42b3f0(0)) 持续 ≥0x14 帧 →
//     静默放行（429946-429969 双计数器任一达 20）；
//   无「自动模式被动 20 帧放行」路径（旧 port 臆测已删：原版 429938/429940
//   对 bit0/bit0x40 缺一即跳回阻塞，不计时放行）。
class MsgPlayer {
public:
    // 打开 entry（0x428820 param>=0 路径：建 0x1a8 对话对象 + 执行器指针）。
    bool open(MsgFile& file, u32 entry_index, MsgHost& host, std::string& error);
    // 每帧执行到时间门、等输入或终止。
    // 返回值 = 是否仍在播放（终止指令命中的当帧起为 false）。
    bool step(const MsgAdvance& input);
    // 兼容面：仅 Z 边沿的旧调用（无持续位 → 不触发快进语义）。
    bool step(bool z_pressed);
    bool active() const { return state_.active; }
    bool done() const { return state_.done; }
    u32 clock() const { return clock_; }
    // 00418090 reads dialogue +0x174; ECL 519 accepts this pulse while active.
    bool ecl_wait_released() const { return !active() || command_delay_ != 0; }
    const MsgDialogState& state() const { return state_; }

private:
    enum class Flow { Advance, Blocked, Finished };
    Flow execute(u8 opcode, const u8* args, u32 arg_bytes, const MsgAdvance& input);
    void finish();  // 终止指令路径（0x4265a0 的当帧销毁）

    MsgFile* file_ = nullptr;
    MsgHost* host_ = nullptr;
    const u8* cursor_ = nullptr;  // 指令流游标（entry 内）
    MsgDialogState state_;
    u32 clock_ = 0;
    i32 line_index_ = 0;    // 对话对象 +0x17c（0x5f）
    i32 block_open_ = 0;    // +0x180（0x60）
    i32 wait_timeout_ = 0;  // opcode 11 arg 倒计时（0x4016c0 置 value-1 起步）
    i32 command_delay_ = 0;  // opcode 12（+0x174 置 1；下次解释器入口递减）
    // 0x428ae0 ruby 伴生前缀缓存（在同行正文之前到达）
    bool pending_ruby_ = false;
    i32 pending_ruby_x_ = 0;
    i32 pending_ruby_spacing_ = 0;
    std::string pending_ruby_cp932_;
    std::string pending_ruby_utf8_;
};

}  // namespace th13
