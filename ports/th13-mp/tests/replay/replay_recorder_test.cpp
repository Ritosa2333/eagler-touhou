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
    if (!recorder.begin_stage(ReplayRecorder::stage_snapshot(source, player, score, 12345, -6163, 47332, true)))
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
        return ReplaySession::restore_stage(file, 0, {&progress, &player, &score, &game_random, &visual}, log, error);
    }
};

TEST(top_snapshot_encodes_original_mode_bits) {
    const auto ordinary = ReplayRecorder::top_snapshot(source_state(0x00, -1));
    const auto extra = ReplayRecorder::top_snapshot(source_state(0x10, -1));
    const auto spell = ReplayRecorder::top_snapshot(source_state(0x20, 7));
    const auto both = ReplayRecorder::top_snapshot(source_state(0x30, 7));
    CHECK_EQ(ordinary[0x0A], u8(0x00));
    CHECK_EQ(extra[0x0A], u8(0x01));
    CHECK_EQ(spell[0x0A], u8(0x02));
    CHECK_EQ(both[0x0A], u8(0x01));
    CHECK_EQ(spell[0x70], u8(7));
}

TEST(encode_preserves_mode_byte_and_roundtrips_name) {
    for (const std::string name : {std::string("AUDIT"), std::string("ABCDEFGH")}) {
        const auto bytes = encode_one_stage(source_state(0x20, 7), name, {0x10, 0x20});
        CHECK(!bytes.empty());
        ReplayFile file;
        CHECK(file.decode(bytes.data(), bytes.size()));
        CHECK_EQ(file.top_header.size(), size_t(0x74));
        CHECK_EQ(file.top_header[0x0A], u8(0x02));
        CHECK_EQ(file.player_name, name);
        CHECK_EQ(file.stage_count, 1u);
        CHECK_EQ(file.stages.size(), 1u);
        CHECK_EQ(file.stages[0].number, u16(1));
        CHECK_EQ(file.stages[0].stamp, u16(12345));
        CHECK_EQ(file.stages[0].frame_count, 2u);
        CHECK_EQ(file.stages[0].input.size(), size_t(12));
        CHECK_EQ(file.stages[0].header[0x0C], u8(0xed));
        CHECK_EQ(file.stages[0].header[0x0D], u8(0xe7));
    }
}

TEST(spell_replay_restores_selected_card_after_roundtrip) {
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
    SessionState source = source_state(0x00, 5);
    HudPlayerState player;
    HudScoreState score;
    ReplayRecorder recorder;
    CHECK(recorder.start(ReplayRecorder::top_snapshot(source)));
    CHECK(recorder.begin_stage(ReplayRecorder::stage_snapshot(source, player, score, 9, 0, 51200, false)));
    std::vector<u8> bytes;
    CHECK(recorder.encode(bytes, "ODD", 0));
    ReplayFile file;
    CHECK(file.decode(bytes.data(), bytes.size()));
    SessionState progress;
    HudPlayerState out_player;
    HudScoreState out_score;
    GameRandom game_random, visual;
    ReplayRestoreLog log;
    std::string error;
    CHECK(
        ReplaySession::restore_stage(file, 0, {&progress, &out_player, &out_score, &game_random, &visual}, log, error));
    CHECK_EQ(progress.selected_spell_id, -1);
    CHECK_EQ(progress.run_mode_flags & 0x30u, 0x20u);
}

TEST(normalize_clears_mode_when_stage_card_missing) {
    SessionState source = source_state(0x20, -1);
    HudPlayerState player;
    HudScoreState score;
    ReplayRecorder recorder;
    CHECK(recorder.start(ReplayRecorder::top_snapshot(source)));
    CHECK(recorder.begin_stage(ReplayRecorder::stage_snapshot(source, player, score, 9, 0, 51200, false)));
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
    CHECK(
        ReplaySession::restore_stage(file, 0, {&progress, &out_player, &out_score, &game_random, &visual}, log, error));
    CHECK_EQ(progress.selected_spell_id, -1);
    CHECK_EQ(progress.run_mode_flags & 0x30u, 0u);
}

TEST(retry_replaces_stage_block_without_losing_top_mode) {
    SessionState source = source_state(0x20, 7);
    HudPlayerState player;
    HudScoreState score;
    ReplayRecorder recorder;
    CHECK(recorder.start(ReplayRecorder::top_snapshot(source)));
    CHECK(recorder.begin_stage(ReplayRecorder::stage_snapshot(source, player, score, 1, 0, 51200, false)));
    GameInput input;
    input.held = 1;
    CHECK(recorder.record(input, 0));
    CHECK(recorder.begin_stage(ReplayRecorder::stage_snapshot(source, player, score, 2, 0, 51200, false)));
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
}

int main() { return th13test::run_all("Replay recorder roundtrip"); }
