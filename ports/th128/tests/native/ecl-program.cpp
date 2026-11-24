#include "ecl/EclProgram.hpp"
#include "../ecl-fixture.hpp"

#include <cstdio>
#include <fstream>
#include <iterator>

using namespace ecl_fixture;
using th128::ecl::Program;

void readerTests() {
    const auto body = join({instruction(42, {0x89abcdef}, -9, 1, 4, 1, 12),
                            instruction(65535, {1, 2, 3}, 37, 3, 255, 2), instruction(10)});
    auto bytes = program({{"main", body}, {"other", instruction(10)}}, {"enemy.anm"}, {"default.ecl"});
    Program loaded;
    std::string error = "old diagnostic";
    require(loaded.load(bytes.data(), bytes.size(), error), "valid reader fixture: " + error);
    require(error.empty(), "successful load clears diagnostics");
    require(loaded.loaded() && loaded.subroutines().size() == 2, "subroutine table");
    require(loaded.resource().bytes == bytes, "whole resource retained");
    require(loaded.resource().animationIncludes == std::vector<std::string>{"enemy.anm"}, "ANIM names");
    require(loaded.resource().eclIncludes == std::vector<std::string>{"default.ecl"}, "ECLI names");
    const auto* main = loaded.find("main");
    require(main && !loaded.find("missing"), "name lookup");
    const auto& first = main->instructions.at(0);
    require(first.time == -9 && first.opcode == 42 && first.size == 20 && first.offset == 0 &&
            first.parameterMask == 1 && first.rankMask == 4 && first.parameterCount == 1 &&
            first.stackOffset == 12 && first.rawWords == std::vector<std::uint32_t>{0x89abcdef},
            "lossless instruction metadata");
    require(main->instructionAt(20) == &main->instructions[1] && !main->instructionAt(4) &&
            !main->instructionAt(main->codeSize), "exact instruction boundaries only");
    const auto originalOffset = main->fileOffset;
    bytes.assign(bytes.size(), 0);
    require(loaded.find("main")->instructions[0].rawWords[0] == 0x89abcdef, "input ownership");

    const auto valid = loaded.resource().bytes;
    const std::size_t table = 36 + (std::uint16_t(valid[6]) | (std::uint16_t(valid[7]) << 8));
    const auto reject = [&](const Bytes& corrupt, const std::string& label) {
        error.clear();
        require(!loaded.load(corrupt.data(), corrupt.size(), error), "accepted corruption: " + label);
        require(!error.empty(), "missing diagnostic: " + label);
        require(loaded.find("main") && loaded.find("main")->fileOffset == originalOffset &&
                loaded.resource().bytes == valid, "nontransactional rejection: " + label);
    };
    for (std::size_t length = 0; length < valid.size(); ++length) {
        if (length != get32(valid, table + 4) + 16)
            reject(Bytes(valid.begin(), valid.begin() + length), "truncation " + std::to_string(length));
    }
    auto corrupt = valid;
    corrupt[0] = 0;
    reject(corrupt, "magic");
    corrupt = valid;
    put32(corrupt, 8, 40);
    reject(corrupt, "include offset");
    corrupt = valid;
    put32(corrupt, 16, 4097);
    reject(corrupt, "subroutine budget");
    corrupt = valid;
    put32(corrupt, table + 4, get32(valid, table));
    reject(corrupt, "duplicate offsets");
    corrupt = valid;
    put32(corrupt, table, 36);
    reject(corrupt, "offset into includes");
    corrupt = valid;
    put32(corrupt, table, 0xfffffff0);
    reject(corrupt, "overflowing offset");
    corrupt = valid;
    put32(corrupt, originalOffset + 4, 20);
    reject(corrupt, "unverified subroutine data offset");
    corrupt = valid;
    put16(corrupt, originalOffset + 16 + 6, 12);
    reject(corrupt, "undersized instruction");
    corrupt = valid;
    put16(corrupt, originalOffset + 16 + 6, 18);
    reject(corrupt, "unaligned instruction");
    corrupt = valid;
    put16(corrupt, originalOffset + 16 + 6, 65532);
    reject(corrupt, "oversized instruction");
    reject(program({{"same", instruction(10)}, {"same", instruction(10)}}), "duplicate names");
    reject(program({{"main", instruction(10)}}, {"same.anm", "SAME.anm"}), "case-aliased include");
    reject(program({{"main", instruction(10)}}, {}, {"same.ecl", "same.ecl"}), "duplicate include");
    reject(program({{"main", instruction(10)}}, {"../enemy.anm"}), "path include");
    reject(program({{"main", instruction(10)}}, {"CON.anm"}), "device include");
    reject(program({{"main", instruction(10)}}, {"enemy..anm"}), "dot alias include");
    reject(program({{"main", instruction(10)}}, {"enemy.anm."}), "trailing dot include");
    error.clear();
    require(!loaded.load(nullptr, 40, error) && !error.empty(), "null input");
    require(!loaded.load(valid.data(), 32u * 1024u * 1024u + 1, error), "size cap before read");

    auto swapped = valid;
    put32(swapped, table, get32(valid, table + 4));
    put32(swapped, table + 4, get32(valid, table));
    require(loaded.load(swapped.data(), swapped.size(), error), "unordered nonoverlapping subroutines");
    require(loaded.find("other")->instructions.size() == 3, "offset/name pairing retained");
    std::puts("PASS ECL reader synthetic ownership, metadata, boundaries and corruption fixtures");

    corrupt = valid;
    std::memcpy(corrupt.data() + 36, "NOPE", 4);
    Program rejected;
    require(!rejected.load(corrupt.data(), corrupt.size(), error), "unknown include section");
    corrupt = valid;
    const auto firstSectionEnd = 36 + 8 + 12;
    std::memcpy(corrupt.data() + firstSectionEnd, "ANIM", 4);
    require(!rejected.load(corrupt.data(), corrupt.size(), error), "duplicate include section");
    corrupt = valid;
    std::fill(corrupt.begin() + table + 8, corrupt.begin() + originalOffset, 'x');
    require(!rejected.load(corrupt.data(), corrupt.size(), error), "unterminated routine names");
    const auto tooMany = program({{"main", join(std::vector<Bytes>(Program::maxInstructions + 1, instruction(0)))}});
    require(!rejected.load(tooMany.data(), tooMany.size(), error) && error.find("instruction budget") != std::string::npos,
            "aggregate instruction cap");
    auto excessiveIncludes = program({{"main", instruction(10)}});
    put32(excessiveIncludes, 40, Program::maxIncludes + 1);
    require(!rejected.load(excessiveIncludes.data(), excessiveIncludes.size(), error), "aggregate include cap");
    std::uint32_t seed = 0x128ec1;
    for (std::size_t iteration = 0; iteration < 2000; ++iteration) {
        auto mutation = valid;
        seed = seed * 1664525u + 1013904223u;
        const auto offset = seed % mutation.size();
        seed = seed * 1664525u + 1013904223u;
        mutation[offset] ^= static_cast<std::uint8_t>((seed >> 24) | 1u);
        Program candidate;
        if (candidate.load(mutation.data(), mutation.size(), error))
            require(candidate.resource().bytes == mutation && error.empty(), "mutation ownership invariant");
        else
            require(!candidate.loaded() && !error.empty(), "mutation rejection invariant");
    }
    std::puts("PASS ECL 2000 deterministic reader mutations and aggregate resource budgets");
}

void realPrograms(int count, char** paths) {
    std::size_t subroutines = 0;
    std::size_t instructions = 0;
    for (int index = 0; index < count; ++index) {
        std::ifstream source(paths[index], std::ios::binary);
        require(source.good(), std::string("cannot open fixture: ") + paths[index]);
        const Bytes bytes((std::istreambuf_iterator<char>(source)), std::istreambuf_iterator<char>());
        Program loaded;
        std::string error;
        require(loaded.load(bytes.data(), bytes.size(), error), std::string(paths[index]) + ": " + error);
        subroutines += loaded.subroutines().size();
        for (const auto& sub : loaded.subroutines())
            instructions += sub.instructions.size();
    }
    require(count == 33 && subroutines == 1459 && instructions == 49097, "catalog corpus totals");
    std::printf("PASS ECL actual resources: %d files, %zu subroutines, %zu instructions\n",
                count, subroutines, instructions);
}

int main(int argc, char** argv) {
    try {
        if (argc > 1)
            realPrograms(argc - 1, argv + 1);
        else
            readerTests();
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "FAIL ECL reader: %s\n", error.what());
        return 1;
    }
}
