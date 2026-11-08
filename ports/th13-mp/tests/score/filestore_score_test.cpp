// FileStore ↔ ScoreDat 集成单测（native 构建直测映射层与裁决；资产依赖见
// run.sh 头注）。语义（docs/plans/scoredat.md 定死）：v6 有效 ⇒ v6 是应用侧
// 唯一权威；th13.scoredat 只是 save_score() 写出的原版格式镜像；dat 导入仅限
// (a) v6 缺失/无效、(b) dat 全条目最大 u64 时间戳 > 榜表最大时间戳。native/WASI
// 构建通道完全不激活（不读不写、无文件访问），因此本套件经公开映射层
// sync_score_face_to_dat()/overlay_score_face_from_dat()（save_score/initialize
// 内部同路径）与裁决 score_dat_should_import() 直测，并断言 native 全程不落 dat：
// 1) v6→ScoreDat 镜像：构造含得分的 v6 档 → save_score() → ScoreDat 模型出现
//    得分面，且 native 不落任何 dat 文件、重载实例 v6 得分面原样（v6 权威）；
// 2) 镜像往返：sync 后 build_image → parse_image 字段保真（校验和重算自洽）；
// 3) 黄金档导入：parse(route_b_alpha) + overlay → 可映射字段照映射（角色映射、
//    ST 表保留文件值不重生成）；规则 (a) 无档必导入；
// 4) v6 权威：坏 dat 字节在侧也不影响装载（native 通道不激活，dat 永不覆盖）；
// 5) save_score 冒烟：镜像 build_file → parse_file 得分一致；重载实例 v6 权威；
//    规则 (b) 等时间戳不导入、dat 明显更新才导入，且导入不臆造妖夢（无映射）行。
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
    // 注意：TH13_SCOREDAT_PATH 不再设置——native 通道已整体不激活（FileStore
    // 不读不写任何 dat 路径），残留环境变量也不该有语义。
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

void check_entry(const ScoreDat::Entry& entry, u32 score, u8 stage, const char* name,
                 u32 time_lo, u32 rate_bits, const char* label) {
    CHECK_EQ(entry.score, score);
    CHECK_EQ(entry.stage, stage);
    CHECK(std::memcmp(entry.name.data(), name, std::strlen(name) + 1) == 0);
    CHECK_EQ(entry.time_lo, time_lo);
    CHECK_EQ(entry.rate_bits, rate_bits);
    (void)label;
}

TEST(v6_migration_into_score_dat) {
    clear_env_files();
    FileStore first;
    CHECK(first.initialize());
    CHECK(!first.loaded);  // 新装
    // 构造 v6 得分面。
    th13::TitlePlayerDataScore score;
    score.score = 500000;
    score.stage = 3;
    score.continues = 2;
    score.name = "PLAYER1";
    score.timestamp = i32(1600000000);
    score.progress_percent = 88.5f;
    score.available = true;
    CHECK_EQ(first.record_high_score(0, 1, score), 0);  // 超过默认 100000 行，插入 rank0
    CHECK(first.record_run_start(0));
    CHECK(first.record_run_clear(0, 1, true));
    first.mark_practice_stage_played(0, 1, 3);
    CHECK(first.record_spell_practice_score(0, 5, 123450));
    first.ending_seen_bytes[0] = 0xA5;
    first.ending_seen_bytes[12] = 0x5A;
    CHECK(first.save_score());  // v6 落盘；native 通道不激活，不落 dat
    CHECK(dat_file_absent());   // native/WASI：save_score 只写 v6，无 dat 副作用

    // 得分面镜像（save_score 内部 sync；与 web 通道写出的镜像同一路径）。
    const ScoreDat& dat = first.score_dat;
    // 榜表：port char0→chara0、diff1→块1、rank0（v6 +5 continues 不映射，恒 0）。
    check_entry(ScoreDat::read_entry(dat.cr[1].data(), 0, 0), 500000, 3, "PLAYER1",
                u32(1600000000), th13::float_to_bits(88.5f), "inserted");
    const ScoreDat::Entry shifted = ScoreDat::read_entry(dat.cr[1].data(), 0, 1);
    CHECK_EQ(shifted.score, u32(100000));  // 被压下的默认行
    CHECK(std::memcmp(shifted.name.data(), "--------\0", 9) == 0);
    // 进度：通关/无续关通关（0x42cd40 块内 +0x7C0/+0x7DC）= 1；游戏回数不映射 = 0。
    CHECK_EQ(ScoreDat::progress_u32(dat.cr[1].data(), 0x7C0, 0), u32(1));
    CHECK_EQ(ScoreDat::progress_u32(dat.cr[1].data(), 0x7DC, 0), u32(1));
    CHECK_EQ(ScoreDat::progress_u32(dat.cr[1].data(), 0x7B8, 0), u32(0));
    // 未涉及的难度块保持默认（除练习合流外）：block0 chara0 clear = 0。
    CHECK_EQ(ScoreDat::progress_u32(dat.cr[0].data(), 0x7C0, 0), u32(0));
    // 游玩旗：row(chara0, stage2)+5 = 1，其余 stage 与 +4 通过旗不臆造。
    CHECK_EQ(ScoreDat::progress_row_flag(dat.cr[1].data(), 0, 2, 1), u8(1));
    CHECK_EQ(ScoreDat::progress_row_flag(dat.cr[1].data(), 0, 0, 1), u8(0));
    CHECK_EQ(ScoreDat::progress_row_flag(dat.cr[1].data(), 0, 2, 0), u8(0));
    // 练习最好成绩合流：每块 slot5 +0x98 = 123450；slot6 = 0。
    for (int diff = 0; diff < 5; ++diff) {
        CHECK_EQ(ScoreDat::practice_u32(dat.cr[size_t(diff)].data(), 5, 0x98), u32(123450));
        CHECK_EQ(ScoreDat::practice_u32(dat.cr[size_t(diff)].data(), 6, 0x98), u32(0));
    }
    // 练习记录号/关卡 id 为默认表值（非零即越界写证据）。
    CHECK_EQ(ScoreDat::practice_u32(dat.cr[1].data(), 0, 0x94), u32(0));
    CHECK_EQ(ScoreDat::practice_u32(dat.cr[1].data(), 126, 0x94), u32(5));
    // ending seen 13 字节（ST+0x16..0x22）。
    u8 seen[ScoreDat::kEndingBytes];
    ScoreDat::read_ending_bytes(dat.st.data(), seen);
    CHECK_EQ(seen[0], u8(0xA5));
    CHECK_EQ(seen[12], u8(0x5A));
    // ST 上次名：端口无名牌追踪，保持默认 8 空格。
    CHECK(std::memcmp(ScoreDat::st_name(dat.st.data()).data(), "        \0", 9) == 0);

    // 重载实例：v6 是应用侧唯一权威——native 通道不激活（score_dat_loaded 为
    // 假），v6 得分面原样（continues/时间戳/达成率不被镜像缺席字段冲掉）。
    FileStore second;
    CHECK(second.initialize());
    CHECK(second.loaded);
    CHECK(!second.score_dat_loaded());
    const auto& row = second.ordinary_records[0].ranking[1][0];
    CHECK_EQ(row.score, 500000);
    CHECK_EQ(row.stage, 3);
    CHECK_EQ(row.continues, 2);
    CHECK_EQ(row.timestamp, i64(1600000000));
    CHECK(th13::float_to_bits(row.progress_percent) == th13::float_to_bits(88.5f));
    CHECK(row.name == "PLAYER1");
}

TEST(migrated_image_round_trip) {
    clear_env_files();
    FileStore first;
    CHECK(first.initialize());
    th13::TitlePlayerDataScore score;
    score.score = 12345678;
    score.stage = 6;
    score.continues = 0;
    score.name = "ROUNDTRP";
    score.timestamp = i32(1700000000);
    score.progress_percent = 100.0f;
    score.available = true;
    CHECK_EQ(first.record_high_score(2, 4, score), 0);  // 早苗→chara4、Extra→块4
    CHECK(first.record_run_clear(2, 4, true));
    CHECK(first.save_score());
    CHECK(dat_file_absent());  // native 通道不激活：镜像只在内存

    std::vector<u8> image(ScoreDat::kImageSize);
    const size_t length = first.score_dat.build_image(image.data(), image.size());
    CHECK_EQ(length, ScoreDat::kImageSize);
    ScoreDat reparsed;
    CHECK(reparsed.parse_image(image.data(), length));
    check_entry(ScoreDat::read_entry(reparsed.cr[4].data(), 4, 0), 12345678, 6, "ROUNDTRP",
                u32(1700000000), th13::float_to_bits(100.0f), "roundtrip");
    CHECK_EQ(ScoreDat::progress_u32(reparsed.cr[4].data(), 0x7DC, 4), u32(1));
    // 重解析后再 build 逐字节稳定（校验和重算自洽）。
    std::vector<u8> rebuilt(ScoreDat::kImageSize);
    CHECK_EQ(reparsed.build_image(rebuilt.data(), rebuilt.size()), length);
    CHECK(std::memcmp(image.data(), rebuilt.data(), length) == 0);
}

TEST(golden_overlay_on_load) {
    clear_env_files();
    std::vector<u8> golden;
    if (!read_file("artifacts/score_golden/route_b_alpha.bin", golden)) {
        std::printf("  缺 artifacts/score_golden/route_b_alpha.bin（见 run.sh 头注）\n");
        CHECK(false);
        return;
    }
    CHECK(write_file(kDatPath, golden));  // 在侧 dat 文件（native 不应有任何读者）
    FileStore store;
    CHECK(store.initialize());
    CHECK(!store.loaded);               // 无 v6 档
    CHECK(!store.score_dat_loaded());   // native 通道不激活：装载不读 dat 文件
    std::vector<u8> after;
    CHECK(read_file(kDatPath, after));  // 且不改写在侧文件
    CHECK(after == golden);
    // 规则 (a)：v6 缺失/无效 ⇒ 任何 dat（含时间戳 0）都具备导入资格。
    CHECK(store.score_dat_should_import(u64(0)));

    // 导入路径（与 web initialize() 采用分支同代码）：镜像 parse + overlay。
    CHECK(store.score_dat.parse_file(golden.data(), golden.size()));
    CHECK(store.score_dat.max_entry_timestamp() > u64(0));
    store.overlay_score_face_from_dat();
    // 块0 chara2 rank3（meta route_b_alpha entry 突变）→ port 角色1（魔理沙）、diff0。
    const auto& cell = store.ordinary_records[1].ranking[0][3];
    CHECK_EQ(cell.score, 123456789);
    CHECK_EQ(cell.stage, 6);
    CHECK_EQ(cell.continues, 0);
    CHECK(cell.name == "PLAYER1");
    CHECK_EQ(cell.timestamp, i64(1600000000));
    CHECK_EQ(th13::float_to_bits(cell.progress_percent), u32(1118765056));  // 100.0f
    CHECK(cell.available);
    // 默认行保持原版默认（available=true：原版 60 行常在）。
    CHECK_EQ(store.ordinary_records[1].ranking[0][0].score, 100000);
    CHECK(store.ordinary_records[1].ranking[0][0].available);
    // 通关计数：块1（route_b_alpha block_raw(1,1992)=通关数 chara4）→ port 角色2。
    // 注意原版 +0x7C0 块内通关数对应 port clear_count[diff]。
    CHECK_EQ(store.ordinary_records[2].clear_count[1],
             i32(std::min(ScoreDat::progress_u32(store.score_dat.cr[1].data(), 0x7C0, 4), 99999u)));
    // ST 表保留文件值（首存后不重生成；route_b_alpha 种子 4660 链首 0x7AC4）。
    const u16 table0 = u16(store.score_dat.st[ScoreDat::kStTableOff]) |
                       u16(store.score_dat.st[ScoreDat::kStTableOff + 1] << 8);
    CHECK_EQ(table0, u16(0x7AC4));
    CHECK(std::memcmp(ScoreDat::st_name(store.score_dat.st.data()).data(), "NAME0008\0", 9) == 0);
}

TEST(corrupt_scoredat_falls_back_to_v6) {
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
    // 坏 dat 字节在侧（非 'TH31' 垃圾）：native 通道不激活，读都不发生。
    const u8 junk[] = {'j', 'u', 'n', 'k'};
    CHECK(write_file(kDatPath, {junk, junk + 4}));
    ScoreDat probe;
    CHECK(!probe.parse_file(junk, 4));  // 坏档在 ScoreDat 层确实失败（回退依据）
    FileStore second;
    CHECK(second.initialize());
    CHECK(second.loaded);
    CHECK(!second.score_dat_loaded());  // dat 永不覆盖 v6
    CHECK_EQ(second.ordinary_records[0].ranking[0][0].score, 777777);
    CHECK(second.ordinary_records[0].ranking[0][0].name == "FALLBACK");
    CHECK_EQ(second.ordinary_records[0].ranking[0][0].timestamp, i64(42));
}

TEST(save_score_scoredat_smoke) {
    if (!FileStore::score_dat_file_ready()) {
        std::printf("# SKIP save_score_scoredat_smoke: ReplayFile::deflate 未落地\n");
        return;
    }
    clear_env_files();
    FileStore first;
    CHECK(first.initialize());
    th13::TitlePlayerDataScore score;
    score.score = 9999990;
    score.stage = 5;
    score.name = "SMOKE001";
    score.timestamp = i32(1800000000);
    score.progress_percent = 97.5f;
    score.available = true;
    // 映射角色（早苗=2）进通道；妖夢（3）无原版槽位，得分只留在 v6。
    CHECK_EQ(first.record_high_score(2, 2, score), 0);
    th13::TitlePlayerDataScore youmu;
    youmu.score = 8888888;
    youmu.stage = 5;
    youmu.name = "YOUMU001";
    youmu.timestamp = i32(1800000001);
    youmu.progress_percent = 90.0f;
    youmu.available = true;
    CHECK_EQ(first.record_high_score(3, 2, youmu), 0);
    CHECK(first.save_score());
    CHECK(dat_file_absent());  // native 通道不激活：无 dat 落盘副作用

    // 镜像 → build_file → parse_file 冒烟（web 通道写出的字节同源）。
    std::vector<u8> stored;
    CHECK(first.score_dat.build_file(stored));
    ScoreDat dat;
    CHECK(dat.parse_file(stored.data(), stored.size()));
    check_entry(ScoreDat::read_entry(dat.cr[2].data(), 4, 0), 9999990, 5, "SMOKE001",
                u32(1800000000), th13::float_to_bits(97.5f), "smoke");
    // 妖夢面（chara 无映射）：所有块 chara 0..5 均无 8888888 行（不臆造写入）。
    for (int diff = 0; diff < 5; ++diff)
        for (int chara = 0; chara < 6; ++chara)
            CHECK(ScoreDat::read_entry(dat.cr[size_t(diff)].data(), chara, 0).score !=
                  u32(8888888));

    // 重载实例：v6 是唯一权威——两行（含妖夢）都从 v6 原样读回。
    FileStore second;
    CHECK(second.initialize());
    CHECK(second.loaded);
    CHECK(!second.score_dat_loaded());
    CHECK_EQ(second.ordinary_records[2].ranking[2][0].score, 9999990);
    CHECK_EQ(second.ordinary_records[3].ranking[2][0].score, 8888888);
    CHECK(second.ordinary_records[3].ranking[2][0].name == "YOUMU001");
    // 规则 (b)：镜像最大时间戳 == 榜表最大时间戳（1800000001，妖夢行）⇒ 不算
    // "明显更新"，dat 不导入；镜像无妖夢行也永不回冲 v6。
    CHECK_EQ(second.max_ranking_timestamp(), i64(1800000001));
    CHECK(!second.score_dat_should_import(u64(1800000001)));
    CHECK(!second.score_dat_should_import(u64(1800000002) - 1));
    // dat 明显更新（榜表时间戳被回拨）才具备导入资格；导入只覆盖可映射字段。
    for (auto& records : second.ordinary_records)
        for (auto& rank : records.ranking)
            for (auto& entry : rank) entry.timestamp = 5;
    CHECK(second.score_dat_should_import(u64(1800000001)));
    second.score_dat = dat;
    second.overlay_score_face_from_dat();
    CHECK_EQ(second.ordinary_records[2].ranking[2][0].score, 9999990);  // dat 覆盖
    CHECK_EQ(second.ordinary_records[3].ranking[2][0].score, 8888888);  // 妖夢 v6 自留
    CHECK(second.ordinary_records[3].ranking[2][0].name == "YOUMU001");
}

}  // namespace

int main() {
    return th13test::run_all("FileStore scoreth13.dat 通道");
}
