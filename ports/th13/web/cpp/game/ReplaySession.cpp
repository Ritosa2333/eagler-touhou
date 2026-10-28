#include "ReplaySession.hpp"
#include "StageBattle.hpp"

namespace th13 {

namespace {
u32 rd32(const u8* at) {
    return u32(at[0]) | u32(at[1]) << 8 | u32(at[2]) << 16 | u32(at[3]) << 24;
}
i32 as_i32(u32 raw) { return i32(raw); }
}  // namespace

i32 ReplaySession::stage_index(const ReplayFile& file, i32 stage_number) {
    for (u32 index = 0; index < file.stages.size(); ++index)
        if (file.stages[index].number == stage_number) return i32(index);
    return -1;
}

bool ReplaySession::restore_player_start(const ReplayStage& stage,StageBattle& battle,std::string& error) {
    if(stage.header.size()<0x70){error="Replay player-start header truncated";return false;}
    // 448ec0 uses header+0c/+10 as signed1/128 coordinates, +6c as focus.
    battle.restore_replay_player(as_i32(rd32(stage.header.data()+0x0c)),
        as_i32(rd32(stage.header.data()+0x10)),rd32(stage.header.data()+0x6c)!=0);
    return true;
}

bool ReplaySession::restore_stage(const ReplayFile& file, u32 stage_index,
                                  const ReplayRestoreTarget& target, ReplayRestoreLog& log,
                                  std::string& error) {
    if (!target.progress || !target.player || !target.score) {
        error = "Replay restore target missing";
        return false;
    }
    if (stage_index >= file.stages.size()) {
        error = "Replay restore stage index out of range";
        return false;
    }
    const ReplayStage& stage = file.stages[stage_index];
    // 0x449070 装备前提：完整 0xC4 stage 头（replay_test stage_header_preserved_raw
    // 已锁解码器不丢头）。
    if (stage.header.size() < 0x6C) {
        error = "Replay stage header truncated (need 0xC4 raw header)";
        return false;
    }
    // ---- 种子恢复（0x449070:40-42；先于资源块，与原版同序）----
    // DAT_004dc324 与 DAT_004dc31c 同置 *(u16*)(iVar2+2)；仅 0x4dc324 的调用
    // 计数 _DAT_004dc328 清零（reseed_restore），0x4dc31c 计数原版不清
    //（reseed 只写种子）。
    const u16 stamp = u16(stage.header[2]) | u16(stage.header[3]) << 8;
    log.seed = stamp;
    log.stage_number = stage.number;
    if (target.game_random) target.game_random->reseed_restore(stamp);
    if (target.visual_random) target.visual_random->reseed(stamp);
    // ---- 顶层头 +0x5C/+0x60/+0x64（0x447900:197-199；先写，随后被覆盖）----
    if (file.top_header.size() >= 0x68) {
        target.progress->character = as_i32(rd32(&file.top_header[0x5C]));
        // +0x60 机体/子角色：port 无独立字段（th13 副机体不换 SHT），仅记录。
        target.progress->difficulty = as_i32(rd32(&file.top_header[0x64]));
    }
    // ---- stage 头 +0x14 起 22 dword（0x449070:43-49 连续复制，覆盖前写）----
    for (u32 k = 0; k < 22; ++k)
        log.block[k] = as_i32(rd32(&stage.header[0x14 + 4 * k]));
    const i32* b = log.block;
    // 头内偏移 = 0x14 + 4k；全局地址 = 0x4BE7B8 + (头内偏移 − 0x14)。
    target.progress->character = b[0];                    // 0x4BE7B8 +0x14
    /* b[1] +0x18 0x4BE7BC 机体/子角色 —— port 无归属，见 kUnmappedOffsets */
    target.score->score = b[2];                           // 0x4BE7C0 +0x1C 点值制分
    target.progress->difficulty = b[3];                   // 0x4BE7C4 +0x20
    target.progress->continues = b[4];                    // 0x4BE7C8 +0x24 续关计数
    /* b[5] +0x28 0x4BE7CC 未定名 —— 无归属 */
    target.progress->graze = b[6];                        // 0x4BE7D0 +0x2C
    target.progress->selected_spell_id = b[7];            // 0x4BE7D4 +0x30
    /* b[8] +0x34 0x4BE7D8 未定名计数 —— 无归属 */
    target.progress->max_point = b[9];                    // 0x4BE7DC +0x38 当前最大得点
    /* b[10] +0x3C 0x4BE7E0 最大得点基准 —— port 无独立字段，无归属 */
    target.progress->max_point_cap = b[11];               // 0x4BE7E4 +0x40 上限
    target.player->power = b[12];                         // 0x4BE7E8 +0x44 功率
    target.player->max_power = b[13];                     // 0x4BE7EC +0x48 功率上限
    target.player->power_step = b[14];                    // 0x4BE7F0 +0x4C 功率步长
    target.player->extra_lives = b[15];                   // 0x4BE7F4 +0x50 残机
    target.player->life_pieces = b[16];                   // 0x4BE7F8 +0x54 残机碎片
    target.progress->life_piece_threshold_index = b[17];  // 0x4BE7FC +0x58 阈值下标
    target.player->bombs = b[18];                         // 0x4BE800 +0x5C 灵击
    target.player->bomb_pieces = b[19];                   // 0x4BE804 +0x60 灵击碎片
    target.progress->ending_points = b[20];               // 0x4BE808 +0x64 灵界值
    /* b[21] +0x68 0x4BE80C 灵界 UI 辅助（语义未定名）—— 无归属 */
    // 00447900: normalize the mode from the copied stage value, but retain
    // a selected card only when the header selected Spell Practice first.
    const bool selected_spell = target.progress->selected_spell_id >= 0;
    if ((target.progress->run_mode_flags & 0x30u) != 0x20u)
        target.progress->selected_spell_id = -1;
    target.progress->run_mode_flags = (target.progress->run_mode_flags & ~0x30u) |
                                     (selected_spell ? 0x20u : 0u);
    return true;
}

}  // namespace th13
