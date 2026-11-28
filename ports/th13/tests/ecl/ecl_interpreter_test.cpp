#include "ecl_test_stubs.hpp"

namespace {

TEST(girl_a01_lunatic_first_frame) {
    auto bytes = th13test::build_scpt({{"GirlA01", th13test::girl_a01_stream()},
                                       {"GirlA01_at", th13test::girl_a01_at_stream()},
                                       {"GirlA01_at_l", th13test::girl_a01_at_stream()}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13test::RecordingCommands commands;
    th13test::StubVariables variables;
    variables.floats.push_back({-9989, 2.0f});
    variables.floats.push_back({-9988, 47.0f});
    th13::EclThreads threads(program, &variables);
    threads.main.commands = &commands;
    CHECK(threads.start("GirlA01", 0x08));
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_NEAR(threads.main.time, -59.0, 1e-6);
    CHECK_EQ(threads.count(), 3u);
    CHECK_NEAR(threads.at(1)->time, 0.0, 1e-6);
    CHECK_NEAR(threads.at(2)->time, 0.0, 1e-6);
    CHECK_EQ(threads.at(1)->difficulty, 0x08);
    CHECK_EQ(threads.at(2)->difficulty, 0x08);
    CHECK(threads.at(1)->routine != threads.at(2)->routine);
    CHECK_EQ(threads.main.instruction_offset, 0xd4);
    CHECK_EQ(threads.main.stack_pointer, 4);
    CHECK_EQ(threads.main.local_base, 0);
    std::vector<th13::u16> opcodes;
    for (const auto& call : commands.calls)
        opcodes.push_back(call.opcode);
    CHECK((opcodes == std::vector<th13::u16>{500, 501, 404, 405}));
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
    variables.floats.push_back({-9989, 2.0f});
    variables.floats.push_back({-9998, 47.0f});
    th13::EclThreads threads(program, &variables);
    threads.main.commands = &commands;
    CHECK(threads.start("GirlA01", 0x08));
    for (int frame = 0; frame < 61; ++frame)
        CHECK_EQ(threads.step(1.0f), 0);
    const th13test::RecordedCall* move = nullptr;
    for (const auto& call : commands.calls)
        if (call.opcode == 429) move = &call;
    CHECK(move != nullptr);
    CHECK_EQ(move->ints[0], 60);
    CHECK_EQ(move->ints[1], 0);
    CHECK_NEAR(move->floats[2], 4.35, 1e-5);
    CHECK_NEAR(move->floats[3], 1.0, 1e-6);
    CHECK_EQ(threads.main.stack_pointer, 4);
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
    CHECK_EQ(frames, 120);
    CHECK(threads.error.empty());
    CHECK(threads.main.routine < 0);
    CHECK_EQ(threads.step(1.0f), -1);
    std::vector<th13::u16> opcodes;
    for (const auto& call : commands.calls)
        opcodes.push_back(call.opcode);
    const std::vector<th13::u16> expected = {548, 600, 607, 602, 625, 604, 624, 609, 601, 601, 601};
    CHECK(opcodes == expected);
}

TEST(sync_call_and_return) {
    std::vector<th13::u8> main_script, callee_script;
    const char callee_name[] = "Callee";
    const th13::u32 name_size = 8;
    th13test::emit(main_script, 0, 42, 0, 0xff, 0, {7});
    {
        std::vector<th13::u8> tail;
        tail.insert(tail.end(), callee_name, callee_name + sizeof callee_name);
        tail.push_back(0);
        th13test::emit(main_script, 0, 11, 0, 0xff, 0, {name_size}, tail.data(), th13::u32(tail.size()));
    }
    th13test::emit(main_script, 0, 23, 0, 0xff, 0, {5});
    th13test::emit(main_script, 0, 10, 0, 0xff, 0, {});
    th13test::emit(callee_script, 0, 40, 0, 0xff, 0, {4});
    th13test::emit(callee_script, 0, 10, 0, 0xff, 0, {});
    auto bytes = th13test::build_scpt({{"Callee", callee_script}, {"Main", main_script}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13::EclThreads threads(program);
    CHECK(threads.start("Main", 0xff));
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_NEAR(threads.main.time, -4.0, 1e-6);
    CHECK_EQ(threads.main.stack_pointer, 8);
    for (int i = 0; i < 4; ++i)
        CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), -1);
}

TEST(base_stack_machine_battery) {
    std::vector<th13::u8> script;
    auto emit = [&](th13::u16 op, th13::u16 mask = 0, const std::vector<th13::u32>& args = {}, th13::u32 sadj = 0) {
        th13test::emit(script, 0, op, mask, 0xff, sadj, args);
    };
    emit(40, 0, {28});
    emit(44, 0, {th13test::bits_of(9.0f)});
    emit(44, 0, {th13test::bits_of(4.0f)});
    emit(57);
    emit(45, 1, {0});
    emit(42, 0, {7});
    emit(42, 0, {3});
    emit(56);
    emit(42, 0, {5});
    emit(58);
    emit(42, 0, {2});
    emit(50);
    emit(43, 1, {4});
    emit(42, 0, {7});
    emit(42, 0, {3});
    emit(59);
    emit(71);
    emit(43, 1, {24});
    emit(42, 0, {12});
    emit(42, 0, {10});
    emit(75);
    emit(42, 0, {3});
    emit(76);
    emit(42, 0, {0xfffffffcu});
    emit(83);
    emit(77);
    emit(43, 1, {8});
    emit(44, 0, {th13test::bits_of(1.0f)});
    emit(79);
    emit(45, 1, {th13test::bits_of(12.0f)});
    emit(44, 0, {th13test::bits_of(0.5235988f)});
    emit(80);
    emit(45, 1, {th13test::bits_of(16.0f)});
    emit(44, 0, {th13test::bits_of(9.0f)});
    emit(88);
    emit(45, 1, {th13test::bits_of(20.0f)});
    emit(10);
    auto bytes = th13test::build_scpt({{"Battery", script}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13::EclThreads threads(program);
    CHECK(threads.start("Battery", 0xff));
    CHECK_EQ(threads.step(1.0f), -1);
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
    CHECK_EQ(threads.main.stack_pointer, 0);
}

TEST(interpolate_channel_semantics) {
    std::vector<th13::u8> script;
    th13test::emit(script, 0, 40, 0, 0xff, 0, {8});
    th13test::emit(script, 0, 91, 2, 0xff, 0, {0, 0, 10, 0, 0, th13test::bits_of(10.0f)});
    th13test::emit(script, 0, 23, 0, 0xff, 0, {30});
    th13test::emit(script, 0, 12, 0, 0xff, 0, {0xffffffecu, 0});
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
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_NEAR(local(), 1.0, 1e-6);
    for (int i = 0; i < 4; ++i)
        CHECK_EQ(threads.step(1.0f), 0);
    CHECK_NEAR(local(), 5.0, 1e-6);
    for (int i = 0; i < 5; ++i)
        CHECK_EQ(threads.step(1.0f), 0);
    CHECK_NEAR(local(), 10.0, 1e-6);
    for (int i = 0; i < 3; ++i)
        CHECK_EQ(threads.step(1.0f), 0);
    CHECK_NEAR(local(), 10.0, 1e-6);
    CHECK_EQ(threads.main.interpolators[0].duration, 0);
}

TEST(wait_uses_top_reference_without_leak) {
    std::vector<th13::u8> script;
    th13test::emit(script, 0, 42, 1, 0xff, 0, {0xffffd8f0u});
    th13test::emit(script, 0, 42, 0, 0xff, 0, {320});
    th13test::emit(script, 0, 58, 0, 0xff, 0, {});
    th13test::emit(script, 0, 23, 1, 0xff, 8, {0xffffffff});
    th13test::emit(script, 0, 10, 0, 0xff, 0, {});
    auto bytes = th13test::build_scpt({{"WaitExpr", script}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13test::StubVariables variables;
    variables.integers.push_back({-10000, 7});
    th13::EclThreads threads(program, &variables);
    CHECK(threads.start("WaitExpr", 0xff));
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_NEAR(threads.main.time, -6.0, 1e-6);
    CHECK_EQ(threads.main.stack_pointer, 0);
}

}

int main() { return th13test::run_all("ecl-interpreter"); }
