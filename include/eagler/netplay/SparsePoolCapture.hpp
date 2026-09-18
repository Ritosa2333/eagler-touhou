#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace Netplay
{
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

