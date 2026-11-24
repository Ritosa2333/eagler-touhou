#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/TitleSpellPracticeMenu.hpp"
#include "game/TitleSpellPracticeView.hpp"

#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct SpellHost final : TitleSpellPracticePlatform {
    i32 stage_count() const override { return kSpellPracticeStages; }
    bool stage_available(i32 stage) const override { return stage >= 1 && stage <= 6; }
    std::string stage_name(i32 stage) const override { return "stage " + std::to_string(stage); }
    i32 spell_count(i32 stage) const override { return kSpellPracticeSlots[stage - 1]; }
    bool spell_unlocked(i32, i32) const override { return true; }
    std::string spell_name(i32 stage, i32 slot) const override {
        return "spell " + std::to_string(stage) + "-" + std::to_string(slot);
    }
    i32 spell_id(i32 stage, i32 slot, i32 difficulty) const override { return stage * 100 + slot * 10 + difficulty; }
    i32 spell_rank(i32 stage, i32 slot, i32 difficulty) const override {
        (void)stage;
        (void)slot;
        return difficulty;
    }
    bool spell_record(i32, i32, SpellPracticeRecord& record) const override {
        record = SpellPracticeRecord{};
        record.encountered = true;
        return true;
    }
    std::string spell_variant_name(i32, i32, i32) const override { return "variant"; }
    i32 character_count() const override { return kSpellPracticeCharacters; }
    std::string character_name(i32 character) const override { return "char " + std::to_string(character); }
};

std::vector<std::vector<AnmIns>> labeled_scripts(u32 count, const std::vector<i32>& labels) {
    std::vector<std::vector<AnmIns>> scripts(count);
    for (u32 i = 0; i < count; ++i) {
        scripts[i].push_back(wait());
        for (i32 id : labels) {
            scripts[i].push_back(label(id));
            scripts[i].push_back(wait());
        }
        scripts[i].push_back(stop());
    }
    return scripts;
}

struct Harness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SpellHost host;
    TitleSpellPracticeMenu menu{host};
    TitleSpellPracticeView view{manager, 16};

    Harness() {
        const std::vector<i32> labels = {1, 3, 6, 7, 8, 9, 10, 11, 12, 13, 39, 38};
        CHECK(load_bank(manager, 16, synthetic_anm(8, labeled_scripts(241, labels))));
        CHECK(load_bank(manager, 2, synthetic_anm(8, labeled_scripts(24, {1}))));
    }
    bool frame(SpellPracticeInput input = {}) {
        menu.update(input);
        frame_rasterized = 0;
        const bool ok = view.update(menu, host, [this](AnmVm&, u32, const std::string&) {
            ++frame_rasterized;
            return true;
        });
        return ok;
    }
    int frame_rasterized = 0;
    std::vector<AnmVm*> by_script(i32 script) const {
        std::vector<AnmVm*> found;
        for (u32 i = 0; i < manager.registry.ordered_count(false); ++i)
            if (auto* vm = manager.registry.find(manager.registry.ordered_handle(false, i)))
                if (vm->source_script == script) found.push_back(vm);
        return found;
    }
    AnmVm* unique(i32 script) const {
        const auto found = by_script(script);
        return found.size() == 1 ? found[0] : nullptr;
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

}

TEST(spell_cancel_rebuilds_stage_list_vm) {
    Harness harness;
    CHECK(harness.frame());
    AnmVm* stage_vm = harness.unique(187);
    CHECK(stage_vm != nullptr);

    CHECK(harness.frame(down_key()));
    CHECK(harness.frame(down_key()));
    CHECK(harness.frame(confirm_key()));
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Spell));
    CHECK(harness.frame(confirm_key()));
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Difficulty));
    CHECK(harness.frame(cancel_key()));
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Spell));
    const auto cards_pair = harness.by_script(188);
    CHECK_EQ(cards_pair.size(), 2u);
    bool cards_old_exiting = false, cards_fresh = false;
    for (AnmVm* vm : cards_pair) {
        cards_old_exiting = cards_old_exiting || vm->pending_interrupt == 1;
        cards_fresh = cards_fresh || vm->pending_interrupt != 1;
    }
    CHECK(cards_old_exiting);
    CHECK(cards_fresh);
    CHECK(harness.frame(cancel_key()));
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Stage));

    CHECK_EQ(stage_vm->pending_interrupt, 1);
    const auto rebuilt = harness.by_script(187);
    bool fresh = false;
    for (AnmVm* vm : rebuilt)
        fresh = fresh || vm != stage_vm;
    CHECK(fresh);
    CHECK_EQ(rebuilt.size(), 2u);
    bool selected = false;
    for (AnmVm* vm : rebuilt)
        selected = selected || (vm != stage_vm && vm->pending_interrupt == 7 + harness.menu.cursor());
    CHECK(selected);
    bool badge_alive = false;
    for (AnmVm* vm : harness.by_script(109))
        badge_alive = badge_alive || vm->pending_interrupt != 1;
    CHECK(badge_alive);
}

TEST(stage_confirm_then_immediate_cancel_restores_list) {
    Harness harness;
    CHECK(harness.frame());
    AnmVm* stage_vm = harness.unique(187);
    CHECK(stage_vm != nullptr);
    CHECK(harness.frame(confirm_key()));
    CHECK(harness.frame(cancel_key()));
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Stage));
    CHECK_EQ(stage_vm->pending_interrupt, 1);
    const auto rebuilt = harness.by_script(187);
    CHECK_EQ(rebuilt.size(), 2u);
    bool selected = false;
    for (AnmVm* vm : rebuilt)
        selected = selected || (vm != stage_vm && vm->pending_interrupt == 7 + harness.menu.cursor());
    CHECK(selected);
}

TEST(spell_difficulty_cancel_rebuilds_cards_vm) {
    Harness harness;
    CHECK(harness.frame());
    CHECK(harness.frame(confirm_key()));
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Spell));
    AnmVm* cards_vm = harness.unique(188);
    CHECK(cards_vm != nullptr);
    CHECK(harness.frame(confirm_key()));
    CHECK(harness.frame(cancel_key()));
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Spell));
    CHECK_EQ(cards_vm->pending_interrupt, 1);
    bool rebuilt = false;
    for (AnmVm* vm : harness.by_script(188))
        rebuilt = rebuilt || (vm != cards_vm && vm->pending_interrupt != 1);
    CHECK(rebuilt);
}

TEST(run_return_cascade_opens_difficulty_level_stack) {
    Harness harness;
    CHECK(harness.menu.restore_selection(1, 0, 3));
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Difficulty));
    CHECK(harness.frame());
    CHECK(harness.unique(187) != nullptr);
    CHECK(harness.unique(188) != nullptr);
    CHECK(harness.unique(240) != nullptr);
    CHECK(harness.unique(103) != nullptr);
    CHECK_EQ(harness.frame_rasterized, 5);
    CHECK(harness.frame(cancel_key()));
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Spell));
    CHECK_EQ(harness.menu.cursor(), 0);
    CHECK(harness.frame(cancel_key()));
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Stage));
    CHECK_EQ(harness.menu.cursor(), 0);
    CHECK(harness.frame(confirm_key()));
    bool cards_rebuilt = false;
    for (AnmVm* vm : harness.by_script(188))
        cards_rebuilt = cards_rebuilt || vm->pending_interrupt != 1;
    CHECK(cards_rebuilt);
}

int main() { return th13test::run_all("title_spell_practice_view_test"); }
