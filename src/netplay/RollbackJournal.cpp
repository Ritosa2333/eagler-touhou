#include <eagler/netplay/RollbackJournal.hpp>

#include <algorithm>
#include <cstring>
#include <limits>

#if defined(__EMSCRIPTEN__) && !defined(__wasm64__)
#include <emscripten/emscripten.h>
// Keep the existing Safari 14.1 target. Its compatibility lowering rewrites
// WASM memory.copy; TypedArray.copyWithin still uses the engine's native bulk
// copy on those browsers. Resolve HEAPU8 on EVERY call after memory growth.
EM_JS(void, eagler_journal_copy_bytes, (void *destination, const void *source, std::size_t size), {
    const to = destination >>> 0, from = source >>> 0, length = size >>> 0;
    if (length > HEAPU8.length || from > HEAPU8.length - length ||
        to > HEAPU8.length - length || (length && (!from || !to)))
        throw new RangeError('rollback copy outside WASM memory');
    HEAPU8.copyWithin(to, from, from + length);
});
#endif

namespace Netplay
{
void RollbackJournal::CopyBytes(void *destination, const void *source, std::size_t size)
{
#if defined(__EMSCRIPTEN__) && !defined(__wasm64__)
    if (config_.fastBulkCopy && size >= 1024)
    {
        eagler_journal_copy_bytes(destination, source, size);
        return;
    }
#endif
    std::memcpy(destination, source, size);
}

bool RollbackJournal::Reset(const RollbackJournalConfig &config)
{
    Clear();
    restoreCopiedBytes_ = restoreSkippedBytes_ = 0;
    arenaGrowths_ = 0;
    if (config.maxFrames == 0 || config.maxBytesPerFrame == 0 ||
        config.maxBlocksPerFrame == 0 || config.maxBlocksPerFrame >= NO_BLOCK)
    {
        return false;
    }
    config_ = config;
    frames_.resize(config_.maxFrames);
    configured_ = true;
    return true;
}

void RollbackJournal::Clear()
{
    frames_.clear();
    firstFrame_ = 0;
    frameCount_ = 0;
    configured_ = false;
    frameOpen_ = false;
    failed_ = false;
    openFrame_ = 0;
}

bool RollbackJournal::BeginFrame(std::uint32_t frame, bool extendPrevious)
{
    if (!configured_ || frameOpen_ || failed_)
    {
        return false;
    }
    if (extendPrevious)
    {
        if (frameCount_ == 0 || BackRecord().endFrame == std::numeric_limits<std::uint32_t>::max() ||
            frame != BackRecord().endFrame + 1u)
        {
            failed_ = true;
            return false;
        }
        BackRecord().endFrame = frame;
        frameOpen_ = true;
        openFrame_ = frame;
        return true;
    }
    if (frameCount_ != 0 && frame <= BackRecord().endFrame)
    {
        failed_ = true;
        return false;
    }
    if (frameCount_ == frames_.size())
    {
        firstFrame_ = (firstFrame_ + 1) % frames_.size();
    }
    else
    {
        ++frameCount_;
    }
    FrameRecord &record = BackRecord();
    record.startFrame = frame;
    record.endFrame = frame;
    record.byteSize = 0;
    record.blocks.clear();
    record.root = NO_BLOCK;
    if (record.blocks.capacity() == 0)
        record.blocks.reserve(config_.maxBlocksPerFrame);
    frameOpen_ = true;
    openFrame_ = frame;
    return true;
}

RollbackJournal::FrameRecord *RollbackJournal::CurrentRecord()
{
    return frameOpen_ && frameCount_ != 0 ? &BackRecord() : nullptr;
}

RollbackJournal::FrameRecord &RollbackJournal::BackRecord()
{
    return frames_[(firstFrame_ + frameCount_ - 1) % frames_.size()];
}

void RollbackJournal::EnsureByteCapacity(FrameRecord &record, std::size_t required)
{
    if (required <= record.byteCapacity)
        return;
    std::size_t capacity = record.byteCapacity != 0 ? record.byteCapacity
        : std::min<std::size_t>(config_.maxBytesPerFrame, 256 * 1024);
    while (capacity < required)
        capacity = capacity <= config_.maxBytesPerFrame / 2
            ? capacity * 2 : config_.maxBytesPerFrame;
    std::unique_ptr<std::uint8_t[]> bytes(new std::uint8_t[capacity]);
    ++arenaGrowths_;
    if (record.byteSize != 0)
        CopyBytes(bytes.get(), record.bytes.get(), record.byteSize);
    record.bytes = std::move(bytes);
    record.byteCapacity = capacity;
}

int RollbackJournal::Height(const FrameRecord &record, std::uint32_t index)
{
    return index == NO_BLOCK ? 0 : record.blocks[index].height;
}

void RollbackJournal::UpdateHeight(FrameRecord &record, std::uint32_t index)
{
    Block &block = record.blocks[index];
    block.height = 1 + std::max(Height(record, block.left), Height(record, block.right));
}

std::uint32_t RollbackJournal::RotateLeft(FrameRecord &record, std::uint32_t root)
{
    const std::uint32_t pivot = record.blocks[root].right;
    record.blocks[root].right = record.blocks[pivot].left;
    record.blocks[pivot].left = root;
    UpdateHeight(record, root);
    UpdateHeight(record, pivot);
    return pivot;
}

std::uint32_t RollbackJournal::RotateRight(FrameRecord &record, std::uint32_t root)
{
    const std::uint32_t pivot = record.blocks[root].left;
    record.blocks[root].left = record.blocks[pivot].right;
    record.blocks[pivot].right = root;
    UpdateHeight(record, root);
    UpdateHeight(record, pivot);
    return pivot;
}

std::uint32_t RollbackJournal::RebalanceBlock(FrameRecord &record, std::uint32_t root)
{
    Block &block = record.blocks[root];
    UpdateHeight(record, root);
    const int balance = Height(record, block.left) - Height(record, block.right);
    if (balance > 1)
    {
        const Block &left = record.blocks[block.left];
        if (Height(record, left.left) < Height(record, left.right))
            block.left = RotateLeft(record, block.left);
        return RotateRight(record, root);
    }
    if (balance < -1)
    {
        const Block &right = record.blocks[block.right];
        if (Height(record, right.right) < Height(record, right.left))
            block.right = RotateRight(record, block.right);
        return RotateLeft(record, root);
    }
    return root;
}

bool RollbackJournal::Touch(void *address, std::size_t size)
{
    FrameRecord *record = CurrentRecord();
    if (!record || failed_ || address == nullptr || size == 0)
    {
        return false;
    }

    const std::uintptr_t start = reinterpret_cast<std::uintptr_t>(address);
    if (size > std::numeric_limits<std::uintptr_t>::max() - start)
    {
        failed_ = true;
        return false;
    }
    // Retain the search path so insertion does not search the tree again.
    // An AVL tree with uint32_t indices cannot reach 64 ancestors.
    std::uint32_t ancestors[64];
    std::size_t depth = 0;
    std::uint32_t index = record->root;
    while (index != NO_BLOCK)
    {
        const Block &block = record->blocks[index];
        if (block.address == start && block.size == size)
        {
            return true;
        }
        if (depth == 64)
        {
            failed_ = true;
            return false;
        }
        ancestors[depth++] = index;
        if (start < block.address)
        {
            if (size > block.address - start)
            {
                failed_ = true;
                return false;
            }
            index = block.left;
        }
        else
        {
            if (start - block.address < block.size)
            {
                failed_ = true;
                return false;
            }
            index = block.right;
        }
    }
    if (record->blocks.size() >= config_.maxBlocksPerFrame ||
        record->byteSize > config_.maxBytesPerFrame ||
        size > config_.maxBytesPerFrame - record->byteSize)
    {
        failed_ = true;
        return false;
    }

    const std::size_t offset = record->byteSize;
    EnsureByteCapacity(*record, offset + size);
    CopyBytes(record->bytes.get() + offset, address, size);
    record->byteSize += size;
    const auto inserted = static_cast<std::uint32_t>(record->blocks.size());
    record->blocks.push_back(Block{start, offset, size});
    if (depth == 0)
        record->root = inserted;
    else
    {
        Block &parent = record->blocks[ancestors[depth - 1]];
        (start < parent.address ? parent.left : parent.right) = inserted;
    }
    while (depth != 0)
    {
        const std::uint32_t root = ancestors[--depth];
        const int previousHeight = record->blocks[root].height;
        const std::uint32_t balanced = RebalanceBlock(*record, root);
        if (balanced != root)
        {
            if (depth == 0)
                record->root = balanced;
            else
            {
                Block &parent = record->blocks[ancestors[depth - 1]];
                (parent.left == root ? parent.left : parent.right) = balanced;
            }
        }
        // Ancestors above an unchanged subtree height cannot need rotation.
        if (record->blocks[balanced].height == previousHeight)
            break;
    }
    return true;
}

bool RollbackJournal::EndFrame()
{
    if (!frameOpen_ || failed_)
    {
        return false;
    }
    frameOpen_ = false;
    return true;
}

bool RollbackJournal::CoversBlock(const FrameRecord &record, const Block &block)
{
    auto index = record.root;
    while (index != NO_BLOCK)
    {
        const Block &candidate = record.blocks[index];
        if (block.address < candidate.address) index = candidate.left;
        else if (block.address - candidate.address >= candidate.size) index = candidate.right;
        else return block.size <= candidate.size - (block.address - candidate.address);
    }
    return false;
}

bool RollbackJournal::UndoTo(std::uint32_t frame, std::uint32_t *restoredFrame)
{
    const FrameRecord *target = FindFrame(frame);
    if (!configured_ || frameOpen_ || failed_ || !target)
    {
        return false;
    }
    const std::uint32_t checkpointStart = target->startFrame;
    if (restoredFrame)
        *restoredFrame = checkpointStart;

    std::size_t targetIndex = 0;
    while (&frames_[(firstFrame_ + targetIndex) % frames_.size()] != target)
        ++targetIndex;

    while (frameCount_ != 0 && BackRecord().endFrame >= checkpointStart)
    {
        FrameRecord &record = BackRecord();
        for (auto it = record.blocks.rbegin(); it != record.blocks.rend(); ++it)
        {
            const Block &block = *it;
            bool overwritten = false;
            if (config_.coalesceRestore && block.size >= 1024)
            {
                for (std::size_t earlier = targetIndex; earlier + 1 < frameCount_; ++earlier)
                {
                    if (CoversBlock(frames_[(firstFrame_ + earlier) % frames_.size()], block))
                    {
                        overwritten = true;
                        break;
                    }
                }
            }
            // Undo is synchronous. No game code can observe these intermediate
            // writes. Skip only FULL coverage by an older restored checkpoint;
            // partial overlaps and first-written/spawned slots keep the original
            // newest-to-oldest restore order and copy every required byte.
            if (overwritten)
            {
                restoreSkippedBytes_ += block.size;
                continue;
            }
            CopyBytes(reinterpret_cast<void *>(block.address),
                      record.bytes.get() + block.offset, block.size);
            restoreCopiedBytes_ += block.size;
        }
        // Keep both arenas alive for the imminent corrected forward pass.
        --frameCount_;
    }
    return true;
}

void RollbackJournal::DiscardBefore(std::uint32_t frame)
{
    if (frameOpen_)
    {
        return;
    }
    while (frameCount_ != 0 && frames_[firstFrame_].endFrame < frame)
    {
        firstFrame_ = (firstFrame_ + 1) % frames_.size();
        --frameCount_;
    }
}

const RollbackJournal::FrameRecord *RollbackJournal::FindFrame(std::uint32_t frame) const
{
    for (std::size_t index = 0; index < frameCount_; ++index)
    {
        const FrameRecord &record = frames_[(firstFrame_ + index) % frames_.size()];
        if (record.startFrame <= frame && frame <= record.endFrame)
            return &record;
    }
    return nullptr;
}

std::size_t RollbackJournal::BytesForFrame(std::uint32_t frame) const
{
    const FrameRecord *record = FindFrame(frame);
    return record ? record->byteSize : 0;
}

std::size_t RollbackJournal::BlocksForFrame(std::uint32_t frame) const
{
    const FrameRecord *record = FindFrame(frame);
    return record ? record->blocks.size() : 0;
}

bool RollbackJournal::AddFloatPairToSnapshots(std::uint32_t afterFrame,
                                              std::uint32_t throughFrame,
                                              void *address, float dx, float dy)
{
    if (!configured_ || frameOpen_ || failed_ || !address || throughFrame <= afterFrame)
        return throughFrame <= afterFrame;
    const auto target = reinterpret_cast<std::uintptr_t>(address);
    constexpr std::size_t bytes = sizeof(float) * 2;
    if (target > std::numeric_limits<std::uintptr_t>::max() - bytes)
        return false;

    struct PatchTarget
    {
        FrameRecord *record;
        std::size_t offset;
    };
    std::vector<PatchTarget> targets;
    targets.reserve(frameCount_);

    for (std::size_t index = 0; index < frameCount_; ++index)
    {
        FrameRecord &record = frames_[(firstFrame_ + index) % frames_.size()];
        if (record.startFrame <= afterFrame || record.startFrame > throughFrame)
            continue;

        bool found = false;
        for (const Block &block : record.blocks)
        {
            if (target < block.address || target + bytes > block.address + block.size)
                continue;
            const std::size_t offset = block.offset + static_cast<std::size_t>(target - block.address);
            if (offset + bytes > record.byteSize)
                return false;
            targets.push_back(PatchTarget{&record, offset});
            found = true;
            break;
        }
        if (!found)
            return false;
    }

    for (const PatchTarget &patch : targets)
    {
        float values[2];
        std::memcpy(values, patch.record->bytes.get() + patch.offset, bytes);
        values[0] += dx;
        values[1] += dy;
        std::memcpy(patch.record->bytes.get() + patch.offset, values, bytes);
    }
    return true;
}
} // namespace Netplay

