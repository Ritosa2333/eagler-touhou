// EclInterpreter 单测：以 st01 GirlA01/GirlA01_at 真实指令序列为固定用例。
// 覆盖：时间模型（wait/帧推进）、难度门（!HL/!L）、callAsync 派生、
// 表达式栈机（全局浮点/字面量/divf/addf + stack_adjust 弹栈）、跳转算术、
// 局部变量（seti/postDec）、同步 call/ret、基础栈机指令电池、插值通道。
#include "ecl_test_stubs.hpp"

namespace {

TEST(girl_a01_lunatic_first_frame) {
    // 聚合真实指令流：GirlA01 + 两个被调子程序。
    auto bytes = th13test::build_scpt({{"GirlA01", th13test::girl_a01_stream()},
                                       {"GirlA01_at", th13test::girl_a01_at_stream()},
                                       {"GirlA01_at_l", th13test::girl_a01_at_stream()}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13test::RecordingCommands commands;
    th13test::StubVariables variables;
    variables.floats.push_back({-9989, 2.0f});   // ANGLE_PLAYER
    variables.floats.push_back({-9988, 47.0f});  // TIME
    th13::EclThreads threads(program, &variables);
    threads.main.commands = &commands;
    CHECK(threads.start("GirlA01", 0x08));  // Lunatic = bit3
    CHECK_EQ(threads.step(1.0f), 0);
    // 帧内已执行：stackAlloc(0) 隐藏、500、501、404、405、两次 callAsync、wait(60)。
    // 时间：0 - 60 = -60，随后 +elapsed → -59。
    CHECK_NEAR(threads.main.time, -59.0, 1e-6);
    CHECK_EQ(threads.count(), 3u);  // main + 两异步子线程
    // 子线程派生但本帧不执行（下帧参战）：time 仍为 0，难度继承 Lunatic。
    CHECK_NEAR(threads.at(1)->time, 0.0, 1e-6);
    CHECK_NEAR(threads.at(2)->time, 0.0, 1e-6);
    CHECK_EQ(threads.at(1)->difficulty, 0x08);
    CHECK_EQ(threads.at(2)->difficulty, 0x08);
    CHECK(threads.at(1)->routine != threads.at(2)->routine);  // GirlA01_at / GirlA01_at_l
    // 游标停在 wait 之后的 pushf（子程序内偏移 0xd4 = 0x4b4-0x3e0）。
    CHECK_EQ(threads.main.instruction_offset, 0xd4);
    CHECK_EQ(threads.main.stack_pointer, 4);  // stackAlloc(0) 后 SP=4
    CHECK_EQ(threads.main.local_base, 0);
    // 游戏指令按序记录（500/501/404/405）。
    std::vector<th13::u16> opcodes;
    for (const auto& call : commands.calls) opcodes.push_back(call.opcode);
    CHECK((opcodes == std::vector<th13::u16>{500, 501, 404, 405}));
    // 500 setHurtbox(24.0f, 24.0f)；501 setHitbox(16.0f, 16.0f)；
    // 404 moveVel(1.1780972f, 4.0f)；405 moveVelTime(60, 1, 1.5707964f, 0.0f)。
    CHECK_EQ(commands.calls[0].floats.size(), 2u);
    CHECK_NEAR(commands.calls[0].floats[0], 24.0, 1e-6);
    CHECK_NEAR(commands.calls[0].floats[1], 24.0, 1e-6);
    CHECK_NEAR(commands.calls[1].floats[0], 16.0, 1e-6);
    CHECK_NEAR(commands.calls[2].floats[0], 1.17809724, 1e-6);
    CHECK_NEAR(commands.calls[2].floats[1], 4.0, 1e-6);
    CHECK_EQ(commands.calls[3].ints[0], 60);
    CHECK_EQ(commands.calls[3].ints[1], 1);
    CHECK_NEAR(commands.calls[3].floats[2], 1.57079637, 1e-6);
}

TEST(girl_a01_expression_and_loop) {
    auto bytes = th13test::build_scpt({{"GirlA01", th13test::girl_a01_stream()},
                                       {"GirlA01_at", th13test::girl_a01_at_stream()},
                                       {"GirlA01_at_l", th13test::girl_a01_at_stream()}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13test::RecordingCommands commands;
    th13test::StubVariables variables;
    variables.floats.push_back({-9989, 2.0f});   // ANGLE_PLAYER
    variables.floats.push_back({-9998, 47.0f});  // RANDRAD（表达式第二项 [-9998.0f]）
    th13::EclThreads threads(program, &variables);
    threads.main.commands = &commands;
    CHECK(threads.start("GirlA01", 0x08));
    // 第 0 帧 wait(60) → time=-59；第 k 帧起 time=k-60；第 60 帧执行表达式。
    for (int frame = 0; frame < 61; ++frame) CHECK_EQ(threads.step(1.0f), 0);
    // ins_429 的参数 2 是表达式 [-9989.0f] + ([-9998.0f] / 20.0f) = 2.0 + 47.0/20 = 4.35。
    const th13test::RecordedCall* move = nullptr;
    for (const auto& call : commands.calls)
        if (call.opcode == 429) move = &call;
    CHECK(move != nullptr);
    CHECK_EQ(move->ints[0], 60);
    CHECK_EQ(move->ints[1], 0);
    CHECK_NEAR(move->floats[2], 4.35, 1e-5);
    CHECK_NEAR(move->floats[3], 1.0, 1e-6);
    // 表达式栈被 stack_adjust=8 清空：SP 回到 4。
    CHECK_EQ(threads.main.stack_pointer, 4);
    // jmp(Δ=0x2C) 落在 GirlA01_396（pushi 1，偏移 0x17C）；jmpNeq 弹 1 跳回 wait(1000)
    // （偏移 0x168）；帧 60 结束时 time = -1000 + 1 = -999，游标回到 pushi 1（0x17C）。
    CHECK_NEAR(threads.main.time, -999.0, 1e-4);
    CHECK_EQ(threads.main.instruction_offset, 0x17c);
}

TEST(girl_a01_rank_filter) {
    auto bytes = th13test::build_scpt({{"GirlA01", th13test::girl_a01_stream()},
                                       {"GirlA01_at", th13test::girl_a01_at_stream()},
                                       {"GirlA01_at_l", th13test::girl_a01_at_stream()}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13test::RecordingCommands commands;
    // Easy(bit0)/Normal(bit1)：cc=0xcc 与难度位无交集 → 两个 callAsync 都被跳过。
    {
        th13::EclThreads threads(program);
        threads.main.commands = &commands;
        CHECK(threads.start("GirlA01", 0x01));
        CHECK_EQ(threads.step(1.0f), 0);
        CHECK_EQ(threads.count(), 1u);
    }
    {
        th13::EclThreads threads(program);
        threads.main.commands = &commands;
        CHECK(threads.start("GirlA01", 0x02));
        CHECK_EQ(threads.step(1.0f), 0);
        CHECK_EQ(threads.count(), 1u);
    }
    // Hard(bit2)：!HL(0xcc) 命中、!L(0xc8) 不命中 → 仅 GirlA01_at 派生。
    {
        th13::EclThreads threads(program);
        threads.main.commands = &commands;
        CHECK(threads.start("GirlA01", 0x04));
        CHECK_EQ(threads.step(1.0f), 0);
        CHECK_EQ(threads.count(), 2u);
        CHECK_EQ(program.subroutine(threads.at(1)->routine).name, std::string("GirlA01_at"));
    }
}

TEST(girl_a01_at_loop_and_retire) {
    // GirlA01_at 单线程直跑：$A=3 → 3 轮 etOn/wait(30)，postDec 减到 0 后 ret 结束。
    auto bytes = th13test::build_scpt({{"GirlA01_at", th13test::girl_a01_at_stream()}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13test::RecordingCommands commands;
    th13::EclThreads threads(program);
    threads.main.commands = &commands;
    CHECK(threads.start("GirlA01_at", 0x08));
    int frames = -1;
    for (int i = 0; i < 400; ++i) {
        const int result = threads.step(1.0f);
        CHECK(result == 0 || result == -1);
        frames = i;
        if (result) break;
    }
    // 帧预算：帧 0 = 548(diffWait 30) + wait(30) → -60；此后每轮 60 帧
    // （首轮 548+wait）+ 两轮 wait(30)，第 120 帧 postDec 出 0 → ret 结束。
    CHECK_EQ(frames, 120);
    CHECK(threads.error.empty());
    CHECK(threads.main.routine < 0);
    // 再步进仍为 -1（线程已结束，非错误）。
    CHECK_EQ(threads.step(1.0f), -1);
    // 游戏指令序列：548,600,607,602,625,604,624,609 各一次；601×3（三轮发弹）。
    std::vector<th13::u16> opcodes;
    for (const auto& call : commands.calls) opcodes.push_back(call.opcode);
    const std::vector<th13::u16> expected = {548, 600, 607, 602, 625, 604, 624, 609, 601, 601, 601};
    CHECK(opcodes == expected);
}

TEST(sync_call_and_return) {
    // 同步调用：Main 压帧调用 Callee，被调者 ret 后恢复 {SP,time,Q}。
    std::vector<th13::u8> main_script, callee_script;
    const char callee_name[] = "Callee";           // 含 NUL 7 字节，对齐 8
    const th13::u32 name_size = 8;
    th13test::emit(main_script, 0, 42, 0, 0xff, 0, {7});  // pushi 7（调用者表达式栈遗留）
    {
        // 参数 0 = 名字长度（偏移 16）；尾巴从偏移 20 起直接是名字。
        std::vector<th13::u8> tail;
        tail.insert(tail.end(), callee_name, callee_name + sizeof callee_name);
        tail.push_back(0);  // 名字区补齐到 8
        th13test::emit(main_script, 0, 11, 0, 0xff, 0, {name_size}, tail.data(), th13::u32(tail.size()));
    }
    th13test::emit(main_script, 0, 23, 0, 0xff, 0, {5});  // wait(5) —— 被调者返回后执行
    th13test::emit(main_script, 0, 10, 0, 0xff, 0, {});   // ret
    th13test::emit(callee_script, 0, 40, 0, 0xff, 0, {4});  // stackAlloc(4)
    th13test::emit(callee_script, 0, 10, 0, 0xff, 0, {});   // ret
    auto bytes = th13test::build_scpt({{"Callee", callee_script}, {"Main", main_script}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13::EclThreads threads(program);
    CHECK(threads.start("Main", 0xff));
    CHECK_EQ(threads.step(1.0f), 0);
    // 帧内：pushi 7（SP 0→8）；call 切帧（被调者 stackAlloc(4) 后 SP=32，ret 弹帧回 SP=8）；
    // wait(5) → time = -5 + 1 = -4。
    CHECK_NEAR(threads.main.time, -4.0, 1e-6);
    CHECK_EQ(threads.main.stack_pointer, 8);  // 遗留的 pushi 7 仍在表达式栈
    // 后续 4 帧追平时钟，第 5 帧执行尾 ret → 线程结束。
    for (int i = 0; i < 4; ++i) CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), -1);
}

TEST(base_stack_machine_battery) {
    // 合成指令覆盖基础栈机：算术/比较/位运算/取负/sin/cos/开方。
    std::vector<th13::u8> script;
    auto emit = [&](th13::u16 op, th13::u16 mask = 0, const std::vector<th13::u32>& args = {}, th13::u32 sadj = 0) {
        th13test::emit(script, 0, op, mask, 0xff, sadj, args);
    };
    emit(40, 0, {28});                  // var：28 字节局部（7 槽）
    emit(44, 0, {th13test::bits_of(9.0f)});
    emit(44, 0, {th13test::bits_of(4.0f)});
    emit(57);                           // divf → 2.25
    emit(45, 1, {0});                   // setf 局部[0]
    emit(42, 0, {7});
    emit(42, 0, {3});
    emit(56);                           // divi → 2
    emit(42, 0, {5});
    emit(58);                           // modi 2%5 → 2
    emit(42, 0, {2});
    emit(50);                           // addi → 2+2 = 4
    emit(43, 1, {4});                   // seti 局部[4]
    emit(42, 0, {7});
    emit(42, 0, {3});
    emit(59);                           // eqi → 0
    emit(71);                           // noti → 1
    emit(43, 1, {24});                  // seti 局部[24]（保留栈平衡）
    emit(42, 0, {12});
    emit(42, 0, {10});
    emit(75);                           // xor → 6
    emit(42, 0, {3});
    emit(76);                           // or → 7
    emit(42, 0, {0xfffffffcu});         // pushi -4
    emit(83);                           // negi → 4
    emit(77);                           // 位与 7&4 → 4
    emit(43, 1, {8});                   // seti 局部[8]
    emit(44, 0, {th13test::bits_of(1.0f)});
    emit(79);                           // sin(1.0) ≈ 0.8414710
    emit(45, 1, {th13test::bits_of(12.0f)});
    emit(44, 0, {th13test::bits_of(0.5235988f)});
    emit(80);                           // cos(π/6) ≈ 0.8660254
    emit(45, 1, {th13test::bits_of(16.0f)});
    emit(44, 0, {th13test::bits_of(9.0f)});
    emit(88);                           // sqrt(9) = 3
    emit(45, 1, {th13test::bits_of(20.0f)});
    emit(10);                           // ret
    auto bytes = th13test::build_scpt({{"Battery", script}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13::EclThreads threads(program);
    CHECK(threads.start("Battery", 0xff));
    CHECK_EQ(threads.step(1.0f), -1);  // 全部帧内执行完并 ret
    const auto* base = threads.main.stack.data() + threads.main.local_base;
    auto local_f = [&](th13::i32 offset) {
        float value = 0;
        std::memcpy(&value, base + offset, 4);
        return value;
    };
    auto local_i = [&](th13::i32 offset) {
        th13::i32 value = 0;
        std::memcpy(&value, base + offset, 4);
        return value;
    };
    CHECK_NEAR(local_f(0), 2.25, 1e-6);
    CHECK_EQ(local_i(4), 4);
    CHECK_EQ(local_i(8), 4);
    CHECK_NEAR(local_f(12), 0.84147101, 1e-6);
    CHECK_NEAR(local_f(16), 0.86602535, 1e-6);
    CHECK_NEAR(local_f(20), 3.0, 1e-6);
    CHECK_EQ(threads.main.stack_pointer, 0);  // ret 弹帧后栈净空
}

TEST(interpolate_channel_semantics) {
    // 指令 91：slot0（字面量）、10 帧、线性模式 0、0→10、目标 = 局部[0]（float）。
    std::vector<th13::u8> script;
    th13test::emit(script, 0, 40, 0, 0xff, 0, {8});  // var
    th13test::emit(script, 0, 91, 2, 0xff, 0,
                   {0, 0, 10, 0, 0, th13test::bits_of(10.0f)});  // mask 仅参数 1（目标）
    th13test::emit(script, 0, 23, 0, 0xff, 0, {30});  // wait(30) 让通道推进
    th13test::emit(script, 0, 12, 0, 0xff, 0, {0xffffffecu, 0});  // jmp 回 wait（保活，Δ=-20）
    auto bytes = th13test::build_scpt({{"Interp", script}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13::EclThreads threads(program);
    CHECK(threads.start("Interp", 0xff));
    auto local = [&]() {
        float value = 0;
        std::memcpy(&value, threads.main.stack.data() + threads.main.local_base, 4);
        return value;
    };
    CHECK_EQ(threads.step(1.0f), 0);  // 挂接帧：指令写起始值 0，帧尾通道步进一格 → 1.0
    CHECK_NEAR(local(), 1.0, 1e-6);
    for (int i = 0; i < 4; ++i) CHECK_EQ(threads.step(1.0f), 0);
    CHECK_NEAR(local(), 5.0, 1e-6);
    for (int i = 0; i < 5; ++i) CHECK_EQ(threads.step(1.0f), 0);
    CHECK_NEAR(local(), 10.0, 1e-6);  // 第 10 帧到终值并停（duration 归零）
    for (int i = 0; i < 3; ++i) CHECK_EQ(threads.step(1.0f), 0);
    CHECK_NEAR(local(), 10.0, 1e-6);
    CHECK_EQ(threads.main.interpolators[0].duration, 0);
}

TEST(wait_uses_top_reference_without_leak) {
    // ins_23([-10000] % 320)：pushi 全局 RAND（stub 整型 7）→ pushi 320 → modi →
    // wait(-1 栈引用)，stack_adjust=8 事后弹栈。
    std::vector<th13::u8> script;
    th13test::emit(script, 0, 42, 1, 0xff, 0, {0xffffd8f0u});  // pushi RAND(-10000)
    th13test::emit(script, 0, 42, 0, 0xff, 0, {320});
    th13test::emit(script, 0, 58, 0, 0xff, 0, {});             // modi
    th13test::emit(script, 0, 23, 1, 0xff, 8, {0xffffffff});   // wait 栈顶引用
    th13test::emit(script, 0, 10, 0, 0xff, 0, {});
    auto bytes = th13test::build_scpt({{"WaitExpr", script}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13test::StubVariables variables;
    variables.integers.push_back({-10000, 7});
    th13::EclThreads threads(program, &variables);
    CHECK(threads.start("WaitExpr", 0xff));
    CHECK_EQ(threads.step(1.0f), 0);
    // time = 0 - (7 % 320) = -7，帧尾 +1 → -6。
    CHECK_NEAR(threads.main.time, -6.0, 1e-6);
    CHECK_EQ(threads.main.stack_pointer, 0);  // sadj=8 已弹净
}

}  // namespace

int main() { return th13test::run_all("ecl-interpreter"); }
