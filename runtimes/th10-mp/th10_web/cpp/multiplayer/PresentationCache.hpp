#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <utility>

namespace th10::multiplayer {

// A draw-side first-sample cache for MP.  The cache deliberately has no
// allocation or erase path: clear() advances the generation and makes all
// previous samples unreachable in O(1), which keeps rollback re-sampling out
// of the allocator.  A full cache is a presentation-only miss; authoritative
// simulation continues and the caller can fall back to a snap.
template <typename Key, typename Value, std::size_t Capacity,
          typename Hash = std::hash<Key>>
class PresentationCache final {
    static_assert(Capacity != 0, "presentation cache needs a positive capacity");

    // Keep payload capacity unchanged while leaving at least half of the
    // lookup table empty. A dense scene may exhaust presentation samples;
    // an uncached embedded VM must not scan every payload on each update.
    static_assert(Capacity <= std::numeric_limits<std::size_t>::max() / 2,
                  "presentation index capacity overflow");
    static_assert(Capacity <= std::numeric_limits<std::uint32_t>::max(),
                  "presentation payload index overflow");
    static constexpr std::size_t IndexCapacity = Capacity * 2;
    struct Entry { Key key{}; Value value{}; };
    struct IndexEntry {
        std::uint32_t generation = 0;
        std::uint32_t payload = 0;
    };

public:
    struct InsertResult {
        Value* value = nullptr;
        bool inserted = false;
        bool overflow = false;
    };

    PresentationCache() noexcept = default;
    PresentationCache(const PresentationCache&) = delete;
    PresentationCache& operator=(const PresentationCache&) = delete;

    // Start a new sample generation.  The generation stamp means that stale
    // keys do not need to be destructed or shifted out of the open table.
    void clear() noexcept {
        ++generation_;
        if (generation_ == 0) {
            generation_ = 1;
            for (auto& entry : index_) entry.generation = 0;
        }
        size_ = 0;
        overflow_count_ = 0;
    }

    // Named reset for rollback callers; it has the same semantics as clear.
    void reset() noexcept { clear(); }

    InsertResult try_emplace(const Key& key, const Value& value) noexcept {
        return try_emplace_with(key, [&]() { return value; });
    }

    // Delay sample construction until insertion. Registry VMs already have
    // their first sample, and exhausted caches do not need a discarded copy.
    template <typename Factory>
    InsertResult try_emplace_with(const Key& key, Factory&& factory) noexcept {
        const std::size_t start = bucket(key);
        for (std::size_t probe = 0; probe < IndexCapacity; ++probe) {
            IndexEntry& slot = index_[(start + probe) % IndexCapacity];
            if (slot.generation != generation_) {
                if (size_ == Capacity) {
                    ++overflow_count_;
                    return {nullptr, false, true};
                }
                Entry& entry = entries_[size_];
                entry.key = key;
                entry.value = std::forward<Factory>(factory)();
                slot.payload = static_cast<std::uint32_t>(size_++);
                slot.generation = generation_;
                return {&entry.value, true, false};
            }
            Entry& entry = entries_[slot.payload];
            if (entry.key == key) return {&entry.value, false, false};
        }
        // The half-empty index cannot fill before payload capacity is hit.
        ++overflow_count_;
        return {nullptr, false, true};
    }

    Value* find(const Key& key) noexcept {
        const auto* result = static_cast<const PresentationCache&>(*this).find(key);
        return const_cast<Value*>(result);
    }

    const Value* find(const Key& key) const noexcept {
        const std::size_t start = bucket(key);
        for (std::size_t probe = 0; probe < IndexCapacity; ++probe) {
            const IndexEntry& slot = index_[(start + probe) % IndexCapacity];
            if (slot.generation != generation_) return nullptr;
            const Entry& entry = entries_[slot.payload];
            if (entry.key == key) return &entry.value;
        }
        return nullptr;
    }

    std::size_t size() const noexcept { return size_; }
    constexpr std::size_t capacity() const noexcept { return Capacity; }
    std::uint32_t generation() const noexcept { return generation_; }
    std::size_t overflow_count() const noexcept { return overflow_count_; }

private:
    std::size_t bucket(const Key& key) const noexcept {
        // Pointer hashes can be the raw address. Fold high address bits and
        // avalanche before indexing so aligned ANM/pool strides do not cluster.
        const auto hashed = Hash{}(key);
        auto mixed = static_cast<std::uint32_t>(hashed);
        if constexpr (sizeof(hashed) > sizeof(mixed))
            mixed ^= static_cast<std::uint32_t>(hashed >> 32);
        mixed ^= mixed >> 16;
        mixed *= 0x7feb352du;
        mixed ^= mixed >> 15;
        mixed *= 0x846ca68bu;
        mixed ^= mixed >> 16;
        return mixed % IndexCapacity;
    }

    std::array<Entry, Capacity> entries_{};
    std::array<IndexEntry, IndexCapacity> index_{};
    std::uint32_t generation_ = 1;
    std::size_t size_ = 0;
    std::size_t overflow_count_ = 0;
};

} // namespace th10::multiplayer
