#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

namespace Netplay
{
struct RollbackJournalConfig
{
    std::size_t maxFrames = 16;
    std::size_t maxBytesPerFrame = 8 * 1024 * 1024;
    std::size_t maxBlocksPerFrame = 4096;
    bool fastBulkCopy = false;
    bool coalesceRestore = false;
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
    std::size_t FrameCount() const { return frameCount_; }
    std::size_t BytesForFrame(std::uint32_t frame) const;
    std::size_t BlocksForFrame(std::uint32_t frame) const;
    // Adjust one rewindable float pair in checkpoints whose start is after
    // `afterFrame` and no later than `throughFrame`. Used by a game adapter
    // only after it has proven that a late input is causally equivalent for
    // already-simulated gameplay but changes deterministic carried remainder.
    bool AddFloatPairToSnapshots(std::uint32_t afterFrame, std::uint32_t throughFrame,
                                 void *address, float dx, float dy);
    std::uint64_t RestoreCopiedBytes() const { return restoreCopiedBytes_; }
    std::uint64_t RestoreSkippedBytes() const { return restoreSkippedBytes_; }
    std::uint64_t ArenaGrowths() const { return arenaGrowths_; }

private:
    static constexpr std::uint32_t NO_BLOCK = std::numeric_limits<std::uint32_t>::max();

    struct Block
    {
        std::uintptr_t address = 0;
        std::size_t offset = 0;
        std::size_t size = 0;
        std::uint32_t left = NO_BLOCK;
        std::uint32_t right = NO_BLOCK;
        int height = 1;
    };

    struct FrameRecord
    {
        std::uint32_t startFrame = 0;
        std::uint32_t endFrame = 0;
        // Uninitialized capacity: Touch overwrites every live byte. A byte
        // vector's resize would zero the entire snapshot before copying it.
        std::unique_ptr<std::uint8_t[]> bytes;
        std::size_t byteSize = 0;
        std::size_t byteCapacity = 0;
        // An address-ordered AVL index in contiguous, reusable storage keeps
        // duplicate/overlap checks logarithmic without one allocation per
        // bullet/item/effect. Indices survive vector storage relocation.
        std::vector<Block> blocks;
        std::uint32_t root = NO_BLOCK;
    };

    FrameRecord *CurrentRecord();
    FrameRecord &BackRecord();
    const FrameRecord *FindFrame(std::uint32_t frame) const;
    void EnsureByteCapacity(FrameRecord &record, std::size_t required);
    void CopyBytes(void *destination, const void *source, std::size_t size);
    static bool CoversBlock(const FrameRecord &record, const Block &block);
    static int Height(const FrameRecord &record, std::uint32_t index);
    static void UpdateHeight(FrameRecord &record, std::uint32_t index);
    static std::uint32_t RotateLeft(FrameRecord &record, std::uint32_t root);
    static std::uint32_t RotateRight(FrameRecord &record, std::uint32_t root);
    static std::uint32_t RebalanceBlock(FrameRecord &record, std::uint32_t root);

    RollbackJournalConfig config_{};
    // Fixed slot ring. Undo and confirmation release logical history, not
    // its backing allocations; resimulation must reuse the same arenas too.
    std::vector<FrameRecord> frames_;
    std::size_t firstFrame_ = 0;
    std::size_t frameCount_ = 0;
    bool configured_ = false;
    bool frameOpen_ = false;
    bool failed_ = false;
    std::uint32_t openFrame_ = 0;
    std::uint64_t restoreCopiedBytes_ = 0;
    std::uint64_t restoreSkippedBytes_ = 0;
    std::uint64_t arenaGrowths_ = 0;
};
} // namespace Netplay

