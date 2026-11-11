#include "../harness/test_framework.hpp"
#include "game/TitlePlayerDataView.hpp"

#include <fstream>

using namespace th13;
namespace {
struct Host : TitlePlayerDataPlatform {
    bool open_player_data() override { return true; }
    i32 difficulty_count() const override { return 5; }
    i32 character_count() const override { return 4; }
    i32 record_count(i32) const override { return 23; }
    bool record(i32, i32 character, i32 ordinal, TitlePlayerDataRecord& record) const override {
        record = {}; record.slot = ordinal; record.name = "synthetic";
        record.recorded = ordinal != 0; record.character_flagged = character == 2;
        record.normal_captures = ordinal; record.normal_attempts = ordinal + 5;
        return true;
    }
    bool summary(i32, i32, TitlePlayerDataSummary& summary) const override {
        summary = {}; summary.available = summary.history_complete = true;
        summary.play_count = 23; summary.total_frames = 219720; summary.clear_count = 3;
        for (size_t row = 0; row < 10; ++row) {
            auto& value = summary.ranking[row]; value.available = true;
            value.name = "SYNTH"; value.score = 100000 - i32(row) * 10000;
        }
        return true;
    }
};
struct Sink : HudDrawServices {
    std::vector<HudTextDraw> texts;
    bool hud_text(const HudTextDraw& text) override { texts.push_back(text); return true; }
};
std::vector<u8> asset(const char* name) {
    std::ifstream file(std::string("artifacts/reference/raw/") + name,std::ios::binary);
    return {std::istreambuf_iterator<char>(file),{}};
}
}

TEST(summary_menu_uses_original_four_characters_five_ranks_and_summary_then_cards) {
    TitleState state; state.screen = TitleScreen::PlayerData;
    Host host; TitlePlayerDataMenu menu(state,host);
    CHECK(menu.open());
    for (i32 i = 0; i < 7; ++i) CHECK(menu.update(0,0));
    CHECK_EQ(menu.difficulty(),1); CHECK_EQ(menu.character(),0);
    CHECK_EQ(menu.page(),0); CHECK_EQ(menu.page_count(),4);
    CHECK(menu.visible_records().empty()); CHECK(menu.summary_available());
    CHECK(menu.update(1,0)); CHECK_EQ(menu.page(),1);
    CHECK_EQ(menu.visible_records().size(),size_t(10)); CHECK_EQ(menu.visible_records()[0].slot,0);
    CHECK(menu.update(1,0)); CHECK_EQ(menu.page(),2); CHECK_EQ(menu.visible_records()[0].slot,10);
    CHECK(menu.update(1,0)); CHECK_EQ(menu.page(),3); CHECK_EQ(menu.visible_records().size(),size_t(3));
    CHECK(menu.update(1,0)); CHECK_EQ(menu.page(),0);
    CHECK(menu.update(0x40,0)); CHECK_EQ(menu.character(),3);
    CHECK(menu.update(0x80,0)); CHECK_EQ(menu.character(),0);
    for (i32 i = 0; i < 5; ++i) CHECK(menu.update(0,0x20));
    CHECK_EQ(menu.difficulty(),1);
}

TEST(summary_view_creates_original_assets_draws_ranking_stats_and_card_texture_rows) {
    GameRandom random; AnmEnvironment environment; AnmManager manager(random,environment);
    for (const auto& item : {std::pair<i32,const char*>{0,"text.anm"},{2,"ascii.anm"},{16,"title.anm"}}) {
        const auto bytes = asset(item.second);
        CHECK(!bytes.empty()); CHECK(manager.load(item.first,bytes.data(),u32(bytes.size())));
    }
    TitleState state; state.screen = TitleScreen::PlayerData;
    Host host; TitlePlayerDataMenu menu(state,host); TitlePlayerDataView view(manager);
    CHECK(menu.open());
    for (i32 i = 0; i < 7; ++i) CHECK(menu.update(0,0));
    std::vector<std::string> labels;
    const auto raster = [&](AnmVm& vm,u32,const std::string& text,AnmTextAlignment align) {
        CHECK_EQ(i32(align),i32(AnmTextAlignment::Center));
        CHECK(vm.resource == manager.resource(0)); labels.push_back(text); return true;
    };
    CHECK(view.update(menu,raster));
    CHECK_EQ(manager.registry.find(view.character_handle())->source_script,143);
    CHECK_EQ(manager.registry.find(view.difficulty_handle())->source_script,148);
    Sink sink; CHECK(view.draw(menu,sink));
    CHECK_EQ(sink.texts.size(),size_t(13));
    CHECK(sink.texts[0].position.x == 48 && sink.texts[0].position.y == 160);
    // 00453db0 只写 shadow(+0x19174=1) 与 color(+0x19160)，从不写 font(+0x19178)；
    // font 随 004038a0 的全局 memset 保持默认 0，值 1 在原版任何路径不可达。
    CHECK_EQ(sink.texts[0].style.font,0);
    CHECK(sink.texts[9].position.y == 322);
    CHECK(sink.texts[10].text == "       23");
    CHECK(sink.texts[11].text == "  1:01:02");
    CHECK(sink.texts[12].text == "        3");
    CHECK(menu.update(1,0)); CHECK(view.update(menu,raster));
    CHECK_EQ(labels.size(),size_t(10));
    CHECK(labels[0].find("synthetic") == std::string::npos);
    CHECK(labels[1].find("synthetic") != std::string::npos);
    for (i32 row = 0; row < 10; ++row)
        CHECK_EQ(manager.registry.find(view.row_handle(row))->source_script,25+row);
    CHECK(menu.back()); CHECK(view.update(menu,raster));
    view.release();
}

int main() { return th13test::run_all("save_records_summary_view_test"); }
