// 原版 th13 录像（.rpy，magic "t13r"）解码：头部 + 双趟 XOR + ZUN LZ77 +
// stage 块链 + 输入帧流。全部布局逆向自 th13.exe（Ghidra 导出）：
//   装载 0x448c40（头校验/解密序/stage 链遍历）  保存 0x4484d0（拼包/加密序/USER 段）
//   XOR 变换 0x472280（解）/0x4723a0（加，互为逆）  LZ77 0x458f30
//   录制 0x4480a0（6 字节/帧 + 每 30 帧慢速率字节）  回放 0x448200（同节奏消费）
// 文件布局：
//   +0x00 "t13r" +0x04 u16 版本(2) +0x0C u32 USER 段偏移(=压缩尺寸+0x24)
//   +0x10 u32 0x100 +0x1C u32 压缩尺寸 +0x20 u32 解压尺寸
//   +0x24 密文（压缩尺寸字节）→ USER 段（"USER"+u32 总长+u8 类型+3 填充+文本，可多块）
// 解压后：0x74 顶层头（+0x00 名字、+0x14 分数、+0x54 慢速率 f32、+0x58 关卡数、
//   +0x5C 角色、+0x60 机体、+0x64 难度、+0x70 练习关卡 -1=本篇），+0x74 起 stage 块链。
// stage 块：0xC4 头（+0x00 u16 关卡号、+0x02 u16 戳、+0x04 u32 帧数、+0x08 u32
//   数据字节长、+0x1C u32 关卡起始分）+ 帧数据（6×帧数）+ 事件数据（(帧数+29)/30
//   字节慢速率）+ 下一块；块步进 = 0xC4 + 数据长（0x448c40:79）。
#pragma once
#include "Types.hpp"
#include <string>
#include <vector>

namespace th13 {

// 单帧输入：6 字节 = 3×u16（当前态/按下沿/释放沿，0x4480a0:41-43 写，
// 0x448200:25-34 读）。沿字段与相邻帧差分一致（0x414f30 重算幂等），注入
// 侧按当前态驱动即可。
struct ReplayFrame {
    u16 state = 0, pressed = 0, released = 0;
    // 结束标记：三字全 F（保存时追加，0x4484d0:55；回放侧判定 0x448200:26）。
    bool eof() const { return state == 0xFFFF && pressed == 0xFFFF && released == 0xFFFF; }
};

// 输入位（0x4e49f0 键位结构 +0x218 态；方向向量表 0x4abc30 实证）：
//   0x01 Z 射击  0x02 X 符卡  0x08 Shift 低速
//   0x10 上  0x20 下  0x40 左  0x80 右
struct ReplayStage {
    u16 number = 0;       // 头 +0x00
    u16 stamp = 0;        // 头 +0x02（原版未命名，录制态原样落盘）
    u32 frame_count = 0;  // 头 +0x04
    u32 blob_size = 0;    // 头 +0x08
    u32 score = 0;        // 头 +0x1C
    // 完整 0xC4 stage 头原样字节：0x449070 恢复路径复制头 +0x14 起 0x58 字节
    // 至 DAT_004be7b8（头内偏移=全局位置），回放开局必须以此为准而非默认值。
    std::vector<u8> header;
    std::vector<u8> input;   // 6×frame_count 字节
    std::vector<u8> events;  // 每 30 帧 1 字节慢速率（原版 FPS，0xff 截断）
    ReplayFrame frame(u32 index) const;
    u8 slow_rate(u32 frame) const {  // 帧号 → 当前慢速率字节（30 帧节拍）
        const u32 tick = frame / 30;
        return tick < events.size() ? events[tick] : 0;
    }
};

class ReplayFile {
public:
    // 全量解码（头 → 解密 → 解压 → stage 链 → USER 段）。
    bool decode(const u8* data, size_t size);
    const std::string& error() const { return error_; }

    // 0x24 文件头
    u16 version = 0;
    u32 user_offset = 0, comp_size = 0, uncomp_size = 0;
    // 0x74 顶层头
    std::string player_name;
    i32 score = 0, character = 0, shot_type = 0, rank = 0, practice_stage = 0;
    float slow_rate = 0.0f;
    u32 stage_count = 0;
    // 解压流 [0,0x74) 顶层头原样字节：0x448c40:67 单独 malloc 的独立块；
    // 0x447900 case 1 装备序从其 +0x5c/0x60/0x64 读出写全局（恢复源，禁止
    // 字段级伪造）。
    std::vector<u8> top_header;
    std::vector<ReplayStage> stages;
    std::vector<std::string> user_blocks;  // USER 文本块（首块=摘要，次块=备注）

    // 变换原语（互逆方向对，单测直调）。
    // 0x4723a0 加密 / 0x472280 解密：块内交织反转 + 递增 XOR（密钥跨块连续，
    // 仅变换 size-(奇对齐)-(尾块<block/4 时的余数) 字节；单方向各自非自逆）。
    static void xor_encrypt(u8* buf, size_t size, u32 init_key, u32 step, u32 block);
    static void xor_decrypt(u8* buf, size_t size, u32 init_key, u32 step, u32 block);
    // 0x458f30：MSB 位流 LZ77。字面量=1+8 位；拷贝=0+13 位环地址+4 位长
    // （长=n+3）；地址 0 终止。环 0x2000，写游标初值 1（与原版全局一致）。
    static bool inflate(const u8* src, size_t src_size, std::vector<u8>& out, size_t limit);

private:
    std::string error_;
    bool fail(const std::string& message) {
        error_ = message;
        return false;
    }
};

// 回放输入游标：逐帧产 ReplayFrame；越界/EOF 后 valid() 为假（回放侧
// 0x448200:26 EOF 即停，越界读零态 0x448200:14-19）。
class ReplayInputCursor {
public:
    explicit ReplayInputCursor(const ReplayStage& stage) : stage_(&stage) {}
    bool valid() const {
        return stage_ && index_ < stage_->frame_count && !eof_reached_;
    }
    u32 frame_index() const { return index_; }
    const ReplayStage* stage() const { return stage_; }  // 宿主侧取总帧数等头字段
    ReplayFrame next() {
        if (!valid()) return ReplayFrame{};
        const ReplayFrame result = stage_->frame(index_++);
        if (result.eof()) eof_reached_ = true;
        return result;
    }
    u8 current_slow_rate() const { return stage_ ? stage_->slow_rate(index_) : 0; }

private:
    const ReplayStage* stage_ = nullptr;
    u32 index_ = 0;
    bool eof_reached_ = false;
};

}  // namespace th13
