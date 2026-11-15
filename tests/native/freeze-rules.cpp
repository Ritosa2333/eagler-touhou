#include "game/FreezeRules.hpp"

#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <optional>
#include <string>

namespace rules = th128::freeze;

namespace {
std::size_t checks = 0;

void expect(bool condition, const char* name) {
    ++checks;
    if (!condition) {
        std::fprintf(stderr, "freeze scalar check failed: %s\n", name);
        std::exit(1);
    }
}

void fixedVectors() {
    expect(rules::ReleasePowerRaw == 3000, "release raw units");
    expect(rules::ReleaseHoldCount == 45, "hold raw count");
    expect(rules::PowerCapRaw == 10000, "ordinary upper cap");
    expect(rules::PerfectFreezeCostRaw == 10000, "Perfect Freeze raw cost");
    expect(rules::PerfectFreezeCapRaw == 30000, "Perfect Freeze upper cap");
    expect(rules::BaselineIncrementRaw == 5, "continuation raw increment");
    expect(rules::canRelease(3000, 45, 0), "inclusive release thresholds");
    expect(!rules::canRelease(2999, 45, 0), "below release power");
    expect(!rules::canRelease(3000, 44, 0), "below release count");
    expect(!rules::canRelease(10000, 45, 1), "nonzero Perfect Freeze state");
    expect(!rules::canRelease(3000, 45, -1), "negative nonzero state host gate");
    expect(rules::durationCount(3000) == 12, "duration-3000-12");
    expect(rules::durationCount(10000) == 100, "duration-10000-100");
    expect(rules::durationCount(3039) == 12, "duration-3039-12");
    expect(rules::durationCount(3040) == 13, "duration-3040-12");
    expect(rules::durationCount(2999) == 12, "duration truncation");
    expect(rules::durationCount(0) == -25, "duration-0-0");
    expect(rules::durationPending(3000, 11), "before exact deadline");
    expect(!rules::durationPending(3000, 12), "equality expires");
    expect(rules::addPowerRaw(0, -1) == -1, "power-add-0--1 returned without lower clamp");
    expect(rules::addPowerRaw(10000, 50) == 10000, "power-add-10000-50 returned upper cap");
    expect(rules::addPowerRaw(2999, 1) == 3000, "power-add-2999-1 pre-call store only");
    expect(rules::baselineRechargeRaw(0) == 5, "baseline-0 boundary store");
    expect(rules::baselineRechargeRaw(10000) == 10000, "baseline-10000 cap");
    expect(rules::activationStoreRaw() == 0, "activation-zero-3000 boundary store");
    expect(!rules::heldRechargeRaw(19), "held-19 no selected call");
    expect(rules::heldRechargeRaw(20) == 5, "held-20 pending argument");
    expect(rules::heldRechargeRaw(45) == 5, "held-45 pending argument");
    expect(rules::heldRechargeRaw(46) == 10, "held-46 pending argument");
    expect(rules::heldRechargeRaw(80) == 10, "held-80 pending argument");
    expect(rules::heldRechargeRaw(81) == 12, "held-81 pending argument");
    expect(rules::heldRechargeRaw(110) == 12, "held-110 pending argument");
    expect(rules::heldRechargeRaw(111) == 13, "held-111 pending argument");
    expect(rules::heldRechargeRaw(140) == 13, "held-140 pending argument");
    expect(rules::heldRechargeRaw(141) == 15, "held-141 pending argument");
    expect(rules::addPerfectFreezeRaw(0, -1) == 0, "perfect-add-0--1 returned");
    expect(rules::addPerfectFreezeRaw(30000, 1) == 30000, "perfect-add-30000-1 returned");
    expect(rules::addPerfectFreezeRaw(30000, -10000) == 20000, "perfect-add-30000--10000 returned");
    expect(rules::spendPerfectFreezeRaw(10000) == 0, "exactly one stock deduction");
    expect(rules::spendPerfectFreezeRaw(30000) == 20000, "three-stock deduction");
    expect(!rules::spendPerfectFreezeRaw(9999), "unaffordable stock host guard");
    expect(!rules::spendPerfectFreezeRaw(30001), "out-of-domain stock host guard");
    expect(rules::areaRewardRaw(0.0f, 10000) == 0, "zero reward differs from no reward path");
    expect(rules::areaRewardRaw(10.0f, 10000) == 500, "area-gain-10 pending argument");
    expect(rules::areaRewardRaw(20.0f, 10000) == 750, "area-gain-20 pending argument");
    expect(rules::areaRewardRaw(30.0f, 10000) == 1000, "area-gain-30 pending argument");
    expect(rules::areaRewardRaw(std::bit_cast<float>(0x41efffffu), 10000) == 999,
           "float32 predecessor of 30 pending argument");
    expect(rules::areaRewardRaw(std::bit_cast<float>(0x41f00001u), 10000) == 2000,
           "float32 successor of 30 discontinuity");
    expect(!rules::areaRewardRaw(30.0f, 10001), "area-gain-bypass-10001 boundary");
    expect(!rules::areaRewardRaw(-1.0f, 10000), "negative area host guard");
    expect(!rules::areaRewardRaw(101.0f, 10000), "out-of-domain area host guard");
    expect(!rules::areaRewardRaw(std::numeric_limits<float>::infinity(), 10000), "infinite area host guard");
    expect(!rules::areaRewardRaw(std::numeric_limits<float>::quiet_NaN(), 10000), "NaN area host guard");
    expect(!rules::areaRewardRaw(10.0f, -1), "negative saved power host guard");
    std::printf("freeze scalar fixed vectors passed: %zu\n", checks);
}
}

int main() {
    fixedVectors();
    std::string identifier;
    std::string operation;
    std::size_t queries = 0;
    while (std::cin >> identifier >> operation) {
        std::optional<std::int32_t> result;
        std::int32_t first = 0;
        std::int32_t second = 0;
        std::int32_t third = 0;
        if (operation == "release") {
            std::cin >> first >> second >> third;
            result = rules::canRelease(first, second, third);
        } else if (operation == "duration") {
            std::cin >> first;
            result = rules::durationCount(first);
        } else if (operation == "pending") {
            std::cin >> first >> second;
            result = rules::durationPending(first, second);
        } else if (operation == "power") {
            std::cin >> first >> second;
            result = rules::addPowerRaw(first, second);
        } else if (operation == "baseline") {
            std::cin >> first;
            result = rules::baselineRechargeRaw(first);
        } else if (operation == "held") {
            std::cin >> first;
            result = rules::heldRechargeRaw(first);
        } else if (operation == "activation-store") {
            result = rules::activationStoreRaw();
        } else if (operation == "perfect") {
            std::cin >> first >> second;
            result = rules::addPerfectFreezeRaw(first, second);
        } else if (operation == "spend") {
            std::cin >> first;
            result = rules::spendPerfectFreezeRaw(first);
        } else if (operation == "area") {
            std::uint32_t bits = 0;
            std::cin >> bits >> second;
            result = rules::areaRewardRaw(std::bit_cast<float>(bits), second);
        } else {
            std::fprintf(stderr, "unknown freeze scalar operation: %s\n", operation.c_str());
            return 2;
        }
        if (!std::cin) {
            std::fprintf(stderr, "malformed freeze query: %s\n", identifier.c_str());
            return 2;
        }
        std::cout << identifier << ' ' << (result ? std::to_string(*result) : "none") << '\n';
        ++queries;
    }
    if (!std::cin.eof()) {
        return 2;
    }
    std::printf("freeze evidence query count: %zu\n", queries);
}
