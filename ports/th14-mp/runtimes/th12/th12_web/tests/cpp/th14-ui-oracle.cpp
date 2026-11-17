#include "AnmGeometry.hpp"
#include "ApplicationCaptureTh14.hpp"
#include "GameConfig.hpp"
#include "LaserState.hpp"
#include "PauseMenu.hpp"
#include "TitleMenu.hpp"

#include <cassert>
#include <cstdio>

using namespace th11;

namespace {
struct CaptureSession {
    struct Dialogue { bool active = true; };
    struct Battle { Dialogue* dialogue; };
    Battle* battle;
};

struct StripObserver final : ZunGraphics {
    touhou::graphics::PipelineState state{};
    std::vector<AnmVertex> vertices;

    touhou::graphics::PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource&, u32) override { return 1; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override {}
    void triangles(u32, const void*, u32) override { assert(false); }
    void primitives(touhou::graphics::Topology topology, u32 count, const void* data, u32 stride) override {
        assert(topology == touhou::graphics::Topology::Strip && stride == sizeof(AnmVertex));
        const auto* first = static_cast<const AnmVertex*>(data);
        vertices.assign(first, first + count + 2);
    }
};

void print_sounds(const char* action, const std::vector<i32>& sounds) {
    std::printf("%s", action);
    for (i32 sound : sounds) std::printf(" %d", sound);
    std::puts("");
}

void observe_menus() {
    AnmManager animations;
    AnmResource unused;
    ScoreFile scores;

    PauseMenu pause(animations, unused, unused, scores);
    pause.state = 18;
    pause.timer.set(15, &animations.rate);
    pause.name_cursor.count = 91;
    pause.name_cursor.wrap = 1;
    assert(pause.update(128, 0));
    print_sounds("score_move", pause.sounds);
    assert(pause.update(1, 0));
    print_sounds("score_accept", pause.sounds);

    const char* labels[] = {"pause_save", "game_over_save", "practice_save"};
    const i32 states[] = {10, 17, 24};
    for (i32 i = 0; i < 3; ++i) {
        pause.state = states[i];
        pause.name_cursor.select(90);
        pause.timer.set(15, &animations.rate);
        pause.save_requested = false;
        assert(pause.update(1, 0) && pause.save_requested);
        print_sounds(labels[i], pause.sounds);
    }

    TitleMenu title(animations, unused, unused, unused, scores);
    title.screen = TitleScreen::ReplaySave;
    title.substate = 3;
    title.name_cursor.count = 91;
    title.name_cursor.select(90);
    assert(title.update(1, 0) && title.replay_save_requested);
    print_sounds("title_save", title.sounds);

    for (i32 difficulty = 0; difficulty < 5; ++difficulty) {
        for (i32 used = 0; used <= 5; ++used) {
            PauseMenu end(animations, unused, unused, scores);
            end.stage = difficulty == 4 ? 7 : 1;
            end.difficulty = difficulty;
            end.continues = used;
            // Enter the replay list from an existing result menu and cancel it.
            end.state = 16;
            end.cursor.count = 5;
            end.cursor.select(2);
            end.cursor.push();
            end.cursor.count = 25;
            end.cursor.select(0);
            end.timer.set(15, &animations.rate);
            assert(end.update(2, 0) && end.state == 14);
            assert(end.update(16, 0) && end.cursor.selected == 1);
            assert(end.update(16, 0));
            const bool selectable = end.cursor.selected == 0;

            end.state = 19;
            end.cursor.select(0);
            end.timer.set(15, &animations.rate);
            assert(end.update(0, 0));
            const bool continued = end.action == PauseAction::Continue;
            std::printf("continue_%d_%d %d %d\n", difficulty, used, selectable, continued);
        }
    }
}

void observe_geometry() {
    CaptureSession::Dialogue message;
    CaptureSession::Battle battle{&message};
    StripObserver graphics;
    AnmRenderer renderer(graphics);
    AnmResource resource;
    AnmSprite sprite{};
    Th14LaserCurve curve;
    curve.base.width = 14;
    curve.body.initialize();
    curve.body.sprite = &sprite;
    curve.body.resource = &resource;
    curve.body.uv[0].y = .25f;
    curve.body.uv[2].y = .5f;
    curve.path.samples.resize(2);
    curve.path.samples[0].position = {-50, 80, 0};
    curve.path.samples[1].position = {50, 80, 0};
    curve.vertices.resize(4);
    curve.curve_timer.current = 3;

    for (u32 width : {640u, 960u, 1280u}) {
        renderer.render_width = width;
        renderer.render_height = width * 3 / 4;
        const auto space = renderer.render_space_th14();

        for (i32 dialogue : {0, 1}) {
            const auto copy = sdl::th14_immediate_capture_source(CaptureSession{dialogue ? &battle : nullptr}, space.scale);
            std::printf("capture_%u_%d %d", width, dialogue, copy.text_intermediate);
            for (i32 value : copy.rectangle) std::printf(" %d", value);
            std::puts("");
        }

        assert(laser_th14_curve_draw(curve, renderer));
        assert(graphics.vertices.size() == 4);
        std::printf("curve_%u", width);
        for (const auto& v : graphics.vertices) {
            std::printf(" %u %u %u %u", float_bits(v.position.x), float_bits(v.position.y),
                        float_bits(v.uv.x), float_bits(v.uv.y));
        }
        std::puts("");

        AnmVm vm;
        vm.initialize();
        // The authored size comes from the display-matched atlas.
        vm.sprite_size = {10 * space.scale, 16 * space.scale};
        Vec3 quad[4];
        auto dimensions = [&](const char* label) {
            std::printf("%s_%u %u %u\n", label, width,
                        float_bits(quad[1].x - quad[0].x), float_bits(quad[2].y - quad[0].y));
        };
        assert(anm_th14_ascii_quad_positions(vm, quad, space));
        dimensions("queued_glyph");
        vm.flags2 = 3u << 19;
        assert(anm_th14_ascii_quad_positions(vm, quad, space));
        dimensions("popup_glyph");
        vm.file_index = 2;
        vm.script_index = 1;
        assert(anm_th14_draw_quad_positions(vm, quad, space));
        dimensions("spell_labels");
        vm.script_index = 2;
        assert(anm_th14_draw_quad_positions(vm, quad, space));
        dimensions("boss_timer");
        vm.script_index = 4;
        assert(anm_th14_draw_quad_positions(vm, quad, space));
        dimensions("spell_award");
        vm.file_index = 5;
        vm.sprite_size = {10, 16};
        assert(anm_th14_draw_quad_positions(vm, quad, space));
        dimensions("other_sprite");
    }
}

void observe_display() {
    GameConfig config;
    for (u8 mode = 0; mode < 6; ++mode) {
        auto bytes = config.bytes;
        bytes[0x1f] = mode;
        assert(config.open(bytes.data(), bytes.size()));
        std::printf("display_%u %u %u %u\n", u32(mode), config.render_width(), config.render_height(), u32(config.bytes[0x1f]));
    }
}
}

int main() {
    observe_menus();
    observe_geometry();
    observe_display();
}
