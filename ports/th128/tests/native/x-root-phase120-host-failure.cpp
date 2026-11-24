#include "game/PerfectFreezeHost.hpp"
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace th128::perfect;

int main(int argc, char** argv) {
    if (argc != 2) return 2;
    std::ifstream input(argv[1], std::ios::binary);
    if (!input) return 2;
    const std::vector<std::uint8_t> animation(std::istreambuf_iterator<char>(input), {});
    Host host;
    Services services;
    unsigned freezeCalls = 0;
    unsigned protectCalls = 0;
    services.protect = [&](int ticks, std::string&) {
        ++protectCalls;
        return ticks == 200 || ticks == 10;
    };
    services.freezeBullets = [&](const BulletRequest&, BulletResult&, std::string& error) {
        ++freezeCalls;
        error = "injected required freeze failure";
        return false;
    };
    services.damage = [](const DamageRequest&, std::string&) { return true; };
    services.effect = [](const EffectRequest&, std::string&) { return true; };
    Context context;
    context.perfectCharge = 30000;
    context.x = 100;
    context.y = 200;
    context.radius = 12;
    context.keys = XKeyBit;
    context.xPressed = true;
    context.systemPresent = true;
    context.systemMode3c = 0;
    context.busy = 0;
    context.playerState = 1;
    context.mode = Mode::Normal;
    context.lasers = LaserContext::Empty;
    context.laserCount = 0;
    std::string error;
    if (!host.loadNativeEffect(animation, error) || !host.configure(context, services, error)
        || !host.activate(context, error)) {
        std::fprintf(stderr, "setup: %s\n", error.c_str());
        return 1;
    }
    if (host.effectSnapshots().size() != 3) {
        std::fprintf(stderr, "activation owner nodes=%zu\n", host.effectSnapshots().size());
        return 1;
    }
    for (int tick = 0; tick < 119; ++tick) {
        if (!host.tick(false, error)) {
            std::fprintf(stderr, "pre120 tick %d: %s\n", tick, error.c_str());
            return 1;
        }
    }
    if (host.status().phase != 119 || host.effectSnapshots().size() != 3) {
        std::fprintf(stderr, "phase119 state=%d nodes=%zu\n", host.status().phase,
                     host.effectSnapshots().size());
        return 1;
    }
    const bool failed = !host.tick(false, error);
    if (!failed || error.find("injected required freeze failure") == std::string::npos) {
        std::fprintf(stderr, "phase120 did not fail: %s\n", error.c_str());
        return 1;
    }
    if (host.status().phase != 120 || freezeCalls != 1 || host.effectSnapshots().size() != 3) {
        std::fprintf(stderr, "failed freeze changed root: phase=%d calls=%u nodes=%zu\n",
                     host.status().phase, freezeCalls, host.effectSnapshots().size());
        return 1;
    }
    const auto failedStatus = host.status();
    if (!failedStatus.failed) {
        std::fprintf(stderr, "failed host status did not latch\n");
        return 1;
    }
    const std::string sticky = error;
    error.clear();
    if (host.tick(false, error) || error != sticky || freezeCalls != 1) {
        std::fprintf(stderr, "failed freeze was not sticky: %s calls=%u\n", error.c_str(), freezeCalls);
        return 1;
    }
    if (!host.shutdown(error) || !error.empty() || !host.effectSnapshots().empty()) {
        std::fprintf(stderr, "owner cleanup failed: %s nodes=%zu\n", error.c_str(),
                     host.effectSnapshots().size());
        return 1;
    }
    if (!host.shutdown(error) || !error.empty() || !host.effectSnapshots().empty()) {
        std::fprintf(stderr, "repeated owner cleanup was not idempotent\n");
        return 1;
    }
    Host success;
    Services successServices;
    successServices.protect = [](int ticks, std::string&) { return ticks == 200 || ticks == 10; };
    successServices.freezeBullets = [](const BulletRequest&, BulletResult& result, std::string&) {
        result = {0, 0, 0, true};
        return true;
    };
    successServices.damage = [](const DamageRequest&, std::string&) { return true; };
    successServices.effect = [](const EffectRequest&, std::string&) { return true; };
    if (!success.loadNativeEffect(animation, error) || !success.configure(context, successServices, error)
        || !success.activate(context, error)) {
        std::fprintf(stderr, "success setup: %s\n", error.c_str());
        return 1;
    }
    for (int tick = 0; tick < 119; ++tick) {
        if (!success.tick(false, error)) {
            std::fprintf(stderr, "success pre120 tick %d: %s\n", tick, error.c_str());
            return 1;
        }
    }
    if (success.effectSnapshots().size() != 3) {
        std::fprintf(stderr, "success phase119 owner nodes=%zu\n", success.effectSnapshots().size());
        return 1;
    }
    if (!success.tick(false, error) || success.status().phase != 120 || !success.effectSnapshots().empty()) {
        std::fprintf(stderr, "success phase120 owner cleanup failed: %s nodes=%zu\n", error.c_str(),
                     success.effectSnapshots().size());
        return 1;
    }
    std::printf("PASS phase120 failed freeze keeps 3 owner nodes, sticky, cleanup once; protects=%u\n",
                protectCalls);
    return 0;
}
