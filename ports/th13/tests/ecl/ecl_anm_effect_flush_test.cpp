#include "ecl_test_fixtures.hpp"

#include "game/AnmRenderer.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"
#include "game/ZunGraphics.hpp"

#include <algorithm>
#include <cstdio>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace th13;

namespace {

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
    i32 stage_script_count = 0;

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
        stage_script_count = i32(stage.scripts.size());
        if (!reader.read("artifacts/reference/raw/st01enm.anm", bytes) ||
            !stage.append(bytes.data(), u32(bytes.size())) || !manager.adopt(4, std::move(stage)))
            return false;
        if (!load(5, "bullet.anm")) return false;
        if (!load(6, "pl00.anm")) return false;
        presentation = std::make_unique<StagePresentation>(manager, renderer, StagePresentation::Banks{4, 5, 6, 8}, 3);
        return true;
    }
};

int execute_literal(StageSim& sim, EclContext& context, u16 opcode, std::initializer_list<u32> args) {
    std::vector<u8> bytes;
    th13test::emit(bytes, 0, opcode, 0, 0xff, 0, args);
    context.instruction = reinterpret_cast<const EclInstruction*>(bytes.data());
    return sim.commands.execute(context, opcode);
}

struct SlotFlushLog {
    int calls = 0;
    size_t queue_at_call = 0;
    u8 kind = 0xff;
    u16 opcode = 0;
    i32 slot = -1, script = -1;
    u32 serial = 0;
    StageEnemy* target = nullptr;
};

}

TEST(slot_commands_flush_synchronously_at_the_ecl_instruction) {
    StageSim sim;
    StageEnemy enemy;
    EclContext context;
    sim.current = &enemy;
    sim.commands.sim = &sim;
    sim.ecl_slot_anm = true;
    sim.anm_effects_noop = false;
    enemy.serial = 42;
    enemy.anm_file = 3;
    SlotFlushLog log;
    sim.flush_anm_slots = [&](StageEnemy* target) {
        ++log.calls;
        log.queue_at_call = sim.anm_effects.size();
        log.target = target;
        if (!sim.anm_effects.empty()) {
            const auto& request = sim.anm_effects.back();
            log.kind = request.kind;
            log.opcode = request.opcode;
            log.slot = request.slot;
            log.script = request.script;
            log.serial = request.serial;
        }
        sim.anm_effects.erase(std::remove_if(sim.anm_effects.begin(), sim.anm_effects.end(),
                                             [](const AnmEffectRequest& request) { return request.kind == 1; }),
                              sim.anm_effects.end());
        return true;
    };

    CHECK_EQ(execute_literal(sim, context, 303, {2, 7}), 0);
    CHECK_EQ(log.calls, 1);
    CHECK_EQ(log.queue_at_call, 1u);
    CHECK(log.target == &enemy);
    CHECK_EQ(log.kind, 1);
    CHECK_EQ(log.opcode, 303);
    CHECK_EQ(log.slot, 2);
    CHECK_EQ(log.script, 7);
    CHECK_EQ(log.serial, 42u);
    CHECK_EQ(enemy.anm_script[2], 7);
    CHECK(sim.anm_effects.empty());

    CHECK_EQ(execute_literal(sim, context, 306, {0, 4}), 0);
    CHECK_EQ(log.calls, 2);
    CHECK_EQ(log.kind, 1);
    CHECK_EQ(log.opcode, 306);
    CHECK_EQ(log.slot, 0);
    CHECK_EQ(log.script, 4);
    CHECK_EQ(enemy.anm_base_script, 4);
    CHECK((enemy.flags & 0x200000u) != 0);
    CHECK(sim.anm_effects.empty());

    CHECK_EQ(execute_literal(sim, context, 313, {0}), 0);
    CHECK_EQ(log.calls, 3);
    CHECK_EQ(log.opcode, 313);
    CHECK_EQ(log.script, 9);
    CHECK(sim.anm_effects.empty());

    CHECK_EQ(execute_literal(sim, context, 316, {0, 1}), 0);
    CHECK_EQ(log.calls, 4);
    CHECK_EQ(log.opcode, 316);
    CHECK_EQ(log.script, 10);
    CHECK(sim.anm_effects.empty());

    CHECK_EQ(execute_literal(sim, context, 318, {}), 0);
    CHECK_EQ(log.calls, 5);
    CHECK_EQ(log.opcode, 318);
    CHECK_EQ(log.script, 4);
    CHECK(sim.anm_effects.empty());
}

TEST(effect_commands_stay_queued_for_the_stage_script_phase) {
    StageSim sim;
    StageEnemy enemy;
    EclContext context;
    sim.current = &enemy;
    sim.commands.sim = &sim;
    sim.ecl_slot_anm = true;
    sim.anm_effects_noop = false;
    enemy.serial = 7;
    enemy.anm_file = 3;
    enemy.position[0] = 12.5f;
    enemy.position[1] = 34.25f;
    enemy.position[2] = 0.75f;
    int flush_calls = 0;
    sim.flush_anm_slots = [&](StageEnemy*) {
        ++flush_calls;
        sim.anm_effects.erase(std::remove_if(sim.anm_effects.begin(), sim.anm_effects.end(),
                                             [](const AnmEffectRequest& request) { return request.kind == 1; }),
                              sim.anm_effects.end());
        return true;
    };

    CHECK_EQ(execute_literal(sim, context, 307, {1, 91}), 0);
    CHECK_EQ(flush_calls, 0);
    CHECK_EQ(sim.anm_effects.size(), 1u);
    if (!sim.anm_effects.empty()) {
        const auto& request = sim.anm_effects[0];
        CHECK_EQ(request.kind, 0);
        CHECK_EQ(request.file, 1);
        CHECK_EQ(request.script, 91);
        CHECK_EQ(request.x, 12.5f);
        CHECK_EQ(request.y, 34.25f);
        CHECK_EQ(request.z, 0.75f);
    }

    CHECK_EQ(execute_literal(sim, context, 308, {1, 92}), 0);
    CHECK_EQ(flush_calls, 0);
    CHECK_EQ(sim.anm_effects.size(), 2u);
    if (sim.anm_effects.size() == 2u) {
        const auto& request = sim.anm_effects[1];
        CHECK_EQ(request.kind, 0);
        CHECK_EQ(request.file, 1);
        CHECK_EQ(request.script, 92);
        CHECK_EQ(request.x, 0.0f);
        CHECK_EQ(request.y, 0.0f);
        CHECK_EQ(request.z, 0.0f);
    }

    CHECK_EQ(execute_literal(sim, context, 303, {0, 0}), 0);
    CHECK_EQ(flush_calls, 1);
    CHECK_EQ(sim.anm_effects.size(), 2u);
    if (sim.anm_effects.size() == 2u) {
        CHECK_EQ(sim.anm_effects[0].script, 91);
        CHECK_EQ(sim.anm_effects[1].script, 92);
    }
}

TEST(presentation_slots_only_flush_defers_effect_creation_to_sync) {
    PresentationRig rig;
    th13test::TestFileReader reader;
    CHECK(rig.load_real_banks(reader));
    if (!rig.presentation) return;

    StageBattle battle;
    StageEnemy enemy;
    enemy.serial = 9;
    enemy.mirror_script_base = 1;
    battle.sim.anm_effects.push_back({3, 0, 0, 0, 0, 1, 306, 0, 9});
    battle.sim.anm_effects.push_back({0, 0, 12.5f, 34.25f, 0.75f, 0});
    const u32 before = rig.manager.registry.ordered_count(false);

    CHECK(rig.presentation->flush_ecl_animations(battle, &enemy, true));
    CHECK(enemy.anm_slot_handle[0] != 0);
    CHECK_EQ(rig.manager.registry.ordered_count(false), before + 1);
    CHECK_EQ(battle.sim.anm_effects.size(), 1u);
    if (!battle.sim.anm_effects.empty()) CHECK_EQ(battle.sim.anm_effects[0].kind, 0);
    const u32 slot_handle = enemy.anm_slot_handle[0];
    {
        const AnmVm* vm = rig.manager.registry.find(slot_handle);
        CHECK(vm != nullptr);
        if (vm) {
            CHECK_EQ(vm->source_script, rig.stage_script_count);
            CHECK_EQ(vm->visual.layer, 8);
        }
    }

    const u32 marker = rig.manager.create(5, 3, -1, 0);
    CHECK(marker != 0);

    CHECK(rig.presentation->flush_ecl_animations(battle));
    CHECK(battle.sim.anm_effects.empty());
    CHECK_EQ(rig.manager.registry.ordered_count(false), before + 3);
    const u32 effect = rig.manager.registry.ordered_handle(false, 0);
    CHECK(effect != 0);
    CHECK(effect != slot_handle);
    CHECK(effect != marker);
    {
        const AnmVm* vm = rig.manager.registry.find(effect);
        CHECK(vm != nullptr);
        if (vm) {
            CHECK_EQ(vm->source_script, 0);
            CHECK_EQ(vm->visual.translation.x, 12.5f);
            CHECK_EQ(vm->visual.translation.y, 34.25f);
            CHECK_EQ(vm->visual.translation.z, 0.75f);
        }
    }
    CHECK_EQ(rig.manager.registry.ordered_handle(false, 1), slot_handle);
    CHECK_EQ(rig.manager.registry.ordered_handle(false, 2), marker);

    battle.sim.anm_effects.push_back({0, 4, 0, 0, 0, 0});
    CHECK(rig.presentation->flush_ecl_animations(battle));
    CHECK(battle.sim.anm_effects.empty());
    const u32 zero = rig.manager.registry.ordered_handle(false, 0);
    CHECK(zero != 0);
    CHECK(zero != effect);
    {
        const AnmVm* vm = rig.manager.registry.find(zero);
        CHECK(vm != nullptr);
        if (vm) {
            CHECK_EQ(vm->source_script, 4);
            CHECK_EQ(vm->visual.translation.x, 0.0f);
            CHECK_EQ(vm->visual.translation.y, 0.0f);
            CHECK_EQ(vm->visual.translation.z, 0.0f);
        }
    }
    CHECK_EQ(rig.manager.registry.ordered_handle(false, 1), effect);
}

TEST(slot_flush_failure_is_reported_without_swallowing_the_error) {
    StageSim sim;
    StageEnemy enemy;
    EclContext context;
    sim.current = &enemy;
    sim.commands.sim = &sim;
    sim.ecl_slot_anm = true;
    sim.anm_effects_noop = false;
    enemy.anm_file = 3;
    sim.flush_anm_slots = [](StageEnemy*) { return false; };
    CHECK_EQ(execute_literal(sim, context, 303, {0, 0}), -2);
    CHECK(!sim.error_.empty());
}

int main() { return th13test::run_all("ecl-anm-effect-flush"); }
