// Named hypotheses: legacy archival migration is lossless; new Practice and
// Extra writes are independent; spell counters retain mode/character identity.
#include "../harness/test_framework.hpp"
#include "../../web/cpp/sdl/FileStore.cpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <unistd.h>

using namespace th13;
using namespace th13::sdl;

namespace {
std::string path() {
    static i32 serial = 0;
    const char* output = std::getenv("TH13_TEST_OUTPUT");
    const std::string directory = output ? output : "/tmp";
    std::filesystem::create_directories(directory);
    const std::string result = directory + "/records-" + std::to_string(getpid()) +
                               "-" + std::to_string(serial++) + ".bin";
    CHECK_EQ(setenv("TH13_SAVE_PATH", result.c_str(), 1), 0);
    return result;
}
void write(const std::string& name, const std::vector<u8>& bytes) {
    std::ofstream file(name, std::ios::binary);
    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    CHECK(bool(file));
}
std::vector<u8> legacy(i32 version) {
    std::vector<u8> bytes(version == 1 ? 36 : version == 2 ? 52 : 64);
    for (size_t i = 0; i < bytes.size(); ++i) bytes[i] = u8(0xa0 + i);
    std::memcpy(bytes.data(), "TH13SAVE", 8);
    put32(bytes.data() + 8, u32(version));
    put32(bytes.data() + 12, 0xf0010001);
    put32(bytes.data() + 16, 43);
    put32(bytes.data() + 20, 65);
    put32(bytes.data() + 24, 0xfedcba85);
    if (version > 1) bytes[36] = 0xd5; // played=0x15, legacy Extra=5
    if (version > 2) {
        bytes[52] = 7; bytes[53] = 0;
        bytes[54] = 0xff; bytes[55] = 0xff; // invalid binding remains archived
    }
    return bytes;
}
}

TEST(legacy_v1_v2_v3_raw_bytes_and_observable_state_survive_repeated_v4_saves) {
    for (i32 version = 1; version <= 3; ++version) {
        const std::string file = path();
        const auto bytes = legacy(version);
        write(file, bytes);
        FileStore initial;
        CHECK(initial.initialize());
        CHECK(initial.loaded);
        CHECK_EQ(initial.bgm_volume, 43);
        CHECK_EQ(initial.effect_volume, 65);
        CHECK_EQ(initial.heard_mask, 0x85u); // keep the legacy reader's mask
        CHECK_EQ(initial.cleared_mask, 0xf0010001u);
        CHECK_EQ(initial.legacy_save_size(), bytes.size());
        CHECK(std::equal(bytes.begin(), bytes.end(), initial.legacy_save_bytes().begin()));
        const auto played = initial.practice_played;
        CHECK_EQ(initial.extra_unlocked_mask, version > 1 ? 5u : 0u);
        CHECK_EQ(initial.legacy_practice_extra_overlap, version > 1 ? 5u : 0u);
        CHECK_EQ(initial.pad_bindings[0], version > 2 ? 7 : 0);
        CHECK_EQ(initial.pad_bindings[1], 1);
        initial.set_extra_unlocked(3);
        CHECK(initial.practice_played == played);
        CHECK(initial.save());
        for (i32 pass = 0; pass < 2; ++pass) {
            FileStore restored;
            CHECK(restored.initialize());
            CHECK(restored.loaded);
            CHECK_EQ(restored.legacy_save_size(), bytes.size());
            CHECK(std::equal(bytes.begin(), bytes.end(), restored.legacy_save_bytes().begin()));
            CHECK(restored.practice_played == played);
            CHECK_EQ(restored.extra_unlocked_mask, (version > 1 ? 5u : 0u) | 8u);
            CHECK(restored.save());
        }
    }
}

TEST(new_v4_practice_and_extra_have_no_cross_write) {
    path();
    FileStore initial;
    initial.mark_practice_stage_played(0, 0, 1);
    CHECK_EQ(initial.extra_unlocked_mask, 0u);
    initial.set_extra_unlocked(2);
    CHECK(initial.save());
    FileStore restored;
    CHECK(restored.initialize());
    CHECK(restored.practice_stage_played(0, 0, 1));
    CHECK(!restored.practice_stage_played(0, 0, 3));
    CHECK_EQ(restored.extra_unlocked_mask, 4u);
    CHECK_EQ(restored.legacy_practice_extra_overlap, 0u);
    CHECK_EQ(restored.legacy_save_size(), 0u);
}

TEST(initial_v4_records_remain_readable_without_fabricated_provenance_or_best) {
    const std::string file = path();
    std::vector<u8> bytes(kV4BaseBytes);
    std::memcpy(bytes.data(), "TH13SAVE", 8);
    put32(bytes.data() + 8, 4);
    put32(bytes.data() + 16, 100);
    put32(bytes.data() + 20, 80);
    std::memcpy(bytes.data() + kSpellRecordsOffset, "synthetic", 9);
    put32(bytes.data() + kSpellRecordsOffset + 128 + 8, 3);
    write(file, bytes);
    FileStore restored;
    CHECK(restored.initialize());
    CHECK(restored.loaded);
    CHECK_EQ(restored.spell_counters(0, 0)->attempts[0], 3);
    CHECK_EQ(restored.spell_counters(0, 0)->practice_best_score, 0);
    CHECK_EQ(restored.legacy_save_size(), 0u);
    CHECK(restored.save());
    FileStore upgraded;
    CHECK(upgraded.initialize());
    CHECK(upgraded.loaded);
    CHECK_EQ(upgraded.spell_counters(0, 0)->attempts[0], 3);
}

TEST(spell_modes_characters_aggregate_saturation_best_and_names_roundtrip) {
    path();
    FileStore initial;
    CHECK(initial.record_spell_attempt(1, 12, "synthetic", false));
    CHECK(initial.record_spell_attempt(1, 12, "synthetic", true));
    CHECK(initial.record_spell_attempt(2, 12, "synthetic", false));
    CHECK(initial.record_spell_capture(1, 12, false));
    CHECK(initial.record_spell_capture(1, 12, true));
    CHECK(initial.record_spell_practice_score(1, 12, 12349));
    CHECK(initial.record_spell_practice_score(1, 12, 10000));
    initial.spell_records[12].columns[2].attempts[1] = 99999;
    CHECK(initial.record_spell_attempt(2, 12, "synthetic", true));
    CHECK_EQ(initial.spell_counters(2, 12)->attempts[1], 99999);
    CHECK(initial.save());
    FileStore restored;
    CHECK(restored.initialize());
    const auto* one = restored.spell_counters(1, 12);
    const auto* all = restored.spell_counters(4, 12);
    CHECK_EQ(one->attempts[0], 1);
    CHECK_EQ(one->attempts[1], 1);
    CHECK_EQ(one->captures[0], 1);
    CHECK_EQ(one->captures[1], 1);
    CHECK_EQ(one->practice_best_score, 12340); // original stores internal tens
    CHECK_EQ(all->attempts[0], 2);
    CHECK_EQ(all->attempts[1], 2);
    CHECK_EQ(all->captures[0], 1);
    CHECK_EQ(all->captures[1], 1);
    CHECK_EQ(all->practice_best_score, 0); // 0042cd40 writes only the chosen character
    CHECK_EQ(restored.spell_counters(0, 12)->attempts[0], 0);
    TitlePlayerDataRecord row;
    CHECK(restored.player_data_record(2, 1, 3, row)); // fourth Hard slot is id12
    CHECK_EQ(row.slot, 12);
    CHECK(row.recorded);
    CHECK_EQ(row.normal_captures, 1);
    CHECK_EQ(row.practice_captures, 1);
    CHECK_EQ(row.practice_best_score, 12340);
    CHECK(!restored.record_spell_attempt(4, 12, "bad", false));
    CHECK(!restored.record_spell_capture(0, 127, false));
    CHECK(!restored.record_spell_practice_score(0, 12, -1));
}

int main() { return th13test::run_all("save_records_migration_test"); }
