// Test the real TH07 Replay callbacks. The first zero input is an intentional
// one-tick pipeline delay: the recorder samples after gameplay at priority 16.
// Device and presentation effects are stubbed; this is not a world-state oracle.
#include "AsciiManager.hpp"
#include "Controller.hpp"
#include "GameManager.hpp"
#include "Gui.hpp"
#include "ReplayExtension.hpp"
#include "ReplayManager.hpp"
#include "Rng.hpp"
#include "Touch.hpp"
#include "pbg4/Lzss.hpp"
#ifdef TH_ENABLE_NETPLAY
#include <eagler/netplay/NetplayInput.hpp>
#endif
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <vector>

GameManager::GameManager() {}
AsciiManager::AsciiManager() {}
PauseMenu::PauseMenu() {}
RetryMenu::RetryMenu() {}
ZunResult Supervisor::CheckIntegrity(const char *, i32, i32) { return ZUN_SUCCESS; }
GameManager g_GameManager{};
Supervisor g_Supervisor{};
AsciiManager g_AsciiManager{};
Gui g_Gui{};
Rng g_Rng{};
u16 g_CurFrameRawInput = 0, g_LastFrameRawInput = 0;
u16 g_IsEighthFrameOfHeldInput = 0, g_NumOfFramesInputsWereHeld = 0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
u16 g_CurFrameGameInputs[TH07_MULTI_MAX_PLAYERS]{};
u16 g_LastFrameGameInputs[TH07_MULTI_MAX_PLAYERS]{};
namespace MultiplayerGameplay
{
bool IsMultiplayer() { return false; }
u8 GetPlayerCount() { return 3; }
}
#else
u16 g_CurFrameGameInput = 0, g_LastFrameGameInput = 0;
#endif

void AsciiManager::AddString(ZunVec3 *, const char *) {}
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
#ifdef TH_ENABLE_NETPLAY
namespace Netplay::Th07DeterminismProbe
{
void BeforeDemoReplayFrame() {}
void AfterDemoReplayFrame() {}
}
#endif

static void Reset(GameConfiguration &cfg)
{
    g_GameManager.defaultCfg = &cfg;
    cfg.slowMode = 0;
    g_GameManager.notInMenu = 1;
    g_CurFrameGameInput = g_LastFrameGameInput = 0;
    g_CurFrameRawInput = g_LastFrameRawInput = 0;
    g_Supervisor.timingBad = 0;
    g_Supervisor.timingErrorCount = 0;
    g_Supervisor.curFps = 60;
    g_NumOfFramesInputsWereHeld = g_IsEighthFrameOfHeldInput = 0;
    ReplayExtension::multiplayer = false;
#ifdef TH_ENABLE_NETPLAY
    Netplay::Input::ClearPlayerButtonOverrides();
#endif
}

static void CheckRecordedInput()
{
    GameConfiguration cfg{};
    std::array<u16, 95> buttons{};
    for (unsigned frame = 0; frame < buttons.size(); ++frame)
        buttons[frame] = u16((frame % 17 < 8 ? TH_BUTTON_SHOOT : 0) |
                            (frame % 13 < 5 ? TH_BUTTON_SKIP : TH_BUTTON_FOCUS) |
                            (frame % 11 < 5 ? TH_BUTTON_RIGHT : TH_BUTTON_LEFT));
    for (int stage = 1; stage <= 8; ++stage)
    {
        Reset(cfg);
        g_GameManager.currentStage = stage;
        std::array<ReplayDataInput, 100> stream{};
        std::array<u8, 8> fps{};
        fps.fill(60);
        ReplayManager recorder{};
        recorder.frameId = 0;
        recorder.replayInputs = stream.data();
        recorder.fpsCursor = fps.data();
        for (unsigned frame = 0; frame < buttons.size(); ++frame)
        {
            g_CurFrameRawInput = buttons[frame];
            recorder.replayEventFlags = u16(frame % 9);
            ReplayManager::OnUpdate(&recorder);
            assert(g_CurFrameGameInput == buttons[frame]);
            assert(stream[frame + 1].frameNum == buttons[frame]);
            assert(stream[frame + 1].inputKey == frame % 9);
        }
        g_ReplayManager = &recorder;
        ReplayManager::StopRecording();
        g_ReplayManager = nullptr;
        assert(recorder.replayInputsByStage[std::min(stage - 1, 6)] == recorder.replayInputs + 1);
        Reset(cfg);
        ReplayManager playback{};
        playback.frameId = 0;
        playback.replayInputs = stream.data();
        playback.fpsCursor = fps.data();
        u16 previous = 0;
        for (unsigned frame = 0; frame <= buttons.size(); ++frame)
        {
            g_CurFrameRawInput = TH_BUTTON_MENU | TH_BUTTON_SHOOT | TH_BUTTON_SKIP;
            ReplayManager::OnUpdateDemoHighPrio(&playback);
            const u16 expected = frame == 0 ? 0 : buttons[frame - 1];
            assert(g_CurFrameGameInput == expected);
            assert(bool(WAS_PRESSED_GAME(TH_BUTTON_SHOOT)) ==
                   bool((expected & TH_BUTTON_SHOOT) && !(previous & TH_BUTTON_SHOOT)));
            assert(g_CurFrameRawInput & TH_BUTTON_MENU);
            assert(g_Supervisor.curFps == 60);
            previous = expected;
        }
        const auto *cursor = playback.replayInputs;
        const int frame = playback.frameId;
        g_GameManager.notInMenu = 0;
        ReplayManager::OnUpdateDemoHighPrio(&playback);
        assert(playback.frameId == frame && playback.replayInputs == cursor);
        g_GameManager.notInMenu = 1;
        cfg.slowMode = 1;
        ReplayManager::OnUpdateDemoHighPrio(&playback);
        assert(playback.frameId == frame && playback.replayInputs == cursor);
    }
}

static std::vector<u8> ReplayBytes(int declaredExtra = 0, int compressedExtra = 0)
{
    const size_t inputOffset = sizeof(ReplayHeader) + sizeof(ReplayData);
    const size_t fpsOffset = inputOffset + offsetof(StageReplayData, replayInputs) + 2 * sizeof(ReplayDataInput);
    std::vector<u8> plain(fpsOffset + 2 - sizeof(ReplayHeader));
    plain[plain.size() - 2] = plain.back() = 60;
    i32 compressedSize = 0;
    u8 *compressed = Lzss::Compress(plain.data(), plain.size(), &compressedSize);
    ReplayHeader header{};
    std::memcpy(&header.magic, "T7RP", 4);
    header.version = 0x1100;
    header.key = 64;
    header.sizeWithoutHeader = plain.size() + declaredExtra;
    header.replaySize = sizeof(ReplayHeader) + header.sizeWithoutHeader;
    header.compressedSize = compressedSize + compressedExtra;
    header.stageReplayDataOffsets[0] = inputOffset;
    header.stageEndDataOffsets[0] = fpsOffset;
    std::vector<u8> bytes(sizeof(header) + compressedSize);
    std::memcpy(bytes.data(), &header, sizeof(header));
    std::memcpy(bytes.data() + sizeof(header), compressed, compressedSize);
    std::free(compressed);
    u32 checksum = 0x3f000318;
    for (size_t i = offsetof(ReplayHeader, key); i < bytes.size(); ++i) checksum += bytes[i];
    std::memcpy(bytes.data() + offsetof(ReplayHeader, checksum), &checksum, sizeof(checksum));
    u8 key = header.key;
    for (size_t i = offsetof(ReplayHeader, replaySize); i < bytes.size(); ++i, key += 7) bytes[i] += key;
    return bytes;
}

static ReplayFile *Validate(const std::vector<u8> &bytes)
{
    auto *copy = static_cast<ReplayFile *>(std::malloc(std::max<size_t>(bytes.size(), 1)));
    std::memcpy(copy, bytes.data(), bytes.size());
    return ReplayManager::ValidateReplayData(copy, bytes.size());
}

static void CheckValidation()
{
    ReplayFile *valid = Validate(ReplayBytes());
    assert(valid && valid->stageReplayData[0] && valid->stageEndData[0]);
    ReplayManager::FreeReplay(valid);
    for (size_t size = 0; size < sizeof(ReplayHeader); ++size)
        assert(Validate(std::vector<u8>(size)) == nullptr);
    assert(Validate(ReplayBytes(1)) == nullptr);
    assert(Validate(ReplayBytes(-1)) == nullptr);
    assert(Validate(ReplayBytes(0, 1)) == nullptr);
}

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
static void CheckMultiplayerInput()
{
    GameConfiguration cfg{};
    Reset(cfg);
    ReplayExtension::multiplayer = true;
    ReplayExtension::players[0].buttons = TH_BUTTON_SHOOT;
    ReplayExtension::players[1].buttons = TH_BUTTON_LEFT;
    ReplayExtension::players[2].buttons = TH_BUTTON_SKIP;
    std::array<ReplayDataInput, 2> stream{};
    std::array<u8, 4> fps{60,60,60,60};
    ReplayManager playback{};
    playback.frameId = 0;
    playback.replayInputs = stream.data();
    playback.fpsCursor = fps.data();
    ReplayManager::OnUpdateDemoHighPrio(&playback);
    for (int player = 0; player < 3; ++player)
        assert(g_CurFrameGameInputs[player] == ReplayExtension::players[player].buttons);
    assert(WAS_PRESSED_GAME(TH_BUTTON_SHOOT));
    ReplayManager::OnUpdateDemoHighPrio(&playback);
    assert(!WAS_PRESSED_GAME(TH_BUTTON_SHOOT));
}
#endif

int main(int argc, char **argv)
{
    CheckRecordedInput();
    CheckValidation();
    for (int i = 1; i < argc; ++i)
    {
        std::ifstream file(argv[i], std::ios::binary);
        assert(file);
        const std::vector<u8> bytes((std::istreambuf_iterator<char>(file)), {});
        ReplayFile *parsed = Validate(bytes);
        assert(parsed);
        ReplayManager::FreeReplay(parsed);
        std::printf("TH07 Replay file validation: PASS %s\n", argv[i]);
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    CheckMultiplayerInput();
#endif
    std::puts("TH07 Replay input: PASS (stage 1-6/Extra/Phantasm, pipeline delay, Z/Ctrl edges, FPS, live-input isolation)");
}
