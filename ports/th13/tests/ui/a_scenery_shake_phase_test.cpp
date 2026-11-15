// A 型差异（布景/屏幕抖动视觉 RNG 帧内相位）的机制契约测试。
//
// 证据锚点：
//  - 原版 0x45f7c0（屏幕抖动任务，调用点 0x45f82e / 0x45f867）每帧恰好
//    两次 0x472c90（视觉 RNG 0x4dc31c），且这两抽是 native 每帧最先的
//    视觉 RNG 消费（artifacts/round-s15/w2-native-blips/visual-rng-draws.jsonl）；
//  - 任务由 0x470b40 的任务循环 call [esi+8]（返回地址 0x470b68）驱动，
//    与 0x470990 调度同构：port 侧等价物 = FrameScheduler 优先级 16，
//    早于对局 update 20（RunScene::update）。
//  - 对局内 start_shake 在战场 update 内动态注册，因此本帧不抽；下一帧
//    调度开始时（优先级 16）先于对局 update 抽取——与 native 帧首两抽一致。
//
// 本测试只钉上述两个可验证事实，不复制猜测性的相位挪动。
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
}  // namespace

// 0x45f7c0 非 envelope 分支：先 Timer tick，age >= duration 即返回 7（不抽）；
// 否则两次 0x472c90，两次结果各自 %3 → {0, +mag, -mag} 写 offset.x/y。
TEST(a_scenery_shake_consumes_exactly_two_visual_draws_per_frame) {
    GameRandom observed(0x1234);
    GameRandom mirror(0x1234);
    ScreenShake shake(4, 6, 6);  // duration 4，start=end=6 → mag 恒 6
    Vec2 offset{-1.f, -1.f};
    for (int frame = 0; frame < 3; ++frame) {
        const u32 index_before = observed.index();
        CHECK(shake.advance(observed, 1.f, offset));
        // 每帧恰两次 next()（每次 index +2），与 native 两抽/帧一致。
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
    // 第 4 帧 age=4 >= duration=4：返回 false 且不再抽。
    CHECK(!shake.advance(observed, 1.f, offset));
    CHECK_EQ(observed.index(), 12u);
}

// 调度相位：优先级 16（shake）先于 20（对局 update），与注册先后无关。
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
    // 刻意先注册对局 update 再注册 shake：优先级序不依赖注册顺序。
    CHECK(scheduler.add(run_update, FramePass::Update, 20) == 0);
    CHECK(scheduler.add(shake, FramePass::Update, 16) == 0);
    CHECK(scheduler.update() == 2);
    CHECK_EQ(order, std::string("SR"));
}

// 动态注册相位：对局 update 内（start_shake）注册的 shake 本帧不抽，
// 下一帧调度开始时先于对局 update 执行——native 帧首两抽的 port 等价物。
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
                *reg->out += 'r';  // 本帧对局 update（start_shake 注册点）
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
    CHECK_EQ(order, std::string("r"));  // 注册帧只有对局 update，无抖动抽取
    order.clear();
    CHECK(scheduler.update() == 2);
    CHECK_EQ(order, std::string("SR"));  // 下一帧：帧首 shake，再对局 update
}

int main() { return th13test::run_all("A型布景/抖动 RNG 相位"); }
