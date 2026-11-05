#pragma once

#include <eagler/netplay/NetplayProtocol.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace Netplay
{
struct InputReplayConfig
{
    std::uint32_t gameId = 0, gameplayAbi = 0;
    std::uint8_t playerCount = 0, recordedPlayer = 0;
    // Title-owned versioned bytes, never a native object or pointer dump.
    std::vector<std::uint8_t> description;
};
struct InputReplayChapter
{
    std::uint32_t label = 0, firstFrame = 0;
};
struct InputReplayInfo
{
    InputReplayConfig config;
    std::uint32_t frameCount = 0, chapterCount = 0;
    std::array<InputReplayChapter, 64> chapters{};
};

// Self-contained all-seat input recording. The title appends only reconciled,
// confirmed frames and owns native boot, transitions and playback policy.
// This is an extended multiplayer Replay, not a retail single-player Replay.
class InputReplay
{
public:
    using Frame = std::array<FrameInput, MAX_PLAYERS>;
    static constexpr std::uint32_t MaxFrames = 250'000;
    static constexpr std::size_t MaxBytes = 16 * 1024 * 1024;
    // Title-owned metadata may include a small set of stage checkpoints. Keep
    // this bounded and tiny relative to the 16 MiB Replay ceiling, while
    // leaving enough room for three-seat TH10 ReplayStage snapshots.
    static constexpr std::size_t MaxDescriptionBytes = 16 * 1024;
    static constexpr std::size_t HeaderBytes = 40, SampleBytes = 12;

    InputReplay() = default;
    InputReplay(InputReplay&&) noexcept = default;
    InputReplay& operator=(InputReplay&&) noexcept = default;
    InputReplay(const InputReplay&) = delete;
    InputReplay& operator=(const InputReplay&) = delete;

    void Clear();
    // Reserve the recording owner before frame zero, never on packet arrival.
    bool Begin(const InputReplayConfig& config);
    bool Append(std::uint32_t frame, std::uint32_t chapter,
                const FrameInput* inputs, std::size_t count);
    const Frame* FrameAt(std::uint32_t frame) const;
    const InputReplayInfo& Info() const { return info_; }
    bool Recording() const { return recording_; }
    bool Loaded() const { return loaded_; }

    // Invalid data leaves the destination unchanged. Inspect validates every
    // sample without allocating an entire frame owner per native menu preview.
    static bool HasSignature(const std::uint8_t* bytes, std::size_t size);
    static bool Inspect(const std::uint8_t* bytes, std::size_t size, InputReplayInfo* out);
    bool Decode(const std::uint8_t* bytes, std::size_t size);
    bool Encode(std::vector<std::uint8_t>* out,
                const std::vector<std::uint8_t>* descriptionOverride = nullptr,
                std::uint32_t frameCount = INVALID_FRAME) const;

private:
    InputReplayInfo info_;
    std::unique_ptr<Frame[]> frames_;
    bool recording_ = false, loaded_ = false;
};
} // namespace Netplay
