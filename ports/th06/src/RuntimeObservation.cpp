#include "GameManager.hpp"
#include "Player.hpp"
#include "Supervisor.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define TH06_OBSERVATION_EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define TH06_OBSERVATION_EXPORT
#endif

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "multiplayer/GameplaySession.hpp"
#endif

#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace
{
constexpr std::uint32_t kMissing = 0xffffffffu;
constexpr std::uint32_t kMissingFloat = 0x7fc00000u;
constexpr std::uint32_t kPhase = 0x001u;
constexpr std::uint32_t kInput = 0x002u;
constexpr std::uint32_t kPosition = 0x004u;
constexpr std::uint32_t kWorldFrame = 0x008u;
constexpr std::uint32_t kRestartFrame = 0x010u;
constexpr std::uint32_t kPause = 0x020u;
constexpr std::uint32_t kLocalSeat = 0x100u;

enum : std::uint32_t
{
    kTitle = 0,
    kGameplay = 1,
    kPaused = 2,
    kResults = 3,
    kEnding = 4,
    kFinished = 5,
    kInactive = 6,
    kTransition = 7,
};

std::array<std::uint32_t, 12> g_Observation{};

void StoreFloat(std::uint32_t &word, float value)
{
    static_assert(sizeof(value) == sizeof(word));
    std::memcpy(&word, &value, sizeof(word));
}

void ResetObservation()
{
    g_Observation.fill(kMissing);
    g_Observation[0] = 1;
    g_Observation[1] = 0;
    g_Observation[8] = kMissingFloat;
    g_Observation[9] = kMissingFloat;
}
} // namespace

extern "C" TH06_OBSERVATION_EXPORT const std::uint32_t *th06_native_observation_v1()
{
    ResetObservation();
    auto &words = g_Observation;

    const auto state = g_Supervisor.curState;
    bool gameManagerActive = false;
    switch (state)
    {
    case SUPERVISOR_STATE_INIT:
        words[2] = kInactive;
        break;
    case SUPERVISOR_STATE_MAINMENU:
    case SUPERVISOR_STATE_MAINMENU_REPLAY:
    case SUPERVISOR_STATE_MUSICROOM:
        words[2] = kTitle;
        break;
    case SUPERVISOR_STATE_GAMEMANAGER:
        gameManagerActive = true;
        if (g_GameManager.isInRetryMenu)
            words[2] = kResults;
        else if (g_GameManager.isInGameMenu)
            words[2] = kPaused;
        else if (g_GameManager.isInMenu)
            words[2] = kGameplay;
        else
            words[2] = kTransition;
        break;
    case SUPERVISOR_STATE_GAMEMANAGER_REINIT:
    case SUPERVISOR_STATE_GAMEMANAGER_RESTART:
        words[2] = kTransition;
        break;
    case SUPERVISOR_STATE_RESULTSCREEN:
    case SUPERVISOR_STATE_RESULTSCREEN_FROMGAME:
        words[2] = kResults;
        break;
    case SUPERVISOR_STATE_ENDING:
        words[2] = kEnding;
        break;
    case SUPERVISOR_STATE_EXITSUCCESS:
    case SUPERVISOR_STATE_EXITERROR:
        words[2] = kFinished;
        break;
    default:
        return words.data();
    }
    words[1] |= kPhase;

    if (!gameManagerActive)
        return words.data();

    words[1] |= kPause;
    words[6] = !g_GameManager.isInRetryMenu && g_GameManager.isInGameMenu ? 1u : 0u;
    if (g_GameManager.gameFrames != kMissing)
    {
        words[1] |= kWorldFrame | kRestartFrame;
        words[4] = g_GameManager.gameFrames;
        words[5] = g_GameManager.gameFrames;
    }

    std::uint32_t localSeat = 0;
    Player *localPlayer = nullptr;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const bool multiplayer = MultiplayerGameplay::IsMultiplayer();
    localSeat = multiplayer ? MultiplayerGameplay::GetLocalPlayerSlot() : 0;
    if (localSeat >= TH06_MULTI_MAX_PLAYERS)
        return words.data();
    localPlayer = &g_Players[localSeat];
    if (multiplayer && !IsPlayerGameplayActive(static_cast<u8>(localSeat)))
        localPlayer = nullptr;
    if (multiplayer && MultiplayerGameplay::IsPlayerPermanentlyDeparted(static_cast<u8>(localSeat)))
        localPlayer = nullptr;
#else
    localPlayer = &g_Player;
#endif

    words[1] |= kLocalSeat;
    words[11] = localSeat;
    if (localPlayer && (words[2] == kGameplay || words[2] == kPaused) &&
        std::isfinite(localPlayer->positionCenter.x) && std::isfinite(localPlayer->positionCenter.y))
    {
        StoreFloat(words[8], localPlayer->positionCenter.x);
        StoreFloat(words[9], localPlayer->positionCenter.y);
        words[1] |= kPosition;
    }

    if (words[2] == kGameplay)
    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        words[3] = g_CurFrameGameInputs[localSeat];
#else
        words[3] = g_CurFrameInput;
#endif
        words[1] |= kInput;
    }

    return words.data();
}
