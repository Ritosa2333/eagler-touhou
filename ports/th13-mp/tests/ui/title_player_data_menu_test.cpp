// プレイヤーデータ屏单测（0x44cd10 case4 项5 → 屏10 = 0x453170 对合）：
//   默认宿主显式失败；固定形状/记录范围/槽号负控；↑↓难度 + ←→角色环绕；
//   page0 汇总页（无记录行）+ 每 10 行卡页分页；X/Esc 6 帧收面回 Main；
//   记录字段原样透传。初始难度 Normal=1（004531a5..b7 FUN_00415030(1)），
//   只加载当前难度（00453170 case2 列表重建），夹具数据放在难度 1。
#include "../harness/test_framework.hpp"

#include "game/TitlePlayerDataMenu.hpp"

#include <string>
#include <vector>

using namespace th13;

namespace {

struct PlayerDataHost final : TitlePlayerDataPlatform {
    bool open_ok = true;
    bool bad_record = false;
    i32 difficulties = kPlayerDataDifficulties;
    i32 characters = kPlayerDataCharacters;
    i32 count = 23;
    i32 opens = 0, closes = 0;

    bool open_player_data() override {
        ++opens;
        return open_ok;
    }
    void close_player_data() override { ++closes; }
    i32 difficulty_count() const override { return difficulties; }
    i32 character_count() const override { return characters; }
    i32 record_count(i32 difficulty) const override {
        // 00453170 case0/case2：菜单初始 Normal=1 且只加载当前难度，
        // 测试数据固定放在难度 1，其余难度如实返回 0。
        return difficulty == 1 ? count : 0;
    }
    bool record(i32 difficulty, i32 character, i32 ordinal,
                TitlePlayerDataRecord& out) const override {
        if (difficulty != 1 || ordinal < 0 || ordinal >= count) return false;
        out.slot = bad_record ? kPlayerDataRecordSlots : ordinal;
        out.name = "record-" + std::to_string(ordinal);
        out.score = 1000 + ordinal;
        out.number_a = ordinal + 1;
        out.number_b = 6;
        out.character_flagged = character == 0;
        return true;
    }
};

bool reach_interactive(TitleState& state, PlayerDataHost& host,
                       TitlePlayerDataMenu& menu) {
    (void)host;
    if (!menu.open()) return false;
    for (i32 i = 0; i < 7; ++i) {
        if (!menu.update(0, 0)) return false;
    }
    return state.substate == 2;
}

}  // namespace

TEST(player_data_default_platform_fails_honestly) {
    TitleState state;
    state.screen = TitleScreen(kTitleScreenPlayerData);
    TitlePlayerDataPlatform platform;  // 未接线：默认失败
    TitlePlayerDataMenu menu{state, platform};
    CHECK(!menu.open());
    CHECK(!menu.error.empty());
    CHECK_EQ(state.substate, 0);
    CHECK(!menu.opened());
    CHECK(!menu.update(0, 0));  // 重试仍如实失败，不空白切屏
    CHECK_EQ(state.substate, 0);
}

TEST(player_data_open_shape_and_record_negative_controls) {
    {
        TitleState state;
        PlayerDataHost host;
        host.difficulties = 4;
        TitlePlayerDataMenu menu{state, host};
        CHECK(!menu.open());
        CHECK(!menu.error.empty());
        CHECK_EQ(host.closes, 1);
        CHECK_EQ(state.substate, 0);
    }
    {
        TitleState state;
        PlayerDataHost host;
        host.count = kPlayerDataRecordSlots + 1;
        TitlePlayerDataMenu menu{state, host};
        CHECK(!menu.open());
        CHECK_EQ(state.substate, 0);
    }
    {
        TitleState state;
        PlayerDataHost host;
        host.bad_record = true;
        TitlePlayerDataMenu menu{state, host};
        CHECK(!menu.open());
        CHECK_EQ(host.closes, 1);
        CHECK_EQ(state.substate, 0);
    }
}

TEST(player_data_lifecycle_and_record_fields) {
    TitleState state;
    state.screen = TitleScreen(kTitleScreenPlayerData);
    PlayerDataHost host;
    TitlePlayerDataMenu menu{state, host};
    std::vector<i32> sounds;
    menu.play_sound = [&sounds](i32 id) { sounds.push_back(id); return true; };

    CHECK(menu.open());
    CHECK(menu.opened());
    CHECK_EQ(host.opens, 1);
    CHECK_EQ(state.substate, 1);
    for (i32 i = 0; i < 6; ++i) CHECK(menu.update(0, 0));
    CHECK_EQ(state.substate, 1);
    CHECK(menu.update(0, 0));
    CHECK_EQ(state.substate, 2);

    // 00453170 case0：初始 Normal=1（FUN_00415030(1)），只加载当前难度。
    CHECK_EQ(menu.difficulty(), 1);
    CHECK_EQ(menu.character(), 0);
    CHECK_EQ(menu.page(), 0);
    // 00453170 case2：页数 = 1 + (23+9)/10 = 4；page0 为汇总页、无记录行。
    CHECK_EQ(menu.page_count(), 4);
    CHECK_EQ(menu.record_count(), 23);
    CHECK(menu.visible_records().empty());
    CHECK(menu.next_page());
    CHECK_EQ(menu.page(), 1);
    const auto visible = menu.visible_records();
    CHECK_EQ(int(visible.size()), 10);
    CHECK_EQ(visible[0].slot, 0);
    CHECK_EQ(visible[0].name, std::string("record-0"));
    CHECK_EQ(visible[0].score, 1000);
    CHECK_EQ(visible[0].number_a, 1);
    CHECK_EQ(visible[0].number_b, 6);
    CHECK(visible[0].character_flagged);
    CHECK_EQ(visible[9].slot, 9);
}

TEST(player_data_navigation_wraps_and_keeps_card_pages) {
    TitleState state;
    state.screen = TitleScreen(kTitleScreenPlayerData);
    PlayerDataHost host;
    host.count = 23;
    TitlePlayerDataMenu menu{state, host};
    std::vector<i32> sounds;
    menu.play_sound = [&sounds](i32 id) { sounds.push_back(id); return true; };
    CHECK(reach_interactive(state, host, menu));

    CHECK(menu.set_page(2));
    CHECK_EQ(menu.page(), 2);
    CHECK(menu.update(0x20, 0));  // ↓ 难度 1→2；page>0 进卡片类页（00453449），
    CHECK_EQ(menu.difficulty(), 2);  // 但该难度 0 记录 → 页数 1，clamp 回汇总页 0
    CHECK_EQ(menu.page(), 0);
    CHECK_EQ(menu.record_count(), 0);  // 难度 2 无夹具数据
    CHECK(menu.update(0x10, 0));  // ↑ 回 Normal=1
    CHECK_EQ(menu.difficulty(), 1);
    CHECK(menu.update(0x10, 0));  // ↑ 环绕 1→0（Easy；+0x108=5 独占计数 0..4）
    CHECK_EQ(menu.difficulty(), 0);
    CHECK(menu.update(0x20, 0));  // ↓ 回 Normal=1，恢复非空记录列
    CHECK_EQ(menu.difficulty(), 1);

    CHECK(menu.update(0x80, 0));  // → 角色 0→1
    CHECK_EQ(menu.character(), 1);
    CHECK(menu.update(1, 0));  // Z 翻页 0→1（00453170 case2 confirm 推进页游标）
    CHECK_EQ(menu.page(), 1);
    const auto nav_visible = menu.visible_records();
    CHECK_EQ(int(nav_visible.size()), 10);
    CHECK(!nav_visible[0].character_flagged);  // 角色列改变标志
    CHECK(menu.update(0x40, 0));  // ← 1→0
    CHECK_EQ(menu.character(), 0);
    CHECK(menu.update(0x40, 0));  // ← 环绕 0→4
    CHECK_EQ(menu.character(), kPlayerDataCharacters - 1);
    CHECK(!sounds.empty());
    CHECK_EQ(sounds.back(), 10);  // 选择变化音
}

TEST(player_data_pages_ten_rows_and_clamp) {
    TitleState state;
    state.screen = TitleScreen(kTitleScreenPlayerData);
    PlayerDataHost host;
    host.count = 23;
    TitlePlayerDataMenu menu{state, host};
    CHECK(reach_interactive(state, host, menu));

    // 00453170 case2：page0 汇总页（*local_10==0）不创建记录行；
    // 页数 = 1 + (23+9)/10 = 4，page1..3 才是 10 行卡页。
    CHECK_EQ(menu.page_count(), 4);
    CHECK(menu.visible_records().empty());
    CHECK(menu.next_page());
    CHECK_EQ(menu.page(), 1);
    CHECK_EQ(int(menu.visible_records().size()), 10);
    CHECK_EQ(menu.visible_records()[0].slot, 0);
    CHECK(menu.next_page());
    CHECK_EQ(menu.page(), 2);
    CHECK_EQ(int(menu.visible_records().size()), 10);
    CHECK_EQ(menu.visible_records()[0].slot, 10);
    CHECK(menu.next_page());
    CHECK_EQ(menu.page(), 3);
    const auto last_page = menu.visible_records();
    CHECK_EQ(int(last_page.size()), 3);
    CHECK_EQ(last_page[0].slot, 20);
    CHECK(!menu.next_page());
    CHECK(menu.previous_page());
    CHECK_EQ(menu.page(), 2);
    CHECK(menu.previous_page());
    CHECK_EQ(menu.page(), 1);
    CHECK(menu.previous_page());
    CHECK_EQ(menu.page(), 0);
    CHECK(!menu.previous_page());
    CHECK(menu.set_page(99));
    CHECK_EQ(menu.page(), 3);
}

TEST(player_data_back_closes_after_six_frames) {
    TitleState state;
    state.screen = TitleScreen(kTitleScreenPlayerData);
    PlayerDataHost host;
    TitlePlayerDataMenu menu{state, host};
    std::vector<i32> sounds;
    menu.play_sound = [&sounds](i32 id) { sounds.push_back(id); return true; };
    CHECK(reach_interactive(state, host, menu));

    CHECK(menu.back());
    CHECK_EQ(state.substate, 3);
    CHECK_EQ(sounds.back(), 9);
    for (i32 i = 0; i < 5; ++i) CHECK(menu.update(0, 0));
    CHECK_EQ(state.substate, 3);
    CHECK_EQ(int(state.screen), kTitleScreenPlayerData);
    CHECK(menu.update(0, 0));
    CHECK_EQ(host.closes, 1);
    CHECK(!menu.opened());
    CHECK_EQ(int(state.screen), int(TitleScreen::Main));
    CHECK(menu.error.empty());
}

int main() {
    std::printf("# title_player_data_menu_test\n");
    return th13test::run_all("title_player_data_menu_test");
}
