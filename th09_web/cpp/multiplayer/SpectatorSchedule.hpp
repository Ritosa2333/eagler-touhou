#pragma once
#include <eagler/netplay/FrameBudget.hpp>
#include <algorithm>
#include <cstdint>

namespace th09::multiplayer {
// One budget for the entire viewer callback, not one per nested catch-up tick.
// Queue entries, not wall-clock debt, retain all unconsumed confirmed input.
struct SpectatorSchedule {
    static unsigned Plan(std::size_t queued, unsigned dueTicks) {
        const unsigned rate = queued > 8 ? 4 : queued > 4 ? 2 : 1;
        return static_cast<unsigned>(std::min<std::size_t>(queued,
            std::min(Netplay::FrameBudget::MaxCatchupTicks, std::min(dueTicks, 4u) * rate)));
    }
    static bool CanStart(unsigned completed, double elapsedMs) {
        return Netplay::FrameBudget::CanStartTick(completed,
            static_cast<std::uint64_t>(std::max(0.0, elapsedMs) * 1000000.0));
    }
    static bool Render(unsigned index, unsigned planned) {
        // TH09 Draw has canonical side effects. Never call it again merely to
        // present after yielding. Render the first tick as a fallback, plus the
        // planned final tick if reached. Even one costly tick remains visible.
        return index == 0 || index + 1 == planned;
    }
};
}
