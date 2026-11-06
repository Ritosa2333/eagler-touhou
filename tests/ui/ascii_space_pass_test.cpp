#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/AsciiText.hpp"
#include "game/AsciiFrame.hpp"
#include "game/FrameScheduler.hpp"
#include "game/RunScene.hpp"
using namespace th13;
using namespace th13::test;
namespace {
struct Sink : AsciiGlyphServices {
    std::vector<Vec3> positions;
    std::vector<u32> flags;
    std::vector<i32> sprites;
    bool ascii_glyph(AnmVm& vm) override {
        positions.push_back(vm.variables.position);
        flags.push_back(vm.visual.render_flags);
        sprites.push_back(vm.visual.sprite);
        return true;
    }
};
}
TEST(large_countdown_glyph_layout_matches_original_center_and_right_alignment) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager(random, environment);
    CHECK(load_bank(manager, 2, synthetic_anm(302, trivial_scripts(1))));
    // Unicorn executes original 004042a0 up to each glyph submission.
    // For "3.63s": x = 163,193,203,233,263 at anchor224, font7, scale1.
    const char* strings[] = {"3.63s", "10.00s", "0.02s"};
    for (i32 font : {6, 7}) for (i32 alignment : {0, 2})
    for (float scale : {1.f, .6f}) for (const char* value : strings) {
        AsciiText text(manager, environment);
        CHECK(text.initialize(2));
        HudTextStyle style;
        style.font = font;
        style.alignment = alignment;
        style.scale = {scale, scale};
        CHECK(text.enqueue({{224,112,0},style,value}));
        Sink sink;
        CHECK(text.draw(0,sink));
        const std::string str(value);
        const float total = str.size() == 6 ? 152.f : 122.f;
        const float start = 224.f - total * scale * (alignment == 0 ? .5f : 1.f);
        float offset = 0;
        CHECK_EQ(sink.positions.size(),str.size());
        for (size_t i = 0; i < str.size(); ++i) {
            CHECK_NEAR(sink.positions[i].x,start+offset*scale,1e-4);
            CHECK_EQ(sink.positions[i].y,112.f);
            CHECK_EQ(sink.flags[i]&0x400u,font==7?0x400u:0u);
            const char ch = str[i];
            CHECK_EQ(sink.sprites[i],ch=='.'?295:ch=='s'?296:i32(ch)+237);
            offset += ch=='.'?10.f:ch=='s'?22.f:30.f;
        }
    }
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
TEST(ascii_space1_text_consumes_same_frame_at_original_priorities) {
    // 2026-11-05 试玩回归：Clear Bonus/符卡数字（space1）曾永不绘制——端口
    // 生产者@76 晚于 pass1 消费者@51，入队次帧被 AsciiText::update 判死。
    // 原版 0x424f4b/0x403a54：生产者 0x2e=46 先于 pass1 0x2f=47 同帧消费，
    // 队列寿命 0（0x4db2e0 零填充）。本测试用真实注册优先级锁死该顺序。
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager(random, environment);
    CHECK(load_bank(manager, 2, synthetic_anm(302, trivial_scripts(1))));
    AsciiText text(manager, environment);
    CHECK(text.initialize(2));
    Sink sink;
    struct Chain { AsciiText* text; Sink* sink; bool drew = false; } chain{&text, &sink};
    FrameCallback producer{}, consumer{};
    producer.owner = consumer.owner = &chain;
    producer.enabled = consumer.enabled = true;
    producer.run = [](void* p) {
        auto& c = *static_cast<Chain*>(p);
        HudTextStyle style;
        style.font = 2;
        style.coordinate_space = 1;
        return c.text->hud_text({{357, 35, 0}, style, "12,345"})
                   ? i32(FrameAction::Continue) : i32(FrameAction::Error);
    };
    consumer.run = [](void* p) {
        auto& c = *static_cast<Chain*>(p);
        c.drew = c.text->draw(1, *c.sink);
        return c.drew ? i32(FrameAction::Continue) : i32(FrameAction::Error);
    };
    FrameScheduler scheduler;
    CHECK(scheduler.add(producer, FramePass::Draw, kRunTextPriority) >= 0);
    CHECK(scheduler.add(consumer, FramePass::Draw, kAsciiPassPriority[1]) >= 0);
    CHECK(scheduler.draw() > 0);
    CHECK(chain.drew);
    CHECK(!sink.positions.empty());
    // 寿命 0 语义保持：同帧消费完成后，下一次队列 update 即移除该 entry。
    CHECK_EQ(text.count(), 1u);
    text.update();
    CHECK_EQ(text.count(), 0u);
}
int main() { return th13test::run_all("ASCII pass spaces"); }
