#include "thprac/portable/th07/patches.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <vector>

using namespace thprac::portable;

int main(int argc, char** argv)
{
    auto patch = th07::Stage1Mid2();
    assert(patch.id == "th07.stage1.mid2");
    assert(patch.expectedCrc32 == 0xb0aec128u);
    assert(patch.writes.size() == 1);

    const auto& write = patch.writes.front();
    std::vector<std::uint8_t> fixture(write.offset + write.expected.size(), 0);
    std::copy(write.expected.begin(), write.expected.end(), fixture.begin() + write.offset);
    patch.expectedCrc32 = Crc32(fixture.data(), fixture.size());

    assert(ApplyPatchSet(fixture.data(), fixture.size(), patch));
    assert(std::equal(
        write.replacement.begin(), write.replacement.end(), fixture.begin() + write.offset));

    if (argc == 2) {
        std::ifstream input(argv[1], std::ios::binary);
        assert(input);
        std::vector<std::uint8_t> realEcl {
            std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
        const auto realResult = ApplyPatchSet(realEcl.data(), realEcl.size(), th07::Stage1Mid2());
        assert(realResult);
        assert(std::equal(
            write.replacement.begin(), write.replacement.end(), realEcl.begin() + write.offset));
    }
}
