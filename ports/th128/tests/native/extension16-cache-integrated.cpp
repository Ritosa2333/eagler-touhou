#include "game/BulletUpdateHost.hpp"
#include "game/BulletExtension16.hpp"
#include "game/BulletExtension256.hpp"
#include "game/BulletVisuals.hpp"
#include <cmath>
#include <bit>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

using namespace th128::bullet;

static std::vector<unsigned char> readFile(const char* path) {
    std::ifstream file(path, std::ios::binary);
    assert(file);
    return {std::istreambuf_iterator<char>(file), {}};
}
static void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAIL " << message << '\n';
        std::exit(1);
    }
}
static ExtensionRaw record800000() {
    ExtensionRaw record;
    record.parameterR = -999999.0f; record.parameterS = -999999.0f;
    record.parameterA = -999999; record.parameterB = -999999;
    record.type = 0x800000; record.control = 1;
    return record;
}
static ExtensionRaw record256() {
    ExtensionRaw record;
    record.parameterR = -999999.0f; record.parameterS = -999999.0f;
    record.parameterA = 120; record.parameterB = 0;
    record.type = 256; record.control = 0;
    return record;
}
static ExtensionRaw recordC() {
    ExtensionRaw record;
    record.parameterR = 0; record.parameterS = 3.5f;
    record.parameterA = 40; record.parameterB = 1;
    record.parameterC = 1; record.parameterD = 3;
    record.type = 16; record.control = 0;
    return record;
}
static Birth makeBirth(const ExtensionRaw& first, const ExtensionRaw& second, const ExtensionRaw& third) {
    Birth birth;
    birth.type = 8; birth.color = 10; birth.angle = -3.002182f; birth.speed = 2.5f;
    birth.state.position = {10, 100, .1f};
    birth.state.velocity = {0, 0, 0};
    birth.state.flags = 0x11; birth.state.motionState = 1; birth.state.transformSound = 67;
    birth.state.extensionIndex = 1; birth.extensionListEmpty = false;
    birth.state.laterExtensions[0] = first;
    birth.state.laterExtensions[1] = second;
    birth.state.laterExtensions[2] = third;
    return birth;
}
int main(int argc, char** argv) {
    assert(argc == 3);
    std::string error;
    const auto profileBytes = readFile(argv[1]);
    const auto animation = readFile(argv[2]);
    Profile profile; require(profile.load(profileBytes.data(), profileBytes.size(), error), error.c_str());
    BulletVisuals visuals; require(visuals.load(animation.data(), animation.size(), error), error.c_str());
    const auto makeVisual = [&]() {
        BulletVisuals::CreateInfo info;
        info.type = 8; info.color = 10; info.script = 43; info.birthInterrupt = 8;
        for (int index = 0; index < 4; ++index) {
            const auto mapped = profile.remap(10, index); require(mapped.has_value(), "profile remap");
            info.spriteRemap[std::size_t(index)] = *mapped;
        }
        auto visual = visuals.create(info, error); require(visual.has_value(), error.c_str());
        visual->primary.control().integers[0] = 1;
        return std::move(*visual);
    };

    auto birth = makeBirth(record800000(), record256(), recordC());
    auto visual = makeVisual();
    bool removed = false;
    int sounds = 0;
    const auto sound = [&](int identifier, std::string&) { ++sounds; require(identifier == 67, "sound id"); return true; };
    require(updateBulletWithKnownMisses(birth, visuals, visual, {0, 0, 0}, 0, removed, error, {}, {}, {}, sound), error.c_str());
    require(birth.extension800000.position.x == 10 && birth.extension800000.position.y == 100,
        "800000 cached position");
    require(birth.extension256.remaining == 119 && (birth.state.activeExtensions & 0x100u) != 0,
        "256 ongoing after first tick");
    require((birth.state.activeExtensions & 16u) != 0, "C1 type16 active");
    birth.state.position = {200, 50, .2f};
    bool boundarySound = false;
    for (int tick = 0; tick < 45; ++tick) {
        const int before = sounds;
        require(updateBulletWithKnownMisses(birth, visuals, visual, {0, 0, 0}, 0, removed, error, {}, {}, {}, sound), error.c_str());
        if (sounds != before) boundarySound = true;
    }
    require(boundarySound && sounds == 1, "C1/D3 boundary sound exactly once");
    require((birth.state.activeExtensions & 16u) == 0, "C1 type16 finishes at period boundary");
    require(birth.extension256.remaining > 0, "256 remains active after C1 boundary");
    const float expected = std::atan2(0.0 - 100.0, 0.0 - 10.0);
    require(std::fabs(birth.angle - expected) < 1e-4f,
        "D3 aims from cached 800000 position, not current position");
    require(std::fabs(birth.state.position.z - .2f) < 1e-6f, "current Z preserved");

    auto noCache = initializeExtension16(recordC(), 2.5f, -3.002182f);
    require(noCache.has_value(), "no-cache C init");
    noCache->time = 40; noCache->fraction = 40;
    require(!stepExtension16(*noCache, 2.5f, -3.002182f, {}, {200, 50, .2f}, {0, 0, 0}, -1, nullptr),
        "missing typed cached context rejects strictly");
    std::cout << "PASS actual 800000->256->C1/D3 host cache aim, one sound, Z and strict missing cache\n";
}
