#include "anm_test_fixtures.hpp"
#include "game/Interpolation.hpp"

#include <array>

using namespace th13;

namespace {
struct AngleCase {
    float start, end, control1, control2;
    i32 frames, mode;
    std::array<u32, 16> expected;
};
const AngleCase kAngleCases[] = {
    {3.5f,
     8.25f,
     0.f,
     0.f,
     6,
     7,
     {0xbf50fdb0u, 0x3f9341bcu, 0x4047812au, 0xbf997c88u, 0x3f448818u, 0x402f0252u, 0x41040000u}},
    {4.25f,
     0.75f,
     0.f,
     2.5f,
     6,
     17,
     {0x3eef0250u, 0xc0243f6cu, 0x3fb78128u, 0xbdcbe440u, 0xbf61fb60u, 0xbf6a7a38u, 0x3f400000u}},
    {5.25f,
     -6.5f,
     8.75f,
     -9.25f,
     12,
     8,
     {0xbf17c138u, 0xbf2d3c78u, 0xbf9a3f6cu, 0xc00645a4u, 0x40407b43u, 0x3fd00000u, 0x3df097e0u, 0xbfb7b42au,
      0xc03d0000u, 0x3ff69e3fu, 0x3f37c133u, 0xbe5e04a0u, 0xc0d00000u}},
    {5.75f,
     -8.25f,
     0.f,
     0.f,
     12,
     0,
     {0xbf271460u, 0xbf45a9e8u, 0xbf643f68u, 0xbf816a78u, 0xbf90b53cu, 0xbfa00000u, 0xbfaf4ac4u, 0xbfbe9588u,
      0xbfcde04cu, 0xbfdd2b0cu, 0xbfec75d0u, 0xbffbc094u, 0xc1040000u}},
    {-7.25f,
     9.5f,
     0.f,
     0.f,
     12,
     1,
     {0xbf7b3cb0u, 0xbf8337a4u, 0xbf8c8c78u, 0xbf999cd4u, 0xbfaa68b8u, 0xbfbef024u, 0xbfd7331cu, 0xbff33194u,
      0xc00975ceu, 0xc01b3096u, 0xc02ec91eu, 0xc0443f6eu, 0x41180000u}},
    {9.75f,
     -12.25f,
     0.f,
     0.f,
     15,
     18,
     {0xc01f4a4cu, 0xc00a8ff8u, 0xbfec9518u, 0xbfc565d0u, 0xbf9ffff8u, 0xbf799900u, 0xbf385c30u, 0xbefa0100u,
      0xbe9058e0u, 0xbdd3a180u, 0x3d3a8000u, 0x3e26f540u, 0x3e7ddd40u, 0x3e993220u, 0x3ea1fb80u, 0xc1440000u}},
    {-10.5f,
     13.75f,
     0.f,
     0.f,
     15,
     22,
     {0x4007830cu, 0x4009c590u, 0x400b06f8u, 0x400b4740u, 0x400a8668u, 0x4008c474u, 0x40060160u, 0x40023d30u,
      0x3ffaefc0u, 0x3fef62e8u, 0x3fe1d3d0u, 0x3fd24280u, 0x3fc0aef0u, 0x3fad1928u, 0x3f978120u, 0x415c0000u}},
};
struct BezierCase {
    float start, end, control1, control2;
    std::array<u32, 11> expected;
};
const BezierCase kBezierCases[] = {
    {-1.25f,
     2.5f,
     4.75f,
     -6.125f,
     {0xbf348312u, 0xbd65602bu, 0x3f24e562u, 0x3fabc6a8u, 0x3ffe0000u, 0x402126eau, 0x4038ef9du, 0x4042f1aau,
      0x403bc49cu, 0x40200000u, 0x40200000u}},
    {3.75f,
     -2.125f,
     7.5f,
     -8.25f,
     {0x40888d50u, 0x408b9db2u, 0x40834dd3u, 0x406374bcu, 0x40320000u, 0x3feced90u, 0x3f5449bcu, 0xbe5f3b72u,
      0xbf9cc49au, 0xc0080000u, 0xc0080000u}},
};
struct DefaultCase {
    float from, to;
    i32 frames, mode;
    std::array<u32, 46> expected;
};
const DefaultCase kDefaultCases[] = {
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
}

TEST(angle_interpolation_matches_retail_oracle) {
    for (const AngleCase& item : kAngleCases) {
        AngleInterpolation tween;
        tween.begin(item.frames, item.mode, {item.start}, {item.end});
        tween.timer.current = -1;
        tween.timer.fractional = 0.f;
        tween.control1[0] = item.control1;
        tween.control2[0] = item.control2;
        for (u32 index = 0; index <= u32(item.frames); index++)
            CHECK_EQ(float_to_bits(tween.step(1.f)[0]), item.expected[index]);
    }
}

TEST(scalar_bezier_matches_retail_oracle) {
    for (const BezierCase& item : kBezierCases) {
        FloatInterpolation<1> tween;
        EclInterpolation scalar;
        tween.begin(10, 8, {item.start}, {item.end});
        scalar.begin(10, 8, item.start, item.end);
        tween.timer.current = -1;
        tween.timer.fractional = 0.f;
        scalar.timer.current = -1;
        scalar.timer.fractional = 0.f;
        tween.control1[0] = item.control1;
        tween.control2[0] = item.control2;
        scalar.control1 = item.control1;
        scalar.control2 = item.control2;
        for (u32 index = 0; index < 11; index++) {
            CHECK_EQ(float_to_bits(tween.step(1.f)[0]), item.expected[index]);
            CHECK_EQ(float_to_bits(scalar.step(1.f)), item.expected[index]);
        }
    }
}

TEST(interpolation_final_value_rounds_once_like_x87_4120f0) {
    for (const DefaultCase& item : kDefaultCases) {
        FloatInterpolation<1> tween;
        EclInterpolation scalar;
        tween.begin(item.frames, item.mode, {item.from}, {item.to});
        scalar.begin(item.frames, item.mode, item.from, item.to);
        for (u32 index = 0; index <= u32(item.frames); index++) {
            CHECK_EQ(float_to_bits(tween.step(1.f)[0]), item.expected[index]);
            CHECK_EQ(float_to_bits(scalar.step(1.f)), item.expected[index]);
        }
    }
}

int main() { return th13test::run_all("interpolation retail oracle"); }
