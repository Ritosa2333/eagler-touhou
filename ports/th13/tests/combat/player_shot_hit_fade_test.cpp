// H-PILLAR-FADE: 0x446bf0 retires a hit standard shot by writing VM+0x4a0=1
// (interrupt label id 1 = the layer-13 fade tail of pl00.anm script 10/11) and
// switching the shot to dying (speed*0.125, z=0.1); 0x4464d0 keeps moving the
// corpse and syncing its VM until the script's op1 self-destructs. The port
// keeps the VM fading instead of releasing it on the hit frame, while the
// out-of-bounds death (0x470110 via consume_player_shot_geometry) still
// releases immediately. Exercises the real battle hit path (enemy_hit ->
// collide_shots) plus the presentation death-cause mirror.
#include "../ecl/ecl_test_fixtures.hpp"
#include "../ui/ui_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"

#include <algorithm>
#include <cstring>

using namespace th13;

namespace {

class HitProvider final : public EclResourceProvider {
public:
    HitProvider() {
        std::vector<u8> root, target;
        th13test::emit(root, 0, 502, 0, 0xff, 0, {0x20});
        th13test::emit(root, 0, 23, 0, 0xff, 0, {600});
        th13test::emit(root, 0, 10, 0, 0xff, 0, {});
        th13test::emit(target, 0, 515, 0, 0xff, 0, {0});
        th13test::emit(target, 0, 42, 1, 0xff, 0, {u32(-9988)});
        th13test::emit(target, 0, 43, 1, 0xff, 0, {u32(-9985)});
        th13test::emit(target, 0, 23, 0, 0xff, 0, {600});
        th13test::emit(target, 0, 10, 0, 0xff, 0, {});
        stage = th13test::build_scpt({{"Target", target}, {"main", root}},
                                    {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name != "fade.ecl") return false;
        out = stage;
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

// Player bank script 10 mirrors pl00.anm script 10's two halves: the layer-11
// flight park, then the interrupt-1 label (time 2) with layer 13, alpha 0xC0,
// a 30-frame op409 fade and an op1 destroy at label-relative time 22. With
// interrupt_label = false the label is omitted (flight-only control script).
std::vector<u8> shot_bank(bool interrupt_label) {
    using namespace th13::test;
    auto scripts = trivial_scripts(11);
    std::vector<AnmIns> flight{AnmIns{304, 0, {11}}, sprite(0), AnmIns{3, 999, {}}};
    const std::vector<AnmIns> fade{AnmIns{5, 2, {1}}, AnmIns{304, 0, {13}},
                                    AnmIns{403, 0, {0xC0}}, AnmIns{409, 0, {30, 0, 0}},
                                    sprite(0), AnmIns{300, 5, {0}}, AnmIns{1, 22, {}}};
    if (interrupt_label) {
        flight.insert(flight.end(), fade.begin(), fade.end());
        flight.push_back(stop());
    } else {
        flight.push_back(stop());
    }
    scripts[10] = std::move(flight);
    auto bytes = synthetic_anm(8, scripts);
    // Give sprite 0 an 8x8 body so quad corners exercise the death mirror.
    u32 sprite_offset = 0;
    std::memcpy(&sprite_offset, bytes.data() + 64, sizeof sprite_offset);
    const float size = 8;
    std::memcpy(bytes.data() + sprite_offset + 12, &size, sizeof size);
    std::memcpy(bytes.data() + sprite_offset + 16, &size, sizeof size);
    return bytes;
}

struct DrawnFade {
    i32 layer = -1;
    u8 alpha = 0;
    float x = 0;
};

struct ShotGraphics final : ZunGraphics {
    touhou::graphics::PipelineState state;
    std::vector<DrawnFade> fades;
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
            fades.push_back({i32(vm.visual.layer), u8(vm.visual.color >> 24),
                             vm.visual.translation.x});
    }
};

struct FadeFixture {
    HitProvider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    GameInput idle;
    GameRandom visual_random{123};
    AnmEnvironment environment;
    AnmManager manager{visual_random, environment};
    ShotGraphics graphics;
    AnmRenderer renderer{graphics};
    std::unique_ptr<StagePresentation> presentation;

    bool init(bool interrupt_label = true) {
        provider.animations = &battle.sim.anim_list;
        player.power = 0;
        if (!battle.load(provider, "fade.ecl", 2, host, progress, player, score))
            return false;
        battle.player_dead = true;  // isolate shot VMs from player visuals
        presentation = std::make_unique<StagePresentation>(
            manager, renderer, StagePresentation::Banks{4, 5, 6}, 0);
        const auto empty = th13::test::synthetic_anm(1, th13::test::trivial_scripts(1));
        const auto player_bank = shot_bank(interrupt_label);
        return th13::test::load_bank(manager, 4, empty) &&
               th13::test::load_bank(manager, 5, empty) &&
               th13::test::load_bank(manager, 6, player_bank);
    }
    // One presentation frame: sync, direct-draw pass 0, flush.
    bool frame() {
        graphics.fades.clear();
        if (!presentation->sync(battle, 0) || !presentation->draw(0)) return false;
        renderer.flush();
        return presentation->error().empty();
    }
    StageEnemy* target(float x, float y, float life) {
        auto* enemy = battle.sim.spawn_enemy("Target", x, y, life, nullptr, false);
        if (enemy) {
            enemy->invulnerable = 0;
            enemy->tick_gate = false;
        }
        return enemy;
    }
    // Fired at x so that after the frame's +24 move it still overlaps the
    // enemy hurtbox at 200 (half 12 + shot half 4): 214-200 = 14 <= 16.
    PlayerShot shot(u32 generation, float x, float y, i32 age = 12) {
        PlayerShot value;
        value.alive = true;
        value.x = x;
        value.y = y;
        value.angle = 0;
        value.speed = 24;  // fade drift = 24*0.125 = 3.0/frame
        value.script = 10;
        value.generation = generation;
        value.age = age;
        value.type = 1;
        value.damage = 3;
        return value;
    }
};

// The real hit path: collide_shots kills the shot through enemy_hit during
// battle.step; the presentation must keep the VM and enter the fade tail.
TEST(hit_death_keeps_vm_fading_on_layer_thirteen_then_releases) {
    FadeFixture fixture;
    CHECK(fixture.init());
    auto* enemy = fixture.target(200, 200, 10000);
    CHECK(enemy != nullptr);
    if (!enemy) return;
    fixture.battle.shots.push_back(fixture.shot(1, 190, 200));
    CHECK(fixture.frame());  // flight frame: VM bound on layer 11
    CHECK_EQ(fixture.graphics.fades.size(), size_t(1));
    if (fixture.graphics.fades.empty()) return;
    CHECK_EQ(fixture.graphics.fades[0].layer, 11);

    CHECK(fixture.battle.step(fixture.idle));  // move to 214, then enemy_hit
    CHECK_EQ(fixture.battle.shots.size(), size_t(1));  // compaction is next step
    CHECK(!fixture.battle.shots[0].alive);
    CHECK(fixture.frame());  // hit frame: interrupt consumed -> layer 13
    CHECK_EQ(fixture.graphics.fades.size(), size_t(1));
    if (fixture.graphics.fades.empty()) return;
    const DrawnFade first = fixture.graphics.fades[0];
    CHECK_EQ(first.layer, 13);
    CHECK(first.alpha <= 0xC0);        // op403 0xC0, op409 already stepping
    CHECK(first.alpha >= 0xC0 - 8);
    CHECK_NEAR(first.x, 214.0f, 0.001f);  // death position: no drift on hit frame

    // op1 fires at label-relative time 22 (entry timer 22 on the 21st tick):
    // drawn through hit+19, released at hit+20. Alpha fades monotonically
    // (first step 0xB9 = the oracle's first visible fade step) and the corpse
    // drifts at speed*0.125 = 3.0 per frame (0x4464d0 dying move with
    // 0x473780 quantization).
    CHECK_EQ(first.alpha, u8(0xB9));
    u8 previous_alpha = first.alpha;
    float previous_x = first.x;
    for (i32 frame = 1; frame <= 19; ++frame) {
        CHECK(fixture.battle.step(fixture.idle));
        CHECK(fixture.frame());
        CHECK_EQ(fixture.graphics.fades.size(), size_t(1));
        if (fixture.graphics.fades.size() != 1) return;
        const DrawnFade& current = fixture.graphics.fades[0];
        CHECK_EQ(current.layer, 13);
        CHECK(current.alpha < previous_alpha);  // op409 fade monotonic
        CHECK_NEAR(current.x, previous_x + 3.0f, 0.001f);  // dying drift
        previous_alpha = current.alpha;
        previous_x = current.x;
    }
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(fixture.frame());
    CHECK(fixture.graphics.fades.empty());  // op1 self-destruct -> released
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(fixture.frame());
    CHECK(fixture.graphics.fades.empty());  // and stays released
}

// A shot hit on its birth frame (never synced alive) binds a fresh VM straight
// into the interrupt tail.
TEST(birth_frame_hit_fades_without_prior_binding) {
    FadeFixture fixture;
    CHECK(fixture.init());
    auto* enemy = fixture.target(200, 200, 10000);
    CHECK(enemy != nullptr);
    if (!enemy) return;
    fixture.battle.shots.push_back(fixture.shot(7, 190, 200, 0));
    CHECK(fixture.battle.step(fixture.idle));  // born, moved, hit in one step
    CHECK(!fixture.battle.shots[0].alive);
    CHECK(fixture.frame());
    CHECK_EQ(fixture.graphics.fades.size(), size_t(1));
    if (fixture.graphics.fades.empty()) return;
    CHECK_EQ(fixture.graphics.fades[0].layer, 13);
    CHECK(fixture.graphics.fades[0].alpha <= 0xC0);
    CHECK(fixture.graphics.fades[0].alpha >= 0xC0 - 8);
}

// 0x446bf0 writes the interrupt unconditionally, but a script without an
// id-1 label has no fade tail; the port must not strand such a VM forever.
TEST(hit_death_without_interrupt_label_releases_immediately) {
    FadeFixture fixture;
    CHECK(fixture.init(false));
    auto* enemy = fixture.target(200, 200, 10000);
    CHECK(enemy != nullptr);
    if (!enemy) return;
    fixture.battle.shots.push_back(fixture.shot(1, 190, 200));
    CHECK(fixture.frame());
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(!fixture.battle.shots[0].alive);
    CHECK(fixture.frame());
    CHECK(fixture.graphics.fades.empty());
}

// Out-of-bounds death (0x470110 semantics via consume_player_shot_geometry)
// must keep the immediate release: the mirror decides by the same previous
// quad, age and type gates the battle used.
TEST(out_of_bounds_death_releases_without_fade) {
    FadeFixture fixture;
    CHECK(fixture.init());
    auto shot = fixture.shot(1, 200, 8);  // previous quad fully above y=16
    shot.speed = 0;
    fixture.battle.shots.push_back(shot);
    CHECK(fixture.frame());
    CHECK_EQ(fixture.graphics.fades.size(), size_t(1));
    CHECK(fixture.battle.step(fixture.idle));  // geometry cull inside step_shots
    CHECK(!fixture.battle.shots[0].alive);
    CHECK(fixture.frame());
    CHECK(fixture.graphics.fades.empty());
}

}  // namespace

// The dead shot stays in battle.shots until the next step's compaction; a
// caller that syncs presentation twice between battle steps must not consume
// the same death twice (duplicate corpse VMs).
TEST(double_sync_between_steps_does_not_duplicate_the_corpse) {
    FadeFixture fixture;
    CHECK(fixture.init());
    auto* enemy = fixture.target(200, 200, 10000);
    CHECK(enemy != nullptr);
    if (!enemy) return;
    fixture.battle.shots.push_back(fixture.shot(1, 190, 200));
    CHECK(fixture.frame());
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(!fixture.battle.shots[0].alive);
    CHECK(fixture.frame());  // first sync consumes the death
    CHECK_EQ(fixture.graphics.fades.size(), size_t(1));
    CHECK(fixture.frame());  // second sync without a battle step
    CHECK_EQ(fixture.graphics.fades.size(), size_t(1));  // same single corpse
    CHECK(fixture.battle.step(fixture.idle));  // compaction happens now
    CHECK(fixture.battle.shots.empty());
    CHECK(fixture.frame());
    CHECK_EQ(fixture.graphics.fades.size(), size_t(1));
}

int main() { return th13test::run_all("player shot hit fade"); }
