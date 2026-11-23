// FileStore 与 ScoreDat 的应用档权威、原版格式镜像和导入裁决测试。
// 黄金样本不在此处重复验证；原始容器见 score_dat_test.cpp，资产依赖见 run.sh。
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
using th13::u8;
using th13::u16;
using th13::u32;
using th13::u64;
using th13::ScoreDat;
using th13::sdl::FileStore;

const char* kSavePath = "/tmp/th13_score_test_save.bin";
const char* kDatPath = "/tmp/th13_score_test_scoredat.bin";

void clear_env_files() {
    std::remove(kSavePath);
    std::remove(kDatPath);
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

bool dat_file_absent() {
    std::FILE* probe = std::fopen(kDatPath, "rb");
    if (!probe) return true;
    std::fclose(probe);
    return false;
}

void check_entry(const ScoreDat::Entry& entry, u32 score, u8 stage, u8 continues,
                 const char* name, u32 time_lo, u32 rate_bits) {
    CHECK_EQ(entry.score, score);
    CHECK_EQ(entry.stage, stage);
    CHECK_EQ(entry.continues, continues);
    CHECK(std::memcmp(entry.name.data(), name, std::strlen(name) + 1) == 0);
    CHECK_EQ(entry.time_lo, time_lo);
    CHECK_EQ(entry.rate_bits, rate_bits);
}

TEST(v7_authority_and_score_dat_mirror) {
    clear_env_files();
    FileStore first;
    CHECK(first.initialize());
    CHECK(!first.loaded);

    th13::TitlePlayerDataScore score;
    score.score = 500000;
    score.stage = 3;
    score.continues = 2;
    score.name = "PLAYER1";
    score.timestamp = 1600000000;
    score.progress_percent = 88.5f;
    score.available = true;
    CHECK_EQ(first.record_high_score(0, 1, score), 0);
    CHECK(first.record_run_start(0));
    for (i32 frame = 0; frame < 1234; ++frame) CHECK(first.record_play_frame(0));
    CHECK(first.record_run_clear(0, 1, true));
    first.mark_practice_stage_played(0, 1, 3);
    CHECK(first.record_spell_practice_score(0, 5, 123450));
    first.ending_seen_bytes[0] = 0xA5;
    first.ending_seen_bytes[12] = 0x5A;
    CHECK(first.save_score());
    CHECK(dat_file_absent());

    const ScoreDat& dat = first.score_dat;
    check_entry(ScoreDat::read_entry(dat.columns[0].data(), 1, 0), 500000, 3, 2,
                "PLAYER1", u32(1600000000), th13::float_to_bits(88.5f));
    CHECK_EQ(ScoreDat::read_entry(dat.columns[0].data(), 1, 1).score, u32(100000));
    CHECK_EQ(ScoreDat::play_count(dat.columns[0].data()), u32(1));
    CHECK_EQ(ScoreDat::play_frames(dat.columns[0].data()), u32(1234));
    CHECK_EQ(ScoreDat::clear_count(dat.columns[0].data(), 1), u32(1));
    CHECK_EQ(ScoreDat::no_continue_clear_count(dat.columns[0].data(), 1), u32(1));
    CHECK_EQ(ScoreDat::progress_row_flag(dat.columns[0].data(), 1, 2, 1), u8(1));
    CHECK_EQ(ScoreDat::progress_row_flag(dat.columns[0].data(), 1, 0, 1), u8(0));
    CHECK_EQ(ScoreDat::practice_u32(dat.columns[0].data(), 5,
                                    ScoreDat::kPracticeBestOff),
             u32(123450));
    CHECK_EQ(ScoreDat::practice_u32(dat.columns[4].data(), 5,
                                    ScoreDat::kPracticeBestOff),
             u32(123450));

    u8 seen[ScoreDat::kEndingBytes];
    ScoreDat::read_ending_bytes(dat.st.data(), seen);
    CHECK_EQ(seen[0], u8(0xA5));
    CHECK_EQ(seen[12], u8(0x5A));

    FileStore second;
    CHECK(second.initialize());
    CHECK(second.loaded);
    CHECK(!second.score_dat_loaded());
    const auto& row = second.ordinary_records[0].ranking[1][0];
    CHECK_EQ(row.score, 500000);
    CHECK_EQ(row.stage, 3);
    CHECK_EQ(row.continues, 2);
    CHECK_EQ(row.timestamp, i64(1600000000));
    CHECK_EQ(second.ordinary_records[0].play_count, 1);
    CHECK_EQ(second.ordinary_records[0].total_frames, 1234);
    CHECK(row.name == "PLAYER1");
}

TEST(mirror_image_round_trip) {
    clear_env_files();
    FileStore first;
    CHECK(first.initialize());
    th13::TitlePlayerDataScore score;
    score.score = 12345678;
    score.stage = 6;
    score.continues = 0;
    score.name = "ROUNDTRP";
    score.timestamp = 1700000000;
    score.progress_percent = 100.0f;
    score.available = true;
    CHECK_EQ(first.record_high_score(2, 4, score), 0);
    CHECK(first.record_run_clear(2, 4, true));
    CHECK(first.save_score());

    std::vector<u8> image(ScoreDat::kImageSize);
    const size_t length = first.score_dat.build_image(image.data(), image.size());
    CHECK_EQ(length, ScoreDat::kImageSize);
    ScoreDat reparsed;
    CHECK(reparsed.parse_image(image.data(), length));
    check_entry(ScoreDat::read_entry(reparsed.columns[2].data(), 4, 0), 12345678, 6, 0,
                "ROUNDTRP", u32(1700000000), th13::float_to_bits(100.0f));
    CHECK_EQ(ScoreDat::no_continue_clear_count(reparsed.columns[2].data(), 4), u32(1));
    std::vector<u8> rebuilt(ScoreDat::kImageSize);
    CHECK_EQ(reparsed.build_image(rebuilt.data(), rebuilt.size()), length);
    CHECK(std::memcmp(image.data(), rebuilt.data(), length) == 0);
}

TEST(last_name_persists_and_mirrors_st_name) {
    clear_env_files();
    FileStore first;
    CHECK(first.initialize());
    CHECK(first.last_name == "        ");
    first.remember_name("MYSTIA");
    CHECK(first.last_name == "MYSTIA  ");
    CHECK(first.save_score());
    CHECK(std::memcmp(ScoreDat::st_name(first.score_dat.st.data()).data(),
                      "MYSTIA  \0", 9) == 0);
    std::vector<u8> v7;
    CHECK(read_file(kSavePath, v7));

    FileStore second;
    CHECK(second.initialize());
    CHECK(second.loaded);
    CHECK(second.last_name == "MYSTIA  ");
    second.remember_name("ABCDEFGHIJ");
    CHECK(second.last_name == "ABCDEFGH");
    second.remember_name(std::string("A\x01Z\xff", 4));
    CHECK(second.last_name == "A Z     ");

    std::vector<u8> v6(v7.begin(), v7.end() - 16);
    v6[8] = 6;
    v6[9] = v6[10] = v6[11] = 0;
    CHECK(write_file(kSavePath, v6));
    FileStore third;
    CHECK(third.initialize());
    CHECK(third.loaded);
    CHECK(third.last_name == "        ");

    std::vector<u8> damaged = v7;
    damaged[damaged.size() - 16] = 'X';
    CHECK(write_file(kSavePath, damaged));
    FileStore fourth;
    CHECK(fourth.initialize());
    CHECK(!fourth.loaded);
}

TEST(corrupt_scoredat_falls_back_to_v7) {
    clear_env_files();
    FileStore first;
    CHECK(first.initialize());
    th13::TitlePlayerDataScore score;
    score.score = 777777;
    score.stage = 2;
    score.name = "FALLBACK";
    score.timestamp = 42;
    score.progress_percent = 0.0f;
    score.available = true;
    CHECK_EQ(first.record_high_score(0, 0, score), 0);
    CHECK(first.save_score());

    const u8 junk[] = {'j', 'u', 'n', 'k'};
    CHECK(write_file(kDatPath, {junk, junk + 4}));
    ScoreDat probe;
    CHECK(!probe.parse_file(junk, 4));
    FileStore second;
    CHECK(second.initialize());
    CHECK(second.loaded);
    CHECK(!second.score_dat_loaded());
    CHECK_EQ(second.ordinary_records[0].ranking[0][0].score, 777777);
    CHECK(second.ordinary_records[0].ranking[0][0].name == "FALLBACK");
    CHECK_EQ(second.ordinary_records[0].ranking[0][0].timestamp, i64(42));
}

TEST(score_dat_file_round_trip_smoke) {
    clear_env_files();
    FileStore first;
    CHECK(first.initialize());
    th13::TitlePlayerDataScore score;
    score.score = 9999990;
    score.stage = 5;
    score.continues = 3;
    score.name = "SMOKE001";
    score.timestamp = 1800000000;
    score.progress_percent = 97.5f;
    score.available = true;
    CHECK_EQ(first.record_high_score(3, 2, score), 0);
    CHECK(first.save_score());
    CHECK(dat_file_absent());

    std::vector<u8> stored;
    CHECK(first.score_dat.build_file(stored));
    ScoreDat dat;
    CHECK(dat.parse_file(stored.data(), stored.size()));
    check_entry(ScoreDat::read_entry(dat.columns[3].data(), 2, 0), 9999990, 5, 3,
                "SMOKE001", u32(1800000000), th13::float_to_bits(97.5f));
    CHECK_EQ(dat.max_entry_timestamp(), u64(1800000000));

    FileStore second;
    CHECK(second.initialize());
    CHECK(second.loaded);
    CHECK(!second.score_dat_loaded());
    CHECK_EQ(second.ordinary_records[3].ranking[2][0].score, 9999990);
    CHECK_EQ(second.max_ranking_timestamp(), i64(1800000000));
    CHECK(!second.score_dat_should_import(u64(1800000000)));
    CHECK(second.score_dat_should_import(u64(1800000001)));
    second.score_dat = dat;
    second.overlay_score_face_from_dat();
    CHECK_EQ(second.ordinary_records[3].ranking[2][0].score, 9999990);
    CHECK_EQ(second.ordinary_records[3].ranking[2][0].continues, 3);
}

TEST(original_youmu_fixture_import) {
    clear_env_files();
    std::vector<u8> fixture;
    if (!read_file("artifacts/score_golden/youmu_original_roundtrip.bin", fixture)) {
        std::printf("  缺 artifacts/score_golden/youmu_original_roundtrip.bin\n");
        CHECK(false);
        return;
    }
    ScoreDat dat;
    CHECK(dat.parse_file(fixture.data(), fixture.size()));
    const u8* column = dat.columns[3].data();
    check_entry(ScoreDat::read_entry(column, 2, 0), 24682468, 6, 2, "YOUMU001",
                u32(1800000001), 0x42666666);
    CHECK_EQ(ScoreDat::play_count(column), u32(137));
    CHECK_EQ(ScoreDat::play_frames(column), u32(215999999));
    CHECK_EQ(ScoreDat::clear_count(column, 2), u32(31));
    CHECK_EQ(ScoreDat::no_continue_clear_count(column, 2), u32(29));
    CHECK_EQ(ScoreDat::progress_row_score(column, 2, 3), u32(777777));
    CHECK_EQ(ScoreDat::practice_u32(column, 5, ScoreDat::kPracticeCapturesOff), u32(31));
    CHECK_EQ(ScoreDat::practice_u32(column, 5, ScoreDat::kPracticeAttemptsOff), u32(37));
    CHECK_EQ(ScoreDat::practice_u32(column, 5, ScoreDat::kPracticeBestOff), u32(123450));

    FileStore store;
    CHECK(store.initialize());
    CHECK(!store.loaded);
    CHECK(store.score_dat_should_import(dat.max_entry_timestamp()));
    store.score_dat = dat;
    store.overlay_score_face_from_dat();
    const auto& records = store.ordinary_records[3];
    CHECK_EQ(records.ranking[2][0].score, 24682468);
    CHECK_EQ(records.ranking[2][0].continues, 2);
    CHECK(records.ranking[2][0].name == "YOUMU001");
    CHECK_EQ(records.play_count, 137);
    CHECK_EQ(records.total_frames, 215999999);
    CHECK_EQ(records.clear_count[2], 31);
    CHECK_EQ(records.no_continue_clear_count[2], 29);
    CHECK(store.practice_stage_played(3, 2, 4));
    CHECK(store.extra_unlocked(3));
    const auto& spell = *store.spell_counters(3, 5);
    CHECK_EQ(spell.captures[0], 31);
    CHECK_EQ(spell.captures[1], 29);
    CHECK_EQ(spell.attempts[0], 37);
    CHECK_EQ(spell.attempts[1], 41);
    CHECK_EQ(spell.practice_best_score, 123450);
    CHECK(store.spell_records[5].name == "YOUMUCARD");
    CHECK(store.last_name == "YOUMU001");
}

}  // namespace

int main() {
    return th13test::run_all("FileStore scoreth13.dat 通道");
}
