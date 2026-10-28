// ECL 630 / ECL 800 / STD ins_12(kind1) / STD ins_14 修复单测（fix-st04-instr）。
// 原版依据：
//   800 = 0x41f5e9 → 0x417f40 查敌 + 0x4176c0 释放子线程链 + 0x417740 复位 +
//        0x479a90（指令 +0x18 内联名经 0x4797d0 查到例程首指令写 [ctx+4]、
//        time=0）；自重挂回到 0x4772a0 尾部后重读游标——扣新首指令
//        stack_adjust（0x478bbf）并推进到新首指令之后（0x477377），即新例程
//        首指令（各例程的 enter(40)）被跳过。
//   630 = 0x41f3f6 → 0x409100：STD ins_16 标号中断（游标跳标号后首指令、
//        计时三槽 = 目标 time——Timer::set 同构）。
//   ins_12 kind1 振幅 = unicorn 直跑 0x408c8a 金标（0x4aef78=-120/0x4aef70=-50）。
//   ins_14 = 0x408a43：arg1<0 清 [stage+slot*0x5A4+0x224] bit0；arg1>=0 的
//        0x403800+0x462450 模板重挂无生产宿主 → 显式 BLOCKED。
#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
#include "game/StageScript.hpp"

#include <cstring>

using namespace th13;

namespace {

// ---------- 合成 STD（0 对象 + 空实例区 + 相机脚本） ----------

void std_emit(std::vector<u8>& out, i32 time, i16 opcode, std::initializer_list<i32> args) {
    const u16 length = u16(8 + args.size() * 4);
    out.insert(out.end(), reinterpret_cast<const u8*>(&time), reinterpret_cast<const u8*>(&time) + 4);
    out.insert(out.end(), reinterpret_cast<const u8*>(&opcode), reinterpret_cast<const u8*>(&opcode) + 2);
    out.insert(out.end(), reinterpret_cast<const u8*>(&length), reinterpret_cast<const u8*>(&length) + 2);
    for (i32 argument : args)
        out.insert(out.end(), reinterpret_cast<const u8*>(&argument), reinterpret_cast<const u8*>(&argument) + 4);
}

std::vector<u8> build_std(const std::vector<u8>& script) {
    std::vector<u8> out(0x92, 0);
    const u32 instance_offset = 0x90, script_offset = 0x92;
    std::memcpy(out.data() + 4, &instance_offset, 4);
    std::memcpy(out.data() + 8, &script_offset, 4);
    out[0x90] = 0xff;  // 实例区终止符（空实例表）
    out[0x91] = 0xff;
    out.insert(out.end(), script.begin(), script.end());
    for (u32 i = 0; i < 8; ++i) out.push_back(0xff);  // 脚本终止符
    return out;
}

const StageInstruction* find_label(const StageResource& resource, i32 id) {
    for (const auto& command : resource.instructions)
        if (command.opcode == 16 && command.argument<i32>(0) == id) return &command;
    return nullptr;
}

// ---------- ECL 800 ----------

TEST(ecl800_self_restart_remounts_and_skips_first_instruction) {
    StageSim sim;
    std::vector<u8> trampoline, bossb;
    th13test::emit(trampoline, 0, 40, 0, 255, 0, {0});   // enter 0
    const char name[8] = {'B', 'o', 's', 's', 'B', 0, 0, 0};
    th13test::emit(trampoline, 3, 800, 0, 255, 0, {u32(-9926), 6}, name, 8);
    th13test::emit(trampoline, 100000, 1, 0, 255, 0, {});  // delete（不可达）
    th13test::emit(bossb, 0, 40, 0, 255, 0, {0});          // enter（len 20，被跳过）
    th13test::emit(bossb, 40, 1, 0, 255, 0, {});           // delete @ t=40
    auto data = th13test::build_scpt({{"BossB", bossb}, {"Trampoline", trampoline}});
    CHECK_EQ(sim.program.attach(data.data(), u32(data.size())), 0);
    sim.commands.sim = &sim;
    sim.gvars.sim = &sim;
    auto* dummy = sim.spawn_enemy("BossB", 0, 300, 100, nullptr, false);  // serial 0（id 0 不可寻址）
    (void)dummy;
    auto* boss = sim.spawn_enemy("Trampoline", 0, 100, 1000, nullptr, false);
    CHECK(boss != nullptr);
    if (!boss) return;
    sim.shared_integers[0] = boss->script_id;  // gvar -9926
    auto& main = boss->threads->main;
    const i32 bossb_index = sim.program.find_index("BossB");
    for (u32 frame = 0; frame < 8 && main.routine != bossb_index; ++frame)
        CHECK(sim.step_frame());
    CHECK_EQ(main.routine, bossb_index);
    // 自重挂尾序（0x478bbf + 0x477377）：游标停在新例程首指令（enter，len 20）之后。
    CHECK_EQ(main.instruction_offset, 20);
    CHECK_EQ(main.time, 1.0f);  // 重挂 time=0，随后 delete@40 时间门 +1
    CHECK_EQ(main.stack_pointer, 0);
    CHECK_EQ(main.local_base, 0);
    CHECK_EQ(main.thread_id, -1);
    CHECK_EQ(main.thread_state, 0);
    CHECK(!(main.thread_flags & 1u));
    CHECK_EQ(main.difficulty, u8(0x08));  // 难度保持（0x417740 不触 +0x101C）
    CHECK(sim.error().empty());
}

TEST(ecl800_restarts_other_enemy_from_first_instruction) {
    StageSim sim;
    std::vector<u8> bossb, idle, root;
    th13test::emit(bossb, 0, 40, 0, 255, 0, {0});
    th13test::emit(bossb, 100000, 1, 0, 255, 0, {});
    th13test::emit(idle, 0, 40, 0, 255, 0, {0});
    th13test::emit(idle, 100000, 1, 0, 255, 0, {});
    th13test::emit(root, 0, 40, 0, 255, 0, {0});
    th13test::emit(root, 100000, 1, 0, 255, 0, {});
    auto data = th13test::build_scpt({{"BossB", bossb}, {"Idle", idle}, {"main", root}});
    CHECK_EQ(sim.program.attach(data.data(), u32(data.size())), 0);
    sim.commands.sim = &sim;
    sim.gvars.sim = &sim;
    auto* owner = sim.spawn_enemy("main", 0, 100, 100, nullptr, false);
    auto* child = sim.spawn_enemy("Idle", 0, 200, 100, nullptr, false);
    CHECK(owner && child);
    if (!owner || !child) return;
    auto& target = child->threads->main;
    // 污染目标上下文：restart（0x4176c0+0x417740）应整体复位。
    target.stack_pointer = 40;
    target.local_base = 24;
    target.time = 77;
    target.thread_flags |= 1u;
    target.thread_id = 9;
    target.interpolators[2].duration = 9;
    const char name[8] = {'B', 'o', 's', 's', 'B', 0, 0, 0};
    std::vector<u8> code;
    th13test::emit(code, 0, 800, 0, 255, 0, {u32(child->script_id), 6}, name, 8);
    auto& caller = owner->threads->main;
    caller.instruction = reinterpret_cast<const EclInstruction*>(code.data());
    sim.current = owner;
    CHECK_EQ(sim.commands.execute(caller, 800), 0);
    CHECK_EQ(target.routine, sim.program.find_index("BossB"));
    CHECK_EQ(target.instruction_offset, 0);  // 非自重挂：从首指令起（无跳过）
    CHECK_EQ(target.time, 0.0f);
    CHECK_EQ(target.stack_pointer, 0);
    CHECK_EQ(target.local_base, 0);
    CHECK_EQ(target.thread_id, -1);
    CHECK(!(target.thread_flags & 1u));
    CHECK_EQ(target.interpolators[2].duration, 0);
    CHECK_EQ(caller.routine, sim.program.find_index("main"));  // 调用者不动
    CHECK(sim.error().empty());
}

TEST(ecl800_unknown_routine_ends_calling_thread_like_original) {
    StageSim sim;
    std::vector<u8> root;
    th13test::emit(root, 0, 40, 0, 255, 0, {0});
    th13test::emit(root, 100000, 1, 0, 255, 0, {});
    auto data = th13test::build_scpt({{"main", root}});
    CHECK_EQ(sim.program.attach(data.data(), u32(data.size())), 0);
    sim.commands.sim = &sim;
    sim.gvars.sim = &sim;
    auto* dummy = sim.spawn_enemy("main", 0, 300, 100, nullptr, false);  // serial 0（id 0 不可寻址）
    (void)dummy;
    auto* owner = sim.spawn_enemy("main", 0, 100, 100, nullptr, false);
    CHECK(owner != nullptr);
    if (!owner) return;
    const char name[8] = {'A', 'b', 's', 'e', 'n', 't', 0, 0};
    std::vector<u8> code;
    th13test::emit(code, 0, 800, 0, 255, 0, {u32(owner->script_id), 7}, name, 8);
    auto& main = owner->threads->main;
    main.instruction = reinterpret_cast<const EclInstruction*>(code.data());
    sim.current = owner;
    // 0x4797d0 查名失败 → [ctx+4]=0 → 0x4772b1 按线程结束返回。
    CHECK_EQ(sim.commands.execute(main, 800), -1);
    CHECK_EQ(main.routine, -1);
    CHECK(main.code == nullptr);
    CHECK_EQ(main.time, 0.0f);
    CHECK(sim.error().empty());
}

TEST(ecl800_missing_enemy_is_harmless_default_tail) {
    StageSim sim;
    std::vector<u8> root;
    th13test::emit(root, 0, 40, 0, 255, 0, {0});
    th13test::emit(root, 100000, 1, 0, 255, 0, {});
    auto data = th13test::build_scpt({{"main", root}});
    CHECK_EQ(sim.program.attach(data.data(), u32(data.size())), 0);
    sim.commands.sim = &sim;
    auto* owner = sim.spawn_enemy("main", 0, 100, 100, nullptr, false);
    CHECK(owner != nullptr);
    if (!owner) return;
    const char name[8] = {'B', 'o', 's', 's', 'B', 0, 0, 0};
    std::vector<u8> code;
    th13test::emit(code, 0, 800, 0, 255, 0, {0, 6}, name, 8);  // id 0 → 0x417f40 null
    auto& main = owner->threads->main;
    main.instruction = reinterpret_cast<const EclInstruction*>(code.data());
    sim.current = owner;
    CHECK_EQ(sim.commands.execute(main, 800), 0);
    CHECK_EQ(main.routine, sim.program.find_index("main"));  // 未被重挂
    CHECK(sim.error().empty());
}

// ---------- ECL 630 ----------

TEST(ecl630_routes_to_std_camera_interrupt) {
    StageSim sim;
    StageEnemy enemy;
    sim.commands.sim = &sim;
    sim.current = &enemy;
    std::vector<u8> script;
    std_emit(script, 0, 12, {1});                             // kind1 摇晃
    std_emit(script, 10, 16, {1});                            // 中断标号 id=1
    std_emit(script, 200, 7, {i32(float_to_bits(0.5f))});     // fov
    std_emit(script, 200, 0, {});                             // stop
    auto bytes = build_std(script);
    StageResource resource;
    CHECK(resource.open(bytes.data(), u32(bytes.size())));
    StageScript camera(resource);
    for (int i = 0; i < 5; ++i) CHECK(camera.step());
    sim.stage_script_interrupt = [&](i32 id) { return camera.interrupt(id); };
    std::vector<u8> code;
    th13test::emit(code, 0, 630, 0, 255, 0, {1});
    EclContext context;
    context.instruction = reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(context, 630), 0);
    const StageInstruction* label = find_label(resource, 1);
    CHECK(label != nullptr);
    if (!label) return;
    const auto* next = resource.instruction(label->offset + u32(label->length));
    CHECK(next != nullptr);
    CHECK_EQ(camera.state.instruction_offset, label->offset + u32(label->length));
    CHECK_EQ(camera.state.timer.current, next->time);
    CHECK_EQ(camera.state.timer.previous, next->time - 1);
    CHECK(camera.step());
    CHECK_NEAR(camera.state.camera.fov, 0.5f, 1e-6f);
    // 未命中标号（原版 0x409100 扫描无果）：无动作返回。
    const u32 parked = camera.state.instruction_offset;
    CHECK(camera.interrupt(5));
    CHECK_EQ(camera.state.instruction_offset, parked);
    // 无宿主钩子（无头路径）：显式报错，不静默吞中断。
    sim.stage_script_interrupt = nullptr;
    CHECK_EQ(sim.commands.execute(context, 630), -2);
    CHECK(!sim.error().empty());
}

// ---------- STD ins_12 kind1（unicorn 金标） ----------

TEST(std_camera_kind1_matches_original_amplitudes) {
    std::vector<u8> script;
    std_emit(script, 0, 12, {1});
    std_emit(script, 100000, 0, {});
    auto bytes = build_std(script);
    StageResource resource;
    CHECK(resource.open(bytes.data(), u32(bytes.size())));
    StageScript camera(resource);
    struct Sample {
        i32 t;
        float ex, ez, ux, tx, tz;
    };
    // 0x408c8a 原版直跑（std_cameff_probe.py，2026-10-28）。
    const Sample golden[] = {
        {32, -45.922f, -35.355f, -0.00383f, 22.961f, 17.678f},
        {100, -112.985f, -31.720f, -0.00942f, 56.493f, 15.860f},
        {400, 117.694f, 19.134f, 0.00981f, -58.847f, -9.567f},
        {900, 119.855f, 4.901f, 0.00999f, -59.928f, -2.450f},
    };
    u32 index = 0;
    for (i32 phase = 0; phase <= 900 && index < 4; ++phase) {
        CHECK(camera.step());
        if (golden[index].t != phase) continue;
        const auto& s = camera.state.camera;
        CHECK_NEAR(s.eye_offset.x, golden[index].ex, 0.01f);
        CHECK_NEAR(s.eye_offset.z, golden[index].ez, 0.01f);
        CHECK_NEAR(s.up.x, golden[index].ux, 0.0001f);
        CHECK_NEAR(s.target_offset.x, golden[index].tx, 0.01f);
        CHECK_NEAR(s.target_offset.z, golden[index].tz, 0.01f);
        ++index;
    }
    CHECK_EQ(index, 4u);
    CHECK(camera.error.empty());
}

// ---------- STD ins_14 ----------

TEST(std_ins14_hides_slot_and_blocks_remount) {
    std::vector<u8> script;
    std_emit(script, 0, 14, {0, -1});   // 隐藏槽 0
    std_emit(script, 10, 14, {1, 4});   // 槽 1 模板重挂（BLOCKED）
    std_emit(script, 100000, 0, {});
    auto bytes = build_std(script);
    StageResource resource;
    CHECK(resource.open(bytes.data(), u32(bytes.size())));
    StageScript camera(resource);
    CHECK(camera.step());  // t=0：隐藏分支（0x408a79: [slot+0x224] &= ~1）
    CHECK(camera.error.empty());
    CHECK(!camera.state.background[0].visible);
    CHECK(camera.state.background[1].visible);
    for (int i = 0; i < 9; ++i) CHECK(camera.step());
    CHECK(!camera.step());  // t=10：arg1>=0 模板重挂无宿主 → 显式 BLOCKED
    CHECK(!camera.error.empty());
    CHECK(camera.error.find("ins_14") != std::string::npos);
}

TEST(std_ins14_rejects_slot_outside_pool) {
    std::vector<u8> script;
    std_emit(script, 0, 14, {9, -1});
    std_emit(script, 100000, 0, {});
    auto bytes = build_std(script);
    StageResource resource;
    CHECK(resource.open(bytes.data(), u32(bytes.size())));
    StageScript camera(resource);
    CHECK(!camera.step());
    CHECK_EQ(camera.error, std::string("STD ins_14 slot outside background pool"));
}

// ---------- 真实 st04.std 集成 ----------

TEST(st04_real_std_kind1_then_interrupt_label) {
    th13test::TestFileReader reader;
    std::vector<u8> bytes;
    if (!reader.read("artifacts/reference/raw/st04.std", bytes)) return;  // raw 缺席时跳过
    StageResource resource;
    CHECK(resource.open(bytes.data(), u32(bytes.size())));
    StageScript camera(resource);
    for (int i = 0; i < 33; ++i) CHECK(camera.step());  // t=0 ins_12(1)；相位 32
    CHECK_NEAR(camera.state.camera.eye_offset.x, -45.922f, 0.01f);
    CHECK_NEAR(camera.state.camera.eye_offset.z, -35.355f, 0.01f);
    // st04.ecl main/MainSub09/MainBossDebug(2) 的 630(1) → ins_16(1)@t=128。
    CHECK(camera.interrupt(1));
    const StageInstruction* label = find_label(resource, 1);
    CHECK(label != nullptr);
    if (!label) return;
    const auto* next = resource.instruction(label->offset + u32(label->length));
    CHECK(next != nullptr);
    CHECK_EQ(camera.state.instruction_offset, label->offset + u32(label->length));
    CHECK_EQ(camera.state.timer.current, next->time);
    CHECK(camera.step());
    CHECK(camera.error.empty());
}

}  // namespace

int main() { return th13test::run_all("ecl-stage-interrupt"); }
