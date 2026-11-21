#include "ecl/EclProgram.hpp"
#include "ecl/EclVm.hpp"

#include <array>
#include <cstdio>
#include <cstring>

int main() {
    std::array<std::uint8_t, 120> bytes{};
    const auto put16 = [&](std::size_t offset, std::uint16_t value) {
        bytes[offset] = static_cast<std::uint8_t>(value);
        bytes[offset + 1] = static_cast<std::uint8_t>(value >> 8);
    };
    const auto put32 = [&](std::size_t offset, std::uint32_t value) {
        for (std::size_t index = 0; index < 4; ++index)
            bytes[offset + index] = static_cast<std::uint8_t>(value >> (index * 8));
    };
    std::memcpy(bytes.data(), "SCPT", 4);
    put16(4, 1);
    put16(6, 16);
    put32(8, 36);
    put32(16, 1);
    std::memcpy(bytes.data() + 36, "ANIM", 4);
    std::memcpy(bytes.data() + 44, "ECLI", 4);
    put32(52, 64);
    std::memcpy(bytes.data() + 56, "main", 4);
    std::memcpy(bytes.data() + 64, "ECLH", 4);
    put32(68, 16);
    put16(84, 40);
    put16(86, 20);
    bytes[90] = 255;
    bytes[91] = 1;
    put32(96, 8);
    put16(104, 402);
    put16(106, 20);
    bytes[110] = 255;
    bytes[111] = 1;
    put32(116, 32);
    th128::ecl::Program program;
    std::string error;
    if (!program.load(bytes.data(), bytes.size(), error))
        return 1;
    th128::ecl::Vm vm(program);
    program = th128::ecl::Program{};
    std::int32_t observed = 0;
    if (!vm.registerHostOpcode(402, [&](th128::ecl::HostCall& call) noexcept {
            call.readInteger(0, observed);
            return false;
        }, error) || !vm.start("main"))
        return 2;
    const auto result = vm.runTick();
    const auto& diagnostic = vm.error();
    if (result.status != th128::ecl::VmStatus::Faulted || result.instructions != 2 || observed != 32 ||
        diagnostic.code != th128::ecl::ErrorCode::UnsupportedOpcode || diagnostic.opcode != 402 ||
        diagnostic.offset != 20 || diagnostic.subroutine != "main" || vm.runTick().instructions != 0)
        return 3;
    std::puts("PASS ECL exception-disabled host contract, structured fail-stop and snapshot ownership");
    return 0;
}
