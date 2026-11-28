#include "ecl_test_stubs.hpp"

namespace {

TEST(threads_spawn_order_and_new_thread_next_frame) {
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
    CHECK_EQ(program.subroutine(threads.at(1)->routine).name, std::string("GirlA01_at_l"));
    CHECK_EQ(program.subroutine(threads.at(2)->routine).name, std::string("GirlA01_at"));
    CHECK_NEAR(threads.at(1)->time, 0.0, 1e-6);
    const auto calls_before = commands.calls.size();
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK(commands.calls.size() > calls_before);
    CHECK_NEAR(threads.at(1)->time, -29.0, 1e-6);
    CHECK_NEAR(threads.at(2)->time, -29.0, 1e-6);
}

TEST(threads_pause_resume_kill) {
    std::vector<th13::u8> script;
    {
        const char name[] = "Child";
        const th13::u32 size = 8;
        th13::u32 first = 0;
        std::memcpy(&first, name, 4);
        std::vector<th13::u8> tail;
        tail.insert(tail.end(), name + 4, name + sizeof name);
        tail.push_back(0);
        tail.push_back(0);
        const th13::u32 id = 5;
        tail.insert(tail.end(), reinterpret_cast<const th13::u8*>(&id), reinterpret_cast<const th13::u8*>(&id) + 4);
        th13test::emit(script, 0, 16, 0, 0xff, 0, {size, first}, tail.data(), th13::u32(tail.size()));
    }
    th13test::emit(script, 0, 40, 0, 0xff, 0, {0});
    th13test::emit(script, 0, 23, 0, 0xff, 0, {3});
    th13test::emit(script, 0, 18, 0, 0xff, 0, {5});
    th13test::emit(script, 0, 23, 0, 0xff, 0, {3});
    th13test::emit(script, 0, 19, 0, 0xff, 0, {5});
    th13test::emit(script, 0, 23, 0, 0xff, 0, {3});
    th13test::emit(script, 0, 17, 0, 0xff, 0, {5});
    th13test::emit(script, 0, 23, 0, 0xff, 0, {3});
    th13test::emit(script, 0, 10, 0, 0xff, 0, {});
    std::vector<th13::u8> child;
    th13test::emit(child, 0, 23, 0, 0xff, 0, {2});
    th13test::emit(child, 0, 12, 0, 0xff, 0, {0xffffffecu, 0});
    auto bytes = th13test::build_scpt({{"Child", child}, {"Main", script}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13::EclThreads threads(program);
    CHECK(threads.start("Main", 0xff));
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.count(), 2u);
    CHECK(threads.find(5) != &threads.main);
    CHECK_EQ(threads.find(5)->thread_id, 5);
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), 0);
    const float frozen = threads.find(5)->time;
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_NEAR(threads.find(5)->time, frozen, 1e-6);
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.count(), 1u);
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.step(1.0f), -1);
    CHECK(threads.error.empty());
}

TEST(threads_spawn_with_argument_descriptor) {
    std::vector<th13::u8> script;
    th13test::emit(script, 0, 40, 0, 0xff, 0, {0});
    {
        const char name[] = "Worker";
        const th13::u32 size = 8;
        th13::u32 first = 0;
        std::memcpy(&first, name, 4);
        std::vector<th13::u8> tail;
        tail.insert(tail.end(), name + 4, name + sizeof name);
        tail.push_back(0);
        const th13::u8 descriptor[] = {'i', 'i', 0, 0, 0x4b, 0xd9, 0xff, 0xff};
        tail.insert(tail.end(), descriptor, descriptor + sizeof descriptor);
        th13test::emit(script, 0, 15, 2, 0xff, 0, {size, first}, tail.data(), th13::u32(tail.size()));
    }
    th13test::emit(script, 0, 23, 0, 0xff, 0, {3});
    th13test::emit(script, 0, 10, 0, 0xff, 0, {});
    std::vector<th13::u8> worker;
    th13test::emit(worker, 0, 40, 0, 0xff, 0, {4});
    th13test::emit(worker, 0, 638, 1, 0xff, 0, {0});
    th13test::emit(worker, 0, 23, 0, 0xff, 0, {2});
    th13test::emit(worker, 0, 10, 0, 0xff, 0, {});
    auto bytes = th13test::build_scpt({{"Main", script}, {"Worker", worker}});
    th13::EclProgram program;
    CHECK(program.attach(bytes.data(), th13::u32(bytes.size())) == 0);
    th13test::RecordingCommands commands;
    th13test::StubVariables variables;
    variables.integers.push_back({-9909, 77});
    th13::EclThreads threads(program, &variables);
    threads.main.commands = &commands;
    CHECK(threads.start("Main", 0xff));
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(threads.count(), 2u);
    CHECK_EQ(threads.step(1.0f), 0);
    CHECK_EQ(commands.calls.size(), 1u);
    CHECK_EQ(commands.calls[0].opcode, 638);
    CHECK_EQ(commands.calls[0].ints[0], 77);
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
    for (int i = 0; i < 10; ++i)
        CHECK_EQ(threads.step(1.0f), 0);
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
    for (int i = 0; i < 5; ++i)
        CHECK_EQ(threads.step(1.0f), 0);
    th13::EclFile file;
    const auto snapshot = threads.snapshot();
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
    variables.integers.push_back({-10000, 7});
    variables.floats.push_back({-9989, 2.0f});
    variables.floats.push_back({-9988, 47.0f});
    variables.floats.push_back({-9999, 0.25f});
    variables.floats.push_back({-9998, 1.5f});
    variables.floats.push_back({-9995, 100.0f});
    variables.floats.push_back({-9994, 200.0f});
    variables.floats.push_back({-9987, 5.0f});
    variables.integers.push_back({-9986, 0});
    variables.integers.push_back({-9985, 0});
    variables.integers.push_back({-9984, 0});
    variables.floats.push_back({-9981, 0.0f});
    variables.integers.push_back({-9946, 3});
    variables.integers.push_back({-9908, 3});
    variables.integers.push_back({-9960, 3});
    variables.integers.push_back({-9959, 3});
    variables.floats.push_back({-9954, 1000.0f});
    variables.floats.push_back({-9963, 0.0f});
    variables.floats.push_back({-9962, 0.0f});
    variables.floats.push_back({-9991, 100.0f});
    variables.floats.push_back({-9990, 200.0f});
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

}

int main() { return th13test::run_all("ecl-threads"); }
