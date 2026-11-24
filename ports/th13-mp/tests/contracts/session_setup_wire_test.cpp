// 会话身份线格式契约：钉住房间选项在线上的现有编码。
// 线格式、版本常量或 GameplayAbi 任一处变化都会让对局双方在握手层互斥，
// 所以这些值只随明确的设计变更更新，不随重构漂移。期望值取自独立算式
//（multiplayer-options.test.mjs 持有同一身份的 JS 向量），不从此处实现反推。
#include "../harness/test_framework.hpp"

#include "multiplayer/SessionSetup.hpp"

#include <array>
#include <cstdint>
#include <vector>

namespace {

using th13::multiplayer::SessionSetup;
using Words = std::array<std::uint32_t, SessionSetup::Words>;

SessionSetup rollback_setup() {
    SessionSetup setup;
    setup.version = SessionSetup::Version;
    setup.mode = 2;
    setup.session_id = 0x0000000200000001ull;
    setup.player_count = 2;
    setup.local_player = 0;
    setup.difficulty = 2;
    setup.seed = 0x1234;
    setup.automatic = false;
    setup.selections = {1, 3, 0};
    setup.build = {0x11111111, 0x22222222, 0x33333333, 0x44444444};
    return setup;
}

SessionSetup delayed_challenge_setup() {
    SessionSetup setup;
    setup.version = SessionSetup::Version;
    setup.mode = 1;
    setup.session_id = 0xdeadbeefcafebabeull;
    setup.player_count = 3;
    setup.local_player = 2;
    setup.difficulty = 4;
    setup.seed = 65535;
    setup.requested_delay = setup.input_delay = 9;
    setup.prediction_reserve = 1;
    setup.automatic = false;
    setup.selections = {0, 0, 2};
    setup.build = {1, 0, 0, 0};
    setup.challenge = setup.keep_bullets_on_miss = true;
    return setup;
}

SessionSetup legacy_setup() {
    SessionSetup setup;
    setup.version = SessionSetup::LegacyVersion;
    setup.mode = 1;
    setup.session_id = 1;
    setup.local_player = 1;
    setup.selections = {2, 2, 0};
    setup.build = {0, 0, 0, 7};
    return setup;
}

// 整体拒绝：解码失败时调用方的会话身份必须原样保留。
bool rejects(const Words& words) {
    SessionSetup out = delayed_challenge_setup();
    const Words before = th13::multiplayer::EncodeSessionSetup(out);
    if (th13::multiplayer::DecodeSessionSetup(out, words.data(), words.size())) return false;
    return th13::multiplayer::EncodeSessionSetup(out) == before;
}

}  // namespace

TEST(constants) {
    CHECK_EQ(SessionSetup::Version, 2u);
    CHECK_EQ(SessionSetup::LegacyVersion, 1u);
    CHECK_EQ(SessionSetup::Words, std::size_t(22));
    CHECK_EQ(SessionSetup::GameplayVersion, 5u);
    CHECK_EQ(SessionSetup::RulesVersion, 3u);
    CHECK_EQ(SessionSetup::MaxRollbackFrames, 12u);
    CHECK_EQ(SessionSetup::MaxCharacters, 4u);
}

TEST(encode_rollback_two_players) {
    const Words expected{2, 2, 0, 2, 0x1234, 0x00000001, 0x00000002, 0, 1, 3, 0, 0, 0, 0,
                         2, 0, 2, 0x11111111, 0x22222222, 0x33333333, 0x44444444, 0};
    CHECK_EQ(th13::multiplayer::EncodeSessionSetup(rollback_setup()), expected);
}

TEST(encode_delayed_three_players_challenge) {
    const Words expected{2, 3, 2, 4, 65535, 0xcafebabe, 0xdeadbeef, 9, 0, 0, 2, 0, 0, 0,
                         1, 0, 1, 1, 0, 0, 0, 3};
    CHECK_EQ(th13::multiplayer::EncodeSessionSetup(delayed_challenge_setup()), expected);
}

TEST(encode_legacy_automatic) {
    const Words expected{1, 2, 1, 0, 1, 1, 0, 0, 2, 2, 0, 0, 0, 0,
                         1, 1, 2, 0, 0, 0, 7, 0};
    CHECK_EQ(th13::multiplayer::EncodeSessionSetup(legacy_setup()), expected);
}

TEST(decode_round_trips_every_field) {
    const Words words = th13::multiplayer::EncodeSessionSetup(delayed_challenge_setup());
    SessionSetup decoded;
    CHECK(th13::multiplayer::DecodeSessionSetup(decoded, words.data(), words.size()));
    CHECK(decoded.valid());
    CHECK_EQ(th13::multiplayer::EncodeSessionSetup(decoded), words);
    CHECK_EQ(decoded.version, SessionSetup::Version);
    CHECK_EQ(decoded.mode, 1u);
    CHECK_EQ(decoded.player_count, 3u);
    CHECK_EQ(decoded.local_player, 2u);
    CHECK_EQ(decoded.difficulty, 4u);
    CHECK_EQ(decoded.seed, 65535u);
    CHECK_EQ(decoded.session_id, 0xdeadbeefcafebabeull);
    CHECK_EQ(decoded.requested_delay, 9u);
    CHECK_EQ(decoded.input_delay, 9u);
    CHECK_EQ(decoded.prediction_reserve, 1u);
    CHECK(decoded.challenge);
    CHECK(decoded.keep_bullets_on_miss);
    CHECK(!decoded.automatic);
    CHECK_EQ(decoded.selections, (std::array<std::uint32_t, 3>{0, 0, 2}));
    CHECK_EQ(decoded.build, (std::array<std::uint32_t, 4>{1, 0, 0, 0}));
}

TEST(abi_matches_independent_reference) {
    CHECK_EQ(th13::multiplayer::GameplayAbi(rollback_setup()), 0xbe86d8fbu);
    CHECK_EQ(th13::multiplayer::GameplayAbi(delayed_challenge_setup()), 0x1a10e285u);
    CHECK_EQ(th13::multiplayer::GameplayAbi(legacy_setup()), 0x3c7a3485u);
}

TEST(rejects_wrong_word_count) {
    const Words words = th13::multiplayer::EncodeSessionSetup(rollback_setup());
    SessionSetup out;
    CHECK(!th13::multiplayer::DecodeSessionSetup(out, words.data(), words.size() - 1));
    CHECK(!th13::multiplayer::DecodeSessionSetup(out, words.data(), words.size() + 1));
    CHECK(!th13::multiplayer::DecodeSessionSetup(out, nullptr, words.size()));
}

TEST(rejects_out_of_range_fields) {
    const Words base = th13::multiplayer::EncodeSessionSetup(rollback_setup());
    std::vector<Words> invalid;
    const auto variant = [&](unsigned index, std::uint32_t value) {
        Words changed = base;
        changed[index] = value;
        invalid.push_back(changed);
    };
    variant(0, 3);      // 未知线格式版本
    variant(14, 3);     // 未知时序模式
    variant(15, 2);     // automatic 只能取 0/1
    variant(21, 2);     // Miss 不清屏必须伴随挑战模式
    variant(1, 1);      // 少于 2 席
    variant(1, 4);      // 多于 3 席
    variant(2, 2);      // 本地席位越界
    variant(3, 6);      // 难度越界
    variant(4, 65536);  // 种子越界
    variant(7, 10);     // 延迟越界
    variant(16, 0);     // 预测保留越界
    variant(16, 3);     // 预测保留越界
    variant(8, 4);      // 角色越界
    variant(10, 1);     // 未用席位必须为 0
    Words no_session = base;
    no_session[5] = no_session[6] = 0;
    invalid.push_back(no_session);
    Words no_build = base;
    no_build[17] = no_build[18] = no_build[19] = no_build[20] = 0;
    invalid.push_back(no_build);
    Words automatic_and_delay = base;
    automatic_and_delay[15] = automatic_and_delay[7] = 1;
    invalid.push_back(automatic_and_delay);
    Words legacy_rollback = base;
    legacy_rollback[0] = SessionSetup::LegacyVersion;
    invalid.push_back(legacy_rollback);
    for (const auto& words : invalid) CHECK(rejects(words));
}

TEST(mode_zero_requires_empty_timing_fields) {
    Words words = th13::multiplayer::EncodeSessionSetup(rollback_setup());
    words[14] = 0;
    SessionSetup decoded;
    CHECK(th13::multiplayer::DecodeSessionSetup(decoded, words.data(), words.size()));
    CHECK_EQ(decoded.mode, 0u);
    words[15] = 1;
    CHECK(rejects(words));
}

int main() { return th13test::run_all("session-setup-wire"); }
