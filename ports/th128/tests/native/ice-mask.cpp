#include "game/IceMaskRules.hpp"
#include <array>
#include <bit>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

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

std::vector<std::uint8_t> readBytes() {
    std::string encoded;
    std::cin >> encoded;
    if (encoded == "-")
        return {};
    check(encoded.size() % 2 == 0, "odd native-golden hexadecimal input");
    auto digit = [](char value) {
        if (value >= '0' && value <= '9')
            return value - '0';
        check(value >= 'a' && value <= 'f', "invalid native-golden hexadecimal input");
        return value - 'a' + 10;
    };
    std::vector<std::uint8_t> bytes(encoded.size() / 2);
    for (std::size_t offset = 0; offset < bytes.size(); ++offset)
        bytes[offset] = std::uint8_t(digit(encoded[offset * 2]) * 16 + digit(encoded[offset * 2 + 1]));
    return bytes;
}

void printBytes(std::span<const std::uint8_t> bytes) {
    constexpr char digits[] = "0123456789abcdef";
    for (const auto value : bytes)
        std::cout << digits[value >> 4] << digits[value & 15];
}

void printFloat(float value) {
    std::cout << ' ' << std::bit_cast<std::uint32_t>(value);
}

int main() {
    std::array<std::uint8_t, IceMaskBytes> mask{};
    const std::array<std::uint8_t, 4> bitmap{255, 255, 255, 255};
    const IceBitmapView source{bitmap, 1, 1};
    check(countIceMask(mask) == 0, "all-zero native plane count");
    mask.fill(255);
    check(countIceMask(mask) == 172032, "complete bit counter must cover all448 rows");
    check(!countIceMask(std::span<const std::uint8_t>(mask).first(IceMaskBytes - 1)), "short plane must reject");
    std::vector<std::uint8_t> oversized(IceMaskBytes + 1);
    check(!countIceMask(oversized), "oversized complete mask must reject");
    check(!updateIceMask(oversized, source, {}, IceMaskOperation::Merge), "oversized writable mask must reject");
    for(std::size_t index=0;index<mask.size();++index)mask[index]=index%2?0x5a:0xa5;
    for(const auto operation:{IceMaskOperation::Merge,IceMaskOperation::Subtract}){
        const auto mixedBefore=mask;
        check(!updateIceMask(std::span<std::uint8_t>(mask).first(IceMaskBytes-1),source,{},operation),"mixed short plane must reject");
        check(mask==mixedBefore,"rejected short plane cannot perform partial OR/ANDNOT writes");
        check(!updateIceMask(mask,{std::span<const std::uint8_t>(bitmap).first(3),1,1},{},operation),"mixed short source must reject");
        check(mask==mixedBefore,"rejected short source cannot perform partial OR/ANDNOT writes");
        auto tinyMask=mask;
        check(bool(updateIceMask(tinyMask,source,{0x1p-46f,0,0},operation)),"finite tiny coordinate must execute native raster placement");
        check(!updateIceMask(mask,{std::span<const std::uint8_t>(mask).first(4),1,1},{1,0,0},operation),"mixed alias must reject");
        check(mask==mixedBefore,"rejected alias cannot mutate either direction");
    }
    auto before = mask;
    check(!updateIceMask(std::span<std::uint8_t>(mask).first(IceMaskBytes - 1), source, {}, IceMaskOperation::Merge), "short writable plane must reject");
    check(!updateIceMask(mask, {std::span<const std::uint8_t>(bitmap).first(3), 1, 1}, {}, IceMaskOperation::Merge), "short source must reject");
    check(!updateIceMask(mask, {bitmap, 449, 1}, {}, IceMaskOperation::Merge), "height domain must reject");
    check(!updateIceMask(mask, {bitmap, 1, 13}, {}, IceMaskOperation::Merge), "word-width domain must reject");
    check(!updateIceMask(mask, {bitmap, 448, 12}, {}, IceMaskOperation::Merge), "source budget must reject");
    check(!updateIceMask(mask, source, {}, static_cast<IceMaskOperation>(99)), "unknown mask operation is unsupported");
    check(!updateIceMask(mask, {std::span<const std::uint8_t>(mask).first(4), 1, 1}, {}, IceMaskOperation::Merge), "source cannot alias writable mask");
    for (const float value : {0x1p-32f, -0x1p-32f, 0x1p-46f, -0x1p-46f}) {
        auto tinyMask=mask;
        check(bool(updateIceMask(tinyMask, source, {value, 0, 0}, IceMaskOperation::Merge)), "tiny X must execute original raster");
        check(bool(updateIceMask(tinyMask, source, {0, value, 0}, IceMaskOperation::Merge)), "tiny Y must execute original raster");
        check(bool(planIceMaskAttachment(4, {value, value, 0})), "tiny position must attach original large ice mask");
    }
    check(!updateIceMask(mask, source, {std::numeric_limits<float>::infinity(), 0, 0}, IceMaskOperation::Merge), "nonfinite X must reject");
    check(mask == before, "every rejected update must be mutation-free");
    check(selectIceMask(0).disposition == IceMaskDisposition::Absent, "selector0 is genuinely absent");
    check(selectIceMask(6).disposition == IceMaskDisposition::Unsupported, "selector6 cannot be a fake absent mask");
    check(selectIceMask(0xffffffffu).disposition == IceMaskDisposition::Unsupported, "unknown selector cannot select arbitrary native addresses");
    check(!planIceMaskAttachment(0, {}) && !planIceMaskAttachment(6, {}), "only known mask attachments have a plan");
    mask.fill(0);
    check(bool(updateIceMask(mask, source, {-176, .5f, 0}, IceMaskOperation::Merge)), "first bitmap merge");
    check(bool(updateIceMask(mask, source, {-176, .5f, 0}, IceMaskOperation::Merge)), "overlapping bitmap merge");
    check(bool(updateIceMask(mask, source, {-176, .5f, 0}, IceMaskOperation::Subtract)), "native AND-NOT removal");
    check(countIceMask(mask) == 0, "native removal is AND-NOT, not a reference-counted union");
    char operation = 0;
    while (std::cin >> operation) {
        if (operation == 'C') {
            const auto bytes = readBytes();
            const auto count = countIceMask(bytes);
            check(bool(count), "native complete counter vector rejected");
            std::cout << *count << '\n';
            continue;
        }
        if (operation == 'A') {
            std::uint32_t selector = 0;
            std::cin >> selector;
            const IceMaskPosition position{readFloat(), readFloat(), readFloat()};
            const auto plan = planIceMaskAttachment(selector, position);
            check(bool(plan), "native attachment plan rejected");
            std::cout << plan->profile.selector << ' ' << plan->profile.height << ' ' << plan->profile.wordsPerRow
                      << ' ' << int(plan->continuation) << ' ' << plan->effectCallSite << ' ' << plan->effectTarget
                      << ' ' << plan->layer << ' ' << plan->script << ' ' << plan->nativeResourceSlot
                      << ' ' << plan->resourceManagerOffset << ' ' << plan->selectorStoreOffset << ' ' << plan->handleStoreOffset;
            printFloat(plan->position.x);
            printFloat(plan->position.y);
            printFloat(plan->position.z);
            std::cout << '\n';
            continue;
        }
        if (operation == 'D') {
            std::uint32_t selector = 0;
            std::cin >> selector;
            const IceMaskPosition position{readFloat(), readFloat(), 0};
            auto destination = readBytes();
            const auto bytes = readBytes();
            const auto selection = selectIceMask(selector);
            if (selection.disposition == IceMaskDisposition::Known)
                check(bool(updateIceMask(destination, {bytes, selection.height, selection.wordsPerRow}, position,
                                         IceMaskOperation::Subtract)), "native selected removal vector rejected");
            std::cout << int(selection.disposition) << ' ';
            printBytes(destination);
            std::cout << '\n';
            continue;
        }
        check(operation == 'R', "unknown native mask golden operation");
        int kind = 0;
        std::cin >> kind;
        const IceMaskPosition position{readFloat(), readFloat(), 0};
        std::uint32_t height = 0, width = 0;
        std::cin >> height >> width;
        auto destination = readBytes();
        const auto bytes = readBytes();
        const auto result = updateIceMask(destination, {bytes, height, width}, position, static_cast<IceMaskOperation>(kind));
        check(bool(result), "native returned raster vector rejected");
        std::cout << result->startRow << ' ' << result->startWord << ' ' << result->signedRemainder
                  << ' ' << result->wordWrites << ' ' << result->changedBits << ' ';
        printBytes(destination);
        std::cout << '\n';
    }
    check(std::cin.eof(), "truncated native mask input");
}
