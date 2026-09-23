#include "NetplayRuntime.hpp"
#include "InputLanes.hpp"

namespace th10::multiplayer {

bool NetplayRuntime::Reset(const SessionSetup& setup, std::uint64_t sessionId) noexcept {
    Clear();
    if (!setup.configured || setup.playerCount < 2 || setup.playerCount > 3 ||
        setup.localPlayer >= setup.playerCount || sessionId == 0) {
        return false;
    }

    Netplay::SessionConfig session{};
    session.sessionId = sessionId;
    session.seed = setup.seed;
    session.gameplayAbi = GameplayContract(setup);
    session.gameId = 10;
    session.playerCount = static_cast<std::uint8_t>(setup.playerCount);
    session.localPlayer = static_cast<std::uint8_t>(setup.localPlayer);
    if (!gate_.Reset(session)) {
        Clear();
        return false;
    }

    Netplay::CoreConfig core{};
    core.sessionId = sessionId;
    core.playerCount = session.playerCount;
    core.localPlayer = session.localPlayer;
    core.inputDelay = 0;
    core.maxRollbackFrames = 12;
    // Match the proven TH07 production policy: movement/focus/shoot may be
    // predicted, while Bomb and Pause remain exact edge actions.
    core.predictableButtons = InputLanes::kShoot | InputLanes::kFocus |
                              InputLanes::kDirection;
    core.directionButtons = InputLanes::kDirection;
    core.maxDirectionPredictionFrames = 3;
    core.maxDirectTouchDeltaPredictionFrames = 0;
    if (!core_.Reset(core)) {
        Clear();
        return false;
    }
    configured_ = true;
    return true;
}

void NetplayRuntime::Clear() noexcept {
    gate_.Clear();
    core_.Clear();
    configured_ = false;
}

} // namespace th10::multiplayer
