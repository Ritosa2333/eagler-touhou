#include "../harness/test_framework.hpp"

#include "game/FrameScheduler.hpp"
#include "game/GameRandom.hpp"
#include "game/ScreenShake.hpp"

#include <string>

using namespace th13;

namespace {
struct TagOwner {
    std::string* out;
    char tag;
    static i32 step(void* self) {
        auto* owner = static_cast<TagOwner*>(self);
        *owner->out += owner->tag;
        return i32(FrameAction::Continue);
    }
};
}

TEST(a_scenery_shake_consumes_exactly_two_visual_draws_per_frame) {
    GameRandom observed(0x1234);
    GameRandom mirror(0x1234);
    ScreenShake shake(4, 6, 6);
    Vec2 offset{-1.f, -1.f};
    for (int frame = 0; frame < 3; ++frame) {
        const u32 index_before = observed.index();
        CHECK(shake.advance(observed, 1.f, offset));
        CHECK_EQ(observed.index(), index_before + 4);
        const float magnitude = 6.f;
        const u32 first = u32(mirror.next()) % 3u;
        const float expected_x = first == 1u ? magnitude : (first == 2u ? -magnitude : 0.f);
        const u32 second = u32(mirror.next()) % 3u;
        const float expected_y = second == 1u ? magnitude : (second == 2u ? -magnitude : 0.f);
        CHECK_EQ(offset.x, expected_x);
        CHECK_EQ(offset.y, expected_y);
        CHECK_EQ(observed.seed(), mirror.seed());
    }
    CHECK(!shake.advance(observed, 1.f, offset));
    CHECK_EQ(observed.index(), 12u);
}

TEST(a_scenery_shake_runs_before_the_run_update) {
    FrameScheduler scheduler;
    std::string order;
    TagOwner run_owner{&order, 'R'}, shake_owner{&order, 'S'};
    FrameCallback run_update, shake;
    run_update.owner = &run_owner;
    run_update.run = TagOwner::step;
    run_update.enabled = true;
    shake.owner = &shake_owner;
    shake.run = TagOwner::step;
    shake.enabled = true;
    CHECK(scheduler.add(run_update, FramePass::Update, 20) == 0);
    CHECK(scheduler.add(shake, FramePass::Update, 16) == 0);
    CHECK(scheduler.update() == 2);
    CHECK_EQ(order, std::string("SR"));
}

TEST(a_scenery_shake_registered_during_update_starts_next_frame) {
    FrameScheduler scheduler;
    std::string order;
    FrameCallback run_update, shake;
    TagOwner shake_owner{&order, 'S'};
    struct Registrar {
        FrameScheduler* scheduler = nullptr;
        FrameCallback* shake = nullptr;
        TagOwner* shake_owner = nullptr;
        std::string* out = nullptr;
        bool registered = false;
        static i32 step(void* self) {
            auto* reg = static_cast<Registrar*>(self);
            if (!reg->registered) {
                reg->registered = true;
                *reg->out += 'r';
                reg->shake->owner = reg->shake_owner;
                reg->shake->run = TagOwner::step;
                reg->shake->enabled = true;
                if (reg->scheduler->add(*reg->shake, FramePass::Update, 16) < 0) return 5;
                return i32(FrameAction::Continue);
            }
            *reg->out += 'R';
            return i32(FrameAction::Continue);
        }
    } registrar{&scheduler, &shake, &shake_owner, &order};
    run_update.owner = &registrar;
    run_update.run = Registrar::step;
    run_update.enabled = true;
    CHECK(scheduler.add(run_update, FramePass::Update, 20) == 0);
    CHECK(scheduler.update() == 1);
    CHECK_EQ(order, std::string("r"));
    order.clear();
    CHECK(scheduler.update() == 2);
    CHECK_EQ(order, std::string("SR"));
}

int main() { return th13test::run_all("A型布景/抖动 RNG 相位"); }
