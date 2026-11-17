// Independent production-Renderer WebGL fixture for COLOR-01.
//
// It compiles the unmodified portable/sdl/Renderer.cpp and drives its public
// API through an offscreen 256x16 GL_RGBA8 target. The reference arithmetic is
// the independently measured Wine/Mesa UNORM8 formula, not a copy of the
// shader: round(src*alpha/255) + round(dst*(255-alpha)/255).
#include <emscripten.h>

#include "../../../portable/sdl/Renderer.hpp"
#include "../color01_expected.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <string>
#include <vector>

using namespace touhou::graphics;
using touhou::sdl::Renderer;
using touhou::sdl::Surface;
using touhou::sdl::u8;
using touhou::sdl::UnormBlendProfile;
namespace expected = th13test::color01;

namespace {

constexpr u32 kWidth = 256;
constexpr u32 kRows = 16;
constexpr u32 kSourceId = 1;
constexpr u32 kTargetId = 2;
constexpr u32 kTargetRgbId = 3;

struct Vertex {
    float x, y, z, w;
    u32 color;
    float u, v;
};
static_assert(sizeof(Vertex) == 28, "ScreenColorUv stride");

int mul_unorm8(int a, int b) {
    const int product = a * b;
    return (product + 128 + (product >> 8)) >> 8;
}

int single_round(int source, int destination, int alpha) {
    const int total = source * alpha + destination * (255 - alpha);
    return std::min(255, (total + 127) / 255);
}

std::array<u8, 3> expected_blend(u32 x) {
    return {expected::kBlendRgb[x][0], expected::kBlendRgb[x][1],
            expected::kBlendRgb[x][2]};
}

std::array<u8, 3> expected_single(u32 x) {
    return {u8(single_round(expected::kSourceRgb[0],
                            expected::kDestinationRgb[0], int(x))),
            u8(single_round(expected::kSourceRgb[1],
                            expected::kDestinationRgb[1], int(x))),
            u8(single_round(expected::kSourceRgb[2],
                            expected::kDestinationRgb[2], int(x)))};
}

struct Owner {
    std::vector<u8> source;
    std::vector<u8> target;
    std::vector<u8> target_rgb;
    u32 source_version = 1;
    u32 target_version = 1;
    u32 target_rgb_version = 1;

    Owner() : source(kWidth * 4), target(kWidth * kRows * 4),
              target_rgb(kWidth * kRows * 4) {
        for (u32 x = 0; x < kWidth; ++x) {
            auto* p = source.data() + x * 4;
            p[0] = expected::kSourceRgb[2];  // B
            p[1] = expected::kSourceRgb[1];  // G
            p[2] = expected::kSourceRgb[0];  // R
            p[3] = u8(x);  // source alpha sweep
        }
        reset_target(target, target_version);
        reset_target(target_rgb, target_rgb_version);
    }

    static void reset_target(std::vector<u8>& pixels, u32& version) {
        for (u32 i = 0; i < kWidth * kRows; ++i) {
            auto* p = pixels.data() + i * 4;
            p[0] = expected::kDestinationRgb[2];  // B
            p[1] = expected::kDestinationRgb[1];  // G
            p[2] = expected::kDestinationRgb[0];  // R
            p[3] = 255;  // A
        }
        ++version;
    }

    Surface resolve(u32 id) {
        Surface s;
        s.handle = id;
        s.width = kWidth;
        s.renderScale = 1;
        if (id == kSourceId) {
            s.height = 1;
            s.format = PixelFormat::Bgra8;
            s.pitch = kWidth * 4;
            s.data = source.data();
            s.size = u32(source.size());
            s.version = source_version;
        } else if (id == kTargetId) {
            s.height = kRows;
            s.format = PixelFormat::Bgra8;
            s.pitch = kWidth * 4;
            s.data = target.data();
            s.size = u32(target.size());
            s.version = target_version;
        } else if (id == kTargetRgbId) {
            s.height = kRows;
            s.format = PixelFormat::Bgrx8;
            s.pitch = kWidth * 4;
            s.data = target_rgb.data();
            s.size = u32(target_rgb.size());
            s.version = target_rgb_version;
        }
        return s;
    }
};

Surface resolve_surface(void* owner, u32 id) {
    return static_cast<Owner*>(owner)->resolve(id);
}

std::string json_escape(const char* text) {
    std::string out;
    if (!text) return out;
    for (const char* p = text; *p; ++p) {
        const unsigned char c = static_cast<unsigned char>(*p);
        if (c == '"' || c == '\\') {
            out += '\\';
            out += char(c);
        } else if (c >= 32 && c < 127) {
            out += char(c);
        } else {
            out += '?';
        }
    }
    return out;
}

struct TestResult {
    std::string name;
    bool pass = true;
    int mismatches = 0;
    int first_x = -1;
    int expected[3] = {0, 0, 0};
    int actual[3] = {0, 0, 0};
};

std::array<u8, 3> rgb_at(const std::vector<u8>& pixels, u32 x) {
    const u8* p = pixels.data() + (kRows / 2 * kWidth + x) * 4;
    return {p[2], p[1], p[0]};
}

void add_test(std::vector<TestResult>& tests, const char* name,
              const std::function<std::array<u8, 3>(u32)>& expected,
              const std::vector<u8>& pixels) {
    TestResult result;
    result.name = name;
    for (u32 x = 0; x < kWidth; ++x) {
        const auto actual = rgb_at(pixels, x);
        const auto want = expected(x);
        if (actual != want) {
            ++result.mismatches;
            if (result.first_x < 0) {
                result.first_x = int(x);
                for (int c = 0; c < 3; ++c) {
                    result.expected[c] = want[c];
                    result.actual[c] = actual[c];
                }
            }
        }
    }
    result.pass = result.mismatches == 0;
    tests.push_back(result);
}

void set_pipeline(Renderer& renderer, u32 target, BlendFactor destination,
                  bool blend, bool separate_alpha) {
    renderer.state.target = target;
    renderer.state.texture = kSourceId;
    renderer.state.depth = 0;
    renderer.state.layout = attributes(VertexLayout::ScreenColorUv);
    renderer.state.viewport = {0, 0, kWidth, kRows, 0, 1};
    auto& p = renderer.state.pipeline;
    p = PipelineState{};
    p.depthTest = false;
    p.depthWrite = false;
    p.blend = blend;
    p.sourceBlend = BlendFactor::SourceAlpha;
    p.destinationBlend = destination;
    p.blendEquation = BlendEquation::Add;
    p.separateAlphaBlend = separate_alpha;
    p.sourceAlphaBlend = BlendFactor::One;
    p.destinationAlphaBlend = BlendFactor::Zero;
    p.alphaBlendEquation = BlendEquation::Add;
    p.alphaTest = false;
    p.fog = false;
    p.dither = false;
    p.colorMask = 15;
    p.color.operation = ColorOperation::Multiply;
    p.color.first = {ArgumentSource::Texture};
    p.color.second = {ArgumentSource::Diffuse};
    p.alpha.operation = ColorOperation::Multiply;
    p.alpha.first = {ArgumentSource::Texture};
    p.alpha.second = {ArgumentSource::Diffuse};
    p.textureFactor = 0xffffffff;
    p.minFilter = Filter::Nearest;
    p.magFilter = Filter::Nearest;
    p.addressU = Address::Clamp;
    p.addressV = Address::Clamp;
}

void draw(Renderer& renderer, u32 target) {
    const Vertex quad[] = {
        {0, 0, 0, 1, 0xffffffff, 0, 0.5f},
        {float(kWidth), 0, 0, 1, 0xffffffff, 1, 0.5f},
        {0, float(kRows), 0, 1, 0xffffffff, 0, 0.5f},
        {0, float(kRows), 0, 1, 0xffffffff, 0, 0.5f},
        {float(kWidth), 0, 0, 1, 0xffffffff, 1, 0.5f},
        {float(kWidth), float(kRows), 0, 1, 0xffffffff, 1, 0.5f},
    };
    renderer.draw(Topology::Triangles, 2, quad, sizeof(Vertex));
    renderer.read(target);
}

std::string run() {
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_CANVAS_SELECTOR, "#canvas");
    Owner owner;
    std::vector<TestResult> tests;
    bool late_configure_rejected = false;
    std::string gl_renderer;
    {
    Renderer renderer(10, resolve_surface, &owner);
    if (!renderer.configure_unorm_blend_profile(
            UnormBlendProfile::SeparateRgbTerms, kTargetId)) {
        return "{\"schema\":\"th13/color01-gl-fixture/v1\",\"pass\":false,"
               "\"error\":\"configure_unorm_blend_profile rejected before initialize\"}";
    }
    if (!renderer.initialize()) {
        std::string error = "{\"schema\":\"th13/color01-gl-fixture/v1\","
                            "\"pass\":false,\"error\":\"renderer initialize failed: ";
        error += json_escape(renderer.error());
        error += "\"}";
        return error;
    }
    late_configure_rejected =
        !renderer.configure_unorm_blend_profile(UnormBlendProfile::NativeGles, kTargetId);
    const char* gl_name = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    gl_renderer = gl_name ? gl_name : "";

    // Blend disabled must reproduce the source RGB (no accidental preweight).
    set_pipeline(renderer, kTargetId, BlendFactor::InverseSourceAlpha, false, false);
    Owner::reset_target(owner.target, owner.target_version);
    draw(renderer, kTargetId);
    add_test(tests, "blend_off_source",
             [](u32) { return std::array<u8, 3>{15, 1, 15}; }, owner.target);

    // The measured default: SRCALPHA / INVSRCALPHA / ADD.
    set_pipeline(renderer, kTargetId, BlendFactor::InverseSourceAlpha, true, false);
    Owner::reset_target(owner.target, owner.target_version);
    draw(renderer, kTargetId);
    add_test(tests, "blend_on_inverse_source_alpha",
             expected_blend,
             owner.target);
    {
        const auto pixel = rgb_at(owner.target, 187);
        TestResult problem;
        problem.name = "problem_pixel_592_102_alpha187";
        problem.pass = pixel == std::array<u8, 3>{12, 2, 12};
        problem.first_x = 187;
        problem.expected[0] = 12; problem.expected[1] = 2; problem.expected[2] = 12;
        problem.actual[0] = pixel[0]; problem.actual[1] = pixel[1]; problem.actual[2] = pixel[2];
        problem.mismatches = problem.pass ? 0 : 1;
        tests.push_back(problem);
    }

    // Additive and replace destination factors.
    set_pipeline(renderer, kTargetId, BlendFactor::One, true, false);
    Owner::reset_target(owner.target, owner.target_version);
    draw(renderer, kTargetId);
    add_test(tests, "blend_on_one",
             [](u32 x) {
                 return std::array<u8, 3>{
                     u8(std::min(255, mul_unorm8(15, int(x)) + 2)),
                     u8(std::min(255, mul_unorm8(1, int(x)) + 2)),
                     u8(std::min(255, mul_unorm8(15, int(x)) + 5))};
             },
             owner.target);

    set_pipeline(renderer, kTargetId, BlendFactor::Zero, true, false);
    Owner::reset_target(owner.target, owner.target_version);
    draw(renderer, kTargetId);
    add_test(tests, "blend_on_zero",
             [](u32 x) {
                 return std::array<u8, 3>{
                     u8(mul_unorm8(15, int(x))),
                     u8(mul_unorm8(1, int(x))),
                     u8(mul_unorm8(15, int(x)))};
             },
             owner.target);

    // Cached transitions: off -> on -> same on -> off -> on.
    set_pipeline(renderer, kTargetId, BlendFactor::InverseSourceAlpha, false, false);
    Owner::reset_target(owner.target, owner.target_version);
    draw(renderer, kTargetId);
    add_test(tests, "cache_off_to_on_step1_off",
             [](u32) { return std::array<u8, 3>{15, 1, 15}; }, owner.target);

    renderer.state.pipeline.blend = true;
    Owner::reset_target(owner.target, owner.target_version);
    draw(renderer, kTargetId);
    add_test(tests, "cache_off_to_on_step2_on",
             expected_blend,
             owner.target);

    Owner::reset_target(owner.target, owner.target_version);
    draw(renderer, kTargetId);
    add_test(tests, "cache_on_same_state",
             expected_blend,
             owner.target);

    renderer.state.pipeline.blend = false;
    Owner::reset_target(owner.target, owner.target_version);
    draw(renderer, kTargetId);
    add_test(tests, "cache_on_to_off",
             [](u32) { return std::array<u8, 3>{15, 1, 15}; }, owner.target);

    renderer.state.pipeline.blend = true;
    Owner::reset_target(owner.target, owner.target_version);
    draw(renderer, kTargetId);
    add_test(tests, "cache_off_to_on_again",
             expected_blend,
             owner.target);

    // Separate alpha factors are safe when neither factor reads the
    // preweighted source RGB. One/Zero reads no source color, so the RGB
    // lowering remains valid; SourceColor reads the preweighted RGB, so it
    // must stay on the old GLES path.
    set_pipeline(renderer, kTargetId, BlendFactor::InverseSourceAlpha, true, true);
    Owner::reset_target(owner.target, owner.target_version);
    draw(renderer, kTargetId);
    add_test(tests, "separate_alpha_safe_factors_use_lowering",
             expected_blend,
             owner.target);

    renderer.state.pipeline.sourceAlphaBlend = BlendFactor::SourceColor;
    Owner::reset_target(owner.target, owner.target_version);
    draw(renderer, kTargetId);
    add_test(tests, "separate_alpha_source_color_falls_back",
             expected_single,
             owner.target);

    // Target-format cache transitions. The proven target type is Bgra8
    // (GL_RGBA8). Bgrx8 (GL_RGB8) must restore the GLES source factor even
    // though the pipeline fields are unchanged, and switching back must
    // restore the preweighted factor.
    set_pipeline(renderer, kTargetId, BlendFactor::InverseSourceAlpha, true, false);
    Owner::reset_target(owner.target, owner.target_version);
    draw(renderer, kTargetId);
    add_test(tests, "target_format_bgra8_before_bgrx8",
             expected_blend,
             owner.target);

    set_pipeline(renderer, kTargetRgbId, BlendFactor::InverseSourceAlpha, true, false);
    Owner::reset_target(owner.target_rgb, owner.target_rgb_version);
    draw(renderer, kTargetRgbId);
    add_test(tests, "target_format_bgrx8_keeps_gles_path",
             expected_single,
             owner.target_rgb);

    set_pipeline(renderer, kTargetId, BlendFactor::InverseSourceAlpha, true, false);
    Owner::reset_target(owner.target, owner.target_version);
    draw(renderer, kTargetId);
    add_test(tests, "target_format_bgra8_after_bgrx8",
             expected_blend,
             owner.target);
    }

    // Default profile: the unmodified PR-head behavior. This is a separate
    // Renderer instance with no profile configuration. SwiftShader's combined
    // rounding is the independent reference for the full 256-alpha sweep.
    {
    Renderer renderer(10, resolve_surface, &owner);
    if (!renderer.initialize()) {
        std::string error = "{\"schema\":\"th13/color01-gl-fixture/v1\","
                            "\"pass\":false,\"error\":\"default renderer initialize failed: ";
        error += json_escape(renderer.error());
        error += "\"}";
        return error;
    }
    TestResult late;
    late.name = "default_late_configure_rejected";
    late.pass = !renderer.configure_unorm_blend_profile(UnormBlendProfile::NativeGles,
                                                         kTargetId);
    late.mismatches = late.pass ? 0 : 1;
    tests.push_back(late);

    set_pipeline(renderer, kTargetId, BlendFactor::InverseSourceAlpha, false, false);
    Owner::reset_target(owner.target, owner.target_version);
    draw(renderer, kTargetId);
    add_test(tests, "default_blend_off_source",
             [](u32) {
                 return std::array<u8, 3>{expected::kSourceRgb[0],
                                          expected::kSourceRgb[1],
                                          expected::kSourceRgb[2]};
             },
             owner.target);

    renderer.state.pipeline.blend = true;
    Owner::reset_target(owner.target, owner.target_version);
    draw(renderer, kTargetId);
    add_test(tests, "default_blend_on_full_alpha",
             expected_single,
             owner.target);
    {
        const auto pixel = rgb_at(owner.target, expected::kProblemAlpha);
        TestResult old;
        old.name = "default_problem_pixel_pr_head_baseline";
        old.pass = pixel == std::array<u8, 3>{12, 1, 12};
        old.first_x = expected::kProblemAlpha;
        old.expected[0] = 12; old.expected[1] = 1; old.expected[2] = 12;
        old.actual[0] = pixel[0]; old.actual[1] = pixel[1]; old.actual[2] = pixel[2];
        old.mismatches = old.pass ? 0 : 1;
        tests.push_back(old);
    }
    }

    bool pass = late_configure_rejected;
    for (const auto& test : tests) pass = pass && test.pass;

    std::string out = "{\"schema\":\"th13/color01-gl-fixture/v1\",\"pass\":";
    out += pass ? "true" : "false";
    out += ",\"renderer\":\"";
    out += json_escape(gl_renderer.c_str());
    out += "\",\"lateConfigureRejected\":";
    out += late_configure_rejected ? "true" : "false";
    out += ",\"tests\":[";
    for (size_t i = 0; i < tests.size(); ++i) {
        const auto& test = tests[i];
        if (i) out += ',';
        out += "{\"name\":\"";
        out += test.name;
        out += "\",\"pass\":";
        out += test.pass ? "true" : "false";
        out += ",\"mismatches\":";
        out += std::to_string(test.mismatches);
        out += ",\"firstX\":";
        out += std::to_string(test.first_x);
        out += ",\"expected\":[";
        out += std::to_string(test.expected[0]) + "," +
               std::to_string(test.expected[1]) + "," +
               std::to_string(test.expected[2]);
        out += "],\"actual\":[";
        out += std::to_string(test.actual[0]) + "," +
               std::to_string(test.actual[1]) + "," +
               std::to_string(test.actual[2]);
        out += "]}";
    }
    out += "]}";
    return out;
}

}  // namespace

extern "C" {

EMSCRIPTEN_KEEPALIVE const char* color01_run() {
    static std::string result = run();
    return result.c_str();
}

}
