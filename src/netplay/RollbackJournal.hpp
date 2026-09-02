#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <map>
#include <vector>

namespace Netplay
{
struct RollbackJournalConfig
{
    std::size_t maxFrames = 16;
    std::size_t maxBytesPerFrame = 8 * 1024 * 1024;
    std::size_t maxBlocksPerFrame = 4096;
};

// Bounded first-write-wins undo storage for logical simulation checkpoints.
// Touch() must be called before a region is mutated. Exact duplicate touches
// are ignored; partial overlaps are rejected so restore order never becomes an
// implicit ownership rule. A checkpoint may span multiple consecutive logical
// frames; extending it keeps the same first-write set alive across those frames.
class RollbackJournal
{
public:
    bool Reset(const RollbackJournalConfig &config);
    void Clear();

    bool BeginFrame(std::uint32_t frame, bool extendPrevious = false);
    bool Touch(void *address, std::size_t size);
    bool EndFrame();

    // Restore the state immediately before the checkpoint containing `frame`
    // began, then discard that checkpoint and every newer checkpoint. When
    // supplied, restoredFrame receives the first logical frame that must be
    // resimulated.
    bool UndoTo(std::uint32_t frame, std::uint32_t *restoredFrame = nullptr);
    void DiscardBefore(std::uint32_t frame);

    bool IsFrameOpen() const { return frameOpen_; }
    bool Failed() const { return failed_; }
    std::uint32_t OpenFrame() const { return openFrame_; }
    std::size_t FrameCount() const { return frames_.size(); }
    std::size_t BytesForFrame(std::uint32_t frame) const;
    std::size_t BlocksForFrame(std::uint32_t frame) const;

private:
    struct Block
    {
        std::uintptr_t address = 0;
        std::size_t offset = 0;
        std::size_t size = 0;
    };

    struct FrameRecord
    {
        std::uint32_t startFrame = 0;
        std::uint32_t endFrame = 0;
        std::vector<std::uint8_t> bytes;
        // Ordered by address so duplicate/overlap checks inspect only the
        // immediate predecessor and successor. Dense bullet/item/effect
        // frames otherwise turn the first-write journal into quadratic work.
        std::map<std::uintptr_t, Block> blocks;
    };

    FrameRecord *CurrentRecord();
    const FrameRecord *FindFrame(std::uint32_t frame) const;
    static bool RangesOverlap(std::uintptr_t a, std::size_t aSize,
                              std::uintptr_t b, std::size_t bSize);

    RollbackJournalConfig config_{};
    std::deque<FrameRecord> frames_;
    bool configured_ = false;
    bool frameOpen_ = false;
    bool failed_ = false;
    std::uint32_t openFrame_ = 0;
};
} // namespace Netplay
