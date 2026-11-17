#include "ecl_test_stubs.hpp"

namespace {
using namespace th13;

TEST(synchronous_call_wait_preserves_caller_and_has_no_child_context) {
    std::vector<u8> main, child;
    th13test::emit(main, 0, 40, 0, 255, 0, {0});
    th13test::emit(main, 0, 100, 0, 255, 0, {1});
    const char name[8] = {'c','h','i','l','d',0,0,0};
    th13test::emit(main, 0, 11, 0, 255, 0, {8}, name, sizeof name);
    th13test::emit(main, 0, 100, 0, 255, 0, {3});
    th13test::emit(main, 0, 23, 0, 255, 0, {100});
    th13test::emit(main, 0, 10, 0, 255, 0, {});
    th13test::emit(child, 0, 40, 0, 255, 0, {0});
    th13test::emit(child, 0, 100, 0, 255, 0, {2});
    th13test::emit(child, 0, 23, 0, 255, 0, {3});
    th13test::emit(child, 0, 100, 0, 255, 0, {4});
    th13test::emit(child, 0, 10, 0, 255, 0, {});
    auto bytes = th13test::build_scpt({{"child", child}, {"main", main}});
    EclProgram program;
    CHECK_EQ(program.attach(bytes.data(), u32(bytes.size())), 0);
    th13test::RecordingCommands commands;
    EclThreads threads(program);
    threads.main.commands = &commands;
    CHECK(threads.start("main", 255));
    const int first = threads.step(1);
    if (first == -2) std::printf("call error: %s\n", threads.error.c_str());
    CHECK_EQ(first, 0);
    CHECK_EQ(threads.count(), 1u);
    CHECK_EQ(commands.calls.size(), 2u);
    if (commands.calls.size() != 2) return;
    CHECK_EQ(commands.calls[0].ints[0], 1);
    CHECK_EQ(commands.calls[1].ints[0], 2);
    CHECK_EQ(threads.main.routine, program.find_index("child"));
    CHECK_EQ(threads.main.time, -2.f);
    CHECK_EQ(threads.step(1), 0);
    CHECK_EQ(threads.step(1), 0);
    CHECK_EQ(commands.calls.size(), 2u);
    CHECK_EQ(threads.step(1), 0);
    // 004772a0/00477030: the original binary emits [1,2] at frame0,
    // then [4,3] at frame3, with one unchanged context address throughout.
    CHECK_EQ(commands.calls.size(), 4u);
    CHECK_EQ(commands.calls[2].ints[0], 4);
    CHECK_EQ(commands.calls[3].ints[0], 3);
    CHECK_EQ(threads.main.routine, program.find_index("main"));
    CHECK_EQ(threads.main.time, -99.f);
    CHECK_EQ(threads.count(), 1u);
}
}

int main() { return th13test::run_all("ecl-call-wait-contract"); }
