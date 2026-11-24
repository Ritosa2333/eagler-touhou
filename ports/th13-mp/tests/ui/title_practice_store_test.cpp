#include "../../web/cpp/sdl/FileStore.cpp"
#include "../harness/test_framework.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>
#include <unistd.h>
#include <vector>

using namespace th13;
using namespace th13::sdl;
namespace {
std::string fixture_path() {
    static int next = 0;
    const char* directory = std::getenv("TH13_TEST_OUTPUT");
    if (directory) {
        std::filesystem::create_directories(directory);
    }
    const std::string path = std::string(directory ? directory : "/tmp") + "/practice-save-" +
                             std::to_string(getpid()) + "-" + std::to_string(next++) + ".bin";
    CHECK_EQ(setenv("TH13_SAVE_PATH", path.c_str(), 1), 0);
    return path;
}
void put32(std::vector<u8>& bytes, size_t at, u32 value) { std::memcpy(bytes.data() + at, &value, 4); }
void write_fixture(const std::string& path, const std::vector<u8>& bytes) {
    FILE* file = std::fopen(path.c_str(), "wb");
    CHECK(file != nullptr);
    if (!file) return;
    CHECK_EQ(std::fwrite(bytes.data(), 1, bytes.size(), file), bytes.size());
    CHECK_EQ(std::fclose(file), 0);
}
std::vector<u8> legacy_save() {
    std::vector<u8> bytes(36);
    std::memcpy(bytes.data(), "TH13SAVE", 8);
    put32(bytes, 8, 1);
    put32(bytes, 12, 1u << 7);
    put32(bytes, 16, 41);
    put32(bytes, 20, 63);
    put32(bytes, 24, 0x85);
    return bytes;
}
}

TEST(practice_missing_save_does_not_invent_stage1_unlock) {
    fixture_path();
    FileStore files;
    CHECK(files.initialize());
    CHECK(!files.loaded);
    for (i32 c = 0; c < 4; ++c)
        for (i32 d = 0; d < 4; ++d)
            for (i32 s = 1; s <= 6; ++s)
                CHECK(!files.practice_stage_played(c, d, s));
}

TEST(practice_v1_preserves_settings_and_clears_unknown_played_data) {
    const std::string path = fixture_path();
    write_fixture(path, legacy_save());
    FileStore files;
    files.mark_practice_stage_played(0, 0, 1);
    CHECK(files.initialize());
    CHECK(files.loaded);
    CHECK_EQ(files.bgm_volume, 41);
    CHECK_EQ(files.effect_volume, 63);
    CHECK_EQ(files.cleared_mask, 1u << 7);
    CHECK_EQ(files.heard_mask, 0x85u);
    for (u8 played : files.practice_played)
        CHECK_EQ(played, u8(0));
    files.mark_practice_stage_played(3, 2, 6);
    CHECK(files.save());
    FileStore roundtrip;
    CHECK(roundtrip.initialize());
    CHECK(roundtrip.loaded);
    CHECK(roundtrip.practice_stage_played(3, 2, 6));
    CHECK_EQ(roundtrip.cleared_mask, 1u << 7);
    CHECK_EQ(roundtrip.bgm_volume, 41);
    CHECK_EQ(roundtrip.heard_mask, 0x85u);
}

TEST(practice_v2_roundtrips_all_stage_character_and_rank_dimensions) {
    fixture_path();
    FileStore files;
    for (i32 c = 0; c < 4; ++c)
        for (i32 d = 0; d < 4; ++d)
            files.mark_practice_stage_played(c, d, (c + d) % 6 + 1);
    CHECK(files.save());
    FileStore read;
    CHECK(read.initialize());
    CHECK(read.loaded);
    for (i32 c = 0; c < 4; ++c)
        for (i32 d = 0; d < 4; ++d)
            for (i32 s = 1; s <= 6; ++s)
                CHECK_EQ(read.practice_stage_played(c, d, s), s == (c + d) % 6 + 1);
    const auto before = read.practice_played;
    for (const std::array<i32, 3>& bad :
         {std::array<i32, 3>{-1, 0, 1}, {4, 0, 1}, {0, -1, 1}, {0, 4, 1}, {0, 0, 0}, {0, 0, 7}}) {
        CHECK(!read.practice_stage_played(bad[0], bad[1], bad[2]));
        read.mark_practice_stage_played(bad[0], bad[1], bad[2]);
    }
    CHECK(read.practice_played == before);
}

TEST(practice_corrupt_truncated_or_overlong_saves_revert_to_defaults) {
    std::vector<std::vector<u8>> invalid;
    auto bytes = legacy_save();
    bytes[0] = 'X';
    invalid.push_back(bytes);
    bytes = legacy_save();
    put32(bytes, 8, 2);
    invalid.push_back(bytes);
    bytes = legacy_save();
    bytes.resize(35);
    invalid.push_back(bytes);
    bytes = legacy_save();
    bytes.resize(52);
    invalid.push_back(bytes);
    bytes = legacy_save();
    put32(bytes, 8, 2);
    bytes.resize(53);
    invalid.push_back(bytes);
    for (const auto& corrupt : invalid) {
        const std::string path = fixture_path();
        write_fixture(path, corrupt);
        FileStore files;
        files.cleared_mask = 7;
        files.bgm_volume = 1;
        files.mark_practice_stage_played(0, 0, 1);
        CHECK(files.initialize());
        CHECK(!files.loaded);
        CHECK_EQ(files.cleared_mask, 0u);
        CHECK_EQ(files.bgm_volume, 100);
        for (u8 played : files.practice_played)
            CHECK_EQ(played, u8(0));
    }
}

TEST(practice_v2_roundtrips_extra_unlock_mask) {
    fixture_path();
    {
        FileStore files;
        CHECK(files.initialize());
        files.set_extra_unlocked(0);
        files.set_extra_unlocked(3);
        CHECK(files.save());
    }
    FileStore reloaded;
    CHECK(reloaded.initialize());
    CHECK(reloaded.loaded);
    CHECK(reloaded.extra_unlocked(0));
    CHECK(!reloaded.extra_unlocked(1));
    CHECK(!reloaded.extra_unlocked(2));
    CHECK(reloaded.extra_unlocked(3));
    const std::string legacy = fixture_path();
    write_fixture(legacy, legacy_save());
    FileStore v1;
    CHECK(v1.initialize());
    CHECK(v1.loaded);
    for (i32 character = 0; character < 4; ++character)
        CHECK(!v1.extra_unlocked(character));
    const std::array<i16, 5> expected_defaults = {0, 1, 2, 3, 4};
    CHECK(v1.pad_bindings == expected_defaults);
}

TEST(pad_bindings_v3_roundtrip_and_defaults) {
    fixture_path();
    {
        FileStore files;
        CHECK(files.initialize());
        files.pad_bindings = {4, 3, 2, 1, 0};
        CHECK(files.save());
    }
    FileStore reloaded;
    CHECK(reloaded.initialize());
    CHECK(reloaded.loaded);
    const std::array<i16, 5> expected_custom = {4, 3, 2, 1, 0};
    CHECK(reloaded.pad_bindings == expected_custom);
}

int main() { return th13test::run_all("Practice save migration"); }
