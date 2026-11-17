// Synthetic ECL validates metadata decoding and independent per-variant gates.
#include "../harness/test_framework.hpp"
#include "game/SpellCatalog.hpp"
#include "game/TitleSpellPracticeMenu.hpp"
#include "game/TitlePlayerDataMenu.hpp"

#include <set>

using namespace th13;
namespace {
void append16(std::vector<u8>& bytes, u16 value) {
    bytes.push_back(u8(value)); bytes.push_back(u8(value >> 8));
}
void append32(std::vector<u8>& bytes, u32 value) {
    for (i32 i = 0; i < 4; ++i) bytes.push_back(u8(value >> (i * 8)));
}
void patch32(std::vector<u8>& bytes, size_t at, u32 value) {
    for (i32 i = 0; i < 4; ++i) bytes[at + size_t(i)] = u8(value >> (i * 8));
}
void instruction(std::vector<u8>& bytes, u16 opcode, u8 ranks, i32 id,
                 const std::string& text) {
    std::vector<u8> encrypted;
    u8 key = 0x77, increment = 7;
    for (char value : text + '\0') {
        encrypted.push_back(u8(value) ^ key);
        key = u8(key + increment);
        increment = u8(increment + 0x10);
    }
    while (encrypted.size() % 4) encrypted.push_back(0);
    append32(bytes, 0); append16(bytes, opcode);
    append16(bytes, u16(32 + encrypted.size()));
    append16(bytes, 0); bytes.push_back(ranks); bytes.push_back(4);
    append32(bytes, 0);
    append32(bytes, u32(id)); append32(bytes, 3000); append32(bytes, 500000);
    append32(bytes, u32(encrypted.size()));
    bytes.insert(bytes.end(), encrypted.begin(), encrypted.end());
}
std::vector<u8> stage1() {
    std::vector<u8> bytes(36 + 16, 0);
    std::memcpy(bytes.data(), "SCPT", 4); bytes[4] = 1; bytes[16] = 4;
    for (i32 i = 0; i < 4; ++i) {
        const std::string name = "Card" + std::to_string(i);
        bytes.insert(bytes.end(), name.begin(), name.end()); bytes.push_back(0);
    }
    while (bytes.size() % 4) bytes.push_back(0);
    for (i32 i = 0; i < 4; ++i) {
        patch32(bytes, 36 + size_t(i) * 4, u32(bytes.size()));
        bytes.insert(bytes.end(), {'E','C','L','H'}); append32(bytes, 16);
        append32(bytes, 0); append32(bytes, 0);
        if (i < 3) {
            instruction(bytes, 537, 3, i * 4, "low-" + std::to_string(i));
            instruction(bytes, 539, 12, i * 4 + 2, "high-" + std::to_string(i));
        } else {
            instruction(bytes, 539, 12, 12, "normal-last");
            instruction(bytes, 522, 32, 119, "hidden-overdrive");
        }
    }
    return bytes;
}
struct CatalogHost : TitleSpellPracticePlatform {
    SpellCatalog catalog;
    std::set<i32> encountered, captured;
    i32 characters = 5; // shared TitleScenePlatform exposes PlayerData's 5 columns
    CatalogHost() {
        CHECK(catalog.load_stage(1, [](const std::string& name, std::vector<u8>& bytes) {
            if (name != "st01.ecl") return false;
            bytes = stage1(); return true;
        }));
    }
    i32 stage_count() const override { return 1; }
    bool stage_available(i32 stage) const override { return stage == 1; }
    i32 spell_count(i32 stage) const override { return catalog.spell_count(stage); }
    bool spell_unlocked(i32 stage, i32 slot) const override {
        return catalog.spell_unlocked(stage, slot, [&](i32 id) { return encountered.count(id); });
    }
    std::string spell_name(i32 stage, i32 slot) const override {
        return catalog.spell_name(stage, slot, [&](i32 id) { return encountered.count(id); });
    }
    i32 spell_id(i32 stage, i32 slot, i32 difficulty) const override {
        const auto* value = catalog.variant(stage, slot, difficulty);
        return value ? value->id : -1;
    }
    i32 spell_rank(i32 stage, i32 slot, i32 difficulty) const override {
        const auto* value = catalog.variant(stage, slot, difficulty);
        return value ? value->rank : -1;
    }
    bool spell_variant_available(i32 stage, i32 slot, i32 difficulty) const override {
        return catalog.variant_available(stage, slot, difficulty,
            [&](i32 id) { return encountered.count(id); },
            [&](i32 id) { return captured.count(id); });
    }
    std::string spell_variant_name(i32 stage, i32 slot, i32 difficulty) const override {
        const auto* value = catalog.variant(stage, slot, difficulty);
        return value ? value->name : std::string{};
    }
    i32 character_count() const override { return characters; }
};
SpellPracticeInput confirm() { SpellPracticeInput result; result.confirm = true; return result; }
SpellPracticeInput down() { SpellPracticeInput result; result.down = true; return result; }
SpellPracticeInput right() { SpellPracticeInput result; result.right = true; return result; }
struct PlayerDataHost : TitlePlayerDataPlatform {
    bool open_player_data() override { return true; }
    // 00453170 case0: +0x108=5 difficulty cursors, +0x30=4 character columns
    // (exclusive counts; FUN_00415030(1) starts the rank at Normal).
    i32 difficulty_count() const override { return 5; }
    i32 character_count() const override { return 4; }
    i32 record_count(i32) const override { return 23; }
    bool record(i32, i32, i32 ordinal, TitlePlayerDataRecord& out) const override {
        out = {}; out.slot = ordinal; out.name = "unseen-name";
        out.recorded = ordinal != 0; return true;
    }
};
}

TEST(catalog_decodes_rank_arithmetic_names_and_hides_unencountered_cards) {
    CatalogHost host;
    CHECK_EQ(host.catalog.spell_count(1), 4);
    for (i32 rank = 0; rank < 4; ++rank) {
        const auto* variant = host.catalog.variant(1, 0, rank);
        CHECK(variant != nullptr);
        if (!variant) continue;
        CHECK_EQ(variant->id, rank);
        CHECK_EQ(variant->rank, rank);
        CHECK(variant->name == (rank < 2 ? "low-0" : "high-0"));
    }
    CHECK(host.catalog.variant(1, 0, 4) == nullptr);
    CHECK(!host.spell_unlocked(1, 0));
    CHECK(host.spell_name(1, 0).empty());
    host.encountered.insert(1);
    CHECK(host.spell_unlocked(1, 0));
    CHECK(host.spell_name(1, 0) == "low-0");
    CHECK(!host.spell_variant_available(1, 0, 0));
    CHECK(host.spell_variant_available(1, 0, 1));
    CHECK(!host.spell_variant_available(1, 0, 2));
}

TEST(overdrive_requires_every_preceding_variant_capture_or_own_encounter) {
    CatalogHost host;
    host.encountered.insert(12);
    CHECK(!host.spell_variant_available(1, 3, 4));
    host.captured.insert(12);
    CHECK(!host.spell_variant_available(1, 3, 4));
    host.captured.insert(13);
    CHECK(host.spell_variant_available(1, 3, 4));
    host.captured.clear();
    host.encountered.insert(119);
    CHECK(host.spell_variant_available(1, 3, 4));
}

TEST(menu_hides_unseen_names_gates_each_rank_and_cycles_only_four_characters) {
    CatalogHost host;
    host.encountered.insert(1);
    TitleSpellPracticeMenu menu(host);
    CHECK(!menu.update(confirm()));
    CHECK(menu.spell_rows()[0].unlocked);
    CHECK(menu.spell_rows()[1].name.empty());
    for (i32 i = 0; i < 4; ++i) CHECK(!menu.update(right()));
    CHECK_EQ(menu.character(), 0);
    CHECK(!menu.update(confirm()));
    CHECK(!menu.difficulty_rows()[0].available);
    CHECK(menu.difficulty_rows()[0].name.empty());
    CHECK(menu.difficulty_rows()[1].available);
    // 004576c0 case0 经 004417c0 把未遭遇行加入游标禁用表，上下移动经
    // 00473210 跳过禁用值：进入难度层光标直接落在首个可用行 Normal(id 1)，
    // 不会停在未遭遇的 Easy(0) 上，故无"确认被阻止"路径。
    CHECK_EQ(menu.cursor(), 1);
    CHECK(!menu.update(down()));  // 其余行全部禁用，跳行后仍停留 id 1
    CHECK_EQ(menu.cursor(), 1);
    CHECK_EQ(i32(menu.command().action), i32(SpellPracticeAction::None));  // 未见变体从未发射
    CHECK(!menu.update(right()));
    CHECK_EQ(menu.cursor(), 1); // character rebuild retains the rank cursor
    CHECK(menu.update(confirm()));
    CHECK_EQ(menu.command().spell_id, 1);
    CHECK_EQ(menu.command().character, 1);
}

TEST(player_data_confirm_pages_ten_rows_and_repeat_navigates_without_name_leak) {
    TitleState state;
    state.screen = TitleScreen::PlayerData;
    PlayerDataHost host;
    TitlePlayerDataMenu menu(state, host);
    CHECK(menu.update(0, 0));
    for (i32 i = 0; i < 7; ++i) CHECK(menu.update(0, 0));
    // 00453170 case0：初始 Normal=1（FUN_00415030(1)），+0x108=5 独占计数。
    CHECK_EQ(menu.difficulty(), 1);
    // 00453170 case2：page0（*local_10==0）为汇总页、不创建 10 个记录行 VM；
    // 页数 = 1 + (23+9)/10 = 4（page1..3 才是 10 行卡页）。
    CHECK_EQ(menu.page_count(), 4);
    CHECK(menu.visible_records().empty());
    CHECK(menu.update(1, 0)); CHECK_EQ(menu.page(), 1);
    CHECK_EQ(menu.visible_records().size(), size_t(10));
    CHECK(menu.visible_records()[0].slot == 0);
    CHECK(menu.visible_records()[0].name.empty());  // 未遭遇记录不泄漏名字
    CHECK(menu.update(1, 0)); CHECK_EQ(menu.page(), 2);
    CHECK_EQ(menu.visible_records().size(), size_t(10));
    CHECK(menu.visible_records()[0].slot == 10);
    CHECK(menu.update(1, 0)); CHECK_EQ(menu.page(), 3);
    CHECK_EQ(menu.visible_records().size(), size_t(3));
    CHECK(menu.update(1, 0)); CHECK_EQ(menu.page(), 0);
    CHECK(menu.update(0, 0x80)); CHECK_EQ(menu.character(), 1);
    CHECK(menu.update(0, 0x20)); CHECK_EQ(menu.difficulty(), 2);  // Normal=1 基准，一次 ↓ → 2
}

int main() { return th13test::run_all("spell_records_catalog_test"); }
