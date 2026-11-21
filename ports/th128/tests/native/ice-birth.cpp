#include "game/IceBirthRules.hpp"
#include "animation/AnmLogic.hpp"
#include <bit>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>

using namespace th128::ice;

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

float readFloat() {
    std::uint32_t bits = 0;
    std::cin >> bits;
    return std::bit_cast<float>(bits);
}

void printFloat(float value) {
    std::cout << ' ' << std::bit_cast<std::uint32_t>(value);
}

Timer readTimer() {
    Timer timer;
    std::cin >> timer.previous >> timer.current;
    timer.time = readFloat();
    std::cin >> timer.clockSource >> timer.flags;
    return timer;
}

void printTimer(const Timer& timer) {
    std::cout << ' ' << timer.previous << ' ' << timer.current;
    printFloat(timer.time);
    std::cout << ' ' << timer.clockSource << ' ' << timer.flags;
}

IceObjectState readState() {
    IceObjectState state;
    std::cin >> state.occupied;
    state.position = {readFloat(), readFloat(), readFloat()};
    state.value = readFloat();
    state.lifetime = readTimer();
    state.delay = readTimer();
    std::cin >> state.state48 >> state.mode >> state.animation50 >> state.animation54 >> state.maskSelector;
    return state;
}

void printState(const IceObjectState& state) {
    std::cout << ' ' << state.occupied;
    printFloat(state.position.x);
    printFloat(state.position.y);
    printFloat(state.position.z);
    printFloat(state.value);
    printTimer(state.lifetime);
    printTimer(state.delay);
    std::cout << ' ' << state.state48 << ' ' << state.mode << ' ' << state.animation50
              << ' ' << state.animation54 << ' ' << state.maskSelector;
}

int main(int argc, char** argv) {
    check(argc == 2, "actual Japanese ice.anm path required");
    std::ifstream file(argv[1], std::ios::binary);
    check(bool(file), "cannot open actual ice.anm");
    const std::vector<std::uint8_t> bytes{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    th128::anm::Registry registry;
    std::string error;
    check(registry.loadBank(0, bytes.data(), bytes.size(), error), error.c_str());
    check(registry.bank(0)->scripts.size() > 33, "required original ice script33 is missing");
    std::array<std::int32_t, SlotCount> occupancy{};
    IceBirthInput input;
    IceObjectState prior;
    input.position = {std::bit_cast<float>(0xa8800000u), 359.f, .1f};
    const auto tinyPositiveY = prepareIceBirth(occupancy, 0, input, prior, true);
    input.position.y = -1792;
    const auto tinyNegativeY = prepareIceBirth(occupancy, 0, input, prior, true);
    check(tinyPositiveY&&!tinyNegativeY,"original extended precision must retain the valid tiny-negative bucket and reject the actual negative bucket");
    input = {};
    for (float horizontal : {-0x1p-46f, 0x1p-46f, -0x1p-149f, 0x1p-149f}) {
        input.position = {horizontal, 359.f, .1f};
        check(bool(prepareIceBirth(occupancy, 0, input, prior, true)), "finite tiny coordinates must retain original bucket conversion");
        check(bool(planActivationIceBirth(3000, input.position)), "activation must accept the same valid finite bucket");
    }
    input = {};
    check(!selectIceSlot(std::span<const std::int32_t>{}, 0), "pool shape must be explicit");
    check(!selectIceSlot(occupancy, SlotCount), "out-of-pool cursor must reject");
    check(!prepareIceBirth(occupancy, 0, input, prior, false), "detached-list proof is mandatory");
    check(bool(prepareIceBirth(occupancy, 0, input, prior, true)), "first free birth must be usable");
    input.position.x = std::numeric_limits<float>::quiet_NaN();
    check(!prepareIceBirth(occupancy, 0, input, prior, true), "nonfinite position must reject");
    input = {};
    input.position.y = 448;
    input.position.x = 191;
    check(!prepareIceBirth(occupancy, 0, input, prior, true), "unproved bucket index must reject");
    input = {};
    occupancy.fill(1);
    const auto full = selectIceSlot(occupancy, 19);
    check(full && full->slot == 18 && full->examined == 4000 && full->nextCursor == 19 && full->overwritesOccupied,
          "native full scan selects its last occupied object, not no-slot");
    check(!prepareIceBirth(occupancy, 19, input, prior, true), "occupied reuse must not invent teardown");
    check(!planActivationIceBirth(-1, {}) && !planActivationIceBirth(10001, {}), "saved power domain is explicit");
    char operation = 0;
    while (std::cin >> operation) {
        if (operation == 'A') {
            std::int32_t power = 0;
            std::cin >> power;
            const Position position{readFloat(), readFloat(), readFloat()};
            const auto plan = planActivationIceBirth(power, position);
            check(bool(plan), "original activation-argument sample rejected");
            printFloat(plan->birth.value);
            std::cout << ' ' << plan->birth.duration << ' ' << plan->birth.mode
                      << ' ' << plan->nativeFrozenPower << ' ' << plan->builderCallSite;
            printFloat(plan->birth.position.x);
            printFloat(plan->birth.position.y);
            printFloat(plan->birth.position.z);
            for (const auto& call : plan->requiredCalls)
                std::cout << ' ' << int(call.kind) << ' ' << call.callSite << ' ' << call.target
                          << ' ' << call.nativeLayer << ' ' << call.nativeResourceSlot
                          << ' ' << call.script << ' ' << call.resourceManagerOffset
                          << ' ' << call.sound << ' ' << call.resultActivationOffset
                          << ' ' << call.managerActiveFlagOffset;
            for (const auto& call : plan->requiredCalls) {
                printFloat(call.position.x);
                printFloat(call.position.y);
                printFloat(call.position.z);
            }
            std::cout << '\n';
            continue;
        }
        check(operation == 'B' || operation == 'S' || operation == 'U', "unknown ice golden operation");
        std::uint32_t cursor = 0, occupiedRun = 0;
        std::cin >> cursor >> occupiedRun;
        occupancy.fill(0);
        for (std::uint32_t index = 0; index < occupiedRun; ++index)
            occupancy[(cursor + index) % SlotCount] = 1;
        const auto selection = selectIceSlot(occupancy, cursor);
        check(bool(selection), "native slot sample rejected");
        std::cout << selection->slot << ' ' << selection->nextCursor << ' ' << selection->examined
                  << ' ' << selection->overwritesOccupied;
        if (operation == 'B' || operation == 'U') {
            input.position = {readFloat(), readFloat(), readFloat()};
            input.value = readFloat();
            std::cin >> input.duration >> input.mode;
            prior = readState();
            const auto plan = prepareIceBirth(occupancy, cursor, input, prior, true);
            if (operation == 'U') {
                check(!plan, "precision-sensitive or out-of-table native vector must be explicitly unsupported");
                std::cout << " unsupported\n";
                continue;
            }
            check(bool(plan), "original returned ice builder sample rejected");
            printState(plan->next);
            std::cout << ' ' << int(plan->continuation);
            for (const auto& insertion : plan->requiredInsertions)
                std::cout << ' ' << insertion.managerHeadOffset << ' ' << insertion.objectNodeOffset;
        }
        check(bool(std::cin), "truncated ice golden input");
        std::cout << '\n';
    }
}
