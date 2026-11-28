#include "game/BulletUpdateHost.hpp"
#include "game/BulletExtension256.hpp"
#include "game/BulletVisuals.hpp"
#include <bit>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
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

static std::uint32_t bits(float value) {
    return std::bit_cast<std::uint32_t>(value);
}

static ExtensionRaw record800000() {
    ExtensionRaw record;
    record.parameterR = -999999.0f;
    record.parameterS = -999999.0f;
    record.parameterA = -999999;
    record.parameterB = -999999;
    record.type = 0x800000;
    record.control = 1;
    return record;
}

static ExtensionRaw record256() {
    ExtensionRaw record;
    record.parameterR = -999999.0f;
    record.parameterS = -999999.0f;
    record.parameterA = 120;
    record.parameterB = 0;
    record.type = 256;
    record.control = 0;
    return record;
}

static ExtensionRaw record16() {
    ExtensionRaw record;
    record.parameterR = 0;
    record.parameterS = -999999.0f;
    record.parameterA = 60;
    record.parameterB = 1;
    record.parameterC = 1;
    record.parameterD = 3;
    record.type = 16;
    record.control = 0;
    return record;
}

static Birth makeBirth(const ExtensionRaw& first, const ExtensionRaw& second, const ExtensionRaw& third) {
    Birth birth;
    birth.type = 8;
    birth.color = 10;
    birth.angle = .5f;
    birth.speed = 1.0f;
    birth.state.position = {10, 100, .1f};
    birth.state.velocity = {0, 0, 0};
    birth.state.flags = 0x11;
    birth.state.motionState = 1;
    birth.state.transformSound = -1;
    birth.state.extensionIndex = 1;
    birth.extensionListEmpty = false;
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
    Profile profile;
    require(profile.load(profileBytes.data(), profileBytes.size(), error), error.c_str());
    BulletVisuals visuals;
    require(visuals.load(animation.data(), animation.size(), error), error.c_str());

    const auto actual800000 = record800000();
    const auto actual256 = record256();
    const auto actual16 = record16();
    const auto initialized = initializeExtension256(actual256);
    require(initialized.has_value(), "actual 256 record must initialize");
    require(initialized->previous == 119 && initialized->remaining == 120
        && bits(initialized->accumulator) == 0x42f00000u && initialized->flags == 1
        && initialized->mode == 0, "actual 256 initialization state");
    Extension256 timer = *initialized;
    for (int tick = 1; tick <= 120; ++tick) {
        const auto step = stepExtension256(timer);
        require(step.has_value(), "type256 timer step");
        timer = step->next;
        if (tick == 1) require(timer.previous == 120 && timer.remaining == 119
            && bits(timer.accumulator) == 0x42ee0000u && !step->finished, "tick1 vector");
        if (tick == 60) require(timer.previous == 61 && timer.remaining == 60
            && bits(timer.accumulator) == 0x42700000u && !step->finished, "tick60 vector");
        if (tick == 120) require(timer.previous == 1 && timer.remaining == 0
            && bits(timer.accumulator) == 0u && step->finished, "tick120 end vector");
    }
    auto bad = actual256;
    bad.parameterB = 1;
    require(!initializeExtension256(bad).has_value(), "unknown B mode rejected");
    bad = actual256;
    bad.type = 64;
    require(!initializeExtension256(bad).has_value(), "wrong type rejected");
    bad = actual256;
    bad.parameterA = std::numeric_limits<int>::min();
    require(!initializeExtension256(bad).has_value(), "INT_MIN A rejected before decrement");
    bad = actual256;
    bad.parameterA = 65536;
    require(!initializeExtension256(bad).has_value(), "oversized A rejected");
    bad = actual256;
    bad.parameterA = 0;
    const auto zero = initializeExtension256(bad);
    require(zero.has_value(), "zero-duration initialization");
    const auto zeroStep = stepExtension256(*zero);
    require(zeroStep.has_value() && zeroStep->finished && zeroStep->next.remaining == -1,
        "zero-duration bounds");
    auto malformed = *initialized;
    malformed.remaining = std::numeric_limits<int>::min();
    require(!stepExtension256(malformed).has_value(), "INT_MIN remaining state rejected");
    malformed = *initialized;
    malformed.accumulator = std::numeric_limits<float>::quiet_NaN();
    require(!stepExtension256(malformed).has_value(), "NaN accumulator rejected");
    malformed = *initialized;
    malformed.accumulator = std::numeric_limits<float>::infinity();
    require(!stepExtension256(malformed).has_value(), "infinite accumulator rejected");
    malformed = *initialized;
    malformed.previous = std::numeric_limits<int>::min();
    require(!stepExtension256(malformed).has_value(), "INT_MIN previous rejected");
    malformed = *initialized;
    malformed.mode = 1;
    require(!stepExtension256(malformed).has_value(), "unknown state mode rejected");
    malformed = *initialized;
    malformed.accumulator = 65536.0f;
    require(!stepExtension256(malformed).has_value(), "oversized accumulator rejected");

    const auto makeVisual = [&]() {
        BulletVisuals::CreateInfo info;
        info.type = 8;
        info.color = 10;
        info.script = 43;
        info.birthInterrupt = 8;
        for (int index = 0; index < 4; ++index) {
            const auto mapped = profile.remap(10, index);
            require(mapped.has_value(), "profile remap");
            info.spriteRemap[std::size_t(index)] = *mapped;
        }
        auto visual = visuals.create(info, error);
        require(visual.has_value(), error.c_str());
        visual->primary.control().integers[0] = 1;
        return std::move(*visual);
    };

    {
        auto birth = makeBirth(actual800000, actual256, actual16);
        auto visual = makeVisual();
        bool removed = false;
        const bool applied = updateBulletWithKnownMisses(birth, visuals, visual, {1000, 1000, 0}, 0,
            removed, error, {}, {}, {}, {});
        require(applied, error.c_str());
        require(birth.extension800000.speed == 1.0f && birth.extension800000.angle == .5f,
            "800000 snapshot precedes 256");
        require((birth.state.activeExtensions & 0x100u) != 0
            && birth.extension256.previous == 120 && birth.extension256.remaining == 119,
            "256 first tick with next type16 active");
        require((birth.state.activeExtensions & 16u) != 0 && birth.extension16.active,
            "actual next type16 record initializes after 256");
    }

    {
        auto birth = makeBirth(actual800000, actual256, ExtensionRaw{});
        auto visual = makeVisual();
        bool removed = false;
        int updates = 0;
        for (; updates < 120; ++updates) {
            require(updateBulletWithKnownMisses(birth, visuals, visual, {1000, 1000, 0}, 0,
                removed, error, {}, {}, {}, {}), error.c_str());
            if (updates == 0) require(birth.extension256.remaining == 119
                && birth.state.activeExtensions == 0x100u, "first ongoing tick");
            if (updates == 59) require(birth.extension256.remaining == 60
                && birth.state.activeExtensions == 0x100u, "tick60 ongoing");
        }
        require(birth.extension256.remaining == 0 && birth.extension256.previous == 1
            && (birth.state.activeExtensions & 0x100u) == 0,
            "type256 retires at authored tick120");
        require(birth.extension800000.speed == 1.0f && birth.extension800000.angle == .5f,
            "800000 snapshot persists through 256 lifetime");
        require(birth.state.position.x == 10 && birth.state.position.y == 100,
            "type256 B0 does not mutate motion");
    }
    std::cout << "PASS actual 800000->256 chain, type16 ordering, timer lifetime and strict contexts\n";
}
