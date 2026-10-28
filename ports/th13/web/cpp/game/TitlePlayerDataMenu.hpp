// PlayerData screen10, 00453170: four character columns, five rank choices.
// Original MenuCursor counts are exclusive (00415030/00473210). Initial rank
// is Normal(1), page0 is the ordinary ranking; Z advances through ten-card
// pages1..ceil(count/10). Up/down changes rank; left/right changes character.
// 00453db0 ordinary scores are separate from 00453b30 spell counters.
// TitlePlayerDataView owns original ANM roots and production text submission.
#pragma once

#include "TitleState.hpp"

#include <functional>
#include <array>
#include <string>
#include <vector>

namespace th13 {

inline constexpr i32 kTitleScreenPlayerData = 10;    // 0x44c570 case a
inline constexpr i32 kPlayerDataRecordSlots = 0x7f;  // DAT_004bbd30 长度
inline constexpr i32 kPlayerDataPageSize = 10;       // 00453b30 每页 10 行
inline constexpr i32 kPlayerDataDifficulties = 5;    // +0x108=5 exclusive, 0..4
inline constexpr i32 kPlayerDataCharacters = 4;      // +0x30=4 exclusive, 0..3

// 00453db0: ordinary ranking, independent of the spell-card DTO below.
struct TitlePlayerDataScore {
    i32 score = 0, stage = 0, continues = 0;
    std::string name;
    i64 timestamp = 0;
    float progress_percent = 0;
    bool available = false;
};

struct TitlePlayerDataSummary {
    std::array<TitlePlayerDataScore, 10> ranking{};
    i32 play_count = 0, total_frames = 0, clear_count = 0, no_continue_clear_count = 0;
    bool available = false;
    bool history_complete = false; // a migrated port save omitted ordinary data
};

// 单条 scoreth13.dat 记录（字段身份以 00453b30/00456b50 实读为准）。
struct TitlePlayerDataRecord {
    i32 slot = -1;        // No.%3d：全局槽 0..126
    std::string name;     // 0x164f0 + slot*0x9c
    i32 score = 0;        // legacy DTO alias: aggregate ordinary attempts, not points
    i32 number_a = 0;     // legacy DTO alias: aggregate ordinary captures
    i32 number_b = 0;     // legacy DTO alias: aggregate practice captures
    i32 normal_captures = 0, normal_attempts = 0;
    i32 practice_captures = 0, practice_attempts = 0;
    i32 practice_best_score = 0; // internal units, displayed with a trailing zero
    bool recorded = false; // aggregate ordinary attempts != 0 (00453b30)
    bool character_flagged = false;  // score+char*0x56dc+0xa00+slot*0x9c != 0
};

// 宿主服务：scoreth13.dat 记录表读取。默认 open=false / 空表 = 未接线，
// 不返回任何伪造记录；Application/FileStore 后续按真实存档实现。
struct TitlePlayerDataPlatform {
    virtual ~TitlePlayerDataPlatform() = default;
    // 载入 0x7f 槽记录表与 5 个角色块。false = 无存档/读取失败/未接线。
    virtual bool open_player_data() { return false; }
    virtual void close_player_data() {}
    // Original exclusive counts: five ranks0..4, four characters0..3.
    virtual i32 difficulty_count() const { return 0; }
    virtual i32 character_count() const { return 0; }
    // DAT_004bbd30 中等于 difficulty 的槽数（00412970 同义）。
    virtual i32 record_count(i32 difficulty) const {
        (void)difficulty;
        return 0;
    }
    // 第 ordinal 条匹配记录的字段；character 用于填 character_flagged。
    // false = 数据缺失，不得以零值冒充记录。
    virtual bool record(i32 difficulty, i32 character, i32 ordinal,
                        TitlePlayerDataRecord& out) const {
        (void)difficulty;
        (void)character;
        (void)ordinal;
        (void)out;
        return false;
    }
    virtual bool summary(i32 difficulty, i32 character, TitlePlayerDataSummary& out) const {
        (void)difficulty; (void)character; (void)out;
        return false;
    }
};

class TitlePlayerDataMenu {
    TitleState& state;
    TitlePlayerDataPlatform& host;
    i32 difficulty_ = 1;  // +0x100 rank0..4, starts at Normal
    i32 character_ = 0;   // +0x28 character0..3
    i32 page_ = 0;        // +0x1d8: 0=ordinary summary, 1..=10-card pages
    i32 frames_ = 0;      // case1 >6 / case3 >5 屏龄门
    bool opened_ = false;
    std::vector<TitlePlayerDataRecord> records_;  // 当前难度的匹配记录
    TitlePlayerDataSummary summary_;
    bool fail(const std::string& reason);
    void sound(i32 id);
    bool reload();  // 00453b30 列表重建（宿主读取 + 范围负控）

public:
    std::function<bool(i32)> play_sound;
    std::string error;
    TitlePlayerDataMenu(TitleState& state_, TitlePlayerDataPlatform& host_)
        : state(state_), host(host_) {}

    // case0：宿主开表 + 固定形状校验；成功进 substate1。
    bool open();
    // 每帧驱动（TitleFrame 屏值 10 分支接线）。pressed/repeated 与其他标题屏同义。
    bool update(u32 pressed, u32 repeated);
    // case2 X/Esc：音9 → substate3（6 帧后收面回 Main）。
    bool back();

    // Direct page controls share the same original page0/1.. cursor as Z.
    bool next_page();
    bool previous_page();
    bool set_page(i32 page);

    i32 difficulty() const { return difficulty_; }
    i32 character() const { return character_; }
    i32 page() const { return page_; }
    i32 phase() const { return state.substate; }
    i32 page_count() const;
    i32 record_count() const { return static_cast<i32>(records_.size()); }
    const std::vector<TitlePlayerDataRecord>& records() const { return records_; }
    std::vector<TitlePlayerDataRecord> visible_records() const;
    const TitlePlayerDataSummary& summary() const { return summary_; }
    bool summary_available() const { return summary_.available; }
    bool opened() const { return opened_; }
};

}  // namespace th13
