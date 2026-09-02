#pragma once

#include <cstdint>

namespace Netplay::SideEffects
{
void SetSpeculative(bool speculative);
bool IsSpeculative();

void BeginSimulationFrame(std::uint32_t generation, std::uint32_t frame, bool speculative);
void EndSimulationFrame();

// Only the initial Bomb calc is special-cased. Normal resimulation SFX remain
// suppressed; sounds attempted while this scope is active are remembered and
// any sound IDs that were not already heard on the forward pass are replayed
// once after the rollback batch.
class BombStartScope
{
public:
    BombStartScope();
    ~BombStartScope();
    BombStartScope(const BombStartScope &) = delete;
    BombStartScope &operator=(const BombStartScope &) = delete;
};

void NoteSpeculativeBombStartSound(std::uint32_t soundId);
void NoteBombStartSoundEnqueued(std::uint32_t soundId);
std::uint32_t ConsumeCorrectedBombStartSounds();

class SimulationFrameScope
{
public:
    SimulationFrameScope(std::uint32_t generation, std::uint32_t frame, bool speculative);
    ~SimulationFrameScope();
    SimulationFrameScope(const SimulationFrameScope &) = delete;
    SimulationFrameScope &operator=(const SimulationFrameScope &) = delete;
    void Finish();

private:
    bool active = true;
};
} // namespace Netplay::SideEffects
