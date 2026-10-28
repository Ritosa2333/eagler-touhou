// H-SPIRIT-ANIMATION-LIFETIME: 00438a77 defers label1 at frame480;
// 00438aca rejects an ordinary spirit whose real ANM handle has ended.
#include "ui_test_fixtures.hpp"
#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"

using namespace th13;

namespace {
class Provider final : public EclResourceProvider {
public:
    Provider() {
        std::vector<u8> main;
        th13test::emit(main, 0, 23, 0, 0xff, 0, {600});
        th13test::emit(main, 0, 10, 0, 0xff, 0, {});
        stage = th13test::build_scpt({{"main", main}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name != "spirit_lifetime.ecl") return false;
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

struct NullGraphics final : ZunGraphics {
    touhou::graphics::PipelineState state;
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
};

std::vector<u8> spirit_bank() {
    using namespace th13::test;
    auto scripts = trivial_scripts(11);
    // Sprite0 parks indefinitely. Deferred label1 selects sprite1, then real
    // opcode1 ends the VM at timer3. Reissuing label1 would rewind that timer.
    const std::vector<AnmIns> lifetime = {
        sprite(0), wait(), label(1), sprite(1), AnmIns{1, 3, {}}, stop(),
    };
    scripts[7] = lifetime;   // ordinary kind1: kind*6+1
    scripts[10] = lifetime;  // trance kind1: kind*6+4
    return synthetic_anm(2, scripts);
}

struct Fixture {
    Provider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    GameInput idle;
    GameRandom visual_random{123};
    AnmEnvironment environment;
    AnmManager animations{visual_random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    StagePresentation presentation{animations, renderer, {4, 5, 6, 8, 25}, 0};
    i32 fade_interrupts = 0;
    i32 alive_queries = 0;

    ~Fixture() { presentation.release(); }

    bool init() {
        provider.animations = &battle.sim.anim_list;
        player.power = 0;
        progress.difficulty = 1;
        progress.max_point = 1000000;
        progress.max_point_cap = 25000000;
        if (!battle.load(provider, "spirit_lifetime.ecl", 2, host,
                         progress, player, score)) return false;
        battle.sim.enemies.clear();
        battle.player_dead = true;  // no collection while observing the fade
        const auto spirits = spirit_bank();
        const auto empty = th13::test::synthetic_anm(1, th13::test::trivial_scripts(1));
        if (!th13::test::load_bank(animations, 4, empty) ||
            !th13::test::load_bank(animations, 8, th13::test::synthetic_anm(1, th13::test::trivial_scripts(11))) ||
            !th13::test::load_bank(animations, 5, empty) ||
            !th13::test::load_bank(animations, 6, empty) ||
            !th13::test::load_bank(animations, 25, spirits)) return false;
        // Observe the actual manager interrupt call; its normal deferred
        // execution still runs through AnmRegistry and the synthetic script.
        animations.effect_interrupt = [this](AnmVm& vm, i32 label, float) {
            if ((vm.source_script == 7 || vm.source_script == 10) && label == 1)
                ++fade_interrupts;
            return true;
        };
        battle.item_manager.spirit_animation_alive = [this](u32 slot, u32 serial) {
            ++alive_queries;
            return presentation.spirit_animation_alive(slot, serial);
        };
        return true;
    }

    Spirit& spawn(bool trance = false) {
        if (trance) {
            progress.ending_points = 400;
            trance_begin(progress);
        }
        battle.item_manager.spawn_value_item(1, 0, 200, 0, 0.5f);
        auto& spirit = battle.item_manager.spirits()[0];
        spirit.move_speed = spirit.speed = 0;
        return spirit;
    }

    bool sync() { return presentation.sync(battle, 0); }

    u32 spirit_handle(i32 script) {
        for (u32 index = 0; index < animations.registry.ordered_count(false); ++index) {
            const u32 handle = animations.registry.ordered_handle(false, index);
            auto* vm = animations.registry.find(handle);
            if (vm && vm->source_script == script) {
                CHECK_EQ(vm->resource, animations.resource(25));
                return handle;
            }
        }
        return 0;
    }

    void finish_animation(u32 handle) {
        // The limit bounds the synthetic script failure; it does not extend
        // or substitute the production lifetime/collection condition.
        for (i32 tick = 0; tick < 8 && animations.registry.find(handle); ++tick)
            CHECK(animations.update(false));
        CHECK(animations.registry.find(handle) == nullptr);
    }

    void become_collecting_player(const Spirit& spirit) {
        battle.player_dead = false;
        battle.set_player_pos(spirit.x, spirit.y);
    }
};
}  // namespace

TEST(frame_480_queues_one_deferred_label_one_without_restarting_the_fade) {
    Fixture fixture;
    CHECK(fixture.init());
    auto& spirit = fixture.spawn();
    spirit.frame = 478;
    CHECK(fixture.sync());
    const u32 handle = fixture.spirit_handle(7);
    CHECK(handle != 0);
    if (!handle) return;
    auto* vm = fixture.animations.registry.find(handle);
    CHECK_EQ(vm->visual.sprite, 0);
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(spirit.frame, 479);
    CHECK((spirit.flags & 2u) != 0);
    CHECK(!spirit.fade_requested);
    CHECK(fixture.sync());
    CHECK_EQ(fixture.fade_interrupts, 0);

    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(spirit.frame, 480);
    CHECK((spirit.flags & 2u) == 0);
    CHECK(spirit.fade_requested);
    const i32 animation_age = vm->age.current;
    CHECK(fixture.sync());
    CHECK(!spirit.fade_requested);
    CHECK_EQ(fixture.fade_interrupts, 1);
    CHECK_EQ(vm->pending_interrupt, 1);
    CHECK_EQ(vm->visual.sprite, 0);  // sync did not tick the label immediately
    CHECK_EQ(vm->age.current, animation_age);

    CHECK(fixture.animations.update(false));
    CHECK_EQ(vm->pending_interrupt, 0);
    CHECK_EQ(vm->visual.sprite, 1);
    const i32 fade_timer = vm->timer.current;
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(spirit.frame, 481);
    CHECK(!spirit.fade_requested);
    CHECK(fixture.sync());
    CHECK_EQ(vm->pending_interrupt, 0);
    CHECK_EQ(fixture.fade_interrupts, 1);
    CHECK(fixture.animations.update(false));
    CHECK_EQ(vm->timer.current, fade_timer + 1);
    CHECK(fixture.sync());
    CHECK_EQ(fixture.fade_interrupts, 1);
    fixture.finish_animation(handle);
}

TEST(real_animation_completion_blocks_later_gameplay_collection_and_respawn) {
    Fixture fixture;
    CHECK(fixture.init());
    auto& spirit = fixture.spawn();
    const u32 serial = spirit.serial;
    spirit.frame = 479;
    CHECK(fixture.sync());
    const u32 handle = fixture.spirit_handle(7);
    CHECK(handle != 0);
    if (!handle) return;
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(fixture.sync());
    fixture.finish_animation(handle);
    CHECK(spirit.alive);  // ANM ended first; the next gameplay update owns removal
    CHECK(!fixture.presentation.spirit_animation_alive(0, serial));

    CHECK(fixture.sync());
    CHECK_EQ(fixture.spirit_handle(7), 0u);  // same serial must not recreate its VM
    CHECK(!fixture.presentation.spirit_animation_alive(0, serial));
    fixture.become_collecting_player(spirit);
    const i32 before = fixture.progress.max_point;
    const i32 queried = fixture.alive_queries;
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(fixture.alive_queries > queried);
    CHECK(!spirit.alive);
    CHECK_EQ(fixture.progress.max_point, before);
    CHECK_EQ(fixture.score.score, 0);
    CHECK(fixture.battle.item_manager.events().empty());
}

TEST(new_serial_reusing_the_same_kind_and_slot_gets_a_new_real_vm) {
    Fixture fixture;
    CHECK(fixture.init());
    auto& old_spirit = fixture.spawn();
    const u32 old_serial = old_spirit.serial;
    old_spirit.frame = 479;
    CHECK(fixture.sync());
    const u32 old_handle = fixture.spirit_handle(7);
    CHECK(old_handle != 0);
    if (!old_handle) return;
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(fixture.sync());
    fixture.finish_animation(old_handle);
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(!old_spirit.alive);

    auto& newborn = fixture.spawn();
    CHECK_EQ(&newborn, &old_spirit);
    CHECK_EQ(newborn.kind, 1);
    CHECK(newborn.serial != old_serial);
    CHECK(!newborn.fade_requested);
    CHECK_EQ(newborn.frame, -1);
    CHECK(fixture.presentation.spirit_animation_alive(0, newborn.serial));
    CHECK(fixture.sync());
    const u32 new_handle = fixture.spirit_handle(7);
    CHECK(new_handle != 0);
    CHECK(new_handle != old_handle);
    auto* vm = fixture.animations.registry.find(new_handle);
    CHECK(vm != nullptr);
    if (!vm) return;
    CHECK_EQ(vm->visual.sprite, 0);
    CHECK_EQ(vm->pending_interrupt, 0);
    CHECK(fixture.presentation.spirit_animation_alive(0, newborn.serial));
}

TEST(trance_birth_bypasses_frame_480_fade_and_missing_animation_gate) {
    Fixture fixture;
    CHECK(fixture.init());
    auto& spirit = fixture.spawn(true);
    const u32 serial = spirit.serial;
    spirit.frame = 479;
    CHECK((spirit.flags & 1u) != 0);
    CHECK(fixture.sync());
    const u32 handle = fixture.spirit_handle(10);
    CHECK(handle != 0);
    if (!handle) return;
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(spirit.frame, 479);
    CHECK(!spirit.fade_requested);
    CHECK((spirit.flags & 2u) != 0);
    CHECK(fixture.sync());
    CHECK_EQ(fixture.fade_interrupts, 0);
    CHECK(fixture.animations.interrupt(handle, 1, false));
    fixture.finish_animation(handle);
    CHECK(!fixture.presentation.spirit_animation_alive(0, serial));
    CHECK(spirit.alive);

    fixture.become_collecting_player(spirit);
    const i32 before = fixture.progress.max_point;
    const i32 queried = fixture.alive_queries;
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(fixture.alive_queries, queried);  // the actual missing VM is ignored
    CHECK(!spirit.alive);
    CHECK_EQ(fixture.progress.max_point, before + 10000);
    CHECK_EQ(fixture.score.score, 0);
}

TEST(trance_transition_replaces_existing_spirit_vm_and_resets_its_lifetime) {
    Fixture fixture; CHECK(fixture.init());
    Spirit& spirit = fixture.spawn(); CHECK(fixture.sync());
    const auto serial = spirit.serial;
    const auto ordinary = fixture.spirit_handle(7); CHECK(ordinary != 0);
    spirit.frame = 480; spirit.fade_requested = true;
    fixture.battle.item_manager.set_spirit_trance(true);
    CHECK_EQ(spirit.flags, 3u); CHECK_EQ(spirit.frame, 0);
    CHECK_EQ(spirit.serial, serial); CHECK(!spirit.fade_requested);
    CHECK(fixture.sync());
    const auto active = fixture.spirit_handle(10); CHECK(active != 0); CHECK(active != ordinary);
    fixture.battle.item_manager.set_spirit_trance(false);
    CHECK_EQ(spirit.flags, 2u); CHECK_EQ(spirit.frame, 0);
    CHECK(fixture.sync());
    const auto restored = fixture.spirit_handle(7); CHECK(restored != 0); CHECK(restored != active);
    CHECK_EQ(spirit.serial, serial); CHECK_EQ(spirit.x, 0.f); CHECK_EQ(spirit.y, 200.f);
}

TEST(existing_point_spirit_collects_trance_reward_after_conversion) {
    Fixture fixture; CHECK(fixture.init());
    Spirit& spirit = fixture.spawn(); CHECK(fixture.sync());
    fixture.battle.item_manager.set_spirit_trance(true);
    CHECK(fixture.sync()); fixture.become_collecting_player(spirit);
    const i32 before = fixture.progress.max_point;
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(!spirit.alive); CHECK_EQ(fixture.progress.max_point, before + 10000);
}

int main() { return th13test::run_all("spirit animation lifetime"); }
