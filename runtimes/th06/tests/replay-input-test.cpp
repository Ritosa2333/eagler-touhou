// Execute the production ReplayManager and dialogue VM with rendering, audio
// and touch side effects stubbed. This checks input ownership and tick order,
// not complete game/ECL/RNG determinism.
#include "AnmManager.hpp"
#include "AsciiManager.hpp"
#include "Controller.hpp"
#include "GameManager.hpp"
#include "Gui.hpp"
#include "ReplayExtension.hpp"
#include "ReplayManager.hpp"
#include "Stage.hpp"
#include "Touch.hpp"
#include "utils.hpp"
#ifdef TH_ENABLE_NETPLAY
#include <eagler/netplay/NetplayInput.hpp>
#endif

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>

StageMenu::StageMenu() {}
GameManager::GameManager() {}
AsciiManager::AsciiManager() {}
AnmManager::AnmManager() {}
AnmManager::~AnmManager() {}
Stage::Stage() {}
GameManager g_GameManager{};
AsciiManager g_AsciiManager{};
Supervisor g_Supervisor{};
Stage g_Stage{};
static AnmManager anm;
AnmManager *g_AnmManager = &anm;
u16 g_CurFrameInput = 0, g_LastFrameInput = 0;
u16 g_IsEigthFrameOfHeldInput = 0, g_NumOfFramesInputsWereHeld = 0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
u16 g_CurFrameGameInputs[TH06_MULTI_MAX_PLAYERS]{};
u16 g_LastFrameGameInputs[TH06_MULTI_MAX_PLAYERS]{};
namespace MultiplayerGameplay
{
bool IsMultiplayer() { return false; }
u8 GetPlayerCount() { return 3; }
}
#endif

// No rendering or device work is needed to execute MSG WAIT/SKIP semantics.
void AsciiManager::AddString(const ZunVec3 *, const char *) {}
i32 AnmManager::ExecuteScript(AnmVm *) { return 0; }
void AnmManager::SetAndExecuteScript(AnmVm *, const AnmRawInstr *) {}
ZunResult AnmManager::SetActiveSprite(AnmVm *, u32) { return ZUN_SUCCESS; }
void AnmManager::DrawVmTextFmt(AnmVm *, ZunColor, ZunColor, const char *, ...) {}
void AnmManager::DrawStringFormat(AnmVm *, ZunColor, ZunColor, const char *, ...) {}
ZunResult Supervisor::PlayMidiFile(i32) { return ZUN_SUCCESS; }
ZunResult Supervisor::PlayAudio(const char *) { return ZUN_SUCCESS; }
ZunResult Supervisor::FadeOutMusic(f32) { return ZUN_SUCCESS; }
void Supervisor::TickTimer(i32 *frames, f32 *) { ++*frames; }
namespace Touch
{
bool WasUsedThisRun() { return false; }
bool UsedTouchToBomb() { return false; }
bool UsedCheatMovementThisRun() { return false; }
void SetReplayUsageState(bool, bool, bool) {}
void BeginReplayTouchFrame() {}
void ApplyReplayTouchEvent(i32, f32, f32, u32, u32, u32) {}
i32 GetReplayTouchPoints(ReplayTouchPoint *, i32) { return 0; }
}
namespace ReplayExtension
{
static bool multiplayer = false;
static std::array<Netplay::FrameInput, 3> players{};
void CaptureTouchState(bool, bool, bool) {}
void RecordFrame(i32, i32) {}
void RecordMultiplayerFrame(i32, i32, const Netplay::FrameInput *, std::size_t) {}
void SetPlaybackFrame(i32, i32) {}
bool GetPlaybackTouchState(bool *, bool *, bool *) { return false; }
const TouchEvent *GetPlaybackTouchEvents(std::size_t *count) { *count = 0; return nullptr; }
bool GetMultiplayerPlaybackFrame(i32, i32, Netplay::FrameInput *out, std::size_t count)
{
    if (!multiplayer) return false;
    std::copy_n(players.begin(), std::min(count, players.size()), out);
    return true;
}
}

// Match Supervisor's pre-Replay hardware sampling, including the ordinary P1
// mirror used by a multiplayer-capable binary without synchronized overrides.
static void SampleHardware(u16 buttons)
{
    g_LastFrameInput = g_CurFrameInput;
    g_CurFrameInput = buttons;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#ifdef TH_ENABLE_NETPLAY
    if (Netplay::Input::PlayerButtonOverridesActive()) return;
#endif
    for (int player = 0; player < TH06_MULTI_MAX_PLAYERS; ++player)
    {
        g_LastFrameGameInputs[player] = g_CurFrameGameInputs[player];
        g_CurFrameGameInputs[player] = player == 0 ? buttons : 0;
    }
#endif
}

static void Reset()
{
    g_CurFrameInput = g_LastFrameInput = 0;
    g_IsEigthFrameOfHeldInput = g_NumOfFramesInputsWereHeld = 0;
    g_GameManager.isInMenu = 1;
    g_GameManager.isInReplay = 1;
    g_GameManager.currentStage = 6;
    g_Supervisor.framerateMultiplier = g_Supervisor.effectiveFramerateMultiplier = 1.0f;
    ReplayExtension::multiplayer = false;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    std::fill_n(g_CurFrameGameInputs, TH06_MULTI_MAX_PLAYERS, 0);
    std::fill_n(g_LastFrameGameInputs, TH06_MULTI_MAX_PLAYERS, 0);
#endif
#ifdef TH_ENABLE_NETPLAY
    Netplay::Input::ClearPlayerButtonOverrides();
#endif
}

static void CheckRecordedInput()
{
    // A full-run recording resets its input cursor at every stage. Exercise
    // that contract through the real recorder and player input callbacks.
    const std::array<u16, 12> recorded{
        TH_BUTTON_RIGHT | TH_BUTTON_SHOOT, TH_BUTTON_RIGHT | TH_BUTTON_SHOOT,
        TH_BUTTON_RIGHT, TH_BUTTON_SHOOT, TH_BUTTON_SHOOT | TH_BUTTON_SKIP,
        TH_BUTTON_SKIP, TH_BUTTON_LEFT | TH_BUTTON_FOCUS, TH_BUTTON_BOMB,
        0, TH_BUTTON_UP, TH_BUTTON_DOWN | TH_BUTTON_SHOOT, 0,
    };
    for (int stage = 1; stage <= 6; ++stage)
    {
        Reset();
        g_GameManager.currentStage = stage;
        g_GameManager.isInReplay = 0;
        std::array<ReplayDataInput, 32> stream{};
        ReplayManager recorder{};
        recorder.frameId = 0;
        recorder.replayInputs = stream.data();
        recorder.replayInputEnd = stream.data() + stream.size();
        for (u16 buttons : recorded)
        {
            SampleHardware(buttons);
            ReplayManager::OnUpdate(&recorder);
        }
        g_ReplayManager = &recorder;
        ReplayManager::StopRecording();
        g_ReplayManager = nullptr;
        const auto *inputEnd = recorder.replayInputs + 1;

        Reset();
        g_GameManager.currentStage = stage;
        ReplayManager playback{};
        playback.frameId = 0;
        playback.replayInputs = stream.data();
        playback.replayInputEnd = inputEnd;
        u16 previous = 0;
        for (u16 expected : recorded)
        {
            // Deliberately conflict with recorded directions, shot and Ctrl.
            SampleHardware(TH_BUTTON_LEFT | TH_BUTTON_SHOOT | TH_BUTTON_SKIP | TH_BUTTON_MENU);
            ReplayManager::OnUpdateDemoHighPrio(&playback);
            assert(g_CurFrameInput == (expected | TH_BUTTON_MENU));
            assert(IS_PRESSED_GAME(TH_BUTTON_DIRECTION | TH_BUTTON_SHOOT | TH_BUTTON_BOMB |
                                   TH_BUTTON_FOCUS | TH_BUTTON_SKIP) == expected);
            assert(bool(WAS_PRESSED_GAME(TH_BUTTON_SHOOT)) ==
                   bool((expected & TH_BUTTON_SHOOT) && !(previous & TH_BUTTON_SHOOT)));
            previous = expected;
        }
        g_GameManager.isInMenu = 0;
        const int frame = playback.frameId;
        const ReplayDataInput *cursor = playback.replayInputs;
        ReplayManager::OnUpdateDemoHighPrio(&playback);
        assert(playback.frameId == frame && playback.replayInputs == cursor);
    }
}

static void CheckDialogue()
{
    // Eight-byte WAIT followed by a real MSGDELETE instruction. Pressing Z
    // after the minimum wait or holding recorded Ctrl must exit on that tick.
    alignas(MsgRawInstr) std::array<u8, 2 * sizeof(MsgRawInstr)> scriptBytes{};
    auto *script = reinterpret_cast<MsgRawInstr *>(scriptBytes.data());
    script[0].time = 0;
    script[0].opcode = MSG_OPCODE_WAIT;
    script[0].argSize = sizeof(MsgRawInstrArgs);
    script[0].args.wait = 120;
    script[1].time = 0;
    script[1].opcode = MSG_OPCODE_MSGDELETE;
    GuiImpl dialogue{};
    dialogue.msg.ignoreWaitCounter = 0;
    g_Gui.impl = &dialogue;
    for (bool skip : {false, true})
    {
        Reset();
        dialogue.msg.currentMsgIdx = 0;
        dialogue.msg.currentInstr = script;
        dialogue.msg.dialogueSkippable = 1;
        dialogue.msg.framesElapsedDuringPause = 8;
        dialogue.msg.timer.SetCurrent(0);
        ReplayDataInput inputs[] = {{0, u16(skip ? TH_BUTTON_SKIP : TH_BUTTON_SHOOT), 0}, {9999999, 0, 0}};
        ReplayManager playback{};
        playback.frameId = 0;
        playback.replayInputs = inputs;
        playback.replayInputEnd = inputs + 2;
        SampleHardware(0);
        ReplayManager::OnUpdateDemoHighPrio(&playback);
        assert(dialogue.RunMsg() == ZUN_ERROR);
        assert(dialogue.msg.currentMsgIdx < 0);
    }
    Reset();
    dialogue.msg.currentMsgIdx = 0;
    dialogue.msg.currentInstr = script;
    dialogue.msg.dialogueSkippable = 1;
    dialogue.msg.framesElapsedDuringPause = 8;
    dialogue.msg.timer.SetCurrent(0);
    ReplayDataInput inputs[] = {{0, 0, 0}, {9999999, 0, 0}};
    ReplayManager playback{};
    playback.frameId = 0;
    playback.replayInputs = inputs;
    playback.replayInputEnd = inputs + 2;
    SampleHardware(TH_BUTTON_SHOOT | TH_BUTTON_SKIP);
    ReplayManager::OnUpdateDemoHighPrio(&playback);
    assert(dialogue.RunMsg() == ZUN_SUCCESS);
    assert(dialogue.msg.currentMsgIdx == 0);
    assert(dialogue.msg.framesElapsedDuringPause == 9);
    g_Gui.impl = nullptr;
}

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
static void CheckMultiplayerInput()
{
    Reset();
    ReplayExtension::multiplayer = true;
    const u16 buttons[] = {TH_BUTTON_RIGHT, TH_BUTTON_UP, TH_BUTTON_LEFT};
    for (int player = 0; player < 3; ++player)
        ReplayExtension::players[player].buttons = buttons[player];
    ReplayDataInput inputs[] = {{0, TH_BUTTON_SHOOT, 0}, {9999999, 0, 0}};
    ReplayManager playback{};
    playback.frameId = 0;
    playback.replayInputs = inputs;
    playback.replayInputEnd = inputs + 2;
    SampleHardware(TH_BUTTON_MENU | TH_BUTTON_LEFT);
    ReplayManager::OnUpdateDemoHighPrio(&playback);
    u16 combined = TH_BUTTON_MENU;
    for (int player = 0; player < 3; ++player)
    {
        assert(g_CurFrameGameInputs[player] == ReplayExtension::players[player].buttons);
        combined |= ReplayExtension::players[player].buttons;
    }
    assert(g_CurFrameInput == combined);
}
#endif

int main()
{
    CheckRecordedInput();
    CheckDialogue();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    CheckMultiplayerInput();
#endif
    std::puts("TH06 Replay input ownership: PASS (six stages, dialogue Z/Ctrl, live-input isolation)");
}
