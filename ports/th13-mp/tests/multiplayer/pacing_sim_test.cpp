#include "../../web/cpp/multiplayer/FrameSchedule.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

namespace {
using th13::multiplayer::FrameSchedule;
using th13::multiplayer::SlowdownGovernor;

constexpr double kInterval = 1000. / 60.;
constexpr unsigned kWindow = SlowdownGovernor::kWindow;
constexpr double kLegacyBudget = 11.5;

struct Scenario {
    double cost_ms[2] = {2, 2};
    double raf_ms[2] = {kInterval, kInterval};
    double latency_ms = 30;
    double hitch_at_ms = -1, hitch_ms = 0;
    bool handling = true;
};

struct Metrics {
    double speed[2] = {0, 0};
    double max_freeze_ms[2] = {0, 0};
    double mean_depth[2] = {0, 0};
    unsigned stalled_callbacks[2] = {0, 0};
    double frame_gap_p99_ms[2] = {0, 0};
};

struct Peer {
    FrameSchedule schedule;
    SlowdownGovernor governor;
    double busy_until = 0, next_callback = 0, previous = -1, step_cost = 0;
    std::vector<double> captured;
    unsigned arrived_seen = 0;
    std::vector<double> sim_times;
    double depth_sum = 0;
    unsigned depth_samples = 0, stalled = 0;
};

Metrics run(const Scenario& scenario, double seconds = 40) {
    Peer peers[2];
    for (auto& peer : peers) peer.captured.reserve(size_t(seconds * 70));
    const unsigned max_steps = scenario.handling ? 48 : 2;
    const auto arrived = [&](int to, double now) {
        const auto& from = peers[1 - to].captured;
        unsigned count = 0;
        while (count < from.size() && from[count] + scenario.latency_ms <= now) ++count;
        return count;
    };
    const auto depth_for = [&](int index, double now) {
        const unsigned count = arrived(index, now);
        const unsigned next = unsigned(peers[index].captured.size());
        return count ? SlowdownGovernor::depth_of(next, count - 1) : next + 1;
    };
    for (double now = 0; now < seconds * 1000; now += .5) {
        for (int index = 0; index < 2; ++index) {
            Peer& peer = peers[index];
            if (now < peer.busy_until) continue;
            const double hitch_start = scenario.hitch_at_ms;
            if (index == 0 && hitch_start >= 0 && now >= hitch_start && now < hitch_start + .5 &&
                peer.busy_until <= hitch_start) {
                peer.busy_until = now + scenario.hitch_ms;
                continue;
            }
            const bool wake = arrived(index, now) != peer.arrived_seen &&
                (peer.schedule.retry_pending() || peer.schedule.has_work());
            if (now < peer.next_callback && !wake) continue;
            const bool display = now >= peer.next_callback;
            const double delta = peer.previous < 0 ? 0 : (now - peer.previous) / 1000.;
            if (display) peer.previous = now;
            if (display) {
                const unsigned depth = depth_for(index, now);
                double scale = 1.;
                if (scenario.handling) scale = std::max(scale, peer.governor.update(depth));
                if (scenario.handling) peer.schedule.advance_gap(delta / scale);
                else peer.schedule.advance(delta / scale);
            } else if (scenario.handling) {
                peer.schedule.advance_gap(0);
            } else {
                peer.schedule.advance(0);
            }
            peer.arrived_seen = arrived(index, now);
            const double kCallbackBudget = scenario.handling
                ? std::clamp(scenario.raf_ms[index] * .75, kLegacyBudget, 500.) : kLegacyBudget;
            const double kCatchupBudget = kCallbackBudget * (8.5 / 11.5);
            double elapsed = 0;
            unsigned steps = 0;
            bool promised = false;
            while (steps < max_steps) {
                const double started = elapsed;
                if (steps >= 1 && (!promised || (!scenario.handling && elapsed >= kCatchupBudget))) break;
                if (!peer.schedule.owed()) break;
                const unsigned depth = depth_for(index, now + elapsed);
                if (depth > kWindow) { peer.schedule.network_wait(); ++peer.stalled; break; }
                peer.depth_sum += depth; ++peer.depth_samples;
                promised = steps + 1 < max_steps && peer.schedule.owed() > 1 && started < kCatchupBudget &&
                    (!scenario.handling || started + 2. * peer.step_cost <= kCallbackBudget);
                peer.captured.push_back(now + elapsed);
                peer.sim_times.push_back(now + elapsed);
                peer.schedule.complete();
                elapsed += scenario.cost_ms[index];
                peer.step_cost += (scenario.cost_ms[index] - peer.step_cost) * .25;
                ++steps;
            }
            peer.schedule.cpu_yield();
            peer.busy_until = now + elapsed;
            if (display) {
                const double period = scenario.raf_ms[index];
                const double boundary = std::ceil((peer.busy_until - 1e-9) / period) * period;
                peer.next_callback = std::max(boundary, now + period * .5);
            }
        }
    }
    Metrics metrics;
    const double from = 5000, to = seconds * 1000 - 5000;
    for (int index = 0; index < 2; ++index) {
        const auto& times = peers[index].sim_times;
        unsigned frames = 0;
        double previous = -1;
        std::vector<double> gaps;
        for (double t : times) {
            if (t < from || t > to) continue;
            ++frames;
            if (previous >= 0) gaps.push_back(t - previous);
            previous = t;
        }
        std::sort(gaps.begin(), gaps.end());
        metrics.speed[index] = frames / ((to - from) / 1000.) / 60.;
        metrics.max_freeze_ms[index] = gaps.empty() ? 0 : gaps.back();
        metrics.frame_gap_p99_ms[index] = gaps.empty() ? 0 : gaps[size_t(gaps.size() * .99)];
        metrics.mean_depth[index] = peers[index].depth_samples
            ? peers[index].depth_sum / peers[index].depth_samples : 0;
        metrics.stalled_callbacks[index] = peers[index].stalled;
    }
    return metrics;
}

void report(const char* name, const Metrics& off, const Metrics& on) {
    std::printf("%-34s speed off %.3f/%.3f on %.3f/%.3f | freeze off %5.0f/%5.0f on %5.0f/%5.0f ms |"
                " depth off %.1f/%.1f on %.1f/%.1f | stalls off %u/%u on %u/%u\n", name,
                off.speed[0], off.speed[1], on.speed[0], on.speed[1],
                off.max_freeze_ms[0], off.max_freeze_ms[1], on.max_freeze_ms[0], on.max_freeze_ms[1],
                off.mean_depth[0], off.mean_depth[1], on.mean_depth[0], on.mean_depth[1],
                off.stalled_callbacks[0], off.stalled_callbacks[1], on.stalled_callbacks[0], on.stalled_callbacks[1]);
}

Metrics both(const char* name, Scenario scenario, Metrics* on_out = nullptr) {
    scenario.handling = false;
    const Metrics off = run(scenario);
    scenario.handling = true;
    const Metrics on = run(scenario);
    report(name, off, on);
    if (on_out) *on_out = on;
    return off;
}
}

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    Metrics on, off;
    off = both("healthy 2ms/2ms 30ms", {}, &on);
    for (int i = 0; i < 2; ++i) assert(on.speed[i] > .99 && off.speed[i] > .99);

    Scenario far; far.latency_ms = 100;
    off = both("healthy 2ms/2ms 100ms", far, &on);
    for (int i = 0; i < 2; ++i) assert(on.speed[i] > .98);

    Scenario slow; slow.cost_ms[0] = 24;
    off = both("peer0 24ms/frame", slow, &on);
    for (int i = 0; i < 2; ++i) assert(on.speed[i] > .5 * .95);

    Scenario mid; mid.cost_ms[0] = 12;
    off = both("peer0 12ms/frame", mid, &on);
    for (int i = 0; i < 2; ++i) assert(on.speed[i] > .95);

    Scenario both_slow; both_slow.cost_ms[0] = both_slow.cost_ms[1] = 22;
    off = both("both 22ms/frame", both_slow, &on);
    for (int i = 0; i < 2; ++i) assert(on.speed[i] > .5 * .95);

    Scenario hitch; hitch.hitch_at_ms = 15000; hitch.hitch_ms = 400;
    off = both("400ms hitch peer0", hitch, &on);

    Scenario display; display.raf_ms[0] = 50;
    off = both("peer0 presents at 20 Hz", display, &on);
    assert(on.speed[0] > .97 && on.speed[1] > .97);
    Scenario slow_display; slow_display.raf_ms[0] = slow_display.raf_ms[1] = 200;
    off = both("both present at 5 Hz", slow_display, &on);
    assert(on.speed[0] > .9 && on.speed[1] > .9);
    assert(off.speed[0] < .7);
    Scenario display_far; display_far.raf_ms[0] = 66.7; display_far.raf_ms[1] = 40;
    off = both("presents at 15 Hz / 25 Hz", display_far, &on);
    assert(on.speed[0] > .95 && on.speed[1] > .95);

    Scenario crawl; crawl.raf_ms[0] = 700; crawl.raf_ms[1] = 400; crawl.cost_ms[0] = 4; crawl.cost_ms[1] = 4;
    off = both("presents at 1.4 Hz / 2.5 Hz", crawl, &on);
    Scenario crawl2 = crawl; crawl2.cost_ms[0] = 24;
    off = both("1.4/2.5 Hz, peer0 24ms", crawl2, &on);
    Scenario crawl3 = crawl; crawl3.raf_ms[1] = 700;
    off = both("1.4/1.4 Hz", crawl3, &on);

    Scenario weighty; weighty.raf_ms[0] = 100; weighty.cost_ms[0] = 6;
    off = both("peer0 10 Hz, 6ms frames", weighty, &on);
    assert(on.speed[0] > .97 && on.speed[1] > .97);

    Scenario heavy; heavy.cost_ms[0] = 16; heavy.cost_ms[1] = 10; heavy.latency_ms = 80;
    off = both("16ms/10ms 80ms", heavy, &on);
    return 0;
}
