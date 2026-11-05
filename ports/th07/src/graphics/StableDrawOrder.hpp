#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace Graphics
{
// Presentation-only scratch. Preserve stable ordering exactly, without an
// allocator or repeatedly chasing large objects from a comparison function.
template <typename T, std::size_t Capacity>
class StableDrawOrder
{
public:
    template <typename Key>
    bool Sort(T **items, std::size_t count, Key key)
    {
        if (count > Capacity) return false;
        if (count < 2) return true;
        bool ordered = true;
        for (std::size_t i = 0; i < count; ++i)
        {
            a_[i] = {items[i], key(items[i])};
            if (i && a_[i - 1].key > a_[i].key) ordered = false;
        }
        if (ordered) return true;
        Entry *source = a_.data(), *destination = b_.data();
        for (unsigned shift = 0; shift < 64; shift += 8)
        {
            std::array<std::size_t, 256> offsets{};
            for (std::size_t i = 0; i < count; ++i)
                ++offsets[(source[i].key >> shift) & 255u];
            // Uniform bytes need no permutation (usual for texture IDs).
            if (offsets[(source[0].key >> shift) & 255u] == count) continue;
            std::size_t start = 0;
            for (auto &offset : offsets)
            {
                const auto length = offset;
                offset = start;
                start += length;
            }
            for (std::size_t i = 0; i < count; ++i)
                destination[offsets[(source[i].key >> shift) & 255u]++] = source[i];
            Entry *swap = source; source = destination; destination = swap;
        }
        for (std::size_t i = 0; i < count; ++i) items[i] = source[i].item;
        return true;
    }

    // Lexicographic (blend, collision kind, signed texture ID).
    static constexpr std::uint64_t BulletKey(bool blend, std::uint8_t collision,
                                              std::int32_t texture)
    {
        return (static_cast<std::uint64_t>(blend) << 40) |
               (static_cast<std::uint64_t>(collision) << 32) |
               (static_cast<std::uint32_t>(texture) ^ 0x80000000u);
    }

private:
    struct Entry { T *item; std::uint64_t key; };
    std::array<Entry, Capacity> a_{};
    std::array<Entry, Capacity> b_{};
};
} // namespace Graphics
