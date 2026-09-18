#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <vector>

namespace Netplay
{
// First-write undo history for a fixed pool with disjoint, exhaustive parts.
// Unlike the general address journal, pool/part identity provides O(1) lookup.
// A caller MUST touch a part before any simulation or presentation write to it.
// Untouched parts are not discarded: their bytes remain live until a writer
// records them. Undo selects the oldest saved value for each touched part.
template <typename T, std::size_t Count, std::size_t Parts>
class PartitionedPoolJournal
{
    static_assert(Count > 0 && Parts > 0 && Parts < 32);
    static_assert(sizeof(T) * Count < std::numeric_limits<std::uint32_t>::max());
    static constexpr std::uint32_t Missing = std::numeric_limits<std::uint32_t>::max();
    struct Record
    {
        std::uint32_t first = 0, last = 0;
        std::array<std::array<std::uint32_t, Parts>, Count> offsets{};
        std::vector<std::uint32_t> keys;
        std::unique_ptr<std::uint8_t[]> bytes;
        std::size_t used = 0, capacity = 0;
    };

public:
    struct Part { std::size_t offset, size; };
    struct CopyRegion { std::uint32_t from, to, bytes; };
    using CopyFunction = void (*)(void *, const void *, const void *, std::size_t);
    static constexpr std::uint32_t AllParts = (1u << Parts) - 1u;

    bool Reset(T *pool, const std::array<Part, Parts> &parts, std::size_t maxFrames,
               CopyFunction copy = nullptr)
    {
        Clear();
        if (!pool || maxFrames == 0) return false;
        std::size_t end = 0;
        for (const auto &part : parts)
        {
            if (!part.size || part.offset != end || part.size > sizeof(T) - end)
                return false;
            end += part.size;
        }
        if (end != sizeof(T)) return false;
        pool_ = pool;
        parts_ = parts;
        records_.resize(maxFrames);
        copy_ = copy;
        plan_.reserve(Count * Parts);
        return true;
    }

    void Clear()
    {
        records_.clear(); pool_ = nullptr;
        head_ = count_ = 0; open_ = failed_ = false;
        copied_ = skipped_ = growths_ = 0;
        copy_ = nullptr; plan_.clear();
    }

    bool BeginFrame(std::uint32_t frame, bool extend = false)
    {
        if (!pool_ || open_ || failed_) return false;
        if (extend)
        {
            if (!count_ || Back().last == Missing || frame != Back().last + 1u)
                return Fail();
            Back().last = frame;
        }
        else
        {
            if (count_ && frame <= Back().last) return Fail();
            if (count_ == records_.size()) head_ = (head_ + 1) % records_.size();
            else ++count_;
            auto &record = Back();
            record.first = record.last = frame;
            record.used = 0;
            record.keys.clear();
            if (!record.keys.capacity()) record.keys.reserve(Count * Parts);
            for (auto &offsets : record.offsets) offsets.fill(Missing);
        }
        open_ = true;
        return true;
    }

    bool Touch(std::size_t slot, std::uint32_t mask)
    {
        if (!open_ || failed_ || slot >= Count || (mask & ~AllParts)) return Fail();
        auto &record = Back();
        const auto initialized = record.used;
        plan_.clear();
        PlanTouch(record, slot, mask);
        EnsureCapacity(record, record.used, initialized);
        CopyBatch(record.bytes.get(), pool_);
        return true;
    }

    // Gather all first-write parts before returning. No game callback can
    // execute between planning and copying; this is not deferred undo capture.
    bool CaptureMasks(const std::array<std::uint32_t, Count> &masks)
    {
        if (!open_ || failed_) return false;
        for (const auto mask : masks) if (mask & ~AllParts) return Fail();
        auto &record = Back();
        const auto initialized = record.used;
        plan_.clear();
        for (std::size_t slot = 0; slot < Count; ++slot) PlanTouch(record, slot, masks[slot]);
        EnsureCapacity(record, record.used, initialized);
        CopyBatch(record.bytes.get(), pool_);
        return true;
    }

    bool EndFrame()
    {
        if (!open_ || failed_) return false;
        open_ = false;
        return true;
    }

    bool UndoTo(std::uint32_t frame, std::uint32_t *restoredFrame = nullptr)
    {
        if (open_ || failed_ || !pool_) return false;
        std::size_t target = 0;
        while (target < count_ && !(At(target).first <= frame && frame <= At(target).last)) ++target;
        if (target == count_) return false;
        if (restoredFrame) *restoredFrame = At(target).first;
        std::array<std::uint32_t, Count> covered{};
        // No game callback can observe intermediate restoration. The oldest
        // first-write value is the exact target value, including parts first
        // written in a later checkpoint. No byte hashes or approximate equality.
        for (std::size_t index = target; index < count_; ++index)
        {
            const auto &record = At(index);
            plan_.clear();
            for (const auto key : record.keys)
            {
                const std::size_t slot = key / Parts, partIndex = key % Parts;
                const auto &part = parts_[partIndex];
                const auto bit = 1u << partIndex;
                if (covered[slot] & bit) { skipped_ += part.size; continue; }
                PlanCopy(record.offsets[slot][partIndex], slot * sizeof(T) + part.offset, part.size);
                copied_ += part.size;
                covered[slot] |= bit;
            }
            CopyBatch(pool_, record.bytes.get());
        }
        count_ = target; // Retain arenas for the corrected replay.
        return true;
    }

    void DiscardBefore(std::uint32_t frame)
    {
        if (open_) return;
        while (count_ && At(0).last < frame) { head_ = (head_ + 1) % records_.size(); --count_; }
    }

    std::size_t BytesForFrame(std::uint32_t frame) const
    {
        const auto *record = Find(frame); return record ? record->used : 0;
    }
    std::size_t BlocksForFrame(std::uint32_t frame) const
    {
        const auto *record = Find(frame); return record ? record->keys.size() : 0;
    }
    bool Failed() const { return failed_; }
    std::uint64_t RestoreCopiedBytes() const { return copied_; }
    std::uint64_t RestoreSkippedBytes() const { return skipped_; }
    std::uint64_t ArenaGrowths() const { return growths_; }

private:
    bool Fail() { failed_ = true; return false; }
    void PlanCopy(std::size_t from, std::size_t to, std::size_t bytes)
    {
        if (!plan_.empty() && plan_.back().from + plan_.back().bytes == from &&
            plan_.back().to + plan_.back().bytes == to)
            plan_.back().bytes += static_cast<std::uint32_t>(bytes);
        else plan_.push_back({static_cast<std::uint32_t>(from), static_cast<std::uint32_t>(to),
                              static_cast<std::uint32_t>(bytes)});
    }
    void CopyBatch(void *destination, const void *source)
    {
        if (plan_.empty()) return;
        if (copy_) { copy_(destination, source, plan_.data(), plan_.size()); return; }
        for (const auto &copy : plan_)
            std::memcpy(static_cast<std::uint8_t *>(destination) + copy.to,
                        static_cast<const std::uint8_t *>(source) + copy.from, copy.bytes);
    }
    void PlanTouch(Record &record, std::size_t slot, std::uint32_t mask)
    {
        for (std::size_t index = 0; index < Parts; ++index)
        {
            if (!(mask & (1u << index)) || record.offsets[slot][index] != Missing) continue;
            const auto &part = parts_[index];
            PlanCopy(slot * sizeof(T) + part.offset, record.used, part.size);
            record.offsets[slot][index] = static_cast<std::uint32_t>(record.used);
            record.keys.push_back(static_cast<std::uint32_t>(slot * Parts + index));
            record.used += part.size;
        }
    }
    Record &At(std::size_t index) { return records_[(head_ + index) % records_.size()]; }
    Record &Back() { return At(count_ - 1); }
    const Record *Find(std::uint32_t frame) const
    {
        for (std::size_t i = 0; i < count_; ++i)
        {
            const auto &record = records_[(head_ + i) % records_.size()];
            if (record.first <= frame && frame <= record.last) return &record;
        }
        return nullptr;
    }
    void EnsureCapacity(Record &record, std::size_t required, std::size_t initialized)
    {
        if (required <= record.capacity) return;
        constexpr std::size_t maximum = Count * sizeof(T);
        std::size_t capacity = record.capacity ? record.capacity : std::min<std::size_t>(maximum, 256 * 1024);
        while (capacity < required) capacity = capacity <= maximum / 2 ? capacity * 2 : maximum;
        std::unique_ptr<std::uint8_t[]> bytes(new std::uint8_t[capacity]);
        if (initialized) std::memcpy(bytes.get(), record.bytes.get(), initialized);
        record.bytes = std::move(bytes); record.capacity = capacity; ++growths_;
    }
    T *pool_ = nullptr;
    std::array<Part, Parts> parts_{};
    std::vector<Record> records_;
    std::vector<CopyRegion> plan_;
    CopyFunction copy_ = nullptr;
    std::size_t head_ = 0, count_ = 0;
    bool open_ = false, failed_ = false;
    std::uint64_t copied_ = 0, skipped_ = 0, growths_ = 0;
};
} // namespace Netplay

