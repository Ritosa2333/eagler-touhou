// 对局内覆盖层 PostGameFlow 单测（S3 第一阶段）：0x42cd40 关终了主干
//（每关百万奖/换关请求/关号推进）、计分公式边界（点值制与 999,999,999 封顶）、
// 六面通关演出 180f/379f、Extra/练习/符卡练习/全灭入口分流。
// Ending 链（fix-ending-20261028）：0xf 请求 → EndingScene（0x416570）→
// 脚本 END → 0x10 请求（0x416610）——宿主侧 0x10 → 标题变体 3 属 sdl 层
// apply_destination，本套件验证 game 层 0xf→EndingScene→0x10 全链。
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
        progress.difficulty = 1;  // Normal
        player.extra_lives = 2;
        player.bombs = 2;
    }
};

}  // namespace

TEST(score_formulas_byte_semantics) {
    // 0x42abf0：bonus = 关号×1,000,000（点值 /10 入账）。
    CHECK_EQ(stage_clear_bonus(1), 1000000);
    CHECK_EQ(stage_clear_bonus(6), 6000000);
    // 0x42ce96-0x42ced0 四难度六面全通奖（残=3 灵=1 例）。
    CHECK_EQ(all_clear_bonus(0, 3, 1), (3 + 1 * 4) * 250000);
    CHECK_EQ(all_clear_bonus(1, 3, 1), (3 + 1 * 3) * 1000000);
    CHECK_EQ(all_clear_bonus(2, 3, 1), 1 * 5000000 + 3 * 1500000);
    CHECK_EQ(all_clear_bonus(3, 3, 1), (3 + 1 * 5) * 2000000);
    // 0x42cfb3：Extra (残+灵×10)×4,000,000。
    CHECK_EQ(extra_clear_bonus(2, 3), (2 + 3 * 10) * 4000000);
    // 点值制入账 + 封顶（0x3b9ac9ff，0x42cef3/0x42cfe7 比较语义）。
    CHECK_EQ(add_bonus_capped(0, 1000000), 100000u);
    CHECK_EQ(add_bonus_capped(999999999, 1000000), 999999999u);
    CHECK_EQ(add_bonus_capped(999999900, 1000000), 999999999u);  // +100000 → 超 → 钳
    CHECK_EQ(add_bonus_capped(500000, -1000000), 400000u);
}

TEST(stage1_clear_requests_next_stage) {
    FlowHarness harness;
    harness.flow.on_stage_clear();
    CHECK_EQ(harness.host.requests.size(), 1u);
    CHECK_EQ(harness.host.requests[0], 0xc);  // dcc24=0xc 无条件（0x42cd40:103）
    CHECK_EQ(harness.progress.stage, 2);      // be81c++（<7）
    CHECK_EQ(harness.score.score, 100000);    // 1,000,000/10 点值
    CHECK_EQ(harness.progress.stage_clear_bonus, 1000000);
    CHECK(harness.flow.state() == OverlayState::Monitor);  // 换关非覆盖层
    CHECK(!harness.flow.frozen());
    CHECK_EQ(harness.progress.stage_cleared_bits, 1u << (1 + 1 * 8));
}

TEST(stage5_clear_caps_score) {
    FlowHarness harness;
    harness.progress.stage = 5;
    harness.score.score = 999999999 - 50000;  // 5M/10=500000 入账必超顶
    harness.flow.on_stage_clear();
    CHECK_EQ(harness.score.score, 999999999);
    CHECK_EQ(harness.host.requests.size(), 1u);
    CHECK_EQ(harness.progress.stage, 6);
}

TEST(practice_clear_stage_chain_and_gates) {
    // 0x42cd40 else 支（关 1-5）：dcc24=0xc 无条件——练习也换关；练习门只拦
    // score 块的 0x7fc 关卡通过位（0x42cd40:97-99）。
    FlowHarness harness;
    harness.progress.practice_mode = 1;  // GameInf+0x74
    harness.progress.stage = 3;
    harness.flow.on_stage_clear();
    CHECK_EQ(harness.host.requests.size(), 1u);
    CHECK_EQ(harness.host.requests[0], 0xc);
    CHECK_EQ(harness.progress.stage, 4);
    CHECK_EQ(harness.score.score, 300000);     // 0x42abf0 只被符卡练习门拦
    CHECK_EQ(harness.progress.stage_cleared_bits, 0u);  // 练习不记通过位
    CHECK(!harness.flow.frozen());             // 换关路径不进覆盖层
}

TEST(spell_practice_skips_stage_bonus) {
    FlowHarness harness;
    harness.progress.run_mode_flags = 0x20;  // be830&0x30 == 0x20（mode2）
    harness.flow.on_stage_clear();
    CHECK_EQ(harness.score.score, 0);  // 0x42abf0 被 &0x30==0x20 门跳过
    CHECK(harness.flow.state() == OverlayState::BSpellVariant);  // 0x43f720→0x1e
    CHECK(harness.flow.frozen());
}

TEST(stage6_all_clear_and_ending_sequence) {
    FlowHarness harness;
    harness.progress.stage = 6;
    harness.progress.difficulty = 1;  // Normal：(残+灵×3)×1,000,000
    harness.progress.ending_points = 600;  // >599 → 好结局资格 +1
    harness.flow.on_stage_clear();
    // 6M（每关奖）+ (2+2*3)*1M=8M（六面全通）= 14M → 点值 1,400,000。
    CHECK_EQ(harness.score.score, 1400000);
    CHECK_EQ(harness.progress.stage_clear_bonus, 14000000);
    CHECK_EQ(harness.progress.good_endings, 1);
    CHECK(harness.host.requests.empty());      // 0x4000 演出期间无请求
    CHECK(harness.flow.clear_sequence());
    // 180f 淡出 / >379f 出口（0x42c550:20-57）；覆盖层冻结会挂起演出。
    for (u32 frame = 0; frame < 379; frame++) harness.flow.update();
    CHECK(harness.host.requests.empty());
    harness.flow.update();  // 第 380 帧
    CHECK_EQ(harness.host.requests.size(), 1u);
    CHECK_EQ(harness.host.requests[0], 0xf);  // 请求结局
    CHECK(!harness.flow.clear_sequence());
    // 残>=3 时资格计数置 3（0x42cd40:36-38）。
    FlowHarness capped;
    capped.progress.stage = 6;
    capped.player.extra_lives = 3;
    capped.flow.on_stage_clear();
    CHECK_EQ(capped.progress.good_endings, 3);
}

TEST(stage6_practice_ending_exits_to_settlement_a) {
    FlowHarness harness;
    harness.progress.stage = 6;
    harness.progress.practice_mode = 1;
    harness.flow.on_stage_clear();
    CHECK(harness.flow.clear_sequence());  // 练习六面同样走 0x4000 演出
    for (u32 frame = 0; frame < 380; frame++) harness.flow.update();
    CHECK(harness.flow.state() == OverlayState::PracticeEntry);  // 0x43e5a0
    CHECK(harness.host.requests.empty());
}

TEST(stage7_extra_clear_bonus) {
    FlowHarness harness;
    harness.progress.stage = 7;  // be81c=7（Extra 关号；be830 保持 mode0）
    harness.flow.on_stage_clear();
    // 7M（每关奖）+ (2+2*10)*4M=88M（Extra 奖）= 95M → 点值 9,500,000。
    CHECK_EQ(harness.score.score, 9500000);
    CHECK(harness.flow.state() == OverlayState::BShow);  // 0x43f720 mode0→0x17
    CHECK(harness.flow.frozen());
    CHECK(harness.host.requests.empty());
}

TEST(game_over_entry_freezes) {
    FlowHarness harness;
    harness.flow.on_game_over();  // 0x43f570（mode0 → 态 0xe）
    CHECK(harness.flow.state() == OverlayState::GameOverShow);
    CHECK(harness.flow.frozen());
    CHECK_EQ(harness.host.bgms.size(), 1u);  // BGM th128_08（音频 TODO 记号）
    CHECK(harness.host.requests.empty());
}

TEST(practice_end_forwards_to_platform_and_checks_error) {
    FlowHarness harness;
    // 成功路径
    CHECK(harness.flow.on_practice_end());
    CHECK_EQ(harness.host.practice_ends, 1);
    CHECK_EQ(int(harness.flow.state()), int(OverlayState::PracticeEntry));
    CHECK(harness.flow.frozen());
    CHECK(harness.flow.error.empty());

    // 失败路径：宿主返回 false 时不吞没，记录 error 并返回 false
    harness.host.practice_end_result = false;
    CHECK(!harness.flow.on_practice_end());
    CHECK_EQ(harness.host.practice_ends, 2);
    CHECK(!harness.flow.error.empty());
}

TEST(stage6_clear_chain_runs_ending_scene_to_after_ending_request) {
    // 0x42c550 0x4000 出口（0xf）→ 0x416570 Ending 场景 → 脚本 END →
    // 0x416610 置 0x10（宿主 apply_destination 0x10 case → 拆卸 + 标题变体 3）。
    FlowHarness harness;
    harness.progress.stage = 6;
    harness.progress.difficulty = 1;
    harness.progress.character = 0;
    harness.flow.on_stage_clear();
    for (u32 frame = 0; frame < 380; frame++) harness.flow.update();
    CHECK_EQ(harness.host.requests.size(), 1u);
    CHECK_EQ(harness.host.requests[0], 0xf);

    // Ending 侧（同一 SessionState）：结局索引 1（e01，char0 无续关）→ END → 0x10。
    struct Save final : EndingSaveBytes {
        std::array<u8, 13> bytes{};
        u8 read(i32 index) const override {
            return index >= 0 && index < 13 ? bytes[u32(index)] : u8(0);
        }
        void write(i32 index, u8 value) override {
            if (index >= 0 && index < 13) bytes[u32(index)] = value;
        }
    };
    struct Host final : EndingPlatform {
        Save* save;
        std::map<std::string, std::vector<u8>> assets;
        std::vector<i32> requests;
        bool read_asset(const std::string& name, std::vector<u8>& bytes,
                        bool) override {
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
    // e01 式短脚本：op5 等待（按下放行）→ END。
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
    CHECK(ending.update(input));   // 行 0：op5 进入等待
    input.pressed = 0x1;           // Z 按下放行
    CHECK(ending.update(input));   // op5 放行 → 行推进到 1
    CHECK(ending.update(input));   // 行 1：END
    CHECK(ending.finished());
    CHECK_EQ(host.requests.size(), 1u);
    CHECK_EQ(host.requests[0], 0x10);  // AfterEnding（拆卸 → 标题变体 3）
}

int main() { return th13test::run_all("对局内覆盖层"); }
