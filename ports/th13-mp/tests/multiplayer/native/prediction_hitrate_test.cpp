#include "../../harness/test_framework.hpp"

#include "game/ReplayFile.hpp"
#include "multiplayer/InputLanes.hpp"
#include "multiplayer/NetplayRuntime.hpp"
#include "multiplayer/SessionSetup.hpp"

#include <eagler/netplay/NetplayCore.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <functional>
#include <queue>
#include <string>
#include <sys/stat.h>
#include <vector>

namespace {

using th13::u8;
using th13::u16;
using th13::u32;

std::string repo_root() {
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    return root && *root ? root : "../../..";
}

bool read_file(const std::string& path, std::vector<u8>& out) {
    std::FILE* file = std::fopen(path.c_str(), "rb");
    if (!file) return false;
    std::fseek(file, 0, SEEK_END);
    const long size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    out.resize(size_t(size < 0 ? 0 : size));
    const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
    std::fclose(file);
    return got == out.size();
}

bool ends_with(const std::string& text, const char* suffix) {
    const size_t n = std::strlen(suffix);
    return text.size() >= n && text.compare(text.size() - n, n, suffix) == 0;
}

void collect_replays(const std::string& dir, const std::string& relative,
                     std::vector<std::string>& out) {
    DIR* handle = ::opendir(dir.c_str());
    if (!handle) return;
    std::vector<std::string> names;
    while (const dirent* item = ::readdir(handle)) {
        const std::string name = item->d_name;
        if (name == "." || name == "..") continue;
        names.push_back(name);
    }
    ::closedir(handle);
    std::sort(names.begin(), names.end());
    for (const std::string& name : names) {
        const std::string full = dir + "/" + name;
        const std::string rel = relative.empty() ? name : relative + "/" + name;
        struct stat info {};
        if (::stat(full.c_str(), &info) != 0) continue;
        if (S_ISDIR(info.st_mode)) {
            collect_replays(full, rel, out);
        } else if (S_ISREG(info.st_mode) && ends_with(name, ".rpy")) {
            out.push_back(rel);
        }
    }
}

bool replay_inputs(const std::string& path, std::vector<Netplay::FrameInput>& out) {
    std::vector<u8> bytes;
    if (!read_file(path, bytes)) {
        std::printf("  失败 读取录像 %s\n", path.c_str());
        return false;
    }
    th13::ReplayFile replay;
    if (!replay.decode(bytes.data(), bytes.size())) {
        std::printf("  失败 解码录像 %s: %s\n", path.c_str(), replay.error().c_str());
        return false;
    }
    if (replay.stages.empty()) {
        std::printf("  失败 录像无 stage: %s\n", path.c_str());
        return false;
    }
    out.clear();
    for (const th13::ReplayStage& stage : replay.stages) {
        for (u32 index = 0; index < stage.frame_count; ++index) {
            const th13::ReplayFrame frame = stage.frame(index);
            if (frame.eof()) break;
            const u32 held = u32(frame.state) & th13::multiplayer::InputLanes::HeldMask;
            Netplay::FrameInput input;
            if (!th13::multiplayer::InputLanes::Capture(held, false, input)) {
                std::printf("  失败 输入转换: %s stage=%u frame=%u state=%04x\n",
                            path.c_str(), unsigned(stage.number), index,
                            unsigned(frame.state));
                return false;
            }
            out.push_back(input);
        }
    }
    return !out.empty();
}

struct Metrics {
    std::uint64_t frames = 0;
    std::uint64_t rollbacks = 0;
    std::uint64_t distance_sum = 0;
    std::uint32_t max_distance = 0;
    std::uint64_t stall_frames = 0;
    std::uint64_t resimulated_frames = 0;
};

Metrics run_stream(const std::vector<Netplay::FrameInput>& remote, unsigned delay,
                   bool jitter) {
    th13::multiplayer::SessionSetup setup;
    setup.version = th13::multiplayer::SessionSetup::Version;
    setup.mode = 2;
    setup.session_id = 0x7013'0000ull + delay;
    setup.player_count = 2;
    setup.local_player = 0;
    setup.input_delay = 2;
    setup.prediction_limit = th13::multiplayer::SessionSetup::MaxRollbackFrames;
    const Netplay::CoreConfig config =
        th13::multiplayer::NetplayRuntime::CoreConfigFor(setup);
    Netplay::RollbackCore core;
    CHECK(core.Reset(config));
    if (!config.allowPrediction) {
        std::printf("  失败 评估器要求预测模式\n");
        return Metrics{};
    }

    Metrics metrics;
    const std::uint32_t total = std::uint32_t(remote.size());
    metrics.frames = total;

    std::uint32_t seed = 0x13579BDFu;
    using Arrival = std::pair<std::uint32_t, std::uint32_t>;
    std::priority_queue<Arrival, std::vector<Arrival>, std::greater<Arrival>> arrivals;
    for (std::uint32_t g = 0; g < total; ++g) {
        std::uint32_t extra = 0;
        if (jitter) {
            seed = seed * 1664525u + 1013904223u;
            extra = (seed >> 16) & 3u;
        }
        arrivals.push({g + delay + extra, g});
    }

    auto submit_frame = [&](std::uint32_t g) {
        const auto result = core.SubmitRemoteInput(1, g, remote[g]);
        CHECK(result != Netplay::RemoteInputResult::InvalidPlayer);
        CHECK(result != Netplay::RemoteInputResult::ConflictingConfirmedInput);
        CHECK(result != Netplay::RemoteInputResult::TooOld);
    };
    auto submit_due = [&](std::uint32_t time) {
        while (!arrivals.empty() && arrivals.top().first <= time) {
            const std::uint32_t g = arrivals.top().second;
            arrivals.pop();
            submit_frame(g);
        }
    };
    std::uint32_t next_frame = 0;
    auto handle_rollback = [&]() {
        if (!core.HasRollbackRequest()) return;
        const std::uint32_t target = core.RollbackFrame();
        CHECK(target < next_frame);
        if (target >= next_frame) return;
        const std::uint32_t distance = next_frame - target;
        ++metrics.rollbacks;
        metrics.distance_sum += distance;
        if (distance > metrics.max_distance) metrics.max_distance = distance;
        CHECK(core.RewindSimulationTo(target));
        next_frame = target;
        while (next_frame < target + distance) {
            const auto decision = core.PrepareFrame(next_frame);
            CHECK(decision.canAdvance);
            CHECK(core.MarkSimulated(next_frame, decision));
            ++next_frame;
            ++metrics.resimulated_frames;
        }
    };

    for (std::uint32_t frame = 0; frame < total; ++frame) {
        CHECK(core.ScheduleLocalInput(frame, Netplay::FrameInput{}));
        submit_due(frame);
        handle_rollback();
        auto decision = core.PrepareFrame(frame);
        if (!decision.canAdvance) {
            ++metrics.stall_frames;
            while (!decision.canAdvance && !arrivals.empty()) {
                const std::uint32_t g = arrivals.top().second;
                arrivals.pop();
                submit_frame(g);
                handle_rollback();
                decision = core.PrepareFrame(frame);
            }
            CHECK(decision.canAdvance);
            if (!decision.canAdvance) break;
        }
        CHECK(core.MarkSimulated(frame, decision));
        ++next_frame;
    }

    while (!arrivals.empty()) {
        const std::uint32_t g = arrivals.top().second;
        arrivals.pop();
        submit_frame(g);
        handle_rollback();
    }

    if (total > 0) {
        CHECK(next_frame == total);
        CHECK(core.LastSimulatedFrame() + 1 == total);
        CHECK(core.ConfirmedThrough(1) == total - 1);
    }
    CHECK(!core.HasRollbackRequest());
    return metrics;
}

}

TEST(prediction_hitrate_baseline) {
    const std::string root = repo_root();
    std::vector<std::string> replays;
    collect_replays(root + "/games/replays", "", replays);
    std::printf("# repository root: %s\n", root.c_str());
    std::printf("# replays: %zu\n", replays.size());
    CHECK(!replays.empty());
    for (const std::string& name : replays) {
        std::vector<Netplay::FrameInput> inputs;
        if (!replay_inputs(root + "/games/replays/" + name, inputs)) {
            CHECK(false);
            continue;
        }
        std::printf("# replay %s: %zu frames\n", name.c_str(), inputs.size());
        for (const unsigned delay : {2u, 4u, 6u, 8u}) {
            for (const bool jitter : {false, true}) {
                const Metrics metrics = run_stream(inputs, delay, jitter);
                const double per_1000 = metrics.frames == 0
                    ? 0.0 : double(metrics.rollbacks) * 1000.0 / double(metrics.frames);
                const double average = metrics.rollbacks == 0
                    ? 0.0 : double(metrics.distance_sum) / double(metrics.rollbacks);
                std::printf(
                    "# prediction {\"replay\":\"%s\",\"delay\":%u,\"jitter\":%s,"
                    "\"frames\":%llu,\"rollbacks\":%llu,"
                    "\"rollbacks_per_1000\":%.3f,\"avg_distance\":%.3f,"
                    "\"max_distance\":%u,\"stall_frames\":%llu,"
                    "\"resimulated_frames\":%llu}\n",
                    name.c_str(), delay, jitter ? "true" : "false",
                    (unsigned long long)metrics.frames,
                    (unsigned long long)metrics.rollbacks, per_1000, average,
                    metrics.max_distance,
                    (unsigned long long)metrics.stall_frames,
                    (unsigned long long)metrics.resimulated_frames);
            }
        }
    }
}

int main() { return th13test::run_all("p0.prediction_hitrate"); }
