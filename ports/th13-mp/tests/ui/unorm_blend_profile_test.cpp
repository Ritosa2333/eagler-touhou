#include "../harness/test_framework.hpp"
#include "../../portable/sdl/UnormBlendProfile.hpp"
#include "../render/color01_expected.hpp"

#include <algorithm>
#include <cmath>

using namespace touhou::graphics;
using namespace touhou::sdl;
namespace expected = th13test::color01;

namespace {

int mul_unorm8(int a, int b) {
    const int product = a * b;
    return (product + 128 + (product >> 8)) >> 8;
}

// Exactly the arithmetic used by UnormBlendProfile.hpp's fragment snippet.
int shader_mul_unorm8(int a, int b) {
    const float product = float(a * b);
    return int(std::floor((product + 128.0f + std::floor(product / 256.0f)) / 256.0f));
}

// The unmodified PR-head GLES path on SwiftShader: one rounding of the
// complete normalized sum, with no source-term preweighting.
int single_round(int source, int destination, int alpha) {
    const int total = source * alpha + destination * (255 - alpha);
    return std::min(255, (total + 127) / 255);
}

PipelineState source_alpha_pipeline() {
    PipelineState pipeline;
    pipeline.blend = true;
    pipeline.sourceBlend = BlendFactor::SourceAlpha;
    pipeline.destinationBlend = BlendFactor::InverseSourceAlpha;
    pipeline.blendEquation = BlendEquation::Add;
    return pipeline;
}

}  // namespace

TEST(unorm_mul8_shader_formula_matches_integer_reference) {
    bool matches = true;
    for (int a = 0; a < 256; ++a)
        for (int b = 0; b < 256; ++b)
            if (shader_mul_unorm8(a, b) != mul_unorm8(a, b)) matches = false;
    CHECK(matches);
}

TEST(unorm_source_and_alpha_quantization_are_exact) {
    bool exact = true;
    for (int value = 0; value < 256; ++value) {
        const float normalized = float(value) / 255.0f;
        if (int(std::floor(normalized * 255.0f + 0.5f)) != value) exact = false;
    }
    CHECK(exact);
}

TEST(unorm_destination_rounding_matches_per_term_sum) {
    // The shader preweights and rounds the source RGB term to an integer q.
    // GL's final UNORM rounding then sees q/255 + dst*(255-a)/255, whose
    // fractional part is k/255 and never a tie. Exhaustive integer check.
    bool matches = true;
    for (int q = 0; q < 256 && matches; ++q)
        for (int d = 0; d < 256 && matches; ++d)
            for (int a = 0; a < 256 && matches; ++a) {
                const int gl = std::min(255, (q * 255 + d * (255 - a) + 127) / 255);
                const int native = std::min(255, q + (d * (255 - a) + 127) / 255);
                if (gl != native) matches = false;
            }
    CHECK(matches);
}

TEST(unorm_blend_profile_default_preserves_gles) {
    PipelineState pipeline = source_alpha_pipeline();
    CHECK(!separate_rgb_terms(UnormBlendProfile::NativeGles, pipeline,
                              PixelFormat::Bgra8));
    CHECK(separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                             PixelFormat::Bgra8));
}

TEST(unorm_blend_profile_gates_target_format) {
    PipelineState pipeline = source_alpha_pipeline();
    CHECK(separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                             PixelFormat::Bgra8));
    for (auto format : {PixelFormat::Bgrx8, PixelFormat::Rgb565,
                        PixelFormat::Argb4444, PixelFormat::Argb1555,
                        PixelFormat::Depth16}) {
        CHECK(!separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                                  format));
    }
}

TEST(unorm_blend_profile_safe_separate_alpha) {
    PipelineState pipeline = source_alpha_pipeline();
    pipeline.separateAlphaBlend = true;
    // The default separate-alpha factors are One/Zero; no alpha factor reads
    // the preweighted source RGB, so the RGB lowering remains safe.
    CHECK(separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                             PixelFormat::Bgra8));
    pipeline.sourceAlphaBlend = BlendFactor::SourceAlpha;
    pipeline.destinationAlphaBlend = BlendFactor::InverseSourceAlpha;
    CHECK(separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                             PixelFormat::Bgra8));
    pipeline.sourceAlphaBlend = BlendFactor::DestinationAlpha;
    pipeline.destinationAlphaBlend = BlendFactor::InverseDestinationAlpha;
    CHECK(separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                             PixelFormat::Bgra8));
    pipeline.sourceAlphaBlend = BlendFactor::DestinationColor;
    pipeline.destinationAlphaBlend = BlendFactor::InverseDestinationColor;
    CHECK(separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                             PixelFormat::Bgra8));
    pipeline.sourceAlphaBlend = BlendFactor::SourceAlphaSaturate;
    pipeline.destinationAlphaBlend = BlendFactor::One;
    CHECK(separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                             PixelFormat::Bgra8));
    pipeline.sourceAlphaBlend = BlendFactor::SourceColor;
    pipeline.destinationAlphaBlend = BlendFactor::Zero;
    CHECK(!separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                              PixelFormat::Bgra8));
    pipeline.sourceAlphaBlend = BlendFactor::InverseSourceColor;
    CHECK(!separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                              PixelFormat::Bgra8));
    pipeline.sourceAlphaBlend = BlendFactor::One;
    pipeline.destinationAlphaBlend = BlendFactor::SourceColor;
    CHECK(!separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                              PixelFormat::Bgra8));
    pipeline.destinationAlphaBlend = BlendFactor::InverseSourceColor;
    CHECK(!separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                              PixelFormat::Bgra8));
}

TEST(unorm_blend_profile_does_not_rewrite_destination_color_factors) {
    PipelineState pipeline = source_alpha_pipeline();
    for (auto destination : {BlendFactor::SourceColor,
                             BlendFactor::InverseSourceColor,
                             BlendFactor::DestinationColor,
                             BlendFactor::InverseDestinationColor,
                             BlendFactor::DestinationAlpha,
                             BlendFactor::InverseDestinationAlpha,
                             BlendFactor::SourceAlphaSaturate}) {
        pipeline.destinationBlend = destination;
        CHECK(!separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                                  PixelFormat::Bgra8));
    }
}

TEST(unorm_blend_profile_only_additive_source_alpha) {
    PipelineState pipeline = source_alpha_pipeline();
    for (auto destination : {BlendFactor::Zero, BlendFactor::One,
                             BlendFactor::InverseSourceAlpha}) {
        pipeline.destinationBlend = destination;
        CHECK(separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                                 PixelFormat::Bgra8));
    }
    for (auto equation : {BlendEquation::Subtract, BlendEquation::ReverseSubtract,
                          BlendEquation::Minimum, BlendEquation::Maximum}) {
        pipeline.blendEquation = equation;
        CHECK(!separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                                  PixelFormat::Bgra8));
    }
    pipeline.blendEquation = BlendEquation::Add;
    for (auto source : {BlendFactor::One, BlendFactor::SourceColor,
                        BlendFactor::InverseSourceColor,
                        BlendFactor::DestinationAlpha}) {
        pipeline.sourceBlend = source;
        CHECK(!separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                                  PixelFormat::Bgra8));
    }
    pipeline.sourceBlend = BlendFactor::SourceAlpha;
    pipeline.blend = false;
    CHECK(!separate_rgb_terms(UnormBlendProfile::SeparateRgbTerms, pipeline,
                              PixelFormat::Bgra8));
}

TEST(unorm_known_color01_case_matches_per_term) {
    // Source RGB (15,1,15), destination RGB (2,2,5), 256 source alphas.
    // Expected values are the synthetic D3D9 specialAlphaSweep table in
    // tests/render/color01_expected.hpp, not a self-comparison.
    CHECK_EQ(expected::kBlendRgb[187][0], 12);
    CHECK_EQ(expected::kBlendRgb[187][1], 2);
    CHECK_EQ(expected::kBlendRgb[187][2], 12);
    bool matches = true;
    for (int alpha = 0; alpha < 256; ++alpha) {
        const int source[3] = {expected::kSourceRgb[0], expected::kSourceRgb[1],
                               expected::kSourceRgb[2]};
        const int destination[3] = {expected::kDestinationRgb[0],
                                    expected::kDestinationRgb[1],
                                    expected::kDestinationRgb[2]};
        for (int channel = 0; channel < 3; ++channel) {
            const int shader = std::min(
                255, mul_unorm8(source[channel], alpha) +
                         mul_unorm8(destination[channel], 255 - alpha));
            if (shader != expected::kBlendRgb[alpha][channel]) matches = false;
        }
    }
    CHECK(matches);
}

TEST(unorm_known_color01_case_distinguishes_legacy_path) {
    const int alpha = expected::kProblemAlpha;
    CHECK_EQ(expected::kBlendRgb[alpha][0], 12);
    CHECK_EQ(expected::kBlendRgb[alpha][1], 2);
    CHECK_EQ(expected::kBlendRgb[alpha][2], 12);
    CHECK_EQ(single_round(expected::kSourceRgb[1], expected::kDestinationRgb[1],
                          alpha),
             1);
    bool differs = false;
    for (int a = 0; a < 256; ++a)
        if (expected::kBlendRgb[a][1] !=
            single_round(expected::kSourceRgb[1], expected::kDestinationRgb[1], a))
            differs = true;
    CHECK(differs);
}

int main() { return th13test::run_all("UNORM blend profile"); }
