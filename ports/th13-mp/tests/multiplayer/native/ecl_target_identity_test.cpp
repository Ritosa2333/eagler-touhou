// Production ECL target identities enter the full world capture and digest.
// The same registered tests run standalone and in the cumulative m6 binary.
#include "../../ecl/ecl_test_fixtures.hpp"
#include "multiplayer/WorldState.hpp"

using namespace th13;
using namespace th13test;

namespace {
u64 threads_digest(const EclThreadsSnapshot& threads) {
    StageBattle battle;
    GameRandom visual(123);
    EclProgram fallback;
    auto enemy = std::make_unique<StageEnemy>();
    enemy->threads = std::make_unique<EclThreads>(fallback);
    enemy->threads->main.program = threads.main.program;
    CHECK(enemy->threads->restore(threads));
    battle.sim.enemies.push_back(std::move(enemy));
    multiplayer::WorldState state;
    CHECK(state.capture(battle, visual));
    return state.digest();
}

std::vector<u8> archive() {
    std::vector<u8> script;
    emit(script, 0, 40, 0, 255, 0, {8});
    // Equal instruction bytes at distinct real offsets must remain distinct.
    for (u32 destination : {0u, 0u, 4u})
        emit(script, 0, 91, 2, 255, 0,
             {0, bits_of(float(destination)), 10, 0, bits_of(0.f), bits_of(10.f)});
    emit(script, 0, 23, 0, 255, 0, {100});
    emit(script, 0, 12, 0, 255, 0, {u32(-20), 0});
    return build_scpt({{"First", script}, {"Second", script}});
}

void load(EclProgram& program) {
    const auto bytes = archive();
    CHECK_EQ(program.attach(bytes.data(), u32(bytes.size())), 0);
}

const EclInstruction* instruction(const EclProgram& program, u32 routine, u32 index) {
    const auto& sub = program.subroutine(routine);
    const u8* code = program.definitions[routine].file->bytes.data() + sub.offset + 16;
    const auto* result = reinterpret_cast<const EclInstruction*>(code);
    while (index--) result = reinterpret_cast<const EclInstruction*>(
        reinterpret_cast<const u8*>(result) + result->length);
    return result;
}

float local(const EclContext& context, u32 offset) {
    float value;
    std::memcpy(&value, context.stack.data() + context.local_base + offset, sizeof value);
    return value;
}

TEST(same_targets_in_separate_program_allocations_have_equal_digests) {
    EclProgram a, b;
    load(a); load(b);
    EclThreads x(a), y(b);
    CHECK(x.start("First", 255)); CHECK(y.start("First", 255));
    for (u32 slot = 0; slot < 8; ++slot) {
        x.main.interpolation_targets[slot] = instruction(a, slot % 2, slot % 3);
        y.main.interpolation_targets[slot] = instruction(b, slot % 2, slot % 3);
        CHECK(x.main.interpolation_targets[slot] != y.main.interpolation_targets[slot]);
    }
    auto sx = x.snapshot(), sy = y.snapshot();
    sx.children.push_back(sx.main); sy.children.push_back(sy.main);
    CHECK_EQ(threads_digest(sx), threads_digest(sy));
}

TEST(distinct_offsets_and_routines_are_visible_even_with_identical_instructions) {
    EclProgram program; load(program);
    EclThreads threads(program); CHECK(threads.start("First", 255));
    auto first = threads.snapshot();
    first.main.interpolators[0].begin(10, 0, 0.f, 10.f);
    first.main.interpolation_targets[0] = instruction(program, 0, 1);
    auto other = first;
    other.main.interpolation_targets[0] = instruction(program, 0, 2);
    CHECK_EQ(std::memcmp(first.main.interpolation_targets[0], other.main.interpolation_targets[0],
                         first.main.interpolation_targets[0]->length), 0);
    CHECK(threads_digest(first) != threads_digest(other));
    other.main.interpolation_targets[0] = instruction(program, 1, 1);
    CHECK_EQ(std::memcmp(first.main.interpolation_targets[0], other.main.interpolation_targets[0],
                         first.main.interpolation_targets[0]->length), 0);
    CHECK(threads_digest(first) != threads_digest(other));
}

TEST(child_interpolation_targets_are_part_of_the_same_production_digest) {
    EclProgram program; load(program);
    EclThreads threads(program); CHECK(threads.start("First", 255));
    auto first = threads.snapshot();
    first.children.push_back(first.main);
    first.children[0].interpolation_targets[7] = instruction(program, 0, 1);
    auto other = first;
    other.children[0].interpolation_targets[7] = instruction(program, 0, 2);
    CHECK(threads_digest(first) != threads_digest(other));
    other = first;
    other.children[0].interpolation_targets[7] = instruction(program, 1, 1);
    CHECK(threads_digest(first) != threads_digest(other));
}

TEST(null_valid_and_invalid_targets_have_separate_identities) {
    EclProgram program; load(program);
    EclThreads threads(program); CHECK(threads.start("First", 255));
    const auto empty = threads.snapshot();
    const auto empty_digest = threads_digest(empty);
    auto valid = empty;
    valid.main.interpolation_targets[0] = instruction(program, 0, 0);
    const auto valid_digest = threads_digest(valid);
    CHECK(valid_digest != empty_digest);
    auto invalid = empty;
    invalid.main.interpolation_targets[0] = reinterpret_cast<const EclInstruction*>(std::uintptr_t(1));
    CHECK(threads_digest(invalid) != empty_digest);
    CHECK(threads_digest(invalid) != valid_digest);
    // In-range aligned parameter bytes and unaligned bytes are not instructions.
    for (u32 offset : {1u, 16u}) {
        invalid.main.interpolation_targets[0] = reinterpret_cast<const EclInstruction*>(
            reinterpret_cast<const u8*>(instruction(program, 0, 1)) + offset);
        CHECK(threads_digest(invalid) != empty_digest);
        CHECK(threads_digest(invalid) != valid_digest);
    }
    invalid = valid; invalid.main.program = nullptr;
    CHECK(threads_digest(invalid) != empty_digest);
    CHECK(threads_digest(invalid) != valid_digest);
}

TEST(restored_targets_write_the_original_destination_after_replay) {
    EclProgram program; load(program);
    EclThreads continuous(program), replay(program);
    CHECK(continuous.start("First", 255)); CHECK(replay.start("First", 255));
    CHECK_EQ(continuous.step(1.f), 0);
    CHECK_EQ(local(continuous.main, 0), 0.f);
    CHECK_EQ(local(continuous.main, 4), 1.f);
    auto saved = continuous.snapshot();
    saved.children.push_back(saved.main);
    CHECK(continuous.restore(saved)); CHECK(replay.restore(saved));
    CHECK(replay.main.interpolation_targets[0] == instruction(program, 0, 3));
    CHECK(replay.at(1)->interpolation_targets[0] == instruction(program, 0, 3));
    const auto original_digest = threads_digest(replay.snapshot());
    replay.main.interpolation_targets[0] = instruction(program, 0, 1);
    CHECK(threads_digest(replay.snapshot()) != original_digest);
    CHECK_EQ(replay.step(1.f), 0);
    CHECK_EQ(local(replay.main, 0), 2.f); CHECK_EQ(local(replay.main, 4), 1.f);
    CHECK(replay.restore(saved));
    CHECK_EQ(threads_digest(replay.snapshot()), original_digest);
    CHECK_EQ(continuous.step(1.f), 0); CHECK_EQ(replay.step(1.f), 0);
    CHECK_EQ(local(replay.main, 0), 0.f); CHECK_EQ(local(replay.main, 4), 2.f);
    CHECK_EQ(local(*replay.at(1), 0), 0.f); CHECK_EQ(local(*replay.at(1), 4), 2.f);
    CHECK_EQ(threads_digest(replay.snapshot()), threads_digest(continuous.snapshot()));
}
}

int __attribute__((weak)) main() { return run_all("ECL production target identities"); }
