#include "ReplayExtension.hpp"
#include "Multiplayer.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <vector>

#include <SDL3/SDL_iostream.h>

#include "FileSystem.hpp"
#include "ReplayData.hpp"

namespace ReplayExtension
{
namespace
{
struct InputSample
{
    f32 x;
    f32 y;
    u32 active;
};
struct DirectTouchSample
{
    f32 x;
    f32 y;
    u32 flags;
};
struct PackedFrameInput
{
    u16 buttons;
    u8 analogMode;
    u8 flags;
    f32 x;
    f32 y;
};
struct MultiplayerInputSample
{
    PackedFrameInput players[3];
};
static_assert(sizeof(InputSample) == 12);
static_assert(sizeof(DirectTouchSample) == 12);
static_assert(sizeof(TouchEvent) == 28);
static_assert(sizeof(PackedFrameInput) == 12);
static_assert(sizeof(MultiplayerInputSample) == 36);

std::vector<InputSample> g_RecordInputs[7];
std::vector<DirectTouchSample> g_RecordDirectTouch[7];
std::vector<TouchEvent> g_RecordTouchEvents[7];
std::vector<MultiplayerInputSample> g_RecordMultiplayerInputs[7];
MultiplayerPlayerResourceSnapshot g_RecordMultiplayerStageResources[7][3]{};
u8 g_RecordMultiplayerStageResourceMask[7]{};
MultiplayerContributionSnapshot g_RecordMultiplayerStageContributions[7][3]{};
u8 g_RecordMultiplayerStageContributionMask[7]{};
std::vector<TouchEvent> g_PendingTouchEvents;
std::vector<InputSample> g_PlaybackInputs[7];
std::vector<DirectTouchSample> g_PlaybackDirectTouch[7];
std::vector<TouchEvent> g_PlaybackTouchEvents[7];
std::vector<MultiplayerInputSample> g_PlaybackMultiplayerInputs[7];
#ifdef TH_DEV_TOOLS
struct ExpectedMultiplayerFrame
{
    bool valid = false;
    u8 playerCount = 0;
    Netplay::FrameInput players[3]{};
};
std::vector<ExpectedMultiplayerFrame> g_ExpectedMultiplayerPlayback[7];
u32 g_ExpectedMultiplayerPlaybackFrames = 0;
u32 g_ComparedMultiplayerPlaybackFrames = 0;
bool g_MultiplayerPlaybackMismatch = false;
u32 g_ExpectedMultiplayerPlaybackCoverage = 0;
#endif
MultiplayerPlayerResourceSnapshot g_PlaybackMultiplayerStageResources[7][3]{};
u8 g_PlaybackMultiplayerStageResourceMask[7]{};
MultiplayerContributionSnapshot g_PlaybackMultiplayerStageContributions[7][3]{};
u8 g_PlaybackMultiplayerStageContributionMask[7]{};
InputSample g_CapturedJoystick = {};
DirectTouchSample g_CapturedDirectTouch = {};
bool g_RecordUsesExtension = false;
bool g_RecordHasTouchState = false;
bool g_RecordHasMultiplayer = false;
MultiplayerReplayConfig g_RecordMultiplayerConfig{};
u32 g_PlaybackVersion = 0;
u32 g_PlaybackFlags = 0;
MultiplayerReplayConfig g_PlaybackMultiplayerConfig{};
i32 g_PlaybackStage = -1;
i32 g_PlaybackFrame = -1;
std::size_t g_PlaybackTouchStart = 0;
std::size_t g_PlaybackTouchCount = 0;

constexpr u32 VERSION = 1;
constexpr u32 DETERMINISM_ABI = 1;
constexpr u32 FLAG_JOYSTICK_INPUT = 1;
constexpr u32 FLAG_TOUCH_EVENTS = 2;
constexpr u32 FLAG_DIRECT_TOUCH_INPUT = 4;
constexpr u32 FLAG_TOUCH_STATE = 8;
constexpr u32 FLAG_MULTIPLAYER_INPUT = 32;
constexpr u32 FLAG_MULTIPLAYER_STAGE_RESOURCES = 64;
constexpr u32 FLAG_MULTIPLAYER_STAGE_CONTRIBUTIONS = 128;
constexpr u32 DIRECT_TOUCH_ACTIVE = 1;
constexpr u32 DIRECT_TOUCH_UNLIMITED = 2;
constexpr u32 TOUCH_STATE_USED_THIS_RUN = 4;
constexpr u32 TOUCH_STATE_BOMBED_WITH_TOUCH = 8;
constexpr u32 TOUCH_STATE_CHEAT_MOVEMENT_USED = 16;
constexpr u32 TOUCH_STATE_MASK = TOUCH_STATE_USED_THIS_RUN | TOUCH_STATE_BOMBED_WITH_TOUCH |
                                 TOUCH_STATE_CHEAT_MOVEMENT_USED;
constexpr std::size_t HEADER_SIZE = 96;
constexpr std::size_t MP_HEADER_SIZE = 160;
constexpr std::size_t MP_PLAYER_COUNT_OFFSET = 96;
constexpr std::size_t MP_DIFFICULTY_OFFSET = 100;
constexpr std::size_t MP_GAMEPLAY_ABI_OFFSET = 104;
constexpr std::size_t MP_LOADOUT_OFFSET = 108;
constexpr std::size_t MP_LOCAL_PLAYER_OFFSET = 120;
constexpr std::size_t MP_SESSION_FLAGS_OFFSET = 124;
constexpr std::size_t MP_COUNTS_OFFSET = 128;
constexpr u32 MP_SESSION_HIDE_CONTRIBUTION_STATS = 1u;
constexpr u32 MP_SESSION_FLAGS_MASK = MP_SESSION_HIDE_CONTRIBUTION_STATS;
constexpr std::size_t MP_STAGE_RESOURCE_BYTES_PER_STAGE = 4 + 3 * 3 * 4;
constexpr std::size_t MP_STAGE_RESOURCE_BYTES = 7 * MP_STAGE_RESOURCE_BYTES_PER_STAGE;
constexpr std::size_t MP_STAGE_CONTRIBUTION_BYTES_PER_STAGE = 4 + 3 * 2 * 4;
constexpr std::size_t MP_STAGE_CONTRIBUTION_BYTES =
    7 * MP_STAGE_CONTRIBUTION_BYTES_PER_STAGE;

u8 PackInputFlags(const Netplay::FrameInput &input)
{
    return (input.unlimited ? 1u : 0u) |
           (input.touchUsed ? 2u : 0u) |
           (input.touchBomb ? 4u : 0u);
}

PackedFrameInput PackInput(const Netplay::FrameInput &input)
{
    return {input.buttons, static_cast<u8>(input.analogMode), PackInputFlags(input),
            input.x, input.y};
}

Netplay::FrameInput UnpackInput(const PackedFrameInput &input)
{
    Netplay::FrameInput result;
    result.buttons = input.buttons;
    result.analogMode = input.analogMode <= static_cast<u8>(Netplay::AnalogMode::DirectTouch)
                            ? static_cast<Netplay::AnalogMode>(input.analogMode)
                            : Netplay::AnalogMode::None;
    result.x = input.x;
    result.y = input.y;
    result.unlimited = (input.flags & 1u) != 0;
    result.touchUsed = (input.flags & 2u) != 0;
    result.touchBomb = (input.flags & 4u) != 0;
    return result;
}

u32 ReadLe32(const u8 *bytes)
{
    return static_cast<u32>(bytes[0]) |
           (static_cast<u32>(bytes[1]) << 8) |
           (static_cast<u32>(bytes[2]) << 16) |
           (static_cast<u32>(bytes[3]) << 24);
}

void WriteLe32(u8 *bytes, u32 value)
{
    bytes[0] = static_cast<u8>(value);
    bytes[1] = static_cast<u8>(value >> 8);
    bytes[2] = static_cast<u8>(value >> 16);
    bytes[3] = static_cast<u8>(value >> 24);
}

bool FindTrailer(const u8 *bytes, std::size_t size, std::size_t &payloadOffset, std::size_t &payloadSize,
                 u32 *outVersion = nullptr, u32 *outFlags = nullptr)
{
    if (bytes == nullptr || size < HEADER_SIZE + 8 || std::memcmp(bytes + size - 4, "EAGX", 4) != 0)
        return false;
    payloadSize = ReadLe32(bytes + size - 8);
    if (payloadSize < HEADER_SIZE || payloadSize > size - 8)
        return false;
    payloadOffset = size - 8 - payloadSize;
    const u32 version = ReadLe32(bytes + payloadOffset);
    const u32 flags = ReadLe32(bytes + payloadOffset + 4);
    if (version != VERSION)
        return false;
    const bool hasMultiplayer = (flags & FLAG_MULTIPLAYER_INPUT) != 0;
    const bool hasMultiplayerStageResources =
        (flags & FLAG_MULTIPLAYER_STAGE_RESOURCES) != 0;
    const bool hasMultiplayerStageContributions =
        (flags & FLAG_MULTIPLAYER_STAGE_CONTRIBUTIONS) != 0;
    if ((hasMultiplayerStageResources || hasMultiplayerStageContributions) && !hasMultiplayer)
        return false;
    const std::size_t headerSize = hasMultiplayer ? MP_HEADER_SIZE : HEADER_SIZE;
    if (payloadSize < headerSize || ReadLe32(bytes + payloadOffset + 92) != DETERMINISM_ABI)
        return false;
    if ((flags & (FLAG_JOYSTICK_INPUT | FLAG_TOUCH_EVENTS | FLAG_DIRECT_TOUCH_INPUT |
                  FLAG_TOUCH_STATE | FLAG_MULTIPLAYER_INPUT |
                  FLAG_MULTIPLAYER_STAGE_RESOURCES |
                  FLAG_MULTIPLAYER_STAGE_CONTRIBUTIONS)) == 0)
        return false;
    if (hasMultiplayer)
    {
        const u32 playerCount = ReadLe32(bytes + payloadOffset + MP_PLAYER_COUNT_OFFSET);
        const u32 difficulty = ReadLe32(bytes + payloadOffset + MP_DIFFICULTY_OFFSET);
        const u32 localPlayer = ReadLe32(bytes + payloadOffset + MP_LOCAL_PLAYER_OFFSET);
        const u32 sessionFlags = ReadLe32(bytes + payloadOffset + MP_SESSION_FLAGS_OFFSET);
        if (playerCount < 2 || playerCount > 3 || difficulty > 4 ||
            localPlayer >= playerCount || (sessionFlags & ~MP_SESSION_FLAGS_MASK) != 0)
            return false;
        for (u32 player = 0; player < playerCount; ++player)
        {
            const u32 loadout = ReadLe32(bytes + payloadOffset + MP_LOADOUT_OFFSET + player * 4);
            if ((loadout & 0xffu) > 1 || ((loadout >> 8) & 0xffu) > 1)
                return false;
        }
    }
    std::size_t expected = headerSize;
    for (i32 stage = 0; stage < 7; ++stage)
    {
        const u32 count = ReadLe32(bytes + payloadOffset + 8 + stage * 4);
        if (expected > payloadSize || count > (payloadSize - expected) / sizeof(InputSample))
            return false;
        expected += static_cast<std::size_t>(count) * sizeof(InputSample);
    }
    for (i32 stage = 0; stage < 7; ++stage)
    {
        const u32 count = ReadLe32(bytes + payloadOffset + 36 + stage * 4);
        if (expected > payloadSize || count > (payloadSize - expected) / sizeof(TouchEvent))
            return false;
        expected += static_cast<std::size_t>(count) * sizeof(TouchEvent);
    }
    for (i32 stage = 0; stage < 7; ++stage)
    {
        const u32 count = ReadLe32(bytes + payloadOffset + 64 + stage * 4);
        if (expected > payloadSize || count > (payloadSize - expected) / sizeof(DirectTouchSample))
            return false;
        expected += static_cast<std::size_t>(count) * sizeof(DirectTouchSample);
    }
    if (hasMultiplayer)
    {
        for (i32 stage = 0; stage < 7; ++stage)
        {
            const u32 count = ReadLe32(bytes + payloadOffset + MP_COUNTS_OFFSET + stage * 4);
            if (expected > payloadSize ||
                count > (payloadSize - expected) / sizeof(MultiplayerInputSample))
                return false;
            expected += static_cast<std::size_t>(count) * sizeof(MultiplayerInputSample);
        }
    }
    if (hasMultiplayerStageResources)
    {
        if (expected > payloadSize || MP_STAGE_RESOURCE_BYTES > payloadSize - expected)
            return false;
        const u32 playerCount = ReadLe32(bytes + payloadOffset + MP_PLAYER_COUNT_OFFSET);
        const u32 allowedMask = (1u << playerCount) - 1u;
        const u8 *resourceBytes = bytes + payloadOffset + expected;
        for (i32 stage = 0; stage < 7; ++stage)
        {
            const u32 mask = ReadLe32(resourceBytes);
            resourceBytes += 4;
            if ((mask & ~allowedMask) != 0)
                return false;
            for (u32 player = 0; player < 3; ++player)
            {
                for (u32 field = 0; field < 3; ++field)
                {
                    if (ReadLe32(resourceBytes) >
                        static_cast<u32>(std::numeric_limits<i32>::max()))
                        return false;
                    resourceBytes += 4;
                }
            }
        }
        expected += MP_STAGE_RESOURCE_BYTES;
    }
    if (hasMultiplayerStageContributions)
    {
        if (expected > payloadSize || MP_STAGE_CONTRIBUTION_BYTES > payloadSize - expected)
            return false;
        const u32 playerCount = ReadLe32(bytes + payloadOffset + MP_PLAYER_COUNT_OFFSET);
        const u32 allowedMask = (1u << playerCount) - 1u;
        const u8 *contributionBytes = bytes + payloadOffset + expected;
        for (i32 stage = 0; stage < 7; ++stage)
        {
            const u32 mask = ReadLe32(contributionBytes);
            contributionBytes += 4;
            if ((mask & ~allowedMask) != 0)
                return false;
            contributionBytes += 3 * 2 * 4;
        }
        expected += MP_STAGE_CONTRIBUTION_BYTES;
    }
    if (expected != payloadSize)
        return false;
    if (outVersion)
        *outVersion = version;
    if (outFlags)
        *outFlags = flags;
    return true;
}

void RecomputeRecordUsage()
{
    g_RecordUsesExtension = g_RecordHasMultiplayer;
    g_RecordHasTouchState = false;
    for (i32 stage = 0; stage < 7; ++stage)
    {
        for (const InputSample &sample : g_RecordInputs[stage])
        {
            if (sample.active)
                g_RecordUsesExtension = true;
        }
        if (!g_RecordTouchEvents[stage].empty())
            g_RecordUsesExtension = true;
        for (const DirectTouchSample &sample : g_RecordDirectTouch[stage])
        {
            if (sample.flags & DIRECT_TOUCH_ACTIVE)
                g_RecordUsesExtension = true;
            if (sample.flags & TOUCH_STATE_MASK)
            {
                g_RecordUsesExtension = true;
                g_RecordHasTouchState = true;
            }
        }
    }
}

bool EndsWith(const char *path, const char *suffix)
{
    if (!path || !suffix)
        return false;
    const std::size_t pathLength = std::strlen(path);
    const std::size_t suffixLength = std::strlen(suffix);
    if (suffixLength > pathLength)
        return false;
    return SDL_strcasecmp(path + pathLength - suffixLength, suffix) == 0;
}

std::string SwapExtension(const char *path, bool extended)
{
    std::string result = path ? path : "";
    if (EndsWith(result.c_str(), ".rpyx"))
        result.resize(result.size() - 5);
    else if (EndsWith(result.c_str(), ".rpy"))
        result.resize(result.size() - 4);
    result += extended ? ".rpyx" : ".rpy";
    return result;
}

bool ReadWholeFile(const char *path, std::vector<u8> &bytes)
{
    SDL_IOStream *file = FileSystem::OpenFileStream(path, "rb");
    if (!file)
        return false;
    const Sint64 size = SDL_GetIOSize(file);
    if (size <= 0 || size > 0x7fffffff || SDL_SeekIO(file, 0, SDL_IO_SEEK_SET) < 0)
    {
        SDL_CloseIO(file);
        return false;
    }
    bytes.resize(static_cast<std::size_t>(size));
    const bool ok = SDL_ReadIO(file, bytes.data(), bytes.size()) == bytes.size();
    SDL_CloseIO(file);
    if (!ok)
        bytes.clear();
    return ok;
}

u32 ReplayChecksumForRawBytes(const std::vector<u8> &bytes)
{
    if (bytes.size() <= offsetof(ReplayHeader, rngValue3))
        return 0;
    const u8 key = bytes[offsetof(ReplayHeader, key)];
    u8 rollingKey = key;
    u32 checksum = 0x3f000318u + key;
    for (std::size_t index = offsetof(ReplayHeader, rngValue3); index < bytes.size(); ++index)
    {
        checksum += static_cast<u8>(bytes[index] - rollingKey);
        rollingKey = static_cast<u8>(rollingKey + 7);
    }
    return checksum;
}
} // namespace

void ResetRecording()
{
    for (auto &stage : g_RecordInputs)
        stage.clear();
    for (auto &stage : g_RecordDirectTouch)
        stage.clear();
    for (auto &stage : g_RecordTouchEvents)
        stage.clear();
    for (auto &stage : g_RecordMultiplayerInputs)
        stage.clear();
    std::memset(g_RecordMultiplayerStageResources, 0, sizeof(g_RecordMultiplayerStageResources));
    std::memset(g_RecordMultiplayerStageResourceMask, 0, sizeof(g_RecordMultiplayerStageResourceMask));
    std::memset(g_RecordMultiplayerStageContributions, 0,
                sizeof(g_RecordMultiplayerStageContributions));
    std::memset(g_RecordMultiplayerStageContributionMask, 0,
                sizeof(g_RecordMultiplayerStageContributionMask));
    g_PendingTouchEvents.clear();
    g_CapturedJoystick = {};
    g_CapturedDirectTouch = {};
    g_RecordUsesExtension = false;
    g_RecordHasTouchState = false;
    g_RecordHasMultiplayer = false;
    g_RecordMultiplayerConfig = {};
}

void BeginStageRecording(i32 stage)
{
    stage = std::clamp(stage, 0, 6);
    g_RecordInputs[stage].clear();
    g_RecordDirectTouch[stage].clear();
    g_RecordTouchEvents[stage].clear();
    g_RecordMultiplayerInputs[stage].clear();
    std::memset(g_RecordMultiplayerStageResources[stage], 0,
                sizeof(g_RecordMultiplayerStageResources[stage]));
    g_RecordMultiplayerStageResourceMask[stage] = 0;
    std::memset(g_RecordMultiplayerStageContributions[stage], 0,
                sizeof(g_RecordMultiplayerStageContributions[stage]));
    g_RecordMultiplayerStageContributionMask[stage] = 0;
    g_PendingTouchEvents.clear();
    g_CapturedJoystick = {};
    g_CapturedDirectTouch = {};
    RecomputeRecordUsage();
}

void BeginMultiplayerRecording(const MultiplayerReplayConfig &config)
{
    if (config.playerCount < 2 || config.playerCount > 3 || config.difficulty > 4)
        return;
    for (u8 playerId = 0; playerId < config.playerCount; ++playerId)
    {
        if (config.characters[playerId] > 1 || config.shots[playerId] > 1)
            return;
    }
    g_RecordMultiplayerConfig = config;
    g_RecordHasMultiplayer = true;
    g_RecordUsesExtension = true;
}

void CaptureMultiplayerStageResources(i32 stage,
                                      const MultiplayerPlayerResourceSnapshot *resources,
                                      std::size_t count)
{
    if (!g_RecordHasMultiplayer || !resources ||
        count < g_RecordMultiplayerConfig.playerCount)
        return;
    stage = std::clamp(stage, 0, 6);
    u8 mask = 0;
    for (u8 playerId = 0; playerId < g_RecordMultiplayerConfig.playerCount; ++playerId)
    {
        g_RecordMultiplayerStageResources[stage][playerId] = resources[playerId];
        mask |= static_cast<u8>(1u << playerId);
    }
    g_RecordMultiplayerStageResourceMask[stage] = mask;
    g_RecordUsesExtension = true;
}

void CaptureMultiplayerStageContributions(
    i32 stage, const MultiplayerContributionSnapshot *contributions,
    std::size_t count)
{
    if (!g_RecordHasMultiplayer || !contributions ||
        count < g_RecordMultiplayerConfig.playerCount)
        return;
    stage = std::clamp(stage, 0, 6);
    u8 mask = 0;
    for (u8 playerId = 0; playerId < g_RecordMultiplayerConfig.playerCount; ++playerId)
    {
        g_RecordMultiplayerStageContributions[stage][playerId] = contributions[playerId];
        mask |= static_cast<u8>(1u << playerId);
    }
    g_RecordMultiplayerStageContributionMask[stage] = mask;
    g_RecordUsesExtension = true;
}

void RecordMultiplayerFrame(i32 stage, i32 frame, const Netplay::FrameInput *inputs,
                            std::size_t count)
{
    if (!g_RecordHasMultiplayer || !inputs || frame < 0 ||
        count < g_RecordMultiplayerConfig.playerCount)
        return;
    stage = std::clamp(stage, 0, 6);
    auto &samples = g_RecordMultiplayerInputs[stage];
    if (samples.size() <= static_cast<std::size_t>(frame))
        samples.resize(static_cast<std::size_t>(frame) + 1, {});
    MultiplayerInputSample &sample = samples[frame];
    for (u32 player = 0; player < 3; ++player)
        sample.players[player] = player < g_RecordMultiplayerConfig.playerCount
                                     ? PackInput(inputs[player])
                                     : PackedFrameInput{};
    g_RecordUsesExtension = true;
}

void BeginInputFrame()
{
    g_CapturedJoystick = {};
    g_CapturedDirectTouch = {};
}

void CaptureJoystick(f32 x, f32 y)
{
    g_CapturedJoystick = {x, y, 1};
    g_RecordUsesExtension = true;
}

void CaptureDirectTouch(f32 x, f32 y, bool unlimited)
{
    g_CapturedDirectTouch = {x, y, DIRECT_TOUCH_ACTIVE | (unlimited ? DIRECT_TOUCH_UNLIMITED : 0)};
    g_RecordUsesExtension = true;
}

void CaptureTouchState(bool usedThisRun, bool bombedWithTouch, bool cheatMovementUsed)
{
    if (!usedThisRun && !bombedWithTouch && !cheatMovementUsed && !g_RecordHasTouchState)
        return;
    g_RecordHasTouchState = true;
    g_RecordUsesExtension = true;
    g_CapturedDirectTouch.flags &= ~TOUCH_STATE_MASK;
    if (usedThisRun)
        g_CapturedDirectTouch.flags |= TOUCH_STATE_USED_THIS_RUN;
    if (bombedWithTouch)
        g_CapturedDirectTouch.flags |= TOUCH_STATE_BOMBED_WITH_TOUCH;
    if (cheatMovementUsed)
        g_CapturedDirectTouch.flags |= TOUCH_STATE_CHEAT_MOVEMENT_USED;
}

void CaptureTouchEvent(i32 fingerId, f32 x, f32 y, u32 action, u32 role, u32 flags)
{
    if (action < TOUCH_ACTION_DOWN || action > TOUCH_ACTION_CANCEL_ALL)
        return;
    g_PendingTouchEvents.push_back({0, fingerId, x, y, action, role, flags});
    g_RecordUsesExtension = true;
}

void CaptureTouchCancelAll()
{
    CaptureTouchEvent(0, 0.0f, 0.0f, TOUCH_ACTION_CANCEL_ALL, TOUCH_ROLE_NONE, 0);
}

void RecordFrame(i32 stage, i32 frame)
{
    stage = std::clamp(stage, 0, 6);
    if (frame < 0)
        return;
    auto &samples = g_RecordInputs[stage];
    if (samples.size() <= static_cast<std::size_t>(frame))
        samples.resize(static_cast<std::size_t>(frame) + 1, {0.0f, 0.0f, 0});
    samples[frame] = g_CapturedJoystick;
    g_CapturedJoystick = {};

    auto &touchSamples = g_RecordDirectTouch[stage];
    if (touchSamples.size() <= static_cast<std::size_t>(frame))
        touchSamples.resize(static_cast<std::size_t>(frame) + 1, {0.0f, 0.0f, 0});
    touchSamples[frame] = g_CapturedDirectTouch;
    g_CapturedDirectTouch = {};

    if (!g_PendingTouchEvents.empty())
    {
        auto &events = g_RecordTouchEvents[stage];
        for (TouchEvent &event : g_PendingTouchEvents)
            event.frame = static_cast<u32>(frame);
        events.insert(events.end(), g_PendingTouchEvents.begin(), g_PendingTouchEvents.end());
        g_PendingTouchEvents.clear();
    }
}

std::string ResolveSavePath(const char *requestedPath)
{
    return SwapExtension(requestedPath, g_RecordUsesExtension);
}

void RemoveAlternateSave(const char *savedPath)
{
    const std::string alternate = SwapExtension(savedPath, !EndsWith(savedPath, ".rpyx"));
    const std::string actual = FileSystem::GetPrefPath(alternate.c_str());
    std::remove(actual.c_str());
}

bool AppendRecording(const char *path)
{
    if (!g_RecordUsesExtension)
        return true;

    std::vector<u8> bytes;
    if (!ReadWholeFile(path, bytes) || bytes.size() < sizeof(ReplayHeader) || std::memcmp(bytes.data(), "T6RP", 4) != 0)
        return false;

    std::size_t oldOffset = 0;
    std::size_t oldSize = 0;
    if (FindTrailer(bytes.data(), bytes.size(), oldOffset, oldSize))
        bytes.resize(oldOffset);

    u32 flags = 0;
    std::size_t bodySize = 0;
    for (const auto &stage : g_RecordInputs)
    {
        bodySize += stage.size() * sizeof(InputSample);
        for (const InputSample &sample : stage)
            if (sample.active)
                flags |= FLAG_JOYSTICK_INPUT;
    }
    for (const auto &stage : g_RecordTouchEvents)
    {
        bodySize += stage.size() * sizeof(TouchEvent);
        if (!stage.empty())
            flags |= FLAG_TOUCH_EVENTS;
    }
    for (const auto &stage : g_RecordDirectTouch)
    {
        bodySize += stage.size() * sizeof(DirectTouchSample);
        for (const DirectTouchSample &sample : stage)
        {
            if (sample.flags & DIRECT_TOUCH_ACTIVE)
                flags |= FLAG_DIRECT_TOUCH_INPUT;
            if (sample.flags & TOUCH_STATE_MASK)
                flags |= FLAG_TOUCH_STATE;
        }
    }
    if (g_RecordHasTouchState)
        flags |= FLAG_TOUCH_STATE;
    const bool hasMultiplayer = g_RecordHasMultiplayer &&
                                g_RecordMultiplayerConfig.playerCount >= 2 &&
                                g_RecordMultiplayerConfig.playerCount <= 3;
    if (hasMultiplayer)
    {
        flags |= FLAG_MULTIPLAYER_INPUT;
        for (const auto &stage : g_RecordMultiplayerInputs)
            bodySize += stage.size() * sizeof(MultiplayerInputSample);
        bool hasStageResources = false;
        for (u8 mask : g_RecordMultiplayerStageResourceMask)
            hasStageResources = hasStageResources || mask != 0;
        if (hasStageResources)
        {
            flags |= FLAG_MULTIPLAYER_STAGE_RESOURCES;
            bodySize += MP_STAGE_RESOURCE_BYTES;
        }
        bool hasStageContributions = false;
        for (u8 mask : g_RecordMultiplayerStageContributionMask)
            hasStageContributions = hasStageContributions || mask != 0;
        if (hasStageContributions)
        {
            flags |= FLAG_MULTIPLAYER_STAGE_CONTRIBUTIONS;
            bodySize += MP_STAGE_CONTRIBUTION_BYTES;
        }
    }
    if (flags == 0)
        return true;
    const std::size_t headerSize = hasMultiplayer ? MP_HEADER_SIZE : HEADER_SIZE;
    const std::size_t payloadSize = headerSize + bodySize;
    const std::size_t payloadOffset = bytes.size();
    bytes.resize(payloadOffset + payloadSize + 8, 0);
    WriteLe32(bytes.data() + payloadOffset, VERSION);
    WriteLe32(bytes.data() + payloadOffset + 4, flags);
    WriteLe32(bytes.data() + payloadOffset + 92, DETERMINISM_ABI);
    if (hasMultiplayer)
    {
        WriteLe32(bytes.data() + payloadOffset + MP_PLAYER_COUNT_OFFSET,
                  g_RecordMultiplayerConfig.playerCount);
        WriteLe32(bytes.data() + payloadOffset + MP_DIFFICULTY_OFFSET,
                  g_RecordMultiplayerConfig.difficulty);
        WriteLe32(bytes.data() + payloadOffset + MP_GAMEPLAY_ABI_OFFSET,
                  g_RecordMultiplayerConfig.gameplayAbi);
        WriteLe32(bytes.data() + payloadOffset + MP_LOCAL_PLAYER_OFFSET,
                  g_RecordMultiplayerConfig.localPlayer);
        WriteLe32(bytes.data() + payloadOffset + MP_SESSION_FLAGS_OFFSET,
                  g_RecordMultiplayerConfig.showContributionStats
                      ? 0u : MP_SESSION_HIDE_CONTRIBUTION_STATS);
        for (u32 player = 0; player < 3; ++player)
        {
            const u32 loadout = static_cast<u32>(g_RecordMultiplayerConfig.characters[player]) |
                                (static_cast<u32>(g_RecordMultiplayerConfig.shots[player]) << 8);
            WriteLe32(bytes.data() + payloadOffset + MP_LOADOUT_OFFSET + player * 4,
                      loadout);
        }
    }

    std::size_t cursor = payloadOffset + headerSize;
    for (i32 stage = 0; stage < 7; ++stage)
    {
        WriteLe32(bytes.data() + payloadOffset + 8 + stage * 4, static_cast<u32>(g_RecordInputs[stage].size()));
        WriteLe32(bytes.data() + payloadOffset + 36 + stage * 4,
                  static_cast<u32>(g_RecordTouchEvents[stage].size()));
        WriteLe32(bytes.data() + payloadOffset + 64 + stage * 4,
                  static_cast<u32>(g_RecordDirectTouch[stage].size()));
        const std::size_t stageBytes = g_RecordInputs[stage].size() * sizeof(InputSample);
        if (stageBytes != 0)
        {
            std::memcpy(bytes.data() + cursor, g_RecordInputs[stage].data(), stageBytes);
            cursor += stageBytes;
        }
    }
    for (i32 stage = 0; stage < 7; ++stage)
    {
        const std::size_t stageBytes = g_RecordTouchEvents[stage].size() * sizeof(TouchEvent);
        if (stageBytes != 0)
        {
            std::memcpy(bytes.data() + cursor, g_RecordTouchEvents[stage].data(), stageBytes);
            cursor += stageBytes;
        }
    }
    for (i32 stage = 0; stage < 7; ++stage)
    {
        const std::size_t stageBytes = g_RecordDirectTouch[stage].size() * sizeof(DirectTouchSample);
        if (stageBytes != 0)
        {
            std::memcpy(bytes.data() + cursor, g_RecordDirectTouch[stage].data(), stageBytes);
            cursor += stageBytes;
        }
    }
    // Keep the established v1 body order identical to TH07 and to
    // LoadPlayback(): ordinary input, touch events, direct-touch/state, then
    // MP input lanes. Optional TH06 per-stage resource snapshots append after
    // those established sections, so old v1 files without the feature flag
    // remain byte-for-byte readable by the same parser.
    // Placing MP lanes before touch data works only when the touch sections are
    // empty and corrupts combined multiplayer + local-touch Replay playback.
    if (hasMultiplayer)
    {
        for (i32 stage = 0; stage < 7; ++stage)
        {
            WriteLe32(bytes.data() + payloadOffset + MP_COUNTS_OFFSET + stage * 4,
                      static_cast<u32>(g_RecordMultiplayerInputs[stage].size()));
            const std::size_t stageBytes =
                g_RecordMultiplayerInputs[stage].size() * sizeof(MultiplayerInputSample);
            if (stageBytes != 0)
            {
                std::memcpy(bytes.data() + cursor, g_RecordMultiplayerInputs[stage].data(), stageBytes);
                cursor += stageBytes;
            }
        }
    }
    if ((flags & FLAG_MULTIPLAYER_STAGE_RESOURCES) != 0)
    {
        for (i32 stage = 0; stage < 7; ++stage)
        {
            WriteLe32(bytes.data() + cursor, g_RecordMultiplayerStageResourceMask[stage]);
            cursor += 4;
            for (u32 player = 0; player < 3; ++player)
            {
                const MultiplayerPlayerResourceSnapshot &resources =
                    g_RecordMultiplayerStageResources[stage][player];
                WriteLe32(bytes.data() + cursor, static_cast<u32>(resources.lives));
                cursor += 4;
                WriteLe32(bytes.data() + cursor, static_cast<u32>(resources.bombs));
                cursor += 4;
                WriteLe32(bytes.data() + cursor, static_cast<u32>(resources.power));
                cursor += 4;
            }
        }
    }
    if ((flags & FLAG_MULTIPLAYER_STAGE_CONTRIBUTIONS) != 0)
    {
        for (i32 stage = 0; stage < 7; ++stage)
        {
            WriteLe32(bytes.data() + cursor, g_RecordMultiplayerStageContributionMask[stage]);
            cursor += 4;
            for (u32 player = 0; player < 3; ++player)
            {
                const MultiplayerContributionSnapshot &contributions =
                    g_RecordMultiplayerStageContributions[stage][player];
                WriteLe32(bytes.data() + cursor, contributions.enemiesDefeated);
                cursor += 4;
                WriteLe32(bytes.data() + cursor, contributions.damageDealt);
                cursor += 4;
            }
        }
    }
    if (cursor != payloadOffset + payloadSize)
        return false;
    WriteLe32(bytes.data() + payloadOffset + payloadSize, static_cast<u32>(payloadSize));
    std::memcpy(bytes.data() + payloadOffset + payloadSize + 4, "EAGX", 4);

    WriteLe32(bytes.data() + offsetof(ReplayHeader, checksum), ReplayChecksumForRawBytes(bytes));
    return FileSystem::WriteDataToFile(path, bytes.data(), bytes.size()) == 0;
}

void ClearPlayback()
{
    for (auto &stage : g_PlaybackInputs)
        stage.clear();
    for (auto &stage : g_PlaybackDirectTouch)
        stage.clear();
    for (auto &stage : g_PlaybackTouchEvents)
        stage.clear();
    for (auto &stage : g_PlaybackMultiplayerInputs)
        stage.clear();
    std::memset(g_PlaybackMultiplayerStageResources, 0,
                sizeof(g_PlaybackMultiplayerStageResources));
    std::memset(g_PlaybackMultiplayerStageResourceMask, 0,
                sizeof(g_PlaybackMultiplayerStageResourceMask));
    std::memset(g_PlaybackMultiplayerStageContributions, 0,
                sizeof(g_PlaybackMultiplayerStageContributions));
    std::memset(g_PlaybackMultiplayerStageContributionMask, 0,
                sizeof(g_PlaybackMultiplayerStageContributionMask));
    g_PlaybackVersion = 0;
    g_PlaybackFlags = 0;
    g_PlaybackMultiplayerConfig = {};
    g_PlaybackStage = -1;
    g_PlaybackFrame = -1;
    g_PlaybackTouchStart = 0;
    g_PlaybackTouchCount = 0;
}

bool LoadPlayback(const u8 *bytes, std::size_t size)
{
    ClearPlayback();
    std::size_t payloadOffset = 0;
    std::size_t payloadSize = 0;
    u32 version = 0;
    u32 flags = 0;
    if (!FindTrailer(bytes, size, payloadOffset, payloadSize, &version, &flags))
        return false;
    g_PlaybackVersion = version;
    g_PlaybackFlags = flags;
    const bool hasMultiplayer = (flags & FLAG_MULTIPLAYER_INPUT) != 0;
    const std::size_t headerSize = hasMultiplayer ? MP_HEADER_SIZE : HEADER_SIZE;
    if (hasMultiplayer)
    {
        g_PlaybackMultiplayerConfig.playerCount =
            static_cast<u8>(ReadLe32(bytes + payloadOffset + MP_PLAYER_COUNT_OFFSET));
        g_PlaybackMultiplayerConfig.difficulty =
            static_cast<u8>(ReadLe32(bytes + payloadOffset + MP_DIFFICULTY_OFFSET));
        g_PlaybackMultiplayerConfig.gameplayAbi =
            ReadLe32(bytes + payloadOffset + MP_GAMEPLAY_ABI_OFFSET);
        g_PlaybackMultiplayerConfig.localPlayer =
            static_cast<u8>(ReadLe32(bytes + payloadOffset + MP_LOCAL_PLAYER_OFFSET));
        const u32 sessionFlags =
            ReadLe32(bytes + payloadOffset + MP_SESSION_FLAGS_OFFSET);
        g_PlaybackMultiplayerConfig.showContributionStats =
            (sessionFlags & MP_SESSION_HIDE_CONTRIBUTION_STATS) == 0;
        for (u32 player = 0; player < 3; ++player)
        {
            const u32 loadout = ReadLe32(bytes + payloadOffset + MP_LOADOUT_OFFSET + player * 4);
            g_PlaybackMultiplayerConfig.characters[player] = static_cast<u8>(loadout & 0xffu);
            g_PlaybackMultiplayerConfig.shots[player] = static_cast<u8>((loadout >> 8) & 0xffu);
        }
    }
    std::size_t cursor = payloadOffset + headerSize;
    for (i32 stage = 0; stage < 7; ++stage)
    {
        const u32 count = ReadLe32(bytes + payloadOffset + 8 + stage * 4);
        g_PlaybackInputs[stage].resize(count);
        const std::size_t stageBytes = static_cast<std::size_t>(count) * sizeof(InputSample);
        if (stageBytes != 0)
        {
            std::memcpy(g_PlaybackInputs[stage].data(), bytes + cursor, stageBytes);
            cursor += stageBytes;
        }
    }
    for (i32 stage = 0; stage < 7; ++stage)
    {
        const u32 count = ReadLe32(bytes + payloadOffset + 36 + stage * 4);
        g_PlaybackTouchEvents[stage].resize(count);
        const std::size_t stageBytes = static_cast<std::size_t>(count) * sizeof(TouchEvent);
        if (stageBytes != 0)
        {
            std::memcpy(g_PlaybackTouchEvents[stage].data(), bytes + cursor, stageBytes);
            cursor += stageBytes;
        }
    }
    for (i32 stage = 0; stage < 7; ++stage)
    {
        const u32 count = ReadLe32(bytes + payloadOffset + 64 + stage * 4);
        g_PlaybackDirectTouch[stage].resize(count);
        const std::size_t stageBytes = static_cast<std::size_t>(count) * sizeof(DirectTouchSample);
        if (stageBytes != 0)
        {
            std::memcpy(g_PlaybackDirectTouch[stage].data(), bytes + cursor, stageBytes);
            cursor += stageBytes;
        }
    }
    if (hasMultiplayer)
    {
        for (i32 stage = 0; stage < 7; ++stage)
        {
            const u32 count = ReadLe32(bytes + payloadOffset + MP_COUNTS_OFFSET + stage * 4);
            g_PlaybackMultiplayerInputs[stage].resize(count);
            const std::size_t stageBytes =
                static_cast<std::size_t>(count) * sizeof(MultiplayerInputSample);
            if (stageBytes != 0)
            {
                std::memcpy(g_PlaybackMultiplayerInputs[stage].data(), bytes + cursor, stageBytes);
                cursor += stageBytes;
            }
        }
    }
    if ((flags & FLAG_MULTIPLAYER_STAGE_RESOURCES) != 0)
    {
        for (i32 stage = 0; stage < 7; ++stage)
        {
            g_PlaybackMultiplayerStageResourceMask[stage] =
                static_cast<u8>(ReadLe32(bytes + cursor));
            cursor += 4;
            for (u32 player = 0; player < 3; ++player)
            {
                MultiplayerPlayerResourceSnapshot &resources =
                    g_PlaybackMultiplayerStageResources[stage][player];
                resources.lives = static_cast<i32>(ReadLe32(bytes + cursor));
                cursor += 4;
                resources.bombs = static_cast<i32>(ReadLe32(bytes + cursor));
                cursor += 4;
                resources.power = static_cast<i32>(ReadLe32(bytes + cursor));
                cursor += 4;
            }
        }
    }
    if ((flags & FLAG_MULTIPLAYER_STAGE_CONTRIBUTIONS) != 0)
    {
        for (i32 stage = 0; stage < 7; ++stage)
        {
            g_PlaybackMultiplayerStageContributionMask[stage] =
                static_cast<u8>(ReadLe32(bytes + cursor));
            cursor += 4;
            for (u32 player = 0; player < 3; ++player)
            {
                MultiplayerContributionSnapshot &contributions =
                    g_PlaybackMultiplayerStageContributions[stage][player];
                contributions.enemiesDefeated = ReadLe32(bytes + cursor);
                cursor += 4;
                contributions.damageDealt = ReadLe32(bytes + cursor);
                cursor += 4;
            }
        }
    }
    if (cursor != payloadOffset + payloadSize)
    {
        ClearPlayback();
        return false;
    }
    return true;
}

void SetPlaybackFrame(i32 stage, i32 frame)
{
    g_PlaybackStage = std::clamp(stage, 0, 6);
    g_PlaybackFrame = frame;
    g_PlaybackTouchStart = 0;
    g_PlaybackTouchCount = 0;
    if (g_PlaybackVersion != VERSION || frame < 0)
        return;

    const auto &events = g_PlaybackTouchEvents[g_PlaybackStage];
    const auto first = std::lower_bound(events.begin(), events.end(), static_cast<u32>(frame),
                                        [](const TouchEvent &event, u32 value) { return event.frame < value; });
    const auto last = std::upper_bound(first, events.end(), static_cast<u32>(frame),
                                       [](u32 value, const TouchEvent &event) { return value < event.frame; });
    g_PlaybackTouchStart = static_cast<std::size_t>(first - events.begin());
    g_PlaybackTouchCount = static_cast<std::size_t>(last - first);
}

bool PlaybackActive()
{
    return g_PlaybackVersion != 0;
}

bool MultiplayerPlaybackActive()
{
    return g_PlaybackVersion == VERSION &&
           (g_PlaybackFlags & FLAG_MULTIPLAYER_INPUT) != 0 &&
           g_PlaybackMultiplayerConfig.playerCount >= 2;
}

bool GetMultiplayerPlaybackConfig(MultiplayerReplayConfig *out)
{
    if (!out || !MultiplayerPlaybackActive())
        return false;
    *out = g_PlaybackMultiplayerConfig;
    return true;
}

bool GetMultiplayerPlaybackStageResources(i32 stage, u8 playerId,
                                          MultiplayerPlayerResourceSnapshot *out)
{
    if (!out || !MultiplayerPlaybackActive() ||
        (g_PlaybackFlags & FLAG_MULTIPLAYER_STAGE_RESOURCES) == 0 ||
        playerId >= g_PlaybackMultiplayerConfig.playerCount)
        return false;
    stage = std::clamp(stage, 0, 6);
    if ((g_PlaybackMultiplayerStageResourceMask[stage] & (1u << playerId)) == 0)
        return false;
    *out = g_PlaybackMultiplayerStageResources[stage][playerId];
    return true;
}

bool GetMultiplayerPlaybackStageContributions(
    i32 stage, u8 playerId, MultiplayerContributionSnapshot *out)
{
    if (!out || !MultiplayerPlaybackActive() ||
        (g_PlaybackFlags & FLAG_MULTIPLAYER_STAGE_CONTRIBUTIONS) == 0 ||
        playerId >= g_PlaybackMultiplayerConfig.playerCount)
        return false;
    stage = std::clamp(stage, 0, 6);
    if ((g_PlaybackMultiplayerStageContributionMask[stage] & (1u << playerId)) == 0)
        return false;
    *out = g_PlaybackMultiplayerStageContributions[stage][playerId];
    return true;
}

bool GetMultiplayerPlaybackFrame(i32 stage, i32 frame, Netplay::FrameInput *inputs,
                                 std::size_t count)
{
    if (!inputs || !MultiplayerPlaybackActive() || frame < 0 ||
        count < g_PlaybackMultiplayerConfig.playerCount)
        return false;
    stage = std::clamp(stage, 0, 6);
    const auto &samples = g_PlaybackMultiplayerInputs[stage];
    if (static_cast<std::size_t>(frame) >= samples.size())
        return false;
    for (u32 player = 0; player < g_PlaybackMultiplayerConfig.playerCount; ++player)
        inputs[player] = UnpackInput(samples[frame].players[player]);
    for (u32 player = g_PlaybackMultiplayerConfig.playerCount; player < count; ++player)
        inputs[player] = {};
    return true;
}

bool UsesFixedTickTouchPlayback()
{
    return g_PlaybackVersion == VERSION;
}

bool GetPlaybackJoystick(f32 *x, f32 *y)
{
    if (MultiplayerPlaybackActive())
        return false;
    if (g_PlaybackVersion != VERSION ||
        g_PlaybackStage < 0 || g_PlaybackFrame < 0 ||
        static_cast<std::size_t>(g_PlaybackFrame) >= g_PlaybackInputs[g_PlaybackStage].size())
        return false;
    const InputSample &sample = g_PlaybackInputs[g_PlaybackStage][g_PlaybackFrame];
    if (!sample.active)
        return false;
    *x = sample.x;
    *y = sample.y;
    return true;
}

bool GetPlaybackDirectTouch(f32 *x, f32 *y, bool *unlimited)
{
    if (MultiplayerPlaybackActive())
        return false;
    if (g_PlaybackVersion != VERSION ||
        g_PlaybackStage < 0 || g_PlaybackFrame < 0 ||
        static_cast<std::size_t>(g_PlaybackFrame) >= g_PlaybackDirectTouch[g_PlaybackStage].size())
        return false;
    const DirectTouchSample &sample = g_PlaybackDirectTouch[g_PlaybackStage][g_PlaybackFrame];
    if ((sample.flags & DIRECT_TOUCH_ACTIVE) == 0)
        return false;
    *x = sample.x;
    *y = sample.y;
    if (unlimited)
        *unlimited = (sample.flags & DIRECT_TOUCH_UNLIMITED) != 0;
    return true;
}

bool GetPlaybackTouchState(bool *usedThisRun, bool *bombedWithTouch, bool *cheatMovementUsed)
{
    if (g_PlaybackVersion != VERSION ||
        (g_PlaybackFlags & FLAG_TOUCH_STATE) == 0 ||
        g_PlaybackStage < 0 || g_PlaybackFrame < 0 ||
        static_cast<std::size_t>(g_PlaybackFrame) >= g_PlaybackDirectTouch[g_PlaybackStage].size())
        return false;
    const u32 flags = g_PlaybackDirectTouch[g_PlaybackStage][g_PlaybackFrame].flags;
    if (usedThisRun)
        *usedThisRun = (flags & TOUCH_STATE_USED_THIS_RUN) != 0;
    if (bombedWithTouch)
        *bombedWithTouch = (flags & TOUCH_STATE_BOMBED_WITH_TOUCH) != 0;
    if (cheatMovementUsed)
        *cheatMovementUsed = (flags & TOUCH_STATE_CHEAT_MOVEMENT_USED) != 0;
    return true;
}

const TouchEvent *GetPlaybackTouchEvents(std::size_t *count)
{
    if (count)
        *count = g_PlaybackTouchCount;
    if (g_PlaybackVersion != VERSION ||
        g_PlaybackStage < 0 || g_PlaybackTouchCount == 0)
        return nullptr;
    return g_PlaybackTouchEvents[g_PlaybackStage].data() + g_PlaybackTouchStart;
}

std::size_t BaseFileSize(const u8 *bytes, std::size_t size)
{
    std::size_t payloadOffset = 0;
    std::size_t payloadSize = 0;
    return FindTrailer(bytes, size, payloadOffset, payloadSize) ? payloadOffset : size;
}

bool MatchesPath(const char *path, const u8 *bytes, std::size_t size)
{
    const bool extendedPath = EndsWith(path, ".rpyx");
    std::size_t payloadOffset = 0;
    std::size_t payloadSize = 0;
    const bool extendedData = FindTrailer(bytes, size, payloadOffset, payloadSize);
    return extendedPath == extendedData;
}

#ifdef TH_DEV_TOOLS
void DebugResetMultiplayerPlaybackAudit()
{
    for (auto &stage : g_ExpectedMultiplayerPlayback)
        stage.clear();
    g_ExpectedMultiplayerPlaybackFrames = 0;
    g_ComparedMultiplayerPlaybackFrames = 0;
    g_MultiplayerPlaybackMismatch = false;
    g_ExpectedMultiplayerPlaybackCoverage = 0;
}

void DebugExpectMultiplayerPlaybackFrame(i32 stage, i32 frame,
                                         const Netplay::FrameInput *inputs,
                                         std::size_t count)
{
    if (!inputs || stage < 0 || stage >= 7 || frame < 0 || count < 2 || count > 3)
        return;
    auto &frames = g_ExpectedMultiplayerPlayback[stage];
    if (frames.size() <= static_cast<std::size_t>(frame))
        frames.resize(static_cast<std::size_t>(frame) + 1);
    ExpectedMultiplayerFrame &expected = frames[frame];
    if (!expected.valid)
        ++g_ExpectedMultiplayerPlaybackFrames;
    expected.valid = true;
    expected.playerCount = static_cast<u8>(count);
    for (std::size_t player = 0; player < count; ++player)
    {
        expected.players[player] = inputs[player];
        if (inputs[player].analogMode == Netplay::AnalogMode::Joystick)
            g_ExpectedMultiplayerPlaybackCoverage |= 1u;
        if (inputs[player].analogMode == Netplay::AnalogMode::DirectTouch)
            g_ExpectedMultiplayerPlaybackCoverage |= 2u;
        if (inputs[player].unlimited)
            g_ExpectedMultiplayerPlaybackCoverage |= 4u;
        if (inputs[player].touchUsed)
            g_ExpectedMultiplayerPlaybackCoverage |= 8u;
        if (inputs[player].touchBomb)
            g_ExpectedMultiplayerPlaybackCoverage |= 16u;
    }
}

int DebugAuditMultiplayerPlaybackFrame(i32 stage, i32 frame,
                                       const Netplay::FrameInput *inputs,
                                       std::size_t count)
{
    if (!inputs || stage < 0 || stage >= 7 || frame < 0)
        return -1;
    const auto &frames = g_ExpectedMultiplayerPlayback[stage];
    if (static_cast<std::size_t>(frame) >= frames.size() || !frames[frame].valid)
        return -1;
    const ExpectedMultiplayerFrame &expected = frames[frame];
    bool match = count >= expected.playerCount;
    for (u8 player = 0; match && player < expected.playerCount; ++player)
        match = inputs[player] == expected.players[player];
    ++g_ComparedMultiplayerPlaybackFrames;
    g_MultiplayerPlaybackMismatch = g_MultiplayerPlaybackMismatch || !match;
    return match ? 1 : 0;
}

u32 DebugExpectedMultiplayerPlaybackFrames()
{
    return g_ExpectedMultiplayerPlaybackFrames;
}

u32 DebugComparedMultiplayerPlaybackFrames()
{
    return g_ComparedMultiplayerPlaybackFrames;
}

bool DebugMultiplayerPlaybackMismatch()
{
    return g_MultiplayerPlaybackMismatch;
}

u32 DebugExpectedMultiplayerPlaybackCoverage()
{
    return g_ExpectedMultiplayerPlaybackCoverage;
}

bool DebugRoundTrip(const char *path)
{
    if (!path || !*path)
        return false;

    ResetRecording();
    const bool normalSuffix = EndsWith(ResolveSavePath(path).c_str(), ".rpy");
    BeginInputFrame();
    CaptureTouchEvent(9, 0.9f, 0.1f, TOUCH_ACTION_DOWN, TOUCH_ROLE_MOVE, 0);
    RecordFrame(0, 5);
    BeginStageRecording(0);
    BeginInputFrame();
    CaptureJoystick(0.5f, -0.25f);
    CaptureDirectTouch(3.25f, -7.5f, true);
    CaptureTouchState(true, true, true);
    CaptureTouchEvent(1, 0.25f, 0.75f, TOUCH_ACTION_DOWN, TOUCH_ROLE_MOVE, 0);
    CaptureTouchEvent(1, 0.50f, 0.60f, TOUCH_ACTION_MOTION, TOUCH_ROLE_MOVE, 0);
    RecordFrame(0, 0);
    BeginInputFrame();
    CaptureTouchState(true, false, true);
    CaptureTouchEvent(1, 0.50f, 0.60f, TOUCH_ACTION_UP, TOUCH_ROLE_MOVE, 0);
    RecordFrame(0, 1);
    const std::string extendedPath = ResolveSavePath(path);
    if (!normalSuffix || !EndsWith(extendedPath.c_str(), ".rpyx"))
    {
        std::fprintf(stderr, "ReplayExtension self-test path failure: normal=%d extended=%s\n",
                     normalSuffix ? 1 : 0, extendedPath.c_str());
        return false;
    }

    std::vector<u8> base(sizeof(ReplayHeader) + 16, 0);
    std::memcpy(base.data(), "T6RP", 4);
    base[offsetof(ReplayHeader, key)] = 0x40;
    if (FileSystem::WriteDataToFile(extendedPath.c_str(), base.data(), base.size()) != 0 ||
        !AppendRecording(extendedPath.c_str()))
    {
        std::fprintf(stderr, "ReplayExtension self-test write/append failure: path=%s\n",
                     extendedPath.c_str());
        return false;
    }

    std::vector<u8> bytes;
    const bool read = ReadWholeFile(extendedPath.c_str(), bytes);
    const bool identity = read && MatchesPath(extendedPath.c_str(), bytes.data(), bytes.size()) &&
                          !MatchesPath(path, bytes.data(), bytes.size()) && BaseFileSize(bytes.data(), bytes.size()) == base.size();
    const bool loaded = identity && LoadPlayback(bytes.data(), bytes.size());
    SetPlaybackFrame(0, 0);
    f32 x = 0.0f;
    f32 y = 0.0f;
    f32 touchX = 0.0f;
    f32 touchY = 0.0f;
    bool unlimited = false;
    bool usedThisRun = false;
    bool bombedWithTouch = false;
    bool cheatMovementUsed = false;
    std::size_t eventCount = 0;
    const TouchEvent *events = GetPlaybackTouchEvents(&eventCount);
    const bool frame0 = loaded && GetPlaybackJoystick(&x, &y) &&
                        std::abs(x - 0.5f) < 0.0001f && std::abs(y + 0.25f) < 0.0001f &&
                        GetPlaybackDirectTouch(&touchX, &touchY, &unlimited) && unlimited &&
                        std::abs(touchX - 3.25f) < 0.0001f && std::abs(touchY + 7.5f) < 0.0001f &&
                        GetPlaybackTouchState(&usedThisRun, &bombedWithTouch, &cheatMovementUsed) &&
                        usedThisRun && bombedWithTouch && cheatMovementUsed &&
                        eventCount == 2 && events != nullptr &&
                        events[0].action == TOUCH_ACTION_DOWN && events[0].role == TOUCH_ROLE_MOVE &&
                        events[1].action == TOUCH_ACTION_MOTION &&
                        std::abs(events[1].x - 0.50f) < 0.0001f && std::abs(events[1].y - 0.60f) < 0.0001f;
    SetPlaybackFrame(0, 1);
    events = GetPlaybackTouchEvents(&eventCount);
    usedThisRun = bombedWithTouch = cheatMovementUsed = false;
    const bool frame1 = eventCount == 1 && events != nullptr && events[0].action == TOUCH_ACTION_UP &&
                        GetPlaybackTouchState(&usedThisRun, &bombedWithTouch, &cheatMovementUsed) &&
                        usedThisRun && !bombedWithTouch && cheatMovementUsed;
    SetPlaybackFrame(0, 5);
    events = GetPlaybackTouchEvents(&eventCount);
    const bool oldAttemptGone = eventCount == 0 && events == nullptr &&
                                !GetPlaybackDirectTouch(&touchX, &touchY, &unlimited);

    std::size_t payloadOffset = 0;
    std::size_t payloadSize = 0;
    u32 version = 0;
    const bool abiPresent = FindTrailer(bytes.data(), bytes.size(), payloadOffset, payloadSize, &version) &&
                            version == VERSION && ReadLe32(bytes.data() + payloadOffset + 92) == DETERMINISM_ABI;
    std::vector<u8> incompatible = bytes;
    if (abiPresent)
        WriteLe32(incompatible.data() + payloadOffset + 92, DETERMINISM_ABI + 1);
    const bool incompatibleRejected = abiPresent && !LoadPlayback(incompatible.data(), incompatible.size());
    std::vector<u8> developmentVersion = bytes;
    if (abiPresent)
        WriteLe32(developmentVersion.data() + payloadOffset, VERSION - 1);
    const bool developmentVersionRejected = abiPresent &&
                                            !LoadPlayback(developmentVersion.data(), developmentVersion.size());

    // EAGX multiplayer is still VERSION=1. Exercise the extended 160-byte
    // header and all three synchronized FrameInput lanes with TH06's narrower
    // Reimu/Marisa + A/B contract. Keep a local touch gesture in the same file:
    // multiplayer input lanes supplement, rather than replace, the gesture
    // visualization/state sidecar.
    ResetRecording();
    ClearPlayback();
    BeginStageRecording(0);
    MultiplayerReplayConfig mpConfig;
    mpConfig.playerCount = 3;
    mpConfig.difficulty = 3;
    mpConfig.localPlayer = 2;
    mpConfig.showContributionStats = false;
    mpConfig.gameplayAbi = TH06_MULTI_GAMEPLAY_ABI;
    mpConfig.characters[0] = 0;
    mpConfig.shots[0] = 1;
    mpConfig.characters[1] = 1;
    mpConfig.shots[1] = 0;
    mpConfig.characters[2] = 0;
    mpConfig.shots[2] = 1;
    BeginMultiplayerRecording(mpConfig);
    const MultiplayerPlayerResourceSnapshot stage0Resources[3] = {
        {2, 3, 64}, {1, 2, 96}, {0, 1, 128}};
    const MultiplayerPlayerResourceSnapshot stage3Resources[3] = {
        {4, 1, 128}, {2, 0, 32}, {3, 2, 80}};
    CaptureMultiplayerStageResources(0, stage0Resources, 3);
    CaptureMultiplayerStageResources(3, stage3Resources, 3);
    const MultiplayerContributionSnapshot stage0Contributions[3] = {
        {11, 1200}, {7, 900}, {5, 700}};
    const MultiplayerContributionSnapshot stage3Contributions[3] = {
        {41, 8200}, {29, 6100}, {33, 7300}};
    CaptureMultiplayerStageContributions(0, stage0Contributions, 3);
    CaptureMultiplayerStageContributions(3, stage3Contributions, 3);
    Netplay::FrameInput mpInputs[3]{};
    mpInputs[0].buttons = 0x0041;
    mpInputs[1].buttons = 0x0020;
    mpInputs[1].analogMode = Netplay::AnalogMode::Joystick;
    mpInputs[1].x = 0.375f;
    mpInputs[1].y = -0.625f;
    mpInputs[2].buttons = 0x0080;
    mpInputs[2].analogMode = Netplay::AnalogMode::DirectTouch;
    mpInputs[2].x = 2.5f;
    mpInputs[2].y = -1.25f;
    mpInputs[2].unlimited = true;
    mpInputs[2].touchUsed = true;
    mpInputs[2].touchBomb = true;
    BeginInputFrame();
    CaptureTouchState(true, true, false);
    CaptureTouchEvent(7, 0.2f, 0.8f, TOUCH_ACTION_DOWN, TOUCH_ROLE_FOCUS, 0);
    RecordFrame(0, 0);
    RecordMultiplayerFrame(0, 0, mpInputs, 3);
    // Rollback correction targets the same stage-local frame.  Keep EAGX
    // multiplayer frames overwriteable so the authoritative second pass
    // replaces an earlier predicted lane rather than creating a duplicate.
    mpInputs[1].buttons = 0x0033;
    mpInputs[1].x = -0.375f;
    mpInputs[1].y = 0.5f;
    RecordMultiplayerFrame(0, 0, mpInputs, 3);

    std::string multiplayerRequested = SwapExtension(path, false);
    if (multiplayerRequested.size() >= 4)
        multiplayerRequested.resize(multiplayerRequested.size() - 4);
    multiplayerRequested += "-mp.rpy";
    const std::string multiplayerPath = ResolveSavePath(multiplayerRequested.c_str());
    const bool multiplayerSuffix = EndsWith(multiplayerPath.c_str(), ".rpyx");
    const bool multiplayerWritten =
        FileSystem::WriteDataToFile(multiplayerPath.c_str(), base.data(), base.size()) == 0 &&
        AppendRecording(multiplayerPath.c_str());
    std::vector<u8> multiplayerBytes;
    const bool multiplayerRead = multiplayerWritten &&
        ReadWholeFile(multiplayerPath.c_str(), multiplayerBytes) &&
        LoadPlayback(multiplayerBytes.data(), multiplayerBytes.size());
    MultiplayerReplayConfig loadedConfig;
    Netplay::FrameInput loadedInputs[3]{};
    SetPlaybackFrame(0, 0);
    std::size_t multiplayerEventCount = 0;
    const TouchEvent *multiplayerEvents = GetPlaybackTouchEvents(&multiplayerEventCount);
    bool multiplayerUsedTouch = false;
    bool multiplayerBombedWithTouch = false;
    bool multiplayerCheatTouch = false;
    const bool multiplayerConfigRead = GetMultiplayerPlaybackConfig(&loadedConfig);
    MultiplayerPlayerResourceSnapshot loadedStage0Player1{};
    MultiplayerPlayerResourceSnapshot loadedStage3Player2{};
    MultiplayerPlayerResourceSnapshot missingStageResources{};
    MultiplayerContributionSnapshot loadedStage0Contribution1{};
    MultiplayerContributionSnapshot loadedStage3Contribution2{};
    MultiplayerContributionSnapshot missingStageContributions{};
    const bool multiplayerResourcesRead =
        GetMultiplayerPlaybackStageResources(0, 1, &loadedStage0Player1) &&
        GetMultiplayerPlaybackStageResources(3, 2, &loadedStage3Player2) &&
        !GetMultiplayerPlaybackStageResources(2, 1, &missingStageResources);
    const bool multiplayerContributionsRead =
        GetMultiplayerPlaybackStageContributions(0, 1, &loadedStage0Contribution1) &&
        GetMultiplayerPlaybackStageContributions(3, 2, &loadedStage3Contribution2) &&
        !GetMultiplayerPlaybackStageContributions(2, 1, &missingStageContributions);
    const bool multiplayerFrameRead = GetMultiplayerPlaybackFrame(0, 0, loadedInputs, 3);
    const bool multiplayerTouchStateRead = GetPlaybackTouchState(
        &multiplayerUsedTouch, &multiplayerBombedWithTouch, &multiplayerCheatTouch);
    const bool multiplayerRoundTrip = multiplayerRead && multiplayerSuffix && multiplayerConfigRead &&
        loadedConfig.playerCount == 3 && loadedConfig.difficulty == 3 &&
        loadedConfig.localPlayer == 2 && !loadedConfig.showContributionStats &&
        loadedConfig.gameplayAbi == TH06_MULTI_GAMEPLAY_ABI &&
        loadedConfig.characters[0] == 0 && loadedConfig.shots[0] == 1 &&
        loadedConfig.characters[1] == 1 && loadedConfig.shots[1] == 0 &&
        loadedConfig.characters[2] == 0 && loadedConfig.shots[2] == 1 &&
        multiplayerResourcesRead &&
        loadedStage0Player1.lives == 1 && loadedStage0Player1.bombs == 2 &&
        loadedStage0Player1.power == 96 &&
        loadedStage3Player2.lives == 3 && loadedStage3Player2.bombs == 2 &&
        loadedStage3Player2.power == 80 &&
        multiplayerContributionsRead &&
        loadedStage0Contribution1.enemiesDefeated == 7 &&
        loadedStage0Contribution1.damageDealt == 900 &&
        loadedStage3Contribution2.enemiesDefeated == 33 &&
        loadedStage3Contribution2.damageDealt == 7300 &&
        multiplayerFrameRead &&
        loadedInputs[0].buttons == mpInputs[0].buttons &&
        loadedInputs[1].buttons == mpInputs[1].buttons &&
        loadedInputs[1].analogMode == Netplay::AnalogMode::Joystick &&
        std::abs(loadedInputs[1].x - mpInputs[1].x) < 0.0001f &&
        std::abs(loadedInputs[1].y - mpInputs[1].y) < 0.0001f &&
        loadedInputs[2].buttons == mpInputs[2].buttons &&
        loadedInputs[2].analogMode == Netplay::AnalogMode::DirectTouch &&
        loadedInputs[2].unlimited && loadedInputs[2].touchUsed && loadedInputs[2].touchBomb &&
        std::abs(loadedInputs[2].x - mpInputs[2].x) < 0.0001f &&
        std::abs(loadedInputs[2].y - mpInputs[2].y) < 0.0001f &&
        multiplayerTouchStateRead &&
        multiplayerUsedTouch && multiplayerBombedWithTouch && !multiplayerCheatTouch &&
        multiplayerEventCount == 1 && multiplayerEvents != nullptr &&
        multiplayerEvents[0].fingerId == 7 && multiplayerEvents[0].role == TOUCH_ROLE_FOCUS;
    if (!multiplayerRoundTrip)
    {
        std::fprintf(stderr,
                     "ReplayExtension multiplayer detail: read=%d suffix=%d configRead=%d "
                     "config=%u/%u/%u loadouts=%u%u/%u%u/%u%u frameRead=%d "
                     "buttons=%04x/%04x/%04x analog=%u/%u/%u touchFlags=%d/%d/%d "
                     "touchStateRead=%d touchState=%d/%d/%d events=%zu event=%d/%u\n",
                     multiplayerRead ? 1 : 0, multiplayerSuffix ? 1 : 0,
                     multiplayerConfigRead ? 1 : 0,
                     loadedConfig.playerCount, loadedConfig.difficulty, loadedConfig.gameplayAbi,
                     loadedConfig.characters[0], loadedConfig.shots[0],
                     loadedConfig.characters[1], loadedConfig.shots[1],
                     loadedConfig.characters[2], loadedConfig.shots[2],
                     multiplayerFrameRead ? 1 : 0,
                     loadedInputs[0].buttons, loadedInputs[1].buttons, loadedInputs[2].buttons,
                     static_cast<unsigned>(loadedInputs[0].analogMode),
                     static_cast<unsigned>(loadedInputs[1].analogMode),
                     static_cast<unsigned>(loadedInputs[2].analogMode),
                     loadedInputs[2].unlimited ? 1 : 0, loadedInputs[2].touchUsed ? 1 : 0,
                     loadedInputs[2].touchBomb ? 1 : 0, multiplayerTouchStateRead ? 1 : 0,
                     multiplayerUsedTouch ? 1 : 0, multiplayerBombedWithTouch ? 1 : 0,
                     multiplayerCheatTouch ? 1 : 0, multiplayerEventCount,
                     multiplayerEvents ? multiplayerEvents[0].fingerId : -1,
                     multiplayerEvents ? multiplayerEvents[0].role : 0);
    }

    std::size_t multiplayerPayloadOffset = 0;
    std::size_t multiplayerPayloadSize = 0;
    u32 multiplayerVersion = 0;
    u32 multiplayerFlags = 0;
    const bool multiplayerHeader = multiplayerRead &&
        FindTrailer(multiplayerBytes.data(), multiplayerBytes.size(),
                    multiplayerPayloadOffset, multiplayerPayloadSize,
                    &multiplayerVersion, &multiplayerFlags) &&
        multiplayerVersion == VERSION &&
        (multiplayerFlags & FLAG_MULTIPLAYER_INPUT) != 0 &&
        (multiplayerFlags & FLAG_MULTIPLAYER_STAGE_RESOURCES) != 0 &&
        (multiplayerFlags & FLAG_MULTIPLAYER_STAGE_CONTRIBUTIONS) != 0 &&
        multiplayerPayloadSize >= MP_HEADER_SIZE &&
        ReadLe32(multiplayerBytes.data() + multiplayerPayloadOffset + 92) == DETERMINISM_ABI;
    std::vector<u8> invalidCharacter = multiplayerBytes;
    if (multiplayerHeader)
        WriteLe32(invalidCharacter.data() + multiplayerPayloadOffset + MP_LOADOUT_OFFSET + 2 * 4,
                  2u | (1u << 8));
    const bool invalidCharacterRejected = multiplayerHeader &&
        !LoadPlayback(invalidCharacter.data(), invalidCharacter.size());
    std::vector<u8> invalidDifficulty = multiplayerBytes;
    if (multiplayerHeader)
        WriteLe32(invalidDifficulty.data() + multiplayerPayloadOffset + MP_DIFFICULTY_OFFSET, 5);
    const bool invalidDifficultyRejected = multiplayerHeader &&
        !LoadPlayback(invalidDifficulty.data(), invalidDifficulty.size());
    std::vector<u8> invalidLocalPlayer = multiplayerBytes;
    if (multiplayerHeader)
        WriteLe32(invalidLocalPlayer.data() + multiplayerPayloadOffset + MP_LOCAL_PLAYER_OFFSET, 3);
    const bool invalidLocalPlayerRejected = multiplayerHeader &&
        !LoadPlayback(invalidLocalPlayer.data(), invalidLocalPlayer.size());

    // Files from the earlier v1 writer have resources but no contribution
    // tail. They remain valid and simply start the optional HUD counters at 0.
    bool preContributionV1Accepted = false;
    if (multiplayerHeader &&
        multiplayerPayloadSize >= MP_HEADER_SIZE + MP_STAGE_CONTRIBUTION_BYTES)
    {
        std::vector<u8> preContribution = multiplayerBytes;
        const std::size_t oldPayloadSize =
            multiplayerPayloadSize - MP_STAGE_CONTRIBUTION_BYTES;
        const std::size_t contributionOffset = multiplayerPayloadOffset + oldPayloadSize;
        preContribution.erase(
            preContribution.begin() + contributionOffset,
            preContribution.begin() + contributionOffset + MP_STAGE_CONTRIBUTION_BYTES);
        WriteLe32(preContribution.data() + multiplayerPayloadOffset + 4,
                  multiplayerFlags & ~FLAG_MULTIPLAYER_STAGE_CONTRIBUTIONS);
        WriteLe32(preContribution.data() + multiplayerPayloadOffset + oldPayloadSize,
                  static_cast<u32>(oldPayloadSize));
        std::memcpy(preContribution.data() + multiplayerPayloadOffset + oldPayloadSize + 4,
                    "EAGX", 4);
        WriteLe32(preContribution.data() + offsetof(ReplayHeader, checksum),
                  ReplayChecksumForRawBytes(preContribution));
        MultiplayerContributionSnapshot oldContributions{};
        preContributionV1Accepted =
            LoadPlayback(preContribution.data(), preContribution.size()) &&
            !GetMultiplayerPlaybackStageContributions(0, 1, &oldContributions);
    }

    // Backward compatibility: VERSION=1 MP Replay files created before the
    // optional stage-resource feature end immediately after the MP input
    // lanes. Strip only the new appended block and its flag, then prove the
    // old v1 payload still loads its config/input lanes while correctly
    // reporting that no per-stage resource snapshot exists.
    bool legacyMultiplayerV1Accepted = false;
    if (multiplayerHeader && multiplayerPayloadSize >=
        MP_HEADER_SIZE + MP_STAGE_RESOURCE_BYTES + MP_STAGE_CONTRIBUTION_BYTES)
    {
        std::vector<u8> legacyMultiplayer = multiplayerBytes;
        const std::size_t legacyPayloadSize = multiplayerPayloadSize -
            MP_STAGE_RESOURCE_BYTES - MP_STAGE_CONTRIBUTION_BYTES;
        const std::size_t optionalTailOffset = multiplayerPayloadOffset + legacyPayloadSize;
        legacyMultiplayer.erase(
            legacyMultiplayer.begin() + optionalTailOffset,
            legacyMultiplayer.begin() + optionalTailOffset +
                MP_STAGE_RESOURCE_BYTES + MP_STAGE_CONTRIBUTION_BYTES);
        WriteLe32(legacyMultiplayer.data() + multiplayerPayloadOffset + 4,
                  multiplayerFlags & ~(FLAG_MULTIPLAYER_STAGE_RESOURCES |
                                       FLAG_MULTIPLAYER_STAGE_CONTRIBUTIONS));
        // Older VERSION=1 writers left this reserved word zero-filled.
        // Zero therefore remains the backward-compatible P1 viewpoint.
        // Old TH06 EAGX v1 files written before the shared ABI owner used 1.
        // Keep accepting that mislabeled legacy value; only new files must carry
        // the current deterministic gameplay ABI.
        WriteLe32(legacyMultiplayer.data() + multiplayerPayloadOffset + MP_GAMEPLAY_ABI_OFFSET, 1);
        WriteLe32(legacyMultiplayer.data() + multiplayerPayloadOffset + MP_LOCAL_PLAYER_OFFSET, 0);
        WriteLe32(legacyMultiplayer.data() + multiplayerPayloadOffset + MP_SESSION_FLAGS_OFFSET, 0);
        WriteLe32(legacyMultiplayer.data() + multiplayerPayloadOffset + legacyPayloadSize,
                  static_cast<u32>(legacyPayloadSize));
        std::memcpy(legacyMultiplayer.data() + multiplayerPayloadOffset + legacyPayloadSize + 4,
                    "EAGX", 4);
        WriteLe32(legacyMultiplayer.data() + offsetof(ReplayHeader, checksum),
                  ReplayChecksumForRawBytes(legacyMultiplayer));

        MultiplayerReplayConfig legacyConfig{};
        Netplay::FrameInput legacyInputs[3]{};
        MultiplayerPlayerResourceSnapshot legacyResources{};
        MultiplayerContributionSnapshot legacyContributions{};
        legacyMultiplayerV1Accepted =
            LoadPlayback(legacyMultiplayer.data(), legacyMultiplayer.size()) &&
            GetMultiplayerPlaybackConfig(&legacyConfig) &&
            legacyConfig.playerCount == 3 && legacyConfig.localPlayer == 0 &&
            legacyConfig.showContributionStats &&
            legacyConfig.gameplayAbi == 1 &&
            GetMultiplayerPlaybackFrame(0, 0, legacyInputs, 3) &&
            legacyInputs[1].buttons == mpInputs[1].buttons &&
            !GetMultiplayerPlaybackStageResources(0, 1, &legacyResources) &&
            !GetMultiplayerPlaybackStageContributions(0, 1, &legacyContributions);
    }

    const std::string actualPath = FileSystem::GetPrefPath(extendedPath.c_str());
    std::remove(actualPath.c_str());
    const std::string actualMultiplayerPath = FileSystem::GetPrefPath(multiplayerPath.c_str());
    std::remove(actualMultiplayerPath.c_str());
    ResetRecording();
    ClearPlayback();
    const bool passed = frame0 && frame1 && oldAttemptGone && incompatibleRejected &&
                        developmentVersionRejected && multiplayerRoundTrip && multiplayerHeader &&
                        invalidCharacterRejected && invalidDifficultyRejected &&
                        invalidLocalPlayerRejected &&
                        preContributionV1Accepted &&
                        legacyMultiplayerV1Accepted;
    if (!passed)
    {
        std::fprintf(stderr,
                     "ReplayExtension self-test detail: frame0=%d frame1=%d oldAttemptGone=%d "
                     "incompatibleRejected=%d developmentVersionRejected=%d multiplayerRoundTrip=%d "
                     "multiplayerHeader=%d invalidCharacterRejected=%d invalidDifficultyRejected=%d "
                     "preContributionV1Accepted=%d legacyMultiplayerV1Accepted=%d\n",
                     frame0 ? 1 : 0, frame1 ? 1 : 0, oldAttemptGone ? 1 : 0,
                     incompatibleRejected ? 1 : 0, developmentVersionRejected ? 1 : 0,
                     multiplayerRoundTrip ? 1 : 0, multiplayerHeader ? 1 : 0,
                     invalidCharacterRejected ? 1 : 0, invalidDifficultyRejected ? 1 : 0,
                     preContributionV1Accepted ? 1 : 0,
                     legacyMultiplayerV1Accepted ? 1 : 0);
    }
    return passed;
}
#endif
} // namespace ReplayExtension
