#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/AsciiText.hpp"
using namespace th13;
using namespace th13::test;
namespace {
struct Sink : AsciiGlyphServices {
    std::vector<Vec3> positions;
    std::vector<u32> flags;
    bool ascii_glyph(AnmVm& vm) override {
        positions.push_back(vm.variables.position);
        flags.push_back(vm.visual.render_flags);
        return true;
    }
};
}
TEST(ascii_pass_one_preserves_screen_coordinates_without_scene_origin) {
    GameRandom random;
    AnmEnvironment environment;
    environment.screen_offsets[0] = 32;
    environment.screen_offsets[1] = 16;
    AnmManager manager(random, environment);
    CHECK(load_bank(manager, 2, synthetic_anm(302, trivial_scripts(1))));
    AsciiText text(manager, environment);
    CHECK(text.initialize(2));
    HudTextStyle style;
    style.font = 2;
    style.coordinate_space = 1;
    CHECK(text.enqueue({{357,35,0},style," 1000000"}));
    CHECK_EQ(text.entry(0)->text.position.x, 357.f);
    CHECK_EQ(text.entry(0)->text.position.y, 35.f);
    Sink sink;
    CHECK(text.draw(0,sink));
    CHECK(sink.positions.empty());
    CHECK(text.draw(1,sink));
    CHECK_EQ(sink.positions.size(),size_t(7));
    if (!sink.positions.empty()) {
        CHECK_EQ(sink.positions[0].x,364.f);
        CHECK_EQ(sink.positions[0].y,35.f);
        CHECK_EQ(sink.flags[0]&0xc0000u,0u);
    }
}
int main() { return th13test::run_all("ASCII pass spaces"); }
