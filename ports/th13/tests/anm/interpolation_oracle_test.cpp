#include "anm_test_fixtures.hpp"
#include "game/Interpolation.hpp"
#include <vector>
using namespace th13;

namespace {
struct Case {
    float from, to;
    i32 frames, mode;
    std::vector<u32> expected;
};
const Case kCases[] = {
    {0.5f, 2.0f, 30, 0, {0x3f0ccccdu, 0x3f19999au, 0x3f266666u, 0x3f333333u, 0x3f400000u, 0x3f4ccccdu, 0x3f59999au,
                         0x3f666667u, 0x3f733334u, 0x3f800000u, 0x3f866666u, 0x3f8ccccdu, 0x3f933333u, 0x3f99999au,
                         0x3fa00000u, 0x3fa66667u, 0x3faccccdu, 0x3fb33334u, 0x3fb9999au, 0x3fc00000u, 0x3fc66666u,
                         0x3fcccccdu, 0x3fd33333u, 0x3fd9999au, 0x3fe00000u, 0x3fe66666u, 0x3fecccccu, 0x3ff33333u,
                         0x3ff99999u, 0x40000000u, 0x40000000u}},
    {1.0f, 0.25f, 20, 1, {0x3f7f851fu, 0x3f7e147bu, 0x3f7bae14u, 0x3f7851ebu, 0x3f740000u, 0x3f6eb852u, 0x3f687ae1u,
                          0x3f6147aeu, 0x3f591eb8u, 0x3f500000u, 0x3f45eb85u, 0x3f3ae148u, 0x3f2ee148u, 0x3f21eb85u,
                          0x3f140000u, 0x3f051eb8u, 0x3eea8f5cu, 0x3ec8f5c4u, 0x3ea570a4u, 0x3e800000u, 0x3e800000u}},
    {0.5f, 3.0f, 45, 0, {0x3f0e38e4u, 0x3f1c71c7u, 0x3f2aaaabu, 0x3f38e38eu, 0x3f471c72u, 0x3f555556u, 0x3f638e39u,
                         0x3f71c71du, 0x3f800000u, 0x3f871c72u, 0x3f8e38e4u, 0x3f955556u, 0x3f9c71c7u, 0x3fa38e39u,
                         0x3faaaaabu, 0x3fb1c71du, 0x3fb8e38eu, 0x3fc00000u, 0x3fc71c72u, 0x3fce38e4u, 0x3fd55555u,
                         0x3fdc71c7u, 0x3fe38e3au, 0x3feaaaabu, 0x3ff1c71du, 0x3ff8e38fu, 0x40000000u, 0x40038e39u,
                         0x40071c72u, 0x400aaaabu, 0x400e38e4u, 0x4011c71du, 0x40155556u, 0x4018e38eu, 0x401c71c7u,
                         0x40200000u, 0x40238e39u, 0x40271c72u, 0x402aaaabu, 0x402e38e4u, 0x4031c71cu, 0x40355555u,
                         0x4038e38eu, 0x403c71c7u, 0x40400000u, 0x40400000u}},
    {1.0f,
     0.5f,
     10,
     18,
     {0x3f6bf9f4u, 0x3f587222u, 0x3f45e3a3u, 0x3f34c374u, 0x3f257d86u, 0x3f187221u, 0x3f0df380u, 0x3f0643c8u,
      0x3f01936eu, 0x3f000000u, 0x3f000000u}},
    {2.0f, 1.0f, 1, 0, {0x3f800000u, 0x3f800000u}},
};
void check(const Case& item) {
    FloatInterpolation<1> tween;
    EclInterpolation scalar;
    tween.begin(item.frames, item.mode, {item.from}, {item.to});
    scalar.begin(item.frames, item.mode, item.from, item.to);
    for (u32 bits : item.expected) {
        CHECK_EQ(float_to_bits(tween.step(1.f)[0]), bits);
        CHECK_EQ(float_to_bits(scalar.step(1.f)), bits);
    }
}
}

TEST(interpolation_final_value_rounds_once_like_x87_4120f0) {
    for (const Case& item : kCases)
        check(item);
}

int main() { return th13test::run_all("interpolation retail oracle"); }
