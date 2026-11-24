#include "ecl_test_fixtures.hpp"

#include "game/AnmRenderer.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"
#include "game/ZunGraphics.hpp"

#include <cstdio>
#include <map>
#include <string>
#include <vector>

using namespace th13;
using th13test::bits_of;

namespace {

std::vector<u8> flip_stage_ecl() {
    std::vector<u8> main;
    th13test::emit(main, 0, 302, 0, 0xff, 0, {3});
    th13test::emit(main, 0, 306, 0, 0xff, 0, {0, 0});
    th13test::emit(main, 0, 23, 0, 0xff, 0, {1});
    th13test::emit(main, 0, 313, 0, 0xff, 0, {0});
    th13test::emit(main, 0, 23, 0, 0xff, 0, {1});
    th13test::emit(main, 0, 316, 0, 0xff, 0, {0, 1});
    th13test::emit(main, 0, 23, 0, 0xff, 0, {999});
    th13test::emit(main, 0, 10, 0, 0xff, 0, {});
    return th13test::build_scpt({{"main", main}}, {"enemy.anm", "st01enm.anm"});
}

class Provider final : public EclResourceProvider {
public:
    std::map<std::string, std::vector<u8>> overrides;
    std::vector<std::string>* anim_list = nullptr;

    bool read(const std::string& name, std::vector<u8>& data) override {
        const auto found = overrides.find(name);
        if (found != overrides.end()) {
            data = found->second;
            return true;
        }
        return reader.read("artifacts/reference/raw/" + name, data);
    }
    bool animation(u32, const std::string& name) override {
        std::vector<u8> probe;
        if (!read(name, probe)) return false;
        if (anim_list) anim_list->push_back(name);
        return true;
    }

private:
    th13test::TestFileReader reader;
};

struct Host final : StageBattleServices {
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
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

struct PresentationRig {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    std::unique_ptr<StagePresentation> presentation;
    i32 enemy_script_count = 0;

    bool load_real_banks(th13test::TestFileReader& reader) {
        const auto load = [&](i32 bank, const char* name) {
            std::vector<u8> bytes;
            if (!reader.read(std::string("artifacts/reference/raw/") + name, bytes)) return false;
            return manager.load(bank, bytes.data(), u32(bytes.size()));
        };
        std::vector<u8> bytes;
        AnmResource stage;
        if (!reader.read("artifacts/reference/raw/enemy.anm", bytes) || !stage.append(bytes.data(), u32(bytes.size())))
            return false;
        enemy_script_count = i32(stage.scripts.size());
        if (!reader.read("artifacts/reference/raw/st01enm.anm", bytes) ||
            !stage.append(bytes.data(), u32(bytes.size())) || !manager.adopt(4, std::move(stage)))
            return false;
        if (!load(5, "bullet.anm")) return false;
        if (!load(6, "pl00.anm")) return false;
        presentation = std::make_unique<StagePresentation>(manager, renderer, StagePresentation::Banks{4, 5, 6, 8}, 3);
        presentation->enemies_from_ecl = true;
        return true;
    }
};

bool step_until_effects(StageBattle& battle, u32& guard) {
    while (battle.sim.anm_effects.empty()) {
        if (!guard--) return false;
        GameInput input;
        if (!battle.step(input)) return false;
    }
    return true;
}

bool step_one(StageBattle& battle, u32& guard) {
    if (!guard--) return false;
    GameInput input;
    return battle.step(input);
}

}

TEST(anm_flip_commands_decode_and_submit) {
    {
        Provider provider;
        provider.overrides["st01.ecl"] = flip_stage_ecl();
        Host host;
        SessionState progress;
        HudPlayerState player;
        HudScoreState score;
        StageBattle battle;
        provider.anim_list = &battle.sim.anim_list;
        battle.sim.anm_effects_noop = true;
        CHECK(battle.load(provider, "st01.ecl", 1 << 3, host, progress, player, score, nullptr, "pl00a.sht"));
        u32 guard = 240;
        for (u32 i = 0; i < 120 && guard; ++i)
            CHECK(step_one(battle, guard));
        CHECK(battle.sim.anm_effects.empty());
    }

    Provider provider;
    provider.overrides["st01.ecl"] = flip_stage_ecl();
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    provider.anim_list = &battle.sim.anim_list;
    battle.sim.anm_effects_noop = false;
    battle.sim.ecl_slot_anm = true;
    CHECK(battle.load(provider, "st01.ecl", 1 << 3, host, progress, player, score, nullptr, "pl00a.sht"));

    PresentationRig rig;
    th13test::TestFileReader reader;
    CHECK(rig.load_real_banks(reader));

    u32 guard = 600;
    CHECK(step_until_effects(battle, guard));
    CHECK_EQ(battle.sim.anm_effects.size(), 1u);
    {
        const auto& request = battle.sim.anm_effects[0];
        CHECK_EQ(request.opcode, 306);
        CHECK_EQ(request.kind, 1);
        CHECK_EQ(request.file, 3);
        CHECK_EQ(request.script, 0);
        CHECK_EQ(request.slot, 0);
    }
    StageEnemy* root = nullptr;
    for (auto& enemy : battle.sim.enemies)
        if (enemy->serial == battle.sim.anm_effects[0].serial) root = enemy.get();
    CHECK(root != nullptr);
    if (!root) return;
    bool synced = rig.presentation->sync(battle, 0);
    CHECK(synced);
    if (!synced) {
        std::printf("  presentation error: %s\n", rig.presentation->error().c_str());
        return;
    }
    CHECK(root->anm_slot_handle[0] != 0);
    {
        const AnmVm* vm = rig.manager.registry.find(root->anm_slot_handle[0]);
        CHECK(vm != nullptr);
        CHECK_EQ(vm->source_script, rig.enemy_script_count);
        CHECK_EQ(vm->visual.layer, 8);
    }
    CHECK(battle.sim.anm_effects.empty());

    CHECK(step_until_effects(battle, guard));
    CHECK_EQ(battle.sim.anm_effects.size(), 1u);
    {
        const auto& request = battle.sim.anm_effects[0];
        CHECK_EQ(request.opcode, 313);
        CHECK_EQ(request.kind, 1);
        CHECK_EQ(request.file, 3);
        CHECK_EQ(request.script, 5);
        CHECK_EQ(request.slot, 0);
        CHECK_EQ(request.serial, root->serial);
    }
    synced = rig.presentation->sync(battle, 0);
    CHECK(synced);
    if (!synced) {
        std::printf("  presentation error: %s\n", rig.presentation->error().c_str());
        return;
    }
    CHECK(root->anm_slot_handle[0] != 0);
    {
        const AnmVm* vm = rig.manager.registry.find(root->anm_slot_handle[0]);
        CHECK(vm != nullptr);
        CHECK_EQ(vm->source_script, rig.enemy_script_count + 5);
        CHECK_EQ(vm->visual.layer, 8);
    }

    CHECK(step_until_effects(battle, guard));
    CHECK_EQ(battle.sim.anm_effects.size(), 1u);
    {
        const auto& request = battle.sim.anm_effects[0];
        CHECK_EQ(request.opcode, 316);
        CHECK_EQ(request.kind, 1);
        CHECK_EQ(request.file, 3);
        CHECK_EQ(request.script, 6);
        CHECK_EQ(request.slot, 0);
    }
    synced = rig.presentation->sync(battle, 0);
    CHECK(synced);
    if (!synced) {
        std::printf("  presentation error: %s\n", rig.presentation->error().c_str());
        return;
    }
    CHECK(root->anm_slot_handle[0] != 0);
    {
        const AnmVm* vm = rig.manager.registry.find(root->anm_slot_handle[0]);
        CHECK(vm != nullptr);
        CHECK_EQ(vm->source_script, rig.enemy_script_count + 6);
        CHECK_EQ(vm->visual.layer, 8);
    }

    CHECK(root->anm_size_a[0] != 0.0f || root->anm_size_a[1] != 0.0f);
    CHECK((root->flags & 0x200000u) != 0);

    EclContext context;
    battle.sim.current = root;
    const auto execute = [&](u16 op, std::initializer_list<u32> args) {
        std::vector<u8> bytes;
        th13test::emit(bytes, 0, op, 0, 255, 0, args);
        context.instruction = reinterpret_cast<const EclInstruction*>(bytes.data());
        return battle.sim.commands.execute(context, op);
    };
    CHECK_EQ(execute(303, {3, 0}), 0);
    CHECK_EQ(execute(320, {3, bits_of(-1.f), bits_of(224.f)}), 0);
    CHECK_EQ(root->anm_slot_offset[3].x, -1.f);
    CHECK_EQ(root->anm_slot_offset[3].y, 224.f);
    CHECK_EQ(root->anm_slot_offset[3].z, 0.f);
    CHECK(rig.presentation->sync(battle, 0));
    const AnmVm* offset_vm = rig.manager.registry.find(root->anm_slot_handle[3]);
    CHECK(offset_vm != nullptr);
    if (offset_vm) {
        CHECK_EQ(offset_vm->visual.translation.x, root->position[0] - 1.f);
        CHECK_EQ(offset_vm->visual.translation.y, root->position[1] + 224.f);
        root->flags |= 0x8000000u;
        CHECK(rig.presentation->sync(battle, 0));
        CHECK_EQ(offset_vm->visual.translation.x, root->position[0]);
        CHECK_EQ(offset_vm->visual.translation.y, root->position[1]);
    }
    battle.sim.current = nullptr;
}

int main() { return th13test::run_all("ecl-effect-commands"); }
