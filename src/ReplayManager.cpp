#include <array>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <new>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "AsciiManager.hpp"
#include "Controller.hpp"
#include "EaglerOptions.hpp"
#include "FileSystem.hpp"
#include "GameManager.hpp"
#include "Gui.hpp"
#include "PracticeRuntime.hpp"
#include "ReplayExtension.hpp"
#include "ReplayManager.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "Multiplayer.hpp"
#include "multiplayer/GameplaySession.hpp"
#endif
#ifdef TH_ENABLE_NETPLAY
#include "netplay/NetplayInput.hpp"
#include "netplay/NetplaySideEffects.hpp"
#endif
#include "Rng.hpp"
#include "Supervisor.hpp"
#include "Touch.hpp"
#include "utils.hpp"

ReplayManager *g_ReplayManager;

static i32 ReplayGameplayDataSize(const u8 *bytes, i32 fileSize)
{
    if (bytes == nullptr || fileSize < static_cast<i32>(sizeof(ReplayHeader)))
        return fileSize;

    const size_t baseSize = ReplayExtension::BaseFileSize(bytes, static_cast<size_t>(fileSize));
    if (baseSize < sizeof(ReplayHeader) + 8 || std::memcmp(bytes, "T6RP", 4) != 0 ||
        std::memcmp(bytes + baseSize - 4, "PRAC", 4) != 0)
        return static_cast<i32>(baseSize);

    const u32 payloadSize = static_cast<u32>(bytes[baseSize - 8]) |
                            (static_cast<u32>(bytes[baseSize - 7]) << 8) |
                            (static_cast<u32>(bytes[baseSize - 6]) << 16) |
                            (static_cast<u32>(bytes[baseSize - 5]) << 24);
    // ReplayLoadParam in thprac v2.3.0.3 accepts TH06 payloads below 512 B.
    if (payloadSize == 0 || payloadSize >= 512 || payloadSize > baseSize - 8 - sizeof(ReplayHeader))
        return static_cast<i32>(baseSize);
    return static_cast<i32>(baseSize - 8 - payloadSize);
}

ZunResult ReplayManager::ValidateReplayData(ReplayHeader *data, i32 fileSize)
{
    u8 *checksumCursor;
    u32 checksum;
    u8 *obfuscateCursor;
    u8 obfOffset;
    i32 idx;

    if (data == NULL || fileSize < static_cast<i32>(sizeof(ReplayHeader)))
    {
        return ZUN_ERROR;
    }

    // Detect thprac's raw PRAC trailer before deobfuscation mutates the
    // in-memory bytes. Checksum/deobfuscation still cover the complete file,
    // exactly like upstream ReplaySaveParam; only stage-record bounds stop at
    // the end of the original replay payload rather than treating JSON as
    // ReplayDataInput records.
    const i32 gameplayDataSize = ReplayGameplayDataSize(reinterpret_cast<const u8 *>(data), fileSize);

    /* "T6RP" magic bytes */
    if (std::memcmp(data->magic, "T6RP", 4) != 0)
    {
        return ZUN_ERROR;
    }

    /* Deobfuscate the replay decryptedData */
    obfuscateCursor = (u8 *)&data->rngValue3;
    obfOffset = data->key;
    for (idx = 0; idx < fileSize - (i32)offsetof(ReplayHeader, rngValue3); idx += 1, obfuscateCursor += 1)
    {
        *obfuscateCursor -= obfOffset;
        obfOffset += 7;
    }

    /* Calculate the checksum */
    /* (0x3f000318 + key + sum(c for c in decryptedData)) % (2 ** 32) */
    checksumCursor = (u8 *)&data->key;
    checksum = 0x3f000318;
    for (idx = 0; idx < fileSize - (i32)offsetof(ReplayHeader, key); idx += 1, checksumCursor += 1)
    {
        checksum += *checksumCursor;
    }

    if (checksum != data->checksum)
    {
        return ZUN_ERROR;
    }

    if (data->version != GAME_VERSION)
    {
        return ZUN_ERROR;
    }

    if (data->shottypeChara >= 4 || data->difficulty >= 5)
    {
        return ZUN_ERROR;
    }

    u32 previousOffset = 0;
    bool foundStage = false;
    for (i32 stage = 0; stage < ARRAY_SIZE_SIGNED(data->stageReplayDataOffsets); ++stage)
    {
        const u32 offset = data->stageReplayDataOffsets[stage];
        if (offset == 0)
        {
            continue;
        }
        foundStage = true;
        if (offset < sizeof(ReplayHeader) || offset <= previousOffset ||
            offset > static_cast<u32>(gameplayDataSize) - offsetof(StageReplayData, replayInputs) - sizeof(ReplayDataInput))
        {
            return ZUN_ERROR;
        }

        u32 nextOffset = static_cast<u32>(gameplayDataSize);
        for (i32 nextStage = stage + 1; nextStage < ARRAY_SIZE_SIGNED(data->stageReplayDataOffsets); ++nextStage)
        {
            if (data->stageReplayDataOffsets[nextStage] != 0)
            {
                nextOffset = data->stageReplayDataOffsets[nextStage];
                break;
            }
        }
        if (nextOffset <= offset + offsetof(StageReplayData, replayInputs) ||
            nextOffset > static_cast<u32>(gameplayDataSize) ||
            (nextOffset - offset - offsetof(StageReplayData, replayInputs)) % sizeof(ReplayDataInput) != 0)
        {
            return ZUN_ERROR;
        }

        const ReplayDataInput *input = reinterpret_cast<const StageReplayData *>(
                                           reinterpret_cast<const u8 *>(data) + offset)
                                           ->replayInputs;
        const ReplayDataInput *inputEnd = reinterpret_cast<const ReplayDataInput *>(
            reinterpret_cast<const u8 *>(data) + nextOffset);
        bool foundTerminator = false;
        for (; input < inputEnd; ++input)
        {
            if (input->frameNum == 9999999)
            {
                foundTerminator = true;
                break;
            }
        }
        if (!foundTerminator)
        {
            return ZUN_ERROR;
        }
        previousOffset = offset;
    }

    return foundStage ? ZUN_SUCCESS : ZUN_ERROR;
}

ZunResult ReplayManager::RegisterChain(i32 isDemo, const char *replayFile)
{
    ReplayManager *replayMgr;

    if (g_Supervisor.framerateMultiplier < 0.99f && !isDemo)
    {
        return ZUN_SUCCESS;
    }
    g_Supervisor.framerateMultiplier = 1.0f;
    if (g_ReplayManager == NULL)
    {
        replayMgr = new ReplayManager();
        g_ReplayManager = replayMgr;
        replayMgr->replayData = NULL;
        replayMgr->isDemo = isDemo;
        replayMgr->replayFile = replayFile;
        replayMgr->replayFileSize = 0;
        switch (isDemo)
        {
        case false:
            replayMgr->calcChain = g_Chain.CreateElem((ChainCallback)ReplayManager::OnUpdate);
            replayMgr->calcChain->addedCallback = (ChainAddedCallback)AddedCallback;
            replayMgr->calcChain->deletedCallback = (ChainDeletedCallback)DeletedCallback;
            replayMgr->drawChain = g_Chain.CreateElem((ChainCallback)ReplayManager::OnDraw);
            replayMgr->calcChain->arg = replayMgr;
            if (g_Chain.AddToCalcChain(replayMgr->calcChain, TH_CHAIN_PRIO_CALC_REPLAYMANAGER))
            {
                return ZUN_ERROR;
            }
            replayMgr->calcChainDemoHighPrio = NULL;
            break;
        case true:
            replayMgr->calcChain = g_Chain.CreateElem((ChainCallback)ReplayManager::OnUpdateDemoHighPrio);
            replayMgr->calcChain->addedCallback = (ChainAddedCallback)AddedCallbackDemo;
            replayMgr->calcChain->deletedCallback = (ChainDeletedCallback)DeletedCallback;
            replayMgr->drawChain = g_Chain.CreateElem((ChainCallback)ReplayManager::OnDraw);
            replayMgr->calcChain->arg = replayMgr;
            if (g_Chain.AddToCalcChain(replayMgr->calcChain, TH_CHAIN_PRIO_CALC_LOW_PRIO_REPLAYMANAGER_DEMO))
            {
                return ZUN_ERROR;
            }
            replayMgr->calcChainDemoHighPrio = g_Chain.CreateElem((ChainCallback)ReplayManager::OnUpdateDemoLowPrio);
            replayMgr->calcChainDemoHighPrio->arg = replayMgr;
            g_Chain.AddToCalcChain(replayMgr->calcChainDemoHighPrio, TH_CHAIN_PRIO_CALC_HIGH_PRIO_REPLAYMANAGER_DEMO);
            break;
        }
        replayMgr->drawChain->arg = replayMgr;
        g_Chain.AddToDrawChain(replayMgr->drawChain, TH_CHAIN_PRIO_DRAW_REPLAYMANAGER);
    }
    else
    {
        switch (isDemo)
        {
        case false:
            AddedCallback(g_ReplayManager);
            break;
        case true:
            return AddedCallbackDemo(g_ReplayManager);
            break;
        }
    }
    return ZUN_SUCCESS;
}

#define TH_BUTTON_REPLAY_CAPTURE                                                                                       \
    (TH_BUTTON_SHOOT | TH_BUTTON_BOMB | TH_BUTTON_FOCUS | TH_BUTTON_SKIP | TH_BUTTON_DIRECTION)

static void ApplyReplayExtensionFrame()
{
    bool usedThisRun = false;
    bool bombedWithTouch = false;
    bool cheatMovementUsed = false;
    if (ReplayExtension::GetPlaybackTouchState(&usedThisRun, &bombedWithTouch, &cheatMovementUsed))
        Touch::SetReplayUsageState(usedThisRun, bombedWithTouch, cheatMovementUsed);

    Touch::BeginReplayTouchFrame();
    std::size_t eventCount = 0;
    const ReplayExtension::TouchEvent *events = ReplayExtension::GetPlaybackTouchEvents(&eventCount);
    for (std::size_t index = 0; index < eventCount; ++index)
    {
        const ReplayExtension::TouchEvent &event = events[index];
        Touch::ApplyReplayTouchEvent(event.fingerId, event.x, event.y, event.action, event.role, event.flags);
    }

    Touch::ReplayTouchPoint points[10];
    const i32 pointCount = Touch::GetReplayTouchPoints(points, ARRAY_SIZE_SIGNED(points));
    if (pointCount == 0)
        return;

    const ZunColor oldColor = g_AsciiManager.color;
    const ZunVec2 oldScale = g_AsciiManager.scale;
    const u32 oldIsGui = g_AsciiManager.isGui;
    const bool oldIsSelected = g_AsciiManager.isSelected;
    g_AsciiManager.color = COLOR_WHITE;
    g_AsciiManager.scale = {0.7f, 0.7f};
    g_AsciiManager.isGui = 0;
    g_AsciiManager.isSelected = false;
    for (i32 index = 0; index < pointCount; ++index)
    {
        ZunVec3 position = {points[index].x - 5.0f, points[index].y - 7.0f, 0.0f};
        g_AsciiManager.AddString(&position, "+");
    }
    g_AsciiManager.color = oldColor;
    g_AsciiManager.scale = oldScale;
    g_AsciiManager.isGui = oldIsGui;
    g_AsciiManager.isSelected = oldIsSelected;
}

ChainCallbackResult ReplayManager::OnUpdate(ReplayManager *mgr)
{
    u16 inputs;

    if (!g_GameManager.isInMenu)
    {
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }
#ifdef TH_ENABLE_NETPLAY
    // Rollback resimulation must reproduce gameplay state but must not append
    // the same logical frame to the persistent replay stream a second time.
    if (Netplay::SideEffects::IsSpeculative())
        return CHAIN_CALLBACK_RESULT_CONTINUE;
#endif
    inputs = IS_PRESSED(TH_BUTTON_REPLAY_CAPTURE);
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY) && defined(TH_ENABLE_NETPLAY)
    if (MultiplayerGameplay::IsMultiplayer())
    {
        // The vanilla TH06 replay stream remains the P1 compatibility lane.
        // The full synchronized input of every participant is stored in the
        // EAGX v1 multiplayer sidecar below.
        inputs = g_CurFrameGameInputs[0] & TH_BUTTON_REPLAY_CAPTURE;
        std::array<Netplay::FrameInput, TH06_MULTI_MAX_PLAYERS> multiplayerInputs{};
        for (i32 playerId = 0; playerId < MultiplayerGameplay::GetPlayerCount(); ++playerId)
        {
            if (!Netplay::Input::PlayerInputOverride(playerId, &multiplayerInputs[playerId]))
                multiplayerInputs[playerId].buttons = g_CurFrameGameInputs[playerId];
        }
        ReplayExtension::RecordMultiplayerFrame(g_GameManager.currentStage - 1, mgr->frameId,
                                                multiplayerInputs.data(),
                                                MultiplayerGameplay::GetPlayerCount());
    }
#endif
    ReplayExtension::CaptureTouchState(Touch::WasUsedThisRun(), Touch::UsedTouchToBomb(),
                                       Touch::UsedCheatMovementThisRun());
    ReplayExtension::RecordFrame(g_GameManager.currentStage - 1, mgr->frameId);
    if (inputs != mgr->replayInputs->inputKey)
    {
        if (mgr->replayInputs + 2 >= mgr->replayInputEnd)
        {
            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }
        mgr->replayInputs += 1;
        mgr->replayInputStageBookmarks[g_GameManager.currentStage - 1] = mgr->replayInputs + 1;
        mgr->replayInputs->frameNum = mgr->frameId;
        mgr->replayInputs->inputKey = inputs;
    }
    mgr->frameId += 1;
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult ReplayManager::OnUpdateDemoLowPrio(ReplayManager *mgr)
{
    if (!g_GameManager.isInMenu)
    {
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }
    if (g_Gui.HasCurrentMsgIdx() && g_Gui.IsDialogueSkippable() && mgr->frameId % 3 != 2)
    {
        return CHAIN_CALLBACK_RESULT_RESTART_FROM_FIRST_JOB;
    }
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult ReplayManager::OnUpdateDemoHighPrio(ReplayManager *mgr)
{
    if (!g_GameManager.isInMenu)
    {
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }

    const u16 liveNonReplayInput = IS_PRESSED(0xFFFFFFFF & ~TH_BUTTON_REPLAY_CAPTURE);
    while (mgr->replayInputs + 1 < mgr->replayInputEnd &&
           mgr->frameId >= mgr->replayInputs[1].frameNum && mgr->replayInputs[1].frameNum != 9999999)
    {
        mgr->replayInputs += 1;
    }
    ReplayExtension::SetPlaybackFrame(g_GameManager.currentStage - 1, mgr->frameId);
    ApplyReplayExtensionFrame();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    std::array<Netplay::FrameInput, TH06_MULTI_MAX_PLAYERS> multiplayerInputs{};
    if (ReplayExtension::GetMultiplayerPlaybackFrame(
            g_GameManager.currentStage - 1, mgr->frameId,
            multiplayerInputs.data(), multiplayerInputs.size()))
    {
        u16 combinedButtons = 0;
        const u8 replayPlayerCount = MultiplayerGameplay::GetPlayerCount();
        bool independentInputs = false;
        for (u8 playerId = 0; playerId < replayPlayerCount; ++playerId)
        {
            combinedButtons = static_cast<u16>(combinedButtons | multiplayerInputs[playerId].buttons);
            if (playerId != 0 && multiplayerInputs[playerId].buttons != multiplayerInputs[0].buttons)
                independentInputs = true;
        }
#if defined(__EMSCRIPTEN__) && defined(TH_DEV_TOOLS)
        const int auditResult = ReplayExtension::DebugAuditMultiplayerPlaybackFrame(
            g_GameManager.currentStage - 1, mgr->frameId,
            multiplayerInputs.data(), replayPlayerCount);
#endif
#ifdef __EMSCRIPTEN__
        if (EaglerOptions::ReplayViewerEnabled())
        {
            EM_ASM({
                globalThis.__eaglerNetplayReplayPlaybackObserved = true;
                globalThis.__eaglerNetplayReplayPlaybackFrame = $0;
                globalThis.__eaglerNetplayReplayPlaybackPlayerCount = $1;
                globalThis.__eaglerNetplayReplayPlaybackIndependentInputs =
                    !!globalThis.__eaglerNetplayReplayPlaybackIndependentInputs || !!$2;
                globalThis.__eaglerNetplayReplayPlaybackInput0 = $3;
                globalThis.__eaglerNetplayReplayPlaybackInput1 = $4;
                globalThis.__eaglerNetplayReplayPlaybackInput2 = $5;
                globalThis.__eaglerNetplayReplayPlaybackStage = $6;
            }, mgr->frameId, static_cast<int>(replayPlayerCount),
               independentInputs ? 1 : 0,
               static_cast<unsigned>(multiplayerInputs[0].buttons),
               static_cast<unsigned>(multiplayerInputs[1].buttons),
               static_cast<unsigned>(multiplayerInputs[2].buttons),
               g_GameManager.currentStage);
#ifdef TH_DEV_TOOLS
            if (auditResult >= 0)
            {
                EM_ASM({
                    globalThis.__eaglerNetplayReplayComparedFrames = $0;
                    globalThis.__eaglerNetplayReplayInputMismatch = !!$1;
                }, ReplayExtension::DebugComparedMultiplayerPlaybackFrames(),
                   ReplayExtension::DebugMultiplayerPlaybackMismatch() ? 1 : 0);
            }
#endif
        }
#endif
#ifdef TH_ENABLE_NETPLAY
        Netplay::Input::SetPlayerInputOverrides(multiplayerInputs.data(),
                                                MultiplayerGameplay::GetPlayerCount());
#else
        for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
        {
            g_LastFrameGameInputs[playerId] = g_CurFrameGameInputs[playerId];
            g_CurFrameGameInputs[playerId] = multiplayerInputs[playerId].buttons;
        }
#endif
        g_CurFrameInput = liveNonReplayInput | combinedButtons;
    }
    else
    {
        g_CurFrameInput = liveNonReplayInput | mgr->replayInputs->inputKey;
        for (u8 playerId = 1; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
        {
            g_LastFrameGameInputs[playerId] = g_CurFrameGameInputs[playerId];
            g_CurFrameGameInputs[playerId] = 0;
        }
    }
#else
    g_CurFrameInput = liveNonReplayInput | mgr->replayInputs->inputKey;
#endif
#ifdef __EMSCRIPTEN__
    if (EaglerOptions::ReplayViewerEnabled() && !ReplayExtension::MultiplayerPlaybackActive())
    {
        EM_ASM({
            globalThis.__eaglerOrdinaryReplayPlaybackObserved = true;
            globalThis.__eaglerOrdinaryReplayPlaybackFrame = $0;
            globalThis.__eaglerOrdinaryReplayPlaybackInput = $1;
            globalThis.__eaglerOrdinaryReplayPlaybackStage = $2;
        }, mgr->frameId, static_cast<unsigned>(mgr->replayInputs->inputKey),
           g_GameManager.currentStage);
    }
#endif
    g_IsEigthFrameOfHeldInput = 0;
    if (g_LastFrameInput == g_CurFrameInput)
    {
        if (30 <= g_NumOfFramesInputsWereHeld)
        {
            if (g_NumOfFramesInputsWereHeld % 8 == 0)
            {
                g_IsEigthFrameOfHeldInput = 1;
            }
            if (38 <= g_NumOfFramesInputsWereHeld)
            {
                g_NumOfFramesInputsWereHeld = 30;
            }
        }
        g_NumOfFramesInputsWereHeld++;
    }
    else
    {
        g_NumOfFramesInputsWereHeld = 0;
    }
    mgr->frameId += 1;
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult ReplayManager::OnDraw(ReplayManager *mgr)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

inline StageReplayData *AllocateStageReplayData(i32 size)
{
    return (StageReplayData *)std::malloc(size);
}

inline void ReleaseReplayData(void *data)
{
    return std::free(data);
}

inline void ReleaseStageReplayData(void *data)
{
    return std::free(data);
}

ZunResult ReplayManager::AddedCallback(ReplayManager *mgr)
{
    StageReplayData *stageReplayData;
    StageReplayData *oldStageReplayData;
    i32 idx;

    mgr->frameId = 0;
    if (mgr->replayData == NULL)
    {
        ReplayExtension::ClearPlayback();
        ReplayExtension::ResetRecording();
        mgr->replayData = new (std::nothrow) ReplayData{};
        if (mgr->replayData == NULL)
        {
            return ZUN_ERROR;
        }
        mgr->replayData->header = (ReplayHeader *)std::calloc(1, sizeof(ReplayHeader));
        if (mgr->replayData->header == NULL)
        {
            delete mgr->replayData;
            mgr->replayData = NULL;
            return ZUN_ERROR;
        }
        std::memcpy(&mgr->replayData->header->magic[0], "T6RP", 4);
        mgr->replayData->header->shottypeChara = g_GameManager.character * 2 + g_GameManager.shotType;
        mgr->replayData->header->version = 0x102;
        mgr->replayData->header->difficulty = g_GameManager.difficulty;
        std::memcpy(&mgr->replayData->header->name, "NO NAME", 4);
        for (idx = 0; idx < ARRAY_SIZE_SIGNED(mgr->replayData->stageReplayData); idx += 1)
        {
            mgr->replayData->stageReplayData[idx] = NULL;
        }
    }
    else
    {
        oldStageReplayData = mgr->replayData->stageReplayData[g_GameManager.currentStage - 2];
        if (oldStageReplayData == NULL)
        {
            return ZUN_ERROR;
        }
        oldStageReplayData->score = g_GameManager.score;
    }
    if (mgr->replayData->stageReplayData[g_GameManager.currentStage - 1] != NULL)
    {
        utils::DebugPrint2("error : replay.cpp");
    }
    ReplayExtension::BeginStageRecording(g_GameManager.currentStage - 1);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
    {
        ReplayExtension::MultiplayerReplayConfig config;
        config.playerCount = MultiplayerGameplay::GetPlayerCount();
        config.difficulty = static_cast<u8>(g_GameManager.difficulty);
        config.localPlayer = MultiplayerGameplay::GetLocalPlayerSlot();
        config.showContributionStats = MultiplayerGameplay::ShouldShowContributionStats();
        config.gameplayAbi = TH06_MULTI_GAMEPLAY_ABI;
        for (u8 playerId = 0; playerId < config.playerCount; ++playerId)
        {
            config.characters[playerId] = MultiplayerGameplay::GetPlayerCharacter(playerId);
            config.shots[playerId] = MultiplayerGameplay::GetPlayerShot(playerId);
        }
        ReplayExtension::BeginMultiplayerRecording(config);
        ReplayExtension::MultiplayerPlayerResourceSnapshot resources[TH06_MULTI_MAX_PLAYERS]{};
        for (u8 playerId = 0; playerId < config.playerCount; ++playerId)
        {
            resources[playerId].lives = GetPlayerLives(playerId);
            resources[playerId].bombs = GetPlayerBombs(playerId);
            resources[playerId].power = GetPlayerPower(playerId);
        }
        ReplayExtension::CaptureMultiplayerStageResources(
            g_GameManager.currentStage - 1, resources, config.playerCount);
        ReplayExtension::MultiplayerContributionSnapshot
            contributions[TH06_MULTI_MAX_PLAYERS]{};
        for (u8 playerId = 0; playerId < config.playerCount; ++playerId)
        {
            contributions[playerId].enemiesDefeated = GetPlayerEnemiesDefeated(playerId);
            contributions[playerId].damageDealt = GetPlayerDamageDealt(playerId);
        }
        ReplayExtension::CaptureMultiplayerStageContributions(
            g_GameManager.currentStage - 1, contributions, config.playerCount);
    }
#endif
    mgr->replayData->stageReplayData[g_GameManager.currentStage - 1] = AllocateStageReplayData(sizeof(StageReplayData));
    stageReplayData = mgr->replayData->stageReplayData[g_GameManager.currentStage - 1];
    if (stageReplayData == NULL)
    {
        return ZUN_ERROR;
    }
    std::memset(stageReplayData, 0, sizeof(*stageReplayData));
    stageReplayData->bombsRemaining = g_GameManager.bombsRemaining;
    stageReplayData->livesRemaining = g_GameManager.livesRemaining;
    stageReplayData->power = g_GameManager.currentPower;
    stageReplayData->rank = g_GameManager.rank;
    stageReplayData->pointItemsCollected = g_GameManager.pointItemsCollected;
    stageReplayData->randomSeed = g_GameManager.randomSeed;
    stageReplayData->powerItemCountForScore = g_GameManager.powerItemCountForScore;
    mgr->replayInputs = stageReplayData->replayInputs;
    mgr->replayInputEnd = stageReplayData->replayInputs + ARRAY_SIZE(stageReplayData->replayInputs);
    mgr->replayInputs->frameNum = 0;
    mgr->replayInputs->inputKey = 0;
    mgr->unk44 = 0;
    return ZUN_SUCCESS;
}

ZunResult ReplayManager::AddedCallbackDemo(ReplayManager *mgr)
{
    i32 idx;
    StageReplayData *replayData;

    mgr->frameId = 0;
    if (mgr->replayData == NULL)
    {
        ReplayExtension::ClearPlayback();
        mgr->replayData = new (std::nothrow) ReplayData{};
        if (mgr->replayData == NULL)
        {
            return ZUN_ERROR;
        }

        mgr->replayData->header = (ReplayHeader *)FileSystem::OpenPath(mgr->replayFile, g_GameManager.demoMode == 0);
        const u32 fileSize = g_LastFileSize;
        mgr->replayFileSize = ReplayGameplayDataSize(reinterpret_cast<const u8 *>(mgr->replayData->header), fileSize);
        if (!ReplayExtension::MatchesPath(mgr->replayFile, reinterpret_cast<const u8 *>(mgr->replayData->header),
                                          fileSize))
        {
            std::free(mgr->replayData->header);
            delete mgr->replayData;
            mgr->replayData = NULL;
            return ZUN_ERROR;
        }
        Touch::ResetReplayTouch();
        ReplayExtension::LoadPlayback(reinterpret_cast<const u8 *>(mgr->replayData->header), fileSize);
        if (ValidateReplayData(mgr->replayData->header, fileSize) != ZUN_SUCCESS)
        {
            std::free(mgr->replayData->header);
            delete mgr->replayData;
            mgr->replayData = NULL;
            return ZUN_ERROR;
        }
        for (idx = 0; idx < ARRAY_SIZE_SIGNED(mgr->replayData->stageReplayData); idx += 1)
        {
            if (mgr->replayData->header->stageReplayDataOffsets[idx] != 0)
            {
                mgr->replayData->stageReplayData[idx] =
                    (StageReplayData *)(((u8 *)mgr->replayData->header) +
                                        mgr->replayData->header->stageReplayDataOffsets[idx]);
            }
            else
            {
                mgr->replayData->stageReplayData[idx] = NULL;
            }
        }
    }
    if (mgr->replayData->stageReplayData[g_GameManager.currentStage - 1] == NULL)
    {
        return ZUN_ERROR;
    }
    replayData = mgr->replayData->stageReplayData[g_GameManager.currentStage - 1];
    g_GameManager.character = mgr->replayData->header->shottypeChara / 2;
    g_GameManager.shotType = mgr->replayData->header->shottypeChara % 2;
    g_GameManager.difficulty = (Difficulty)mgr->replayData->header->difficulty;
    g_GameManager.pointItemsCollected = replayData->pointItemsCollected;
    g_Rng.Initialize(replayData->randomSeed);
    g_GameManager.rank = replayData->rank;
    g_GameManager.livesRemaining = replayData->livesRemaining;
    g_GameManager.bombsRemaining = replayData->bombsRemaining;
    g_GameManager.currentPower = replayData->power;
    mgr->replayInputs = replayData->replayInputs;
    const i32 currentStageIndex = g_GameManager.currentStage - 1;
    u32 nextOffset = mgr->replayFileSize;
    for (i32 nextStage = currentStageIndex + 1; nextStage < ARRAY_SIZE_SIGNED(mgr->replayData->stageReplayData);
         ++nextStage)
    {
        if (mgr->replayData->header->stageReplayDataOffsets[nextStage] != 0)
        {
            nextOffset = mgr->replayData->header->stageReplayDataOffsets[nextStage];
            break;
        }
    }
    mgr->replayInputEnd = reinterpret_cast<const ReplayDataInput *>(
        reinterpret_cast<const u8 *>(mgr->replayData->header) + nextOffset);
    g_GameManager.powerItemCountForScore = replayData->powerItemCountForScore;
    if (2 <= g_GameManager.currentStage && mgr->replayData->stageReplayData[g_GameManager.currentStage - 2] != NULL)
    {
        g_GameManager.guiScore = mgr->replayData->stageReplayData[g_GameManager.currentStage - 2]->score;
        g_GameManager.score = g_GameManager.guiScore;
    }
    return ZUN_SUCCESS;
}

ZunResult ReplayManager::DeletedCallback(ReplayManager *mgr)
{
    Touch::ResetReplayTouch();
    ReplayExtension::ClearPlayback();
    g_Chain.Cut(mgr->drawChain);
    mgr->drawChain = NULL;
    if (mgr->calcChainDemoHighPrio != NULL)
    {
        g_Chain.Cut(mgr->calcChainDemoHighPrio);
        mgr->calcChainDemoHighPrio = NULL;
    }
    if (mgr->replayData != NULL)
    {
        if (!mgr->IsDemo())
        {
            for (StageReplayData *&stage : mgr->replayData->stageReplayData)
            {
                std::free(stage);
                stage = NULL;
            }
        }
        std::free(mgr->replayData->header);
        delete mgr->replayData;
        mgr->replayData = NULL;
    }
    delete g_ReplayManager;
    g_ReplayManager = NULL;
    g_ReplayManager = NULL;
    return ZUN_SUCCESS;
}

void ReplayManager::StopRecording()
{
    ReplayManager *mgr = g_ReplayManager;
    if (mgr != NULL)
    {
        if (mgr->replayInputs == NULL || mgr->replayInputs + 2 >= mgr->replayInputEnd)
        {
            return;
        }
        mgr->replayInputs += 1;
        mgr->replayInputs->frameNum = mgr->frameId;
        mgr->replayInputs->inputKey = 0;
        mgr->replayInputs += 1;
        mgr->replayInputs->frameNum = 9999999;
        mgr->replayInputs->inputKey = 0;
        mgr->replayInputStageBookmarks[g_GameManager.currentStage - 1] = mgr->replayInputs + 1;
    }
}

void ReplayManager::SaveReplay(const char *replayPath, char *replayName)
{
    ReplayManager *mgr;
    SDL_IOStream *file;
    const u8 *checksumCursor;
    ReplayHeader replayCopy;
    u8 *obfuscateCursor;
    i32 obfStagePos;
    u8 obfOffset;
    u32 checksum;
    i32 csumStagePos;
    size_t stageReplayPos;
    f32 slowDown;
    i32 stageIdx;
    std::time_t time;
    const std::tm *tm;

    time = std::time(NULL);
    tm = std::localtime(&time);

    if (g_ReplayManager != NULL)
    {
        mgr = g_ReplayManager;
        if (!mgr->IsDemo())
        {
            if (replayPath != NULL)
            {
                replayCopy = *mgr->replayData->header;
                ReplayManager::StopRecording();
                stageReplayPos = sizeof(ReplayHeader);
                for (stageIdx = 0; stageIdx < ARRAY_SIZE_SIGNED(g_ReplayManager->replayData->stageReplayData);
                     stageIdx += 1)
                {
                    if (mgr->replayData->stageReplayData[stageIdx] != NULL)
                    {
                        replayCopy.stageReplayDataOffsets[stageIdx] = (u32)stageReplayPos;
                        stageReplayPos += (size_t)((u8 *)mgr->replayInputStageBookmarks[stageIdx] -
                                                   (u8 *)mgr->replayData->stageReplayData[stageIdx]);
                    }
                }
                const std::string actualReplayPath = ReplayExtension::ResolveSavePath(replayPath);
                utils::DebugPrint2("%s write ...\n", actualReplayPath.c_str());
                replayCopy.score = g_GameManager.guiScore;
                slowDown = (g_Supervisor.unk1b4 / g_Supervisor.unk1b8 - 0.5f) * 2.0f;
                if (slowDown < 0.0f)
                {
                    slowDown = 0.0f;
                }
                else if (slowDown >= 1.0f)
                {
                    slowDown = 1.0f;
                }
                replayCopy.slowdownRate = Touch::UsedCheatMovementThisRun()
                                              ? 100.0f
                                              : (1.0f - slowDown) * 100.0f;
                replayCopy.slowdownRate2 = replayCopy.slowdownRate + 1.12f;
                replayCopy.slowdownRate3 = replayCopy.slowdownRate + 2.34f;
                mgr->replayData->stageReplayData[g_GameManager.currentStage - 1]->score = g_GameManager.score;
                std::snprintf(replayCopy.name, sizeof(replayCopy.name), "%s", replayName != NULL ? replayName : "");
                std::sprintf(replayCopy.date, "%02i/%02i/%02i", tm->tm_mon, tm->tm_mday, tm->tm_year % 100);
                replayCopy.key = g_Rng.GetRandomU16InRange(128) + 64;
                replayCopy.rngValue3 = g_Rng.GetRandomU16InRange(256);
                replayCopy.rngValue1 = g_Rng.GetRandomU16InRange(256);
                replayCopy.rngValue2 = g_Rng.GetRandomU16InRange(256);

                // Calculate the checksum.
                checksumCursor = (u8 *)&replayCopy.key;
                checksum = 0x3f000318;
                for (stageIdx = 0; stageIdx < sizeof(ReplayHeader) - offsetof(ReplayHeader, key);
                     stageIdx += 1, checksumCursor += 1)
                {
                    checksum += *checksumCursor;
                }
                for (stageIdx = 0; stageIdx < ARRAY_SIZE_SIGNED(mgr->replayData->stageReplayData); stageIdx += 1)
                {
                    if (mgr->replayData->stageReplayData[stageIdx] != NULL)
                    {
                        checksumCursor = (u8 *)mgr->replayData->stageReplayData[stageIdx];
                        for (csumStagePos = 0; csumStagePos < ((iptr)mgr->replayInputStageBookmarks[stageIdx]) -
                                                                  ((iptr)mgr->replayData->stageReplayData[stageIdx]);
                             csumStagePos += 1, checksumCursor += 1)
                        {
                            checksum += *checksumCursor;
                        }
                    }
                }
                replayCopy.checksum = checksum;

                // Obfuscate the data.
                obfuscateCursor = (u8 *)&replayCopy.rngValue3;
                obfOffset = replayCopy.key;
                for (stageIdx = 0; stageIdx < sizeof(ReplayHeader) - offsetof(ReplayHeader, rngValue3);
                     stageIdx += 1, obfuscateCursor += 1)
                {
                    *obfuscateCursor += obfOffset;
                    obfOffset += 7;
                }
                for (stageIdx = 0; stageIdx < ARRAY_SIZE_SIGNED(mgr->replayData->stageReplayData); stageIdx += 1)
                {
                    if (mgr->replayData->stageReplayData[stageIdx] != NULL)
                    {
                        obfuscateCursor = (u8 *)mgr->replayData->stageReplayData[stageIdx];
                        for (obfStagePos = 0; obfStagePos < ((iptr)mgr->replayInputStageBookmarks[stageIdx]) -
                                                                ((iptr)mgr->replayData->stageReplayData[stageIdx]);
                             obfStagePos += 1, obfuscateCursor += 1)
                        {
                            *obfuscateCursor += obfOffset;
                            obfOffset += 7;
                        }
                    }
                }

                // Write the data to the replay file.
                file = FileSystem::OpenFileStream(actualReplayPath.c_str(), "wb");
                if (file == NULL)
                {
                    return;
                }
                if (SDL_WriteIO(file, &replayCopy, sizeof(ReplayHeader)) != sizeof(ReplayHeader))
                {
                    SDL_CloseIO(file);
                    return;
                }
                for (stageIdx = 0; stageIdx < ARRAY_SIZE_SIGNED(mgr->replayData->stageReplayData); stageIdx += 1)
                {
                    if (mgr->replayData->stageReplayData[stageIdx] != NULL)
                    {
                        const size_t replaySize = ((iptr)mgr->replayInputStageBookmarks[stageIdx]) -
                                                  ((iptr)mgr->replayData->stageReplayData[stageIdx]);
                        if (SDL_WriteIO(file, mgr->replayData->stageReplayData[stageIdx], replaySize) != replaySize)
                        {
                            SDL_CloseIO(file);
                            return;
                        }
                    }
                }
                SDL_CloseIO(file);
                PracticeRuntime::SaveReplayMetadata(actualReplayPath.c_str());
                if (ReplayExtension::AppendRecording(actualReplayPath.c_str()))
                {
                    ReplayExtension::RemoveAlternateSave(actualReplayPath.c_str());
#if defined(__EMSCRIPTEN__) && defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
                    if (MultiplayerGameplay::IsMultiplayer())
                    {
                        const std::string storagePath = FileSystem::GetPrefPath(actualReplayPath.c_str());
                        EM_ASM({
                            globalThis.__eaglerNetplayReplaySaved = true;
                            globalThis.__eaglerNetplayReplaySavedPath = UTF8ToString($0);
                            globalThis.__eaglerNetplayReplaySavedStoragePath = UTF8ToString($1);
                        }, actualReplayPath.c_str(), storagePath.c_str());
                    }
#endif
                }
            }
            for (stageIdx = 0; stageIdx < ARRAY_SIZE_SIGNED(mgr->replayData->stageReplayData); stageIdx += 1)
            {
                if (g_ReplayManager->replayData->stageReplayData[stageIdx] != NULL)
                {
                    utils::DebugPrint2("Replay Size %d\n", ((iptr)mgr->replayInputStageBookmarks[stageIdx]) -
                                                               ((iptr)mgr->replayData->stageReplayData[stageIdx]));
                    ReleaseStageReplayData(g_ReplayManager->replayData->stageReplayData[stageIdx]);
                    g_ReplayManager->replayData->stageReplayData[stageIdx] = NULL;
                }
            }
        }
        g_Chain.Cut(g_ReplayManager->calcChain);
    }
    return;
}
