#include "../harness/test_framework.hpp"
#include "../../web/cpp/sdl/FileStore.cpp"

#include <filesystem>
#include <fstream>
#include <unistd.h>

using namespace th13;
using namespace th13::sdl;
namespace {
std::string fixture() {
    static i32 serial = 0;
    const char* output = std::getenv("TH13_TEST_OUTPUT");
    const std::string directory = output ? output : "/tmp";
    std::filesystem::create_directories(directory);
    const std::string path = directory + "/summary-" + std::to_string(getpid()) + "-" +
                             std::to_string(serial++) + ".bin";
    CHECK_EQ(setenv("TH13_SAVE_PATH", path.c_str(), 1), 0);
    return path;
}
void write(const std::string& name, const std::vector<u8>& bytes) {
    std::ofstream file(name, std::ios::binary);
    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    CHECK(bool(file));
}
std::vector<u8> read(const std::string& name) {
    std::ifstream file(name, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), {}};
}
}

TEST(summary_defaults_match_original_without_inventing_play_or_clear_history) {
    fixture();
    FileStore files;
    CHECK(files.initialize());
    CHECK(!files.loaded);
    for (i32 character = 0; character < 4; ++character) for (i32 difficulty = 0; difficulty < 5; ++difficulty) {
        TitlePlayerDataSummary summary;
        CHECK(files.player_data_summary(difficulty, character, summary));
        CHECK(summary.available);
        CHECK(summary.history_complete);
        CHECK_EQ(summary.play_count, 0);
        CHECK_EQ(summary.total_frames, 0);
        CHECK_EQ(summary.clear_count, 0);
        for (size_t row = 0; row < 10; ++row) {
            CHECK_EQ(summary.ranking[row].score, 100000 - i32(row) * 10000);
            CHECK(summary.ranking[row].name == "--------");
            CHECK_EQ(summary.ranking[row].stage, 1);
            CHECK_EQ(summary.ranking[row].timestamp, i64(0));
        }
    }
}

TEST(summary_events_keep_character_rank_and_no_continue_identity_and_saturate) {
    fixture();
    FileStore files;
    CHECK(files.record_run_start(2));
    CHECK(files.record_play_frame(2));
    CHECK(files.record_run_clear(2, 3, false));
    CHECK(files.record_run_clear(2, 3, true));
    CHECK(files.cleared(2,3));
    TitlePlayerDataSummary summary;
    CHECK(files.player_data_summary(3,2,summary));
    CHECK_EQ(summary.play_count, 1);
    CHECK_EQ(summary.total_frames, 1);
    CHECK_EQ(summary.clear_count, 2);
    CHECK_EQ(summary.no_continue_clear_count, 1);
    CHECK(files.player_data_summary(2,2,summary));
    CHECK_EQ(summary.clear_count, 0);
    CHECK(files.player_data_summary(3,1,summary));
    CHECK_EQ(summary.play_count, 0);
    files.ordinary_records[0].play_count = 99999;
    files.ordinary_records[0].total_frames = 215999999;
    CHECK(files.record_run_start(0)); CHECK(files.record_play_frame(0));
    CHECK_EQ(files.ordinary_records[0].play_count, 99999);
    CHECK_EQ(files.ordinary_records[0].total_frames, 215999999);
    CHECK(!files.record_run_start(4)); CHECK(!files.record_play_frame(-1));
    CHECK(!files.record_run_clear(0,5,true));
}

TEST(summary_ranking_inserts_ties_before_old_rows_and_roundtrips_real_fields) {
    fixture();
    FileStore files;
    TitlePlayerDataScore score;
    score.score = 90000; score.name = "SYNTHETIC"; score.stage = 8;
    score.continues = 2; score.timestamp = 1700000000; score.progress_percent = 1.25f;
    CHECK_EQ(files.record_high_score(1,2,score), 1);
    CHECK(files.ordinary_records[1].ranking[2][1].name == "SYNTHETI");
    CHECK(files.ordinary_records[1].ranking[2][2].name == "--------");
    CHECK(files.save());
    FileStore restored;
    CHECK(restored.initialize()); CHECK(restored.loaded);
    TitlePlayerDataSummary summary;
    CHECK(restored.player_data_summary(2,1,summary));
    const auto& row = summary.ranking[1];
    CHECK_EQ(row.score,90000); CHECK_EQ(row.stage,8); CHECK_EQ(row.continues,2);
    CHECK_EQ(row.timestamp,i64(1700000000)); CHECK(row.progress_percent == 1.25f);
    CHECK(row.name == "SYNTHETI");
    score.score = 0; CHECK_EQ(files.record_high_score(1,2,score),-1);
}

TEST(summary_v1_through_v4_migration_archives_original_full_file_and_marks_gap) {
    for (i32 version = 1; version <= 4; ++version) {
        const std::string path = fixture();
        size_t size = version == 1 ? 36 : version == 2 ? 52 : version == 3 ? 64 : kV4SaveBytes;
        std::vector<u8> bytes(size,0);
        std::memcpy(bytes.data(),"TH13SAVE",8);
        put32(bytes.data()+8,u32(version)); put32(bytes.data()+16,41); put32(bytes.data()+20,63);
        if (version == 4) {
            std::memcpy(bytes.data()+kV4BaseBytes,"REC1",4);
            put32(bytes.data()+64,5); // v4 Extra lives independently from +36
        }
        if (version > 1) bytes[36] = 0x35;
        bytes[28] = 0xaa; // unknown byte must remain recoverable
        write(path,bytes);
        FileStore files;
        CHECK(files.initialize()); CHECK(files.loaded);
        CHECK(files.migrated_save_bytes() == bytes);
        CHECK(!files.ordinary_records[0].history_complete);
        CHECK(!files.ordinary_records[0].ranking[0][0].available);
        CHECK(files.record_run_start(0));
        CHECK(files.save());
        const auto v5 = read(path);
        CHECK_EQ(get32(v5.data()+8),5u);
        FileStore restored;
        CHECK(restored.initialize()); CHECK(restored.loaded);
        CHECK(restored.migrated_save_bytes() == bytes);
        CHECK(!restored.ordinary_records[0].history_complete);
        CHECK(!restored.ordinary_records[0].ranking[0][0].available);
        CHECK_EQ(restored.ordinary_records[0].play_count,1);
        CHECK_EQ(restored.bgm_volume,41); CHECK_EQ(restored.effect_volume,63);
        if (version > 1) { CHECK_EQ(restored.extra_unlocked_mask,5u); CHECK_EQ(restored.practice_played[0],u8(0x35)); }
    }
}

int main() { return th13test::run_all("save_records_summary_test"); }
