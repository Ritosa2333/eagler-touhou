#include "../../ecl/ecl_test_fixtures.hpp"
#include "game/AnmManager.hpp"
#include "game/FrameScheduler.hpp"
#include "game/StageBattle.hpp"
#include "multiplayer/WorldState.hpp"
#include "multiplayer/Session.hpp"
#include "multiplayer/StageEntry.hpp"
#include "sdl/ApplicationState.hpp"
#include <cmath>
#include <cstdlib>
#include <sstream>

using namespace th13;
using namespace th13test;

namespace {

struct Host final : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
    bool hud_sound(i32, float) override { return true; }
    bool stop_sound(i32) override { return true; }
};

class Provider final : public EclResourceProvider {
public:
    Provider() {
        std::vector<u8> main;
        emit(main, 0, 23, 0, 0xff, 0, {600});
        emit(main, 0, 10, 0, 0xff, 0, {});
        stage = build_scpt({{"main", main}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name == "mp6.ecl") {
            out = stage;
            return true;
        }
        if (name.size() > 4 && name.compare(name.size() - 4, 4, ".sht") == 0) {
            static const std::string root = [] {
                std::string path = __FILE__;
                for (int i = 0; i < 8; ++i) {
                    const std::string probe = path + "/artifacts/reference/raw/pl00a.sht";
                    std::FILE* check = std::fopen(probe.c_str(), "rb");
                    if (check) { std::fclose(check); break; }
                    const auto cut = path.find_last_of("/\\");
                    if (cut == std::string::npos) break;
                    path.erase(cut);
                }
                return path;
            }();
            std::FILE* file = std::fopen((root + "/artifacts/reference/raw/" + name).c_str(), "rb");
            if (!file) return false;
            std::fseek(file, 0, SEEK_END);
            const long size = std::ftell(file);
            std::fseek(file, 0, SEEK_SET);
            out.resize(size_t(size < 0 ? 0 : size));
            const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
            std::fclose(file);
            return got == out.size();
        }
        return false;
    }
    bool animation(u32, const std::string& name) override {
        if (animations) animations->push_back(name);
        return true;
    }
    std::vector<std::string>* animations = nullptr;
    std::vector<u8> stage;
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
        progress.max_point_cap = 500000;
        return battle.load(provider, "mp6.ecl", 2, host, progress, player, score);
    }
    void enable(i32 count, std::array<i32, 3> characters) {
        CHECK(battle.enable_seats(provider, count, characters));
    }
};

}

TEST(full_state_restore_replay) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0});
    GameRandom visual(0x1234);
    GameInput fire;
    for (int frame = 0; frame < 40; ++frame) {
        fire.update(0x01);
        dual.battle.seat_inputs[0] = fire;
        dual.battle.seat_inputs[1] = fire;
        CHECK(dual.battle.step(GameInput{}));
    }
    multiplayer::WorldState snapshot;
    CHECK(snapshot.capture(dual.battle, visual));
    const uint64_t digest_at_capture = snapshot.digest();
    std::printf("# digest-baseline full=%016llx core=%016llx\n",
        static_cast<unsigned long long>(digest_at_capture),
        static_cast<unsigned long long>(snapshot.digest_core()));
    CHECK(snapshot.ready());
    CHECK(std::string(multiplayer::WorldState::field_ledger()).size() > 32);
    for (int frame = 0; frame < 25; ++frame) {
        fire.update(0x01);
        dual.battle.seat_inputs[0] = fire;
        dual.battle.seat_inputs[1] = fire;
        CHECK(dual.battle.step(GameInput{}));
    }
    CHECK(snapshot.restore(dual.battle, visual));
    multiplayer::WorldState verify;
    CHECK(verify.capture(dual.battle, visual));
    CHECK_EQ(verify.digest(), digest_at_capture);
    Fixture baseline;
    CHECK(baseline.init());
    baseline.enable(2, {0, 0});
    for (int frame = 0; frame < 40; ++frame) {
        fire.update(0x01);
        baseline.battle.seat_inputs[0] = fire;
        baseline.battle.seat_inputs[1] = fire;
        CHECK(baseline.battle.step(GameInput{}));
    }
    for (int frame = 0; frame < 30; ++frame) {
        fire.update(0x01);
        dual.battle.seat_inputs[0] = fire;
        dual.battle.seat_inputs[1] = fire;
        CHECK(dual.battle.step(GameInput{}));
        baseline.battle.seat_inputs[0] = fire;
        baseline.battle.seat_inputs[1] = fire;
        CHECK(baseline.battle.step(GameInput{}));
        multiplayer::WorldState a, b;
        CHECK(a.capture(dual.battle, visual));
        CHECK(b.capture(baseline.battle, visual));
        if (a.digest() != b.digest()) {
            CHECK_EQ(a.digest(), b.digest());
            return;
        }
    }
}

TEST(snapshot_mutation_sensitivity) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0});
    GameRandom visual(0xBEEF);
    GameInput fire;
    for (int frame = 0; frame < 30; ++frame) {
        fire.update(0x01);
        dual.battle.seat_inputs[0] = fire;
        dual.battle.seat_inputs[1] = fire;
        CHECK(dual.battle.step(GameInput{}));
    }
    multiplayer::WorldState snapshot;
    CHECK(snapshot.capture(dual.battle, visual));
    const uint64_t before = snapshot.digest();
    dual.battle.seat_huds[1].life_piece_threshold_index=3;
    multiplayer::WorldState mutated;
    CHECK(mutated.capture(dual.battle,visual)); CHECK(mutated.digest()!=before);
    CHECK(snapshot.restore(dual.battle,visual));
    CHECK_EQ(dual.battle.seat_huds[1].life_piece_threshold_index,0);
    const float seat0_x = dual.battle.seats[0].x;
    const i32 seat0_power = dual.battle.seat_huds[0].power;
    const size_t seat1_shots = dual.battle.seats[1].shots.size();
    dual.battle.seats[0].x = 123.0f;
    dual.battle.seat_huds[0].power = 399;
    dual.battle.seats[1].shots.push_back(PlayerShot{});
    dual.battle.sim.frame += 77;
    dual.progress.ending_points = 555;
    dual.score.score = 98765;
    dual.battle.item_manager.spawn_value_item(1, 0.0f, 0.0f, 0.0f, 1.0f);
    if (!dual.battle.sim.enemies.empty()) dual.battle.sim.enemies[0]->life = 1.0f;
    CHECK_EQ(snapshot.digest(), before);
    CHECK(snapshot.restore(dual.battle, visual));
    CHECK_EQ(dual.battle.seats[0].x, seat0_x);
    CHECK_EQ(dual.battle.seat_huds[0].power, seat0_power);
    CHECK_EQ(dual.battle.seats[1].shots.size(), seat1_shots);
    CHECK_EQ(dual.progress.ending_points, 0);
    CHECK_EQ(dual.score.score, 0);
    dual.battle.item_manager.spawn_value_item(1, 0.0f, 0.0f, 0.0f, 1.0f);
    bool restored_spirit_seen = false;
    for (const Spirit& spirit : dual.battle.item_manager.spirits())
        if (spirit.alive && spirit.serial == 1u) restored_spirit_seen = true;
    CHECK(restored_spirit_seen);
    CHECK(snapshot.ready());
}

TEST(transfer_snapshot_and_digest) {
    Fixture world; CHECK(world.init()); world.enable(3, {0, 0, 0});
    auto& battle = world.battle;
    battle.seats[0].life_transfer_state = 0x43;
    battle.seats[1].power_transfer_state = 0x62;
    CHECK(battle.item_manager.spawn_targeted_item(0, 300, ItemExtraLife, 2));
    GameRandom visual(0xbeef);
    multiplayer::WorldState saved, changed;
    CHECK(saved.capture(battle, visual));
    const auto original = saved.digest(), core = saved.digest_core();
    char domains[4096]{}, mutated[4096]{};
    saved.digest_domains(domains, sizeof(domains));
    battle.seats[1].power_transfer_state ^= 1;
    CHECK(changed.capture(battle, visual));
    CHECK(changed.digest() != original); CHECK(changed.digest_core() != core);
    changed.digest_domains(mutated, sizeof(mutated));
    CHECK(std::string(domains) != mutated);
    CHECK(saved.restore(battle, visual));
    CHECK_EQ(battle.seats[1].power_transfer_state, 0x62);
    Item* gift = nullptr;
    for (auto& item : battle.item_manager.items())
        if (item.alive && item.target_seat == 2) gift = &item;
    CHECK(gift != nullptr); if (!gift) return;
    gift->target_seat = 1;
    CHECK(changed.capture(battle, visual));
    CHECK(changed.digest() != original); CHECK(changed.digest_core() != core);
    changed.digest_domains(mutated, sizeof(mutated));
    CHECK(std::string(domains) != mutated);
    CHECK(saved.restore(battle, visual));
    CHECK(changed.capture(battle, visual)); CHECK_EQ(changed.digest(), original);
    CHECK_EQ(battle.seats[0].life_transfer_state, 0x43);
    multiplayer::StageEntry entry;
    entry.Capture(battle, world.progress, world.score, 1, 0, 2, 0);
    entry.RestoreBattleCarry(battle);
    CHECK_EQ(battle.seats[0].life_transfer_state, 0);
    CHECK_EQ(battle.seats[1].power_transfer_state, 0);
}

TEST(pools_ids_free_order) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0});
    GameRandom visual(0x0F0F);
    GameInput fire;
    for (int frame = 0; frame < 35; ++frame) {
        fire.update(0x01);
        dual.battle.seat_inputs[0] = fire;
        dual.battle.seat_inputs[1] = fire;
        CHECK(dual.battle.step(GameInput{}));
    }
    const u32 spawned = dual.battle.sim.spawn_count();
    CHECK(dual.battle.mp_registry() == nullptr);
    multiplayer::WorldState snapshot;
    CHECK(snapshot.capture(dual.battle, visual));
    const auto captured_digest = snapshot.digest();
    dual.battle.item_manager.spawn_value_item(1, 0.0f, 0.0f, 0.0f, 1.0f);
    multiplayer::WorldState next_allocation;
    CHECK(next_allocation.capture(dual.battle, visual));
    const auto next_digest = next_allocation.digest();
    CHECK(snapshot.restore(dual.battle, visual));
    for (int frame = 0; frame < 20; ++frame) {
        fire.update(0x01);
        dual.battle.seat_inputs[0] = fire;
        dual.battle.seat_inputs[1] = fire;
        CHECK(dual.battle.step(GameInput{}));
    }
    dual.battle.item_manager.spawn_value_item(2, 5.0f, 5.0f, 0.0f, 1.0f);
    dual.battle.item_manager.spawn_value_item(3, -5.0f, 5.0f, 0.0f, 1.0f);
    CHECK(snapshot.restore(dual.battle, visual));
    CHECK_EQ(dual.battle.sim.spawn_count(), spawned);
    multiplayer::WorldState restored;
    CHECK(restored.capture(dual.battle, visual));
    CHECK_EQ(restored.digest(), captured_digest);
    dual.battle.item_manager.spawn_value_item(1, 0.0f, 0.0f, 0.0f, 1.0f);
    CHECK(restored.capture(dual.battle, visual));
    CHECK_EQ(restored.digest(), next_digest);
}

TEST(anm_borrowed_parent_chains) {
    GameRandom visual(7);
    AnmEnvironment environment;
    AnmManager manager(visual, environment);
    std::vector<u8> anm_bytes;
    {
        std::string path = __FILE__;
        for (int i = 0; i < 8; ++i) {
            const std::string probe = path + "/artifacts/reference/raw/front.anm";
            std::FILE* check = std::fopen(probe.c_str(), "rb");
            if (check) { std::fclose(check); break; }
            const auto cut = path.find_last_of("/\\");
            if (cut == std::string::npos) break;
            path.erase(cut);
        }
        std::FILE* file = std::fopen((path + "/artifacts/reference/raw/front.anm").c_str(), "rb");
        CHECK(file != nullptr);
        std::fseek(file, 0, SEEK_END);
        const long size = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);
        anm_bytes.resize(size_t(size < 0 ? 0 : size));
        const size_t got = anm_bytes.empty() ? 1 : std::fread(anm_bytes.data(), 1, anm_bytes.size(), file);
        std::fclose(file);
        CHECK_EQ(got, anm_bytes.size());
    }
    CHECK(manager.load(3, anm_bytes.data(), u32(anm_bytes.size())));
    AnmVm* parent = manager.create_instance(*manager.resource(3), 0);
    CHECK(parent != nullptr);
    const u32 parent_handle = manager.registry.submit(*parent);
    CHECK(parent_handle != 0);
    AnmVm* child = manager.create_instance(*manager.resource(3), 1);
    CHECK(child != nullptr);
    const u32 child_handle = manager.registry.submit(*child);
    CHECK(child_handle != 0);
    CHECK(manager.registry.attach(*child, *parent));
    AnmVm external_root;
    CHECK(external_root.bind(*manager.resource(3), 0));
    AnmVm* borrowed_child = manager.create_instance(*manager.resource(3), 2);
    CHECK(borrowed_child != nullptr);
    const u32 borrowed_child_handle = manager.registry.submit(*borrowed_child);
    CHECK(manager.registry.attach(*borrowed_child, external_root));
    CHECK_EQ(manager.registry.ordered_count(false), 3u);
    CHECK_EQ(manager.registry.ordered_handle(false, 0), parent_handle);
    CHECK_EQ(manager.registry.ordered_handle(false, 1), child_handle);
    CHECK_EQ(manager.registry.ordered_handle(false, 2), borrowed_child_handle);
    multiplayer::WorldState snapshot;
    Fixture fixture;
    CHECK(fixture.init());
    fixture.battle.mp_registry_hook = &manager.registry;
    GameRandom visual_rng(0x77);
    CHECK(snapshot.capture(fixture.battle, visual_rng));
    CHECK(manager.registry.destroy_tree(*parent));
    CHECK(manager.registry.destroy_tree(*borrowed_child));
    AnmVm* spare = manager.create_instance(*manager.resource(3), 0);
    CHECK(spare != nullptr);
    CHECK(manager.registry.submit(*spare) != 0);
    CHECK_EQ(manager.registry.ordered_count(false), 1u);
    CHECK(snapshot.restore(fixture.battle, visual_rng));
    CHECK_EQ(manager.registry.ordered_count(false), 3u);
    CHECK_EQ(manager.registry.ordered_handle(false, 0), parent_handle);
    CHECK_EQ(manager.registry.ordered_handle(false, 1), child_handle);
    CHECK_EQ(manager.registry.ordered_handle(false, 2), borrowed_child_handle);
    AnmVm* restored_parent = manager.registry.find(parent_handle);
    CHECK(restored_parent != nullptr);
    AnmVm* restored_child = manager.registry.find(child_handle);
    CHECK(restored_child != nullptr);
    const std::vector<AnmVm*> children = manager.registry.children(*restored_parent);
    bool child_linked = false;
    for (const AnmVm* entry : children)
        if (entry == restored_child) child_linked = true;
    CHECK(child_linked);
    const std::vector<AnmVm*> borrowed_children = manager.registry.children(external_root);
    bool borrowed_linked = false;
    for (const AnmVm* entry : borrowed_children)
        if (manager.registry.handle(*entry) == borrowed_child_handle) borrowed_linked = true;
    CHECK(borrowed_linked);
}

TEST(wire_digest_uses_enemy_animation_occupancy) {
    Fixture fixture;
    CHECK(fixture.init());
    CHECK(!fixture.battle.sim.enemies.empty());
    if (fixture.battle.sim.enemies.empty()) return;
    auto& enemy = *fixture.battle.sim.enemies.front();
    GameRandom visual(0x1234);
    auto before = std::make_unique<multiplayer::WorldState>();
    auto after = std::make_unique<multiplayer::WorldState>();
    enemy.anm_slot_handle[0] = 0x2001;
    CHECK(before->capture(fixture.battle, visual));
    enemy.anm_slot_handle[0] = 0x4002;
    CHECK(after->capture(fixture.battle, visual));
    CHECK_EQ(before->digest_core(), after->digest_core());
    CHECK(before->digest() != after->digest());
    enemy.anm_slot_handle[0] = 0;
    CHECK(after->capture(fixture.battle, visual));
    CHECK(before->digest_core() != after->digest_core());
    enemy.anm_slot_handle[0] = 0x4002;
    enemy.life += 1;
    CHECK(after->capture(fixture.battle, visual));
    CHECK(before->digest_core() != after->digest_core());
}

TEST(anm_recreated_and_frozen_roots) {
    AnmRegistry registry;
    auto original = std::make_unique<AnmVm>();
    auto* child = registry.allocate();
    const u32 handle = registry.submit(*child);
    CHECK(registry.attach(*child, *original));
    child->creation_parent = child->rotation_parent = original.get();
    AnmRegistry::SnapshotData borrowed;
    CHECK(registry.capture(borrowed));
    const auto* identity = original.get();
    original.reset();
    AnmVm replacement;
    CHECK(registry.restore(borrowed, {{identity, &replacement}}));
    child = registry.find(handle);
    CHECK(child && child->creation_parent == &replacement &&
          child->rotation_parent == &replacement);
    replacement.visual.translation.x = 137;
    CHECK(registry.preserve_embedded_children(replacement));
    AnmRegistry::SnapshotData frozen;
    CHECK(registry.capture(frozen));
    for (int repeat = 0; repeat < 2; ++repeat) {
        CHECK(registry.restore(frozen));
        child = registry.find(handle);
        CHECK(child && child->creation_parent);
        if (!child || !child->creation_parent) return;
        CHECK(child->creation_parent == child->rotation_parent);
        CHECK_EQ(child->creation_parent->visual.translation.x, 137);
        CHECK_EQ(registry.children(*child->creation_parent).size(), 1u);
    }
}

TEST(scheduler_callback_rebind) {
    FrameScheduler scheduler;
    std::array<FrameCallback, 4> callbacks{};
    i32 counters[4] = {0, 0, 0, 0};
    const i32 priorities[4] = {20, 22, 24, 34};
    for (int i = 0; i < 4; ++i) {
        callbacks[i].owner = &counters[i];
        callbacks[i].enabled = true;
        callbacks[i].run = [](void* owner) -> i32 {
            ++*static_cast<i32*>(owner);
            return 1;
        };
        CHECK(scheduler.add(callbacks[i], FramePass::Update, priorities[i]) >= 0);
    }
    CHECK_EQ(scheduler.update(), 4);
    for (int i = 0; i < 4; ++i) CHECK_EQ(counters[i], 1);
    for (int i = 0; i < 4; ++i) CHECK(scheduler.contains(callbacks[i]));
    Fixture fixture;
    CHECK(fixture.init());
    multiplayer::WorldState snapshot;
    GameRandom visual(3);
    CHECK(snapshot.capture(fixture.battle, visual));
    GameInput fire;
    for (int frame = 0; frame < 5; ++frame) {
        fire.update(0x01);
        CHECK(fixture.battle.step(fire));
    }
    CHECK(snapshot.restore(fixture.battle, visual));
    for (int i = 0; i < 4; ++i) CHECK(scheduler.contains(callbacks[i]));
    CHECK_EQ(scheduler.update(), 4);
    for (int i = 0; i < 4; ++i) CHECK_EQ(counters[i], 2);
    scheduler.remove(callbacks[2]);
    CHECK(!scheduler.contains(callbacks[2]));
    CHECK_EQ(scheduler.update(), 3);
    CHECK_EQ(counters[0], 3);
    CHECK_EQ(counters[1], 3);
    CHECK_EQ(counters[2], 2);
    CHECK_EQ(counters[3], 3);
}

TEST(dual_rng_events_transition) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 1});
    GameRandom visual(0x5A5A);
    GameInput fire;
    for (int frame = 0; frame < 30; ++frame) {
        fire.update(0x01);
        dual.battle.seat_inputs[0] = fire;
        dual.battle.seat_inputs[1] = fire;
        CHECK(dual.battle.step(GameInput{}));
    }
    const u16 game_seed = dual.battle.sim.random.seed();
    const u32 game_index = dual.battle.sim.random.index();
    const u16 visual_seed = visual.seed();
    const u32 visual_index = visual.index();
    multiplayer::WorldState snapshot;
    CHECK(snapshot.capture(dual.battle, visual));
    const uint64_t digest_a = snapshot.digest();
    for (int frame = 0; frame < 15; ++frame) {
        fire.update(0x01);
        dual.battle.seat_inputs[0] = fire;
        dual.battle.seat_inputs[1] = fire;
        CHECK(dual.battle.step(GameInput{}));
        (void)dual.battle.sim.random.next();
        (void)visual.next();
    }
    CHECK(dual.battle.sim.random.seed() != game_seed ||
          dual.battle.sim.random.index() != game_index);
    CHECK(snapshot.restore(dual.battle, visual));
    CHECK_EQ(dual.battle.sim.random.seed(), game_seed);
    CHECK_EQ(dual.battle.sim.random.index(), game_index);
    CHECK_EQ(visual.seed(), visual_seed);
    CHECK_EQ(visual.index(), visual_index);
    multiplayer::WorldState verify;
    CHECK(verify.capture(dual.battle, visual));
    CHECK_EQ(verify.digest(), digest_a);
    Fixture baseline;
    CHECK(baseline.init());
    baseline.enable(2, {0, 1});
    for (int frame = 0; frame < 30; ++frame) {
        fire.update(0x01);
        baseline.battle.seat_inputs[0] = fire;
        baseline.battle.seat_inputs[1] = fire;
        CHECK(baseline.battle.step(GameInput{}));
    }
    for (int frame = 0; frame < 10; ++frame) {
        fire.update(0x01);
        for (StageBattle* battle : {&dual.battle, &baseline.battle}) {
            battle->seat_inputs[0] = fire;
            battle->seat_inputs[1] = fire;
            CHECK(battle->step(GameInput{}));
        }
        CHECK_EQ(dual.battle.sim.random.seed(), baseline.battle.sim.random.seed());
        CHECK_EQ(dual.battle.sim.random.index(), baseline.battle.sim.random.index());
    }
}

namespace {

using App = th13::sdl::ApplicationState;

bool p32_pin_boot(App& app, const char* save_name) {
    ::setenv("TH13_SAVE_PATH", save_name, 1);
    const char* asset_root = std::getenv("TH13_NATIVE_ASSET_ROOT");
    if (!asset_root || !*asset_root) return false;
    if (!app.audio_device.initialize_for_test(
            asset_root, std::string(std::getenv("TH13_NATIVE_REPO_ROOT")
                                       ? std::getenv("TH13_NATIVE_REPO_ROOT")
                                       : "../../..") +
                            "/games/th13/thbgmogg.dat"))
        return false;
    return app.initialize(false);
}

bool p32_boot_live_run(App& app, const char* save_name) {
    if (!p32_pin_boot(app, save_name)) return false;
    if (!app.start_game(1)) return false;
    return app.pending_destination == 13 && app.failure.empty();
}

bool p32_enter_run(App& app, const char* save_name) {
    if (!p32_boot_live_run(app, save_name)) return false;
    GameInput input;
    for (int barrier = 0; barrier < 4 && !app.run; ++barrier) {
        input.update(0);
        if (!app.update(input, false) || !app.commit(60.0f) || !app.present())
            return false;
    }
    return app.run != nullptr && app.failure.empty();
}

bool p32_frame(App& app, u32 held) {
    static GameInput input;
    input.update(held);
    return app.update(input, false) && app.commit(60.0f) && app.present() &&
           app.failure.empty();
}

}

TEST(rollback_across_bomb_shake) {
    App rollback;
    CHECK(p32_enter_run(rollback, "t7m6-shake-rb.bin"));
    if (!rollback.run) return;
    App baseline;
    CHECK(p32_enter_run(baseline, "t7m6-shake-bl.bin"));
    if (!baseline.run) return;
    CHECK(rollback.run->inject_shake(th13::ScreenShake::bomb(12, 4, 30, 20)));
    CHECK(baseline.run->inject_shake(th13::ScreenShake::bomb(12, 4, 30, 20)));
    for (int i = 0; i < 3; ++i) {
        CHECK(p32_frame(rollback, 0));
        CHECK(p32_frame(baseline, 0));
    }
    multiplayer::WorldState snapshot;
    GameRandom& visual = rollback.visual;
    CHECK(snapshot.capture(rollback.run->battle, visual, nullptr,
                           rollback.run->presentation.get(), rollback.run.get(),
                           rollback.run_transition.handle()));
    for (int i = 0; i < 8; ++i) CHECK(p32_frame(rollback, 0));
    u32 handle_back = 0;
    CHECK(snapshot.restore(rollback.run->battle, visual, nullptr,
                           rollback.run->presentation.get(), rollback.run.get(),
                           &handle_back));
    for (int i = 0; i < 3; ++i) {
        CHECK(p32_frame(rollback, 0));
        CHECK(p32_frame(baseline, 0));
    }
    multiplayer::WorldState a, b;
    CHECK(a.capture(rollback.run->battle, visual, nullptr,
                    rollback.run->presentation.get(), rollback.run.get(),
                    rollback.run_transition.handle()));
    CHECK(b.capture(baseline.run->battle, baseline.visual, nullptr,
                    baseline.run->presentation.get(), baseline.run.get(),
                    baseline.run_transition.handle()));
    CHECK_EQ(a.digest_core(), b.digest_core());
    CHECK_EQ(rollback.run->battle.sim.random.index(),
             baseline.run->battle.sim.random.index());
}

TEST(rollback_across_msg_start) {
    App rollback;
    CHECK(p32_enter_run(rollback, "t7m6-msg-rb.bin"));
    if (!rollback.run) return;
    App baseline;
    CHECK(p32_enter_run(baseline, "t7m6-msg-bl.bin"));
    if (!baseline.run) return;
    rollback.run->open_dialog(0);
    baseline.run->open_dialog(0);
    CHECK(rollback.run->dialog_active());
    for (int i = 0; i < 2; ++i) {
        CHECK(p32_frame(rollback, 0));
        CHECK(p32_frame(baseline, 0));
    }
    multiplayer::WorldState snapshot;
    CHECK(snapshot.capture(rollback.run->battle, rollback.visual, nullptr,
                           rollback.run->presentation.get(), rollback.run.get(),
                           rollback.run_transition.handle()));
    for (int i = 0; i < 8; ++i) CHECK(p32_frame(rollback, 0));
    u32 handle_back = 0;
    CHECK(snapshot.restore(rollback.run->battle, rollback.visual, nullptr,
                           rollback.run->presentation.get(), rollback.run.get(),
                           &handle_back));
    CHECK(rollback.run->dialog_active());
    for (int i = 0; i < 3; ++i) {
        CHECK(p32_frame(rollback, 0));
        CHECK(p32_frame(baseline, 0));
    }
    CHECK_EQ(rollback.run->dialog_clock(), baseline.run->dialog_clock());
    CHECK_EQ(rollback.run->dialog_active(), baseline.run->dialog_active());
    multiplayer::WorldState a, b;
    CHECK(a.capture(rollback.run->battle, rollback.visual, nullptr,
                    rollback.run->presentation.get(), rollback.run.get(),
                    rollback.run_transition.handle()));
    CHECK(b.capture(baseline.run->battle, baseline.visual, nullptr,
                    baseline.run->presentation.get(), baseline.run.get(),
                    baseline.run_transition.handle()));
    CHECK_EQ(a.digest_core(), b.digest_core());
}

TEST(rollback_across_stage_transition) {
    App rollback;
    CHECK(p32_enter_run(rollback, "t7m6-std-rb.bin"));
    if (!rollback.run) return;
    App baseline;
    CHECK(p32_enter_run(baseline, "t7m6-std-bl.bin"));
    if (!baseline.run) return;
    for (int i = 0; i < 6; ++i) {
        CHECK(p32_frame(rollback, 0));
        CHECK(p32_frame(baseline, 0));
    }
    const u32 scenery_frames_at_capture =
        rollback.run->scenery ? rollback.run->scenery->frames : 0;
    multiplayer::WorldState snapshot;
    CHECK(snapshot.capture(rollback.run->battle, rollback.visual, nullptr,
                           rollback.run->presentation.get(), rollback.run.get(),
                           rollback.run_transition.handle()));
    for (int i = 0; i < 8; ++i) CHECK(p32_frame(rollback, 0));
    u32 handle_back = 0;
    CHECK(snapshot.restore(rollback.run->battle, rollback.visual, nullptr,
                           rollback.run->presentation.get(), rollback.run.get(),
                           &handle_back));
    if (rollback.run->scenery)
        CHECK_EQ(rollback.run->scenery->frames, scenery_frames_at_capture);
    for (int i = 0; i < 4; ++i) {
        CHECK(p32_frame(rollback, 0));
        CHECK(p32_frame(baseline, 0));
    }
    CHECK_EQ(rollback.run->scenery ? rollback.run->scenery->frames : 0,
             baseline.run->scenery ? baseline.run->scenery->frames : 0);
    multiplayer::WorldState a, b;
    CHECK(a.capture(rollback.run->battle, rollback.visual, nullptr,
                    rollback.run->presentation.get(), rollback.run.get(),
                    rollback.run_transition.handle()));
    CHECK(b.capture(baseline.run->battle, baseline.visual, nullptr,
                    baseline.run->presentation.get(), baseline.run.get(),
                    baseline.run_transition.handle()));
    CHECK_EQ(a.digest_core(), b.digest_core());
}

TEST(scene_restore_full_one_three_twelve) {
    for (int distance : {1, 3, 12}) {
        App app;
        CHECK(p32_enter_run(app, "scene-full-restore.bin"));
        if (!app.run) return;
        for (int i=0;i<6;++i) CHECK(p32_frame(app,0));
        multiplayer::WorldState snapshot;
        CHECK(snapshot.capture(app.run->battle,app.visual,nullptr,
            app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
        GameInput input;const auto saved_input=input;
        const auto frame=[&] { input.update(1);return app.update(input,false)&&app.commit(60.f)&&app.present(); };
        std::vector<uint64_t> expected;
        std::vector<std::string> expected_domains;
        std::vector<std::string> expected_details;
        for(int i=0;i<distance;++i) {
            CHECK(frame());
            multiplayer::WorldState state;
            CHECK(state.capture(app.run->battle,app.visual,nullptr,
                app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
            expected.push_back(state.digest());
            char domains[2048];state.digest_domains(domains,sizeof domains);expected_domains.emplace_back(domains);expected_details.push_back(state.diagnostic_detail());
        }
        u32 transition=0;
        CHECK(snapshot.restore(app.run->battle,app.visual,nullptr,
            app.run->presentation.get(),app.run.get(),&transition));
        multiplayer::WorldState restored;
        CHECK(restored.capture(app.run->battle,app.visual,nullptr,
            app.run->presentation.get(),app.run.get(),transition));
        CHECK_EQ(restored.digest(),snapshot.digest());
        input=saved_input;
        for(int i=0;i<distance;++i) {
            CHECK(frame());
            multiplayer::WorldState state;
            CHECK(state.capture(app.run->battle,app.visual,nullptr,
                app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
            if(state.digest()!=expected[i]) {
                char domains[2048];state.digest_domains(domains,sizeof domains);
                std::printf("distance=%d frame=%d\nexpected %s\nactual   %s\n",distance,i,expected_domains[i].c_str(),domains);
                std::printf("expected detail %s\nactual detail %s\n",expected_details[i].c_str(),state.diagnostic_detail().c_str());
            }
            CHECK_EQ(state.digest(),expected[i]);
        }
    }
}

TEST(scene_digest_mutation_controls) {
    App app;
    CHECK(p32_enter_run(app,"scene-mutation.bin"));
    if(!app.run)return;
    const auto capture=[&] { multiplayer::WorldState state;
        CHECK(state.capture(app.run->battle,app.visual,nullptr,
            app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
        return state;
    };
    auto initial=capture();
    const auto state=app.run->capture_rollback();
    std::unordered_map<const AnmVm*,AnmVm*> remapped;
    auto changed=state; ++changed.post.clear_frame_;
    app.run->restore_rollback(changed,remapped);
    CHECK(capture().digest_core()!=initial.digest_core());
    app.run->restore_rollback(state,remapped);
    changed=state;changed.player_activation_pending=!changed.player_activation_pending;
    app.run->restore_rollback(changed,remapped);
    CHECK(capture().digest_core()!=initial.digest_core());
    app.run->restore_rollback(state,remapped);
    changed=state; changed.camera_origin.x+=1;
    app.run->restore_rollback(changed,remapped);
    CHECK(capture().digest_core()!=initial.digest_core());
    app.run->restore_rollback(state,remapped);
    changed=state;++changed.hud.boss.warn_state_;
    app.run->restore_rollback(changed,remapped);
    CHECK(capture().digest()!=initial.digest());
    app.run->restore_rollback(state,remapped);
    const auto hud_change = [&](auto mutate) {
        auto updated=state; mutate(updated.hud);
        app.run->restore_rollback(updated,remapped);
        CHECK(capture().digest()!=initial.digest());
        app.run->restore_rollback(state,remapped);
        CHECK_EQ(capture().digest(),initial.digest());
    };
    hud_change([](auto& hud){++hud.mp_seat_count;});
    hud_change([](auto& hud){++hud.mp_life_icons[0][0];});
    hud_change([](auto& hud){++hud.mp_bomb_icons[1][7];});
    hud_change([](auto& hud){++hud.mp_resource_labels[0][0];});
    hud_change([](auto& hud){++hud.mp_shared_labels[2];});
    changed=state;changed.spell.anchor_[0]+=1;
    app.run->restore_rollback(changed,remapped);
    CHECK(capture().digest()!=initial.digest());
    app.run->restore_rollback(state,remapped);
    changed=state;++changed.popups.cursor;
    app.run->restore_rollback(changed,remapped);
    CHECK(capture().digest()!=initial.digest());
    app.run->restore_rollback(state,remapped);
    CHECK(app.run->inject_shake(ScreenShake::bomb(12,4,30,20)));
    CHECK(capture().digest_core()!=initial.digest_core());
    CHECK(initial.restore(app.run->battle,app.visual,nullptr,
        app.run->presentation.get(),app.run.get(),nullptr));
    CHECK_EQ(capture().digest(),initial.digest());
    if(app.run->scenery) {
        ++app.run->scenery->script.state.instruction_offset;
        CHECK(capture().digest_core()!=initial.digest_core());
    }
}

TEST(anm_draw_layers_restore_and_digest) {
    {
        AnmRegistry registry;
        std::vector<AnmVm*> vms;
        std::vector<u32> handles;
        bool allocated = true;
        for (u32 i = 0; i < 0x2001; ++i) {
            auto* vm = registry.allocate();
            const auto handle = vm ? registry.submit(*vm, i & 1 ? 4 : 0) : 0;
            if (!handle) { allocated = false; break; }
            vm->visual.translation.x = float(i);
            vms.push_back(vm); handles.push_back(handle);
        }
        CHECK(allocated); if (!allocated) return;
        CHECK(registry.discard(*vms[42]));
        CHECK(registry.attach(*vms[0x1fff], *vms[100]));
        AnmVm borrowed;
        CHECK(registry.attach(*vms[0x2000], borrowed));
        vms[0x1fff]->creation_parent = vms[100];
        vms[0x2000]->rotation_parent = &borrowed;
        registry.append_draw(*vms[0]);
        registry.append_draw(*vms[0x1fff]);
        registry.append_draw(borrowed);
        registry.append_draw(*vms[0]);
        AnmRegistry::SnapshotData snapshot;
        CHECK(registry.capture(snapshot));
        CHECK_EQ(snapshot.version, AnmRegistry::SnapshotData::kContractVersion);
        CHECK_EQ(snapshot.entries.size(), 0x2001u);
        CHECK_EQ(snapshot.entries[41].slot, 41u);
        CHECK_EQ(snapshot.entries[42].slot, 43u);
        CHECK(snapshot.entries[0x1ffe].in_overflow);
        CHECK_EQ(snapshot.entries[0x1ffe].parent, 99u);
        CHECK_EQ(snapshot.entries[0x1ffe].creation_parent, 99u);
        CHECK(snapshot.entries[0x2000].borrowed);
        CHECK_EQ(snapshot.entries[0x1fff].parent, 0x2000u);
        CHECK_EQ(snapshot.entries[0x1fff].rotation_parent, 0x2000u);
        CHECK_EQ(snapshot.layers[0].size(), 4u);
        CHECK_EQ(snapshot.layers[0][1].entry, 0x1ffeu);
        CHECK_EQ(snapshot.layers[0][2].entry, 0x2000u);
        CHECK_EQ(snapshot.layers[0][0].entry, snapshot.layers[0][3].entry);
        Fixture fixture; CHECK(fixture.init());
        fixture.battle.mp_registry_hook = &registry;
        GameRandom visual(123);
        multiplayer::WorldState saved, restored;
        CHECK(saved.capture(fixture.battle, visual));
        const auto full = saved.digest(), core = saved.digest_core();
        CHECK(registry.discard(*vms[0x1fff]));
        CHECK(saved.restore(fixture.battle, visual));
        CHECK(restored.capture(fixture.battle, visual));
        CHECK_EQ(restored.digest(), full); CHECK_EQ(restored.digest_core(), core);
        std::printf("capture-pool-overflow full=%016llx core=%016llx\n",
                    (unsigned long long)full, (unsigned long long)core);
        CHECK_EQ(registry.layer(0).size(), 4u);
        CHECK(registry.layer(0)[0] == registry.layer(0)[3]);
        CHECK_EQ(registry.find(handles[0x1fff])->creation_parent, registry.find(handles[100]));
        CHECK_EQ(registry.find(handles[0x2000])->rotation_parent, &borrowed);
    }
    {
        AnmRegistry first_registry, second_registry;
        auto* parent = first_registry.allocate();
        auto* child = first_registry.allocate();
        CHECK(parent && child); if (!parent || !child) return;
        CHECK(first_registry.submit(*parent)); CHECK(first_registry.submit(*child));
        CHECK(first_registry.attach(*child, *parent));
        parent->geometry.screen_vertices.resize(24);
        parent->geometry.screen_vertices.back().position.x = 17;
        child->geometry.world_vertices.resize(12);
        AnmRegistry::SnapshotData reused;
        CHECK(first_registry.capture(reused));
        const auto original = reused;
        auto* root0 = second_registry.allocate();
        auto* root1 = second_registry.allocate();
        CHECK(root0 && root1); if (!root0 || !root1) return;
        const auto handle0 = second_registry.submit(*root0);
        const auto handle1 = second_registry.submit(*root1);
        CHECK(handle0 && handle1);
        root0->geometry.screen_vertices.resize(1);
        root0->geometry.screen_vertices[0].position.x = -23;
        CHECK(second_registry.capture(reused));
        CHECK_EQ(reused.entries.size(), 2u);
        CHECK(reused.entries[0].children.empty());
        CHECK(reused.entries[1].parent == AnmRegistry::SnapshotData::kNoVm);
        CHECK_EQ(reused.entries[0].vm.geometry.screen_vertices.size(), 1u);
        CHECK_EQ(reused.entries[0].vm.geometry.screen_vertices[0].position.x, -23.f);
        CHECK(reused.entries[1].vm.geometry.world_vertices.empty());
        CHECK_EQ(original.entries[0].vm.geometry.screen_vertices.size(), 24u);
        CHECK_EQ(original.entries[0].vm.geometry.screen_vertices.back().position.x, 17.f);
        root0->geometry.screen_vertices[0].position.x = 101;
        CHECK(second_registry.restore(reused));
        CHECK_EQ(second_registry.find(handle0)->geometry.screen_vertices[0].position.x, -23.f);
        CHECK(second_registry.children(*second_registry.find(handle0)).empty());
        CHECK(second_registry.children(*second_registry.find(handle1)).empty());
        AnmRegistry empty_registry;
        CHECK(empty_registry.capture(reused));
        CHECK(reused.entries.empty());
    }
    App app;
    CHECK(p32_pin_boot(app, "anm-layer-restore.bin"));
    CHECK(app.configure_multiplayer(3, {0, 1, 3}, 0));
    CHECK(app.start_game(1)); app.pending_destination = -1;
    CHECK(app.begin_run()); if (!app.run) return;
    auto& battle = app.run->battle;
    battle.item_manager.spawn_item(-80, 260, 0, ItemPower, 0, 0);
    battle.item_manager.spawn_item(80, 260, 0, ItemPower, 0, 0);
    GameInput input;
    for (int frame = 0; frame < 8; ++frame) {
        input.update(0);
        CHECK(app.update(input, false) && app.commit(60.f) && app.present());
    }
    const AnmVm* first = nullptr;
    const AnmVm* second = nullptr;
    u32 first_slot = 0, second_slot = 0;
    for (u32 slot = 0; slot < ItemManager::kItemCapacity; ++slot) {
        const auto* item = app.run->presentation->item_body_vm(slot);
        if (!item) continue;
        if (!first) { first = item; first_slot = slot; }
        else if (!second) { second = item; second_slot = slot; break; }
    }
    CHECK(first && second); if (!first || !second) return;
    const auto& items = app.animations.registry.layer(15);
    CHECK(std::find(items.begin(), items.end(), first) != items.end());
    CHECK(std::find(items.begin(), items.end(), second) != items.end());
    const auto capture = [&] {
        multiplayer::WorldState state;
        CHECK(state.capture(battle, app.visual, nullptr, app.run->presentation.get(),
                            app.run.get(), app.run_transition.handle()));
        return state;
    };
    auto saved = capture();
    const u32 game_seed = app.game_random.seed(), game_index = app.game_random.index();
    const u32 visual_seed = app.visual.seed(), visual_index = app.visual.index();
    app.animations.registry.append_draw(*const_cast<AnmVm*>(first));
    const auto duplicate = capture();
    CHECK(duplicate.digest() != saved.digest());
    app.animations.registry.append_draw(*const_cast<AnmVm*>(second));
    const auto first_then_second = capture();
    CHECK(saved.restore(battle, app.visual, nullptr, app.run->presentation.get(), app.run.get()));
    first = app.run->presentation->item_body_vm(first_slot);
    second = app.run->presentation->item_body_vm(second_slot);
    CHECK(first && second); if (!first || !second) return;
    app.animations.registry.append_draw(*const_cast<AnmVm*>(second));
    app.animations.registry.append_draw(*const_cast<AnmVm*>(first));
    const auto second_then_first = capture();
    CHECK(first_then_second.digest() != second_then_first.digest());
    CHECK(saved.restore(battle, app.visual, nullptr, app.run->presentation.get(), app.run.get()));
    CHECK_EQ(app.game_random.seed(), game_seed); CHECK_EQ(app.game_random.index(), game_index);
    CHECK_EQ(app.visual.seed(), visual_seed); CHECK_EQ(app.visual.index(), visual_index);
    const auto restored = capture();
    CHECK_EQ(restored.digest(), saved.digest());
    CHECK(app.present());
    CHECK_EQ(capture().digest(), saved.digest());
    AnmVm unknown;
    unknown.visual.layer = 15;
    app.animations.registry.append_draw(unknown);
    CHECK(!saved.capture(battle, app.visual, nullptr, app.run->presentation.get(), app.run.get()));
    CHECK(!saved.ready());
    CHECK(!saved.restore(battle, app.visual, nullptr, app.run->presentation.get(), app.run.get()));
}

TEST(scene_present_does_not_advance_state) {
    App app;CHECK(p32_enter_run(app,"draw-repeat-state.bin"));
    for(int i=0;i<6;++i) CHECK(p32_frame(app,0));
    CHECK(app.run->scenery!=nullptr);app.run->scenery->begin_exit();
    const auto capture=[&] { multiplayer::WorldState s;
        CHECK(s.capture(app.run->battle,app.visual,nullptr,app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
        return s; };
    const auto a=capture();CHECK(app.present());const auto b=capture();
    CHECK(app.present());const auto c=capture();
    if(a.digest()!=b.digest() || b.digest()!=c.digest())
        std::printf("draw A %s\ndraw B %s\ndraw C %s\n",a.diagnostic_detail().c_str(),b.diagnostic_detail().c_str(),c.diagnostic_detail().c_str());
    CHECK_EQ(a.digest(),b.digest());CHECK_EQ(b.digest(),c.digest());
}

TEST(scene_update_then_present_preserves_full_state) {
    App app;CHECK(p32_enter_run(app,"update-draw-state.bin"));
    for(int i=0;i<6;++i) CHECK(p32_frame(app,0));
    for(int frame=0;frame<12;++frame) {
        GameInput input;input.update(1);CHECK(app.update(input,false));CHECK(app.commit(60.f));
        multiplayer::WorldState before,after;
        CHECK(before.capture(app.run->battle,app.visual,nullptr,app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
        CHECK(app.present());
        CHECK(after.capture(app.run->battle,app.visual,nullptr,app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
        if(before.digest()!=after.digest()) {
            char a[2048],b[2048];before.digest_domains(a,sizeof a);after.digest_domains(b,sizeof b);
            std::printf("update-draw frame=%d before %s\nafter %s\n",frame,a,b);
        }
        CHECK_EQ(before.digest(),after.digest());
    }
}

TEST(registry_restore_removes_speculative_high_water_slots) {
    AnmRegistry registry;AnmRegistry::SnapshotData before,after;
    CHECK(registry.capture(before));
    auto* vm=registry.allocate();CHECK(vm!=nullptr);CHECK(registry.submit(*vm)!=0);
    CHECK(registry.restore(before));CHECK(registry.capture(after));
    CHECK_EQ(after.entries.size(),before.entries.size());
    CHECK(after.free_slots==before.free_slots);CHECK_EQ(after.generation,before.generation);
    vm=registry.allocate();CHECK(vm!=nullptr);CHECK(registry.submit(*vm)!=0);
}

TEST(new_network_world_ignores_local_title_allocation_history) {
    using namespace th13::multiplayer;
    std::uint64_t expected=0;
    for(unsigned preview_frames : {0u,37u,143u}) {
        session.Stop();App app;CHECK(p32_pin_boot(app,"world-epoch.bin"));
        for(unsigned i=0;i<preview_frames;++i) CHECK(p32_frame(app,0));
        CHECK(app.configure_multiplayer(2,{0,0,0},0));
        SessionSetup setup;setup.version=2;setup.mode=2;setup.seed=1234;setup.session_id=42;setup.build[0]=1;
        const auto words=EncodeSessionSetup(setup);CHECK(session.Configure(words.data(),words.size()));
        CHECK(session.BeginLocalSimulation());
        app.progress.difficulty=1;CHECK(app.start_game(1));app.pending_destination=-1;
        app.game_random.reseed_restore(1234);app.visual.reseed_restore(1234);
        CHECK(app.begin_run());
        WorldState state;CHECK(state.capture(app.run->battle,app.visual,nullptr,
            app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
        if(preview_frames==0) expected=state.digest();
        else CHECK_EQ(state.digest(),expected);
        session.Stop();
    }
}

TEST(scene_laser_retire_restore_update_only) {
    for (int distance : {1,3,12}) {
        App app; CHECK(p32_enter_run(app,"scene-laser-restore.bin"));
        if(!app.run)return;
        auto& sim=app.run->battle.sim;
        StageEt et;et.reset();et.sprite=8;et.subtype=6;
        et.spawn[0]=et.spawn[1]=480;et.laser_width=16;et.speed1=8;et.sound=-1;
        et.laser_timing[0]=20;et.laser_timing[1]=8;
        et.laser_timing[2]=140;et.laser_timing[3]=10;
        StageEnemy enemy;enemy.position[0]=96;enemy.position[1]=144;
        sim.emit_infinite_laser(et,enemy,257);
        CHECK(sim.error().empty()); CHECK(!sim.lasers.empty());
        multiplayer::WorldState snapshot;
        CHECK(snapshot.capture(app.run->battle,app.visual,nullptr,
            app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
        std::vector<uint64_t> expected;
        for(int i=0;i<distance;++i) {
            sim.advance_lasers();CHECK(sim.error().empty());
            multiplayer::WorldState state;
            CHECK(state.capture(app.run->battle,app.visual,nullptr,
                app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
            expected.push_back(state.digest());
        }
        sim.lasers.front().phase=1;sim.advance_lasers();CHECK(sim.lasers.empty());
        CHECK(snapshot.restore(app.run->battle,app.visual,nullptr,
            app.run->presentation.get(),app.run.get(),nullptr));
        for(int i=0;i<distance;++i) {
            sim.advance_lasers();CHECK(sim.error().empty());
            multiplayer::WorldState state;
            CHECK(state.capture(app.run->battle,app.visual,nullptr,
                app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
            CHECK_EQ(state.digest(),expected[i]);
        }
    }
}

TEST(stage6_clear_flow_restore_179_379) {
    struct Host final : PostGamePlatform {
        int tick=0;std::vector<int> fades,ends;
        void request_destination(i32 raw) override { CHECK_EQ(raw,15);ends.push_back(tick); }
        bool request_stage6_fade() override { fades.push_back(tick);return true; }
        void bgm(i32) override {}
    } host;
    SessionState progress;progress.stage=6;
    HudPlayerState player;HudScoreState score;
    PostGameFlow flow(progress,player,score,host);
    flow.on_stage_clear();CHECK(flow.clear_sequence());
    for(int boundary:{179,379}) {
        auto state=flow.capture_rollback();
        state.clear_sequence_=true;state.clear_frame_=u32(boundary);
        state.age_=u32(boundary);flow.restore_rollback(state);
        const auto saved_progress=progress;
        std::vector<int> expected_fades,expected_ends;
        for(int pass=0;pass<2;++pass) {
            host.fades.clear();host.ends.clear();
            flow.restore_rollback(state);progress=saved_progress;
            for(host.tick=0;host.tick<3;++host.tick)flow.update();
            if(pass==0){expected_fades=host.fades;expected_ends=host.ends;}
            else {CHECK(host.fades==expected_fades);CHECK(host.ends==expected_ends);}
        }
        if(boundary==179){CHECK(expected_fades==std::vector<int>{0});CHECK(expected_ends.empty());}
        else {CHECK(expected_fades.empty());CHECK(expected_ends==std::vector<int>{0});}
    }
}

int main() { return th13test::run_all("m6.world_state"); }

TEST(host_outputs_and_handoff_rollback_one_three_twelve) {
    using namespace th13::multiplayer;
    for (unsigned distance : {1u,3u,12u}) {
        session.Stop();
        App app; CHECK(p32_enter_run(app,"host-commit-restore.bin"));
        if (!app.run) return;
        CHECK(app.run->enable_multiplayer(2,{0,0,0}));
        SessionSetup setup;setup.version=2;setup.mode=2;setup.automatic=false;
        setup.session_id=42;setup.build[0]=1;
        const auto words=EncodeSessionSetup(setup);
        CHECK(session.Configure(words.data(),words.size()));
        session.BindWorld(&app.run->battle,&app.visual,app.run->presentation.get(),app.run.get(),&app.mp_transition_handle_);
        session.capture_host=[&] { return app.capture_multiplayer_host(); };
        app.run->playback_input=[] { return session.LocalDelayedInput(); };
        CHECK(session.BeginLocalSimulation());
        const auto step=[&] { CHECK(session.CanSimulate());
            CHECK(app.update(session.MenuInput(),false));CHECK(app.commit(60.f));session.OnFrameSimulated(); };
        CHECK(session.CaptureLocal(0,0));session.AdvanceCapturePhase();
        CHECK(session.SubmitRemote(1,0,{})==Netplay::RemoteInputResult::Accepted);
        step();
        const auto base_frames=app.frames;
        const auto base_fades=app.ending_fades.size();
        const auto base_play=app.files.ordinary_records[0].total_frames;
        CHECK(app.loading_prayer!=0);
        auto* caption=app.loading_animations.registry.find(app.loading_prayer);
        CHECK(caption!=nullptr);
        const auto base_caption_time=caption->timer.current;
        app.slow_window_time_=.98;app.slow_window_frames_=49;
        const auto base_slow=app.slow_num_;
        for(unsigned i=1;i<=distance;++i) {
            CHECK(session.CaptureLocal(1,i));session.AdvanceCapturePhase();
            step();
        }
        CHECK_EQ(app.files.ordinary_records[0].total_frames,base_play);
        CHECK(!session.AllFramesCommitted());
        CHECK(!session.FinishWorldSimulation());
        CHECK(session.Simulating());
        app.pending_destination=i32(th13::sdl::Destination::NextStage);
        CHECK(app.request_stage6_fade());
        auto* old=app.run.get();
        CHECK(app.present());CHECK(app.run.get()==old);
        CHECK_EQ(app.pending_destination,i32(th13::sdl::Destination::NextStage));
        for(unsigned i=1;i<=distance;++i) {
            Netplay::FrameInput input;if(i==1) input.buttons=0x2;
            const auto result=session.SubmitRemote(1,i,input);
            CHECK(result==Netplay::RemoteInputResult::Accepted || result==Netplay::RemoteInputResult::RollbackRequired || result==Netplay::RemoteInputResult::PredictionCorrect);
        }
        CHECK(session.Pump(distance+1));
        CHECK_EQ(session.Restores(),1u);CHECK_EQ(session.SimFrame(),1u);
        CHECK_EQ(app.frames,base_frames);CHECK_EQ(app.pending_destination,-1);
        CHECK_EQ(app.ending_fades.size(),base_fades);
        CHECK_EQ(app.slow_window_time_,.98);CHECK_EQ(app.slow_window_frames_,49u);
        CHECK_EQ(app.slow_num_,base_slow);
        CHECK(app.loading_prayer!=0);
        caption=app.loading_animations.registry.find(app.loading_prayer);
        CHECK(caption!=nullptr);CHECK_EQ(caption->timer.current,base_caption_time);
        CHECK_EQ(app.files.ordinary_records[0].total_frames,base_play);
        while(session.Correcting()) step();
        CHECK(session.AllFramesCommitted());
        CHECK_EQ(app.frames,base_frames+distance);
        CHECK_EQ(app.files.ordinary_records[0].total_frames,base_play+distance);
        CHECK(session.FinishWorldSimulation());CHECK(!session.Simulating());
        CHECK(app.begin_title(3));CHECK(app.run==nullptr);CHECK(app.title!=nullptr);
        CHECK(app.step(GameInput{},60.f,false));CHECK(!session.Simulating());
        session.Stop();
    }
}

TEST(network_stage_activation_inputs_and_resources) {
    using namespace th13::multiplayer;
    for (bool ghost : {false, true}) {
    std::uint64_t expected = 0;
    for (unsigned local : {0u, 1u}) {
        session.Stop();
        App app; CHECK(p32_pin_boot(app, "network-stage-activation.bin"));
        CHECK(app.configure_multiplayer(2, {0, 1, 0}, int(local)));
        SessionSetup setup; setup.version=2; setup.mode=2; setup.automatic=false;
        setup.session_id=42; setup.seed=1234; setup.build[0]=1; setup.local_player=local;
        setup.selections={0,1,0};
        const auto words=EncodeSessionSetup(setup);
        CHECK(session.Configure(words.data(),words.size()));
        CHECK(app.start_game(1)); CHECK(app.complete_loading());
        CHECK(app.apply_destination()); CHECK(app.run != nullptr);
        if (!app.run) return;
        CHECK(session.BeginLocalSimulation());
        auto& old=app.run->battle;
        old.seat_huds[0].extra_lives=4; old.seat_huds[0].bombs=1;
        old.seat_huds[0].power=173; old.seat_huds[0].life_pieces=2;
        old.seat_huds[1].extra_lives=0; old.seat_huds[1].bombs=0;
        old.seat_huds[1].power=249; old.seat_huds[1].bomb_pieces=3;
        old.seats[1].ghost=ghost;
        old.trance_generation=7;
        const std::array<u32,2> held{0x49u,0x81u};
        CHECK(session.CaptureLocal(held[local],0));session.AdvanceCapturePhase();
        CHECK(session.SubmitRemote(1-local,0,Netplay::FrameInput(held[1-local])) == Netplay::RemoteInputResult::Accepted);
        CHECK(session.CanSimulate());
        app.progress.stage=2; app.stage_transition_active=true; app.stage_startup_age=30;
        CHECK(app.update(session.MenuInput(),false));CHECK(app.commit(60.f));
        session.OnFrameSimulated();
        CHECK_EQ(app.progress.stage_frame,1);
        auto& next=app.run->battle;
        CHECK_EQ(next.seat_inputs[0].held,held[0]);
        CHECK_EQ(next.seat_inputs[1].held,held[1]);
        CHECK(next.seats[0].x<0);CHECK(next.seats[1].x>0);
        CHECK_EQ(next.seat_huds[0].extra_lives,4);CHECK_EQ(next.seat_huds[0].bombs,1);
        CHECK_EQ(next.seat_huds[0].power,173);CHECK_EQ(next.seat_huds[0].life_pieces,2);
        CHECK_EQ(next.seat_huds[1].extra_lives,0);CHECK_EQ(next.seat_huds[1].bombs,ghost ? 2 : 0);
        CHECK_EQ(next.seat_huds[1].power,ghost ? next.seat_huds[1].max_power / 2 : 249);
        CHECK_EQ(next.seat_huds[1].bomb_pieces,ghost ? 0 : 3);
        if (ghost) CHECK_EQ(next.seats[1].invulnerable,119);
        CHECK(!next.seats[1].ghost);CHECK(!next.seats[1].dead);
        CHECK_EQ(next.trance_generation,7u);
        WorldState state;CHECK(state.capture(next,app.visual,nullptr,app.run->presentation.get(),app.run.get(),app.mp_transition_handle_));
        if (local==0) expected=state.digest(); else CHECK_EQ(state.digest(),expected);
        session.Stop();
    }
    }
}

TEST(audit_epoch_and_timing_bounds) {
    using namespace th13::multiplayer;
    DialogueText::RollbackState dialogue{};
    dialogue.pixels = std::make_shared<std::vector<u8>>(16, 0);
    const auto original_host = WorldState::digest_dialogue(dialogue);
    auto mutated_host = dialogue;
    mutated_host.box = 42;
    CHECK(WorldState::digest_dialogue(mutated_host) != original_host);
    mutated_host = dialogue;
    mutated_host.cached[0] = true;
    CHECK(WorldState::digest_dialogue(mutated_host) != original_host);
    mutated_host = dialogue;
    mutated_host.pixels = std::make_shared<std::vector<u8>>(16, 1);
    CHECK(WorldState::digest_dialogue(mutated_host) != original_host);
    session.Stop();
    Fixture world;CHECK(world.init());CHECK(world.battle.enable_seats(world.provider,2,{0,0,0}));
    GameRandom visual{1234};
    SessionSetup setup;setup.version=2;setup.mode=2;setup.automatic=false;
    setup.session_id=42;setup.build[0]=1;
    const auto words=EncodeSessionSetup(setup);CHECK(session.Configure(words.data(),words.size()));
    session.BindWorld(&world.battle,&visual,nullptr);CHECK(session.BeginLocalSimulation());
    session.SetAudit(true);
    CHECK(session.CaptureLocal(0,0));session.AdvanceCapturePhase();CHECK(session.CanSimulate());
    CHECK(world.battle.step(*session.LocalDelayedInput()));session.OnFrameSimulated();
    CHECK_EQ(session.RecordCount(),1u);
    session.SetAudit(false);
    CHECK(session.SubmitRemote(1,0,{})==Netplay::RemoteInputResult::PredictionCorrect);
    CHECK(session.Pump(1));CHECK_EQ(session.RecordCount(),0u);
    CHECK(session.TakeAuditJson().find("\"frames\":[]")!=std::string::npos);
    session.SetAudit(true);
    WorldState before;CHECK(before.capture(world.battle,visual));
    for(unsigned i=0;i<4097;++i)session.TraceTiming("probe",i);
    const auto timing=session.TakeTimingJson();
    CHECK(timing.find("\"overflow\":1")!=std::string::npos);
    std::size_t events=0,at=0;
    while((at=timing.find("\"kind\":",at))!=std::string::npos){++events;++at;}
    CHECK_EQ(events,4096u);
    WorldState after;CHECK(after.capture(world.battle,visual));CHECK_EQ(after.digest(),before.digest());
    CHECK(session.FinishWorldSimulation(true));
    const auto terminal=session.TakeAuditJson();
    std::ostringstream expected;
    expected << "\"full\":\"" << std::hex << after.digest() << "\"";
    CHECK(terminal.find(expected.str())!=std::string::npos);
    CHECK(terminal.find("\"frames\":1,\"completed\":true")!=std::string::npos);
    CHECK(session.TakeAuditJson().find(expected.str())!=std::string::npos);
    session.Stop();CHECK(session.TakeTimingJson().find("\"events\":[]")!=std::string::npos);
    CHECK(session.TakeAuditJson().find("\"terminal\":null")!=std::string::npos);
}

TEST(network_dialog_prediction_one_three_twelve) {
    using namespace th13::multiplayer;
    for (unsigned distance : {1u, 3u, 12u}) {
        std::vector<u64> expected;
        for (bool prediction : {false, true}) {
            session.Stop();
            App app; CHECK(p32_pin_boot(app, "network-dialog-prediction.bin"));
            CHECK(app.configure_multiplayer(2, {0,0,0}, 0));
            SessionSetup setup; setup.version=2; setup.mode=2; setup.automatic=false;
            setup.session_id=42; setup.seed=1234; setup.build[0]=1;
            const auto words=EncodeSessionSetup(setup); CHECK(session.Configure(words.data(),words.size()));
            CHECK(session.BeginLocalSimulation());
            app.progress.difficulty=1; CHECK(app.start_game(1)); app.pending_destination=-1;
            app.game_random.reseed_restore(1234); app.visual.reseed_restore(1234);
            CHECK(app.begin_run()); CHECK(app.complete_loading());
            const auto step=[&] {
                CHECK(session.CanSimulate());
                CHECK(app.update(session.MenuInput(),false)); CHECK(app.commit(60.f));
                session.OnFrameSimulated(); CHECK(app.present());
            };
            for(unsigned frame=0;frame<2;++frame) {
                CHECK(session.CaptureLocal(0,frame)); session.AdvanceCapturePhase();
                CHECK(session.SubmitRemote(1,frame,{})==Netplay::RemoteInputResult::Accepted);
                step(); CHECK(session.Pump(frame));
                if(frame==0) app.run->open_dialog(0);
            }
            CHECK(app.run->dialog_active()); CHECK(!session.PredictionBarrier());
            const auto observe=[&](unsigned index) {
                WorldState state; CHECK(state.capture(app.run->battle,app.visual,nullptr,
                    app.run->presentation.get(),app.run.get(),app.mp_transition_handle_));
                if(prediction) CHECK_EQ(state.digest(),expected[index]);
                else expected.push_back(state.digest());
            };
            for(unsigned index=0;index<distance;++index) {
                const unsigned frame=index+2;
                CHECK(session.CaptureLocal(0,frame)); session.AdvanceCapturePhase();
                if(!prediction) CHECK(session.SubmitRemote(1,frame,
                    Netplay::FrameInput(InputLanes::Confirm | InputLanes::Dialogue | 1))==Netplay::RemoteInputResult::Accepted);
                step();
                if(!prediction) { CHECK(session.Pump(frame)); observe(index); }
            }
            if(prediction) {
                for(unsigned index=0;index<distance;++index) {
                    const auto result=session.SubmitRemote(1,index+2,
                        Netplay::FrameInput(InputLanes::Confirm | InputLanes::Dialogue | 1));
                    CHECK(result==Netplay::RemoteInputResult::RollbackRequired ||
                          result==Netplay::RemoteInputResult::Accepted);
                }
                CHECK(session.Pump(distance+2)); CHECK_EQ(session.SimFrame(),2u);
                for(unsigned index=0;index<distance;++index) { step(); observe(index); }
                CHECK_EQ(session.Restores(),1u); CHECK(!session.Correcting());
            }
            session.Stop();
        }
    }
}

TEST(history_remote_menu_edges) {
    using namespace th13::multiplayer;
    session.Stop();
    App app; CHECK(p32_pin_boot(app, "history-remote-menu.bin"));
    CHECK(app.configure_multiplayer(3, {0, 0, 0}, 0));
    SessionSetup setup; setup.version=2; setup.mode=2; setup.automatic=false;
    setup.session_id=42; setup.build[0]=1; setup.player_count=3;
    const auto words=EncodeSessionSetup(setup); CHECK(session.Configure(words.data(), words.size()));
    app.mp_history_playback = true;
    CHECK(app.start_game(1)); CHECK(app.apply_destination()); CHECK(app.complete_loading());
    std::array<Netplay::FrameInput, 3> inputs{};
    inputs[0].buttons=InputLanes::Dialogue;
    inputs[2].buttons=InputLanes::Pause | InputLanes::Dialogue | 1;
    auto menu=InputLanes::AdvanceHistory(app.run->battle, inputs.data(), inputs.size());
    CHECK(menu.pressed & 0x100u); CHECK(menu.pressed & 1);
    CHECK(app.run->playback_input() == &app.run->battle.seat_inputs[0]);
    CHECK(app.update(menu, false)); CHECK(app.commit(60)); CHECK(app.pause->active());
    inputs[2].buttons=0;
    for (int i=0;i<12;++i) {
        menu=InputLanes::AdvanceHistory(app.run->battle,inputs.data(),inputs.size());
        CHECK(app.update(menu,false));CHECK(app.commit(60));
    }
    CHECK_EQ(app.pause->state.phase,6);
    inputs[2].buttons=InputLanes::Pause | InputLanes::Dialogue;
    menu=InputLanes::AdvanceHistory(app.run->battle,inputs.data(),inputs.size());
    CHECK(menu.pressed & 0x100u);
    CHECK(menu.pressed & 0x200u);
    CHECK(app.update(menu,false));CHECK(app.commit(60));
    inputs[2].buttons=0;
    for(int i=0;i<15;++i) {
        menu=InputLanes::AdvanceHistory(app.run->battle,inputs.data(),inputs.size());
        CHECK(app.update(menu,false));CHECK(app.commit(60));
    }
    CHECK(!app.pause->active());
    session.Stop();
}

TEST(network_pause_restart_and_terminal_save) {
    using namespace th13::multiplayer;
    session.Stop();
    App app;CHECK(p32_pin_boot(app,"network-menu-restart.bin"));
    CHECK(app.configure_multiplayer(2,{0,0,0},0));
    SessionSetup setup;setup.version=2;setup.mode=2;setup.automatic=false;
    setup.session_id=42;setup.build[0]=1;
    const auto words=EncodeSessionSetup(setup);CHECK(session.Configure(words.data(),words.size()));
    CHECK(app.start_game(1));CHECK(app.apply_destination());CHECK(session.BeginLocalSimulation());
    unsigned frame=0;
    const auto tick=[&](u32 remote) {
        CHECK(app.complete_loading());
        CHECK(session.CaptureLocal(0,frame));session.AdvanceCapturePhase();
        CHECK(session.SubmitRemote(1,frame,Netplay::FrameInput(remote))==Netplay::RemoteInputResult::Accepted);
        CHECK(session.CanSimulate());
        CHECK(session.Pump(frame));CHECK_EQ(session.RecordCount(),1u);
        CHECK(app.update(session.MenuInput(),false));CHECK(app.commit(60.f));
        session.OnFrameSimulated();CHECK(app.present());++frame;
    };
    tick(InputLanes::Pause);
    CHECK(app.pause->active());
    for(int i=0;i<12;++i)tick(0);
    CHECK_EQ(app.pause->state.phase,6);
    app.run->battle.seat_huds[0].bombs=0;
    tick(InputLanes::Restart);
    for(int i=0;i<35;++i)tick(0);
    CHECK_EQ(app.progress.stage,1);CHECK(!app.pause->active());
    CHECK_EQ(app.run->battle.seat_huds[0].bombs,2);
    CHECK_EQ(app.run->battle.seat_huds[1].bombs,2);
    Netplay::FrameInput input;
    CHECK(session.CaptureLocal(0x200000u | 0x10000u,frame));
    const auto captured=session.Runtime().Prepare(frame);
    CHECK_EQ(captured.inputs[0].buttons,InputLanes::Restart | InputLanes::Quit);
    bool available=false;unsigned saved=0;
    app.multiplayer_replay_available=[&]{return available;};
    app.multiplayer_replay_save=[&](i32 slot,const std::string& name){
        CHECK_EQ(slot,3);CHECK(name=="NETPLAY");++saved;return true;
    };
    CHECK(!app.can_save_replay());CHECK(!app.result_replay_save_available());
    available=true;CHECK(app.can_save_replay());CHECK(app.result_replay_save_available());
    CHECK(app.save_replay_slot(3,"NETPLAY"));CHECK_EQ(saved,1u);
    CHECK(session.FinishWorldSimulation(true));CHECK(session.WorldCompleted());
    session.Stop();CHECK(!session.WorldCompleted());
}

TEST(v2_dead_slot_poison_and_negative_controls) {
    using namespace th13::multiplayer;
    App app;
    CHECK(p32_enter_run(app, "v2-dead-slot-poison.bin"));
    if (!app.run) return;
    auto& battle = app.run->battle;
    auto& registry = app.animations.registry;

    std::vector<u32> temp_handles;
    for (int i = 0; i < 64; ++i) {
        auto* vm = registry.allocate();
        CHECK(vm != nullptr);
        vm->visual.translation = {float(i * 10), float(-i * 5), 1.f};
        vm->variables.integers[0] = 100 + i;
        temp_handles.push_back(registry.submit(*vm, 0));
    }
    for (int i = 0; i < 48; ++i) {
        if (auto* vm = registry.find(temp_handles[size_t(i)])) {
            CHECK(registry.discard(*vm));
        }
    }
    {
        auto* chain = registry.allocate();
        CHECK(chain != nullptr);
        CHECK(registry.submit(*chain, 4) != 0);
        CHECK(registry.ordered_count(true) >= 1u);
        CHECK(registry.discard(*chain));
        auto* reused = registry.allocate();
        CHECK(reused != nullptr);
        reused->source_script = 0x13572468;
        AnmRegistry::SnapshotData snap;
        std::vector<const AnmVm*> embedded;
        CHECK(registry.capture(snap, embedded));
        const AnmRegistry::SnapshotData::Entry* fresh = nullptr;
        for (const auto& entry : snap.entries)
            if (entry.vm.source_script == 0x13572468) fresh = &entry;
        CHECK(fresh && fresh->active && !fresh->alternate);
        CHECK(registry.discard(*reused));
    }
    GameInput fire;
    for (int f = 0; f < 10; ++f) {
        fire.update(1);
        CHECK(app.update(fire, false) && app.commit(60.f));
    }
    WorldState clean_snap, poisoned_snap;
    CHECK(clean_snap.capture(battle, app.visual, nullptr, app.run->presentation.get(),
                             app.run.get(), app.run_transition.handle()));
    const auto clean_full = clean_snap.digest();
    const auto clean_core = clean_snap.digest_core();

    registry.poison_dead_slots_for_test();
    if (app.run->presentation) app.run->presentation->poison_dead_slots_for_test();
    CHECK(poisoned_snap.capture(battle, app.visual, nullptr, app.run->presentation.get(),
                                app.run.get(), app.run_transition.handle()));
    CHECK_EQ(poisoned_snap.digest(), clean_full);
    CHECK_EQ(poisoned_snap.digest_core(), clean_core);

    for (int f = 0; f < 6; ++f) {
        fire.update(1 | 64);
        CHECK(app.update(fire, false) && app.commit(60.f));
    }
    WorldState expected_end;
    CHECK(expected_end.capture(battle, app.visual, nullptr, app.run->presentation.get(),
                               app.run.get(), app.run_transition.handle()));
    CHECK(clean_snap.restore(battle, app.visual, nullptr, app.run->presentation.get(),
                             app.run.get(), &app.mp_transition_handle_));
    fire = GameInput{};
    for (int f = 0; f < 10; ++f) fire.update(1);
    for (int f = 0; f < 6; ++f) {
        fire.update(1 | 64);
        CHECK(app.update(fire, false) && app.commit(60.f));
    }
    WorldState replayed_end;
    CHECK(replayed_end.capture(battle, app.visual, nullptr, app.run->presentation.get(),
                               app.run.get(), app.run_transition.handle()));
    CHECK_EQ(replayed_end.digest(), expected_end.digest());
    CHECK_EQ(replayed_end.digest_core(), expected_end.digest_core());

    {
        AnmRegistry reg;
        auto* live0 = reg.allocate();
        auto* live1 = reg.allocate();
        CHECK(live0 && live1);
        const u32 h0 = reg.submit(*live0, 0);
        const u32 h1 = reg.submit(*live1, 0);
        CHECK(h0 && h1);
        live1->variables.integers[0] = 999;
        AnmRegistry::SnapshotData valid, corrupted;
        CHECK(reg.capture(valid));
        corrupted = valid;
        corrupted.entries.pop_back();
        CHECK(reg.restore(corrupted));
        CHECK(reg.find(h1) == nullptr);
        AnmRegistry::SnapshotData after_corrupt;
        CHECK(reg.capture(after_corrupt));
        CHECK(after_corrupt.entries.size() != valid.entries.size());
    }

    {
        AnmRegistry reg_a, reg_b;
        auto* a0 = reg_a.allocate(); reg_a.submit(*a0);
        auto* a1 = reg_a.allocate(); reg_a.submit(*a1);
        reg_a.discard(*a0);
        reg_a.discard(*a1);
        AnmRegistry::SnapshotData snap_ok, snap_bad_free;
        CHECK(reg_a.capture(snap_ok));
        snap_bad_free = snap_ok;
        std::swap(snap_bad_free.free_slots.back(), snap_bad_free.free_slots[snap_bad_free.free_slots.size() - 2]);
        CHECK(reg_a.restore(snap_ok));
        CHECK(reg_b.restore(snap_bad_free));
        auto* next_a = reg_a.allocate(); const u32 ha = reg_a.submit(*next_a);
        auto* next_b = reg_b.allocate(); const u32 hb = reg_b.submit(*next_b);
        CHECK(ha != hb);
    }

    {
        AnmRegistry reg;
        AnmRegistry::SnapshotData snap;
        CHECK(reg.capture(snap));
        snap.version = 1;
        CHECK(!reg.restore(snap));
    }
}
