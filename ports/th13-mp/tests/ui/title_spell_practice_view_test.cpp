// 符卡练习 View 回归（0x4560a0/0x456650 屏间往返）：
//   Spell→Stage 取消时 0x456650 case4 对 names(+0x694×5)/103 徽章/188 卡表/
//   187 关卡表发 label1 中断并清句柄，随后 0x4560a0 case0 检测 187 句柄失效
//   经 0044bb80 整表重建（0x46fb90 失效判定）。移植不得对已 label6 锁定的
//   旧 187 VM 重发 select——那正是「返回后列表只剩选中项」的成因。
// 合成脚本只验证句柄/中断语义，不模拟真实美术。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/TitleSpellPracticeMenu.hpp"
#include "game/TitleSpellPracticeView.hpp"

#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct SpellHost final : TitleSpellPracticePlatform {
    i32 stage_count() const override { return kSpellPracticeStages; }
    bool stage_available(i32 stage) const override { return stage >= 1 && stage <= 6; }
    std::string stage_name(i32 stage) const override {
        return "stage " + std::to_string(stage);
    }
    i32 spell_count(i32 stage) const override { return kSpellPracticeSlots[stage - 1]; }
    // View 级回归不覆盖锁定门（menu 套件已测）；全解锁让任意 stage 可走通
    // 三级往返。
    bool spell_unlocked(i32, i32) const override { return true; }
    std::string spell_name(i32 stage, i32 slot) const override {
        return "spell " + std::to_string(stage) + "-" + std::to_string(slot);
    }
    i32 spell_id(i32 stage, i32 slot, i32 difficulty) const override {
        return stage * 100 + slot * 10 + difficulty;
    }
    i32 spell_rank(i32 stage, i32 slot, i32 difficulty) const override {
        (void)stage; (void)slot;
        return difficulty;
    }
    bool spell_record(i32, i32, SpellPracticeRecord& record) const override {
        record = SpellPracticeRecord{};
        record.encountered = true;
        return true;
    }
    std::string spell_variant_name(i32, i32, i32) const override { return "variant"; }
    i32 character_count() const override { return kSpellPracticeCharacters; }
    std::string character_name(i32 character) const override {
        return "char " + std::to_string(character);
    }
};

// 带 label 的合成脚本体：label(id); wait(); … 让 0x46fca0/0x46fc30 的
// label 跳转与 pending_interrupt 语义可观测。
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
    int frame_rasterized = 0;  // 本帧 rebuild_names 栅格化行数（0 = 未重建）
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

}  // namespace

TEST(spell_cancel_rebuilds_stage_list_vm) {
    Harness harness;
    CHECK(harness.frame());  // 0x4560a0 case0：开表（187/240/109/particles）
    AnmVm* stage_vm = harness.unique(187);
    CHECK(stage_vm != nullptr);

    CHECK(harness.frame(down_key()));   // 关卡光标 0 -> 1（STAGE 2）
    CHECK(harness.frame(down_key()));   // 1 -> 2（STAGE 3，复现截图路径）
    CHECK(harness.frame(confirm_key()));
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Spell));
    CHECK(harness.frame(confirm_key()));  // 解锁卡进难度层，188 仍在
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Difficulty));
    // 难度层取消回卡表：0x4576c0 case4（0x457941-0x457977）对 188 发
    // label1 并清句柄；重入 0x12 由 0x456650 case0 判 [esi+0x5b8] 失效
    // 整表重建（0x4566ca-0x4566e6）。旧 VM 退场（pending=1）+ 新 VM 同帧共存。
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
    // 卡表取消回关卡层：0x456650 case4 label1 清 187 句柄 + 0x4560a0 case0 重建。
    CHECK(harness.frame(cancel_key()));
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Stage));

    // 旧 187 VM 收到 label1 退场中断（00456650 case4：非立即中断挂 pending）。
    CHECK_EQ(stage_vm->pending_interrupt, 1);
    // 187 是新建实例：对已 label6 锁定的旧 VM 重发 select 正是列表消失成因。
    const auto rebuilt = harness.by_script(187);
    bool fresh = false;
    for (AnmVm* vm : rebuilt) fresh = fresh || vm != stage_vm;
    CHECK(fresh);
    CHECK_EQ(rebuilt.size(), 2u);  // 旧 VM 退场 + 新 VM（同帧共存，原版同构）
    // 新 VM 处于选中态：case1 的 label3 立即 + label(7+cursor) 选中。
    bool selected = false;
    for (AnmVm* vm : rebuilt)
        selected = selected || (vm != stage_vm &&
                                vm->pending_interrupt == 7 + harness.menu.cursor());
    CHECK(selected);
    // 关卡徽章按 0x4560a0 case0 重建：至少存在一个未退场的 109 实例
    // （旧 109 在进卡表时已 label1 退场，合成脚本 stop 不自删故仍占池）。
    bool badge_alive = false;
    for (AnmVm* vm : harness.by_script(109))
        badge_alive = badge_alive || vm->pending_interrupt != 1;
    CHECK(badge_alive);
}

TEST(stage_confirm_then_immediate_cancel_restores_list) {
    // 最短路径：确认进卡表后立刻取消（不经过难度层）。
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
        selected = selected || (vm != stage_vm &&
                                vm->pending_interrupt == 7 + harness.menu.cursor());
    CHECK(selected);
}

TEST(spell_difficulty_cancel_rebuilds_cards_vm) {
    // Stage→Spell 正向路径：188 只建一次，禁用槽 label39… 发往 188；难度层
    // 退回卡表时 0x4576c0 case4（0x457941-0x457977）对旧 188 发 label1 并
    // 清句柄（0x457960 `mov [esi+0x5b8],0`），0x456650 case0 判失效重建。
    Harness harness;
    CHECK(harness.frame());
    CHECK(harness.frame(confirm_key()));
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Spell));
    AnmVm* cards_vm = harness.unique(188);
    CHECK(cards_vm != nullptr);
    CHECK(harness.frame(confirm_key()));
    CHECK(harness.frame(cancel_key()));  // 难度层退回卡表
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Spell));
    CHECK_EQ(cards_vm->pending_interrupt, 1);  // 旧 188 label1 退场
    bool rebuilt = false;
    for (AnmVm* vm : harness.by_script(188))
        rebuilt = rebuilt || (vm != cards_vm && vm->pending_interrupt != 1);
    CHECK(rebuilt);  // 0x456650 case0 整表重建的新 188
}

TEST(run_return_cascade_opens_difficulty_level_stack) {
    // [H-SP2] 对局返回（DAT_004bb4c4>=0）：0x4561bf 重建 187 并立即锁定
    // （label3/label(7+stage)/label6），0x45673a 建 188 同序锁定，直达难度层。
    Harness harness;
    CHECK(harness.menu.restore_selection(1, 0, 3));
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Difficulty));
    CHECK(harness.frame());
    CHECK(harness.unique(187) != nullptr);  // 0x4560a0 case0 重建
    CHECK(harness.unique(188) != nullptr);  // 0x456650 case0 新建
    CHECK(harness.unique(240) != nullptr);  // 角色层句柄仍活
    CHECK(harness.unique(103) != nullptr);  // 卡表层徽章
    // 难度行（names 182..186，family=slot0）已按 +0x5d24 栅格化。
    CHECK_EQ(harness.frame_rasterized, 5);
    // Esc 逐级退场不受影响：难度层 → 卡表（光标=启动槽位）→ 关卡层。
    CHECK(harness.frame(cancel_key()));
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Spell));
    CHECK_EQ(harness.menu.cursor(), 0);
    CHECK(harness.frame(cancel_key()));
    CHECK_EQ(i32(harness.menu.level()), i32(SpellPracticeLevel::Stage));
    CHECK_EQ(harness.menu.cursor(), 0);
    // 关卡层退场后再入卡表仍走重建路径（旧 188 已 label1 退场、新 188 在场）。
    CHECK(harness.frame(confirm_key()));
    bool cards_rebuilt = false;
    for (AnmVm* vm : harness.by_script(188))
        cards_rebuilt = cards_rebuilt || vm->pending_interrupt != 1;
    CHECK(cards_rebuilt);
}

int main() { return th13test::run_all("title_spell_practice_view_test"); }
