#include "ReplayGlobals.hpp"

#include "GameGlobals.hpp"

#include <cstring>

namespace th16 {

namespace {

// 偏移相对 DAT_004a5790
template <class Globals, class Fn>
void each_field(Globals& g, Fn&& fn) {
    fn(0x000, &g.stage);
    fn(0x004, &g.stage_copy);
    fn(0x008, &g.spell_id);
    fn(0x00c, &g.game_frame);
    fn(0x010, &g.stage_frame);
    fn(0x014, &g.shot);
    fn(0x018, &g.shot_aux);
    fn(0x01c, &g.season);
    fn(0x020, &g.score);
    fn(0x024, &g.difficulty);
    fn(0x028, &g.continues);
    fn(0x02c, &g.rank);
    fn(0x030, &g.graze);
    fn(0x034, &g.graze_total);
    fn(0x038, &g.replay_slot);
    fn(0x03c, &g.miss_count);
    fn(0x044, &g.point_items);
    fn(0x048, &g.point_value);
    fn(0x04c, &g.point_value_initial);
    fn(0x050, &g.point_value_max);
    fn(0x054, &g.power);
    fn(0x058, &g.power_max);
    fn(0x05c, &g.power_per_level);
    fn(0x064, &g.lives);
    fn(0x068, &g.life_fragments);
    fn(0x06c, &g.extend_index);
    fn(0x070, &g.bombs);
    fn(0x074, &g.bomb_fragments);
    fn(0x078, &g.season_gauge);
    fn(0x07c, &g.season_gauge_max);
    for (int i = 0; i < 8; ++i) fn(0x080 + u32(i) * 4, &g.season_step[i]);
    for (int i = 0; i < 8; ++i) fn(0x0a8 + u32(i) * 4, &g.season_threshold[i]);
    fn(0x0d0, &g.collect_sum);
    fn(0x0d8, &g.collect_count);
    fn(0x0dc, &g.collect_indicator);
    fn(0x0e0, &g.collect_pos.x);
    fn(0x0e4, &g.collect_pos.y);
    fn(0x0e8, &g.collect_pos.z);
    fn(0x0ec, &g.bomb_fragment_items);
    fn(0x0f0, &g.enemy_bonus_pending);
    fn(0x0f4, &g.enemy_bonus);
}

}  // namespace

void store_replay_globals(const GameGlobals& globals, u8* out) {
    std::memset(out, 0, kReplayGlobalsSize);
    each_field(globals, [out](u32 offset, const auto* field) {
        std::memcpy(out + offset, field, sizeof(*field));
    });
}

void load_replay_globals(GameGlobals& globals, const u8* in) {
    each_field(globals, [in](u32 offset, auto* field) { std::memcpy(field, in + offset, sizeof(*field)); });
}

}  // namespace th16
