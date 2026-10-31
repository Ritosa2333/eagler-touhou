#include "NetplaySideEffects.hpp"
#include <eagler/audio/ReplayMusicScope.hpp>

#include <cstddef>
#include <cstring>

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
constexpr std::size_t BGM_PATH_CAPACITY = 256;
char g_PresentedBgmPath[BGM_PATH_CAPACITY] = {};
char g_CorrectedBgmPath[BGM_PATH_CAPACITY] = {};
bool g_PresentedBgmKnown = false;
bool g_CorrectedBgmPending = false;

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
struct MissFrameHistory
{
    std::uint32_t generation = 0, frame = 0, misses = 0, emitted = 0;
    bool valid = false;
};
constexpr std::size_t MISS_HISTORY_FRAMES = 32;
MissFrameHistory g_MissHistory[MISS_HISTORY_FRAMES] = {};
std::uint32_t g_MissConfirmedGeneration = 0, g_MissConfirmedFrame = 0xffffffffu;
unsigned g_MissScopeDepth = 0;

MissFrameHistory &CurrentMissSlot()
{
    auto &slot = g_MissHistory[g_CurrentFrame % MISS_HISTORY_FRAMES];
    if (!slot.valid || slot.generation != g_CurrentGeneration || slot.frame != g_CurrentFrame)
    {
        slot = {};
        slot.generation = g_CurrentGeneration;
        slot.frame = g_CurrentFrame;
        slot.valid = true;
    }
    return slot;
}

constexpr std::size_t BOMB_HISTORY_FRAMES = 32;
BombStartFrameHistory g_BombStartHistory[BOMB_HISTORY_FRAMES] = {};

void ResetBombStartHistory(std::uint32_t generation)
{
    for (BombStartFrameHistory &slot : g_BombStartHistory)
        slot = {};
    g_HistoryGeneration = generation;
    for (auto &slot : g_MissHistory) slot = {};
    g_MissConfirmedGeneration = generation;
    g_MissConfirmedFrame = 0xffffffffu;
    g_MissScopeDepth = 0;
    ResetBgmHistory();
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

bool SuppressMusic()
{
    return g_Speculative && !eagler::audio::ReplayMusicScope::Active();
}

void ResetBgmHistory()
{
    g_PresentedBgmPath[0] = '\0';
    g_CorrectedBgmPath[0] = '\0';
    g_PresentedBgmKnown = false;
    g_CorrectedBgmPending = false;
}

bool ObserveBgmPlay(const char *path)
{
    if (!path || !std::memchr(path, '\0', BGM_PATH_CAPACITY))
        return SuppressMusic();

    if (!SuppressMusic())
    {
        std::strcpy(g_PresentedBgmPath, path);
        g_PresentedBgmKnown = true;
        g_CorrectedBgmPending = false;
        return false;
    }

    if (g_PresentedBgmKnown && std::strcmp(g_PresentedBgmPath, path) == 0)
    {
        g_CorrectedBgmPending = false;
    }
    else
    {
        std::strcpy(g_CorrectedBgmPath, path);
        g_CorrectedBgmPending = true;
    }
    return true;
}

bool ConsumeCorrectedBgmPlay(char *path, std::size_t capacity)
{
    if (!g_CorrectedBgmPending || !path || capacity == 0)
        return false;
    const std::size_t length = std::strlen(g_CorrectedBgmPath);
    if (length >= capacity)
        return false;
    std::memcpy(path, g_CorrectedBgmPath, length + 1);
    std::strcpy(g_PresentedBgmPath, g_CorrectedBgmPath);
    g_PresentedBgmKnown = true;
    g_CorrectedBgmPending = false;
    return true;
}

void BeginSimulationFrame(std::uint32_t generation, std::uint32_t frame, bool speculative)
{
    if (generation != g_HistoryGeneration)
        ResetBombStartHistory(generation);
    g_CurrentGeneration = generation;
    g_CurrentFrame = frame;
    g_SimulationFrameActive = true;
    // Re-evaluating a frame replaces its attempted Misses, including a
    // correction that contains no death at all.
    CurrentMissSlot().misses = 0;
    g_MissScopeDepth = 0;
    g_Speculative = speculative;
    g_BombStartScopeDepth = 0;
}

void EndSimulationFrame()
{
    g_Speculative = false;
    g_SimulationFrameActive = false;
    g_MissScopeDepth = 0;
    g_BombStartScopeDepth = 0;
}

MissScope::MissScope() { if (g_SimulationFrameActive) ++g_MissScopeDepth; }
MissScope::~MissScope() { if (g_SimulationFrameActive && g_MissScopeDepth) --g_MissScopeDepth; }
MissFrameTag CurrentMissFrameTag()
{
    return {g_CurrentGeneration, g_CurrentFrame, g_SimulationFrameActive && g_MissScopeDepth != 0};
}
bool RecordCommittedMiss(std::uint32_t player)
{
    if (!g_SimulationFrameActive || player >= 3) return false;
    CurrentMissSlot().misses |= std::uint32_t{1} << player;
    return true;
}
std::uint32_t ConsumeConfirmedMisses(std::uint32_t generation, std::uint32_t confirmedFrame)
{
    if (generation != g_HistoryGeneration || confirmedFrame == 0xffffffffu) return 0;
    g_MissConfirmedGeneration = generation;
    g_MissConfirmedFrame = confirmedFrame;
    std::uint32_t result = 0;
    for (auto &slot : g_MissHistory)
    {
        if (!slot.valid || slot.generation != generation || slot.frame > confirmedFrame) continue;
        result |= slot.misses & ~slot.emitted;
        slot.emitted |= slot.misses;
    }
    return result;
}
bool MissFrameVisible(const MissFrameTag &tag)
{
    return !tag.deferred || (tag.generation == g_MissConfirmedGeneration &&
        g_MissConfirmedFrame != 0xffffffffu && tag.frame <= g_MissConfirmedFrame);
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
