#include "../harness/test_framework.hpp"

#include "game/AnmDrawSchedule.hpp"
#include "game/PlayfieldTargets.hpp"
#include "game/SceneMatrices.hpp"
#include "game/ScreenViews.hpp"
#include "game/StageScript.hpp"

#include <cmath>
#include <fstream>
#include <vector>

using namespace th13;
using namespace touhou::graphics;

namespace {

struct RecordedClear {
    u32 order = 0, target = 0, color = 0;
    bool has_rect = false;
    GraphicsViewport rect{};
};
struct RecordedDraw {
    u32 order = 0, target = 0, texture = 0;
    std::vector<AnmGeometryVertex> vertices;
    GraphicsViewport viewport;
};

struct Graphics final : ZunGraphics {
    PipelineState p;
    u32 target = 0, texture_id = 0;
    GraphicsViewport viewport;
    bool reject_targets = false;
    std::vector<u32> selected;
    std::vector<RecordedClear> clears;
    std::vector<RecordedDraw> draws;
    u32 clock = 0;
    PipelineState& pipeline() override { return p; }
    u32 texture(const AnmResource&, u32 index) override { return index + 10; }
    void bind_texture(u32 id) override { texture_id = id; }
    void set_layout(VertexLayout) override {}
    void set_matrix(MatrixKind, const Matrix4&) override {}
    void triangles(u32 n, const void* ptr, u32 stride) override {
        CHECK_EQ(stride, u32(sizeof(AnmGeometryVertex)));
        auto* v = static_cast<const AnmGeometryVertex*>(ptr);
        draws.push_back({++clock, target, texture_id, {v, v + n * 3}, viewport});
        CHECK(target != texture_id);
    }
    void primitives(Topology, u32, const void*, u32) override {}
    bool select_target(const AnmResource* r, u32 index) override {
        if (reject_targets) return false;
        target = r ? index + 10 : 0;
        selected.push_back(target);
        return true;
    }
    bool clear_target(u32 color, const GraphicsViewport* rect) override {
        RecordedClear entry;
        entry.order = ++clock;
        entry.target = target;
        entry.color = color;
        if (rect) {
            entry.has_rect = true;
            entry.rect = *rect;
        }
        clears.push_back(entry);
        return true;
    }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport& v) override { viewport = v; }
};

struct Harness {
    GameRandom rng;
    AnmEnvironment env;
    AnmManager manager{rng, env};
    Graphics graphics;
    AnmRenderer renderer{graphics};
    ScreenViews views{renderer, env};
    FrameScheduler scheduler;
    PlayfieldTargets targets{scheduler, manager, renderer, graphics, views};
    Harness() {
        env.screen_offsets = {32, 16, 320, 16};
        std::ifstream input("artifacts/reference/raw/text.anm", std::ios::binary);
        CHECK(input.good());
        std::vector<u8> bytes{std::istreambuf_iterator<char>(input), {}};
        CHECK(manager.load(0, bytes.data(), u32(bytes.size())));
        CHECK(targets.initialize(0));
    }
};

void near(float a, float b) {
    if (std::abs(a - b) >= 0.001f) std::printf("near actual %.9g expected %.9g\n", double(a), double(b));
    CHECK(std::abs(a - b) < 0.001f);
}

struct StripOwner {
    AnmRenderer* renderer = nullptr;
    ScreenViews* views = nullptr;
    AnmVm* strip = nullptr;
    u32 submitted_order = 0;
    Graphics* graphics = nullptr;
};
i32 draw_strip(void* owner) {
    auto& o = *static_cast<StripOwner*>(owner);
    o.renderer->invalidate();
    CHECK(o.views->camera(DrawCamera::Fullscreen, false));
    CHECK_EQ(o.renderer->draw(*o.strip), 0);
    o.renderer->flush();
    o.submitted_order = o.graphics->clock;
    return 1;
}

}

TEST(rt_ping_pong_and_clear_rects_match_original_callbacks) {
    Harness h;
    CHECK(h.scheduler.draw() >= 0);
    CHECK_EQ(h.graphics.draws.size(), 4u);
    const std::vector<u32> target_seq{0, 12, 13, 12, 13, 0};
    CHECK(h.graphics.selected == target_seq);
    const u32 out[] = {13, 12, 13, 0}, src[] = {12, 13, 12, 13};
    for (u32 i = 0; i < 4; ++i) {
        CHECK_EQ(h.graphics.draws[i].target, out[i]);
        CHECK_EQ(h.graphics.draws[i].texture, src[i]);
    }
    CHECK_EQ(h.graphics.clears.size(), 6u);
    const u32 clear_targets[] = {0, 12, 13, 12, 13, 0};
    const bool full[] = {true, false, false, false, false, true};
    const u32 colors[] = {0xffffffff, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000};
    for (u32 i = 0; i < 6; ++i) {
        CHECK_EQ(h.graphics.clears[i].target, clear_targets[i]);
        CHECK_EQ(h.graphics.clears[i].has_rect, !full[i]);
        CHECK_EQ(h.graphics.clears[i].color, colors[i]);
        if (!full[i]) {
            CHECK_EQ(h.graphics.clears[i].rect.x, 116u);
            CHECK_EQ(h.graphics.clears[i].rect.y, 2u);
            CHECK_EQ(h.graphics.clears[i].rect.width, 408u);
            CHECK_EQ(h.graphics.clears[i].rect.height, 476u);
        }
    }
    CHECK_EQ(h.manager.registry.count(), 0u);
}

TEST(compositor_vm_layers_match_supervisor_assignment) {
    Harness h;
    const i32 expected[] = {29, 30, 30, 20};
    for (u32 i = 0; i < 4; ++i)
        CHECK_EQ(h.targets.compositor_layer(i), expected[i]);
    CHECK(h.scheduler.update() >= 0);
    for (u32 i = 0; i < 4; ++i)
        CHECK_EQ(h.targets.compositor_layer(i), expected[i]);
    CHECK(h.scheduler.draw() >= 0);
    for (u32 i = 0; i < 4; ++i)
        CHECK_EQ(h.targets.compositor_layer(i), expected[i]);
}

TEST(stage_camera_does_not_mirror_view_y_column) {
    StageCamera s;
    s.position = {100, 50, 200};
    s.eye_offset = {-30, 10, -5};
    s.direction = {0.008f, -0.03f, 0.999f};
    s.up = {0, 1, 0};
    s.fov = 0.63f;
    const AnmCamera camera = stage_camera(s, {116, 2, 408, 476, 0, 1});
    const Vec3 eye{100 - 30.f, 50 + 10.f, 200 - 5.f};
    const Matrix4 direct =
        scene_look_at(eye, {eye.x + s.direction.x, eye.y + s.direction.y, eye.z + s.direction.z}, s.up);
    for (u32 i = 0; i < 16; ++i)
        CHECK_EQ(camera.view.m[i], direct.m[i]);
    CHECK(camera.view.m[5] > 0.99f);
    const float cot = camera.projection.m[5];
    near(camera.projection.m[0], float(float(cot) / float(float(408.f) / float(476.f))));
}

TEST(hud_strip_at_layer22_slot_50_lands_on_render_target_not_screen) {
    Harness h;
    AnmVm strip;
    CHECK(h.manager.bind_template(strip, 0, 82));
    CHECK(h.manager.tick_instance(strip) >= 0);
    strip.visual.flags = 7;
    strip.visual.render_flags = 0x8000;
    strip.visual.layer = 22;
    strip.visual.color = 0xffffffff;
    strip.visual.scale = {1, 1};
    strip.visual.secondary_scale = {1, 1};
    strip.visual.size = {0, 0};
    strip.visual.sprite_size = {224, 480};
    strip.variables.position = {527.5f, 239.5f, 0};
    StripOwner owner{&h.renderer, &h.views, &strip, 0, &h.graphics};
    FrameCallback callback;
    callback.owner = &owner;
    callback.enabled = true;
    callback.run = draw_strip;
    CHECK(h.scheduler.add(callback, FramePass::Draw, 50) >= 0);
    CHECK(h.scheduler.draw() >= 0);
    h.scheduler.remove(callback);
    CHECK_EQ(owner.submitted_order > 0u, true);
    CHECK_EQ(h.graphics.draws.size(), 5u);
    const auto& strip_draw = h.graphics.draws[3];
    CHECK_EQ(strip_draw.target, 13u);
    near(strip_draw.vertices[0].position.x, 415.5f);
    near(strip_draw.vertices[1].position.x, 639.5f);
    const auto& compose = h.graphics.draws.back();
    CHECK_EQ(compose.target, 0u);
    near(compose.vertices[0].uv.x, 128.f / 640);
    near(compose.vertices[1].uv.x, 512.f / 640);
    near(compose.vertices[0].position.x, 31.5f);
    near(compose.vertices[1].position.x, 415.5f);
    const auto map = [&](float rt_x) { return 31.5f + (rt_x - 128.f) / (512.f - 128.f) * (415.5f - 31.5f); };
    near(map(415.5f), 319.0f);
    near(map(512.f), 415.5f);
}

TEST(hud_strip_at_original_layer22_priority_55_lands_on_final_screen) {
    Harness h;
    AnmVm strip;
    CHECK(h.manager.bind_template(strip, 0, 82));
    CHECK(h.manager.tick_instance(strip) >= 0);
    strip.visual.flags = 7;
    strip.visual.render_flags = 0x8000;
    strip.visual.layer = 22;
    strip.visual.color = 0xffffffff;
    strip.visual.scale = {1, 1};
    strip.visual.secondary_scale = {1, 1};
    strip.visual.size = {0, 0};
    strip.visual.sprite_size = {224, 480};
    strip.variables.position = {527.5f, 239.5f, 0};
    StripOwner owner{&h.renderer, &h.views, &strip, 0, &h.graphics};
    FrameCallback callback;
    callback.owner = &owner;
    callback.enabled = true;
    callback.run = draw_strip;
    CHECK(h.scheduler.add(callback, FramePass::Draw, 55) >= 0);
    CHECK(h.scheduler.draw() >= 0);
    h.scheduler.remove(callback);
    CHECK_EQ(h.graphics.draws.size(), 5u);
    const auto& strip_draw = h.graphics.draws.back();
    CHECK_EQ(strip_draw.target, 0u);
    near(strip_draw.vertices[0].position.x, 415.5f);
    near(strip_draw.vertices[1].position.x, 639.5f);
    const auto& screen_clear = h.graphics.clears.back();
    CHECK_EQ(screen_clear.has_rect, false);
    CHECK_EQ(screen_clear.target, 0u);
    CHECK(screen_clear.order < strip_draw.order);
    CHECK(h.graphics.draws[3].target == 0u);
    CHECK(h.graphics.draws[3].order < strip_draw.order);
}

TEST(real_schedule_draws_layer22_after_compose_on_final_screen) {
    Harness h;
    const u32 handle = h.manager.create(0, 82, 22, 0, Vec3{}, 0.f, false);
    CHECK(handle != 0);
    AnmVm* strip = h.manager.registry.find(handle);
    CHECK(strip != nullptr);
    strip->visual.layer = 22;
    strip->visual.flags |= 0x100000;
    CHECK(h.manager.update(false));
    CHECK(h.manager.update(true));
    CHECK_EQ(strip->visual.layer, 22);
    strip->visual.flags = 7;
    strip->visual.render_flags = 0x8000;
    strip->visual.color = 0xffffffff;
    strip->visual.scale = {1, 1};
    strip->visual.secondary_scale = {1, 1};
    strip->visual.size = {0, 0};
    strip->visual.sprite_size = {224, 480};
    strip->variables.position = {527.5f, 239.5f, 0};
    struct Services final : AnmDrawServices {
        ScreenViews* views = nullptr;
        bool camera(DrawCamera index, bool refresh) override { return views->camera(index, refresh); }
    } services;
    services.views = &h.views;
    AnmDrawSchedule schedule(h.scheduler, h.manager, h.renderer, h.graphics, services);
    CHECK(schedule.error.empty());
    CHECK(h.scheduler.draw() >= 0);
    CHECK_EQ(h.graphics.draws.size(), 5u);
    const auto& strip_draw = h.graphics.draws.back();
    CHECK_EQ(strip_draw.target, 0u);
    near(strip_draw.vertices[0].position.x, 415.5f);
    near(strip_draw.vertices[1].position.x, 639.5f);
    const auto& screen_clear = h.graphics.clears.back();
    CHECK_EQ(screen_clear.has_rect, false);
    CHECK_EQ(screen_clear.target, 0u);
    CHECK(screen_clear.order < strip_draw.order);
    CHECK(h.graphics.draws[3].target == 0u);
    CHECK(h.graphics.draws[3].order < strip_draw.order);
}

TEST(spell_name_layer20_preserves_intermediate_right_edge_before_final_crop) {
    Harness h;
    struct Services final : AnmDrawServices {
        ScreenViews& views;
        explicit Services(ScreenViews& v) : views(v) {}
        bool camera(DrawCamera index, bool refresh) override { return views.camera(index, refresh); }
    } services{h.views};
    AnmDrawSchedule schedule(h.scheduler, h.manager, h.renderer, h.graphics, services);
    const auto handle = h.manager.create(0, 74);
    auto* name = h.manager.registry.find(handle);
    CHECK(name);
    if (!name) return;
    CHECK(h.manager.update());
    name->visual.flags |= 0x100000u;
    name->visual.color = 0xffffffffu;
    name->visual.scale = {1, 1};
    name->variables.position = {524, 384, 0};
    CHECK(h.scheduler.draw() >= 0);
    h.renderer.flush();
    const RecordedDraw* submission = nullptr;
    for (const auto& draw : h.graphics.draws)
        if (draw.texture == 10) submission = &draw;
    CHECK(submission);
    if (!submission) return;
    CHECK_EQ(submission->target, 13u);
    CHECK_EQ(submission->viewport.x, 116u);
    CHECK_EQ(submission->viewport.width, 408u);
    CHECK_EQ(submission->viewport.x + submission->viewport.width, 524u);
    near(submission->vertices[1].position.x, 523.5f);
}

TEST(distortion_layers_are_composed_once_into_opposite_render_targets) {
    Harness h;
    struct Services final : AnmDrawServices {
        ScreenViews& views;
        explicit Services(ScreenViews& v) : views(v) {}
        bool camera(DrawCamera index, bool refresh) override { return views.camera(index, refresh); }
    } services{h.views};
    AnmDrawSchedule schedule(h.scheduler, h.manager, h.renderer, h.graphics, services);
    const auto fog = h.manager.create(0, 82, 29, 0);
    const auto overlay = h.manager.create(0, 83, 30, 0);
    CHECK(fog && overlay);
    CHECK(h.manager.update());
    CHECK(h.scheduler.draw() >= 0);
    h.renderer.flush();
    std::vector<const RecordedDraw*> quads;
    for (const auto& draw : h.graphics.draws) {
        CHECK_EQ(draw.vertices.size() % 6, 0u);
        for (size_t i = 0; i < draw.vertices.size(); i += 6)
            quads.push_back(&draw);
    }
    CHECK_EQ(quads.size(), 6u);
    if (quads.size() != 6) return;
    const u32 targets[] = {13, 13, 12, 12, 13, 0};
    const u32 sources[] = {12, 12, 13, 13, 12, 13};
    for (u32 i = 0; i < 6; ++i) {
        CHECK_EQ(quads[i]->target, targets[i]);
        CHECK_EQ(quads[i]->texture, sources[i]);
    }
    CHECK(quads[1]->order < h.graphics.clears[3].order);
    CHECK(quads[3]->order < h.graphics.clears[4].order);
}

TEST(final_original_anm84_maps_center_and_both_edges) {
    Harness h;
    CHECK(h.scheduler.draw() >= 0);
    const auto& d = h.graphics.draws.back();
    near(d.vertices[0].uv.x, 128.f / 640);
    near(d.vertices[1].uv.x, 512.f / 640);
    near(d.vertices[0].position.x, 31.5f);
    near(d.vertices[1].position.x, 415.5f);
    CHECK_EQ(h.env.screen_offsets[2], 320);
    CHECK_EQ(h.env.screen_offsets[3], 16);
    near(h.renderer.offset.x, 0);
    near(h.renderer.offset.y, 0);
}

TEST(repeat_presentation_does_not_mutate_game_origin) {
    Harness h;
    CHECK(h.scheduler.update() >= 0);
    CHECK(h.scheduler.draw() >= 0);
    const auto first = h.graphics.draws;
    h.graphics.draws.clear();
    CHECK(h.scheduler.draw() >= 0);
    CHECK_EQ(first.size(), h.graphics.draws.size());
    for (u32 i = 0; i < first.size(); ++i)
        for (u32 j = 0; j < first[i].vertices.size(); ++j) {
            const auto& a = first[i].vertices[j];
            const auto& b = h.graphics.draws[i].vertices[j];
            CHECK(std::memcmp(&a, &b, sizeof(a)) == 0);
        }
    CHECK_EQ(h.env.screen_offsets[2], 320);
}

TEST(real_target_failure_is_propagated_not_stale_screen_success) {
    Harness h;
    h.graphics.reject_targets = true;
    CHECK(h.scheduler.draw() < 0);
    CHECK(!h.targets.error.empty());
    CHECK(h.graphics.draws.empty());
}

int main() { return th13test::run_all("playfield RT schedule and final screen domain"); }
