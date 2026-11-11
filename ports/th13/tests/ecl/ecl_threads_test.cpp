// EclThreads/EclFile 单测：线程派生顺序与回收、pause/resume/kill、
// 快照落盘/恢复往返（0x11C4 块）、整 st01.ecl 冒烟（59 子程序步进无 VM 错误）。
#include "ecl_test_stubs.hpp"

namespace {

TEST(threads_spawn_order_and_new_thread_next_frame) {
    // 头插链表：后派生者在前；当帧派生的新线程下帧参战。
    auto bytes = th13test::build_scpt({{"GirlA01", th13test::girl_a01_stream()},
                                       {"GirlA01_at", th13test::girl_a01_at_stream()},
                                       {"GirlA01_at_l", th13test::girl_a01_at_stream()}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13test::RecordingCommands commands;
    th13::EclThreads threads(program);
    threads.main.commands = &commands;
    CHECK(threads.start("GirlA01", 0x08));
    CHECK_EQ(threads.step(1.0f), 0);
    // at(1) 是链表头 = 最后派生的 GirlA01_at_l。
    CHECK_EQ(program.subroutine(threads.at(1)->routine).name, std::string("GirlA01_at_l"));
    CHECK_EQ(program.subroutine(threads.at(2)->routine).name, std::string("GirlA01_at"));
    // 帧 0 时两个子线程 time 均为 0（未执行）；帧 1 起参战：et 指令族执行至 wait(30)。
    CHECK_NEAR(threads.at(1)->time, 0.0, 1e-6);
    const auto calls_before = commands.calls.size();
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK(commands.calls.size() > calls_before);
    CHECK_NEAR(threads.at(1)->time, -29.0, 1e-6);
    CHECK_NEAR(threads.at(2)->time, -29.0, 1e-6);
}

TEST(threads_pause_resume_kill) {
    // callAsyncId(16) 派生带 id 子线程；pauseAsync(18) 冻结；resumeAsync(19) 恢复；
    // killAsync(17) 置游标 -1 → 当帧被遍历器回收。
    // 注：th13 全部 raw ECL 无 op16 实例（扫描验证），此处按 th15 协议验证本实现：
    // [16]=名字长度，[20..]=名字（参数 1 槽与名字首 dword 重叠），名字后内联 id。
    std::vector<th13::u8> script;
    {
        const char name[] = "Child";  // 含 NUL 6 字节，对齐 8
        const th13::u32 size = 8;
        th13::u32 first = 0;
        std::memcpy(&first, name, 4);  // 参数 1 = 名字首 dword（与名字区重叠）
        std::vector<th13::u8> tail;
        tail.insert(tail.end(), name + 4, name + sizeof name);  // 名字余部 "d\0"
        tail.push_back(0);
        tail.push_back(0);  // 名字补齐到 8（此后跟内联 id）
        const th13::u32 id = 5;
        tail.insert(tail.end(), reinterpret_cast<const th13::u8*>(&id), reinterpret_cast<const th13::u8*>(&id) + 4);
        th13test::emit(script, 0, 16, 0, 0xff, 0, {size, first}, tail.data(), th13::u32(tail.size()));
    }
    th13test::emit(script, 0, 40, 0, 0xff, 0, {0});  // var（thecl 恒在函数头发 stackAlloc）
    th13test::emit(script, 0, 23, 0, 0xff, 0, {3});  // wait(3)
    th13test::emit(script, 0, 18, 0, 0xff, 0, {5});  // pauseAsync(5)
    th13test::emit(script, 0, 23, 0, 0xff, 0, {3});
    th13test::emit(script, 0, 19, 0, 0xff, 0, {5});  // resumeAsync(5)
    th13test::emit(script, 0, 23, 0, 0xff, 0, {3});
    th13test::emit(script, 0, 17, 0, 0xff, 0, {5});  // killAsync(5)
    th13test::emit(script, 0, 23, 0, 0xff, 0, {3});
    th13test::emit(script, 0, 10, 0, 0xff, 0, {});
    std::vector<th13::u8> child;
    th13test::emit(child, 0, 23, 0, 0xff, 0, {2});       // wait(2)
    th13test::emit(child, 0, 12, 0, 0xff, 0, {0xffffffecu, 0});  // jmp 回自身（Δ=-20）
    auto bytes = th13test::build_scpt({{"Child", child}, {"Main", script}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13::EclThreads threads(program);
    CHECK(threads.start("Main", 0xff));
    CHECK_EQ(threads.step(1.0f), 0);  // 帧 0：派生 Child(id=5) + wait(3)
    CHECK_EQ(threads.count(), 2u);
    CHECK(threads.find(5) != &threads.main);
    CHECK_EQ(threads.find(5)->thread_id, 5);
    // 帧 1-2：子线程参战（wait(2) 自循环）。帧 3：主线程执行 pauseAsync。
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), 0);
    const float frozen = threads.find(5)->time;
    // 帧 4-5：子线程被暂停（时间冻结）。
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_NEAR(threads.find(5)->time, frozen, 1e-6);
    // 帧 6：主线程 resumeAsync + wait(3)；帧 7-8 子线程恢复推进。
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), 0);
    // 帧 9：主线程 killAsync(5) → 子线程当帧回收。
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.count(), 1u);
    // 帧 10-11 追平时钟，帧 12 执行尾 ret 结束。
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), -1);
    CHECK(threads.error.empty());
}

TEST(threads_spawn_with_argument_descriptor) {
    // 真实编码（st01.ecl sub#52 → TraceEnemyPos）：callAsync cnt=2、mask=2、
    // 名字后 8 字节描述符 {char 源类型, char 目标类型, u32 原始值}，实参写入
    // 被调者栈帧 = 其首个局部（dump 的 void TraceEnemyPos(var A)，$A 即 FP+0）。
    std::vector<th13::u8> script;
    th13test::emit(script, 0, 40, 0, 0xff, 0, {0});  // var
    {
        const char name[] = "Worker";  // 含 NUL 7 字节，对齐 8
        const th13::u32 size = 8;
        th13::u32 first = 0;
        std::memcpy(&first, name, 4);  // 参数 1 槽 = 名字首 dword（param_count=2）
        std::vector<th13::u8> tail;
        tail.insert(tail.end(), name + 4, name + sizeof name);  // 名字余部 "er\0"
        tail.push_back(0);  // 名字补齐到 8
        const th13::u8 descriptor[] = {'i', 'i', 0, 0, 0x4b, 0xd9, 0xff, 0xff};  // {i→i, -9909}
        tail.insert(tail.end(), descriptor, descriptor + sizeof descriptor);
        th13test::emit(script, 0, 15, 2, 0xff, 0, {size, first}, tail.data(), th13::u32(tail.size()));
    }
    th13test::emit(script, 0, 23, 0, 0xff, 0, {3});
    th13test::emit(script, 0, 10, 0, 0xff, 0, {});
    std::vector<th13::u8> worker;
    th13test::emit(worker, 0, 40, 0, 0xff, 0, {4});          // var A（= 实参槽 FP+0）
    th13test::emit(worker, 0, 638, 1, 0xff, 0, {0});         // scoreAdd($A) —— 记录实参值
    th13test::emit(worker, 0, 23, 0, 0xff, 0, {2});
    th13test::emit(worker, 0, 10, 0, 0xff, 0, {});
    auto bytes = th13test::build_scpt({{"Main", script}, {"Worker", worker}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13test::RecordingCommands commands;
    th13test::StubVariables variables;
    variables.integers.push_back({-9909, 77});  // -9909 = SPELL_ID（dump 里的 [-9909]）
    th13::EclThreads threads(program, &variables);
    threads.main.commands = &commands;
    CHECK(threads.start("Main", 0xff));
    CHECK_EQ(threads.step(1.0f), 0);  // 帧 0：派生 Worker（实参 -9909 → 全局解析 77）
    CHECK_EQ(threads.count(), 2u);
    CHECK_EQ(threads.step(1.0f), 0);  // 帧 1：Worker 参战，scoreAdd 读 $A
    CHECK_EQ(commands.calls.size(), 1u);
    CHECK_EQ(commands.calls[0].opcode, 638);
    CHECK_EQ(commands.calls[0].ints[0], 77);
    // 子线程的 $A 就是实参槽：直接读栈验证。
    auto* worker_context = threads.at(1);
    th13::i32 slot = 0;
    std::memcpy(&slot, worker_context->stack.data() + worker_context->local_base, 4);
    CHECK_EQ(slot, 77);
}

TEST(threads_snapshot_restore_roundtrip) {
    auto bytes = th13test::build_scpt({{"GirlA01", th13test::girl_a01_stream()},
                                       {"GirlA01_at", th13test::girl_a01_at_stream()},
                                       {"GirlA01_at_l", th13test::girl_a01_at_stream()}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13test::RecordingCommands commands_a, commands_b;
    th13test::StubVariables variables;
    variables.floats.push_back({-9989, 2.0f});
    variables.floats.push_back({-9988, 47.0f});
    th13::EclThreads threads(program, &variables);
    threads.main.commands = &commands_a;
    CHECK(threads.start("GirlA01", 0x08));
    for (int i = 0; i < 10; ++i) CHECK_EQ(threads.step(1.0f), 0);
    // 快照 → 新线程组恢复（先挂 commands 再 restore，恢复会传播到子线程）→
    // 继续跑到表达式帧，与连续运行逐步一致。
    const auto snapshot = threads.snapshot();
    th13::EclThreads restored(program, &variables);
    restored.main.commands = &commands_b;
    CHECK(restored.restore(snapshot));
    for (int i = 10; i < 61; ++i) {
        CHECK_EQ(threads.step(1.0f), 0);
        CHECK_EQ(restored.step(1.0f), 0);
        CHECK_NEAR(restored.main.time, threads.main.time, 1e-5);
        CHECK_EQ(restored.main.instruction_offset, threads.main.instruction_offset);
        CHECK_EQ(restored.count(), threads.count());
    }
    // 两边的 ins_429 表达式参数一致。
    const th13test::RecordedCall *move_a = nullptr, *move_b = nullptr;
    for (const auto& call : commands_a.calls)
        if (call.opcode == 429) move_a = &call;
    for (const auto& call : commands_b.calls)
        if (call.opcode == 429) move_b = &call;
    CHECK(move_a != nullptr);
    CHECK(move_b != nullptr);
    CHECK_NEAR(move_b->floats[2], move_a->floats[2], 1e-6);
}

TEST(eclfile_block_roundtrip) {
    auto bytes = th13test::build_scpt({{"GirlA01", th13test::girl_a01_stream()},
                                       {"GirlA01_at", th13test::girl_a01_at_stream()},
                                       {"GirlA01_at_l", th13test::girl_a01_at_stream()}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13test::RecordingCommands commands_a, commands_b;
    th13test::StubVariables variables;
    variables.floats.push_back({-9989, 2.0f});
    variables.floats.push_back({-9988, 47.0f});
    th13::EclThreads threads(program, &variables);
    threads.main.commands = &commands_a;
    CHECK(threads.start("GirlA01", 0x08));
    for (int i = 0; i < 5; ++i) CHECK_EQ(threads.step(1.0f), 0);
    // 线程组容器往返：0x11D4 步长、子链继续标志、恢复后行为一致。
    th13::EclFile file;
    const auto snapshot = threads.snapshot();
    // owner 区 [0x100, 0x12DC)，子线程区须在其后（write_threads 有重叠防护）。
    std::vector<th13::u8> image(0x8000, 0);
    CHECK(file.write_threads(snapshot, image, 0x100, 0x2000));
    th13::EclThreadsSnapshot loaded;
    th13::u32 consumed = 0;
    CHECK(file.read_threads(image.data(), th13::u32(image.size()), loaded, program, 0x100, 0x2000, consumed));
    CHECK_EQ(consumed, snapshot.children.size() * 0x11d4u);
    CHECK_EQ(loaded.children.size(), snapshot.children.size());
    CHECK_NEAR(loaded.main.time, snapshot.main.time, 1e-6);
    CHECK_EQ(loaded.main.instruction_offset, snapshot.main.instruction_offset);
    CHECK_EQ(loaded.main.stack_pointer, snapshot.main.stack_pointer);
    CHECK_EQ(loaded.main.difficulty, snapshot.main.difficulty);
    for (th13::u32 i = 0; i < loaded.children.size(); ++i) {
        CHECK_EQ(loaded.children[i].instruction_offset, snapshot.children[i].instruction_offset);
        CHECK_EQ(loaded.children[i].thread_id, snapshot.children[i].thread_id);
    }
    // 恢复进线程组继续跑，与原始路径逐步一致。
    th13::EclThreads restored(program, &variables);
    restored.main.commands = &commands_b;
    CHECK(restored.restore(loaded));
    for (int i = 5; i < 61; ++i) {
        CHECK_EQ(threads.step(1.0f), 0);
        CHECK_EQ(restored.step(1.0f), 0);
        CHECK_EQ(restored.main.instruction_offset, threads.main.instruction_offset);
        CHECK_EQ(restored.count(), threads.count());
    }
}

TEST(full_st01_smoke) {
    // 整个 st01.ecl（聚合 default.ecl + st01bs.ecl，与游戏加载序一致）：
    // 59 个子程序逐个以 Lunatic 启动跑 120 帧，不产生 VM 错误（-2）。
    th13test::TestFileReader reader;
    std::vector<th13::u8> st01, default_ecl, st01bs;
    CHECK(reader.read("artifacts/reference/raw/st01.ecl", st01));
    CHECK(reader.read("artifacts/reference/raw/default.ecl", default_ecl));
    CHECK(reader.read("artifacts/reference/raw/st01bs.ecl", st01bs));
    th13::EclProgram program;
    CHECK_EQ(program.attach(st01.data(), th13::u32(st01.size())), 0);
    CHECK_EQ(program.attach(default_ecl.data(), th13::u32(default_ecl.size())), 1);
    CHECK_EQ(program.attach(st01bs.data(), th13::u32(st01bs.size())), 2);
    th13test::RecordingCommands commands;
    th13test::StubVariables variables;
    variables.integers.push_back({-10000, 7});    // RAND
    variables.floats.push_back({-9989, 2.0f});    // ANGLE_PLAYER
    variables.floats.push_back({-9988, 47.0f});   // TIME
    variables.floats.push_back({-9999, 0.25f});   // RANDF
    variables.floats.push_back({-9998, 1.5f});    // RANDRAD
    variables.floats.push_back({-9995, 100.0f});  // FINAL_X
    variables.floats.push_back({-9994, 200.0f});  // FINAL_Y
    variables.floats.push_back({-9987, 5.0f});    // RANDF2
    variables.integers.push_back({-9986, 0});     // TIMEOUT
    variables.integers.push_back({-9985, 0});     // I0
    variables.integers.push_back({-9984, 0});     // I1
    variables.floats.push_back({-9981, 0.0f});    // F0
    variables.integers.push_back({-9946, 3});     // ENM_CNT_REAL
    variables.integers.push_back({-9908, 3});     // ENM_CNT
    variables.integers.push_back({-9960, 3});     // RANK
    variables.integers.push_back({-9959, 3});     // DIFF
    variables.floats.push_back({-9954, 1000.0f}); // LIFE
    variables.floats.push_back({-9963, 0.0f});    // BOSS_X
    variables.floats.push_back({-9962, 0.0f});    // BOSS_Y
    variables.floats.push_back({-9991, 100.0f});  // PLAYER_X
    variables.floats.push_back({-9990, 200.0f});  // PLAYER_Y
    th13::GameRandom random(1);
    int scripts_run = 0;
    for (const auto& sub : program.files[0]->subroutines) {
        th13::EclThreads threads(program, &variables, &random);
        threads.main.commands = &commands;
        commands.calls.clear();
        CHECK(threads.start(sub.name.c_str(), 0x08));
        for (int frame = 0; frame < 120; ++frame) {
            const int result = threads.step(1.0f);
            if (result == -2) {
                std::printf("  子程序 %s 出错: %s\n", sub.name.c_str(), threads.error.c_str());
                CHECK(false);
                break;
            }
            if (result) break;
        }
        ++scripts_run;
    }
    CHECK_EQ(scripts_run, 59);
}

}  // namespace

int main() { return th13test::run_all("ecl-threads"); }
