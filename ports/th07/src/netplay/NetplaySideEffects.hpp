#pragma once

#include <cstddef>

namespace Netplay::SideEffects
{
void SetSpeculative(bool speculative);
bool IsSpeculative();
void SetResimulation(bool resimulation);
bool IsResimulation();
// Read-only Replay music commands follow the reconstructed native timeline.
bool SuppressMusic();

// Reconcile the final long-lived BGM selection after rollback while leaving
// already-presented one-shot sounds suppressed during resimulation.
void ResetBgmHistory();
bool ObserveBgmPlay(const char *path);
bool ConsumeCorrectedBgmPlay(char *path, std::size_t capacity);
} // namespace Netplay::SideEffects
