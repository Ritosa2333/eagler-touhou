#include "game/BulletVisuals.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>

using th128::bullet::BulletVisuals;
using Bytes = std::vector<uint8_t>;

void check(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

void near(float actual, float expected) {
    check(std::isfinite(actual) && std::abs(actual - expected) < .00001f,
          "float mismatch: " + std::to_string(actual) + " != " + std::to_string(expected));
}

uint32_t word(const Bytes& bytes, size_t offset) {
    return uint32_t(bytes.at(offset)) | uint32_t(bytes.at(offset + 1)) << 8 |
           uint32_t(bytes.at(offset + 2)) << 16 | uint32_t(bytes.at(offset + 3)) << 24;
}

void put16(Bytes& bytes, size_t offset, uint16_t value) {
    bytes.at(offset) = uint8_t(value);
    bytes.at(offset + 1) = uint8_t(value >> 8);
}

void put32(Bytes& bytes, size_t offset, uint32_t value) {
    for (size_t index = 0; index < 4; ++index)
        bytes.at(offset + index) = uint8_t(value >> (index * 8));
}

size_t scriptOffset(const Bytes& bytes, size_t index) {
    size_t base = 0;
    for (;;) {
        const size_t sprites = word(bytes, base + 4) & 65535;
        const size_t scripts = word(bytes, base + 4) >> 16;
        if (index < scripts)
            return base + word(bytes, base + 64 + sprites * 4 + index * 8 + 4);
        index -= scripts;
        check(word(bytes, base + 36) != 0, "script index not found");
        base += word(bytes, base + 36);
    }
}

int main(int argc, char** argv) {
    check(argc == 2, "actual bullet.anm path required");
    std::ifstream file(argv[1], std::ios::binary);
    check(bool(file), "cannot open actual bullet.anm");
    const Bytes bytes{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    BulletVisuals visuals;
    std::string error;
    BulletVisuals::CreateInfo info;
    check(!visuals.create(info, error) && !error.empty(), "unloaded create must fail");
    check(!visuals.load(nullptr, 0, error) && !error.empty(), "null load must fail");
    check(visuals.load(bytes.data(), bytes.size(), error) && error.empty(), error);
    check(!visuals.create(info, error) && !error.empty(), "explicit four-slot profile input is required");
    std::array<std::array<int32_t, 4>, 16> maps{};
    for (auto& map : maps)
        for (auto& value : map)
            check(bool(std::cin >> value), "original-byte remap vector required");

    for (int color = 0; color < 16; ++color) {
        info.color = color;
        info.spriteRemap = maps[color];
        info.birthInterrupt = 0;
        auto natural = visuals.create(info, error);
        check(bool(natural) && error.empty(), error);
        const auto& birth = natural->primary.state();
        check(birth.bank == BulletVisuals::BankId && birth.script == 43, "wrong profile script/bank");
        check(birth.ticks == 2 && birth.clock == 2 && birth.integers[0] == 0, "extra/missing birth tick or lost int0");
        check(birth.sprite == maps[color][1] && birth.visible && !birth.held, "startup sprite selection");
        check((birth.primary >> 24) == 63, "natural startup alpha after two ticks");
        near(birth.sx, 1.90625f);
        near(birth.sy, 1.90625f);
        check(!natural->hasSecondary && natural->secondary.state().ticks == 0, "absent secondary must not bind/tick");
        check(visuals.advance(*natural, 0, error), error);
        check(visuals.animationState(*natural) == &birth && birth.ticks == 2, "draw access must not advance");
        check(visuals.animationState(*natural, true) == nullptr, "absent secondary has no presentation");
        const auto* sprite = visuals.spriteInfo(birth.sprite);
        check(sprite && sprite->width > 0 && sprite->height > 0 && sprite->textureEntry >= 0, "missing sprite geometry");
        check(visuals.registry().bank(BulletVisuals::BankId) != nullptr, "registry draw access");
        check(visuals.advance(*natural, 6, error), error);
        check(birth.clock == 8 && birth.integers[0] == 0, "startup skipped authored time8 jump");
        check(visuals.advance(*natural, 1, error), error);
        check(birth.integers[0] == 1 && birth.held && birth.clock == 48 && birth.ticks == 9,
              "natural startup must hand int0=1 to bullet control");
        check(birth.sprite == maps[color][0] && (birth.primary >> 24) == 255, "steady-state sprite/alpha");
        near(birth.sx, 1.f);

        info.birthInterrupt = 8;
        auto delayed = visuals.create(info, error);
        check(bool(delayed) && error.empty(), error);
        const auto& waiting = delayed->primary.state();
        check(waiting.ticks == 2 && waiting.clock == 9 && waiting.integers[0] == 0,
              "IRQ8 must be queued after bind and consumed by post-tail tick");
        check(waiting.sprite == maps[color][1] && (waiting.primary >> 24) == 45, "IRQ8 startup state");
        near(waiting.sx, 3.f + (.5f - 3.f) * float(1. - std::pow(1. - double(1.f / 15.f), 2)));
        check(visuals.advance(*delayed, 14, error), error);
        check(waiting.clock == 23 && waiting.integers[0] == 0, "IRQ8 duration ended too early");
        check(visuals.advance(*delayed, 1, error), error);
        check(waiting.held && waiting.integers[0] == 1 && waiting.clock == 48 && waiting.ticks == 17,
              "IRQ8 branch did not reach authored control handoff");
        check(waiting.sprite == maps[color][0], "IRQ8 steady sprite remap");
        uint32_t randomCalls = 0;
        visuals.setRandomSources([&randomCalls] { ++randomCalls; return 0x80000000u; }, {});
        check(visuals.interrupt(*delayed, 1, error), error);
        check(delayed->primary.diagnostics().pendingInterrupt == 1 && waiting.ticks == 17,
              "interrupt queue must not execute a hidden tick");
        check(visuals.advance(*delayed, 1, error), error);
        check(waiting.sprite == maps[color][2] && waiting.clock == 64, "later IRQ sprite remap");
        check(randomCalls == 1, "authored IRQ1 random-angle operand must consume caller RNG exactly once");
        check(visuals.advance(*delayed, 15, error) && !waiting.ended, error);
        near(waiting.sx, 0.f);
        check(visuals.advance(*delayed, 1, error), error);
        check(waiting.ended && waiting.deleteRequested, "authored delete must survive presentation API");
        const auto endedTicks = waiting.ticks;
        check(visuals.advance(*delayed, 2, error) && waiting.ticks == endedTicks, "ended VM must not tick");
        visuals.setRandomSources({}, {});
    }

    info.color = 10;
    info.spriteRemap = maps[10];
    info.birthInterrupt = 8;
    info.secondaryScript = 43;
    auto pair = visuals.create(info, error);
    check(bool(pair) && pair->hasSecondary, error);
    check(pair->primary.state().sprite == 328 && pair->primary.state().clock == 9, "primary callback lost");
    check(pair->secondary.state().sprite == 1 && pair->secondary.state().clock == 2 &&
          pair->secondary.state().ticks == 2, "secondary must tick independently without primary remap/IRQ");
    check(visuals.animationState(*pair, true) == &pair->secondary.state(), "secondary draw access");
    check(visuals.advance(*pair, 7, error), error);
    check(pair->secondary.state().integers[0] == 1 && pair->primary.state().integers[0] == 0,
          "two embedded VMs must retain independent control integers");
    info.secondaryScript = 0;
    auto explicitZero = visuals.create(info, error);
    check(bool(explicitZero) && explicitZero->hasSecondary && explicitZero->secondary.state().script == 0,
          "optional script0 differs from absent secondary");
    info.secondaryScript.reset();
    const auto before = pair->primary.state().ticks;
    check(!visuals.advance(*pair, BulletVisuals::MaxAdvanceTicks + 1, error) && !error.empty() &&
          pair->primary.state().ticks == before, "tick budget rejection must not mutate");
    BulletVisuals foreign;
    check(!foreign.advance(*pair, 1, error) && pair->primary.state().ticks == before, "foreign visual rejected");
    check(foreign.animationState(*pair) == nullptr, "foreign presentation rejected");
    check(!visuals.load(bytes.data(), 65, error), "truncated ANM rejected");
    check(visuals.advance(*pair, 1, error), "failed reload must preserve old visual");
    check(visuals.load(bytes.data(), bytes.size(), error), error);
    check(!visuals.advance(*pair, 1, error) && !error.empty(), "successful reload invalidates old visuals");
    check(visuals.animationState(*pair) == nullptr, "stale presentation rejected");
    check(!visuals.spriteInfo(-1) && !visuals.spriteInfo(999999), "invalid sprite geometry rejected");
    BulletVisuals::Visual unbound;
    check(!visuals.advance(unbound, 1, error), "default Visual must not masquerade as bound");
    for (int color : {-1, 16}) {
        info.color = color;
        check(!visuals.create(info, error) && !error.empty(), "invalid color rejected");
    }
    info.color = 10;
    info.type = 9;
    check(!visuals.create(info, error), "unsupported type rejected");
    info.type = 8;
    info.script = -1;
    check(!visuals.create(info, error), "invalid profile script rejected");
    info.script = 43;
    info.spriteRemap[0] = -1;
    check(!visuals.create(info, error), "negative remap must not trigger inherited white-sprite fallback");
    info.spriteRemap[0] = 999999;
    check(!visuals.create(info, error), "out-of-bank remap rejected");
    info.spriteRemap = maps[10];
    info.secondaryScript = 999999;
    check(!visuals.create(info, error), "missing secondary script rejected");
    info.secondaryScript.reset();
    info.birthInterrupt = 0;

    visuals.setRandomSources([value = 0u]() mutable { value += 0x40000000u; return value; }, {});
    auto randomVisual = visuals.create(info, error);
    check(bool(randomVisual), error);
    check(visuals.interrupt(*randomVisual, 1, error) && visuals.advance(*randomVisual, 1, error), error);
    near(randomVisual->primary.state().rotation, -th128::anm::Pi / 2.f);
    check(visuals.interrupt(*randomVisual, 1, error) && visuals.advance(*randomVisual, 1, error), error);
    near(randomVisual->primary.state().rotation, 0.f);
    visuals.setRandomSources({}, {});

    const auto offset = scriptOffset(bytes, 43);
    Bytes modified = bytes;
    put16(modified, offset, 115);
    check(visuals.load(modified.data(), modified.size(), error), error);
    check(!visuals.create(info, error) && error.find("opcode=115") != std::string::npos,
          "unknown ANM opcode must fail on birth: " + error);
    modified = bytes;
    put16(modified, offset, 88);
    check(visuals.load(modified.data(), modified.size(), error), error);
    check(!visuals.create(info, error) && error.find("child") != std::string::npos,
          "unsupported child call must fail, never no-op: " + error);
    modified = bytes;
    put16(modified, offset + 36, 40);
    put32(modified, offset + 48, 1);
    check(visuals.load(modified.data(), modified.size(), error), error);
    check(!visuals.create(info, error) && error.find("RNG") != std::string::npos,
          "unprovided RNG must fail, never invent a result: " + error);
    modified = bytes;
    put32(modified, offset + 72, 4);
    check(visuals.load(modified.data(), modified.size(), error), error);
    check(!visuals.create(info, error) && error.find("sprite") != std::string::npos,
          "unknown primary remap source must fail: " + error);
    modified = bytes;
    put16(modified, offset + 136, 115);
    check(visuals.load(modified.data(), modified.size(), error), error);
    auto laterFault = visuals.create(info, error);
    check(bool(laterFault), error);
    check(!visuals.advance(*laterFault, 20, error) && laterFault->faulted() &&
          error.find("opcode=115") != std::string::npos, "late unknown instruction must fail closed");
    const auto faultTicks = laterFault->primary.state().ticks;
    const auto originalError = error;
    check(!visuals.advance(*laterFault, 1, error) && error == originalError &&
          laterFault->primary.state().ticks == faultTicks, "fault must remain sticky");
    check(visuals.animationState(*laterFault) == nullptr, "faulted visual must not render");
    check(!visuals.interrupt(*laterFault, 8, error), "faulted visual cannot be resumed by interrupt");
    std::cout << "bullet visuals actual script43 regressions passed: 16 colors\n";
}
