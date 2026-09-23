#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
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

    struct Entry {
        std::uint32_t generation = 0;
        Key key{};
        Value value{};
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
            for (auto& entry : entries_) entry.generation = 0;
        }
        size_ = 0;
        overflow_count_ = 0;
    }

    // Named reset for rollback callers; it has the same semantics as clear.
    void reset() noexcept { clear(); }

    InsertResult try_emplace(const Key& key, const Value& value) noexcept {
        const std::size_t start = bucket(key);
        for (std::size_t probe = 0; probe < Capacity; ++probe) {
            Entry& entry = entries_[(start + probe) % Capacity];
            if (entry.generation != generation_) {
                entry.generation = generation_;
                entry.key = key;
                entry.value = value;
                ++size_;
                return {&entry.value, true, false};
            }
            if (entry.key == key) return {&entry.value, false, false};
        }
        ++overflow_count_;
        return {nullptr, false, true};
    }

    Value* find(const Key& key) noexcept {
        const std::size_t start = bucket(key);
        for (std::size_t probe = 0; probe < Capacity; ++probe) {
            Entry& entry = entries_[(start + probe) % Capacity];
            if (entry.generation != generation_) return nullptr;
            if (entry.key == key) return &entry.value;
        }
        return nullptr;
    }

    const Value* find(const Key& key) const noexcept {
        const std::size_t start = bucket(key);
        for (std::size_t probe = 0; probe < Capacity; ++probe) {
            const Entry& entry = entries_[(start + probe) % Capacity];
            if (entry.generation != generation_) return nullptr;
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
        return Hash{}(key) % Capacity;
    }

    std::array<Entry, Capacity> entries_{};
    std::uint32_t generation_ = 1;
    std::size_t size_ = 0;
    std::size_t overflow_count_ = 0;
};

} // namespace th10::multiplayer
