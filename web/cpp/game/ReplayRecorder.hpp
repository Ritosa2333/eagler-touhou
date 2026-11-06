#pragma once
#include "GameInput.hpp"
#include "ReplayFile.hpp"
#include "SessionState.hpp"
#include <ctime>

namespace th13 {

// 0x4484d0 保存路径的注入缝：与原版"保存确认时刻"写入顶层/流的可变字段一一
// 对应。默认值 = 0x4549d0 保存对话框路径语义（确认时 __time64、+0x68=8、
// 无 EOF 追加），供端到端测试注入原值以逐字节复刻真机文件。
struct ReplaySaveMeta {
    // 顶层 +0x0C u64 time_t：0x4549d0:84 在名字确认时 __time64 写入（0x43fa90:196 /
    // 0x43e780:360 同）。0 = encode 时刻取真实时钟（默认语义 = 原版"保存时写"）。
    std::time_t date = 0;
    // fps 管理器计数器（对象 0x4c218c；+0x24 分子 / +0x2c 分母，0x424990:37-50
    // 逐秒窗聚合：窗内 fps≤57 计实测值、否则计满 60，每窗分母 +60）。
    // 0x4484d0:0x4486e5..0x448715 保存时按
    //   slow = (float)(100.0 - (double)(float)(num/den) * 100.0)
    // 写顶层 +0x54 f32。denominator>0 时按此公式；否则 encode 用事件字节流
    // （每 30 帧一窗 fps 采样，0x4480a0:25-37）按同一分支结构聚合。
    double slow_numerator = 0.0;
    double slow_denominator = 0.0;
    // 顶层 +0x68（i32）：保存确认流程写入——对话框路径恒 8（0x4549d0:86）、
    // 中断保存路径写当前关卡 DAT_004be81c（0x43fa90:197 / 0x43e780:361）。
    // 0x4484d0:0x448a35 以其 >7 与否选择 "Stage All Clear"/区间行。
    i32 stage_field = 8;
    // 0x4484d0 第三参 != 0 时末尾 stage 追加 6×0xFF EOF（0x44854c..0x448598，
    // 无事件字节）。仅 0x43e780:508（中断保存）传 1；保存对话框 0x4549d0:201
    // 与 game over 流 0x43fa90:347 / 0x440960:348 均传 0（完整录像无 EOF）。
    bool append_eof = false;
    // USER "Version %s\r\n" 参数（0x4484d0:0x44899a 推 .rdata 0x4abde8 字面量；
    // 本移植目标 1.00c。1.00a 版本存档的文件为 "1.00a"，注入用于复刻）。
    const char* version = "1.00c";
};

// Collects the original six-byte input records and emits a t13r container.
// The caller supplies the exact 0x74 and 0xc4 start snapshots.
class ReplayRecorder {
public:
    bool start(const std::vector<u8>& top);
    bool begin_stage(const std::vector<u8>& header);
    bool record(const GameInput& input, u8 slow_rate);
    bool record_spell_time(u32 index, i32 packed);
    // 兼容旧签名 = 默认 ReplaySaveMeta（真实时钟、事件流聚合慢速率、
    // stage_field=8、无 EOF、"1.00c"）。
    bool encode(std::vector<u8>& out, const std::string& name, i32 score) const;
    // 0x4484d0 保存路径逐字节同构（LZ77 = ReplayFile::deflate，USER 段为
    // 完整 Shift-JIS 摘要两块：flag=0 摘要 + flag=1 コメント占位）。
    bool encode(std::vector<u8>& out, const std::string& name, i32 score,
                const ReplaySaveMeta& meta) const;
    static std::vector<u8> top_snapshot(const SessionState& progress);
    static std::vector<u8> stage_snapshot(const SessionState& progress,
        const HudPlayerState& player, const HudScoreState& score, u16 seed,
        i32 x128, i32 y128, bool focused);
    bool ready() const { return top_.size() == 0x74 && !stages_.empty(); }
    void clear() { top_.clear(); stages_.clear(); }
private:
    std::vector<u8> top_;
    std::vector<ReplayStage> stages_;
};
}
