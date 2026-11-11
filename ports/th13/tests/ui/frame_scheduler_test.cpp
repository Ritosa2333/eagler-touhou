// 调度器与光标单测：FrameScheduler 优先级序/Remove/Stop，MenuCursor 历史/禁用。
#include "../harness/test_framework.hpp"

#include "game/FrameScheduler.hpp"
#include "game/MenuCursor.hpp"

#include <string>

using namespace th13;

TEST(scheduler_runs_by_priority) {
    FrameScheduler scheduler;
    std::string order;
    FrameCallback low, high, middle;
    low.run = high.run = middle.run = nullptr;
    struct Owner {
        std::string* out;
        char tag;
        static i32 step(void* self) {
            auto* owner = static_cast<Owner*>(self);
            *owner->out += owner->tag;
            return i32(FrameAction::Continue);
        }
    } a{&order, 'a'}, b{&order, 'b'}, c{&order, 'c'};
    low.owner = &a; low.run = Owner::step; low.enabled = true;
    middle.owner = &b; middle.run = Owner::step; middle.enabled = true;
    high.owner = &c; high.run = Owner::step; high.enabled = true;
    CHECK(scheduler.add(low, FramePass::Update, 10) == 0);
    CHECK(scheduler.add(high, FramePass::Update, 1) == 0);
    CHECK(scheduler.add(middle, FramePass::Update, 5) == 0);
    CHECK(scheduler.update() == 3);
    CHECK_EQ(order, std::string("cba"));  // 优先级 1(c) → 5(b) → 10(a)
}

TEST(scheduler_remove_and_stop) {
    FrameScheduler scheduler;
    static int runs = 0;
    struct Owner {
        static i32 counting(void*) { ++runs; return i32(FrameAction::Continue); }
        static i32 stopping(void*) { ++runs; return i32(FrameAction::Stop); }
    };
    FrameCallback counter, stopper;
    counter.owner = nullptr; counter.run = Owner::counting; counter.enabled = true;
    stopper.owner = nullptr; stopper.run = Owner::stopping; stopper.enabled = true;
    scheduler.add(counter, FramePass::Update, 1);
    scheduler.add(stopper, FramePass::Update, 2);
    runs = 0;
    CHECK(scheduler.update() == 0);  // Stop 返回 0
    CHECK_EQ(runs, 2);
    scheduler.remove(stopper);
    runs = 0;
    CHECK(scheduler.update() == 1);
    CHECK_EQ(runs, 1);
}

TEST(menu_cursor_select_and_move) {
    MenuCursor menu;
    menu.count = 4;
    menu.select(0);
    CHECK_EQ(menu.cursor, 0);
    menu.move(1);
    CHECK_EQ(menu.cursor, 1);
    menu.move(1);
    menu.move(1);
    CHECK_EQ(menu.cursor, 3);
    menu.move(1);  // wrapping=true 回绕
    CHECK_EQ(menu.cursor, 0);
    menu.move(-1);
    CHECK_EQ(menu.cursor, 3);
}

TEST(menu_cursor_disable_skips) {
    MenuCursor menu;
    menu.count = 3;
    menu.select(0);
    CHECK(menu.disable(1));
    menu.move(1);
    CHECK_EQ(menu.cursor, 2);  // 跳过禁用项 1
    menu.move(1);
    CHECK_EQ(menu.cursor, 0);
}

TEST(menu_cursor_history_push_pop) {
    MenuCursor menu;
    menu.count = 5;
    menu.select(2);
    menu.push();  // 进入子菜单
    menu.count = 4;
    menu.select(0);
    menu.move(1);
    CHECK_EQ(menu.cursor, 1);
    menu.pop();  // 回主菜单
    CHECK_EQ(menu.cursor, 2);
    CHECK_EQ(menu.count, 5);
    CHECK_EQ(menu.disabled_count, 0);  // 子菜单禁用不带回
}

int main() { return th13test::run_all("调度器与光标"); }
