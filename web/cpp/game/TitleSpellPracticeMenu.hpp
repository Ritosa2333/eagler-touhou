// 符卡练习三级菜单（原版标题 supervisor 0x44c570 case 0x11/0x12/0x13 →
// 0x4560a0/0x456650/0x4576c0）：
//   stage（1..6 与 Extra）→ spell 槽（0x456650 local_24 = 每 stage 槽数
//   {4,4,4,4,5,7,13}）→ 难度变体（5 槽；DAT_004be7d4 = 选中 spell id，
//   DAT_004bbd30[id] = 原版 rank）。左/右在 0x12/0x13 切换角色（0x4564c0），
//   0x4576c0 启动拍写 DAT_004be81c = stage+1、DAT_004be7b8 = character。
// 本类只做只读/交互状态机与选择输出；数据、解锁（0x456af0）与真实启动路由
// 全部由宿主提供。缺省 stage_count()==0 / unlock=false / spell_id=-1，
// 显式失败或空列表，不制造卡。
#pragma once

#include "Types.hpp"

#include <string>
#include <vector>

namespace th13 {

inline constexpr i32 kSpellPracticeStages = 7;
inline constexpr i32 kSpellPracticeSlots[kSpellPracticeStages] = {4, 4, 4, 4, 5, 7, 13};
inline constexpr i32 kSpellPracticeDifficulties = 5;
inline constexpr i32 kSpellPracticeCharacters = 4;

struct SpellPracticeRecord {
    i32 captures[2] = {}, attempts[2] = {};
    i32 best_score = 0;
    bool encountered = false;
};

// stage 参数均为原版 1 基编号（1..6，Extra=7）。slot 为 stage 内 0 基槽位；
// difficulty 为 0..4 的变体槽（原版 +0x5d24 数组下标）。
struct TitleSpellPracticePlatform {
    virtual ~TitleSpellPracticePlatform() = default;
    virtual bool spell_record(i32, i32, SpellPracticeRecord&) const { return false; }
    virtual i32 stage_count() const { return 0; }
    virtual bool stage_available(i32 /*stage*/) const { return false; }
    virtual std::string stage_name(i32 /*stage*/) const { return {}; }
    virtual i32 spell_count(i32 /*stage*/) const { return 0; }
    // 0x456af0：任意变体已记录（score 数据 0x16578 + id*0x9c）才可确认。
    virtual bool spell_unlocked(i32 /*stage*/, i32 /*slot*/) const { return false; }
    virtual std::string spell_name(i32 /*stage*/, i32 /*slot*/) const { return {}; }
    // DAT_004be7d4；-1 = 该难度槽无此卡。
    virtual i32 spell_id(i32 /*stage*/, i32 /*slot*/, i32 /*difficulty*/) const {
        return -1;
    }
    // DAT_004bbd30[spell_id]；-1 = 无 rank，禁止启动（不推断难度）。
    virtual i32 spell_rank(i32 /*stage*/, i32 /*slot*/, i32 /*difficulty*/) const {
        return -1;
    }
    // 00456b50 gates each variant separately after the family gate 00456af0.
    virtual bool spell_variant_available(i32 stage, i32 slot, i32 /*difficulty*/) const {
        return spell_unlocked(stage, slot);
    }
    virtual std::string spell_variant_name(i32 /*stage*/, i32 /*slot*/,
                                           i32 /*difficulty*/) const { return {}; }
    virtual i32 character_count() const { return 0; }
    virtual std::string character_name(i32 /*character*/) const { return {}; }
};

struct SpellPracticeStageRow {
    i32 index = -1;
    i32 stage = -1;
    bool available = false;
    std::string name;
};

struct SpellPracticeSpellRow {
    i32 slot = -1;
    bool unlocked = false;
    std::string name;
};

struct SpellPracticeDifficultyRow {
    i32 difficulty = -1;
    i32 spell_id = -1;
    i32 rank = -1;
    bool available = false;
    std::string name;
};

enum class SpellPracticeLevel : i32 { Stage = 0, Spell = 1, Difficulty = 2 };
enum class SpellPracticeAction : i32 { None = 0, Cancel = 1, Launch = 2 };

struct SpellPracticeInput {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool confirm = false;
    bool cancel = false;
};

struct SpellPracticeCommand {
    SpellPracticeAction action = SpellPracticeAction::None;
    i32 stage = -1;
    i32 spell_slot = -1;
    i32 difficulty = -1;
    i32 spell_id = -1;
    i32 rank = -1;
    i32 character = -1;
};

class TitleSpellPracticeMenu {
public:
    explicit TitleSpellPracticeMenu(TitleSpellPracticePlatform& platform,
                                    i32 initial_character = 0);

    // 单帧输入；返回 true 表示产生 Terminal 事件（Cancel/Launch）。
    bool update(const SpellPracticeInput& input);

    SpellPracticeLevel level() const noexcept { return level_; }
    i32 cursor() const noexcept { return cursor_; }
    i32 cursor_count() const noexcept;
    i32 character() const noexcept { return character_; }
    i32 selected_stage() const noexcept { return selected_stage_; }
    i32 selected_slot() const noexcept { return selected_slot_; }

    const std::vector<SpellPracticeStageRow>& stage_rows() const noexcept {
        return stage_rows_;
    }
    const std::vector<SpellPracticeSpellRow>& spell_rows() const noexcept {
        return spell_rows_;
    }
    const std::vector<SpellPracticeDifficultyRow>& difficulty_rows() const noexcept {
        return difficulty_rows_;
    }

    const SpellPracticeCommand& command() const noexcept { return command_; }
    void clear_command() noexcept { command_ = {}; }

    std::string error;
    std::string blocked;  // 最近一次被禁用原因（锁定卡/不可用变体等）

private:
    TitleSpellPracticePlatform& platform_;
    SpellPracticeLevel level_ = SpellPracticeLevel::Stage;
    i32 cursor_ = 0;
    i32 character_ = 0;
    i32 selected_stage_ = -1;
    i32 selected_slot_ = -1;
    std::vector<SpellPracticeStageRow> stage_rows_;
    std::vector<SpellPracticeSpellRow> spell_rows_;
    std::vector<SpellPracticeDifficultyRow> difficulty_rows_;
    SpellPracticeCommand command_;

    void rebuild_stage();
    void rebuild_spells();
    void rebuild_difficulties();
    void move_cursor(i32 delta);
    bool cycle_character(i32 delta);
    bool character_ready() const;
    bool confirm();
    bool cancel();
    void emit(SpellPracticeAction action);
};

}  // namespace th13
