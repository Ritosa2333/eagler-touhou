#pragma once

#include <cstddef>

namespace Netplay::SideEffects
{
void SetSpeculative(bool speculative);
bool IsSpeculative();

// Reconcile the final long-lived BGM selection after rollback while leaving
// already-presented one-shot sounds suppressed during resimulation.
void ResetBgmHistory();
bool ObserveBgmPlay(const char *path);
bool ConsumeCorrectedBgmPlay(char *path, std::size_t capacity);
} // namespace Netplay::SideEffects
