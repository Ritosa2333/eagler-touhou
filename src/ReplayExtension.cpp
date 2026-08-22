#include "ReplayExtension.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
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
static_assert(sizeof(InputSample) == 12);
static_assert(sizeof(DirectTouchSample) == 12);
static_assert(sizeof(TouchEvent) == 28);

std::vector<InputSample> g_RecordInputs[7];
std::vector<DirectTouchSample> g_RecordDirectTouch[7];
std::vector<TouchEvent> g_RecordTouchEvents[7];
std::vector<TouchEvent> g_PendingTouchEvents;
std::vector<InputSample> g_PlaybackInputs[7];
std::vector<DirectTouchSample> g_PlaybackDirectTouch[7];
std::vector<TouchEvent> g_PlaybackTouchEvents[7];
InputSample g_CapturedJoystick = {};
DirectTouchSample g_CapturedDirectTouch = {};
bool g_RecordUsesExtension = false;
bool g_RecordHasTouchState = false;
u32 g_PlaybackVersion = 0;
u32 g_PlaybackFlags = 0;
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
constexpr u32 DIRECT_TOUCH_ACTIVE = 1;
constexpr u32 DIRECT_TOUCH_UNLIMITED = 2;
constexpr u32 TOUCH_STATE_USED_THIS_RUN = 4;
constexpr u32 TOUCH_STATE_BOMBED_WITH_TOUCH = 8;
constexpr u32 TOUCH_STATE_CHEAT_MOVEMENT_USED = 16;
constexpr u32 TOUCH_STATE_MASK = TOUCH_STATE_USED_THIS_RUN | TOUCH_STATE_BOMBED_WITH_TOUCH |
                                 TOUCH_STATE_CHEAT_MOVEMENT_USED;
constexpr std::size_t HEADER_SIZE = 96;

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
    if (version != VERSION || ReadLe32(bytes + payloadOffset + 92) != DETERMINISM_ABI)
        return false;
    if ((flags & (FLAG_JOYSTICK_INPUT | FLAG_TOUCH_EVENTS | FLAG_DIRECT_TOUCH_INPUT | FLAG_TOUCH_STATE)) == 0)
        return false;
    std::size_t expected = HEADER_SIZE;
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
    g_RecordUsesExtension = false;
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
    g_PendingTouchEvents.clear();
    g_CapturedJoystick = {};
    g_CapturedDirectTouch = {};
    g_RecordUsesExtension = false;
    g_RecordHasTouchState = false;
}

void BeginStageRecording(i32 stage)
{
    stage = std::clamp(stage, 0, 6);
    g_RecordInputs[stage].clear();
    g_RecordDirectTouch[stage].clear();
    g_RecordTouchEvents[stage].clear();
    g_PendingTouchEvents.clear();
    g_CapturedJoystick = {};
    g_CapturedDirectTouch = {};
    RecomputeRecordUsage();
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
    std::size_t payloadSize = HEADER_SIZE;
    for (const auto &stage : g_RecordInputs)
    {
        payloadSize += stage.size() * sizeof(InputSample);
        for (const InputSample &sample : stage)
            if (sample.active)
                flags |= FLAG_JOYSTICK_INPUT;
    }
    for (const auto &stage : g_RecordTouchEvents)
    {
        payloadSize += stage.size() * sizeof(TouchEvent);
        if (!stage.empty())
            flags |= FLAG_TOUCH_EVENTS;
    }
    for (const auto &stage : g_RecordDirectTouch)
    {
        payloadSize += stage.size() * sizeof(DirectTouchSample);
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
    if (flags == 0)
        return true;
    const std::size_t payloadOffset = bytes.size();
    bytes.resize(payloadOffset + payloadSize + 8, 0);
    WriteLe32(bytes.data() + payloadOffset, VERSION);
    WriteLe32(bytes.data() + payloadOffset + 4, flags);
    WriteLe32(bytes.data() + payloadOffset + 92, DETERMINISM_ABI);

    std::size_t cursor = payloadOffset + HEADER_SIZE;
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
    g_PlaybackVersion = 0;
    g_PlaybackFlags = 0;
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
    std::size_t cursor = payloadOffset + HEADER_SIZE;
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

bool UsesFixedTickTouchPlayback()
{
    return g_PlaybackVersion == VERSION;
}

bool GetPlaybackJoystick(f32 *x, f32 *y)
{
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
        return false;

    std::vector<u8> base(sizeof(ReplayHeader) + 16, 0);
    std::memcpy(base.data(), "T6RP", 4);
    base[offsetof(ReplayHeader, key)] = 0x40;
    if (FileSystem::WriteDataToFile(extendedPath.c_str(), base.data(), base.size()) != 0 ||
        !AppendRecording(extendedPath.c_str()))
        return false;

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

    const std::string actualPath = FileSystem::GetPrefPath(extendedPath.c_str());
    std::remove(actualPath.c_str());
    ResetRecording();
    ClearPlayback();
    return frame0 && frame1 && oldAttemptGone && incompatibleRejected && developmentVersionRejected;
}
#endif
} // namespace ReplayExtension
