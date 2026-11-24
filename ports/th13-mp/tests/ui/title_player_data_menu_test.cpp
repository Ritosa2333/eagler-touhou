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
    i32 record_count(i32 difficulty) const override { return difficulty == 1 ? count : 0; }
    bool record(i32 difficulty, i32 character, i32 ordinal, TitlePlayerDataRecord& out) const override {
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

bool reach_interactive(TitleState& state, PlayerDataHost& host, TitlePlayerDataMenu& menu) {
    (void)host;
    if (!menu.open()) return false;
    for (i32 i = 0; i < 7; ++i) {
        if (!menu.update(0, 0)) return false;
    }
    return state.substate == 2;
}

}

TEST(player_data_default_platform_fails_honestly) {
    TitleState state;
    state.screen = TitleScreen(kTitleScreenPlayerData);
    TitlePlayerDataPlatform platform;
    TitlePlayerDataMenu menu{state, platform};
    CHECK(!menu.open());
    CHECK(!menu.error.empty());
    CHECK_EQ(state.substate, 0);
    CHECK(!menu.opened());
    CHECK(!menu.update(0, 0));
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
    menu.play_sound = [&sounds](i32 id) {
        sounds.push_back(id);
        return true;
    };

    CHECK(menu.open());
    CHECK(menu.opened());
    CHECK_EQ(host.opens, 1);
    CHECK_EQ(state.substate, 1);
    for (i32 i = 0; i < 6; ++i)
        CHECK(menu.update(0, 0));
    CHECK_EQ(state.substate, 1);
    CHECK(menu.update(0, 0));
    CHECK_EQ(state.substate, 2);

    CHECK_EQ(menu.difficulty(), 1);
    CHECK_EQ(menu.character(), 0);
    CHECK_EQ(menu.page(), 0);
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
    menu.play_sound = [&sounds](i32 id) {
        sounds.push_back(id);
        return true;
    };
    CHECK(reach_interactive(state, host, menu));

    CHECK(menu.set_page(2));
    CHECK_EQ(menu.page(), 2);
    CHECK(menu.update(0x20, 0));
    CHECK_EQ(menu.difficulty(), 2);
    CHECK_EQ(menu.page(), 0);
    CHECK_EQ(menu.record_count(), 0);
    CHECK(menu.update(0x10, 0));
    CHECK_EQ(menu.difficulty(), 1);
    CHECK(menu.update(0x10, 0));
    CHECK_EQ(menu.difficulty(), 0);
    CHECK(menu.update(0x20, 0));
    CHECK_EQ(menu.difficulty(), 1);

    CHECK(menu.update(0x80, 0));
    CHECK_EQ(menu.character(), 1);
    CHECK(menu.update(1, 0));
    CHECK_EQ(menu.page(), 1);
    const auto nav_visible = menu.visible_records();
    CHECK_EQ(int(nav_visible.size()), 10);
    CHECK(!nav_visible[0].character_flagged);
    CHECK(menu.update(0x40, 0));
    CHECK_EQ(menu.character(), 0);
    CHECK(menu.update(0x40, 0));
    CHECK_EQ(menu.character(), kPlayerDataCharacters - 1);
    CHECK(!sounds.empty());
    CHECK_EQ(sounds.back(), 10);
}

TEST(player_data_pages_ten_rows_and_clamp) {
    TitleState state;
    state.screen = TitleScreen(kTitleScreenPlayerData);
    PlayerDataHost host;
    host.count = 23;
    TitlePlayerDataMenu menu{state, host};
    CHECK(reach_interactive(state, host, menu));

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
    menu.play_sound = [&sounds](i32 id) {
        sounds.push_back(id);
        return true;
    };
    CHECK(reach_interactive(state, host, menu));

    CHECK(menu.back());
    CHECK_EQ(state.substate, 3);
    CHECK_EQ(sounds.back(), 9);
    for (i32 i = 0; i < 5; ++i)
        CHECK(menu.update(0, 0));
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
