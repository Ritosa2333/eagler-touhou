#include "game/PerfectFreezeHost.hpp"
#include "game/PlayerHitHost.hpp"
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace th128;
using namespace th128::perfect;

static std::vector<unsigned char> readFile(const char* path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) throw std::runtime_error("missing input");
    return {std::istreambuf_iterator<char>(input), {}};
}
static void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}
static void requireTimer(const player::PlayerHitHost& player, int previous, int current, float fraction) {
    const auto timer = player.snapshot().protection;
    require(timer.previous == previous && timer.current == current && timer.fractional == fraction &&
            timer.clockSource == 0x4b6f58 && (timer.flags & 1U) &&
            player.snapshot().gate.counter438dc == current, "exact player protection timer fields");
}
static Context context() {
    Context value;
    value.perfectCharge = 30000;
    value.x = 320;
    value.y = 416;
    value.radius = 256;
    value.keys = XKeyBit;
    value.xPressed = true;
    value.systemPresent = true;
    value.systemMode3c = 0;
    value.busy = 0;
    value.playerState = 1;
    value.mode = Mode::Normal;
    value.lasers = LaserContext::Empty;
    value.laserCount = 0;
    return value;
}

int main(int argc, char** argv) {
    try {
        require(argc == 3, "usage: x-protection-integration ice.anm pl00.anm");
        const auto ice = readFile(argv[1]);
        const auto pl00 = readFile(argv[2]);
        std::string error;
        player::PlayerHitHost player;
        const player::hit::Timer zero{-1, 0, 0, 0x4b6f58, 1};
        const player::HitHostSnapshot snapshot{{1, 0, 0}, zero, zero, zero, {true, 0}, {0, 0, 0}, {true, 0}};
        require(player.configure(pl00.data(), pl00.size(), snapshot,
            [](int, player::hit::Position, int, std::string&) { return true; },
            [](int, std::string&) { return true; }, error), error);

        int freezeCalls = 0, protect200 = 0, protect10 = 0;
        bool frozenAtTen = false;
        Services services;
        services.freezeBullets = [&](const BulletRequest& request, BulletResult& result, std::string&) {
            ++freezeCalls;
            frozenAtTen = player.snapshot().protection.current == 10;
            require(request.overrideImmune, "freeze request must be native override");
            result = {1, 0, 0, true};
            return true;
        };
        services.protect = [&](int ticks, std::string& reason) {
            if (ticks == 200) { ++protect200; return player.assignPerfectProtection200(reason); }
            if (ticks == 10) { ++protect10; return player.assignPerfectProtection10(reason); }
            reason = "unexpected perfect protection request";
            return false;
        };

        Host host;
        require(host.loadNativeEffect(ice, error), error);
        require(host.configure(context(), std::move(services), error), error);
        require(host.activate(context(), error), error);
        require(protect200 == 1 && protect10 == 0 && freezeCalls == 0, "activation calls protect200 only");
        requireTimer(player, 199, 200, 200.f);
        require(host.status().protectionTicks == 200 && host.status().phase == 0, "activation mirror is 200");

        const auto beforePause = player.snapshot().protection;
        require(host.tick(true, error), error);
        require(player.snapshot().protection.current == beforePause.current && host.status().phase == 0,
                "paused tick does not advance protection/phase");

        for (int tick = 0; tick < 120; ++tick) {
            require(player.stepProtection(1, error), error);
            require(host.tick(false, error), error);
            require(host.status().protectionTicks == player.snapshot().protection.current,
                    "status mirrors actual player timer before phase120");
            if (tick == 0) requireTimer(player, 9, 10, 10.f);
            if (tick == 119) {
                require(freezeCalls == 1 && frozenAtTen && protect10 == 120,
                        "phase120 refresh10 must precede one whole-field freeze");
            }
        }

        for (int tick = 120; tick < 180; ++tick) {
            require(player.stepProtection(1, error), error);
            require(host.tick(false, error), error);
            require(player.snapshot().protection.current == 10 && host.status().protectionTicks == 10,
                    "pre180 ticks refresh player protection to 10");
        }

        for (int tick = 180; tick < 190; ++tick) {
            require(player.stepProtection(1, error), error);
            require(host.tick(false, error), error);
            const int actual = player.snapshot().protection.current;
            require(host.status().protectionTicks == actual, "status mirrors post180 owner timer");
            if (tick == 180) require(actual == 9, "phase181 decrements native timer to 9");
            if (tick == 189) require(actual == 0, "phase190 timer reaches zero");
        }

        for (int tick = 190; tick < 200; ++tick) {
            require(player.stepProtection(1, error), error);
            require(host.tick(false, error), error);
            require(player.snapshot().protection.current == 0 && host.status().protectionTicks == 0,
                    "expired protection stays zero to phase200");
        }
        require(!host.status().active && host.status().phase == 200 && freezeCalls == 1,
                "phase200 ends host without another freeze");
        require(protect200 == 1 && protect10 == 180, "only activation200 and pre180 protection10 refresh");
        std::puts("PASS real PlayerHitHost X protection200/10 refresh and phase200 expiry");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
