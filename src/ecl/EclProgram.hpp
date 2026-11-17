#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

namespace th128::ecl {

struct Instruction {
    std::uint32_t offset = 0;
    std::int32_t time = 0;
    std::uint16_t opcode = 0;
    std::uint16_t size = 16;
    std::uint16_t parameterMask = 0;
    std::uint8_t rankMask = 0;
    std::uint8_t parameterCount = 0;
    std::uint32_t stackOffset = 0;
    std::vector<std::uint32_t> rawWords;
};

struct Subroutine {
    std::string name;
    std::uint32_t fileOffset = 0;
    std::uint32_t codeSize = 0;
    std::vector<Instruction> instructions;

    const Instruction* instructionAt(std::uint32_t offset) const;
};

struct Resource {
    std::vector<std::uint8_t> bytes;
    std::vector<std::string> animationIncludes;
    std::vector<std::string> eclIncludes;
};

class Program {
public:
    static constexpr std::size_t maxBytes = 32u * 1024u * 1024u;
    static constexpr std::size_t maxSubroutines = 4096;
    static constexpr std::size_t maxInstructions = 200000;
    static constexpr std::size_t maxIncludes = 4096;

    // Absolute instruction offsets in one unlinked original SCPT resource.
    struct PracticeBranch {
        std::uint32_t sourceOffset = 0;
        std::uint32_t targetOffset = 0;
        std::int32_t eclTime = 0;
        std::int32_t targetTime = 0;
    };
    bool withPracticeBranches(const std::vector<std::uint8_t>& expectedSource,
        const std::vector<PracticeBranch>& branches, Program& output, std::string& error) const;
    bool load(const void* bytes, std::size_t size, std::string& error);
    bool loaded() const;
    const Resource& resource() const;
    const std::vector<Subroutine>& subroutines() const;
    const Subroutine* find(std::string_view name) const;
    bool linkIncludes(const std::vector<std::pair<std::string,Program>>& providers, std::string& error);
    std::size_t sourceCount() const;
    std::size_t sourceBytes() const;

private:
    struct Data;
    std::shared_ptr<const Data> data_;
};

}
