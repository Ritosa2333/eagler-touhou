#include "RuntimeExtension.hpp"
#include "EnemyManager.hpp"

namespace RuntimeExtension
{
static EclLoadedHook g_EclLoadedHook;
static GameStartedHook g_GameStartedHook;

void SetEclLoadedHook(EclLoadedHook hook)
{
    g_EclLoadedHook = hook;
}

bool OnEclLoaded(const char *path, u8 *data, std::size_t size)
{
    return g_EclLoadedHook == nullptr || g_EclLoadedHook(path, data, size);
}

void SetGameStartedHook(GameStartedHook hook)
{
    g_GameStartedHook = hook;
}

void OnGameStarted(void *gameManager)
{
    if (g_GameStartedHook != nullptr)
        g_GameStartedHook(gameManager);
}

void SetTimelineTime(std::size_t count, i32 time)
{
    const std::size_t limit = count > 3 ? 3 : count;
    for (std::size_t index = 0; index < limit; ++index)
        g_EnemyManager.timelines[index].timelineTime.current = time;
}
} // namespace RuntimeExtension
