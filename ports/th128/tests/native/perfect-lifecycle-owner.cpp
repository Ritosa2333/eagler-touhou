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
static bool protectionMirror(int current, int step, const Status& status) {
    return current == ProtectionTicks - step && status.protectionTicks == current;
}
static Context context(int charge) {
    Context value;
    value.perfectCharge = charge;
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
        require(argc == 3, "usage: ice.anm pl00.anm");
        const auto ice = readFile(argv[1]);
        const auto pl00 = readFile(argv[2]);
        std::string error;
        player::PlayerHitHost player;
        const player::hit::Timer zero{-1, 0, 0, 0x4b6f58, 1};
        const player::HitHostSnapshot snapshot{{1, 0, 0}, zero, zero, zero, {true, 0}, {0, 0, 0}, {true, 0}};
        require(player.configure(pl00.data(), pl00.size(), snapshot,
            [](int, player::hit::Position, int, std::string&) { return true; },
            [](int, std::string&) { return true; }, error), error);

        int freezeCalls = 0, damageCalls = 0, protectCalls = 0;
        BulletRequest seenBullet;
        auto freeze = [&](const BulletRequest& request, BulletResult& result, std::string&) {
            seenBullet = request;
            ++freezeCalls;
            result.frozen = 2;
            result.cleared = 1;
            result.completed = true;
            return true;
        };
        auto damage = [&](const DamageRequest&, std::string&) { ++damageCalls; return true; };
        auto protect = [&](int ticks, std::string& reason) {
            ++protectCalls;
            return ticks == ProtectionTicks && player.assignPerfectProtection200(reason);
        };
        Services services{freeze, damage, {}, protect, {}};
        Host host;
        require(host.loadNativeEffect(ice, error), error);
        require(host.configure(context(30000), services, error), error);
        require(host.activate(context(30000), error), error);
        require(protectCalls == 1 && host.status().charge == 20000 && host.status().active, "activation state");
        auto protection = player.snapshot().protection;
        require(protection.previous == 199 && protection.current == 200 &&
                protection.fractional == 200.f && protection.clockSource == 0x4b6f58 &&
                (protection.flags & 1U) && player.snapshot().gate.counter438dc == 200,
                "native perfect protection timer assignment");
        require(host.tick(true, error) && player.snapshot().protection.current == 200 && freezeCalls == 0,
                "paused tick must not advance");
        bool saw120 = false;
        for (int tick = 0; tick < ProtectionTicks; ++tick) {
            require(host.tick(false, error), error);
            require(player.stepProtection(1, error), error);
            const auto current = player.snapshot().protection.current;
            require(host.status().protectionTicks == current, "host/player protection mirror");
            require(protectionMirror(current, tick + 1, host.status()), "protection step count");
            if (tick == 119) {
                saw120 = true;
                require(freezeCalls == 1 && seenBullet.overrideImmune && current == 80,
                        "phase120 must freeze once and leave 80 protection ticks");
            }
        }
        require(saw120 && freezeCalls == 1 && damageCalls == 0 && !host.status().active &&
                player.snapshot().protection.current == 0,
                "known-empty laser phase must not call damage and protection must expire at200");

        auto released = context(20000);
        released.xPressed = false;
        require(!host.activate(released, error), "released X must reset latch");
        require(host.activate(context(20000), error), error);
        require(host.status().charge == 10000 && host.status().active, "repeat activation charge spend");
        require(host.shutdown(error), error);

        auto populated = context(30000);
        populated.lasers = LaserContext::Populated;
        populated.laserCount = 2;
        Host noLaser;
        require(!noLaser.configure(populated, services, error), "populated laser domain must remain unsupported");
        auto unknown = context(30000);
        unknown.lasers = LaserContext::Unknown;
        Host unknownHost;
        require(!unknownHost.configure(unknown, services, error), "unknown laser domain must remain unsupported");

        auto failingFreeze = [&](const BulletRequest&, BulletResult&, std::string& reason) {
            reason = "required freeze owner rejected";
            return false;
        };
        Services failing{failingFreeze, damage, {}, protect, {}};
        Host failed;
        require(failed.loadNativeEffect(ice, error), error);
        require(failed.configure(context(30000), failing, error), error);
        require(failed.activate(context(30000), error), error);
        for (int tick = 0; tick < NativeOverridePhase; ++tick)
            require(failed.tick(false, error) == (tick + 1 < NativeOverridePhase), "freeze failure sticky");
        require(failed.status().failed && failed.status().charge == 20000, "failure must not refund");
        std::puts("PASS perfect lifecycle owner protection200/known-empty/phase120/200-tick/repeat/fail-sticky");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
