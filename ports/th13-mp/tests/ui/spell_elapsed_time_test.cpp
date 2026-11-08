#include "../harness/test_framework.hpp"
#include "game/SpellElapsedTime.hpp"
#include "game/ReplayRecorder.hpp"
using namespace th13;
namespace {
std::pair<i32,i32> decode(i32 v) {
    const i32 sec = ((v / 100) % 1000 + 0x3a6) % 1000;
    const i32 hh = (v % 100 + 0x43) % 100;
    return v / 100000 - 22 == sec + hh ? std::pair{sec,hh} : std::pair{999,99};
}
}
TEST(wall_time_quantizes_by_original_0167_not_sixty_hz) {
    CHECK(decode(encode_spell_elapsed_time(0)) == std::pair(0,0));
    CHECK(decode(encode_spell_elapsed_time(.008)) == std::pair(0,0));
    CHECK(decode(encode_spell_elapsed_time(.00835)) == std::pair(0,1));
    CHECK(decode(encode_spell_elapsed_time(.009)) == std::pair(0,1));
    CHECK(decode(encode_spell_elapsed_time(.025)) == std::pair(0,1));
    CHECK(decode(encode_spell_elapsed_time(.026)) == std::pair(0,3));
    CHECK(decode(encode_spell_elapsed_time(2.505)) == std::pair(2,50));
    CHECK(decode(encode_spell_elapsed_time(1000.01)).first == 999);
}
// 0x412760 是回放读取后的原版校验；0x413041 对失败槽写 0x6ad1584。
TEST(spell_time_replay_validation_matches_0x412760) {
    CHECK(spell_time_packed_valid(encode_spell_elapsed_time(0)));
    CHECK(spell_time_packed_valid(encode_spell_elapsed_time(12.345)));
    CHECK(spell_time_packed_valid(encode_spell_elapsed_time(999.99)));
    CHECK(!spell_time_packed_valid(0));            // 旧录像未写槽
    // 0x6ad1584 本身是合法的 999.99 编码（0x412760 通过）；0x413041 用它
    // 覆盖失败槽，绘制端照常显示 999.99。
    CHECK(spell_time_packed_valid(0x6ad1584));
    CHECK(decode(0x6ad1584) == std::pair(999,99));
    CHECK(!spell_time_packed_valid(-1));
}
TEST(recorded_spell_time_roundtrips_in_stage_header_without_touching_inputs) {
    ReplayRecorder recorder;
    SessionState state;
    state.stage = 1;
    HudPlayerState player;
    HudScoreState score;
    CHECK(recorder.start(ReplayRecorder::top_snapshot(state)));
    CHECK(recorder.begin_stage(ReplayRecorder::stage_snapshot(state, player, score, 7, 0, 51200, false)));
    const i32 packed = encode_spell_elapsed_time(12.345);
    CHECK(recorder.record_spell_time(0, packed));
    CHECK(recorder.record_spell_time(20, packed));
    CHECK(!recorder.record_spell_time(21, packed));
    CHECK(recorder.record({1,1,0}, 0));
    std::vector<u8> bytes;
    CHECK(recorder.encode(bytes, "TEST", 100));
    ReplayFile file;
    CHECK(file.decode(bytes.data(), bytes.size()));
    CHECK_EQ(file.stages.size(), size_t(1));
    if (!file.stages.empty()) {
        const auto& stage = file.stages[0];
        i32 first = 0, last = 0;
        std::memcpy(&first, stage.header.data()+0x70, 4);
        std::memcpy(&last, stage.header.data()+0xc0, 4);
        CHECK_EQ(first, packed);
        CHECK_EQ(last, packed);
        CHECK_EQ(stage.frame(0).state, 1u);
    }
}
int main() { return th13test::run_all("spell elapsed time"); }
