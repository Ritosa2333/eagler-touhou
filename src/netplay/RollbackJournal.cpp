#include "RollbackJournal.hpp"

#include <algorithm>
#include <cstring>
#include <limits>

namespace Netplay
{
bool RollbackJournal::Reset(const RollbackJournalConfig &config)
{
    Clear();
    if (config.maxFrames == 0 || config.maxBytesPerFrame == 0 ||
        config.maxBlocksPerFrame == 0)
    {
        return false;
    }
    config_ = config;
    configured_ = true;
    return true;
}

void RollbackJournal::Clear()
{
    frames_.clear();
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
        if (frames_.empty() || frames_.back().endFrame == std::numeric_limits<std::uint32_t>::max() ||
            frame != frames_.back().endFrame + 1u)
        {
            failed_ = true;
            return false;
        }
        frames_.back().endFrame = frame;
        frameOpen_ = true;
        openFrame_ = frame;
        return true;
    }
    if (!frames_.empty() && frame <= frames_.back().endFrame)
    {
        failed_ = true;
        return false;
    }
    if (frames_.size() == config_.maxFrames)
    {
        // Reuse the multi-megabyte byte buffer from the evicted history slot.
        // Growing a fresh vector on every dense frame creates allocator churn
        // and WebAssembly heap-growth stalls.
        FrameRecord recycled = std::move(frames_.front());
        frames_.pop_front();
        recycled.bytes.clear();
        recycled.blocks.clear();
        frames_.push_back(std::move(recycled));
    }
    else
    {
        frames_.emplace_back();
    }
    FrameRecord &record = frames_.back();
    record.startFrame = frame;
    record.endFrame = frame;
    if (record.bytes.capacity() == 0)
        record.bytes.reserve(std::min<std::size_t>(config_.maxBytesPerFrame, 256 * 1024));
    frameOpen_ = true;
    openFrame_ = frame;
    return true;
}

RollbackJournal::FrameRecord *RollbackJournal::CurrentRecord()
{
    return frameOpen_ && !frames_.empty() ? &frames_.back() : nullptr;
}

bool RollbackJournal::RangesOverlap(std::uintptr_t a, std::size_t aSize,
                                    std::uintptr_t b, std::size_t bSize)
{
    const std::uintptr_t max = std::numeric_limits<std::uintptr_t>::max();
    if (aSize > max - a || bSize > max - b)
    {
        return true;
    }
    return a < b + bSize && b < a + aSize;
}

bool RollbackJournal::Touch(void *address, std::size_t size)
{
    FrameRecord *record = CurrentRecord();
    if (!record || failed_ || address == nullptr || size == 0)
    {
        return false;
    }

    const std::uintptr_t start = reinterpret_cast<std::uintptr_t>(address);
    const auto next = record->blocks.lower_bound(start);
    if (next != record->blocks.end())
    {
        const Block &block = next->second;
        if (block.address == start && block.size == size)
        {
            return true;
        }
        if (RangesOverlap(block.address, block.size, start, size))
        {
            failed_ = true;
            return false;
        }
    }
    if (next != record->blocks.begin())
    {
        const Block &block = std::prev(next)->second;
        if (RangesOverlap(block.address, block.size, start, size))
        {
            failed_ = true;
            return false;
        }
    }
    if (record->blocks.size() >= config_.maxBlocksPerFrame ||
        record->bytes.size() > config_.maxBytesPerFrame ||
        size > config_.maxBytesPerFrame - record->bytes.size())
    {
        failed_ = true;
        return false;
    }

    const std::size_t offset = record->bytes.size();
    record->bytes.resize(offset + size);
    std::memcpy(record->bytes.data() + offset, address, size);
    record->blocks.emplace(start, Block{start, offset, size});
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

    while (!frames_.empty() && frames_.back().endFrame >= checkpointStart)
    {
        FrameRecord &record = frames_.back();
        for (auto it = record.blocks.rbegin(); it != record.blocks.rend(); ++it)
        {
            const Block &block = it->second;
            std::memcpy(reinterpret_cast<void *>(block.address),
                        record.bytes.data() + block.offset, block.size);
        }
        frames_.pop_back();
    }
    return true;
}

void RollbackJournal::DiscardBefore(std::uint32_t frame)
{
    if (frameOpen_)
    {
        return;
    }
    while (!frames_.empty() && frames_.front().endFrame < frame)
    {
        frames_.pop_front();
    }
}

const RollbackJournal::FrameRecord *RollbackJournal::FindFrame(std::uint32_t frame) const
{
    const auto it = std::find_if(frames_.begin(), frames_.end(),
                                 [frame](const FrameRecord &record) {
                                     return record.startFrame <= frame && frame <= record.endFrame;
                                 });
    return it == frames_.end() ? nullptr : &*it;
}

std::size_t RollbackJournal::BytesForFrame(std::uint32_t frame) const
{
    const FrameRecord *record = FindFrame(frame);
    return record ? record->bytes.size() : 0;
}

std::size_t RollbackJournal::BlocksForFrame(std::uint32_t frame) const
{
    const FrameRecord *record = FindFrame(frame);
    return record ? record->blocks.size() : 0;
}
} // namespace Netplay
