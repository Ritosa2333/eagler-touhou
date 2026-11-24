#include "../harness/test_framework.hpp"

#include "game/ScoreDat.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace {

using th13::ScoreDat;
using th13::u16;
using th13::u32;
using th13::u8;

bool read_file(const std::string& relative, std::vector<u8>& out) {
    static const char* bases[] = {".", "../../..", "../../../..", "../../../../.."};
    for (const char* base : bases) {
        std::FILE* file = std::fopen((std::string(base) + "/" + relative).c_str(), "rb");
        if (!file) continue;
        std::fseek(file, 0, SEEK_END);
        const long size = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);
        out.resize(size_t(size < 0 ? 0 : size));
        const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
        std::fclose(file);
        if (got == out.size()) return true;
        return false;
    }
    return false;
}

bool images_equal(const u8* actual, const u8* expected, size_t size, const char* label) {
    for (size_t i = 0; i < size; ++i) {
        if (actual[i] != expected[i]) {
            std::printf("  %s 首个分歧字节 0x%zX: 实际 %02X 期望 %02X\n", label, i, actual[i], expected[i]);
            return false;
        }
    }
    return true;
}

bool load_golden(const char* name, std::vector<u8>& file, std::vector<u8>& image) {
    if (!read_file(std::string("artifacts/score_golden/") + name + ".bin", file)) {
        std::printf("  缺 artifacts/score_golden/%s.bin\n", name);
        return false;
    }
    if (!read_file(std::string("artifacts/score_golden/") + name + ".image.bin", image)) {
        std::printf("  缺 artifacts/score_golden/%s.image.bin\n", name);
        return false;
    }
    return true;
}

bool parse_and_build(const char* name, ScoreDat& dat, std::vector<u8>& image) {
    std::vector<u8> file;
    if (!load_golden(name, file, image)) return false;
    if (!dat.parse_file(file.data(), file.size())) {
        std::printf("  %s parse_file 失败: %s\n", name, dat.error.c_str());
        return false;
    }
    return true;
}

TEST(parse_golden_route_a_image_equal) {
    ScoreDat dat;
    std::vector<u8> image;
    if (!parse_and_build("route_a_default", dat, image)) {
        CHECK(false);
        return;
    }
    std::vector<u8> built(ScoreDat::kImageSize);
    const size_t length = dat.build_image(built.data(), built.size());
    CHECK_EQ(length, image.size());
    CHECK(images_equal(built.data(), image.data(), image.size(), "route_a"));
}

TEST(defaults_frozen_seed_route_a_image_equal) {
    ScoreDat dat;
    dat.set_defaults(4660);
    std::vector<u8> file, image;
    if (!load_golden("route_a_default", file, image)) {
        CHECK(false);
        return;
    }
    std::vector<u8> built(ScoreDat::kImageSize);
    const size_t length = dat.build_image(built.data(), built.size());
    CHECK_EQ(length, image.size());
    CHECK(images_equal(built.data(), image.data(), image.size(), "defaults(4660)"));
}

TEST(parse_golden_mutated_images_equal) {
    for (const char* name : {"route_b_alpha", "route_b_beta", "route_b_gamma"}) {
        ScoreDat dat;
        std::vector<u8> image;
        if (!parse_and_build(name, dat, image)) {
            CHECK(false);
            return;
        }
        std::vector<u8> built(ScoreDat::kImageSize);
        const size_t length = dat.build_image(built.data(), built.size());
        CHECK_EQ(length, image.size());
        CHECK(images_equal(built.data(), image.data(), image.size(), name));
    }
}

TEST(route_c_dropped_column_quirk) {
    ScoreDat dat;
    std::vector<u8> image;
    if (!parse_and_build("route_c_dropped", dat, image)) {
        CHECK(false);
        return;
    }
    CHECK_EQ(image.size(), size_t(0x15F9C));
    CHECK(!dat.column_present[1]);
    for (int column : {0, 2, 3, 4})
        CHECK(dat.column_present[column]);
    std::vector<u8> built(ScoreDat::kImageSize);
    const size_t length = dat.build_image(built.data(), built.size());
    CHECK_EQ(length, image.size());
    CHECK(images_equal(built.data(), image.data(), image.size(), "route_c"));
}

TEST(corrupted_cr_magic_drops_column_and_shortens) {
    ScoreDat dat;
    std::vector<u8> image;
    if (!parse_and_build("route_a_default", dat, image)) {
        CHECK(false);
        return;
    }
    dat.column_present[2] = false;
    std::vector<u8> built(ScoreDat::kImageSize);
    const size_t length = dat.build_image(built.data(), built.size());
    CHECK_EQ(length, image.size() - ScoreDat::kCRSize);
    const size_t cut = 2 * ScoreDat::kCRSize;
    CHECK(images_equal(built.data(), image.data(), cut, "before dropped column"));
    CHECK(
        images_equal(built.data() + cut, image.data() + cut + ScoreDat::kCRSize, length - cut, "after dropped column"));
    ScoreDat reparsed;
    CHECK(reparsed.parse_image(built.data(), length));
    CHECK(!reparsed.column_present[2]);
    CHECK(reparsed.column_present[0] && reparsed.column_present[1] && reparsed.column_present[3] &&
          reparsed.column_present[4]);
}

TEST(entry_and_progress_field_views) {
    ScoreDat dat;
    dat.set_defaults(4660);
    u8* column = dat.columns[3].data();

    ScoreDat::Entry entry = ScoreDat::read_entry(column, 2, 3);
    CHECK_EQ(entry.score, u32(70000));
    CHECK_EQ(entry.stage, u8(1));
    CHECK_EQ(entry.continues, u8(0));
    entry.score = 123456789;
    entry.stage = 6;
    entry.continues = 4;
    std::memcpy(entry.name.data(), "YOUMU\0\0\0", 9);
    entry.time_lo = 1600000000;
    entry.time_hi = 0x22222222;
    entry.rate_bits = 0x3F800000;
    ScoreDat::write_entry(column, 2, 3, entry);
    const ScoreDat::Entry round = ScoreDat::read_entry(column, 2, 3);
    CHECK_EQ(round.score, u32(123456789));
    CHECK_EQ(round.stage, u8(6));
    CHECK_EQ(round.continues, u8(4));
    CHECK(std::memcmp(round.name.data(), "YOUMU\0\0\0", 9) == 0);
    CHECK_EQ(round.time_lo, u32(1600000000));
    CHECK_EQ(round.time_hi, u32(0x22222222));
    CHECK_EQ(round.rate_bits, u32(0x3F800000));
    CHECK_EQ(column[ScoreDat::kEntriesOff + 2 * ScoreDat::kRankTableStride + 3 * ScoreDat::kEntrySize + 15], u8(0));

    ScoreDat::set_play_count(column, 12345);
    ScoreDat::set_play_frames(column, 215999999);
    ScoreDat::set_clear_count(column, 4, 99998);
    ScoreDat::set_no_continue_clear_count(column, 4, 99999);
    ScoreDat::set_progress_row_score(column, 3, 5, 777777);
    ScoreDat::set_progress_row_flag(column, 3, 5, 0, 1);
    ScoreDat::set_progress_row_flag(column, 3, 5, 1, 1);
    CHECK_EQ(ScoreDat::play_count(column), u32(12345));
    CHECK_EQ(ScoreDat::play_frames(column), u32(215999999));
    CHECK_EQ(ScoreDat::clear_count(column, 4), u32(99998));
    CHECK_EQ(ScoreDat::no_continue_clear_count(column, 4), u32(99999));
    CHECK_EQ(ScoreDat::progress_row_score(column, 3, 5), u32(777777));
    CHECK_EQ(ScoreDat::progress_row_flag(column, 3, 5, 0), u8(1));
    CHECK_EQ(ScoreDat::progress_row_flag(column, 3, 5, 1), u8(1));

    std::array<u8, ScoreDat::kPracticeNameSize> name{};
    std::memcpy(name.data(), "SPELL CARD", 10);
    ScoreDat::set_practice_name(column, 5, name);
    ScoreDat::set_practice_u32(column, 5, ScoreDat::kPracticeCapturesOff + 4, 31);
    ScoreDat::set_practice_u32(column, 5, ScoreDat::kPracticeAttemptsOff, 47);
    ScoreDat::set_practice_u32(column, 5, ScoreDat::kPracticeBestOff, 987654);
    CHECK(std::memcmp(ScoreDat::practice_name(column, 5).data(), "SPELL CARD\0", 11) == 0);
    CHECK_EQ(ScoreDat::practice_u32(column, 5, ScoreDat::kPracticeCapturesOff + 4), u32(31));
    CHECK_EQ(ScoreDat::practice_u32(column, 5, ScoreDat::kPracticeAttemptsOff), u32(47));
    CHECK_EQ(ScoreDat::practice_u32(column, 5, ScoreDat::kPracticeBestOff), u32(987654));
}

TEST(st_name_and_ending_views) {
    ScoreDat dat;
    dat.set_defaults(4660);
    CHECK(std::memcmp(ScoreDat::st_name(dat.st.data()).data(), "        \0", 9) == 0);
    const std::array<u8, 9> name = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 0};
    ScoreDat::set_st_name(dat.st.data(), name);
    CHECK(std::memcmp(ScoreDat::st_name(dat.st.data()).data(), "ABCDEFGH\0", 9) == 0);
    const u8 seen[ScoreDat::kEndingBytes] = {1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    ScoreDat::write_ending_bytes(dat.st.data(), seen);
    u8 read_back[ScoreDat::kEndingBytes] = {};
    ScoreDat::read_ending_bytes(dat.st.data(), read_back);
    CHECK(std::memcmp(read_back, seen, ScoreDat::kEndingBytes) == 0);
}

TEST(lcg_vectors) {
    u16 state = 0;
    CHECK_EQ(ScoreDat::lcg_next(state), u16(0xC374));
    CHECK_EQ(ScoreDat::lcg_next(state), u16(0xBFC7));
    CHECK_EQ(ScoreDat::lcg_next(state), u16(0x1293));
    CHECK_EQ(ScoreDat::lcg_next(state), u16(0x7D40));
    CHECK_EQ(ScoreDat::lcg_next(state), u16(0x1876));
    CHECK_EQ(ScoreDat::lcg_next(state), u16(0xA3CC));
    u16 seed_state = 4660;
    CHECK_EQ(ScoreDat::lcg_next(seed_state), u16(0x7AC4));
    CHECK_EQ(ScoreDat::lcg_next(seed_state), u16(0x1E86));
    CHECK_EQ(ScoreDat::lcg_next(seed_state), u16(0x8D8C));
    CHECK_EQ(ScoreDat::lcg_next(seed_state), u16(0xD9A6));
}

TEST(parse_failure_paths) {
    std::vector<u8> file, image;
    if (!load_golden("route_a_default", file, image)) {
        CHECK(false);
        return;
    }
    ScoreDat dat;
    CHECK(dat.parse_file(file.data(), file.size()));

    std::vector<u8> bad = file;
    bad[0] = 'X';
    CHECK(!dat.parse_file(bad.data(), bad.size()));
    bad = file;
    bad[8] = 5;
    CHECK(!dat.parse_file(bad.data(), bad.size()));
    CHECK(!dat.parse_file(file.data(), 4));
    CHECK(!dat.parse_file(file.data(), 0x18));

    std::vector<u8> broken_image = image;
    broken_image[image.size() - 4] ^= 0x40;
    CHECK(!dat.parse_image(broken_image.data(), broken_image.size()));
    broken_image = image;
    broken_image[0] = 'X';
    broken_image[1] = 'X';
    CHECK(!dat.parse_image(broken_image.data(), broken_image.size()));
}

}

int main() { return th13test::run_all("ScoreDat scoreth13.dat 容器"); }
