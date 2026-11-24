#include "game/PerfectFreezeHost.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using namespace th128::perfect;

namespace {
std::vector<unsigned char> readFile(const char* path) {
    std::ifstream input(path, std::ios::binary);
    assert(input);
    return std::vector<unsigned char>(std::istreambuf_iterator<char>(input), {});
}
Context context(int charge) {
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
}

int main(int argc, char** argv) {
    assert(argc == 2);
    const auto ice = readFile(argv[1]);
    assert(!ice.empty());

    int protectCalls = 0, damageCalls = 0, effectCalls = 0, freezeCalls = 0;
    BulletRequest seenBullet;
    EffectRequest seenEffect;
    auto freeze = [&](const BulletRequest& request, BulletResult& result, std::string&) {
        seenBullet = request;
        ++freezeCalls;
        result.frozen = 2;
        result.cleared = 1;
        result.immune = 1;
        result.completed = true;
        return true;
    };
    auto damage = [&](const DamageRequest&, std::string&) { ++damageCalls; return true; };
    auto effect = [&](const EffectRequest& request, std::string&) {
        seenEffect = request;
        ++effectCalls;
        return true;
    };
    auto protect = [&](int ticks, std::string&) { ++protectCalls; assert(ticks == ProtectionTicks); return true; };
    Services services{freeze, damage, effect, protect, {}};

    Host host;
    std::string error;
    assert(host.loadNativeEffect(ice, error));
    assert(host.configure(context(30000), services, error));
    assert(host.activate(context(30000), error));
    auto status = host.status();
    assert(status.active && !status.failed && status.charge == 20000);
    assert(status.protectionTicks == ProtectionTicks && status.frozen == 0 && status.cleared == 0 && status.immune == 0);
    assert(status.effect && protectCalls == 1 && damageCalls == 1 && effectCalls == 0 && freezeCalls == 0);
    assert(host.activation() && host.activation()->chargeAfter == 20000 && host.activation()->bullets.frozen == 0);

    assert(host.tick(true, error));
    assert(host.status().protectionTicks == ProtectionTicks && freezeCalls == 0);
    assert(host.tick(false, error));
    assert(host.status().protectionTicks == ProtectionTicks - 1 && freezeCalls == 0);
    for (int frame = 1; frame < NativeOverridePhase; ++frame) {
        assert(host.tick(false, error));
        if (frame < NativeOverridePhase - 1) assert(freezeCalls == 0);
    }
    assert(freezeCalls == 1 && seenBullet.overrideImmune);
    assert(host.status().frozen == 2 && host.activation()->bullets.frozen == 2);
    assert(host.status().protectionTicks == ProtectionTicks - NativeOverridePhase);
    for (int frame = 0; frame < ProtectionTicks - NativeOverridePhase; ++frame) assert(host.tick(false, error));
    assert(!host.status().active && host.status().protectionTicks == 0);

    assert(!host.activate(context(20000), error));
    assert(host.status().charge == 20000 && host.status().failed == false);
    auto released = context(20000);
    released.xPressed = false;
    assert(!host.activate(released, error));
    assert(host.activate(context(20000), error));
    assert(host.status().charge == 10000 && host.status().active);
    for (int frame = 0; frame < ProtectionTicks; ++frame) assert(host.tick(false, error));
    assert(!host.status().active && freezeCalls == 2);
    released.perfectCharge = 10000;
    assert(!host.activate(released, error));
    assert(host.activate(context(10000), error));
    assert(host.status().charge == 0);
    for (int frame = 0; frame < ProtectionTicks; ++frame) assert(host.tick(false, error));
    assert(!host.status().active && freezeCalls == 3);
    released.perfectCharge = 0;
    assert(!host.activate(released, error));
    assert(!host.activate(context(0), error));
    assert(host.status().charge == 0);
    assert(host.shutdown(error));
    assert(host.effectSnapshots().empty());

    int failProtect = 0, failFreezeCalls = 0;
    Services failing{[&](const BulletRequest&, BulletResult&, std::string& reason) {
                         ++failFreezeCalls;
                         reason = "required bullet owner rejected";
                         return false;
                     },
                     [](const DamageRequest&, std::string&) { return true; },
                     [](const EffectRequest&, std::string&) { return true; },
                     [&](int, std::string&) { ++failProtect; return true; },
                     {}};
    Host failed;
    assert(failed.configure(context(30000), failing, error));
    assert(failed.activate(context(30000), error));
    assert(failed.status().active && failed.status().charge == 20000 && failProtect == 1);
    for (int frame = 0; frame < NativeOverridePhase; ++frame) {
        const bool ok = failed.tick(false, error);
        assert(ok == (frame + 1 < NativeOverridePhase));
    }
    assert(failed.status().failed && failed.status().charge == 20000);
    assert(failFreezeCalls == 1);

    int popLaserCalls = 0;
    Services laserServices{freeze, damage, effect, protect, [&](int count, std::string&) {
        ++popLaserCalls;
        return count == 2;
    }};
    auto populated = context(30000);
    populated.lasers = LaserContext::Populated;
    populated.laserCount = 2;
    Host lasers;
    assert(lasers.configure(populated, laserServices, error));
    assert(lasers.activate(populated, error));
    assert(lasers.status().lasers == 2 && popLaserCalls == 1);
    assert(seenEffect.layer == -1 && seenEffect.script == -1);

    auto noLaserService = context(30000);
    noLaserService.lasers = LaserContext::Populated;
    noLaserService.laserCount = 2;
    Host unownedLaser;
    assert(!unownedLaser.configure(noLaserService, services, error));

    auto deathbomb = context(30000);
    deathbomb.mode = Mode::DeathbombWindow;
    Host window;
    assert(!window.configure(deathbomb, services, error));
    assert(!window.status().configured && !window.status().failed);

    auto busy = context(30000);
    busy.busy = 1;
    Host gated;
    assert(gated.configure(busy, services, error));
    assert(!gated.activate(busy, error));
    assert(gated.status().charge == 30000 && !gated.status().failed);

    std::cout << "actual ice PerfectFreezeHost X gate, stock, protection, freeze/damage/effect services passed\n";
}
