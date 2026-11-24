#define TH13_APPLICATION_PLATFORMS_ONLY
#include "sdl/ApplicationState.hpp"

#include "../harness/test_framework.hpp"
#include "game/TitlePlayerDataMenu.hpp"
#include "game/TitleSpellPracticeMenu.hpp"

#include <string>

using namespace th13;
using namespace th13::sdl;

namespace {

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

constexpr i32 kSpellSlots[kSpellPracticeStages] = {4, 4, 4, 4, 5, 7, 13};

}

TEST(player_data_host_reports_fixed_shape_and_empty_real_table) {
    TitlePlayerDataHost host;
    CHECK(host.open_player_data());
    CHECK_EQ(host.difficulty_count(), kPlayerDataDifficulties);
    CHECK_EQ(host.character_count(), kPlayerDataCharacters);
    for (i32 difficulty = 0; difficulty < kPlayerDataDifficulties; ++difficulty)
        CHECK_EQ(host.record_count(difficulty), 0);
    CHECK_EQ(host.record_count(-1), 0);
    CHECK_EQ(host.record_count(kPlayerDataDifficulties), 0);
    TitlePlayerDataRecord record;
    CHECK(!host.record(0, 0, 0, record));
}

TEST(player_data_menu_opens_and_returns_with_empty_table) {
    TitleState state;
    state.screen = TitleScreen(kTitleScreenPlayerData);
    TitlePlayerDataHost host;
    TitlePlayerDataMenu menu{state, host};
    CHECK(menu.open());
    CHECK(menu.error.empty());
    CHECK(menu.opened());
    CHECK_EQ(state.substate, 1);
    for (i32 i = 0; i < 7; ++i)
        CHECK(menu.update(0, 0));
    CHECK_EQ(state.substate, 2);
    CHECK_EQ(menu.record_count(), 0);
    CHECK_EQ(menu.page_count(), 1);
    CHECK_EQ(int(menu.visible_records().size()), 0);
    for (i32 i = 0; i < kPlayerDataDifficulties; ++i) {
        CHECK(menu.update(0x20, 0));
        CHECK_EQ(menu.record_count(), 0);
        CHECK(menu.error.empty());
    }
    CHECK_EQ(menu.difficulty(), 1);
    for (i32 i = 0; i < kPlayerDataCharacters; ++i) {
        CHECK(menu.update(0x80, 0));
        CHECK_EQ(menu.record_count(), 0);
        CHECK(menu.error.empty());
    }
    CHECK_EQ(menu.character(), 0);
    CHECK(menu.back());
    for (i32 i = 0; i < 6; ++i)
        CHECK(menu.update(0, 0));
    CHECK_EQ(int(state.screen), int(TitleScreen::Main));
    CHECK(!menu.opened());
    CHECK(menu.error.empty());
}

TEST(spell_practice_host_lists_real_slot_counts_but_keeps_cards_locked) {
    TitleSpellPracticeHost host;
    CHECK_EQ(host.stage_count(), kSpellPracticeStages);
    CHECK_EQ(host.character_count(), kSpellPracticeCharacters);
    for (i32 stage = 1; stage <= kSpellPracticeStages; ++stage) {
        CHECK(host.stage_available(stage));
        CHECK(!host.stage_name(stage).empty());
        CHECK_EQ(host.spell_count(stage), kSpellSlots[stage - 1]);
        for (i32 slot = 0; slot < kSpellSlots[stage - 1]; ++slot) {
            CHECK(!host.spell_unlocked(stage, slot));
            CHECK(host.spell_name(stage, slot).empty());
            for (i32 difficulty = 0; difficulty < kSpellPracticeDifficulties; ++difficulty) {
                CHECK_EQ(host.spell_id(stage, slot, difficulty), -1);
                CHECK_EQ(host.spell_rank(stage, slot, difficulty), -1);
            }
        }
    }
    CHECK(!host.stage_available(0));
    CHECK(!host.stage_available(kSpellPracticeStages + 1));
    CHECK_EQ(host.spell_count(0), 0);
    CHECK_EQ(host.spell_count(kSpellPracticeStages + 1), 0);
    CHECK_EQ(host.character_name(0), std::string("Reimu  "));
    CHECK(host.character_name(kSpellPracticeCharacters).empty());
}

TEST(spell_practice_menu_never_launches_locked_cards) {
    TitleSpellPracticeHost host;
    TitleSpellPracticeMenu menu{host};
    CHECK(menu.error.empty());
    CHECK_EQ(int(menu.stage_rows().size()), kSpellPracticeStages);
    for (const auto& row : menu.stage_rows())
        CHECK(row.available);

    for (i32 stage = 0; stage < kSpellPracticeStages; ++stage) {
        CHECK_EQ(menu.cursor(), stage);
        CHECK(!menu.update(confirm_key()));
        CHECK_EQ(int(menu.level()), int(SpellPracticeLevel::Spell));
        CHECK_EQ(int(menu.spell_rows().size()), kSpellSlots[stage]);
        for (const auto& row : menu.spell_rows()) {
            CHECK(!row.unlocked);
            CHECK(row.name.empty());
        }
        CHECK(!menu.update(confirm_key()));
        CHECK_EQ(menu.blocked, std::string("spell card locked"));
        CHECK_EQ(int(menu.command().action), int(SpellPracticeAction::None));
        CHECK(!menu.update(cancel_key()));
        CHECK_EQ(int(menu.level()), int(SpellPracticeLevel::Stage));
        if (stage + 1 < kSpellPracticeStages) CHECK(!menu.update(down_key()));
    }
    CHECK(menu.update(cancel_key()));
    CHECK_EQ(int(menu.command().action), int(SpellPracticeAction::Cancel));

    TitleSpellPracticeMenu cycle{host};
    CHECK(!cycle.update(confirm_key()));
    CHECK_EQ(cycle.character(), 0);
    for (i32 i = 0; i < kSpellPracticeCharacters; ++i) {
        CHECK(!cycle.update(right_key()));
        CHECK_EQ(cycle.character(), (i + 1) % kSpellPracticeCharacters);
    }
    CHECK(!cycle.update(left_key()));
    CHECK_EQ(cycle.character(), kSpellPracticeCharacters - 1);
    CHECK_EQ(int(cycle.command().action), int(SpellPracticeAction::None));
}

int main() { return th13test::run_all("title_platform_host_wiring_test"); }
