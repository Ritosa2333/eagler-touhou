// 符卡练习三级菜单回归：0x44c570 case 0x11/0x12/0x13 →
// 0x4560a0/0x456650/0x4576c0。合成宿主数据只验证状态/禁用/输出，不验证画面。
#include "../harness/test_framework.hpp"
#include "game/TitleSpellPracticeMenu.hpp"

#include <string>

using namespace th13;

namespace {
struct SpellHost final : TitleSpellPracticePlatform {
    i32 stages = 7;
    i32 characters = 4;
    bool overcount = false;
    bool stage1_ok = true;

    i32 stage_count() const override { return stages; }
    bool stage_available(i32 stage) const override {
        return stage >= 1 && stage <= stages && (stage != 1 || stage1_ok);
    }
    std::string stage_name(i32 stage) const override {
        return "stage " + std::to_string(stage);
    }
    i32 spell_count(i32 stage) const override {
        if (overcount) return 99;
        return stage >= 1 && stage <= stages ? kSpellPracticeSlots[stage - 1] : 0;
    }
    bool spell_unlocked(i32 stage, i32 slot) const override {
        return stage == 1 && slot == 0;
    }
    std::string spell_name(i32 stage, i32 slot) const override {
        return "spell " + std::to_string(stage) + "-" + std::to_string(slot);
    }
    i32 spell_id(i32 stage, i32 slot, i32 difficulty) const override {
        if (stage == 1 && slot == 0 && difficulty == 3) return -1;  // 变体缺失
        return stage * 100 + slot * 10 + difficulty;
    }
    i32 spell_rank(i32 stage, i32 slot, i32 difficulty) const override {
        (void)stage; (void)slot;
        return difficulty;
    }
    i32 character_count() const override { return characters; }
    std::string character_name(i32 character) const override {
        return "char " + std::to_string(character);
    }
};

SpellPracticeInput confirm_key() {
    SpellPracticeInput input;
    input.confirm = true;
    return input;
}
SpellPracticeInput cancel_key() {
    SpellPracticeInput input;
    input.cancel = true;
    return input;
}
SpellPracticeInput down_key() {
    SpellPracticeInput input;
    input.down = true;
    return input;
}
SpellPracticeInput right_key() {
    SpellPracticeInput input;
    input.right = true;
    return input;
}
SpellPracticeInput left_key() {
    SpellPracticeInput input;
    input.left = true;
    return input;
}
}  // namespace

TEST(empty_catalog_is_explicit_failure_not_invented_cards) {
    SpellHost host;
    host.stages = 0;
    TitleSpellPracticeMenu menu(host);
    CHECK(menu.stage_rows().empty());
    CHECK(!menu.error.empty());
    CHECK(!menu.update(confirm_key()));
    CHECK_EQ(i32(menu.command().action), i32(SpellPracticeAction::None));
    CHECK(!menu.blocked.empty());
}

TEST(stage_spell_difficulty_confirm_emits_original_launch_fields) {
    SpellHost host;
    TitleSpellPracticeMenu menu(host);
    CHECK_EQ(i32(menu.stage_rows().size()), 7);
    CHECK_EQ(menu.stage_rows()[0].stage, 1);
    CHECK_EQ(menu.stage_rows()[6].stage, 7);
    CHECK(menu.stage_rows()[0].available);

    CHECK(!menu.update(confirm_key()));  // stage 1
    CHECK_EQ(i32(menu.level()), i32(SpellPracticeLevel::Spell));
    CHECK_EQ(i32(menu.spell_rows().size()), 4);
    CHECK(menu.spell_rows()[0].unlocked);
    CHECK(!menu.spell_rows()[1].unlocked);

    CHECK(!menu.update(confirm_key()));  // unlocked spell 0
    CHECK_EQ(i32(menu.level()), i32(SpellPracticeLevel::Difficulty));
    CHECK_EQ(i32(menu.difficulty_rows().size()), 5);
    CHECK(menu.difficulty_rows()[0].available);
    CHECK(!menu.difficulty_rows()[3].available);  // spell_id=-1

    CHECK(menu.update(confirm_key()));
    CHECK_EQ(i32(menu.command().action), i32(SpellPracticeAction::Launch));
    CHECK_EQ(menu.command().stage, 1);          // DAT_004be81c = index+1
    CHECK_EQ(menu.command().spell_slot, 0);
    CHECK_EQ(menu.command().difficulty, 0);
    CHECK_EQ(menu.command().spell_id, 100);     // DAT_004be7d4
    CHECK_EQ(menu.command().rank, 0);           // DAT_004bbd30[id]
    CHECK_EQ(menu.command().character, 0);
}

TEST(locked_card_and_missing_variant_are_disabled_but_navigable) {
    SpellHost host;
    TitleSpellPracticeMenu menu(host);
    CHECK(!menu.update(confirm_key()));  // enter spell list
    CHECK(!menu.update(down_key()));
    CHECK_EQ(menu.cursor(), 1);
    CHECK(!menu.update(confirm_key()));  // locked: 0x456af0 false
    CHECK_EQ(i32(menu.level()), i32(SpellPracticeLevel::Spell));
    CHECK(!menu.blocked.empty());

    menu.clear_command();
    SpellPracticeInput up;
    up.up = true;
    CHECK(!menu.update(up));  // slot 1 -> slot 0
    CHECK_EQ(menu.cursor(), 0);
    // 难度层：004576c0 case0 对不可用行（id<0，spell_id=-1 变体缺失）经 004417c0
    // 加入游标禁用表，上下移动经 00473210 跳过禁用值；光标不停留在锁定行。
    CHECK(!menu.update(confirm_key()));  // enter difficulty; cursor skips to row 0 (Easy usable)
    CHECK(!menu.update(down_key()));     // 0 -> 1
    CHECK(!menu.update(down_key()));     // 1 -> 2
    CHECK(!menu.update(down_key()));     // 2 -> row 3 (id<0 禁用) 跳到 4
    CHECK_EQ(menu.cursor(), 4);
    // 004576c0 case2 确认无可用性闸门：光标所在可用行直接发射。
    CHECK(menu.update(confirm_key()));
    CHECK_EQ(i32(menu.level()), i32(SpellPracticeLevel::Difficulty));
    CHECK_EQ(i32(menu.command().action), i32(SpellPracticeAction::Launch));
    CHECK_EQ(menu.command().difficulty, 4);
    CHECK_EQ(menu.command().spell_id, 104);  // stage1*100 + slot0*10 + difficulty4
    CHECK_EQ(menu.command().rank, 4);
}

TEST(cancel_pops_levels_and_stage_cancel_emits_cancel) {
    SpellHost host;
    TitleSpellPracticeMenu menu(host);
    CHECK(!menu.update(confirm_key()));
    CHECK(!menu.update(confirm_key()));
    CHECK_EQ(i32(menu.level()), i32(SpellPracticeLevel::Difficulty));
    CHECK(!menu.update(cancel_key()));
    CHECK_EQ(i32(menu.level()), i32(SpellPracticeLevel::Spell));
    CHECK(!menu.update(cancel_key()));
    CHECK_EQ(i32(menu.level()), i32(SpellPracticeLevel::Stage));
    CHECK(menu.update(cancel_key()));
    CHECK_EQ(i32(menu.command().action), i32(SpellPracticeAction::Cancel));
}

TEST(character_switch_also_on_stage_level_and_is_carried_to_output) {
    SpellHost host;
    TitleSpellPracticeMenu menu(host);
    // 004560a0 case2 在 stage 层也无条件调用 004564c0 切角色
    // （左右键写 +0x5c44 并同步 DAT_004be7b8），不是只在深层切换。
    CHECK(!menu.update(right_key()));
    CHECK_EQ(menu.character(), 1);
    CHECK(!menu.update(confirm_key()));  // stage 层角色游标带入 spell 层
    CHECK(!menu.update(right_key()));
    CHECK_EQ(menu.character(), 2);
    CHECK(!menu.update(left_key()));
    CHECK_EQ(menu.character(), 1);
    CHECK(!menu.update(right_key()));
    CHECK_EQ(menu.character(), 2);
    CHECK(!menu.update(confirm_key()));  // difficulty, character 2
    CHECK(menu.update(confirm_key()));
    CHECK_EQ(menu.command().character, 2);
}

TEST(disabled_stage_blocks_progress_and_original_slot_counts_clamp_host) {
    SpellHost host;
    host.stage1_ok = false;
    host.overcount = true;
    TitleSpellPracticeMenu menu(host);
    CHECK(!menu.update(confirm_key()));  // stage 1 disabled
    CHECK_EQ(i32(menu.level()), i32(SpellPracticeLevel::Stage));
    CHECK(!menu.blocked.empty());

    SpellHost available;
    available.overcount = true;
    TitleSpellPracticeMenu clamped(available);
    CHECK(!clamped.update(confirm_key()));
    CHECK_EQ(i32(clamped.spell_rows().size()), 4);  // stage1 table count, not 99
}

int main() { return th13test::run_all("title_spell_practice_menu_test"); }
