// 录像编码/解码往返回归：顶层头 +0x0A 模式位（00447900:64-68 录制）、
// 名字填充只清 9 字节（004484d0:26-31）、stage 头 22 dword 的卡号恢复与
// 00447900:209-219 normalize 分支。
//
// 审计 P1：旧 encode 把前 12 字节填空格，覆盖 +0x0A 的符卡位；回放侧
// begin_replay_stage 读不到 bit1 → 模式归 0、卡号被 normalize 清掉。
// 本套件用真实 start→begin_stage→record→encode→decode→restore_stage 链路
// 锁定“当前卡号不丢”，不是只拼 top_header 的旁路探针。
#include "../harness/test_framework.hpp"

#include "game/ReplayFile.hpp"
#include "game/ReplayRecorder.hpp"
#include "game/ReplaySession.hpp"

#include <cstring>

namespace {
using namespace th13;

SessionState source_state(u32 mode, i32 card) {
    SessionState state;
    state.stage = 1;
    state.character = 0;
    state.difficulty = 1;
    state.run_mode_flags = mode;
    state.selected_spell_id = card;
    return state;
}

std::vector<u8> encode_one_stage(const SessionState& source, const std::string& name,
                                 const std::vector<u32>& held = {}) {
    HudPlayerState player;
    HudScoreState score;
    ReplayRecorder recorder;
    if (!recorder.start(ReplayRecorder::top_snapshot(source))) return {};
    if (!recorder.begin_stage(ReplayRecorder::stage_snapshot(source, player, score, 12345,
                                                              -6163, 47332, true)))
        return {};
    for (u32 value : held) {
        GameInput input;
        input.held = value;
        input.pressed = value;
        input.released = 0;
        if (!recorder.record(input, 0)) return {};
    }
    std::vector<u8> bytes;
    if (!recorder.encode(bytes, name, 123456)) return {};
    return bytes;
}

// 回放侧装配（ApplicationState::begin_replay_stage 的等价序）：顶层头
// +0x0A bit1 决定模式，+0x70 是候选卡号；stage 头随后在 restore_stage
// 覆盖，并由 00447900:209-219 normalize。
struct ReplayView {
    ReplayFile file;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    GameRandom game_random, visual;
    ReplayRestoreLog log;
    std::string error;

    bool decode(const std::vector<u8>& bytes) {
        if (!file.decode(bytes.data(), bytes.size())) return false;
        const bool spell = file.top_header.size() > 10 && (file.top_header[10] & 2u) != 0;
        progress.run_mode_flags = spell ? 0x20u : 0u;
        progress.selected_spell_id = -1;
        if (spell && file.top_header.size() >= 0x74)
            std::memcpy(&progress.selected_spell_id, file.top_header.data() + 0x70, 4);
        return ReplaySession::restore_stage(
            file, 0, {&progress, &player, &score, &game_random, &visual}, log, error);
    }
};

TEST(top_snapshot_encodes_original_mode_bits) {
    // 00447900:64-68：bit0 = (be830>>4)&1，bit1 = (be830&0x30)==0x20。
    const auto ordinary = ReplayRecorder::top_snapshot(source_state(0x00, -1));
    const auto extra = ReplayRecorder::top_snapshot(source_state(0x10, -1));
    const auto spell = ReplayRecorder::top_snapshot(source_state(0x20, 7));
    const auto both = ReplayRecorder::top_snapshot(source_state(0x30, 7));
    CHECK_EQ(ordinary[0x0A], u8(0x00));
    CHECK_EQ(extra[0x0A], u8(0x01));
    CHECK_EQ(spell[0x0A], u8(0x02));
    // 0x30 不是合法组合：bit1 只在 (mode&0x30)==0x20 时置位，故 bit0=1。
    CHECK_EQ(both[0x0A], u8(0x01));
    // 卡号写顶层 +0x70（4525b0 候选值；最终以 stage 头为准）。
    CHECK_EQ(spell[0x70], u8(7));
}

TEST(encode_preserves_mode_byte_and_roundtrips_name) {
    for (const std::string name : {std::string("AUDIT"), std::string("ABCDEFGH")}) {
        const auto bytes = encode_one_stage(source_state(0x20, 7), name, {0x10, 0x20});
        CHECK(!bytes.empty());
        ReplayFile file;
        CHECK(file.decode(bytes.data(), bytes.size()));
        // 名字填充只清 0x00..0x08，不得覆盖 +0x0A 的符卡模式位。
        CHECK_EQ(file.top_header.size(), size_t(0x74));
        CHECK_EQ(file.top_header[0x0A], u8(0x02));
        CHECK_EQ(file.player_name, name);
        CHECK_EQ(file.stage_count, 1u);
        CHECK_EQ(file.stages.size(), 1u);
        CHECK_EQ(file.stages[0].number, u16(1));
        CHECK_EQ(file.stages[0].stamp, u16(12345));
        // 2 个录制帧 + 保存侧 EOF 6 字节。
        CHECK_EQ(file.stages[0].frame_count, 3u);
        CHECK_EQ(file.stages[0].input.size(), size_t(18));
        CHECK_EQ(file.stages[0].header[0x0C], u8(0xed));  // -6163 = 0xffffe7ed 低字节
        CHECK_EQ(file.stages[0].header[0x0D], u8(0xe7));
    }
}

TEST(spell_replay_restores_selected_card_after_roundtrip) {
    // 审计 P1 主断言：start→record→encode→decode→restore 后卡号 7 仍在，
    // 且模式位保持 0x20（normalize 不把卡号/模式清掉）。
    const auto bytes = encode_one_stage(source_state(0x20, 7), "CARD7");
    CHECK(!bytes.empty());
    ReplayView view;
    CHECK(view.decode(bytes));
    CHECK(view.error.empty());
    CHECK_EQ(view.progress.selected_spell_id, 7);
    CHECK_EQ(view.progress.run_mode_flags & 0x30u, 0x20u);
    CHECK_EQ(view.log.block[7], 7);
    CHECK_EQ(view.log.seed, u16(12345));
    CHECK_EQ(view.player.power, 100);
}

TEST(ordinary_replay_has_no_selected_card) {
    const auto bytes = encode_one_stage(source_state(0x00, -1), "PLAIN");
    CHECK(!bytes.empty());
    ReplayView view;
    CHECK(view.decode(bytes));
    CHECK_EQ(view.progress.selected_spell_id, -1);
    CHECK_EQ(view.progress.run_mode_flags & 0x30u, 0u);
}

TEST(normalize_matches_447900_branch_when_stage_card_present_without_top_bit) {
    // 00447900:209-219 else 分支：stage 头卡号 >=0 而顶层模式位非 0x20 时，
    // 原版先清卡号 -1，再把模式位强制 0x20（不是清 0）。端口必须同构。
    SessionState source = source_state(0x00, 5);
    HudPlayerState player;
    HudScoreState score;
    ReplayRecorder recorder;
    CHECK(recorder.start(ReplayRecorder::top_snapshot(source)));
    CHECK(recorder.begin_stage(ReplayRecorder::stage_snapshot(source, player, score, 9, 0,
                                                              51200, false)));
    std::vector<u8> bytes;
    CHECK(recorder.encode(bytes, "ODD", 0));
    ReplayFile file;
    CHECK(file.decode(bytes.data(), bytes.size()));
    // 回放侧按顶层 bit1（0）装配模式 0，卡号先置 -1。
    SessionState progress;
    HudPlayerState out_player;
    HudScoreState out_score;
    GameRandom game_random, visual;
    ReplayRestoreLog log;
    std::string error;
    CHECK(ReplaySession::restore_stage(file, 0,
        {&progress, &out_player, &out_score, &game_random, &visual}, log, error));
    CHECK_EQ(progress.selected_spell_id, -1);
    CHECK_EQ(progress.run_mode_flags & 0x30u, 0x20u);
}

TEST(normalize_clears_mode_when_stage_card_missing) {
    // 00447900:209-219 if 分支：卡号 <0 → 模式位清 0（即使顶层曾置 0x20）。
    SessionState source = source_state(0x20, -1);
    HudPlayerState player;
    HudScoreState score;
    ReplayRecorder recorder;
    CHECK(recorder.start(ReplayRecorder::top_snapshot(source)));
    CHECK(recorder.begin_stage(ReplayRecorder::stage_snapshot(source, player, score, 9, 0,
                                                              51200, false)));
    std::vector<u8> bytes;
    CHECK(recorder.encode(bytes, "NOCARD", 0));
    ReplayFile file;
    CHECK(file.decode(bytes.data(), bytes.size()));
    SessionState progress;
    progress.run_mode_flags = 0x20;
    progress.selected_spell_id = -1;
    HudPlayerState out_player;
    HudScoreState out_score;
    GameRandom game_random, visual;
    ReplayRestoreLog log;
    std::string error;
    CHECK(ReplaySession::restore_stage(file, 0,
        {&progress, &out_player, &out_score, &game_random, &visual}, log, error));
    CHECK_EQ(progress.selected_spell_id, -1);
    CHECK_EQ(progress.run_mode_flags & 0x30u, 0u);
}

TEST(retry_replaces_stage_block_without_losing_top_mode) {
    // 449070: 同关号重录替换该 stage 块；顶层模式位/名字不受影响。
    SessionState source = source_state(0x20, 7);
    HudPlayerState player;
    HudScoreState score;
    ReplayRecorder recorder;
    CHECK(recorder.start(ReplayRecorder::top_snapshot(source)));
    CHECK(recorder.begin_stage(ReplayRecorder::stage_snapshot(source, player, score, 1, 0,
                                                              51200, false)));
    GameInput input;
    input.held = 1;
    CHECK(recorder.record(input, 0));
    CHECK(recorder.begin_stage(ReplayRecorder::stage_snapshot(source, player, score, 2, 0,
                                                              51200, false)));
    input.held = 2;
    CHECK(recorder.record(input, 0));
    std::vector<u8> bytes;
    CHECK(recorder.encode(bytes, "RETRY", 0));
    ReplayFile file;
    CHECK(file.decode(bytes.data(), bytes.size()));
    CHECK_EQ(file.stage_count, 1u);
    CHECK_EQ(file.stages.size(), 1u);
    CHECK_EQ(file.stages[0].stamp, u16(2));
    CHECK_EQ(file.top_header[0x0A], u8(0x02));
    CHECK_EQ(file.player_name, std::string("RETRY"));
}
}  // namespace

int main() { return th13test::run_all("Replay recorder roundtrip"); }
