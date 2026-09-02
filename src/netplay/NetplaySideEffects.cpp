#include "NetplaySideEffects.hpp"

#include <cstddef>

namespace Netplay::SideEffects
{
namespace
{
bool g_Speculative = false;
bool g_SimulationFrameActive = false;
std::uint32_t g_CurrentGeneration = 0;
std::uint32_t g_CurrentFrame = 0;
std::uint32_t g_HistoryGeneration = static_cast<std::uint32_t>(-1);
unsigned g_BombStartScopeDepth = 0;

struct BombStartFrameHistory
{
    std::uint32_t generation = 0;
    std::uint32_t frame = 0;
    std::uint32_t emittedMask = 0;
    std::uint32_t pendingMask = 0;
    bool valid = false;
};

// Rollback is capped at 12 frames. 32 keeps this tiny presentation-only history
// safely beyond the correction window. TH06 has exactly 32 logical SFX IDs, so
// one bit mask also matches the existing per-tick same-ID de-duplication.
constexpr std::size_t BOMB_HISTORY_FRAMES = 32;
BombStartFrameHistory g_BombStartHistory[BOMB_HISTORY_FRAMES] = {};

void ResetBombStartHistory(std::uint32_t generation)
{
    for (BombStartFrameHistory &slot : g_BombStartHistory)
        slot = {};
    g_HistoryGeneration = generation;
}

BombStartFrameHistory &CurrentBombStartSlot()
{
    BombStartFrameHistory &slot = g_BombStartHistory[g_CurrentFrame % BOMB_HISTORY_FRAMES];
    if (!slot.valid || slot.generation != g_CurrentGeneration || slot.frame != g_CurrentFrame)
    {
        slot = {};
        slot.generation = g_CurrentGeneration;
        slot.frame = g_CurrentFrame;
        slot.valid = true;
    }
    return slot;
}

bool BombStartCaptureActive()
{
    return g_SimulationFrameActive && g_BombStartScopeDepth != 0;
}
} // namespace

void SetSpeculative(bool speculative)
{
    g_Speculative = speculative;
}

bool IsSpeculative()
{
    return g_Speculative;
}

void BeginSimulationFrame(std::uint32_t generation, std::uint32_t frame, bool speculative)
{
    if (generation != g_HistoryGeneration)
        ResetBombStartHistory(generation);
    g_CurrentGeneration = generation;
    g_CurrentFrame = frame;
    g_SimulationFrameActive = true;
    g_Speculative = speculative;
    g_BombStartScopeDepth = 0;
}

void EndSimulationFrame()
{
    g_Speculative = false;
    g_SimulationFrameActive = false;
    g_BombStartScopeDepth = 0;
}

BombStartScope::BombStartScope()
{
    if (g_SimulationFrameActive)
        ++g_BombStartScopeDepth;
}

BombStartScope::~BombStartScope()
{
    if (g_SimulationFrameActive && g_BombStartScopeDepth != 0)
        --g_BombStartScopeDepth;
}

void NoteSpeculativeBombStartSound(std::uint32_t soundId)
{
    if (!g_Speculative || !BombStartCaptureActive() || soundId >= 32)
        return;
    BombStartFrameHistory &slot = CurrentBombStartSlot();
    const std::uint32_t bit = std::uint32_t{1} << soundId;
    if ((slot.emittedMask & bit) == 0)
        slot.pendingMask |= bit;
}

void NoteBombStartSoundEnqueued(std::uint32_t soundId)
{
    if (g_Speculative || !BombStartCaptureActive() || soundId >= 32)
        return;
    BombStartFrameHistory &slot = CurrentBombStartSlot();
    const std::uint32_t bit = std::uint32_t{1} << soundId;
    slot.emittedMask |= bit;
    slot.pendingMask &= ~bit;
}

std::uint32_t ConsumeCorrectedBombStartSounds()
{
    std::uint32_t result = 0;
    for (BombStartFrameHistory &slot : g_BombStartHistory)
    {
        if (!slot.valid || slot.generation != g_CurrentGeneration || slot.pendingMask == 0)
            continue;
        result |= slot.pendingMask;
        // This runs before the current live simulation frame, when the previous
        // tick's SFX queue has already been consumed. A Bomb start can contribute
        // at most the small set of Bomb-start IDs, so the normal five-slot queue
        // has room for the correction. Mark them heard to prevent a later rewind
        // of the same frame from replaying them again.
        slot.emittedMask |= slot.pendingMask;
        slot.pendingMask = 0;
    }
    return result;
}

SimulationFrameScope::SimulationFrameScope(std::uint32_t generation, std::uint32_t frame, bool speculative)
{
    BeginSimulationFrame(generation, frame, speculative);
}

SimulationFrameScope::~SimulationFrameScope()
{
    Finish();
}

void SimulationFrameScope::Finish()
{
    if (!active)
        return;
    EndSimulationFrame();
    active = false;
}
} // namespace Netplay::SideEffects
