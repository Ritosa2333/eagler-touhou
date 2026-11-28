#include "../harness/test_framework.hpp"

#include "game/AnmManager.hpp"
#include "game/AnmRenderer.hpp"
#include "game/AnmResource.hpp"
#include "game/ItemManager.hpp"
#include "game/SessionState.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"

#include <cstdio>
#include <vector>

namespace {

using namespace th13;

bool read_file(const char* relative, std::vector<u8>& out) {
    for (const char* base : {".", "..", "../../..", "../../../.."}) {
        std::string path = std::string(base) + "/" + relative;
        FILE* f = std::fopen(path.c_str(), "rb");
        if (!f) continue;
        std::fseek(f, 0, SEEK_END);
        long n = std::ftell(f);
        std::fseek(f, 0, SEEK_SET);
        out.resize(n);
        std::fread(out.data(), 1, n, f);
        std::fclose(f);
        return true;
    }
    return false;
}

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

struct PresentationFixture {
    GameRandom random{0x50};
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    StagePresentation presentation;

    PresentationFixture(const char* player_anm = "artifacts/reference/raw/pl00.anm")
        : presentation(manager, renderer, StagePresentation::Banks{4, 5, 6}, 0) {
        std::vector<u8> bytes;
        AnmResource stage;
        CHECK(read_file("artifacts/reference/raw/enemy.anm", bytes));
        CHECK(stage.open(bytes.data(), u32(bytes.size())));
        CHECK(read_file("artifacts/reference/raw/st01enm.anm", bytes));
        CHECK(stage.append(bytes.data(), u32(bytes.size())));
        CHECK(manager.adopt(4, std::move(stage)));
        CHECK(read_file("artifacts/reference/raw/bullet.anm", bytes));
        CHECK(manager.load(5, bytes.data(), u32(bytes.size())));
        CHECK(read_file(player_anm, bytes));
        CHECK(manager.load(6, bytes.data(), u32(bytes.size())));
        CHECK(read_file("artifacts/reference/raw/astral.anm", bytes));
        CHECK(manager.load(25, bytes.data(), u32(bytes.size())));
    }
};

ItemCollector collector_for(SessionState& progress, HudPlayerState& player, HudScoreState& score, float x, float y) {
    ItemCollector collector;
    collector.x = x;
    collector.y = y;
    collector.state = 1;
    collector.homing_speed = 5.0f;
    collector.box = {x - 15.0f, y - 15.0f, x + 15.0f, y + 15.0f};
    collector.progress = &progress;
    collector.player = &player;
    collector.score = &score;
    progress.difficulty = 1;
    progress.character = 0;
    return collector;
}

}

TEST(spirit_vm_submit_layer8_and_follow) {
    PresentationFixture fx;
    StageBattle battle;
    battle.item_manager.bind_random(battle.sim.random);
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    battle.item_manager.spawn_value_item(0, 10.0f, 300.0f, 0.0f, 0.5f);
    Spirit& s = battle.item_manager.spirits()[0];
    s.x = 10.0f;
    s.y = 300.0f;
    CHECK(fx.presentation.sync(battle, 0));
    CHECK(fx.presentation.error().empty());
    AnmVm* vm = nullptr;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 1) {
                vm = candidate;
                break;
            }
    CHECK(vm != nullptr);
    if (!vm) return;
    CHECK(!vm->visible);
    CHECK_EQ(vm->visual.layer, 6);
    CHECK_EQ(vm->visual.translation.y, 300.0f);
    CHECK_NEAR(vm->visual.translation.x, 10.0f, 1e-5f);
    bool saw_child_sprite = false;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* child = fx.manager.registry.find(h))
            if (child->source_script == 2 || child->source_script == 3) {
                saw_child_sprite = true;
                CHECK(child->sprite != nullptr);
                if (child->sprite) CHECK_EQ(child->sprite->width, 30.0f);
            }
    CHECK(saw_child_sprite);
    s.y = 280.0f;
    CHECK(fx.presentation.sync(battle, 0));
    CHECK_NEAR(vm->visual.translation.y, 280.0f, 1e-5f);
    CHECK_NEAR(vm->visual.translation.x, 10.0f, 1e-5f);
}

TEST(spirit_vm_kind1_and_trance_variant_script) {
    PresentationFixture fx;
    StageBattle battle;
    battle.item_manager.bind_random(battle.sim.random);
    battle.item_manager.spawn_value_item(1, 0.0f, 300.0f, 0.0f, 0.5f);
    CHECK(fx.presentation.sync(battle, 0));
    bool saw7 = false;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 7) saw7 = true;
    CHECK(saw7);

    SessionState session;
    trance_begin(session);
    battle.item_manager.bind_session(session);
    battle.item_manager.spawn_value_item(2, 40.0f, 300.0f, 0.0f, 0.5f);
    CHECK(fx.presentation.sync(battle, 0));
    bool saw16 = false;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 16) saw16 = true;
    CHECK(saw16);
}

TEST(spirit_vm_retire_on_collect) {
    PresentationFixture fx;
    StageBattle battle;
    battle.item_manager.bind_random(battle.sim.random);
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    battle.item_manager.spawn_value_item(0, 5.0f, 205.0f, 0.0f, 0.5f);
    Spirit& s = battle.item_manager.spirits()[0];
    s.x = 5.0f;
    s.y = 205.0f;
    CHECK(fx.presentation.sync(battle, 0));
    AnmVm* vm = nullptr;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 1) {
                vm = candidate;
                break;
            }
    CHECK(vm != nullptr);
    auto collector = collector_for(progress, player, score, 0.0f, 200.0f);
    battle.item_manager.update_spirits(collector);
    CHECK(!s.alive);
    CHECK_EQ(progress.ending_points, 2);
    CHECK_EQ(player.life_pieces, 1);
    CHECK(fx.presentation.sync(battle, 0));
    CHECK(fx.manager.update());
    bool still = false;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 1) still = true;
    CHECK(!still);
}

TEST(spirit_vm_retire_on_cull) {
    PresentationFixture fx;
    StageBattle battle;
    battle.item_manager.bind_random(battle.sim.random);
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    battle.item_manager.spawn_value_item(3, 230.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s = battle.item_manager.spirits()[0];
    s.x = 230.0f;
    s.y = 200.0f;
    CHECK(fx.presentation.sync(battle, 0));
    auto collector = collector_for(progress, player, score, 0.0f, 200.0f);
    battle.item_manager.update_spirits(collector);
    CHECK(!s.alive);
    CHECK(fx.presentation.sync(battle, 0));
    CHECK(fx.manager.update());
    bool still = false;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 19) still = true;
    CHECK(!still);
}

TEST(stage_file3_script_offset_resolves_local_to_global) {
    PresentationFixture fx;
    StageBattle battle;
    AnmEffectRequest req;
    req.kind = 0;
    req.x = 10.0f;
    req.y = 20.0f;
    req.file = 3;
    req.script = 0;
    battle.sim.anm_effects.push_back(req);
    CHECK(fx.presentation.sync(battle, 0));
    bool saw139 = false;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 139) saw139 = true;
    CHECK(saw139);
    battle.sim.anm_effects.clear();
    req.file = 2;
    req.script = 0;
    battle.sim.anm_effects.push_back(req);
    CHECK(fx.presentation.sync(battle, 0));
    bool saw0 = false;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 0) saw0 = true;
    CHECK(saw0);
    battle.sim.anm_effects.clear();
    req.file = 4;
    battle.sim.anm_effects.push_back(req);
    CHECK(!fx.presentation.sync(battle, 0));
    CHECK(!fx.presentation.error().empty());
}

TEST(item_vm_two_scripts_layer15_and_collect_retire) {
    PresentationFixture fx;
    StageBattle battle;
    battle.item_manager.bind_random(battle.sim.random);
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    battle.item_manager.spawn_item(10.0f, 205.0f, 0.0f, 1, -1.5707964f, 2.2f);
    CHECK(fx.presentation.sync(battle, 0));
    CHECK(fx.presentation.error().empty());
    const AnmVm* body = fx.presentation.item_body_vm(599);
    const AnmVm* side = fx.presentation.item_side_vm(599);
    CHECK(body != nullptr);
    CHECK(side != nullptr);
    bool chained_item_vm = false;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script >= 89 && candidate->source_script <= 107) chained_item_vm = true;
    CHECK(!chained_item_vm);
    if (body && side) {
        CHECK_EQ(body->source_script, 90);
        CHECK_EQ(side->source_script, 99);
        CHECK_EQ(body->visual.layer, 15);
        CHECK_EQ(side->visual.layer, 15);
        CHECK_NEAR(body->visual.translation.x, 10.0f, 1e-4f);
        CHECK_NEAR(body->visual.translation.y, 205.0f, 1e-4f);
        CHECK(body->visible && side->visible);
    }
    auto collector = collector_for(progress, player, score, 10.0f, 205.0f);
    battle.item_manager.update(collector);
    CHECK(!battle.item_manager.items()[599].alive);
    CHECK_EQ(player.power, 101);
    CHECK(fx.presentation.sync(battle, 0));
    CHECK(fx.manager.update());
    CHECK(fx.presentation.item_body_vm(599) == nullptr);
    CHECK(fx.presentation.item_side_vm(599) == nullptr);
}

TEST(youmu_slash_vm_script19_layer11_and_lifecycle) {
    PresentationFixture fx("artifacts/reference/raw/pl03.anm");
    StageBattle battle;
    battle.youmu_bomb_.reset();
    auto spawn = battle.youmu_bomb_.step(0, battle.sim.random);
    CHECK(spawn.has_value());
    if (!spawn) return;
    CHECK_EQ(spawn->script, 19);
    CHECK_EQ(spawn->slot, 0);
    battle.youmu_slash_spawns_.push_back(*spawn);
    CHECK(fx.presentation.sync(battle, 0));
    battle.youmu_slash_spawns_.clear();
    CHECK(fx.presentation.error().empty());
    AnmVm* vm = nullptr;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 19) {
                vm = candidate;
                break;
            }
    CHECK(vm != nullptr);
    if (!vm) return;
    CHECK_EQ(vm->visual.layer, 11);
    CHECK_NEAR(vm->visual.translation.x, 0.0f, 1e-5f);
    CHECK_NEAR(vm->visual.translation.y, spawn->y, 1e-4f);
    CHECK(vm->variables.rotation.z != 0.0f);
    CHECK(battle.youmu_bomb_.has_anm(0));
    CHECK(battle.youmu_bomb_.active(0));
    CHECK_NEAR(battle.youmu_bomb_.angle(0), vm->variables.rotation.z, 1e-6f);
    for (int i = 0; i < 24 && battle.youmu_bomb_.has_anm(0); ++i) {
        CHECK(fx.manager.update());
        CHECK(fx.presentation.sync(battle, 0));
    }
    CHECK(!battle.youmu_bomb_.has_anm(0));
    CHECK(!battle.youmu_bomb_.active(0));
}

TEST(power_rebuild_creates_option_trees_with_deferred_translation) {
    PresentationFixture fx("artifacts/reference/raw/pl03.anm");
    StageBattle battle;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    struct ShtProvider : EclResourceProvider {
        std::vector<u8> sht;
        bool read(const std::string&, std::vector<u8>& bytes) override {
            bytes = sht;
            return true;
        }
        bool animation(u32, const std::string&) override { return true; }
    } provider;
    CHECK(read_file("artifacts/reference/raw/pl03a.sht", provider.sht));
    progress.character = 3;
    progress.stage = 2;
    CHECK(battle.prepare_player(provider, progress, player, "player.sht", true));
    battle.restore_replay_player(0, 51200, false);
    CHECK(fx.presentation.sync_player_options(battle));
    player.power = 300;
    battle.restore_replay_player(6400, 12800, false);
    const u32 before = fx.random.index();
    CHECK(fx.presentation.sync_player_options(battle, true));
    CHECK_EQ(fx.random.index() - before, 12u);
    int found = 0;
    for (u32 i = 0; i < fx.manager.registry.ordered_count(false); ++i) {
        auto* vm = fx.manager.registry.find(fx.manager.registry.ordered_handle(false, i));
        if (!vm || vm->source_script != 15 || (vm->visual.render_flags & 0x20)) continue;
        ++found;
        CHECK_EQ(vm->visual.translation.x, 0.f);
        CHECK_EQ(vm->visual.translation.y, -32.f);
        CHECK_EQ(fx.manager.registry.children(*vm).size(), size_t(1));
    }
    CHECK_EQ(found, 3);
    CHECK(fx.presentation.sync_player_options(battle));
    CHECK(fx.presentation.sync_player_options(battle));
    found = 0;
    for (u32 i = 0; i < fx.manager.registry.ordered_count(false); ++i) {
        auto* vm = fx.manager.registry.find(fx.manager.registry.ordered_handle(false, i));
        if (!vm || vm->source_script != 15 || (vm->visual.render_flags & 0x20)) continue;
        ++found;
        CHECK_EQ(vm->visual.translation.x, 50.f);
        CHECK_EQ(vm->visual.translation.y, 100.f);
    }
    CHECK_EQ(found, 3);
}

int main() { return th13test::run_all("items-presentation") ? 1 : 0; }
