// H-SHOT-HIT-480: 004464d0 tests the preceding ANM quad before writing the
// current shot position. Exercise the public battle snapshot and real ANM path.
#include "../ecl/ecl_test_fixtures.hpp"
#include "../ui/ui_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"

#include <algorithm>
#include <array>
#include <cstring>

using namespace th13;

namespace {
using Quad = std::array<Vec3, 4>;

Quad rectangle(float left, float top, float right, float bottom) {
    return {{{left, top, 0}, {right, top, 0},
             {left, bottom, 0}, {right, bottom, 0}}};
}

class Provider final : public EclResourceProvider {
public:
    Provider() {
        std::vector<u8> main;
        th13test::emit(main, 0, 23, 0, 0xff, 0, {600});
        th13test::emit(main, 0, 10, 0, 0xff, 0, {});
        stage = th13test::build_scpt({{"main", main}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& bytes) override {
        if (name != "culling.ecl") return false;
        bytes = stage;
        return true;
    }
    bool animation(u32, const std::string& name) override {
        animations->push_back(name);
        return true;
    }
    std::vector<std::string>* animations = nullptr;
    std::vector<u8> stage;
};

struct Host final : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};

struct Fixture {
    Provider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    GameInput idle;

    bool init() {
        provider.animations = &battle.sim.anim_list;
        player.power = 0;
        if (!battle.load(provider, "culling.ecl", 2, host, progress, player, score))
            return false;
        battle.sim.enemies.clear();
        return true;
    }
    static PlayerShot shot(u32 generation = 1, i32 age = 10) {
        PlayerShot shot;
        shot.alive = true;
        shot.x = 200;
        shot.y = 200;
        shot.angle = 0;
        shot.speed = 0;
        shot.script = 10;
        shot.generation = generation;
        shot.age = age;
        return shot;
    }
    void snapshot(size_t slot, const Quad& quad) {
        const auto& shot = battle.shots[slot];
        battle.set_player_shot_geometry(i32(slot), shot.age, shot.script,
                                        shot.generation, quad);
    }
};

i32 float_bits(float value) {
    i32 result;
    std::memcpy(&result, &value, sizeof result);
    return result;
}

// External synthetic texture plus real ANM script 10. Each VM tick adds 0.25
// to x scale, so renderer quad width exposes both rebinding and animation age.
std::vector<u8> animated_shot_bank() {
    using namespace th13::test;
    auto scripts = trivial_scripts(11);
    scripts[10] = {sprite(0), AnmIns{302, 0, {1}},
                   AnmIns{416, 0, {float_bits(0.25f), float_bits(0.0f)}},
                   wait(), stop()};
    auto bytes = synthetic_anm(1, scripts);
    u32 sprite_offset = 0;
    std::memcpy(&sprite_offset, bytes.data() + 64, sizeof sprite_offset);
    const float width = 8, height = 8;
    std::memcpy(bytes.data() + sprite_offset + 12, &width, sizeof width);
    std::memcpy(bytes.data() + sprite_offset + 16, &height, sizeof height);
    return bytes;
}

struct NullGraphics final : ZunGraphics {
    struct DrawnShot {
        u64 generation = 0;
        i32 animation_age = 0;
        Quad quad{};
        float width() const {
            float left = quad[0].x, right = left;
            for (const auto& point : quad) {
                left = std::min(left, point.x);
                right = std::max(right, point.x);
            }
            return right - left;
        }
    };
    touhou::graphics::PipelineState state;
    std::vector<DrawnShot> shots;
    touhou::graphics::PipelineState& pipeline() override { return state; }
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
    void presentation_range(const AnmVm& vm, u32, u32, u32) override {
        if (vm.source_script == 10)
            shots.push_back({vm.presentation_generation, vm.age.current, vm.visual.quad});
    }
};

struct PresentationFixture : Fixture {
    GameRandom visual_random{123};
    AnmEnvironment environment;
    AnmManager manager{visual_random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    StagePresentation presentation{manager, renderer, {4, 5, 6}, 0};

    bool init() {
        if (!Fixture::init()) return false;
        battle.player_dead = true;  // isolate shot VMs from player/option visuals
        const auto empty = th13::test::synthetic_anm(1, th13::test::trivial_scripts(1));
        const auto player_bank = animated_shot_bank();
        return th13::test::load_bank(manager, 4, empty) &&
               th13::test::load_bank(manager, 5, empty) &&
               th13::test::load_bank(manager, 6, player_bank);
    }
    bool sync_and_draw() {
        graphics.shots.clear();
        if (!presentation.sync(battle, 0) || !presentation.draw(0)) return false;
        renderer.flush();
        return true;
    }
};
}  // namespace

TEST(culling_age_gate_uses_pre_step_age_nine_and_ten) {
    for (i32 age : {9, 10}) {
        Fixture fixture;
        CHECK(fixture.init());
        fixture.battle.shots.push_back(Fixture::shot(1, age));
        fixture.snapshot(0, rectangle(200, 0, 210, 8));
        CHECK(fixture.battle.step(fixture.idle));
        CHECK_EQ(fixture.battle.shots.size(), age == 9 ? size_t(1) : size_t(0));
        if (!fixture.battle.shots.empty()) CHECK_EQ(fixture.battle.shots[0].age, 10);
    }
}

TEST(each_playfield_boundary_equality_is_outside) {
    const Quad quads[] = {rectangle(120, 200, 128, 210),
                          rectangle(200, 8, 210, 16),
                          rectangle(512, 200, 520, 210),
                          rectangle(200, 464, 210, 472)};
    for (const auto& quad : quads) {
        Fixture fixture;
        CHECK(fixture.init());
        fixture.battle.shots.push_back(Fixture::shot());
        fixture.snapshot(0, quad);
        CHECK(fixture.battle.step(fixture.idle));
        CHECK(fixture.battle.shots.empty());
    }
}

TEST(all_corners_outside_on_different_sides_still_retires) {
    Fixture fixture;
    CHECK(fixture.init());
    fixture.battle.shots.push_back(Fixture::shot());
    fixture.snapshot(0, {{{128, 200, 0}, {512, 200, 0},
                          {200, 16, 0}, {200, 464, 0}}});
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(fixture.battle.shots.empty());
}

TEST(one_strictly_inside_corner_preserves_the_shot) {
    const Vec3 inside[] = {{128.25f, 200, 0}, {511.75f, 200, 0},
                           {200, 16.25f, 0}, {200, 463.75f, 0}};
    for (const auto& point : inside) {
        Fixture fixture;
        CHECK(fixture.init());
        fixture.battle.shots.push_back(Fixture::shot());
        fixture.snapshot(0, {{{128, 200, 0}, {512, 200, 0}, {200, 16, 0}, point}});
        CHECK(fixture.battle.step(fixture.idle));
        CHECK_EQ(fixture.battle.shots.size(), size_t(1));
    }
}

TEST(culling_precedes_compaction_when_an_earlier_shot_retires) {
    Fixture fixture;
    CHECK(fixture.init());
    auto first = Fixture::shot(11);
    first.script = -1;
    first.y = -33;  // animation-free center bound retires the first slot
    fixture.battle.shots = {first, Fixture::shot(22), Fixture::shot(33)};
    fixture.snapshot(1, rectangle(200, 0, 210, 8));
    fixture.snapshot(2, rectangle(200, 200, 210, 210));
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(fixture.battle.shots.size(), size_t(1));
    if (!fixture.battle.shots.empty()) CHECK_EQ(fixture.battle.shots[0].generation, 33u);
}

TEST(stale_generation_script_or_age_cannot_retire_another_shot) {
    for (i32 mismatch = 0; mismatch < 3; ++mismatch) {
        Fixture fixture;
        CHECK(fixture.init());
        const auto shot = Fixture::shot(41);
        fixture.battle.shots.push_back(shot);
        fixture.battle.set_player_shot_geometry(0,
            shot.age + (mismatch == 2 ? 1 : 0),
            shot.script + (mismatch == 1 ? 1 : 0),
            shot.generation + (mismatch == 0 ? 1u : 0u),
            rectangle(200, 0, 210, 8));
        CHECK(fixture.battle.step(fixture.idle));
        CHECK_EQ(fixture.battle.shots.size(), size_t(1));
    }
}

TEST(sht_type_two_bypasses_quad_culling) {
    Fixture fixture;
    CHECK(fixture.init());
    ShtShotRecord record;
    record.interval = 255;
    record.script = 10;
    record.type = 2;
    fixture.battle.sht_shots_loaded = true;
    fixture.battle.sht_shots[0].push_back(record);
    for (i32 frame = 0; frame < 19; ++frame) CHECK(fixture.battle.step(fixture.idle));
    GameInput fire;
    fire.update(1);
    CHECK(fixture.battle.step(fire));
    CHECK_EQ(fixture.battle.shots.size(), size_t(1));
    if (fixture.battle.shots.empty()) return;
    CHECK_EQ(fixture.battle.shots[0].type, 2);
    fixture.battle.shots[0].age = 10;
    fixture.snapshot(0, rectangle(200, 0, 210, 8));
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(fixture.battle.shots.size(), size_t(1));
}

TEST(production_anm_quad_snapshot_is_consumed_without_post_step_translation) {
    PresentationFixture fixture;
    CHECK(fixture.init());
    auto shot = Fixture::shot(51);
    shot.y = 8;
    fixture.battle.shots.push_back(shot);
    CHECK(fixture.sync_and_draw());
    CHECK_EQ(fixture.graphics.shots.size(), size_t(1));
    if (fixture.graphics.shots.empty()) return;
    for (const auto& point : fixture.graphics.shots[0].quad) CHECK(point.y < 16);
    // The next frame's new position is inside, but 004464d0 culls against the
    // preceding ANM quad before writing its new translation into the ANM VM.
    fixture.battle.shots[0].y = 200;
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(fixture.battle.shots.empty());
}

TEST(same_script_new_generation_binds_a_fresh_animation) {
    PresentationFixture fixture;
    CHECK(fixture.init());
    fixture.battle.shots.push_back(Fixture::shot(61, 0));
    CHECK(fixture.sync_and_draw());
    CHECK_EQ(fixture.graphics.shots.size(), size_t(1));
    if (fixture.graphics.shots.empty()) return;
    const auto first = fixture.graphics.shots[0];
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(fixture.sync_and_draw());
    CHECK_EQ(fixture.graphics.shots.size(), size_t(1));
    if (fixture.graphics.shots.empty()) return;
    const auto grown = fixture.graphics.shots[0];
    CHECK_EQ(grown.generation, first.generation);
    CHECK_EQ(grown.animation_age, first.animation_age + 1);
    CHECK(grown.width() > first.width());
    fixture.battle.shots[0] = Fixture::shot(62, 0);
    CHECK(fixture.sync_and_draw());
    CHECK_EQ(fixture.graphics.shots.size(), size_t(1));
    if (fixture.graphics.shots.empty()) return;
    const auto newborn = fixture.graphics.shots[0];
    CHECK(newborn.generation != first.generation);
    CHECK_EQ(newborn.animation_age, first.animation_age);
    CHECK_EQ(newborn.width(), first.width());
}

TEST(compaction_preserves_surviving_animation_age_and_scale) {
    PresentationFixture fixture;
    CHECK(fixture.init());
    auto first = Fixture::shot(71, 0);
    first.x = 180;
    fixture.battle.shots = {first, Fixture::shot(72, 0)};
    CHECK(fixture.sync_and_draw());
    CHECK_EQ(fixture.graphics.shots.size(), size_t(2));
    if (fixture.graphics.shots.size() != 2) return;
    const auto survivor_before = fixture.graphics.shots[1];
    fixture.battle.shots[0].alive = false;
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(fixture.battle.shots.size(), size_t(1));
    if (fixture.battle.shots.empty()) return;
    CHECK_EQ(fixture.battle.shots[0].generation, 72u);
    CHECK(fixture.sync_and_draw());
    CHECK_EQ(fixture.graphics.shots.size(), size_t(1));
    if (fixture.graphics.shots.empty()) return;
    const auto survivor_after = fixture.graphics.shots[0];
    CHECK_EQ(survivor_after.generation, survivor_before.generation);
    CHECK_EQ(survivor_after.animation_age, survivor_before.animation_age + 1);
    CHECK_EQ(survivor_after.width(), survivor_before.width() + 2.0f);
}

int main() { return th13test::run_all("player shot culling and ANM identity"); }
