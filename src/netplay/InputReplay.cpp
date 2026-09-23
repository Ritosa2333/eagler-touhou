#include <eagler/netplay/InputReplay.hpp>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <new>

namespace Netplay
{
namespace
{
constexpr std::uint8_t Signature[8] = {'E','A','G','L','R','P','Y','1'};
std::uint32_t word(const std::uint8_t* p)
{
    return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
           (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
}
void put(std::vector<std::uint8_t>& bytes, std::uint32_t value)
{
    for (unsigned shift = 0; shift < 32; shift += 8)
        bytes.push_back(static_cast<std::uint8_t>(value >> shift));
}
void put_float(std::vector<std::uint8_t>& bytes, float value)
{
    std::uint32_t bits;
    static_assert(sizeof(bits) == sizeof(value));
    std::memcpy(&bits, &value, sizeof(bits)); put(bytes, bits);
}
std::uint32_t checksum(const std::uint8_t* bytes, std::size_t size)
{
    // Corruption detection, not authentication. Include metadata; omit only
    // the checksum word. All structural and allocation bounds are separate.
    std::uint32_t hash = 2166136261u;
    for (std::size_t i = 0; i < size; ++i)
        if (i < 36 || i >= 40) { hash ^= bytes[i]; hash *= 16777619u; }
    return hash;
}
bool valid_config(const InputReplayConfig& config)
{
    return config.gameId && config.gameplayAbi && config.playerCount >= 2 &&
           config.playerCount <= MAX_PLAYERS && config.recordedPlayer < config.playerCount &&
           config.description.size() <= InputReplay::MaxDescriptionBytes;
}
bool valid_sample(const FrameInput& input)
{
    return static_cast<unsigned>(input.analogMode) <= static_cast<unsigned>(AnalogMode::DirectTouchBegin) &&
           std::isfinite(input.x) && std::isfinite(input.y);
}
bool sample(const std::uint8_t* bytes, FrameInput& input)
{
    input.buttons = static_cast<std::uint16_t>(bytes[0] | (std::uint16_t(bytes[1]) << 8));
    input.analogMode = static_cast<AnalogMode>(bytes[2]);
    const auto flags = bytes[3];
    input.unlimited = (flags & 1u) != 0;
    input.touchUsed = (flags & 2u) != 0;
    input.touchBomb = (flags & 4u) != 0;
    const std::uint32_t x = word(bytes + 4), y = word(bytes + 8);
    std::memcpy(&input.x, &x, sizeof(x)); std::memcpy(&input.y, &y, sizeof(y));
    return !(flags & ~7u) && valid_sample(input);
}
}

void InputReplay::Clear()
{
    info_ = {}; frames_.reset(); recording_ = loaded_ = false;
}

bool InputReplay::Begin(const InputReplayConfig& config)
{
    if (!valid_config(config)) return false;
    InputReplay candidate;
    candidate.frames_.reset(new (std::nothrow) Frame[MaxFrames]);
    if (!candidate.frames_) return false;
    candidate.info_.config = config; candidate.recording_ = true;
    *this = std::move(candidate); return true;
}

bool InputReplay::Append(std::uint32_t frame, std::uint32_t chapter,
                         const FrameInput* inputs, std::size_t count)
{
    if (!recording_ || !frames_ || !inputs || count != info_.config.playerCount ||
        frame != info_.frameCount || frame >= MaxFrames || !chapter) return false;
    for (std::size_t seat = 0; seat < count; ++seat)
        if (!valid_sample(inputs[seat])) return false;
    const bool newChapter = !info_.chapterCount ||
        chapter != info_.chapters[info_.chapterCount - 1].label;
    if (newChapter)
    {
        if (info_.chapterCount >= info_.chapters.size()) return false;
        for (std::uint32_t i = 0; i < info_.chapterCount; ++i)
            if (info_.chapters[i].label == chapter) return false;
    }
    if (newChapter) info_.chapters[info_.chapterCount++] = {chapter, frame};
    auto& row = frames_[frame]; std::copy(inputs, inputs + count, row.begin());
    for (std::size_t seat = count; seat < MAX_PLAYERS; ++seat) row[seat] = {};
    ++info_.frameCount; return true;
}

const InputReplay::Frame* InputReplay::FrameAt(std::uint32_t frame) const
{
    return frames_ && frame < info_.frameCount ? frames_.get() + frame : nullptr;
}

bool InputReplay::HasSignature(const std::uint8_t* bytes, std::size_t size)
{
    return bytes && size >= sizeof(Signature) && !std::memcmp(bytes, Signature, sizeof(Signature));
}

bool InputReplay::Inspect(const std::uint8_t* bytes, std::size_t size, InputReplayInfo* out)
{
    if (!out || !HasSignature(bytes, size) || size < HeaderBytes || size > MaxBytes ||
        word(bytes + 8) != 1 || bytes[22] || bytes[23]) return false;
    InputReplayInfo candidate; auto& config = candidate.config;
    config.gameId = word(bytes + 12); config.gameplayAbi = word(bytes + 16);
    config.playerCount = bytes[20]; config.recordedPlayer = bytes[21];
    const auto descriptionBytes = word(bytes + 24);
    candidate.chapterCount = word(bytes + 28); candidate.frameCount = word(bytes + 32);
    if (!valid_config(config) || descriptionBytes > MaxDescriptionBytes ||
        !candidate.chapterCount || candidate.chapterCount > candidate.chapters.size() ||
        !candidate.frameCount || candidate.frameCount > MaxFrames) return false;
    const std::uint64_t prefix = HeaderBytes + std::uint64_t(descriptionBytes) +
                                 std::uint64_t(candidate.chapterCount) * 8;
    const std::uint64_t expected = prefix + std::uint64_t(candidate.frameCount) *
                                             config.playerCount * SampleBytes;
    if (expected != size || word(bytes + 36) != checksum(bytes, size)) return false;
    config.description.assign(bytes + HeaderBytes, bytes + HeaderBytes + descriptionBytes);
    std::size_t at = HeaderBytes + descriptionBytes;
    for (std::uint32_t i = 0; i < candidate.chapterCount; ++i, at += 8)
    {
        auto& chapter = candidate.chapters[i];
        chapter = {word(bytes + at), word(bytes + at + 4)};
        if (!chapter.label || chapter.firstFrame >= candidate.frameCount ||
            (!i && chapter.firstFrame) ||
            (i && chapter.firstFrame <= candidate.chapters[i - 1].firstFrame)) return false;
        for (std::uint32_t j = 0; j < i; ++j)
            if (candidate.chapters[j].label == chapter.label) return false;
    }
    for (std::uint32_t frame = 0; frame < candidate.frameCount; ++frame)
        for (std::uint8_t seat = 0; seat < config.playerCount; ++seat, at += SampleBytes)
        {
            FrameInput input; if (!sample(bytes + at, input)) return false;
        }
    *out = std::move(candidate); return true;
}

bool InputReplay::Decode(const std::uint8_t* bytes, std::size_t size)
{
    InputReplay candidate;
    if (!Inspect(bytes, size, &candidate.info_)) return false;
    candidate.frames_.reset(new (std::nothrow) Frame[candidate.info_.frameCount]);
    if (!candidate.frames_) return false;
    std::size_t at = HeaderBytes + candidate.info_.config.description.size() +
                     candidate.info_.chapterCount * 8;
    for (std::uint32_t frame = 0; frame < candidate.info_.frameCount; ++frame)
        for (std::uint8_t seat = 0; seat < candidate.info_.config.playerCount; ++seat, at += SampleBytes)
            if (!sample(bytes + at, candidate.frames_[frame][seat])) return false;
    candidate.loaded_ = true; *this = std::move(candidate); return true;
}

bool InputReplay::Encode(std::vector<std::uint8_t>* out,
                         const std::vector<std::uint8_t>* descriptionOverride,
                         std::uint32_t frameCount) const
{
    const auto& description = descriptionOverride ? *descriptionOverride : info_.config.description;
    if (frameCount == INVALID_FRAME) frameCount = info_.frameCount;
    if (!out || !frames_ || !frameCount || frameCount > info_.frameCount || !valid_config(info_.config) ||
        description.size() > MaxDescriptionBytes) return false;
    std::uint32_t chapterCount = 0;
    while (chapterCount < info_.chapterCount && info_.chapters[chapterCount].firstFrame < frameCount) ++chapterCount;
    const std::size_t bytes = HeaderBytes + description.size() + chapterCount * 8 +
                              std::size_t(frameCount) * info_.config.playerCount * SampleBytes;
    if (bytes > MaxBytes) return false;
    std::vector<std::uint8_t> candidate; candidate.reserve(bytes);
    candidate.insert(candidate.end(), Signature, Signature + sizeof(Signature));
    put(candidate, 1); put(candidate, info_.config.gameId); put(candidate, info_.config.gameplayAbi);
    candidate.push_back(info_.config.playerCount); candidate.push_back(info_.config.recordedPlayer);
    candidate.push_back(0); candidate.push_back(0);
    put(candidate, static_cast<std::uint32_t>(description.size()));
    put(candidate, chapterCount); put(candidate, frameCount); put(candidate, 0);
    candidate.insert(candidate.end(), description.begin(), description.end());
    for (std::uint32_t i = 0; i < chapterCount; ++i)
    {
        put(candidate, info_.chapters[i].label); put(candidate, info_.chapters[i].firstFrame);
    }
    for (std::uint32_t frame = 0; frame < frameCount; ++frame)
        for (std::uint8_t seat = 0; seat < info_.config.playerCount; ++seat)
        {
            const auto& input = frames_[frame][seat];
            candidate.push_back(static_cast<std::uint8_t>(input.buttons));
            candidate.push_back(static_cast<std::uint8_t>(input.buttons >> 8));
            candidate.push_back(static_cast<std::uint8_t>(input.analogMode));
            candidate.push_back((input.unlimited ? 1u : 0u) | (input.touchUsed ? 2u : 0u) |
                                (input.touchBomb ? 4u : 0u));
            put_float(candidate, input.x); put_float(candidate, input.y);
        }
    const auto hash = checksum(candidate.data(), candidate.size());
    for (unsigned i = 0; i < 4; ++i) candidate[36 + i] = static_cast<std::uint8_t>(hash >> (8 * i));
    *out = std::move(candidate); return true;
}
} // namespace Netplay
