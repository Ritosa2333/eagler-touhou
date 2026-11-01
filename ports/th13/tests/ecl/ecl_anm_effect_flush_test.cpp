// ECL 30x ANM 创建请求的相位分流测试（B3）。
//
// 原版锚点（capstone 实证，见 tools/b3_enemy_phase_probe.py 与报告 evidence）：
//   303  → 0x41fe80：0x470110 回收旧槽 → 0x4243e0（0x403800 模板拷贝 →
//         0x462450 立即 tick → 0x46f4a0 主池头插）→ 句柄写回敌机 +0x120
//         槽数组（0x41ff31/0x41ff38）。306 → 0x41b7cc 同构（+0x278+7 层、
//         slot0 尺寸 0x3f30/0x3f34）。313/316 汇入 0x41b93c 共享尾。
//         ——全部在 ECL 解释器内同步完成，指令返回前 VM 已存在。
//   307  → 0x41b510：内联 0x403800 + 0x462450(0x41b5c1) + 0x46f4a0(0x41b5cd)
//         头插，位置 = 敌机 VM+0x44（敌机 xyz）。
//   308  → 0x41b63e → 0x4243e0：位置清零（0x42443d-0x424462）+ 0x46f4a0 头插。
//   0x41b260 任务序：enemy_manager(0x418e00) 之后、main_chain(0x46f170) 之前。
//   port 的 stage root（serial 1 'main'）在敌机向量内先于 Boss（serial 174）
//   tick，故 kind=0 特效请求必须留到 battle.step 之后的 sync（stage_script
//   相位）排空；enemy_animation（0x41ffb0 变更前解析，且经 in_bounds 在敌机
//   循环内触发）只允许冲刷 kind=1 槽位请求。
//
// 本测试覆盖：
//   1) 303/306/313/316/318 在 execute() 内同步调用 flush_anm_slots；
//   2) 307/308 不入同步路径，保持在 anm_effects 队列（307 敌机位置、308 零位置）；
//   3) 表现层 slots_only 冲刷只建槽位 VM，kind=0 保留到全量 sync 冲刷；
//   4) 全量冲刷按 ordering=2 头插：307 敌机位置、308 零位置。
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

// ZunGraphics 纯虚桩（同 ecl_effect_commands_test：逻辑帧验证，不落 GPU）。
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

// 真实 ANM bank：4 = enemy.anm + st01enm.anm 合并 stage 表、5 = bullet.anm、
// 6 = pl00.anm。effect.anm（bank 8）在本容器资源面无法装载（模板 32 精灵
// 缺失，既有 ecl_effect_commands_test 同注），故 kind=0 用例用文件 0
// （bullet.anm）承载 307/308 的创建请求——冲刷逻辑与文件索引无关，ECL 侧
// 的 file/script 解码由用例 1/2 覆盖。
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
            if (!reader.read(std::string("artifacts/reference/raw/") + name, bytes))
                return false;
            return manager.load(bank, bytes.data(), u32(bytes.size()));
        };
        std::vector<u8> bytes;
        AnmResource stage;
        if (!reader.read("artifacts/reference/raw/enemy.anm", bytes) ||
            !stage.append(bytes.data(), u32(bytes.size()))) return false;
        stage_script_count = i32(stage.scripts.size());
        if (!reader.read("artifacts/reference/raw/st01enm.anm", bytes) ||
            !stage.append(bytes.data(), u32(bytes.size())) ||
            !manager.adopt(4, std::move(stage))) return false;
        if (!load(5, "bullet.anm")) return false;
        if (!load(6, "pl00.anm")) return false;
        presentation = std::make_unique<StagePresentation>(
            manager, renderer, StagePresentation::Banks{4, 5, 6, 8}, 3);
        return true;
    }
};

// 单条 ECL 指令执行夹具：mask=0（全部实参为常量）、rank=0xff。
int execute_literal(StageSim& sim, EclContext& context, u16 opcode,
                    std::initializer_list<u32> args) {
    std::vector<u8> bytes;
    th13test::emit(bytes, 0, opcode, 0, 0xff, 0, args);
    context.instruction = reinterpret_cast<const EclInstruction*>(bytes.data());
    return sim.commands.execute(context, opcode);
}

// 记录指令级槽位冲刷。mimic 表现层：只清 kind=1 请求（kind=0 保留）。
struct SlotFlushLog {
    int calls = 0;
    size_t queue_at_call = 0;
    u8 kind = 0xff;
    u16 opcode = 0;
    i32 slot = -1, script = -1;
    u32 serial = 0;
    StageEnemy* target = nullptr;
};

}  // namespace

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
        // 表现层冲刷语义：kind=1 建 VM 后出队，kind=0 保留。
        sim.anm_effects.erase(
            std::remove_if(sim.anm_effects.begin(), sim.anm_effects.end(),
                           [](const AnmEffectRequest& request) { return request.kind == 1; }),
            sim.anm_effects.end());
        return true;
    };

    // 303 anmScript(slot=2, script=7)：指令返回前同步建 VM。
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

    // 306 anmRun(slot=0, script=4)：记录 +0x26c 基与镜像门。
    CHECK_EQ(execute_literal(sim, context, 306, {0, 4}), 0);
    CHECK_EQ(log.calls, 2);
    CHECK_EQ(log.kind, 1);
    CHECK_EQ(log.opcode, 306);
    CHECK_EQ(log.slot, 0);
    CHECK_EQ(log.script, 4);
    CHECK_EQ(enemy.anm_base_script, 4);
    CHECK((enemy.flags & 0x200000u) != 0);
    CHECK(sim.anm_effects.empty());

    // 313 anmFlip(slot=0)：脚本 = +0x26c 基 + 5。
    CHECK_EQ(execute_literal(sim, context, 313, {0}), 0);
    CHECK_EQ(log.calls, 3);
    CHECK_EQ(log.opcode, 313);
    CHECK_EQ(log.script, 9);
    CHECK(sim.anm_effects.empty());

    // 316 anmFlipDelta(slot=0, delta=1)：脚本 = 基 + 1 + 5。
    CHECK_EQ(execute_literal(sim, context, 316, {0, 1}), 0);
    CHECK_EQ(log.calls, 4);
    CHECK_EQ(log.opcode, 316);
    CHECK_EQ(log.script, 10);
    CHECK(sim.anm_effects.empty());

    // 318：恢复 slot0 到 +0x26c 基。
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
        sim.anm_effects.erase(
            std::remove_if(sim.anm_effects.begin(), sim.anm_effects.end(),
                           [](const AnmEffectRequest& request) { return request.kind == 1; }),
            sim.anm_effects.end());
        return true;
    };

    // 307 anmAt(file=1, script=91)：保留敌机 xyz，不入同步冲刷。
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

    // 308 anmVm(file=1, script=92)：位置清零，不入同步冲刷。
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

    // 同一 ECL 回合内 303 的同步冲刷不得顺带排空 kind=0。
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
    battle.sim.anm_effects.push_back(
        {3, 0, 0, 0, 0, 1, 306, 0, 9});              // 306 slot0 script0（stage 文件 3）
    battle.sim.anm_effects.push_back(
        {0, 0, 12.5f, 34.25f, 0.75f, 0});            // 307 特效（file 0 = bullet bank）
    const u32 before = rig.manager.registry.ordered_count(false);

    // enemy_animation 路径（slots_only）：只建槽位 VM，kind=0 保留。
    CHECK(rig.presentation->flush_ecl_animations(battle, &enemy, true));
    CHECK(enemy.anm_slot_handle[0] != 0);
    CHECK_EQ(rig.manager.registry.ordered_count(false), before + 1);
    CHECK_EQ(battle.sim.anm_effects.size(), 1u);
    if (!battle.sim.anm_effects.empty())
        CHECK_EQ(battle.sim.anm_effects[0].kind, 0);
    const u32 slot_handle = enemy.anm_slot_handle[0];
    {
        const AnmVm* vm = rig.manager.registry.find(slot_handle);
        CHECK(vm != nullptr);
        if (vm) {
            CHECK_EQ(vm->source_script, rig.stage_script_count);  // 文件 3 局部 0
            CHECK_EQ(vm->visual.layer, 8);                        // +0x278 + 7
        }
    }

    // 尾插标记 VM：全量冲刷的 307 必须以 ordering=2 头插到它之前。
    // bullet.anm 中本容器可创建（精灵可用）的脚本：0、3..9；script1/2 在
    // 该资源面初始 tick 报 "Animation sprite not available"，故取 3。
    const u32 marker = rig.manager.create(5, 3, -1, 0);
    CHECK(marker != 0);

    // sync 相位（全量）：创建 307 特效，位置 = 敌机 xyz、头插。
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

    // 308：零位置 + 头插（StageCommands 对 308 已把位置清零，见用例 2）。
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
