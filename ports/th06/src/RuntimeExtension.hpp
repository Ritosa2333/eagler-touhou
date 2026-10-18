#pragma once

#include <cstddef>
#include "inttypes.hpp"

namespace RuntimeExtension
{
using EclLoadedHook = bool (*)(const char *path, u8 *data, std::size_t size);
using GameStartedHook = void (*)(void *gameManager);

void SetEclLoadedHook(EclLoadedHook hook);
bool OnEclLoaded(const char *path, u8 *data, std::size_t size);
void SetGameStartedHook(GameStartedHook hook);
void OnGameStarted(void *gameManager);
void SetTimelineTime(std::size_t count, i32 time);
} // namespace RuntimeExtension
