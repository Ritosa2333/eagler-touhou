// TH13 MP 多人 Replay 独立格式（M9）。
// 与 SP 单人 .rpy 完全独立：不冒称原版 bit-exact，不写 SP 回放槽。
// 格式（docs/MP-DEVELOPMENT.md §M9；RULES.md R-X「多人 Replay 按共用协议
// 独立保存」）：
//   [Magic "T13M" | Version | HeaderLen | Header(SessionSetup Words + 元数据)]
//   [InputReplay 线格式（共用栈——全席确认输入流 + 章节表）]
//   [StageEntry 表（每关头一份：全席资源/槽/RNG/生命周期延续）]
// Header 内 SessionSetup 保证：同构建身份（build[4]）、同 GameplayAbi、
// 同 seed/难度/角色选择——回放端与录制端的模拟前提逐位一致。
// 关头（StageEntry）承载跨关延续：Replay 的输入流按「游戏帧」单调编号，
// 关头表按章节（InputReplay chapter）对齐——seek 到章节 C = 重建 C 的
// StageEntry 态 + 从 C.firstFrame 重放输入（不伪切当前帧号）。
#pragma once

#include "SessionSetup.hpp"

#include <eagler/netplay/InputReplay.hpp>

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace th13::multiplayer {

class ReplayArchive {
public:
    static constexpr std::uint32_t Magic = 0x4d333154u;  // 'T13M' LE
    static constexpr std::uint32_t Version = 1;
    static constexpr std::uint32_t MaxFrames = Netplay::InputReplay::MaxFrames;
    static constexpr std::uint32_t MaxChapters = 64;

    // ---- 录制 ----
    // Begin 在 frame0 前调用（会话身份冻结）。chapterLabel = 关号（1 开始）。
    bool Begin(const SessionSetup& setup, std::uint64_t timestamp);
    // 只追加确认帧（ConfirmedInputs——预测帧不落盘；回滚后重放帧号
    // 单调，InputReplay 按帧号连续校验）。
    bool Append(std::uint32_t frame, std::uint32_t chapter,
                const Netplay::FrameInput* inputs, unsigned count);
    // 关头延续快照登记（跨关推进时调用；同章节重复登记拒绝）。
    bool AppendStageEntry(std::uint32_t chapter, std::vector<std::uint8_t> entry);
    // 终局编码（name/score/completed 进 header 元数据）。
    bool Encode(std::vector<std::uint8_t>& out, const char* name, std::uint32_t score,
                bool completed) const;

    // ---- 回放 ----
    // Load：整档校验（magic/version/header 合法/章节表单调）。
    bool Load(const std::uint8_t* bytes, std::size_t size);
    static bool HasSignature(const std::uint8_t* bytes, std::size_t size);

    const SessionSetup& Setup() const { return setup_; }
    const Netplay::InputReplayInfo& Info() const { return replay_.Info(); }
    std::uint32_t FrameCount() const { return replay_.Info().frameCount; }
    std::uint64_t Timestamp() const { return timestamp_; }
    std::uint32_t Score() const { return score_; }
    bool Completed() const { return completed_; }
    const std::string& Name() const { return name_; }
    bool Recording() const { return replay_.Recording(); }
    bool Loaded() const { return replay_.Loaded(); }

    // 帧输入（回放驱动面）：frame 的全席确认输入。
    const Netplay::InputReplay::Frame* FrameAt(std::uint32_t frame) const;
    // 章节表：seek 的目标锚点（chapter → firstFrame + StageEntry）。
    std::uint32_t ChapterCount() const { return info_.chapterCount; }
    const Netplay::InputReplayChapter* Chapters() const { return info_.chapters.data(); }
    // 关头延续字节（章节 label 查找；无该章 = 空——回放端按普通开局）。
    const std::vector<std::uint8_t>* StageEntryAt(std::uint32_t chapter) const;

    void Clear();

private:
    static bool DecodeHeader(const std::uint8_t* bytes, std::size_t size,
                             SessionSetup& setup, std::uint64_t& timestamp,
                             std::uint32_t& score, bool& completed, std::string& name,
                             std::size_t& headerBytes);
    std::vector<std::uint8_t> EncodeHeader(const char* name, std::uint32_t score,
                                           bool completed) const;

    Netplay::InputReplay replay_;
    SessionSetup setup_{};
    std::uint64_t timestamp_ = 0;
    std::uint32_t score_ = 0;
    bool completed_ = false;
    std::string name_;
    Netplay::InputReplayInfo info_{};  // Load 后的章节表（replay_.Info 镜像）
    // 关头延续：chapter label → 关头字节（StageEntry 编码）。
    std::vector<std::array<std::uint8_t, 2>> entryLabels_{};  // debug 面预留
    std::vector<std::uint32_t> entryChapters_{};
    std::vector<std::vector<std::uint8_t>> entryBytes_{};
};

}  // namespace th13::multiplayer
