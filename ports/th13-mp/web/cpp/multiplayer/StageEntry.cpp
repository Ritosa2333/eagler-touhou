// TH13 MP 关头延续快照实现（M9）。契约见 StageEntry.hpp。
#include "StageEntry.hpp"

#include "../game/StageBattle.hpp"

#include <cstring>

namespace th13::multiplayer {

namespace {
// 线布局（v2；v1 读取时以原共享下标迁移各席）：
//   version(4) stage(4) chapter_frame(4) seatCount(4)
//   shared: ending(4) goodEndings(4) pointItems(4) maxPoint(4) maxPointCap(4)
//           graze(4) lifePieceIndex(4) score(4) highScore(4)
//   trance: mode(4) owner(4) generation(4) taskFrame(4)
//   rng: gameSeed(2) gameIndex(4) visualSeed(2) visualIndex(4)
//   seats × seatCount: lives(4) lifePieces(4) bombs(4) bombPieces(4) power(4)
//                      maxPower(4) powerStep(4) deaths(4) modeFlags(4)
//                      character(4) ghost(4) lifePieceIndex(4)
//                      input: held(4) previous(4) repeated(4) pressed(4)
//                             released(4) longHeld(4) repeatAge(32*4)
//                             duration(32*4)
constexpr std::size_t kSeatBytes = 12 * 4 + 6 * 4 + 32 * 4 * 2;
// 头部：4(头) + 9×4(shared) + 4×4(trance) + 2+4+2+4(rng) = 80。
constexpr std::size_t kHeaderBytes = 4 * 4 + 9 * 4 + 4 * 4 + 2 + 4 + 2 + 4;

std::uint32_t Read32(const std::uint8_t* p) {
    return std::uint32_t(p[0]) | std::uint32_t(p[1]) << 8 |
           std::uint32_t(p[2]) << 16 | std::uint32_t(p[3]) << 24;
}
void Write32(std::vector<std::uint8_t>& out, std::uint32_t value) {
    out.push_back(std::uint8_t(value));
    out.push_back(std::uint8_t(value >> 8));
    out.push_back(std::uint8_t(value >> 16));
    out.push_back(std::uint8_t(value >> 24));
}
}  // namespace

std::vector<std::uint8_t> StageEntry::Encode() const {
    if (!valid()) return {};
    std::vector<std::uint8_t> out;
    out.reserve(kHeaderBytes + seat_count * kSeatBytes);
    Write32(out, Version);
    Write32(out, stage);
    Write32(out, chapter_frame);
    Write32(out, seat_count);
    Write32(out, std::uint32_t(ending_points));
    Write32(out, std::uint32_t(good_endings));
    Write32(out, std::uint32_t(point_items));
    Write32(out, std::uint32_t(max_point));
    Write32(out, std::uint32_t(max_point_cap));
    Write32(out, std::uint32_t(graze));
    Write32(out, std::uint32_t(life_piece_threshold_index));
    Write32(out, std::uint32_t(score));
    Write32(out, std::uint32_t(high_score));
    Write32(out, std::uint32_t(trance_mode));
    Write32(out, std::uint32_t(trance_owner_seat));
    Write32(out, trance_generation);
    Write32(out, std::uint32_t(trance_task_frame));
    out.push_back(std::uint8_t(game_seed));
    out.push_back(std::uint8_t(game_seed >> 8));
    Write32(out, game_index);
    out.push_back(std::uint8_t(visual_seed));
    out.push_back(std::uint8_t(visual_seed >> 8));
    Write32(out, visual_index);
    for (std::uint32_t seat = 0; seat < seat_count; ++seat) {
        const Seat& s = seats[seat];
        Write32(out, std::uint32_t(s.extra_lives));
        Write32(out, std::uint32_t(s.life_pieces));
        Write32(out, std::uint32_t(s.bombs));
        Write32(out, std::uint32_t(s.bomb_pieces));
        Write32(out, std::uint32_t(s.power));
        Write32(out, std::uint32_t(s.max_power));
        Write32(out, std::uint32_t(s.power_step));
        Write32(out, std::uint32_t(s.deaths));
        Write32(out, s.mode_flags);
        Write32(out, std::uint32_t(s.character));
        Write32(out, s.ghost ? 1u : 0u);
        Write32(out, std::uint32_t(s.life_piece_threshold_index));
        const GameInput& input = inputs[seat];
        Write32(out, input.held);
        Write32(out, input.previous);
        Write32(out, input.repeated);
        Write32(out, input.pressed);
        Write32(out, input.released);
        Write32(out, input.long_held);
        for (std::uint32_t bit = 0; bit < 32; ++bit) Write32(out, input.repeat_age[bit]);
        for (std::uint32_t bit = 0; bit < 32; ++bit) Write32(out, input.duration[bit]);
    }
    return out;
}

bool StageEntry::Decode(const std::uint8_t* bytes, std::size_t size, StageEntry& out) {
    if (!bytes || size < kHeaderBytes) return false;
    const auto version = Read32(bytes);
    if (version != Version && version != LegacyVersion) return false;
    StageEntry entry;
    entry.stage = Read32(bytes + 4);
    entry.chapter_frame = Read32(bytes + 8);
    const std::uint32_t seatCount = Read32(bytes + 12);
    if (seatCount < 1 || seatCount > MaxSeats) return false;
    const auto seat_bytes = version == Version ? kSeatBytes : kSeatBytes - 4;
    if (size != kHeaderBytes + seatCount * seat_bytes) return false;
    const std::uint8_t* p = bytes + 16;
    // shared 段 9×4（ending..highScore，Encode 同序）。
    entry.ending_points = std::int32_t(Read32(p));
    entry.good_endings = std::int32_t(Read32(p + 4));
    entry.point_items = std::int32_t(Read32(p + 8));
    entry.max_point = std::int32_t(Read32(p + 12));
    entry.max_point_cap = std::int32_t(Read32(p + 16));
    entry.graze = std::int32_t(Read32(p + 20));
    entry.life_piece_threshold_index = std::int32_t(Read32(p + 24));
    entry.score = std::int32_t(Read32(p + 28));
    entry.high_score = std::int32_t(Read32(p + 32));
    // trance 段 4×4。
    entry.trance_mode = std::int32_t(Read32(p + 36));
    entry.trance_owner_seat = std::int32_t(Read32(p + 40));
    entry.trance_generation = Read32(p + 44);
    entry.trance_task_frame = std::int32_t(Read32(p + 48));
    // RNG 段（p+52 起 = offset 68）：gameSeed(2) gameIndex(4) visualSeed(2) visualIndex(4)。
    entry.game_seed = std::uint16_t(p[52] | std::uint16_t(p[53]) << 8);
    entry.game_index = Read32(p + 54);
    entry.visual_seed = std::uint16_t(p[58] | std::uint16_t(p[59]) << 8);
    entry.visual_index = Read32(p + 60);
    p = bytes + kHeaderBytes;
    for (std::uint32_t seat = 0; seat < seatCount; ++seat) {
        Seat& s = entry.seats[seat];
        s.extra_lives = std::int32_t(Read32(p));
        s.life_pieces = std::int32_t(Read32(p + 4));
        s.bombs = std::int32_t(Read32(p + 8));
        s.bomb_pieces = std::int32_t(Read32(p + 12));
        s.power = std::int32_t(Read32(p + 16));
        s.max_power = std::int32_t(Read32(p + 20));
        s.power_step = std::int32_t(Read32(p + 24));
        s.deaths = std::int32_t(Read32(p + 28));
        s.mode_flags = Read32(p + 32);
        s.character = std::int32_t(Read32(p + 36));
        s.ghost = Read32(p + 40) != 0;
        s.life_piece_threshold_index = version == Version ? std::int32_t(Read32(p + 44))
                                                           : entry.life_piece_threshold_index;
        const auto* input_bytes = p + (version == Version ? 48 : 44);
        GameInput& input = entry.inputs[seat];
        input.held = Read32(input_bytes + 0);
        input.previous = Read32(input_bytes + 4);
        input.repeated = Read32(input_bytes + 8);
        input.pressed = Read32(input_bytes + 12);
        input.released = Read32(input_bytes + 16);
        input.long_held = Read32(input_bytes + 20);
        const std::uint8_t* q = input_bytes + 24;
        for (std::uint32_t bit = 0; bit < 32; ++bit)
            input.repeat_age[bit] = Read32(q + bit * 4);
        q += 32 * 4;
        for (std::uint32_t bit = 0; bit < 32; ++bit)
            input.duration[bit] = Read32(q + bit * 4);
        p += seat_bytes;
    }
    entry.seat_count = seatCount;
    if (!entry.valid()) return false;
    out = entry;
    return true;
}

void StageEntry::Capture(const StageBattle& battle, const SessionState& progress,
                         const HudScoreState& score, std::uint16_t game_seed,
                         std::uint32_t game_index, std::uint16_t visual_seed,
                         std::uint32_t visual_index) {
    stage = std::uint32_t(progress.stage);
    chapter_frame = std::uint32_t(progress.chapter_frame);
    ending_points = progress.ending_points;
    good_endings = progress.good_endings;
    point_items = progress.point_items;
    max_point = progress.max_point;
    max_point_cap = progress.max_point_cap;
    graze = progress.graze;
    life_piece_threshold_index = progress.life_piece_threshold_index;
    this->score = score.score;  // 成员（i32）← HudScoreState::score（参数名遮蔽）
    high_score = progress.high_score;
    seat_count = std::uint32_t(battle.seat_count >= 2 ? battle.seat_count : 1);
    for (std::uint32_t seat = 0; seat < seat_count && seat < MaxSeats; ++seat) {
        Seat& s = seats[seat];
        const HudPlayerState& hud = battle.seat_huds[seat];
        s.extra_lives = hud.extra_lives;
        s.life_pieces = hud.life_pieces;
        s.life_piece_threshold_index = hud.life_piece_threshold_index;
        s.bombs = hud.bombs;
        s.bomb_pieces = hud.bomb_pieces;
        s.power = hud.power;
        s.max_power = hud.max_power;
        s.power_step = hud.power_step;
        s.deaths = hud.deaths;
        s.mode_flags = hud.mode_flags;
        s.character = battle.seat_characters[size_t(seat)];
        s.ghost = battle.seats[size_t(seat)].ghost;
        inputs[seat] = battle.seat_inputs[size_t(seat)];
    }
    trance_mode = std::int32_t(battle.trance_mode);
    trance_owner_seat = battle.trance_owner_seat;
    trance_generation = battle.trance_generation;
    trance_task_frame = battle.trance_task_frame_;
    // 双 RNG 位相由调用方供值（参数名与成员同名——this 消歧）。
    this->game_seed = game_seed;
    this->game_index = game_index;
    this->visual_seed = visual_seed;
    this->visual_index = visual_index;
}

void StageEntry::Restore(StageBattle& battle, SessionState& progress,
                         HudScoreState& score, std::uint16_t& game_seed,
                         std::uint32_t& game_index, std::uint16_t& visual_seed,
                         std::uint32_t& visual_index) const {
    progress.stage = std::int32_t(stage);
    progress.chapter_frame = std::int32_t(chapter_frame);
    progress.ending_points = ending_points;
    progress.good_endings = good_endings;
    progress.point_items = point_items;
    progress.max_point = max_point;
    progress.max_point_cap = max_point_cap;
    progress.graze = graze;
    progress.life_piece_threshold_index = life_piece_threshold_index;
    progress.high_score = high_score;
    score.score = this->score;  // HudScoreState::score ← 成员（参数名遮蔽）
    RestoreBattleCarry(battle);
    game_seed = this->game_seed;
    game_index = this->game_index;
    visual_seed = this->visual_seed;
    visual_index = this->visual_index;
}

void StageEntry::PrepareStageClear(HudPlayerState& first_player, const StageBattle& previous) {
    for (std::uint32_t seat = 0; seat < seat_count && seat < MaxSeats; ++seat) {
        // The last life is deducted at death commit, before the 30-frame
        // animation marks the seat as a ghost. Clear revives both phases.
        // Shared rewards may increase stock during the final death animation.
        // Its committed death remains final until explicit rescue or clear.
        if (seats[seat].extra_lives < 0 || previous.seats[seat].final_death)
            seats[seat].ghost = true;
    }
    if (seats[0].ghost) {
        first_player.extra_lives = std::max(0, first_player.extra_lives);
        first_player.bombs = 2;
        first_player.bomb_pieces = 0;
        first_player.power = first_player.max_power / 2;
    }
}

void StageEntry::RestoreBattleCarry(StageBattle& battle) const {
    for (std::uint32_t seat = 0; seat < seat_count && seat < MaxSeats; ++seat) {
        const Seat& s = seats[seat];
        HudPlayerState& hud = battle.seat_huds[seat];
        hud.extra_lives = s.extra_lives;
        hud.life_pieces = s.life_pieces;
        hud.life_piece_threshold_index = s.life_piece_threshold_index;
        hud.bombs = s.bombs;
        hud.bomb_pieces = s.bomb_pieces;
        hud.power = s.power;
        hud.max_power = s.max_power;
        hud.power_step = s.power_step;
        hud.deaths = s.deaths;
        hud.mode_flags = s.mode_flags;
        battle.seat_characters[seat] = s.character;
        battle.seats[seat].ghost = s.ghost;
        battle.seats[seat].life_transfer_state = 0;
        battle.seats[seat].power_transfer_state = 0;
        battle.seat_inputs[seat] = inputs[seat];
    }
    battle.trance_mode = static_cast<StageBattle::TranceMode>(trance_mode);
    battle.trance_owner_seat = trance_owner_seat;
    battle.trance_generation = trance_generation;
    battle.trance_task_frame_ = trance_task_frame;
}

}  // namespace th13::multiplayer
