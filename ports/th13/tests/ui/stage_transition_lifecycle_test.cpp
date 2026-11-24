#include "../ecl/ecl_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/FrameScheduler.hpp"
#include "game/RunScene.hpp"

#include <cstring>
#include <fstream>
#include <iterator>
#include <map>
#include <string>

using namespace th13;
using namespace th13::test;

namespace {

std::vector<u8> synthetic_stage_ecl(const char* anim_a, const char* anim_b) {
    std::vector<u8> main;
    th13test::emit(main, 0, 23, 0, 0xff, 0, {600});
    th13test::emit(main, 0, 10, 0, 0xff, 0, {});
    return th13test::build_scpt({{"main", main}}, {anim_a, anim_b});
}

std::vector<u8> synthetic_std() {
    std::vector<u8> out(0x110, 0);
    auto put16 = [&](u32 at, u16 v) { std::memcpy(out.data() + at, &v, 2); };
    auto put32 = [&](u32 at, u32 v) { std::memcpy(out.data() + at, &v, 4); };
    auto putf = [&](u32 at, float v) { std::memcpy(out.data() + at, &v, 4); };
    put16(0, 1);
    put16(2, 1);
    put32(4, 0xd0);
    put32(8, 0xe4);
    const char* name = "scenery.anm";
    std::memcpy(out.data() + 0x10, name, std::strlen(name));
    put32(0x90, 0x94);
    put32(0x94, 0x00010000);
    putf(0x98, 0.f);
    putf(0x9c, 0.f);
    putf(0xa0, 0.f);
    putf(0xa4, 64.f);
    putf(0xa8, 64.f);
    putf(0xac, 64.f);
    put16(0xb0, 0);
    put16(0xb2, 28);
    put16(0xb4, 0);
    put16(0xb6, 0);
    putf(0xb8, 0.f);
    putf(0xbc, 0.f);
    putf(0xc0, 0.f);
    putf(0xc4, 64.f);
    putf(0xc8, 64.f);
    put32(0xcc, 0x0004ffff);
    put32(0xd0, 0);
    putf(0xd4, 0.f);
    putf(0xd8, 0.f);
    putf(0xdc, 0.f);
    put32(0xe0, 0xffffffff);
    put32(0xe4, 0);
    put16(0xe8, 2);
    put16(0xea, 20);
    putf(0xec, 10.f);
    putf(0xf0, 20.f);
    putf(0xf4, 30.f);
    put32(0xf8, 0);
    put16(0xfc, 0);
    put16(0xfe, 8);
    for (u32 at = 0x100; at < 0x110; at += 4)
        put32(at, 0xffffffff);
    return out;
}

std::vector<u8> read_asset(const char* name) {
    std::ifstream f(std::string("artifacts/reference/raw/") + name, std::ios::binary);
    return {std::istreambuf_iterator<char>(f), {}};
}

struct TransitionHost final : RunSceneServices {
    AnmManager& manager;
    ZunGraphics* graphics = nullptr;
    std::map<std::string, std::vector<u8>> assets;
    explicit TransitionHost(AnmManager& manager) : manager(manager) {}
    bool hud_sound(i32) override { return true; }
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_pause() override { return true; }
    bool request_game_over() override { return true; }
    bool camera(DrawCamera, bool) override { return true; }
    bool hud_text(const HudTextDraw&) override { return true; }
    bool read_asset(const std::string& name, std::vector<u8>& bytes, bool required = true) override {
        (void)required;
        const auto found = assets.find(name);
        if (found == assets.end()) return false;
        bytes = found->second;
        return true;
    }
    ZunGraphics& stage_graphics() override { return *graphics; }
    bool preload_scenery_animation(i32 bank, const std::string& name) override {
        if (manager.resource(bank)) return true;
        std::vector<u8> bytes;
        return read_asset(name, bytes) && manager.load(bank, bytes.data(), u32(bytes.size()));
    }
};

struct NullGraphics final : ZunGraphics {
    touhou::graphics::PipelineState& pipeline() override { return pipeline_state; }
    u32 texture(const AnmResource&, u32) override { return 0; }
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

struct TransitionHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    FrameScheduler scheduler;
    TransitionHost host{manager};
    std::unique_ptr<RunScene> scene;
    FrameCallback animation_main{}, animation_alternate{};

    TransitionHarness() {
        CHECK(load_bank(manager, 3, synthetic_anm(8, trivial_scripts(176))));
        CHECK(load_bank(manager, 2, synthetic_anm(8, trivial_scripts(46))));
        CHECK(load_bank(manager, 4, synthetic_anm(8, trivial_scripts(4))));
        CHECK(load_bank(manager, 5, synthetic_anm(8, trivial_scripts(1))));
        CHECK(load_bank(manager, 6, synthetic_anm(64, trivial_scripts(35))));
        CHECK(load_bank(manager, 26, synthetic_anm(8, trivial_scripts(3))));
        CHECK(load_bank(manager, 8, synthetic_anm(8, trivial_scripts(57))));
        CHECK(load_bank(manager, 25, synthetic_anm(8, trivial_scripts(5))));
        host.assets["st01.ecl"] = synthetic_stage_ecl("stage_a.anm", "stage_b.anm");
        host.assets["st02.ecl"] = synthetic_stage_ecl("stage2_a.anm", "stage2_b.anm");
        host.assets["stage_a.anm"] = synthetic_anm(8, trivial_scripts(1));
        host.assets["stage_b.anm"] = synthetic_anm(8, trivial_scripts(1));
        host.assets["stage2_a.anm"] = synthetic_anm(8, trivial_scripts(1));
        host.assets["stage2_b.anm"] = synthetic_anm(8, trivial_scripts(1));
        host.assets["st01.std"] = synthetic_std();
        host.assets["st02.std"] = synthetic_std();
        host.assets["scenery.anm"] = synthetic_anm(8, trivial_scripts(1));
        host.graphics = &graphics;
        StagePresentation::Banks banks{4, 5, 6};
        scene = std::make_unique<RunScene>(progress, player, score, manager, renderer, environment, host, scheduler,
                                           banks, 3, 2, 7, "st01.std");
        scene->hud.stage_logo_bank = 26;
        const bool initialized = scene->initialize();
        if (!initialized) std::printf("transition harness init error: %s\n", scene->error.c_str());
        CHECK(initialized);
    }

    void register_animation_updates() {
        animation_main.owner = animation_alternate.owner = this;
        animation_main.enabled = animation_alternate.enabled = true;
        animation_main.run = [](void* p) -> i32 {
            return static_cast<TransitionHarness*>(p)->manager.update(true) ? 1 : 5;
        };
        animation_alternate.run = [](void* p) -> i32 {
            return static_cast<TransitionHarness*>(p)->manager.update(false) ? 1 : 5;
        };
        CHECK(scheduler.add(animation_main, FramePass::Update, 9) >= 0);
        CHECK(scheduler.add(animation_alternate, FramePass::Update, 34) >= 0);
    }
};

u32 transition_visual_delta(bool with_prayer, u32* game_delta = nullptr) {
    TransitionHarness h;
    h.progress.stage = 2;
    h.progress.stage_frame = 11975;
    CHECK(h.scene->prepare_transition(26, "st02.std"));
    h.register_animation_updates();
    u32 prayer = 0;
    if (with_prayer) {
        const auto ascii = read_asset("ascii.anm");
        CHECK(!ascii.empty());
        CHECK(load_bank(h.manager, 27, ascii));
        prayer = h.manager.create(27, 17, -1, 0, {480, 392, 0});
        CHECK(prayer != 0);
    }
    const u32 visual_before = h.random.index();
    const u32 game_before = h.scene->battle.sim.random.index();
    for (int tick = 0; tick < 150; ++tick)
        CHECK(h.scheduler.update() >= 0);
    if (game_delta) *game_delta = h.scene->battle.sim.random.index() - game_before;
    if (prayer) {
        CHECK(h.manager.retire(prayer));
        CHECK(h.manager.update(true));
        CHECK(h.manager.update(false));
        CHECK(h.manager.registry.find(prayer) == nullptr);
    }
    return h.random.index() - visual_before;
}

}

TEST(transition_destroys_old_stage_managers_at_original_boundary) {
    TransitionHarness h;
    auto& sim = h.scene->battle.sim;
    auto& items = h.scene->battle.item_manager;

    sim.enemies.clear();
    for (u32 serial : {11u, 22u}) {
        auto enemy = std::make_unique<StageEnemy>();
        enemy->serial = serial;
        sim.enemies.push_back(std::move(enemy));
    }
    std::vector<u32> retired;
    sim.retire_sink = [&](u32 serial) { retired.push_back(serial); };
    sim.bullets.push_back(StageBullet{});
    items.spawn_value_item(0, 0.f, 0.f, 0.f, 0.5f, false);
    CHECK(items.spirits()[0].alive);
    const bool synced = h.scene->presentation->sync(h.scene->battle, 0);
    if (!synced) std::printf("presentation sync error: %s\n", h.scene->presentation->error().c_str());
    CHECK(synced);
    CHECK(h.scene->presentation->sync_item_spirits_phase(h.scene->battle));
    CHECK(h.scene->presentation->debug_report().find("spirits=1") != std::string::npos);
    items.spawn_item(0.f, 0.f, 0.f, 1, 0.f, 1.f);
    bool any_item = false;
    for (const auto& item : items.items())
        any_item |= item.alive;
    CHECK(any_item);

    h.progress.stage = 2;
    h.progress.stage_frame = 11975;
    const i32 old_frame = h.progress.stage_frame;
    CHECK(h.scene->prepare_transition(26, "st02.std"));

    CHECK(h.scene->transition_wait);
    CHECK(sim.enemies.empty());
    CHECK(sim.bullets.empty());
    CHECK_EQ(retired.size(), 2u);
    if (retired.size() == 2u) {
        CHECK_EQ(retired[0], 11u);
        CHECK_EQ(retired[1], 22u);
    }
    for (const auto& item : items.items())
        CHECK(!item.alive);
    for (const auto& spirit : items.spirits())
        CHECK(!spirit.alive);
    CHECK(h.scene->presentation->debug_report().find("spirits=0") != std::string::npos);
    CHECK(h.scene->presentation->sync(h.scene->battle, 0));
    CHECK(h.scene->presentation->debug_report().find("spirits=0") != std::string::npos);

    CHECK_EQ(h.progress.stage, 2);
    CHECK_EQ(h.progress.stage_frame, old_frame);
    CHECK(h.scene->scenery != nullptr);
    CHECK(h.scene->scenery->ready());
    CHECK(h.scene->previous_scenery != nullptr);
    CHECK_EQ(h.scene->previous_scenery_bank, 7);
}

TEST(transition_caption_contributes_original_912_visual_index) {
    u32 game_with = 0, game_without = 0;
    const u32 without = transition_visual_delta(false, &game_without);
    const u32 with = transition_visual_delta(true, &game_with);
    CHECK_EQ(with - without, 912u);
    CHECK_EQ(game_with, 0u);
    CHECK_EQ(game_without, 0u);
}

TEST(next_stage_activation_resets_frame_and_loads_stage_two_assets) {
    TransitionHarness h;
    h.score.score = 123456;
    h.player.power = 321;
    h.progress.stage = 2;
    h.progress.stage_frame = 11975;
    CHECK(h.scene->prepare_transition(26, "st02.std"));

    h.progress.stage_frame = 0;
    h.progress.scene_flags = 0;
    h.progress.rate = 1;
    StagePresentation::Banks banks{4, 5, 6};
    auto next = std::make_unique<RunScene>(h.progress, h.player, h.score, h.manager, h.renderer, h.environment, h.host,
                                           h.scheduler, banks, 3, 2, 7, "st02.std");
    next->preserve_presentation_from(*h.scene);
    const bool initialized = next->initialize();
    if (!initialized) std::printf("next stage init error: %s\n", next->error.c_str());
    CHECK(initialized);

    CHECK_EQ(h.progress.stage_frame, 0);
    CHECK_EQ(h.progress.stage, 2);
    CHECK_EQ(h.score.score, 123456);
    CHECK_EQ(h.player.power, 321);
    CHECK_EQ(next->battle.sim.anim_list.size(), 2u);
    if (next->battle.sim.anim_list.size() == 2u) {
        CHECK_EQ(next->battle.sim.anim_list[0], std::string("stage2_a.anm"));
        CHECK_EQ(next->battle.sim.anim_list[1], std::string("stage2_b.anm"));
    }
    CHECK(next->scenery != nullptr);
    CHECK(next->scenery->ready());
    for (const auto& enemy : next->battle.sim.enemies) {
        CHECK(enemy->serial != 11u);
        CHECK(enemy->serial != 22u);
    }
}

TEST(next_stage_activation_unlinks_every_outgoing_scene_callback) {
    TransitionHarness h;
    h.register_animation_updates();
    h.progress.stage = 2;
    h.progress.stage_frame = 11975;
    CHECK(h.scene->prepare_transition(26, "st02.std"));
    h.progress.stage_frame = 0;
    h.progress.scene_flags = 0;
    h.progress.rate = 1;
    StagePresentation::Banks banks{4, 5, 6};
    auto next = std::make_unique<RunScene>(h.progress, h.player, h.score, h.manager, h.renderer, h.environment, h.host,
                                           h.scheduler, banks, 3, 2, 7, "st02.std");
    next->preserve_presentation_from(*h.scene);
    RunScene* outgoing = h.scene.release();
    outgoing->~RunScene();
    std::memset(static_cast<void*>(outgoing), 0xa5, sizeof(RunScene));
    CHECK(next->initialize());
    for (int tick = 0; tick < 4; ++tick)
        CHECK(h.scheduler.update() >= 0);
    CHECK(h.scheduler.draw() >= 0);
    next.reset();
    ::operator delete(static_cast<void*>(outgoing));
}

int main() { return th13test::run_all("跨关过渡生命周期"); }
