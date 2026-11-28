#include "../harness/test_framework.hpp"

#include "game/ScoreDat.hpp"
#include "sdl/FileStore.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace {

using th13::i32;
using th13::i64;
using th13::ScoreDat;
using th13::u32;
using th13::u64;
using th13::u8;
using th13::sdl::FileStore;

const char* kSavePath = "/tmp/th13_score_mapping_save.bin";

void clear_env_files() {
    std::remove(kSavePath);
    ::setenv("TH13_SAVE_PATH", kSavePath, 1);
}

bool read_file(const std::string& path, std::vector<u8>& out) {
    std::FILE* file = std::fopen(path.c_str(), "rb");
    if (!file) return false;
    std::fseek(file, 0, SEEK_END);
    const long size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    out.resize(size_t(size < 0 ? 0 : size));
    const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
    std::fclose(file);
    return got == out.size();
}

bool write_file(const std::string& path, const std::vector<u8>& bytes) {
    std::FILE* file = std::fopen(path.c_str(), "wb");
    if (!file) return false;
    const bool ok = std::fwrite(bytes.data(), 1, bytes.size(), file) == bytes.size();
    std::fclose(file);
    return ok;
}

u32 get32(const u8* at) {
    u32 value = 0;
    for (size_t i = 0; i < 4; ++i)
        value |= u32(at[i]) << (i * 8);
    return value;
}

const char* character_name(i32 character) {
    static const char* names[] = {"REIMU", "MARISA", "SANA", "YOUMU"};
    return names[character];
}

void populate_store(FileStore& store) {
    constexpr i32 kSpellSlot = 5;
    for (i32 character = 0; character < 4; ++character) {
        auto& records = store.ordinary_records[size_t(character)];
        records.play_count = 10 + character;
        records.total_frames = 1000 + character * 7;
        for (i32 difficulty = 0; difficulty < 5; ++difficulty) {
            th13::TitlePlayerDataScore score;
            score.score = 1000000 + character * 20000 + difficulty * 300;
            score.stage = 1 + difficulty;
            score.continues = (character + difficulty) % 4;
            score.name = character_name(character);
            score.timestamp = 1700000000 + character * 100 + difficulty;
            score.progress_percent = 50.0f + character + difficulty;
            score.available = true;
            CHECK_EQ(store.record_high_score(character, difficulty, score), 0);
            records.clear_count[size_t(difficulty)] = 11 + character * 5 + difficulty;
            records.no_continue_clear_count[size_t(difficulty)] = 7 + character * 3 + difficulty;
            if (difficulty < 4)
                store.mark_practice_stage_played(character, difficulty, (character + difficulty) % 6 + 1);
        }
        store.ending_seen_bytes[size_t(character * 2 + 1)] |= u8(0x10);
        store.set_extra_unlocked(character);

        const i32 slot = kSpellSlot;
        CHECK(store.record_spell_attempt(character, slot, "MAPPING", false));
        CHECK(store.record_spell_capture(character, slot, false));
        CHECK(store.record_spell_attempt(character, slot, "MAPPING", true));
        CHECK(store.record_spell_practice_score(character, slot, 10000 + character * 10));
    }
}

void check_mirror(const ScoreDat& dat) {
    constexpr i32 kSpellSlot = 5;
    u64 newest = 0;
    for (i32 character = 0; character < 4; ++character) {
        const u8* column = dat.columns[size_t(character)].data();
        CHECK_EQ(ScoreDat::play_count(column), u32(10 + character));
        CHECK_EQ(ScoreDat::play_frames(column), u32(1000 + character * 7));
        for (i32 difficulty = 0; difficulty < 5; ++difficulty) {
            const ScoreDat::Entry entry = ScoreDat::read_entry(column, difficulty, 0);
            CHECK_EQ(entry.score, u32(1000000 + character * 20000 + difficulty * 300));
            CHECK_EQ(entry.stage, u8(1 + difficulty));
            CHECK_EQ(entry.continues, u8((character + difficulty) % 4));
            CHECK(std::memcmp(entry.name.data(), character_name(character),
                              std::strlen(character_name(character)) + 1) == 0);
            CHECK_EQ(entry.time_lo, u32(1700000000 + character * 100 + difficulty));
            CHECK_EQ(ScoreDat::clear_count(column, difficulty), u32(11 + character * 5 + difficulty));
            CHECK_EQ(ScoreDat::no_continue_clear_count(column, difficulty), u32(7 + character * 3 + difficulty));
            if (difficulty < 4) {
                const i32 stage = (character + difficulty) % 6;
                CHECK_EQ(ScoreDat::progress_row_flag(column, difficulty, stage, 1), u8(1));
                CHECK_EQ(ScoreDat::progress_row_flag(column, difficulty, (stage + 1) % 6, 1), u8(0));
            }
            newest = std::max(newest, u64(entry.time_lo));
        }
    }
    CHECK_EQ(dat.max_entry_timestamp(), newest);
    CHECK_EQ(ScoreDat::read_entry(dat.columns[4].data(), 0, 0).score, u32(100000));

    for (i32 character = 0; character < 4; ++character) {
        const i32 slot = kSpellSlot;
        const u8* personal = dat.columns[size_t(character)].data();
        CHECK_EQ(ScoreDat::practice_u32(personal, slot, ScoreDat::kPracticeAttemptsOff), u32(1));
        CHECK_EQ(ScoreDat::practice_u32(personal, slot, ScoreDat::kPracticeAttemptsOff + 4), u32(1));
        CHECK_EQ(ScoreDat::practice_u32(personal, slot, ScoreDat::kPracticeCapturesOff), u32(1));
        CHECK_EQ(ScoreDat::practice_u32(personal, slot, ScoreDat::kPracticeBestOff), u32(10000 + character * 10));
    }
    {
        const i32 slot = kSpellSlot;
        const u8* aggregate = dat.columns[4].data();
        CHECK_EQ(ScoreDat::practice_u32(aggregate, slot, ScoreDat::kPracticeAttemptsOff), u32(4));
        CHECK_EQ(ScoreDat::practice_u32(aggregate, slot, ScoreDat::kPracticeAttemptsOff + 4), u32(4));
        CHECK_EQ(ScoreDat::practice_u32(aggregate, slot, ScoreDat::kPracticeCapturesOff), u32(4));
        CHECK_EQ(ScoreDat::practice_u32(aggregate, slot, ScoreDat::kPracticeBestOff), u32(10030));
        CHECK(std::memcmp(ScoreDat::practice_name(aggregate, slot).data(), "MAPPING\0", 8) == 0);
    }
}

void check_imported(const FileStore& store) {
    constexpr i32 kSpellSlot = 5;
    for (i32 character = 0; character < 4; ++character) {
        const auto& records = store.ordinary_records[size_t(character)];
        CHECK_EQ(records.play_count, 10 + character);
        CHECK_EQ(records.total_frames, 1000 + character * 7);
        CHECK(records.history_complete);
        CHECK(store.extra_unlocked(character));
        for (i32 difficulty = 0; difficulty < 5; ++difficulty) {
            const auto& row = records.ranking[size_t(difficulty)][0];
            CHECK_EQ(row.score, 1000000 + character * 20000 + difficulty * 300);
            CHECK_EQ(row.stage, 1 + difficulty);
            CHECK_EQ(row.continues, (character + difficulty) % 4);
            CHECK(row.name == character_name(character));
            CHECK_EQ(row.timestamp, 1700000000 + character * 100 + difficulty);
            CHECK_EQ(records.clear_count[size_t(difficulty)], 11 + character * 5 + difficulty);
            CHECK_EQ(records.no_continue_clear_count[size_t(difficulty)], 7 + character * 3 + difficulty);
        }
    }
    for (i32 character = 0; character < 4; ++character) {
        const auto& counters = *store.spell_counters(character, kSpellSlot);
        CHECK_EQ(counters.attempts[0], 1);
        CHECK_EQ(counters.attempts[1], 1);
        CHECK_EQ(counters.captures[0], 1);
        CHECK_EQ(counters.captures[1], 0);
        CHECK_EQ(counters.practice_best_score, 10000 + character * 10);
    }
    {
        const auto& aggregate = *store.spell_counters(4, kSpellSlot);
        CHECK_EQ(aggregate.attempts[0], 4);
        CHECK_EQ(aggregate.attempts[1], 4);
        CHECK_EQ(aggregate.captures[0], 4);
        CHECK_EQ(aggregate.practice_best_score, 10030);
        CHECK(store.spell_records[size_t(kSpellSlot)].name == "MAPPING");
    }
    CHECK_EQ(store.max_ranking_timestamp(), i64(1700000304));
}

TEST(character_columns_and_difficulty_tables_round_trip) {
    clear_env_files();
    FileStore store;
    CHECK(store.initialize());
    populate_store(store);
    store.score_dat.columns[1][ScoreDat::kMidOff] = 0xA5;
    store.score_dat.column_present[2] = false;
    CHECK(store.save_score());
    check_mirror(store.score_dat);
    CHECK_EQ(store.score_dat.columns[1][ScoreDat::kMidOff], u8(0xA5));
    CHECK(store.score_dat.column_present[2]);

    std::vector<u8> file;
    CHECK(store.score_dat.build_file(file));
    ScoreDat imported;
    CHECK(imported.parse_file(file.data(), file.size()));
    check_mirror(imported);

    ::setenv("TH13_SAVE_PATH", "/tmp/th13_score_mapping_import.bin", 1);
    std::remove("/tmp/th13_score_mapping_import.bin");
    FileStore destination;
    CHECK(destination.initialize());
    CHECK(!destination.loaded);
    CHECK(destination.score_dat_should_import(imported.max_entry_timestamp()));
    destination.practice_played.fill(0x3f);
    destination.score_dat = imported;
    destination.overlay_score_face_from_dat();
    check_imported(destination);

    ::setenv("TH13_SAVE_PATH", kSavePath, 1);
    FileStore reloaded;
    CHECK(reloaded.initialize());
    CHECK(reloaded.loaded);
    check_imported(reloaded);
    ScoreDat::set_progress_row_flag(reloaded.score_dat.columns[0].data(), 0, 5, 1, 1);
    CHECK(reloaded.save_score());
    CHECK_EQ(ScoreDat::progress_row_flag(reloaded.score_dat.columns[0].data(), 0, 5, 1), u8(0));
    check_mirror(reloaded.score_dat);
}

TEST(practice_flags_v8_and_original_row_offsets) {
    clear_env_files();
    FileStore store;
    CHECK(store.initialize());
    store.mark_practice_stage_played(0, 0, 2);
    store.mark_practice_stage_cleared(0, 0, 3);
    store.mark_practice_stage_played(3, 3, 6);
    store.mark_practice_stage_cleared(3, 3, 6);
    CHECK(store.save_score());
    const u8* column = store.score_dat.columns[0].data();
    CHECK_EQ(column[0x805], u8(1));
    CHECK_EQ(column[0x80c], u8(1));
    ScoreDat::set_progress_row_score(store.score_dat.columns[0].data(), 0, 0, 777777);
    CHECK_EQ(get32(column + 0x7f8), u32(777777));
    CHECK_EQ(store.score_dat.columns[3].data()[0x8e4], u8(1));
    CHECK_EQ(store.score_dat.columns[3].data()[0x8e5], u8(1));
    CHECK_EQ(column[0x804], u8(0));
    CHECK_EQ(column[0x808], u8(0));

    std::vector<u8> image(ScoreDat::kImageSize);
    CHECK_EQ(store.score_dat.build_image(image.data(), image.size()), ScoreDat::kImageSize);
    ScoreDat imported;
    CHECK(imported.parse_image(image.data(), image.size()));
    store.score_dat = imported;
    store.overlay_score_face_from_dat();
    CHECK(store.practice_stage_cleared(0, 0, 3));
    CHECK(store.practice_stage_cleared(3, 3, 6));
    for (i32 character = 0; character < 4; ++character) {
        for (i32 difficulty = 0; difficulty < 4; ++difficulty) {
            for (i32 stage = 1; stage <= 6; ++stage) {
                const bool expected = (character == 0 && difficulty == 0 && stage == 3) ||
                                      (character == 3 && difficulty == 3 && stage == 6);
                CHECK_EQ(store.practice_stage_cleared(character, difficulty, stage), expected);
            }
        }
    }

    store.mark_practice_stage_cleared(0, 0, 3);
    store.mark_practice_stage_cleared(3, 3, 6);
    CHECK(store.save());
    std::vector<u8> v8;
    CHECK(read_file(kSavePath, v8));
    FileStore reloaded;
    CHECK(reloaded.initialize());
    CHECK(reloaded.loaded);
    CHECK(reloaded.practice_stage_cleared(0, 0, 3));
    CHECK(reloaded.practice_stage_cleared(3, 3, 6));

    std::vector<u8> v7(v8.begin(), v8.end() - 24);
    v7[8] = 7;
    CHECK(write_file(kSavePath, v7));
    FileStore legacy;
    CHECK(legacy.initialize());
    CHECK(legacy.loaded);
    for (const auto flags : legacy.practice_cleared)
        CHECK_EQ(flags, u8(0));
}

}

int main() { return th13test::run_all("ScoreDat 角色/难度坐标"); }
