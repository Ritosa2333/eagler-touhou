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
    static constexpr std::uint32_t Magic = 0x4d333154u;
    static constexpr std::uint32_t Version = 4, LegacyVersion = 1;
    static constexpr std::uint32_t MaxFrames = Netplay::InputReplay::MaxFrames;
    static constexpr std::uint32_t MaxChapters = 64;

    bool Begin(const SessionSetup& setup, std::uint64_t timestamp);
    bool Append(std::uint32_t frame, std::uint32_t chapter,
                const Netplay::FrameInput* inputs, unsigned count);
    bool AppendStageEntry(std::uint32_t chapter, std::vector<std::uint8_t> entry);
    bool Encode(std::vector<std::uint8_t>& out, const char* name, std::uint32_t score,
                bool completed) const;

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
    bool UsesLegacyStagePositions() const { return version_ == LegacyVersion; }
    bool UsesLegacyBossMitigation() const { return version_ <= 2; }
    bool UsesLegacyPickupRewards() const { return version_ <= 3; }

    const Netplay::InputReplay::Frame* FrameAt(std::uint32_t frame) const;
    std::uint32_t ChapterCount() const { return info_.chapterCount; }
    const Netplay::InputReplayChapter* Chapters() const { return info_.chapters.data(); }
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
    std::uint32_t version_ = Version;
    std::uint64_t timestamp_ = 0;
    std::uint32_t score_ = 0;
    bool completed_ = false;
    std::string name_;
    Netplay::InputReplayInfo info_{};
    std::vector<std::array<std::uint8_t, 2>> entryLabels_{};
    std::vector<std::uint32_t> entryChapters_{};
    std::vector<std::vector<std::uint8_t>> entryBytes_{};
};

}
