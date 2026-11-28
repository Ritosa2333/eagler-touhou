#include "ReplayArchive.hpp"

#include <cstring>

namespace th13::multiplayer {

namespace {
constexpr std::size_t kFixedHeader = 4 + 4 + 4 + SessionSetup::Words * 4 + 8 + 4 + 4 + 4;

std::uint32_t Read32(const std::uint8_t* p) {
    return std::uint32_t(p[0]) | std::uint32_t(p[1]) << 8 |
           std::uint32_t(p[2]) << 16 | std::uint32_t(p[3]) << 24;
}
void Write32(std::vector<std::uint8_t>& out, std::uint32_t value) {
    out.push_back(std::uint8_t(value));
    out.push_back(std::uint8_t(value >> 8));
    out.push_back(std::uint8_t(value >> 16));
    out.push_back(std::uint8_t(value >> 24));
}
std::uint64_t Read64(const std::uint8_t* p) {
    return std::uint64_t(Read32(p)) | std::uint64_t(Read32(p + 4)) << 32;
}
void Write64(std::vector<std::uint8_t>& out, std::uint64_t value) {
    Write32(out, std::uint32_t(value));
    Write32(out, std::uint32_t(value >> 32));
}
}

bool ReplayArchive::Begin(const SessionSetup& setup, std::uint64_t timestamp) {
    if (!setup.valid()) return false;
    Netplay::InputReplayConfig config;
    config.gameId = 0x54483133u;
    config.gameplayAbi = GameplayAbi(setup);
    config.playerCount = std::uint8_t(setup.player_count);
    config.recordedPlayer = std::uint8_t(setup.local_player);
    if (!replay_.Begin(config)) return false;
    setup_ = setup;
    version_ = Version;
    timestamp_ = timestamp;
    score_ = 0;
    completed_ = false;
    name_.clear();
    entryChapters_.clear();
    entryBytes_.clear();
    info_ = {};
    return true;
}

bool ReplayArchive::Append(std::uint32_t frame, std::uint32_t chapter,
                           const Netplay::FrameInput* inputs, unsigned count) {
    if (!replay_.Recording()) return false;
    if (count < 2 || count > 3) return false;
    return replay_.Append(frame, chapter, inputs, count);
}

bool ReplayArchive::AppendStageEntry(std::uint32_t chapter,
                                     std::vector<std::uint8_t> entry) {
    if (!replay_.Recording() || !chapter || entry.empty()) return false;
    for (std::uint32_t label : entryChapters_)
        if (label == chapter) return false;
    if (entryChapters_.size() >= MaxChapters) return false;
    entryChapters_.push_back(chapter);
    entryBytes_.push_back(std::move(entry));
    return true;
}

bool ReplayArchive::Encode(std::vector<std::uint8_t>& out, const char* name,
                           std::uint32_t score, bool completed) const {
    if (!replay_.Recording() || !replay_.Info().frameCount) return false;
    out.clear();
    Write32(out, Magic);
    Write32(out, Version);
    const std::vector<std::uint8_t> header =
        EncodeHeader(name ? name : "", score, completed);
    Write32(out, std::uint32_t(header.size()));
    out.insert(out.end(), header.begin(), header.end());
    std::vector<std::uint8_t> body;
    if (!replay_.Encode(&body)) return false;
    out.insert(out.end(), body.begin(), body.end());
    return true;
}

std::vector<std::uint8_t> ReplayArchive::EncodeHeader(const char* name,
                                                      std::uint32_t score,
                                                      bool completed) const {
    std::vector<std::uint8_t> header;
    const auto words = EncodeSessionSetup(setup_);
    for (std::uint32_t word : words) Write32(header, word);
    Write64(header, timestamp_);
    Write32(header, score);
    Write32(header, completed ? 1u : 0u);
    const std::string text = name ? name : "";
    const std::size_t nameLen = text.size() < 64 ? text.size() : 64;
    Write32(header, std::uint32_t(nameLen));
    header.insert(header.end(), text.begin(), text.begin() + nameLen);
    Write32(header, std::uint32_t(entryChapters_.size()));
    for (std::size_t index = 0; index < entryChapters_.size(); ++index) {
        Write32(header, entryChapters_[index]);
        Write32(header, std::uint32_t(entryBytes_[index].size()));
        header.insert(header.end(), entryBytes_[index].begin(),
                      entryBytes_[index].end());
    }
    return header;
}

bool ReplayArchive::HasSignature(const std::uint8_t* bytes, std::size_t size) {
    return bytes && size >= 12 && Read32(bytes) == Magic;
}

bool ReplayArchive::DecodeHeader(const std::uint8_t* bytes, std::size_t size,
                                 SessionSetup& setup, std::uint64_t& timestamp,
                                 std::uint32_t& score, bool& completed,
                                 std::string& name, std::size_t& headerBytes) {
    if (size < kFixedHeader) return false;
    const auto version = Read32(bytes + 4);
    if (version < LegacyVersion || version > Version) return false;
    const std::uint32_t declared = Read32(bytes + 8);
    if (declared < kFixedHeader - 12 || std::size_t(declared) + 12 > size)
        return false;
    const std::uint8_t* p = bytes + 12;
    const std::uint8_t* end = p + declared;
    SessionSetup decoded;
    std::array<std::uint32_t, SessionSetup::Words> words{};
    for (std::size_t index = 0; index < SessionSetup::Words; ++index) {
        words[index] = Read32(p);
        p += 4;
    }
    if (!DecodeSessionSetup(decoded, words.data(), words.size())) return false;
    timestamp = Read64(p);
    p += 8;
    score = Read32(p);
    p += 4;
    completed = Read32(p) != 0;
    p += 4;
    const std::uint32_t nameLen = Read32(p);
    p += 4;
    if (nameLen > 64 || p + nameLen > end) return false;
    name.assign(reinterpret_cast<const char*>(p), nameLen);
    p += nameLen;
    if (p + 4 > end) return false;
    const std::uint32_t entryCount = Read32(p);
    p += 4;
    if (entryCount > MaxChapters) return false;
    headerBytes = 12 + declared;
    setup = decoded;
    return true;
}

bool ReplayArchive::Load(const std::uint8_t* bytes, std::size_t size) {
    if (!HasSignature(bytes, size)) return false;
    SessionSetup setup;
    std::uint64_t timestamp = 0;
    std::uint32_t score = 0;
    bool completed = false;
    std::string name;
    std::size_t headerBytes = 0;
    if (!DecodeHeader(bytes, size, setup, timestamp, score, completed, name,
                      headerBytes))
        return false;
    const std::uint32_t declared = Read32(bytes + 8);
    const std::uint8_t* p = bytes + 12 + SessionSetup::Words * 4 + 8 + 4 + 4;
    const std::uint32_t nameLen = Read32(p);
    p += 4 + nameLen;
    const std::uint32_t entryCount = Read32(p);
    p += 4;
    std::vector<std::uint32_t> chapters;
    std::vector<std::vector<std::uint8_t>> entries;
    chapters.reserve(entryCount);
    entries.reserve(entryCount);
    for (std::uint32_t index = 0; index < entryCount; ++index) {
        const std::uint8_t* q = p;
        if (q + 8 > bytes + 12 + declared) return false;
        const std::uint32_t chapter = Read32(q);
        q += 4;
        const std::uint32_t length = Read32(q);
        q += 4;
        if (!length || q + length > bytes + 12 + declared) return false;
        for (std::uint32_t prior : chapters)
            if (prior == chapter) return false;
        chapters.push_back(chapter);
        entries.emplace_back(q, q + length);
        p = q + length;
    }
    Netplay::InputReplay replay;
    if (!replay.Decode(bytes + headerBytes, size - headerBytes)) return false;
    const auto& info = replay.Info();
    for (std::uint32_t chapter : chapters) {
        bool found = false;
        for (std::uint32_t index = 0; index < info.chapterCount; ++index)
            if (info.chapters[index].label == chapter) { found = true; break; }
        if (!found) return false;
    }
    replay_ = std::move(replay);
    setup_ = setup;
    version_ = Read32(bytes + 4);
    timestamp_ = timestamp;
    score_ = score;
    completed_ = completed;
    name_ = std::move(name);
    info_ = replay_.Info();
    entryChapters_ = std::move(chapters);
    entryBytes_ = std::move(entries);
    return true;
}

const Netplay::InputReplay::Frame* ReplayArchive::FrameAt(std::uint32_t frame) const {
    return replay_.FrameAt(frame);
}

const std::vector<std::uint8_t>* ReplayArchive::StageEntryAt(
    std::uint32_t chapter) const {
    for (std::size_t index = 0; index < entryChapters_.size(); ++index)
        if (entryChapters_[index] == chapter) return &entryBytes_[index];
    return nullptr;
}

void ReplayArchive::Clear() {
    replay_.Clear();
    setup_ = {};
    version_ = Version;
    timestamp_ = 0;
    score_ = 0;
    completed_ = false;
    name_.clear();
    info_ = {};
    entryChapters_.clear();
    entryBytes_.clear();
}

}
