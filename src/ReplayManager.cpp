#include "ReplayManager.hpp"
#ifdef TH_ENABLE_NETPLAY
#include <eagler/netplay/NetplayInput.hpp>
#include "netplay/NetplaySideEffects.hpp"
#include "netplay/Th07DeterminismProbe.hpp"
#endif

#include "AsciiManager.hpp"
#include "Chain.hpp"
#include "EffectManager.hpp"
#include "EaglerOptions.hpp"
#include "EnemyManager.hpp"
#include "FileSystem.hpp"
#include "GameManager.hpp"
#include "Gui.hpp"
#include "Player.hpp"
#include "PracticeRuntime.hpp"
#include "ReplayExtension.hpp"
#include "Rng.hpp"
#include "Supervisor.hpp"
#include "Touch.hpp"
#include "dxutil.hpp"
#include "pbg4/Lzss.hpp"
#include <algorithm>
#include <array>
#include <cstdio>
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "multiplayer/GameplaySession.hpp"
#endif

ReplayManager *g_ReplayManager;

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
    const i32 pointCount = Touch::GetReplayTouchPoints(points, 10);
    if (pointCount == 0)
        return;

    const u32 oldColor = g_AsciiManager.color;
    const Float2 oldScale = g_AsciiManager.scale;
    const i32 oldIsGui = g_AsciiManager.isGui;
    const i32 oldIsSelected = g_AsciiManager.isSelected;
    g_AsciiManager.color = 0xffffffff;
    g_AsciiManager.scale = {0.7f, 0.7f};
    g_AsciiManager.isGui = 0;
    g_AsciiManager.isSelected = 0;
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

u32 ReplayManager::OnUpdateRng(ReplayManager *arg)
{
    arg->replayEventFlags = 0;
    arg->rngSeed = g_Rng.seed;
    g_Rng.generationCount = 0;
    if (g_GameManager.isPaused)
    {
        arg->replayEventFlags |= 256;
    }
    g_GameManager.isPaused = FALSE;
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

u32 ReplayManager::OnUpdate(ReplayManager *arg)
{
    u16 curInput;
    i32 stage;

    if (!g_GameManager.notInMenu)
    {
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#ifdef TH_ENABLE_NETPLAY
    // Netplay installs current/previous logical lanes before the simulation
    // chain starts.  Do not advance them a second time here or edge-triggered
    // player actions would disappear.  The fallback below remains the exact
    // non-netplay multiplayer behavior.
    if (!Netplay::Input::PlayerButtonOverridesActive())
    {
#endif
        for (i32 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
            g_LastFrameGameInputs[playerId] = g_CurFrameGameInputs[playerId];
        g_CurFrameGameInputs[0] = g_CurFrameRawInput;
        for (i32 playerId = 1; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
            g_CurFrameGameInputs[playerId] = 0;
#ifdef TH_ENABLE_NETPLAY
    }
#endif
#else
    g_LastFrameGameInput = g_CurFrameGameInput;
    g_CurFrameGameInput = g_CurFrameRawInput;
#endif
    if (g_GameManager.defaultCfg->slowMode)
    {
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }
    if (g_Supervisor.timingBad)
    {
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }

#ifdef TH_ENABLE_NETPLAY
    // The first pass of a rollback probe/prediction still needs to produce
    // g_CurFrameGameInput for downstream game logic, but must not advance or
    // write the persistent replay stream. The committed replay pass records it
    // once after state restoration.
    if (Netplay::SideEffects::IsSpeculative())
        return CHAIN_CALLBACK_RESULT_CONTINUE;
#endif

    stage = g_GameManager.currentStage - 1;
    if (stage >= REPLAY_STAGE_COUNT) // PHANTASMSTAGE
    {
        stage = 6; // EXTRASTAGE
    }
    curInput = g_CurFrameGameInput;
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY) && defined(TH_ENABLE_NETPLAY)
    if (MultiplayerGameplay::IsMultiplayer())
    {
        std::array<Netplay::FrameInput, TH07_MULTI_MAX_PLAYERS> inputs{};
        for (i32 playerId = 0; playerId < MultiplayerGameplay::GetPlayerCount(); ++playerId)
        {
            if (!Netplay::Input::PlayerInputOverride(playerId, &inputs[playerId]))
                inputs[playerId].buttons = g_CurFrameGameInputs[playerId];
        }
        ReplayExtension::RecordMultiplayerFrame(stage, arg->frameId, inputs.data(),
                                                MultiplayerGameplay::GetPlayerCount());
    }
#endif
    ReplayExtension::CaptureTouchState(Touch::WasUsedThisRun(), Touch::UsedTouchToBomb(),
                                       Touch::UsedCheatMovementThisRun());
    ReplayExtension::RecordFrame(stage, arg->frameId);
    arg->replayInputs++;
    arg->replayInputsByStage[stage] = arg->replayInputs + 1;
    arg->replayInputs->frameNum = curInput;
    arg->replayInputs->inputKey = arg->replayEventFlags;
    if (arg->frameId % 30 == 0)
    {
        // Vanilla TH07 stores measured FPS because one game tick is tied to
        // each presented picture and playback deliberately reproduces that
        // lag. Our uncapped portable path instead catches up every logical
        // 60 Hz tick, so a low presentation FPS there did not skip game logic.
        // Record logical 60 in that mode; retain vanilla measured FPS only on
        // the one-tick-per-picture Web path.
        u8 replayFps = 60;
#ifdef __EMSCRIPTEN__
        if (EaglerOptions::LimitPresentationTo60())
            replayFps = (u8)std::clamp<i32>(g_Supervisor.curFps, 0, 60);
#endif
        *arg->fpsCursor = replayFps |
                          ((g_Supervisor.timingErrorCount != 0) ? 128 : 0);
        *(arg->fpsCursor + 1) = replayFps;
        arg->replayDataEndPointers[stage] = (uintptr_t)(arg->fpsCursor + 2);
        arg->fpsCursor++;
    }
    arg->frameId++;
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

u32 ReplayManager::OnUpdateDemoLowPrio(ReplayManager *arg)
{
    if (!g_GameManager.notInMenu)
    {
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }

#ifdef TH_ENABLE_NETPLAY
    Netplay::Th07DeterminismProbe::AfterDemoReplayFrame();
#endif

    if (g_Gui.HasCurrentMsgIdx() && g_Gui.IsDialogueSkippable() && arg->frameId % 3 != 2)
    {
        return CHAIN_CALLBACK_RESULT_RESTART_FROM_FIRST_JOB;
    }
    if (g_GameManager.replayStage == 2 && !g_EnemyManager.HasActiveBoss() && arg->frameId % 5 != 4)
    {
        return CHAIN_CALLBACK_RESULT_RESTART_FROM_FIRST_JOB;
    }

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

u32 ReplayManager::OnUpdateDemoHighPrio(ReplayManager *arg)
{
    if (!g_GameManager.notInMenu)
    {
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }

    if (g_GameManager.defaultCfg->slowMode)
    {
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }

#ifdef TH_ENABLE_NETPLAY
    Netplay::Th07DeterminismProbe::BeforeDemoReplayFrame();
#endif

    g_LastFrameGameInput = g_CurFrameGameInput;
    ReplayExtension::SetPlaybackFrame(std::min(g_GameManager.currentStage - 1, 6), arg->frameId);
    ApplyReplayExtensionFrame();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    std::array<Netplay::FrameInput, TH07_MULTI_MAX_PLAYERS> multiplayerInputs{};
    if (ReplayExtension::GetMultiplayerPlaybackFrame(
            std::min(g_GameManager.currentStage - 1, 6), arg->frameId,
            multiplayerInputs.data(), multiplayerInputs.size()))
    {
#ifdef __EMSCRIPTEN__
        if (EaglerOptions::ReplayViewerEnabled())
        {
            bool independentInputs = false;
            for (u8 playerId = 1; playerId < MultiplayerGameplay::GetPlayerCount(); ++playerId)
                independentInputs = independentInputs ||
                                    multiplayerInputs[playerId] != multiplayerInputs[0];
#ifdef TH_DEV_TOOLS
            const int auditResult = ReplayExtension::DebugAuditMultiplayerPlaybackFrame(
                std::min(g_GameManager.currentStage - 1, 6), arg->frameId,
                multiplayerInputs.data(), MultiplayerGameplay::GetPlayerCount());
#endif
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
            }, arg->frameId, static_cast<int>(MultiplayerGameplay::GetPlayerCount()),
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
        for (i32 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            g_LastFrameGameInputs[playerId] = g_CurFrameGameInputs[playerId];
            g_CurFrameGameInputs[playerId] = multiplayerInputs[playerId].buttons;
        }
#endif
    }
    else
    {
        g_CurFrameGameInput = arg->replayInputs->frameNum;
#ifdef __EMSCRIPTEN__
        if (EaglerOptions::ReplayViewerEnabled())
        {
            EM_ASM({
                globalThis.__eaglerOrdinaryReplayPlaybackObserved = true;
                globalThis.__eaglerOrdinaryReplayPlaybackFrame = $0;
                globalThis.__eaglerOrdinaryReplayPlaybackInput = $1;
                globalThis.__eaglerOrdinaryReplayPlaybackStage = $2;
            }, arg->frameId, static_cast<unsigned>(g_CurFrameGameInput),
               g_GameManager.currentStage);
        }
#endif
        for (i32 playerId = 1; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            g_LastFrameGameInputs[playerId] = g_CurFrameGameInputs[playerId];
            g_CurFrameGameInputs[playerId] = 0;
        }
    }
#else
    g_CurFrameGameInput = arg->replayInputs->frameNum;
#ifdef __EMSCRIPTEN__
    if (EaglerOptions::ReplayViewerEnabled())
    {
        EM_ASM({
            globalThis.__eaglerOrdinaryReplayPlaybackObserved = true;
            globalThis.__eaglerOrdinaryReplayPlaybackFrame = $0;
            globalThis.__eaglerOrdinaryReplayPlaybackInput = $1;
            globalThis.__eaglerOrdinaryReplayPlaybackStage = $2;
        }, arg->frameId, static_cast<unsigned>(g_CurFrameGameInput),
           g_GameManager.currentStage);
    }
#endif
#endif
    arg->replayInputs = arg->replayInputs + 1;
    g_IsEighthFrameOfHeldInput = 0;
    if (g_LastFrameGameInput == g_CurFrameGameInput)
    {
        if (g_NumOfFramesInputsWereHeld >= 30)
        {
            if (g_NumOfFramesInputsWereHeld % 8 == 0)
            {
                g_IsEighthFrameOfHeldInput = 1;
            }
            if (g_NumOfFramesInputsWereHeld >= 38)
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
    if (arg->frameId % 30 == 0)
    {
        g_Supervisor.curFps = (i16) * (arg->fpsCursor + 1) & 0x7f;
        g_Supervisor.isFpsBad = (i32) * (arg->fpsCursor + 1) >> 7;
        arg->fpsCursor++;
    }
    arg->frameId = arg->frameId + 1;
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ZunResult ReplayManager::AddedCallback(ReplayManager *arg)
{
    StageReplayData *prevData;
    StageReplayData *endData;
    i32 i;
    StageReplayData *replayData;

    arg->frameId = 0;
    arg->unused_40 = NULL;
    const bool freshReplayRun = !arg->data;
    if (!arg->data)
    {
        ReplayExtension::ClearPlayback();
        ReplayExtension::ResetRecording();
        arg->data = new ReplayFile;
        memset(arg->data, 0, sizeof(ReplayFile));
        memcpy(&arg->data->head.magic, "T7RP", 4);
        arg->data->data.shotType = g_GameManager.shotTypeAndCharacter;
        arg->data->head.version = 0x1100;
        arg->data->data.replayVersion = 256;
        arg->data->data.versionChar1 = 'b';
        memcpy(arg->data->data.replayStr, "0100", 4);
        arg->data->data.versionChar2 = 'b';
        arg->data->data.exeSize = g_Supervisor.exeSize;
        arg->data->data.exeChecksum = g_Supervisor.exeChecksum;
        arg->data->data.difficulty = g_GameManager.difficulty;
        memcpy(arg->data->data.name, "NO NAME", 4);
        arg->data->data.cfg = *g_GameManager.defaultCfg;
        for (i = 0; i < REPLAY_STAGE_COUNT; i++)
        {
            arg->data->stageReplayData[i] = NULL;
            arg->data->stageEndData[i] = NULL;
        }
    }
    else if (g_GameManager.currentStage - 2 >= 0)
    {
        prevData = arg->data->stageReplayData[g_GameManager.currentStage - 2];
        if (prevData)
        {
            prevData->score = g_GameManager.globals->score;
        }
    }
    i = g_GameManager.currentStage - 1;
    if (i >= REPLAY_STAGE_COUNT) // PHANTASMSTAGE
    {
        i = 6; // EXTRASTAGE
    }
    const bool restartingSameStage = !freshReplayRun && arg->data->stageReplayData[i] != NULL;
    if (freshReplayRun || restartingSameStage)
    {
        // A new run/restart owns a new gesture stream. Unlike the vanilla key
        // replay, touch recording has an extra finger-id sidecar that survives
        // ReplayManager reuse unless it is explicitly reset. Do not do this on
        // normal stage progression so a legitimately held finger can continue.
        Touch::CancelTouches();
        Touch::ResetReplayRecordingState();
        Touch::ResetReplayTouch();
    }
    SAFE_FREE(arg->data->stageReplayData[i]);
    SAFE_FREE(arg->data->stageEndData[i]);
    ReplayExtension::BeginStageRecording(i);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
    {
        ReplayExtension::MultiplayerReplayConfig config;
        config.playerCount = MultiplayerGameplay::GetPlayerCount();
        config.difficulty = static_cast<u8>(g_GameManager.difficulty);
        config.localPlayer = MultiplayerGameplay::GetLocalPlayerSlot();
        config.stage4BossChain = MultiplayerGameplay::IsStage4BossChainEnabled();
        config.showContributionStats = MultiplayerGameplay::ShouldShowContributionStats();
        config.showStagePlayerNames = MultiplayerGameplay::ShouldShowStagePlayerNames();
        config.gameplayAbi = TH07_MULTI_GAMEPLAY_ABI;
        for (u8 playerId = 0; playerId < config.playerCount; ++playerId)
        {
            config.characters[playerId] = MultiplayerGameplay::GetPlayerCharacter(playerId);
            config.shots[playerId] = MultiplayerGameplay::GetPlayerShot(playerId);
        }
        ReplayExtension::BeginMultiplayerRecording(config);
        ReplayExtension::MultiplayerPlayerResourceSnapshot resources[TH07_MULTI_MAX_PLAYERS]{};
        for (u8 playerId = 0; playerId < config.playerCount; ++playerId)
        {
            resources[playerId].lives = GetPlayerLives(playerId);
            resources[playerId].bombs = GetPlayerBombs(playerId);
            resources[playerId].power = GetPlayerPower(playerId);
        }
        ReplayExtension::CaptureMultiplayerStageResources(i, resources, config.playerCount);
        ReplayExtension::MultiplayerContributionSnapshot contributions[TH07_MULTI_MAX_PLAYERS]{};
        for (u8 playerId = 0; playerId < config.playerCount; ++playerId)
        {
            contributions[playerId].enemiesDefeated = GetPlayerEnemiesDefeated(playerId);
            contributions[playerId].damageDealt = GetPlayerDamageDealt(playerId);
        }
        ReplayExtension::CaptureMultiplayerStageContributions(
            i, contributions, config.playerCount);
    }
#endif
    arg->data->stageReplayData[i] = (StageReplayData *)malloc(sizeof(StageReplayData));
    arg->data->stageEndData[i] = (StageReplayData *)malloc(sizeof(StageReplayData));

    replayData = arg->data->stageReplayData[i];
    endData = arg->data->stageEndData[i];

    replayData->grazeInTotal = g_GameManager.globals->grazeInTotal;
    replayData->bombsRemaining = g_GameManager.globals->bombsRemaining;
    replayData->livesRemaining = g_GameManager.globals->livesRemaining;
    replayData->currentPower = g_GameManager.globals->currentPower;
    replayData->rank = g_GameManager.rank.rank;
    replayData->pointItemsCollectedForExtend = g_GameManager.globals->pointItemsCollectedForExtend;
    replayData->stageRngSeed = g_GameManager.stageRngSeed;
    replayData->powerItemCountForScore = g_GameManager.powerItemCountForScore;
    replayData->cherry = g_GameManager.cherry - g_GameManager.globals->cherryStart;
    replayData->cherryMax = g_GameManager.cherryMax - g_GameManager.globals->cherryStart;
    replayData->cherryPlus = g_GameManager.cherryPlus - g_GameManager.globals->cherryStart;
    replayData->spellCardsCaptured = (u8)g_GameManager.globals->spellCardsCaptured;
    replayData->extendsFromPointItems = g_GameManager.globals->extendsFromPointItems;
    replayData->nextNeededPointItemsForExtend =
        g_GameManager.globals->nextNeededPointItemsForExtend;

    arg->replayInputs = replayData->replayInputs;
    arg->stageReplayData = endData;
    arg->fpsCursor = (u8 *)&endData->score;
    arg->replayInputs->frameNum = 0;
    arg->unused_82 = 0;
    return ZUN_SUCCESS;
}

void ReplayManager::FreeReplay(ReplayFile *replay)
{
    if (replay)
    {
        free(replay->rawData);
        delete replay;
    }
}

ReplayFile *ReplayManager::ValidateReplayData(ReplayFile *data, i32 size)
{
    ReplayFile *parsed;
    u8 *dataDecompressed;
    u8 *csumPtr;
    i32 csum;
    u8 *curByte;
    u8 obfOffset;
    i32 i;

    u8 *rawFile = (u8 *)data;
    ReplayHeader *rawHead = (ReplayHeader *)rawFile;

    if (!rawFile)
    {
        return NULL;
    }

    u32 magicT7RP;
    memcpy(&magicT7RP, "T7RP", 4);

    if (rawHead->magic != magicT7RP)
    {
        goto bad;
    }

    if (rawHead->version != 0x1100)
    {
        goto bad;
    }

    curByte = rawFile + offsetof(ReplayHeader, replaySize);
    obfOffset = rawHead->key;
    for (i32 i = 0; i < size - 16; i++, curByte++)
    {
        *curByte -= obfOffset;
        obfOffset += 7;
    }

    csumPtr = &rawHead->key;
    csum = 0x3f000318;
    for (i32 i = 0; i < size - 13; i++, csumPtr++)
    {
        csum += (u32)*csumPtr;
    }
    if (csum != rawHead->checksum)
    {
        goto bad;
    }

    dataDecompressed = (u8 *)malloc(rawHead->sizeWithoutHeader + sizeof(ReplayHeader));
    memcpy(dataDecompressed, rawHead, sizeof(ReplayHeader));
    Lzss::Decompress(rawFile + sizeof(ReplayHeader), rawHead->compressedSize,
                     dataDecompressed + sizeof(ReplayHeader), rawHead->sizeWithoutHeader);

    parsed = new ReplayFile;
    parsed->head = *(ReplayHeader *)dataDecompressed;
    parsed->data = *(ReplayData *)(dataDecompressed + sizeof(ReplayHeader));
    parsed->rawData = dataDecompressed;

    for (i = 0; i < 7; i++)
    {
        if (parsed->head.stageReplayDataOffsets[i] != 0)
        {
            parsed->stageReplayData[i] =
                (StageReplayData *)(dataDecompressed + parsed->head.stageReplayDataOffsets[i]);
        }
        else
        {
            parsed->stageReplayData[i] = NULL;
        }

        if (parsed->head.stageEndDataOffsets[i] != 0)
        {
            parsed->stageEndData[i] =
                (StageReplayData *)(dataDecompressed + parsed->head.stageEndDataOffsets[i]);
        }
        else
        {
            parsed->stageEndData[i] = NULL;
        }
    }

    if (parsed->data.cfg.slowMode)
    {
        FreeReplay(parsed);
        goto bad;
    }

    if (g_Supervisor.CheckIntegrity(parsed->data.replayStr, parsed->data.exeSize,
                                    parsed->data.exeChecksum) != ZUN_SUCCESS)
    {
        FreeReplay(parsed);
        goto bad;
    }

    free(rawFile);
    return parsed;

bad:
    free(rawFile);
    return NULL;
}

ZunResult ReplayManager::AddedCallbackDemo(ReplayManager *arg)
{
    StageReplayData *endData;
    i32 i;
    StageReplayData *replayData;

    arg->frameId = 0;
    if (!arg->data)
    {
        ReplayExtension::ClearPlayback();
        arg->data = (ReplayFile *)FileSystem::OpenFile(arg->replayFilename, !g_GameManager.demo);
        if (!ReplayExtension::MatchesPath(arg->replayFilename, reinterpret_cast<const u8 *>(arg->data), g_LastFileSize))
        {
            free(arg->data);
            arg->data = NULL;
            return ZUN_ERROR;
        }
        Touch::ResetReplayTouch();
        ReplayExtension::LoadPlayback(reinterpret_cast<const u8 *>(arg->data), g_LastFileSize);
        arg->data = ValidateReplayData(arg->data, g_LastFileSize);
        if (!arg->data)
        {
            return ZUN_ERROR;
        }
        arg->unused_40 = NULL;
        for (i = 0; i < REPLAY_STAGE_COUNT; i++)
        {
            arg->stageReplayDataSize[i] = 0;
            arg->stageEndDataSize[i] = 0;
            if (arg->data->head.stageReplayDataOffsets[i] != 0)
            {
                if (i < 6 && arg->data->head.stageReplayDataOffsets[i + 1] != 0)
                {
                    arg->stageReplayDataSize[i] = arg->data->head.stageReplayDataOffsets[i + 1] -
                                                  arg->data->head.stageReplayDataOffsets[i];
                }
                else
                {
                    arg->stageReplayDataSize[i] = arg->data->head.stageEndDataOffsets[i] -
                                                  arg->data->head.stageReplayDataOffsets[i];
                }
                if (i < 6 && arg->data->head.stageEndDataOffsets[i + 1] != 0)
                {
                    arg->stageEndDataSize[i] = arg->data->head.stageEndDataOffsets[i + 1] -
                                               arg->data->head.stageEndDataOffsets[i];
                }
                else
                {
                    arg->stageEndDataSize[i] = arg->data->head.sizeWithoutHeader +
                                               sizeof(ReplayHeader) -
                                               arg->data->head.stageEndDataOffsets[i];
                }
            }

            if (arg->data->head.stageReplayDataOffsets[i] != 0)
            {
                arg->data->stageReplayData[i] =
                    (StageReplayData *)(arg->data->head.stageReplayDataOffsets[i] +
                                        arg->data->rawData);
            }
            if (arg->data->head.stageEndDataOffsets[i] != 0)
            {
                arg->data->stageEndData[i] =
                    (StageReplayData *)(arg->data->head.stageEndDataOffsets[i] +
                                        arg->data->rawData);
            }
        }
    }
    i = g_GameManager.currentStage - 1;
    if (i >= REPLAY_STAGE_COUNT) // PHANTASMSTAGE
    {
        i = 6; // EXTRASTAGE
    }
    if (!arg->data->stageReplayData[i])
    {
        return ZUN_ERROR;
    }

    replayData = arg->data->stageReplayData[i];
    endData = arg->data->stageEndData[i];

    g_GameManager.character = arg->data->data.shotType / 2;
    g_GameManager.shotType = arg->data->data.shotType % 2;
    g_GameManager.shotTypeAndCharacter = arg->data->data.shotType;
    g_GameManager.difficulty = arg->data->data.difficulty;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Built-in demos enter replay playback without passing through the replay
    // menu, so the gameplay session can still contain its process-default
    // ReimuA loadout here.  The multiplayer collision path consults this
    // session even for its single active player (for ReimuA's stage-specific
    // damage reduction, homing behaviour, and contribution ownership).  Keep
    // ordinary replay/demo playback synchronized with the authoritative TH07
    // replay header; multiplayer replays already own a configured sidecar.
    if (!ReplayExtension::MultiplayerPlaybackActive())
    {
        MultiplayerGameplay::ResetToSinglePlayer(
            static_cast<u8>(g_GameManager.character),
            static_cast<u8>(g_GameManager.shotType));
    }
#endif
    g_GameManager.globals->pointItemsCollectedForExtend = replayData->pointItemsCollectedForExtend;
    g_GameManager.rank.rank = replayData->rank;
    g_GameManager.SetLivesRemaining(replayData->livesRemaining);
    g_GameManager.RegenerateGameIntegrityCsum();
    g_GameManager.SetBombsRemainingAndComputeCsum(replayData->bombsRemaining);
    g_GameManager.SetCurrentPower(replayData->currentPower);
    g_GameManager.RegenerateGameIntegrityCsum();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() && ReplayExtension::MultiplayerPlaybackActive())
    {
        // Vanilla StageReplayData restores only P1.  Guest sidecars were
        // already initialized by Player::RegisterChain before this callback,
        // so direct Stage2+ MP Replay playback otherwise starts P2/P3 with
        // default lives/bombs/power.  That changes whether a death enters
        // Spirit, shifts the two RNG calls that choose Spirit drift, and makes
        // the recorded 90-frame Focus revival input miss its original target.
        for (u8 playerId = 0; playerId < MultiplayerGameplay::GetPlayerCount(); ++playerId)
        {
            ReplayExtension::MultiplayerPlayerResourceSnapshot resources;
            if (!ReplayExtension::GetMultiplayerPlaybackStageResources(
                    i, playerId, &resources))
                continue;
            SetPlayerLives(playerId, resources.lives);
            SetPlayerBombs(playerId, resources.bombs);
            SetPlayerPower(playerId, resources.power);

            ReplayExtension::MultiplayerContributionSnapshot contributions;
            if (ReplayExtension::GetMultiplayerPlaybackStageContributions(
                    i, playerId, &contributions))
            {
                g_MultiplayerContributionStats[playerId].enemiesDefeated =
                    contributions.enemiesDefeated;
                g_MultiplayerContributionStats[playerId].damageDealt =
                    contributions.damageDealt;
            }
        }
    }
#endif
    g_GameManager.globals->grazeInTotal = replayData->grazeInTotal;
    arg->replayInputs = replayData->replayInputs;
    g_GameManager.powerItemCountForScore = replayData->powerItemCountForScore;
    g_GameManager.cherry = replayData->cherry + g_GameManager.globals->cherryStart;
    g_GameManager.cherryMax = replayData->cherryMax + g_GameManager.globals->cherryStart;
    g_GameManager.cherryPlus = replayData->cherryPlus + g_GameManager.globals->cherryStart;
    const i32 borderThreshold =
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        GetSharedBorderThreshold();
#else
        50000;
#endif
    if (g_GameManager.cherryPlus >= g_GameManager.globals->cherryStart + borderThreshold)
    {
        g_GameManager.cherryPlus = g_GameManager.globals->cherryStart + borderThreshold;
        g_Player.ActivateBorder();
    }
    *g_GameManager.defaultCfg = arg->data->data.cfg;
    g_Rng.SetSeed(replayData->stageRngSeed);
    g_GameManager.globals->spellCardsCaptured = replayData->spellCardsCaptured;
    g_GameManager.globals->extendsFromPointItems = replayData->extendsFromPointItems;
    g_GameManager.globals->nextNeededPointItemsForExtend =
        replayData->nextNeededPointItemsForExtend;
    arg->stageReplayData = endData;
    arg->fpsCursor = (u8 *)&endData->score;
    if (g_GameManager.currentStage >= STAGE2 && g_GameManager.currentStage <= STAGE6 &&
        arg->data->stageReplayData[g_GameManager.currentStage - 2])
    {
        g_GameManager.globals->guiScore = g_GameManager.globals->score =
            arg->data->stageReplayData[g_GameManager.currentStage - 2]->score;
    }
    return ZUN_SUCCESS;
}

ZunResult ReplayManager::DeletedCallback(ReplayManager *arg)
{
    Touch::ResetReplayTouch();
    ReplayExtension::ClearPlayback();
    g_Chain.Cut(arg->drawChain);
    arg->drawChain = NULL;
    if (arg->demoCalcChain)
    {
        g_Chain.Cut(arg->demoCalcChain);
        arg->demoCalcChain = NULL;
    }
    if (arg->rngCalcChain)
    {
        g_Chain.Cut(arg->rngCalcChain);
        arg->rngCalcChain = NULL;
    }
    if (g_ReplayManager->data && !g_ReplayManager->isDemo)
    {
        for (i32 i = 0; i < 7; i++)
        {
            if (g_ReplayManager->data->stageReplayData[i])
            {
                free(g_ReplayManager->data->stageReplayData[i]);
            }
            if (g_ReplayManager->data->stageEndData[i])
            {
                free(g_ReplayManager->data->stageEndData[i]);
            }
        }
    }
    FreeReplay(g_ReplayManager->data);

    if (arg->unused_40)
    {
        free(arg->unused_40);
    }

    delete g_ReplayManager;
    g_ReplayManager = NULL;

    return ZUN_SUCCESS;
}

ZunResult ReplayManager::RegisterChain(ZunBool isDemo, const char *replayFilename)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for (i32 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        g_LastFrameGameInputs[playerId] = 0;
        g_CurFrameGameInputs[playerId] = 0;
    }
#else
    g_LastFrameGameInput = 0;
    g_CurFrameGameInput = 0;
#endif
    if (!g_ReplayManager)
    {
        ReplayManager *mgr = new ReplayManager();
        g_ReplayManager = mgr;
        mgr->data = NULL;
        mgr->isDemo = isDemo;
        mgr->replayFilename = replayFilename;
        switch (isDemo)
        {
        case 0:
            mgr->calcChain = g_Chain.CreateElem((ChainCallback)OnUpdate);
            mgr->calcChain->addedCallback = (ChainLifecycleCallback)AddedCallback;
            mgr->calcChain->deletedCallback = (ChainLifecycleCallback)DeletedCallback;
            mgr->drawChain =
                g_Chain.CreateElem((ChainCallback)EffectManager::UpdateNoOp); // idk either bro
            mgr->calcChain->arg = mgr;
            if (g_Chain.AddToCalcChain(mgr->calcChain, 16))
            {
                return ZUN_ERROR;
            }

            mgr->demoCalcChain = NULL;
            mgr->rngCalcChain = g_Chain.CreateElem((ChainCallback)OnUpdateRng);
            mgr->rngCalcChain->arg = mgr;
            g_Chain.AddToCalcChain(mgr->rngCalcChain, 6);
            break;
        case 1:
            mgr->calcChain = g_Chain.CreateElem((ChainCallback)OnUpdateDemoHighPrio);
            mgr->calcChain->addedCallback = (ChainLifecycleCallback)AddedCallbackDemo;
            mgr->calcChain->deletedCallback = (ChainLifecycleCallback)DeletedCallback;
            mgr->drawChain = g_Chain.CreateElem((ChainCallback)EffectManager::UpdateNoOp);
            mgr->calcChain->arg = mgr;
            if (g_Chain.AddToCalcChain(mgr->calcChain, 5))
            {
                return ZUN_ERROR;
            }

            mgr->demoCalcChain = g_Chain.CreateElem((ChainCallback)OnUpdateDemoLowPrio);
            mgr->demoCalcChain->arg = mgr;
            g_Chain.AddToCalcChain(mgr->demoCalcChain, 17);
            mgr->rngCalcChain = NULL;
            break;
        }
        mgr->drawChain->arg = mgr;
        g_Chain.AddToDrawChain(mgr->drawChain, 14);
    }
    else
    {
        switch (isDemo)
        {
        case 0:
            AddedCallback(g_ReplayManager);
            break;
        case 1:
            AddedCallbackDemo(g_ReplayManager);
            break;
        }
    }
    return ZUN_SUCCESS;
}

void ReplayManager::StopRecording()
{
    ReplayManager *mgr = g_ReplayManager;

    if (mgr)
    {
        mgr->replayInputs++;
        mgr->replayInputs->frameNum = 0;
        i32 stage = g_GameManager.currentStage - 1;
        if (stage >= REPLAY_STAGE_COUNT) // PHANTASMSTAGE
        {
            stage = 6; // EXTRASTAGE
        }
        mgr->replayInputsByStage[stage] = mgr->replayInputs + 1;
    }
}

void ReplayManager::SaveReplay(const char *filename, char *replayName)
{
    u8 *curByte;
    u8 obfOffset;
    u8 *csumPtr;
    i32 csum;
    i32 replaySize;
    SDL_IOStream *file;
    ReplayFile replayCopy;
    u8 *replayData;
    i32 stageSize;
    i32 compressedSize;
    f32 slowdown;
    u8 *lpBuffer;
    ReplayManager *mgr;
    i32 i;

    if (g_ReplayManager)
    {
        mgr = g_ReplayManager;
        if (!mgr->IsDemo())
        {
            if (!g_GameManager.practice && g_GameManager.difficulty < 4 &&
                memcmp(&g_Supervisor.cfg, &mgr->data->data.cfg, sizeof(g_Supervisor.cfg)) != 0)
            {
                goto SKIP_WRITE;
            }
            if (mgr->data->data.cfg.slowMode)
            {
                goto SKIP_WRITE;
            }
            if (filename)
            {
                const std::string actualFilename = ReplayExtension::ResolveSavePath(filename);
                Supervisor::DebugPrint("info : Replay File write %s\n", actualFilename.c_str());
                replayData = (u8 *)malloc(0x100000);
                replayCopy = *mgr->data;
                StopRecording();
                i = g_GameManager.currentStage - 1;
                if (i >= REPLAY_STAGE_COUNT) // PHANTASMSTAGE
                {
                    i = 6; // EXTRASTAGE
                }
                mgr->data->stageReplayData[i]->score = g_GameManager.globals->score;
                replaySize = sizeof(ReplayHeader);
                replaySize += sizeof(ReplayData);
                for (i = 0; i < REPLAY_STAGE_COUNT; i++)
                {
                    if (mgr->data->stageReplayData[i])
                    {
                        stageSize =
                            (u8 *)mgr->replayInputsByStage[i] - (u8 *)mgr->data->stageReplayData[i];
                        memcpy((StageReplayData *)(replayData + replaySize - sizeof(ReplayHeader)),
                               mgr->data->stageReplayData[i], stageSize);
                        replayCopy.head.stageReplayDataOffsets[i] = replaySize;
                        replaySize += stageSize;
                    }
                }
                for (i = 0; i < REPLAY_STAGE_COUNT; i++)
                {
                    if (mgr->data->stageEndData[i])
                    {
                        stageSize =
                            (u8 *)mgr->replayDataEndPointers[i] - (u8 *)mgr->data->stageEndData[i];
                        memcpy((StageReplayData *)(replayData + replaySize - sizeof(ReplayHeader)),
                               mgr->data->stageEndData[i], stageSize);
                        replayCopy.head.stageEndDataOffsets[i] = replaySize;
                        replaySize += stageSize;
                    }
                }
                replayCopy.data.score = g_GameManager.globals->guiScore;
                slowdown =
                    (g_Supervisor.framerateMultiplier / g_Supervisor.fpsAccumulator - 0.5f) * 2.0f;
                if (slowdown < 0.0f)
                {
                    slowdown = 0.0f;
                }
                else if (slowdown >= 1.0f)
                {
                    slowdown = 1.0f;
                }
                replayCopy.data.slowdownRate = Touch::UsedCheatMovementThisRun()
                                                   ? 100.0f
                                                   : (1.0f - slowdown) * 100.0f;
                replayCopy.head.replaySize = replaySize;
                strcpy(replayCopy.data.name, replayName);
                ResultScreen::GetDate(replayCopy.data.date);
                replayCopy.head.key = g_Rng.GetRandomU16InRange(128) + 64;
                replayCopy.data.rngValue3 = g_Rng.GetRandomU16InRange(256);
                replayCopy.head.rngValue1 = g_Rng.GetRandomU16InRange(256);
                replayCopy.data.slowdownRate2 = replayCopy.data.slowdownRate + 1.12f;
                replayCopy.data.slowdownRate3 = replayCopy.data.slowdownRate + 2.34f;
                replayCopy.data.magic30 = 30;
                memcpy(replayData, &replayCopy.data.rngValue3, sizeof(ReplayData));
                Supervisor::DebugPrint("info : original size %d\n", replaySize);
                replayCopy.head.sizeWithoutHeader = replaySize - sizeof(ReplayHeader);
                lpBuffer = Lzss::Compress(replayData, replayCopy.head.sizeWithoutHeader,
                                          &replayCopy.head.compressedSize);
                free(replayData);
                compressedSize = replayCopy.head.compressedSize;
                csumPtr = &replayCopy.head.key;
                csum = 0x3f000318;
                for (i = 0; (u32)i < 0x47; i++, csumPtr++)
                {
                    csum += (u32)*csumPtr;
                }
                csumPtr = lpBuffer;
                for (i = 0; i < compressedSize; i++, csumPtr++)
                {
                    csum += (u32)*csumPtr;
                }
                replayCopy.head.checksum = csum;
                curByte = (u8 *)&replayCopy.head.replaySize;
                obfOffset = replayCopy.head.key;
                for (i = 0; (u32)i < 0x44; i++, curByte++)
                {
                    *curByte += obfOffset;
                    obfOffset += 7;
                }
                curByte = lpBuffer;
                for (i = 0; i < compressedSize; i++, curByte++)
                {
                    *curByte += obfOffset;
                    obfOffset += 7;
                }
                file = SDL_IOFromFile(actualFilename.c_str(), "wb");
                if (file)
                {
                    SDL_WriteIO(file, &replayCopy, sizeof(ReplayHeader));
                    SDL_WriteIO(file, lpBuffer, compressedSize);
                    SDL_CloseIO(file);
                    PracticeRuntime::SaveReplayMetadata(actualFilename.c_str());
                    if (ReplayExtension::AppendRecording(actualFilename.c_str()))
                    {
                        ReplayExtension::RemoveAlternateSave(actualFilename.c_str());
#if defined(__EMSCRIPTEN__) && defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
                        EM_ASM({
                            if (Module.eaglerOptions?.netplayMode !== "lan")
                                return;
                                globalThis.__eaglerNetplayReplaySaved = true;
                                globalThis.__eaglerNetplayReplaySavedPath = UTF8ToString($0);
                                globalThis.__eaglerNetplayReplaySavedStoragePath = UTF8ToString($1);
                        }, actualFilename.c_str(), actualFilename.c_str());
#endif
                    }
                    Supervisor::DebugPrint("info : Size %d -> %d\n", replaySize,
                                           compressedSize + sizeof(ReplayHeader));
                    free(lpBuffer);
                }
            }
        SKIP_WRITE:
            for (i = 0; i < REPLAY_STAGE_COUNT; i++)
            {
                SAFE_FREE(g_ReplayManager->data->stageReplayData[i]);
                SAFE_FREE(g_ReplayManager->data->stageEndData[i]);
            }
        }
        g_Chain.Cut(g_ReplayManager->calcChain);
    }
}

void ReplayManager::SaveReplay2(const char *filename)
{
    u8 *curByte;
    u8 obfOffset;
    u8 *csumPtr;
    u32 csum;
    i32 replaySize;
    SDL_IOStream *file;
    ReplayFile replayCopy;
    u8 *replayData;
    i32 stageSize;
    i32 compressedSize;
    u8 *lpBuffer;
    ReplayManager *mgr;
    i32 i;

    if (g_ReplayManager)
    {
        mgr = g_ReplayManager;
        if (!g_GameManager.practice && g_GameManager.difficulty < 4 &&
            memcmp(&g_Supervisor.cfg, &mgr->data->data.cfg, sizeof(g_Supervisor.cfg)) != 0)
        {
            goto SKIP_WRITE;
        }
        if (mgr->data->data.cfg.slowMode)
        {
            goto SKIP_WRITE;
        }
        if (filename)
        {
            Supervisor::DebugPrint("info : Replay File rewrite %s\n", filename);
            replayData = (u8 *)malloc(0x100000);
            replayCopy = *mgr->data;
            i = g_GameManager.currentStage - 1;
            if (i >= REPLAY_STAGE_COUNT) // PHANTASMSTAGE
            {
                i = 6; // EXTRASTAGE
            }
            mgr->data->stageReplayData[i]->score = g_GameManager.globals->score;
            replaySize = sizeof(ReplayHeader);
            replaySize += sizeof(ReplayData);
            for (i = 0; i < REPLAY_STAGE_COUNT; i++)
            {
                if (mgr->data->stageReplayData[i])
                {
                    stageSize = mgr->stageReplayDataSize[i];
                    memcpy((StageReplayData *)(replayData + replaySize - sizeof(ReplayHeader)),
                           mgr->data->stageReplayData[i], stageSize);
                    replayCopy.head.stageReplayDataOffsets[i] = replaySize;
                    replaySize += stageSize;
                }
            }
            for (i = 0; i < REPLAY_STAGE_COUNT; i++)
            {
                if (mgr->data->stageEndData[i])
                {
                    stageSize = mgr->stageEndDataSize[i];
                    memcpy((StageReplayData *)(replayData + replaySize - sizeof(ReplayHeader)),
                           mgr->data->stageEndData[i], stageSize);
                    replayCopy.head.stageEndDataOffsets[i] = replaySize;
                    replaySize += stageSize;
                }
            }
            replayCopy.data.score = g_GameManager.globals->guiScore;
            replayCopy.head.replaySize = replaySize;
            replayCopy.head.key = g_Rng.GetRandomU16InRange(128) + 64;
            replayCopy.data.rngValue3 = g_Rng.GetRandomU16InRange(256);
            replayCopy.head.rngValue1 = g_Rng.GetRandomU16InRange(256);
            if (Touch::UsedCheatMovementThisRun())
                replayCopy.data.slowdownRate = 100.0f;
            replayCopy.data.slowdownRate2 = replayCopy.data.slowdownRate + 1.12f;
            replayCopy.data.slowdownRate3 = replayCopy.data.slowdownRate + 2.34f;
            replayCopy.data.magic30 = 30;
            memcpy(replayData, &replayCopy.data.rngValue3, sizeof(ReplayData));
            Supervisor::DebugPrint("info : original size %d\n", replaySize);
            replayCopy.head.sizeWithoutHeader = replaySize - sizeof(ReplayHeader);
            lpBuffer = Lzss::Compress(replayData, replayCopy.head.sizeWithoutHeader,
                                      &replayCopy.head.compressedSize);
            free(replayData);
            compressedSize = replayCopy.head.compressedSize;
            csumPtr = &replayCopy.head.key;
            csum = 0x3f000318;
            for (i = 0; (u32)i < 0x47; i++, csumPtr++)
            {
                csum += (u32)*csumPtr;
            }
            csumPtr = lpBuffer;
            for (i = 0; i < compressedSize; i++, csumPtr++)
            {
                csum += (u32)*csumPtr;
            }
            replayCopy.head.checksum = csum;
            curByte = (u8 *)&replayCopy.head.replaySize;
            obfOffset = replayCopy.head.key;
            for (i = 0; (u32)i < 0x44; i++, curByte++)
            {
                *curByte += obfOffset;
                obfOffset += 7;
            }
            curByte = lpBuffer;
            for (i = 0; i < compressedSize; i++, curByte++)
            {
                *curByte += obfOffset;
                obfOffset += 7;
            }
            file = SDL_IOFromFile(filename, "wb");
            if (file)
            {
                SDL_WriteIO(file, &replayCopy, sizeof(ReplayHeader));
                SDL_WriteIO(file, lpBuffer, compressedSize);
                SDL_CloseIO(file);
                PracticeRuntime::SaveReplayMetadata(filename);
                ReplayExtension::AppendPlayback(filename);
                Supervisor::DebugPrint("info : Size %d -> %d\n", replaySize,
                                       compressedSize + sizeof(ReplayHeader));
                free(lpBuffer);
            }
        }
    SKIP_WRITE:
        g_Chain.Cut(g_ReplayManager->calcChain);
    }
}
