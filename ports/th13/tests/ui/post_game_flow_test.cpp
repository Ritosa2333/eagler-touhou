#include "../harness/test_framework.hpp"

#include "game/EndingScene.hpp"
#include "game/PostGameFlow.hpp"
#include "game/ScoreBoard.hpp"

#include <map>
#include <vector>

using namespace th13;

namespace {

struct RecordPlatform final : PostGamePlatform {
    std::vector<i32> requests, bgms;
    int practice_ends = 0;
    bool practice_end_result = true;
    void request_destination(i32 raw) override { requests.push_back(raw); }
    void bgm(i32 id) override { bgms.push_back(id); }
    bool request_practice_end() override {
        ++practice_ends;
        return practice_end_result;
    }
};

struct FlowHarness {
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    RecordPlatform host;
    PostGameFlow flow{progress, player, score, host};

    FlowHarness() {
        progress.stage = 1;
        progress.difficulty = 1;
        player.extra_lives = 2;
        player.bombs = 2;
    }
};

}

TEST(score_formulas_byte_semantics) {
    CHECK_EQ(stage_clear_bonus(1), 1000000);
    CHECK_EQ(stage_clear_bonus(6), 6000000);
    CHECK_EQ(all_clear_bonus(0, 3, 1), (1 + 3 * 4) * 250000);
    CHECK_EQ(all_clear_bonus(1, 3, 1), (1 + 3 * 3) * 1000000);
    CHECK_EQ(all_clear_bonus(2, 3, 1), 3 * 5000000 + 1 * 1500000);
    CHECK_EQ(all_clear_bonus(3, 3, 1), (1 + 3 * 5) * 2000000);
    CHECK_EQ(extra_clear_bonus(2, 3), (3 + 2 * 10) * 4000000);
    CHECK_EQ(add_bonus_capped(0, 1000000), 100000u);
    CHECK_EQ(add_bonus_capped(999999999, 1000000), 999999999u);
    CHECK_EQ(add_bonus_capped(999999900, 1000000), 999999999u);
    CHECK_EQ(add_bonus_capped(500000, -1000000), 400000u);
}

TEST(stage1_clear_requests_next_stage) {
    FlowHarness harness;
    harness.flow.on_stage_clear();
    CHECK_EQ(harness.host.requests.size(), 1u);
    CHECK_EQ(harness.host.requests[0], 0xc);
    CHECK_EQ(harness.progress.stage, 2);
    CHECK_EQ(harness.score.score, 100000);
    CHECK_EQ(harness.progress.stage_clear_bonus, 1000000);
    CHECK(harness.flow.state() == OverlayState::Monitor);
    CHECK(!harness.flow.frozen());
    CHECK_EQ(harness.progress.stage_cleared_bits, 1u << (1 + 1 * 8));
}

TEST(stage5_clear_caps_score) {
    FlowHarness harness;
    harness.progress.stage = 5;
    harness.score.score = 999999999 - 50000;
    harness.flow.on_stage_clear();
    CHECK_EQ(harness.score.score, 999999999);
    CHECK_EQ(harness.host.requests.size(), 1u);
    CHECK_EQ(harness.progress.stage, 6);
}

TEST(practice_clear_stage_chain_and_gates) {
    FlowHarness harness;
    harness.progress.practice_mode = 1;
    harness.progress.stage = 3;
    harness.flow.on_stage_clear();
    CHECK_EQ(harness.host.requests.size(), 1u);
    CHECK_EQ(harness.host.requests[0], 0xc);
    CHECK_EQ(harness.progress.stage, 4);
    CHECK_EQ(harness.score.score, 300000);
    CHECK_EQ(harness.progress.stage_cleared_bits, 0u);
    CHECK(!harness.flow.frozen());
}

TEST(spell_practice_skips_stage_bonus) {
    FlowHarness harness;
    harness.progress.run_mode_flags = 0x20;
    harness.flow.on_stage_clear();
    CHECK_EQ(harness.score.score, 0);
    CHECK(harness.flow.state() == OverlayState::BSpellVariant);
    CHECK(harness.flow.frozen());
}

TEST(stage6_all_clear_and_ending_sequence) {
    FlowHarness harness;
    harness.progress.stage = 6;
    harness.progress.difficulty = 1;
    harness.progress.ending_points = 600;
    harness.flow.on_stage_clear();
    CHECK_EQ(harness.score.score, 1400000);
    CHECK_EQ(harness.progress.stage_clear_bonus, 14000000);
    CHECK_EQ(harness.progress.good_endings, 1);
    CHECK(harness.host.requests.empty());
    CHECK(harness.flow.clear_sequence());
    for (u32 frame = 0; frame < 379; frame++)
        harness.flow.update();
    CHECK(harness.host.requests.empty());
    harness.flow.update();
    CHECK_EQ(harness.host.requests.size(), 1u);
    CHECK_EQ(harness.host.requests[0], 0xf);
    CHECK(!harness.flow.clear_sequence());
    FlowHarness lives_high;
    lives_high.progress.stage = 6;
    lives_high.player.extra_lives = 3;
    lives_high.player.bombs = 2;
    lives_high.flow.on_stage_clear();
    CHECK_EQ(lives_high.progress.good_endings, 0);
    FlowHarness capped;
    capped.progress.stage = 6;
    capped.player.extra_lives = 0;
    capped.player.bombs = 3;
    capped.flow.on_stage_clear();
    CHECK_EQ(capped.progress.good_endings, 3);
}

TEST(stage6_practice_ending_exits_to_settlement_a) {
    FlowHarness harness;
    harness.progress.stage = 6;
    harness.progress.practice_mode = 1;
    harness.flow.on_stage_clear();
    CHECK(harness.flow.clear_sequence());
    for (u32 frame = 0; frame < 380; frame++)
        harness.flow.update();
    CHECK(harness.flow.state() == OverlayState::PracticeEntry);
    CHECK(harness.host.requests.empty());
}

TEST(stage7_extra_clear_bonus) {
    FlowHarness harness;
    harness.progress.stage = 7;
    harness.flow.on_stage_clear();
    CHECK_EQ(harness.score.score, 9500000);
    CHECK(harness.flow.state() == OverlayState::BShow);
    CHECK(harness.flow.frozen());
    CHECK(harness.host.requests.empty());
}

TEST(game_over_entry_freezes) {
    FlowHarness harness;
    harness.flow.on_game_over();
    CHECK(harness.flow.state() == OverlayState::GameOverShow);
    CHECK(harness.flow.frozen());
    CHECK_EQ(harness.host.bgms.size(), 1u);
    CHECK(harness.host.requests.empty());
}

TEST(practice_end_forwards_to_platform_and_checks_error) {
    FlowHarness harness;
    CHECK(harness.flow.on_practice_end());
    CHECK_EQ(harness.host.practice_ends, 1);
    CHECK_EQ(int(harness.flow.state()), int(OverlayState::PracticeEntry));
    CHECK(harness.flow.frozen());
    CHECK(harness.flow.error.empty());

    harness.host.practice_end_result = false;
    CHECK(!harness.flow.on_practice_end());
    CHECK_EQ(harness.host.practice_ends, 2);
    CHECK(!harness.flow.error.empty());
}

TEST(stage6_clear_chain_runs_ending_scene_to_after_ending_request) {
    FlowHarness harness;
    harness.progress.stage = 6;
    harness.progress.difficulty = 1;
    harness.progress.character = 0;
    harness.flow.on_stage_clear();
    for (u32 frame = 0; frame < 380; frame++)
        harness.flow.update();
    CHECK_EQ(harness.host.requests.size(), 1u);
    CHECK_EQ(harness.host.requests[0], 0xf);

    struct Save final : EndingSaveBytes {
        std::array<u8, 13> bytes{};
        u8 read(i32 index) const override { return index >= 0 && index < 13 ? bytes[u32(index)] : u8(0); }
        void write(i32 index, u8 value) override {
            if (index >= 0 && index < 13) bytes[u32(index)] = value;
        }
    };
    struct Host final : EndingPlatform {
        Save* save;
        std::map<std::string, std::vector<u8>> assets;
        std::vector<i32> requests;
        bool read_asset(const std::string& name, std::vector<u8>& bytes, bool) override {
            const auto found = assets.find(name);
            if (found == assets.end()) return false;
            bytes = found->second;
            return true;
        }
        EndingSaveBytes* save_bytes() override { return save; }
        bool request_destination(i32 raw) override {
            requests.push_back(raw);
            return true;
        }
        bool queue_music(i32, i32, const std::string&) override { return true; }
        bool fade_out_music(i32) override { return true; }
        bool advance_sound() override { return true; }
        bool ending_page_line(i32, u32, const std::string&) override { return true; }
    };
    Save save;
    Host host;
    host.save = &save;
    std::vector<u8> message{1, 0, 0, 0, 0x0c, 0, 0, 0, 0x00, 0x01, 0, 0};
    const u8 wait[8] = {0, 0, 5, 4, 0x80, 0x8d, 0x5b, 0x00};
    const u8 end[4] = {1, 0, 0, 0};
    message.insert(message.end(), wait, wait + 8);
    message.insert(message.end(), end, end + 4);
    host.assets["e01.msg"] = std::move(message);
    EndingScene ending(harness.progress, host);
    CHECK(ending.initialize());
    CHECK_EQ(ending.ending_index(), 1);
    GameInput input;
    CHECK(ending.update(input));
    input.pressed = 0x1;
    CHECK(ending.update(input));
    CHECK(ending.update(input));
    CHECK(ending.finished());
    CHECK_EQ(host.requests.size(), 1u);
    CHECK_EQ(host.requests[0], 0x10);
}

int main() { return th13test::run_all("对局内覆盖层"); }
