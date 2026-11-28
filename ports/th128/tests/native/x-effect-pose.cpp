#include "game/PerfectFreezeHost.hpp"
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <utility>
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
static Context context(float x, float y) {
    Context value;
    value.perfectCharge = 30000;
    value.x = x; value.y = y; value.radius = 12;
    value.keys = XKeyBit; value.xPressed = true;
    value.systemPresent = true; value.systemMode3c = 0; value.busy = 0;
    value.playerState = 1; value.mode = Mode::Normal;
    value.lasers = LaserContext::Empty; value.laserCount = 0;
    return value;
}
static void requireWorld(const std::vector<EffectSnapshot>& effects) {
    require(effects.size() == 3, "root plus two visible children");
    int roots = 0, children = 0;
    for (const auto& effect : effects) {
        require(effect.x == 320.f && effect.y == 240.f, "native effect world pose must be 320,240");
        if (effect.script == 36) { ++roots; require(effect.state.x == 320.f && effect.state.y == 240.f, "root local"); }
        if (effect.script == 37) { ++children; require(effect.state.x == 0.f && effect.state.y == 0.f, "child37 local");
            require(effect.state.sprite == 23 && effect.sprite && effect.sprite->textureEntry == 2, "child37 sprite"); }
        if (effect.script == 38) { ++children; require(effect.state.x == 0.f && effect.state.y == 0.f, "child38 local");
            require(effect.state.sprite == 24 && effect.sprite && effect.sprite->textureEntry == 2, "child38 sprite"); }
    }
    require(roots == 1 && children == 2, "exact effect scripts");
}

int main(int argc, char** argv) {
    try {
        require(argc == 2, "usage: x-effect-pose ice.anm");
        const auto ice = readFile(argv[1]);
        for (const auto pose : {std::pair<float, float>{100, 200}, std::pair<float, float>{400, 80}}) {
            std::vector<std::string> events;
            Services services;
            services.protect = [&](int ticks, std::string&) {
                events.push_back("protect:" + std::to_string(ticks));
                return ticks == 200 || ticks == 10;
            };
            services.freezeBullets = [&](const BulletRequest&, BulletResult& result, std::string&) {
                events.push_back("freeze");
                result = {0, 0, 0, true};
                return true;
            };
            Host host;
            std::string error;
            require(host.loadNativeEffect(ice, error), error);
            require(host.configure(context(pose.first, pose.second), std::move(services), error), error);
            require(host.activate(context(pose.first, pose.second), error), error);
            require(events.size() == 1 && events[0] == "protect:200", "activation protect200");
            requireWorld(host.effectSnapshots());
            for (int tick = 1; tick <= 120; ++tick) require(host.tick(false, error), error);
            require(events.size() == 122 && events[120] == "protect:10" && events[121] == "freeze",
                    "protect10 before the single phase120 freeze");
            requireWorld(host.effectSnapshots());
            for (int tick = 121; tick <= 200; ++tick) require(host.tick(false, error), error);
            requireWorld(host.effectSnapshots());
            require(!host.status().active && host.status().phase == 200, "phase200 ends host");
        }
        std::puts("PASS production Host ice36 effect pose 320,240 across player poses");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
