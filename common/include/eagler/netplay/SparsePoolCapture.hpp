#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace Netplay
{
// Occupancy bytes are authoritative metadata, not a derived live-object list.
// Skip only complete groups of zero bytes. The tail is checked byte by byte;
// memcpy supports unaligned storage, and every nonzero value means occupied.
inline std::size_t NextOccupiedSlot(const std::uint8_t *occupied,
                                    std::size_t index, std::size_t count)
{
    if (index >= count) return count;
    while (count - index >= sizeof(std::uint64_t))
    {
        std::uint64_t group;
        std::memcpy(&group, occupied + index, sizeof(group));
        if (group != 0) break;
        index += sizeof(group);
    }
    while (index < count && occupied[index] == 0) ++index;
    return index;
}

// Metadata for ONE journal checkpoint, not world state. Contiguous live slots
// are copied together; a spawn still captures an initially inactive slot before
// its first write. A recycled live slot is already covered by its original run.
template <std::size_t Capacity>
class SparsePoolCapture
{
public:
    template <typename T, typename IsLive, typename Touch>
    bool Capture(T *pool, IsLive isLive, Touch touch)
    {
        captured_.fill(false);
        for (std::size_t index = 0; index < Capacity;)
        {
            if (!isLive(pool[index])) { ++index; continue; }
            const std::size_t first = index++;
            while (index < Capacity && isLive(pool[index])) ++index;
            if (!touch(pool + first, (index - first) * sizeof(T))) return false;
            for (std::size_t slot = first; slot < index; ++slot) captured_[slot] = true;
        }
        return true;
    }

    // Preserve the exact ordered runs for owners with byte occupancy. No
    // cursor/count shortcut: the entire declared capacity remains covered.
    template <typename T, typename Touch>
    bool CaptureOccupied(T *pool, const std::uint8_t *occupied, Touch touch)
    {
        captured_.fill(false);
        for (std::size_t index = NextOccupiedSlot(occupied, 0, Capacity);
             index < Capacity; index = NextOccupiedSlot(occupied, index, Capacity))
        {
            const std::size_t first = index++;
            while (index < Capacity && occupied[index] != 0) ++index;
            if (!touch(pool + first, (index - first) * sizeof(T))) return false;
            for (std::size_t slot = first; slot < index; ++slot) captured_[slot] = true;
        }
        return true;
    }

    template <typename T, typename Touch>
    bool TouchSlot(T *pool, T *slot, Touch touch)
    {
        if (!slot) return true;
        const auto begin = reinterpret_cast<std::uintptr_t>(pool);
        const auto address = reinterpret_cast<std::uintptr_t>(slot);
        if (address < begin || address - begin >= sizeof(T) * Capacity ||
            (address - begin) % sizeof(T) != 0)
            return touch(slot, sizeof(T));
        const std::size_t index = (address - begin) / sizeof(T);
        if (captured_[index]) return true;
        if (!touch(slot, sizeof(T))) return false;
        captured_[index] = true;
        return true;
    }

private:
    std::array<bool, Capacity> captured_{};
};
} // namespace Netplay

