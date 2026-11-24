#include "../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "game/ReplayFile.hpp"
#include "game/ReplayRecorder.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>

namespace {
using namespace th13;

bool read_file(const std::string& relative, std::vector<u8>& out) {
    static const char* bases[] = {".", "../../../..", "../../../../.."};
    for (const char* base : bases) {
        std::FILE* file = std::fopen((std::string(base) + "/" + relative).c_str(), "rb");
        if (!file) continue;
        std::fseek(file, 0, SEEK_END);
        const long size = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);
        out.resize(size_t(size < 0 ? 0 : size));
        const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
        std::fclose(file);
        if (got == out.size()) return true;
        return false;
    }
    return false;
}

struct Sample {
    const char* path;
};

const Sample kSamples[] = {
    {"games/replays/th13_01.rpy"},     {"games/replays/th13_udMy25.rpy"},     {"games/replays/th13_udN01.rpy"},
    {"games/replays/th13_udPn16.rpy"}, {"games/replays/th13_udSr01.rpy"},     {"games/replays/th13_udWi11.rpy"},
    {"games/th13/replay/th13_01.rpy"}, {"games/th13/replay/th13_udSr01.rpy"},
};

u32 read_u32(const u8* p) { return u32(p[0]) | u32(p[1]) << 8 | u32(p[2]) << 16 | u32(p[3]) << 24; }

i32 read_i32(const u8* p) { return i32(read_u32(p)); }

std::string user_line(const std::string& text, const char* key) {
    const std::string k = std::string(key) + " ";
    size_t at = text.find(k);
    if (at != 0) {
        at = text.find(std::string("\r\n") + k);
        if (at == std::string::npos) return "";
        at += 2;
    }
    size_t end = text.find("\r\n", at + k.size());
    if (end == std::string::npos) end = text.size();
    return text.substr(at + k.size(), end - (at + k.size()));
}

bool set_replay_timezone(const std::string& date_line, std::time_t stamp) {
    if (date_line.size() < 14) return false;
    std::tm tm{};
    tm.tm_year = 100 + std::atoi(date_line.substr(0, 2).c_str());
    tm.tm_mon = std::atoi(date_line.substr(3, 2).c_str()) - 1;
    tm.tm_mday = std::atoi(date_line.substr(6, 2).c_str());
    tm.tm_hour = std::atoi(date_line.substr(9, 2).c_str());
    tm.tm_min = std::atoi(date_line.substr(12, 2).c_str());
    tm.tm_isdst = -1;
    const std::time_t as_utc = timegm(&tm);
    if (as_utc == std::time_t(-1)) return false;
    const long long minute = static_cast<long long>(stamp) - static_cast<long long>(stamp) % 60;
    const long long offset = static_cast<long long>(as_utc) - minute;
    const long long hours = offset / 3600;
    if (offset % 3600 != 0) return false;
    char tz[32];
    const char sign = hours > 0 ? '-' : '+';
    const long long abs_hours = hours < 0 ? -hours : hours;
    std::snprintf(tz, sizeof tz, "UTC%c%lld", sign, abs_hours);
#ifdef _WIN32
    _putenv(tz);
#else
    setenv("TZ", tz, 1);
#endif
    ::tzset();
    return true;
}

struct RebuildResult {
    bool decoded = false;
    bool fed = false;
    bool encoded = false;
    bool equal = false;
    size_t file_size = 0;
    size_t out_size = 0;
    size_t divergence = 0;
};

RebuildResult rebuild_and_encode(const char* path) {
    RebuildResult result;
    std::vector<u8> bytes;
    if (!read_file(path, bytes)) return result;
    result.file_size = bytes.size();
    ReplayFile file;
    if (!file.decode(bytes.data(), bytes.size())) return result;
    result.decoded = true;

    u64 stamp = 0;
    for (int i = 0; i < 8; ++i)
        stamp |= u64(file.top_header[0x0C + i]) << (8 * i);

    const std::string summary = file.user_blocks.empty() ? std::string() : file.user_blocks[0];
    const std::string version = user_line(summary, "Version");
    const std::string date_line = user_line(summary, "Date");
    if (version.empty() || !set_replay_timezone(date_line, std::time_t(stamp))) return result;

    ReplaySaveMeta meta;
    meta.date = std::time_t(stamp);
    meta.stage_field = read_i32(file.top_header.data() + 0x68);
    meta.version = "1.00c";
    static std::string version_storage;
    version_storage = version;
    if (version_storage == "1.00a") meta.version = "1.00a";
    const double x = double(file.slow_rate);
    const float r = float((100.0 - x) / 100.0);
    constexpr double kScale = 1099511627776.0;
    meta.slow_numerator = double((long long)(r * kScale + (r >= 0 ? 0.5 : -0.5)));
    meta.slow_denominator = kScale;

    ReplayRecorder recorder;
    if (!recorder.start(file.top_header)) return result;
    for (const ReplayStage& stage : file.stages) {
        if (!recorder.begin_stage(stage.header)) return result;
        for (u32 f = 0; f < stage.frame_count; ++f) {
            GameInput input;
            const ReplayFrame frame = stage.frame(f);
            input.held = frame.state;
            input.pressed = frame.pressed;
            input.released = frame.released;
            if (!recorder.record(input, stage.slow_rate(f))) return result;
        }
    }
    result.fed = true;
    std::vector<u8> out;
    if (!recorder.encode(out, file.player_name, file.score, meta)) return result;
    result.encoded = true;
    result.out_size = out.size();
    const size_t common = out.size() < bytes.size() ? out.size() : bytes.size();
    result.divergence = common;
    for (size_t b = 0; b < common; ++b) {
        if (out[b] != bytes[b]) {
            result.divergence = b;
            break;
        }
    }
    result.equal = out.size() == bytes.size() && result.divergence == out.size();
    if (!result.equal) {
        const size_t at = result.divergence;
        std::printf("#   bytes at %zu: got", at);
        for (size_t b = at; b < out.size() && b < at + 24; ++b)
            std::printf(" %02X", out[b]);
        std::printf("\n#               want");
        for (size_t b = at; b < bytes.size() && b < at + 24; ++b)
            std::printf(" %02X", bytes[b]);
        std::printf("\n");
    }
    return result;
}

TEST(encoded_replay_is_byte_identical_to_original_file) {
    for (const Sample& sample : kSamples) {
        const RebuildResult r = rebuild_and_encode(sample.path);
        std::printf("# %-38s decoded=%d fed=%d encoded=%d size=%zu vs %zu first_diff=%zu\n", sample.path,
                    int(r.decoded), int(r.fed), int(r.encoded), r.out_size, r.file_size, r.divergence);
        CHECK(r.decoded);
        CHECK(r.fed);
        CHECK(r.encoded);
        CHECK(r.equal);
    }
}

TEST(append_eof_roundtrips_through_decode) {
    SessionState source;
    source.stage = 1;
    HudPlayerState player;
    HudScoreState score;
    ReplayRecorder recorder;
    CHECK(recorder.start(ReplayRecorder::top_snapshot(source)));
    CHECK(recorder.begin_stage(ReplayRecorder::stage_snapshot(source, player, score, 42, 0, 51200, false)));
    for (u32 i = 0; i < 30; ++i) {
        GameInput input;
        input.held = u32(i % 7);
        CHECK(recorder.record(input, u8(60)));
    }
    ReplaySaveMeta meta;
    meta.append_eof = true;
    std::vector<u8> bytes;
    CHECK(recorder.encode(bytes, "EOFTEST", 100, meta));
    ReplayFile file;
    CHECK(file.decode(bytes.data(), bytes.size()));
    CHECK_EQ(file.stages[0].frame_count, 31u);
    CHECK_EQ(u32(file.stages[0].input.size()), 31u * 6);
    CHECK_EQ(file.stages[0].events.size(), size_t(1));
    const ReplayFrame last = file.stages[0].frame(30);
    CHECK(last.eof());
    CHECK_EQ(file.stages[0].slow_rate(30), u8(0));
    ReplayRecorder plain_recorder;
    CHECK(plain_recorder.start(ReplayRecorder::top_snapshot(source)));
    CHECK(plain_recorder.begin_stage(ReplayRecorder::stage_snapshot(source, player, score, 42, 0, 51200, false)));
    for (u32 i = 0; i < 30; ++i) {
        GameInput input;
        input.held = u32(i % 7);
        CHECK(plain_recorder.record(input, u8(60)));
    }
    std::vector<u8> plain_bytes;
    CHECK(plain_recorder.encode(plain_bytes, "NOEOF30", 100));
    ReplayFile plain_file;
    CHECK(plain_file.decode(plain_bytes.data(), plain_bytes.size()));
    CHECK_EQ(plain_file.stages[0].frame_count, 30u);
    CHECK_EQ(plain_file.stages[0].events.size(), size_t(1));
    CHECK(!plain_file.stages[0].frame(29).eof());
}

}

int main() { return th13test::run_all("encode 0x4484d0 全文件复刻"); }
