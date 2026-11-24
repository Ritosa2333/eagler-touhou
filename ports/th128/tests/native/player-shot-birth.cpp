#include "game/PlayerShotBirthRules.hpp"
#include <bit>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>

using namespace th128::playershot;

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

float readFloat() {
    std::uint32_t value = 0;
    std::cin >> value;
    return std::bit_cast<float>(value);
}

void printImage(const FirstShotBirthPlan& plan) {
    auto image = plan.slotImage;
    for (std::uint32_t index = 0; index < 4; ++index)
        image[0x74 + index] = std::uint8_t(plan.sourceRecordOffset >> (8 * index));
    constexpr char digits[] = "0123456789abcdef";
    for (auto byte : image)
        std::cout << digits[byte >> 4] << digits[byte & 15];
}

std::vector<std::uint8_t> readImage() {
    std::string text;
    std::cin >> text;
    check(text.size() == ShotSlotBytes * 2, "exact previous shot image is required");
    std::vector<std::uint8_t> result(ShotSlotBytes);
    for (std::size_t index = 0; index < result.size(); ++index)
        result[index] = std::uint8_t(std::stoul(text.substr(index * 2, 2), nullptr, 16));
    return result;
}

int main(int argc, char** argv) {
    check(argc == 2, "actual pl00a.sht path required");
    std::ifstream file(argv[1], std::ios::binary);
    check(bool(file), "cannot read actual shot resource");
    const std::vector<std::uint8_t> bytes{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    ShtBank bank;
    std::string error;
    check(!bank.load({}, error) && !error.empty(), "empty required resource must reject");
    check(bank.load(bytes, error) && error.empty(), error.c_str());
    check(bank.groupCount() == 16 && bank.recordCount() == 114, "actual group/record counts");
    std::array<std::int32_t, ShotSlotCount> occupancy{};
    ShotBirthInput input;
    check(!planFirstShotBirth(bank, 0, 0, occupancy, input), "unproved zero-state snapshot must reject");
    input.zeroUnmodeledStateConfirmed = true;
    auto put32 = [](std::vector<std::uint8_t>& target, std::size_t offset, std::uint32_t value) {
        for (std::size_t index = 0; index < 4; ++index)
            target[offset + index] = std::uint8_t(value >> (8 * index));
    };
    std::vector<std::uint8_t> retired(ShotSlotBytes);
    put32(retired, 0x44, 1);
    check(!planRecycledShotBirth(bank, 0, 0, occupancy, input, retired), "angular retained state cannot become ordinary polar motion");
    put32(retired, 0x44, 0);
    put32(retired, 0x48, 1);
    check(!planRecycledShotBirth(bank, 0, 0, occupancy, input, retired), "active slot cannot be recycled");
    check(!planRecycledShotBirth(bank, 0, 0, occupancy, input, std::span<const std::uint8_t>(retired).first(12)), "truncated previous slot rejects");
    for (int mode = 0; mode < 5; ++mode) {
        auto malformed = bytes;
        if (mode == 0)
            malformed[0] = 4;
        if (mode == 1) {
            malformed[2] = 1;
            malformed[3] = 1;
        }
        if (mode == 2)
            put32(malformed, 0x268, 4);
        if (mode == 3)
            malformed[bank.group(0)->fileOffset] = 0;
        if (mode == 4)
            malformed.resize(0x270);
        check(!bank.load(malformed, error) && !error.empty(), "malformed SHT region/tag/count/period must reject");
    }
    {
        auto imported = bytes;
        ShtBank copy;
        check(copy.load(imported, error), "copy-import required resource");
        std::fill(imported.begin(), imported.end(), 0);
        check(copy.groupCount() == 16 && copy.group(0)->records[0].raw == bank.group(0)->records[0].raw,
              "bank must own records instead of borrowing caller bytes");
    }
    {
        auto unknown = bytes;
        put32(unknown, bank.group(0)->records[0].fileOffset + 0x24, 0xffffffffu);
        ShtBank copy;
        check(copy.load(unknown, error), "unknown callback IDs are raw data, not live calls");
        const auto pending = planFirstShotBirth(copy, 0, 0, occupancy, input);
        check(pending && pending->requiredCallbacks[0].fileId == 0xffffffffu && pending->requiredCallbacks[0].tableIdentity == 0x4b1cd8,
              "unknown callback ID must remain an explicit runtime-resolution request");
    }
    {
        auto nonfinite = bytes;
        put32(nonfinite, bank.group(0)->records[0].fileOffset + 0x14, 0x7fc00000u);
        ShtBank copy;
        check(copy.load(nonfinite, error), "structural reader must preserve opaque payload bytes");
        check(!planFirstShotBirth(copy, 0, 0, occupancy, input), "nonfinite used payload must reject at execution preflight");
    }
    {
        std::optional<FirstShotBirthPlan> orphan;
        {
            ShtBank owner;
            check(owner.load(bytes, error), "owner bank load");
            orphan = planFirstShotBirth(owner, 0, 0, occupancy, input);
        }
        check(orphan && !bank.owns(*orphan) && !completeFirstShotMotion(bank, *orphan, {}),
              "destroyed/foreign owner cannot authorize a pending motion completion");
    }
    const auto missingGroupInputs = planShotGroup(bank, std::nullopt, std::nullopt);
    check(missingGroupInputs && missingGroupInputs->continuation == ShotGroupContinuation::NeedsRuntimeInputs &&
          !missingGroupInputs->group, "missing native area/difficulty must not default to group0");
    check(missingGroupInputs->requiredAreaIdentity == 0x4b4d60 && missingGroupInputs->requiredDifficultyIdentity == 0x4b4d0c &&
          missingGroupInputs->requiredPlayerShtOffset == 0xf7c, "group inputs name original state/resource obligations");
    const auto measuredGroup = planShotGroup(bank, 0.f, 0);
    check(measuredGroup && measuredGroup->group == 2 && measuredGroup->groupFileOffset == bank.group(2)->fileOffset,
          "explicit actual group inputs select the real SHT group");
    auto initial = planFirstShotBirth(bank, 0, 0, occupancy, input);
    check(bool(initial), "actual first record must produce a bounded plan");
    check(!bank.load(std::span<const std::uint8_t>(bytes).first(12), error) && bank.owns(*initial), "failed reload preserves current generation");
    check(bank.load(bytes, error) && !bank.owns(*initial), "successful reload invalidates pending old record plans");
    check(!completeFirstShotMotion(bank, *initial, {}), "stale plan cannot resume");
    input.position.x = std::numeric_limits<float>::infinity();
    check(!planFirstShotBirth(bank, 0, 0, occupancy, input), "nonfinite position must reject");
    check(!selectShotGroup(-1, 0) && !selectShotGroup(0, 4), "explicit group domain");
    input = {};
    input.zeroUnmodeledStateConfirmed = true;
    char operation = 0;
    while (std::cin >> operation) {
        if (operation == 'D') {
            const auto area = readFloat();
            std::int32_t difficulty = 0, count = 0;
            std::cin >> difficulty >> count;
            const auto selected = planShotGroup(bank, area, difficulty);
            check(selected && selected->group, "actual scheduler group input rejected");
            std::int32_t offset = -1;
            for (const auto& record : bank.group(*selected->group)->records) {
                const auto due = shotRecordDue(record, count);
                check(bool(due), "actual record period/phase rejected");
                if (*due) {
                    offset = std::int32_t(record.fileOffset);
                    break;
                }
            }
            std::cout << offset << '\n';
            continue;
        }
        if (operation == 'G') {
            const auto area = readFloat();
            std::int32_t difficulty = 0;
            std::cin >> difficulty;
            const auto group = selectShotGroup(area, difficulty);
            check(bool(group), "native group case rejected");
            std::cout << *group << '\n';
            continue;
        }
        if (operation == 'R') {
            std::uint32_t group = 0, record = 0;
            std::cin >> group >> record;
            const auto* selected = bank.group(group);
            check(selected && record < selected->records.size(), "actual record index rejected");
            const auto& value = selected->records[record];
            constexpr char digits[] = "0123456789abcdef";
            std::cout << value.fileOffset << ' ' << selected->unknownWord << ' ';
            for (auto byte : value.raw)
                std::cout << digits[byte >> 4] << digits[byte & 15];
            std::cout << '\n';
            continue;
        }
        check(operation == 'B' || operation == 'M' || operation == 'C' || operation == 'N', "unknown first-shot golden operation");
        std::uint32_t group = 0, record = 0, occupiedRun = 0;
        std::cin >> group >> record >> occupiedRun >> input.timerFlags >> input.shotCount;
        input.timerClockIdentity = 0xabcdef01u;
        input.position = {readFloat(), readFloat(), readFloat()};
        occupancy.fill(0);
        for (std::uint32_t index = 0; index < occupiedRun; ++index)
            occupancy[index] = 1;
        auto plan = operation == 'C' || operation == 'N'
            ? planRecycledShotBirth(bank, group, record, occupancy, input, readImage())
            : planFirstShotBirth(bank, group, record, occupancy, input);
        check(bool(plan), "native first-shot prefix rejected");
        if (operation == 'M' || operation == 'N') {
            ShotMotionResult motion;
            motion.velocity = {readFloat(), readFloat(), readFloat()};
            motion.normalizedAngle = readFloat();
            plan = completeFirstShotMotion(bank, *plan, motion);
            check(bool(plan), "genuine sampled native motion result rejected");
        }
        std::cout << int(plan->continuation) << ' ' << plan->slot << ' ' << plan->examined;
        if (plan->continuation != ShotBirthContinuation::PoolFull) {
            std::cout << ' ';
            printImage(*plan);
            std::cout << ' ' << plan->animation.script << ' ' << plan->animation.layer << ' ' << plan->animation.playerResourceOffset
                      << ' ' << plan->sound << ' ' << std::bit_cast<std::uint32_t>(plan->motion.angle)
                      << ' ' << std::bit_cast<std::uint32_t>(plan->motion.speed);
            std::cout << ' ' << plan->animation.allocationSite << ' ' << plan->animation.allocationTarget
                      << ' ' << plan->animation.bindSite << ' ' << plan->animation.bindTarget
                      << ' ' << plan->animation.registrationSite << ' ' << plan->animation.registrationTarget
                      << ' ' << plan->animation.preAllocationCounterOffset
                      << ' ' << plan->animation.nativeDebugFlagsIdentity << ' ' << plan->animation.nativeLockFlagMask;
            for (const auto target : plan->motion.targets)
                std::cout << ' ' << target;
            std::cout << ' ' << plan->motion.clockIdentity << ' ' << std::bit_cast<std::uint32_t>(plan->motion.clock);
            for (const auto& callback : plan->requiredCallbacks)
                std::cout << ' ' << callback.recordOffset << ' ' << callback.tableIdentity << ' ' << callback.fileId;
        }
        std::cout << '\n';
    }
}
