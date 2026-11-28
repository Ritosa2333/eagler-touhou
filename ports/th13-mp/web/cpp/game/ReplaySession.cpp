#include "ReplaySession.hpp"
#include "StageBattle.hpp"

namespace th13 {

namespace {
u32 rd32(const u8* at) { return u32(at[0]) | u32(at[1]) << 8 | u32(at[2]) << 16 | u32(at[3]) << 24; }
i32 as_i32(u32 raw) { return i32(raw); }
}

i32 ReplaySession::stage_index(const ReplayFile& file, i32 stage_number) {
    for (u32 index = 0; index < file.stages.size(); ++index)
        if (file.stages[index].number == stage_number) return i32(index);
    return -1;
}

bool ReplaySession::restore_player_start(const ReplayStage& stage, StageBattle& battle, std::string& error) {
    if (stage.header.size() < 0x70) {
        error = "Replay player-start header truncated";
        return false;
    }
    battle.restore_replay_player(as_i32(rd32(stage.header.data() + 0x0c)), as_i32(rd32(stage.header.data() + 0x10)),
                                 rd32(stage.header.data() + 0x6c) != 0);
    return true;
}

bool ReplaySession::restore_stage(const ReplayFile& file, u32 stage_index, const ReplayRestoreTarget& target,
                                  ReplayRestoreLog& log, std::string& error, ReplayRestorePhase phase) {
    if (!target.progress || !target.player || !target.score) {
        error = "Replay restore target missing";
        return false;
    }
    if (stage_index >= file.stages.size()) {
        error = "Replay restore stage index out of range";
        return false;
    }
    const ReplayStage& stage = file.stages[stage_index];
    if (stage.header.size() < 0x6C) {
        error = "Replay stage header truncated (need 0xC4 raw header)";
        return false;
    }
    const u16 stamp = u16(stage.header[2]) | u16(stage.header[3]) << 8;
    log.seed = stamp;
    log.stage_number = stage.number;
    if (target.game_random) target.game_random->reseed_restore(stamp);
    if (phase == ReplayRestorePhase::Transition && target.visual_random) target.visual_random->reseed(stamp);
    if (file.top_header.size() >= 0x68) {
        target.progress->character = as_i32(rd32(&file.top_header[0x5C]));
        target.progress->difficulty = as_i32(rd32(&file.top_header[0x64]));
    }
    for (u32 k = 0; k < 22; ++k)
        log.block[k] = as_i32(rd32(&stage.header[0x14 + 4 * k]));
    const i32* b = log.block;
    target.progress->character = b[0];
    target.score->score = b[2];
    target.progress->difficulty = b[3];
    target.progress->continues = b[4];
    target.progress->graze = b[6];
    target.progress->selected_spell_id = b[7];
    target.progress->max_point = b[9];
    target.progress->max_point_cap = b[11];
    target.player->power = b[12];
    target.player->max_power = b[13];
    target.player->power_step = b[14];
    target.player->extra_lives = b[15];
    target.player->life_pieces = b[16];
    target.progress->life_piece_threshold_index = b[17];
    target.player->bombs = b[18];
    target.player->bomb_pieces = b[19];
    target.progress->ending_points = b[20];
    const bool selected_spell = target.progress->selected_spell_id >= 0;
    if ((target.progress->run_mode_flags & 0x30u) != 0x20u) target.progress->selected_spell_id = -1;
    target.progress->run_mode_flags = (target.progress->run_mode_flags & ~0x30u) | (selected_spell ? 0x20u : 0u);
    return true;
}

}
