#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/EclResource.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"

#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct NullGraphics final : ZunGraphics {
    touhou::graphics::PipelineState& pipeline() override { return pipeline_state; }
    u32 texture(const AnmResource&, u32) override { return 1; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override {}
    void triangles(u32, const void*, u32) override {}
    void primitives(touhou::graphics::Topology, u32, const void*, u32) override {}
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}

private:
    touhou::graphics::PipelineState pipeline_state;
};

std::vector<u8> read_asset(const char* name) {
    std::ifstream f(std::string("artifacts/reference/raw/") + name, std::ios::binary);
    return {std::istreambuf_iterator<char>(f), {}};
}

struct UnusedProvider final : EclResourceProvider {
    bool read(const std::string&, std::vector<u8>&) override { return false; }
    bool animation(u32, const std::string&) override { return false; }
};

u32 count_script(AnmManager& manager, i32 script) {
    u32 count = 0;
    for (u32 i = 0; i < manager.registry.ordered_count(false); ++i) {
        const AnmVm* vm = manager.registry.find(manager.registry.ordered_handle(false, i));
        if (vm && vm->source_script == script) ++count;
    }
    return count;
}

u32 satellite_count(AnmManager& manager) {
    u32 count = 0;
    for (u32 i = 0; i < manager.registry.ordered_count(false); ++i) {
        AnmVm* vm = manager.registry.find(manager.registry.ordered_handle(false, i));
        if (vm && vm->source_script == 15) count += u32(manager.registry.children(*vm).size());
    }
    return count;
}

std::vector<Vec3> option_root_positions(AnmManager& manager) {
    std::vector<Vec3> positions;
    for (u32 i = 0; i < manager.registry.ordered_count(false); ++i) {
        AnmVm* vm = manager.registry.find(manager.registry.ordered_handle(false, i));
        if (vm && vm->source_script == 15) positions.push_back(vm->visual.translation);
    }
    return positions;
}

struct OptionHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    StageBattle battle;
    SessionState progress;
    HudPlayerState player;
    UnusedProvider provider;
    std::unique_ptr<StagePresentation> presentation;
    bool ready = false;

    OptionHarness() {
        const auto pl03 = read_asset("pl03.anm");
        if (pl03.empty()) {
            CHECK(!"artifacts/reference/raw/pl03.anm missing");
            return;
        }
        if (!manager.load(6, pl03.data(), u32(pl03.size()))) {
            std::printf("pl03 load error: %s\n", manager.error.c_str());
            CHECK(false);
            return;
        }
        progress.character = 3;
        player.power = 293;
        player.max_power = 400;
        player.power_step = 100;
        if (!battle.prepare_player(provider, progress, player, nullptr, false)) {
            std::printf("player prepare error: %s\n", battle.error().c_str());
            CHECK(false);
            return;
        }
        battle.option_count = 2;
        for (size_t i = 0; i < 2; ++i) {
            battle.options[i].active = true;
            battle.options[i].x = i == 0 ? -20.f : 20.f;
            battle.options[i].y = 300.f;
        }
        presentation =
            std::make_unique<StagePresentation>(manager, renderer, StagePresentation::Banks{4, 5, 6, 8, 25}, 0);
        ready = true;
    }

    u32 frame() {
        const u32 before = random.index();
        CHECK(presentation->sync_player_options(battle));
        CHECK(manager.update(false));
        if (!presentation->error().empty()) std::printf("option harness error: %s\n", presentation->error().c_str());
        CHECK(presentation->error().empty());
        return random.index() - before;
    }

    u32 create_frame() { return frame(); }
};

u32 run_gate(bool hide, bool interrupt, std::vector<u32>* per_frame = nullptr, u32 frames = 150) {
    OptionHarness h;
    CHECK(h.ready);
    if (!h.ready) return 0;
    CHECK_EQ(h.create_frame(), 8u);
    CHECK_EQ(h.frame(), 0u);
    CHECK_EQ(h.frame(), 0u);
    if (per_frame) per_frame->assign(frames, 0u);
    u32 total = 0;
    for (u32 f = 0; f < frames; ++f) {
        if (f == 0 && hide) {
            CHECK(h.presentation->hide_player_options());
            CHECK(h.presentation->error().empty());
        }
        if (f >= 29 && interrupt) {
            CHECK(h.presentation->interrupt_player_options());
            CHECK(h.presentation->error().empty());
        }
        const u32 delta = h.frame();
        if (per_frame) (*per_frame)[f] = delta;
        total += delta;
    }
    return total;
}

}

TEST(hidden_option_tree_delays_satellite_draws_and_stops_after_frame_25) {
    std::vector<u32> per_frame;
    const u32 total = run_gate(true, true, &per_frame);
    CHECK_EQ(per_frame.size(), 150u);
    if (per_frame.size() != 150u) return;
    CHECK_EQ(per_frame[0], 0u);
    for (u32 f = 0; f < per_frame.size(); ++f) {
        const bool satellite = f >= 1 && f <= 25 && (f - 1) % 4 == 0;
        CHECK_EQ(per_frame[f], satellite ? 8u : 0u);
    }
    CHECK_EQ(total, 56u);
}

TEST(without_interrupt_hidden_tree_keeps_spawning_38_cycles_plus_248) {
    const u32 total = run_gate(true, false);
    CHECK_EQ(total, 304u);
    CHECK_EQ(total - 56u, 248u);
}

TEST(without_hide_first_companion_draws_on_frame_zero) {
    OptionHarness h;
    CHECK(h.ready);
    if (!h.ready) return;
    CHECK_EQ(h.create_frame(), 8u);
    CHECK_EQ(h.frame(), 0u);
    CHECK_EQ(h.frame(), 0u);
    std::vector<u32> per_frame(150, 0u);
    u32 total = 0;
    for (u32 f = 0; f < 150; ++f) {
        per_frame[f] = h.frame();
        total += per_frame[f];
    }
    CHECK_EQ(per_frame[0], 8u);
    for (u32 f = 0; f < 150; ++f)
        CHECK_EQ(per_frame[f], (f % 4 == 0) ? 8u : 0u);
    CHECK_EQ(total, 304u);
}

TEST(hidden_gate_pins_option_roots_to_player_position_not_stale_options) {
    OptionHarness h;
    CHECK(h.ready);
    if (!h.ready) return;
    CHECK_EQ(h.create_frame(), 8u);
    CHECK_EQ(h.frame(), 0u);
    CHECK_EQ(h.frame(), 0u);
    CHECK_EQ(h.battle.options[0].x, -20.f);
    CHECK_EQ(h.battle.options[0].y, 300.f);
    CHECK_EQ(h.battle.options[1].x, 20.f);
    h.battle.set_player_pos(64.f, 400.f);
    CHECK(h.presentation->hide_player_options());
    CHECK(h.presentation->error().empty());
    CHECK(h.presentation->sync_player_options(h.battle));
    CHECK(h.presentation->error().empty());
    const auto roots = option_root_positions(h.manager);
    CHECK_EQ(roots.size(), 2u);
    for (const auto& position : roots) {
        CHECK_EQ(position.x, 64.f);
        CHECK_EQ(position.y, 400.f);
    }
    CHECK_EQ(h.frame(), 0u);
    for (const auto& position : option_root_positions(h.manager)) {
        CHECK_EQ(position.x, 64.f);
        CHECK_EQ(position.y, 400.f);
    }
}

TEST(stage_activation_does_not_rebroadcast_label2_to_old_option_tree) {
    {
        OptionHarness h;
        CHECK(h.ready);
        if (!h.ready) return;
        CHECK_EQ(h.create_frame(), 8u);
        CHECK_EQ(h.frame(), 0u);
        CHECK_EQ(h.frame(), 0u);
        for (u32 f = 0; f < 40; ++f) {
            if (f == 0) CHECK(h.presentation->hide_player_options());
            if (f >= 29) CHECK(h.presentation->interrupt_player_options());
            const bool satellite = f >= 1 && f <= 25 && (f - 1) % 4 == 0;
            CHECK_EQ(h.frame(), satellite ? 8u : 0u);
        }
        CHECK_EQ(count_script(h.manager, 15), 0u);
        CHECK_EQ(count_script(h.manager, 16), 0u);
        CHECK_EQ(satellite_count(h.manager), 0u);
        const u32 before = h.random.index();
        CHECK(h.presentation->activate_player_effects(h.battle));
        CHECK(h.manager.update(false));
        const u32 activation = h.random.index() - before;
        CHECK_EQ(activation, 8u);
        CHECK_EQ(count_script(h.manager, 15), 2u);
        CHECK_EQ(count_script(h.manager, 16), 2u);
    }
    {
        OptionHarness h;
        CHECK(h.ready);
        if (!h.ready) return;
        CHECK_EQ(h.create_frame(), 8u);
        CHECK_EQ(h.frame(), 0u);
        CHECK_EQ(h.frame(), 0u);
        for (u32 f = 0; f < 40; ++f)
            CHECK_EQ(h.frame(), (f % 4 == 0) ? 8u : 0u);
        const u32 satellites = satellite_count(h.manager);
        CHECK_EQ(satellites, 6u);
        const u32 before = h.random.index();
        CHECK(h.presentation->activate_player_effects(h.battle));
        CHECK(h.manager.update(false));
        const u32 label2 = h.random.index() - before;
        CHECK_EQ(label2, satellites * 4u);
        CHECK_EQ(label2, 24u);
        CHECK_EQ(count_script(h.manager, 15), 2u);
    }
}

int main() { return th13test::run_all("option hidden lifecycle"); }
