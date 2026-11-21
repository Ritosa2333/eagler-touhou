#include "game/PerfectFreezeHost.hpp"
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace th128::perfect;

static std::vector<unsigned char> readFile(const char* path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) throw std::runtime_error("missing ice.anm");
    return {std::istreambuf_iterator<char>(input), {}};
}
static void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
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
        require(argc == 2, "usage: x-fidelity-native ice.anm");
        const auto ice = readFile(argv[1]);
        std::vector<std::string> events;
        Services services;
        services.freezeBullets = [&](const BulletRequest& request, BulletResult& result, std::string&) {
            require(request.overrideImmune && request.perfect, "phase120 must use override-immune request");
            events.push_back("freeze");
            result = {1, 0, 0, true};
            return true;
        };
        services.protect = [&](int ticks, std::string&) {
            events.push_back("protect:" + std::to_string(ticks));
            return ticks == 200 || ticks == 10;
        };
        Host host;
        std::string error;
        require(host.loadNativeEffect(ice, error), error);
        require(host.configure(context(), std::move(services), error), error);
        require(host.activate(context(), error), error);
        require(events.size() == 1 && events[0] == "protect:200", "activation protection is 200");
        require(host.tick(false, error), error);
        require(events.size() == 2 && events[1] == "protect:10", "next native tick rewrites protection to 10");
        for (int tick = 1; tick < 119; ++tick) require(host.tick(false, error), error);
        require(events.size() == 120 && events.back() == "protect:10", "pre120 ticks refresh protection10");
        require(host.tick(false, error), error);
        require(events.size() == 122 && events[120] == "protect:10" && events[121] == "freeze",
                "phase120 must refresh protection10 before whole-field freeze");
        require(host.status().frozen == 1, "freeze result propagated");
        const auto beforePause = events.size();
        require(host.tick(true, error) && events.size() == beforePause, "paused tick does not repeat phase120");
        for (int tick = 120; tick < 180; ++tick) require(host.tick(false, error), error);
        const auto at180 = events.size();
        require(at180 == 182 && events.back() == "protect:10", "phase180 is the last protection10 refresh");
        require(host.tick(false, error) && events.size() == at180, "post180 ticks do not refresh protection");
        std::puts("PASS X phase120 native protection10-before-freeze");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
