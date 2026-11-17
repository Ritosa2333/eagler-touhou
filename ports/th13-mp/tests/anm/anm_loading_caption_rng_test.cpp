#include "anm_test_fixtures.hpp"
#include "game/AnmEnvironment.hpp"

using namespace th13;

// 原版回放直入装载序的 visual RNG 逐点契约（A5，round-s12）。
// 证据：原版 oracle boot 迹线（TH13_VISUAL_RNG_TRACE=1，oracle.battle_oracle
// --replay games/replays/th13_udPn16.rpy --replay-stage 0 --boot-seed 0，
// artifacts/round-s12/A5/native-trace-boot/battle-visual-rng-trace.jsonl）：
//   0x43a040 boot：0x4dc31c/0x4dc324 ← timeGetTime()（合成 0）；
//   0x45ac90 字体噪声 256 步（index 256，seed 57045）；
//   f0：0x42bfa0 destination0xc 建 ASCII script17 字幕 → 2 次 10022 抽
//        （0x4621b0:0x2726 → 0x472c90，index 260，seed 30195）；
//   f1：装载帧常规更新整链 → 12 次指令抽取（0x461f3c/0x461f5f/0x461f82
//        → 0x472de0/0x472e10，index 284，seed 40152）；
//   f4（首输入帧）：对局侧再 +20（index 304，seed 33690，与
//    battle-replay-core-v2.jsonl 帧 0 逐位一致）。
// 生产对应：ApplicationState::draw_loading(false)（建字幕）后，装载帧照常
// 推进装载 ANM 主/副链（BrowserRuntime th13_debug_replay 补的这一步）。
// 缺这一步时 index 停在 260，对局首帧后为 280≠304——这正是修复前 Web 侧
// 全窗 visual_seed/visual_index 首差的根因。

TEST(loading_caption_boot_sequence_matches_original_visual_rng_window) {
    GameRandom visual{0};
    for (int i = 0; i < 256; ++i) (void)visual.step16();  // 0x45ac90 噪声表
    CHECK_EQ(visual.index(), 256u);
    CHECK_EQ(visual.seed(), 57045u);

    AnmEnvironment environment;
    environment.resolution_scale = 1;
    environment.raster_scale = 1;
    AnmManager manager{visual, environment};
    th13test::TestFileReader reader;
    std::vector<u8> bytes;
    CHECK(reader.read("artifacts/reference/raw/ascii.anm", bytes));
    CHECK(manager.load(2, bytes.data(), u32(bytes.size())));
    CHECK(manager.sprite_fallback(2));

    // ApplicationState::draw_loading(false) 同行。port 侧创建即时首拍不抽
    // （原版 f0 的 2 次 10022 抽在 port 侧由对局侧等量补偿，总量一致）。
    const u32 prayer = manager.create(2, 17, -1, 0, {480, 392, 0});
    CHECK(prayer != 0);
    CHECK_EQ(visual.index(), 256u);

    CHECK(manager.update());          // 装载帧常规更新：主链+副链一遍
    CHECK_EQ(visual.index(), 280u);   // 12 次指令抽取（= 原版 f1 的 12 抽）
    CHECK_EQ(visual.seed(), 47358u);  // 原版迹线同位置（index 280, seed 47358）
}

int main() { return th13test::run_all("anm loading caption boot rng window"); }
