// TH13 MP 关头延续快照（M9 StageEntry）。
// 跨关推进（stage N → N+1）时的完整延续面：全席资源（残机/碎片/Bomb/
// Power/死亡数/碎片阈值下标）、共享会话态（灵界槽/最大得点/Graze/点道具/
// 好结局资格）、双 RNG 位相、灵界生命周期（mode/owner/generation/计时）、
// 关卡号与章节帧。ReplayArchive 的关头表按章节携带本编码——回放 seek 到
// 章节 C 时重建 C 的 StageEntry 态（不伪切当前帧号：从 C 的 firstFrame
// 起重放输入，WorldState 按关头重建后逐帧推进）。
// 编码为显式线格式（v2，兼容读取 v1）：无指针/无 padding/无容器序列化陷阱。
#pragma once

#include "../game/GameInput.hpp"
#include "../game/SessionState.hpp"

#include <array>
#include <cstdint>
#include <vector>

namespace th13 {
class StageBattle;  // game/StageBattle.hpp（StageEntry.cpp 内含）
}

namespace th13::multiplayer {

struct StageEntry {
    static constexpr std::uint32_t Version = 2, LegacyVersion = 1;
    static constexpr std::uint32_t MaxSeats = 3;

    std::uint32_t stage = 1;            // 关号（1..6/7）
    std::uint32_t chapter_frame = 0;    // 章节内帧（跨关延续通常 0）
    // 共享会话态（SessionState 的跨关延续子集——0x42b990 的 0xc 延续分支）。
    std::int32_t ending_points = 0;    // 灵界槽（跨关保留）
    std::int32_t good_endings = 0;
    std::int32_t point_items = 0;
    std::int32_t max_point = 0, max_point_cap = 0;
    std::int32_t graze = 0;
    std::int32_t life_piece_threshold_index = 0;
    std::int32_t score = 0;            // HudScoreState.score（团队一份）
    std::int32_t high_score = 0;
    // 全席资源（RULES R-X：席位资源独立；幽灵席也延续）。
    struct Seat {
        std::int32_t extra_lives = 2, life_pieces = 0;
        std::int32_t life_piece_threshold_index = 0;
        std::int32_t bombs = 2, bomb_pieces = 0;
        std::int32_t power = 100, max_power = 400, power_step = 100;
        std::int32_t deaths = 0;
        std::uint32_t mode_flags = 0;
        std::int32_t character = 0;
        bool ghost = false;
    };
    std::array<Seat, MaxSeats> seats{};
    std::uint32_t seat_count = 0;
    // 灵界生命周期（RULES R-T：mode/owner/generation 随关头延续——跨关
    // 不应处于活跃灵界，但 generation 单调性必须保留以防旧死亡路径）。
    std::int32_t trance_mode = 0;  // StageBattle::TranceMode 序数
    std::int32_t trance_owner_seat = 0;
    std::uint32_t trance_generation = 0;
    std::int32_t trance_task_frame = 0;
    // 双 RNG 位相（game=战斗流 / visual=ANM 流）。
    std::uint16_t game_seed = 0;
    std::uint32_t game_index = 0;
    std::uint16_t visual_seed = 0;
    std::uint32_t visual_index = 0;
    // 输入沿（跨关瞬间的沿状态机——重放从关头继续时 pressed/duration
    // 不携带跨关前的历史；lead-in neutral 语义与 SessionSetup 延迟窗一致）。
    std::array<GameInput, MaxSeats> inputs{};

    bool valid() const {
        return stage >= 1 && stage <= 7 && seat_count >= 1 && seat_count <= MaxSeats;
    }

    // 线格式编码/解码（v2，读取 v1；非法输入整体拒绝）。
    std::vector<std::uint8_t> Encode() const;
    static bool Decode(const std::uint8_t* bytes, std::size_t size, StageEntry& out);
    // 从权威面取值（battle 的共享态 + 席位资源 + RNG；调用方供 RNG 位相）。
    // battle：StageBattle（seats/seat_huds/seat_characters/trance 面）；
    // progress：SessionState（共享会话延续子集）；score：HudScoreState。
    void Capture(const th13::StageBattle& battle, const SessionState& progress,
                 const HudScoreState& score, std::uint16_t game_seed,
                 std::uint32_t game_index, std::uint16_t visual_seed,
                 std::uint32_t visual_index);
    // 写回权威面（seek 重建：全席资源/共享态/RNG 位相/灵界生命周期）。
    void Restore(th13::StageBattle& battle, SessionState& progress,
                 HudScoreState& score, std::uint16_t& game_seed, std::uint32_t& game_index,
                 std::uint16_t& visual_seed, std::uint32_t& visual_index) const;
    // Live clear activation builds the HUD before enabling the new seats.
    // Seed its first player with the revival resources, and include a final
    // death still in its animation among the seats revived after restoration.
    // Capture/Restore alone retain the raw chapter snapshot for replay seeks.
    void PrepareStageClear(HudPlayerState& first_player, const th13::StageBattle& previous);
    // Live activation keeps the new stage's clocks and constructor RNG draws.
    void RestoreBattleCarry(th13::StageBattle& battle) const;
};

}  // namespace th13::multiplayer
