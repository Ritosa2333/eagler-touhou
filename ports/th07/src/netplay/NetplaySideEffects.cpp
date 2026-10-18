#include "NetplaySideEffects.hpp"

#include <cstring>

namespace Netplay::SideEffects
{
namespace
{
bool g_Speculative = false;
constexpr std::size_t BGM_PATH_CAPACITY = 256;
char g_PresentedBgmPath[BGM_PATH_CAPACITY] = {};
char g_CorrectedBgmPath[BGM_PATH_CAPACITY] = {};
bool g_PresentedBgmKnown = false;
bool g_CorrectedBgmPending = false;
}

void SetSpeculative(bool speculative)
{
    g_Speculative = speculative;
}

bool IsSpeculative()
{
    return g_Speculative;
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
        return g_Speculative;

    if (!g_Speculative)
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
} // namespace Netplay::SideEffects
