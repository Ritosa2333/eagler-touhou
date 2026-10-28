// AnmManager/AnmRegistry 单测（⑤：生命周期/桶序/中断子树传播/create 坐标语义）。
// 断言依据 docs/plans/anm-pipeline.md §五（0x46f820/0x46f170/0x46f240/0x46fca0）。
#include "anm_test_fixtures.hpp"

#include <cmath>

using namespace th13;

namespace {

struct Fixture {
    th13::GameRandom rng{0};
    th13::AnmEnvironment environment;
    std::unique_ptr<th13::AnmManager> manager;

    Fixture() { manager = std::make_unique<th13::AnmManager>(rng, environment); }

    bool load(const std::vector<th13::u8>& bytes) {
        return manager->load(1, bytes.data(), th13::u32(bytes.size()));
    }
};

TEST(create_first_tick_and_semantics) {
    Fixture fixture;
    std::vector<th13::u8> script;
    th13test::push_insn(script, 300, 0, 0, {0});   // 选精灵 0 → 可见
    th13test::push_insn(script, 304, 0, 0, {5});   // 层 5（∈[3,18] → 0x40000）
    th13test::push_insn(script, 2, 0);             // 停止：保持可见存活
    th13test::push_terminator(script);             // 归档解析要求终止符闭环
    CHECK(fixture.load(th13test::build_simple_anm({script})));

    // create 即首 tick（0x46f820）：time<=0 指令立即生效。
    const th13::u32 handle = fixture.manager->create(1, 0, 7, 0, {100.f, 100.f, 0.f});
    CHECK(handle != 0);
    auto* vm = fixture.manager->registry.find(handle);
    CHECK(vm != nullptr);
    if (!vm) return;  // CHECK 不中断：空指针后续断言直接崩，这里手工挡下
    CHECK(vm->visible);                                  // op300 已执行
    CHECK(vm->sprite != nullptr);
    CHECK_EQ(vm->sprite->source_id, 0u);
    CHECK_EQ(vm->visual.layer, 5);                       // op304 覆盖 create 层
    CHECK(vm->visual.render_flags & 0x200);              // 活动位（th13）
    CHECK(vm->visual.render_flags & 0x40000);            // 层∈[3,18]（op304 语义）

    // create 层语义：layer<21 → 0x40000；layer>=21 → 无。
    std::vector<th13::u8> idle;
    th13test::push_insn(idle, 2, 1);  // t1 停止（首 tick 无可执行指令）
    th13test::push_terminator(idle);
    Fixture other;
    CHECK(other.load(th13test::build_simple_anm({idle})));
    const auto low = other.manager->create(1, 0, 20, 0);
    const auto high = other.manager->create(1, 0, 25, 0);
    CHECK(other.manager->registry.find(low)->visual.render_flags & 0x40000);
    CHECK(!(other.manager->registry.find(high)->visual.render_flags & 0x40000));

    // 坐标偏移旗标（flags bit0）：+320/+16（游戏区 384×448 → ANM 坐标）。
    const auto shifted = other.manager->create(1, 0, -1, 0, {64.f, 80.f, 2.f}, 0.f, true);
    auto* moved = other.manager->registry.find(shifted);
    CHECK_NEAR(moved->visual.translation.x, 384.0, 1e-6);
    CHECK_NEAR(moved->visual.translation.y, 96.0, 1e-6);
    CHECK_NEAR(moved->visual.translation.z, 2.0, 1e-6);
    // anmArg 落变量 10025（vm+0x50，旋转 z 槽）。
    const auto argful = other.manager->create(1, 0, -1, 0, {}, 5.5f);
    CHECK_NEAR(other.manager->registry.find(argful)->variables.rotation.z, 5.5, 1e-6);
}

TEST(handle_generation_and_find) {
    Fixture fixture;
    std::vector<th13::u8> script;
    th13test::push_insn(script, 2, 1);
    th13test::push_terminator(script);
    CHECK(fixture.load(th13test::build_simple_anm({script})));
    const th13::u32 a = fixture.manager->create(1, 0);
    const th13::u32 b = fixture.manager->create(1, 0);
    CHECK(a != 0 && b != 0 && a != b);
    CHECK((a & 0x1fff) != (b & 0x1fff));          // 并存占用不同槽位
    CHECK_EQ(((a >> 13) + 1) & 0x7ffff, (b >> 13) & 0x7ffff);  // 代计数 +1
    CHECK(fixture.manager->registry.find(a) != nullptr);
    CHECK(fixture.manager->registry.find(0x7fffffff) == nullptr);  // 陈旧句柄不命中
}

TEST(update_buckets_and_alternate_clamp) {
    // 桶序（0x46f170 主链 31 桶 / 0x46f240 副链层∉{31,32}→31）。
    Fixture fixture;
    std::vector<th13::u8> main_script, alt_script;
    th13test::push_insn(main_script, 304, 0, 0, {5});
    th13test::push_insn(main_script, 3, 0);   // 停等：常驻
    th13test::push_terminator(main_script);
    th13test::push_insn(alt_script, 304, 0, 0, {3});
    th13test::push_insn(alt_script, 3, 0);
    th13test::push_terminator(alt_script);
    std::vector<std::vector<th13::u8>> scripts{main_script, alt_script};
    CHECK(fixture.load(th13test::build_simple_anm(scripts)));

    CHECK(fixture.manager->create(1, 0, 0, 0) != 0);   // 主链尾（flags=0）
    CHECK(fixture.manager->create(1, 1, 0, 4) != 0);   // 副链尾（bit2）
    CHECK(fixture.manager->update());
    CHECK_EQ(fixture.manager->registry.layer(5).size(), 1u);   // 主链层 5
    CHECK_EQ(fixture.manager->registry.layer(3).size(), 0u);   // 副链层 3 被钳走
    CHECK_EQ(fixture.manager->registry.layer(31).size(), 1u);  // 钳到 31
    // 副链层 32 保留。
    std::vector<th13::u8> alt32;
    th13test::push_insn(alt32, 304, 0, 0, {32});
    th13test::push_insn(alt32, 3, 0);
    th13test::push_terminator(alt32);
    Fixture single;
    CHECK(single.load(th13test::build_simple_anm({alt32})));
    CHECK(single.manager->create(1, 0, 32, 4) != 0);
    CHECK(single.manager->update());
    CHECK_EQ(single.manager->registry.layer(32).size(), 1u);
    // 主链层 31 越界：显式报错（原版为未定义行为，port 收紧）。
    CHECK(single.manager->create(1, 0, 31, 0) != 0);
    CHECK(!single.manager->update());
    CHECK(single.manager->error.find("layer") != std::string::npos);
}

TEST(recycle_on_script_end) {
    // 结束语义：tick 非零 → 管理器回收（0x46f170 → 0x46f690）。
    Fixture fixture;
    std::vector<th13::u8> script;
    th13test::push_insn(script, 402, 0, 0, {0x40000000, 0x40000000});
    th13test::push_insn(script, 1, 2);   // t2 结束
    th13test::push_terminator(script);
    CHECK(fixture.load(th13test::build_simple_anm({script})));
    const th13::u32 handle = fixture.manager->create(1, 0);
    CHECK(handle != 0);
    CHECK_EQ(fixture.manager->registry.count(), 1u);
    CHECK(fixture.manager->update());   // f1
    CHECK_EQ(fixture.manager->registry.count(), 1u);
    CHECK(fixture.manager->update());   // f2：op1 执行 → 返回 1 → 回收
    CHECK_EQ(fixture.manager->registry.count(), 0u);
    CHECK(fixture.manager->registry.find(handle) == nullptr);
    // retire 路径：标 0x20，下一帧不 tick 直接回收。
    const th13::u32 steady = fixture.manager->create(1, 0);
    CHECK(steady != 0);
    th13::u32 held = steady;
    CHECK(fixture.manager->retire(held));
    CHECK_EQ(held, 0u);
    CHECK(fixture.manager->update());
    CHECK_EQ(fixture.manager->registry.count(), 0u);
}

TEST(spawn_inherits_layer_and_child_interrupt) {
    // op500 派生：子 VM 继承父 layer（0046f990:20-22）；中断向子树传播并
    // 立即 tick（0x46fca0）。
    Fixture fixture;
    std::vector<th13::u8> parent, child;
    th13test::push_insn(parent, 500, 0, 0, {1});  // 派生子脚本 1
    th13test::push_insn(parent, 3, 0);            // 停等
    th13test::push_terminator(parent);
    th13test::push_insn(child, 3, 0);             // 先停等
    th13test::push_label(child, 7, 0);            // 中断标签
    th13test::push_insn(child, 402, 0, 0, {0x41100000, 0x41100000});  // scale=(9,9)
    th13test::push_insn(child, 7, 0);             // 返回
    th13test::push_terminator(child);
    CHECK(fixture.load(th13test::build_simple_anm({parent, child})));

    const th13::u32 handle = fixture.manager->create(1, 0, 9, 0);
    CHECK(handle != 0);
    auto* root = fixture.manager->registry.find(handle);
    CHECK(root != nullptr);
    auto children = fixture.manager->registry.children(*root);
    CHECK_EQ(children.size(), 1u);                       // op500 已派生
    CHECK_EQ(children[0]->visual.layer, 9);              // 继承父层
    CHECK(children[0]->rotation_parent == root);         // 父指针直挂本源
    CHECK_NEAR(children[0]->visual.scale.x, 1.0, 1e-6);  // 标签块未执行
    // 中断（立即）：父无标签（回落无），子命中标签 → scale=9。
    CHECK(fixture.manager->interrupt(handle, 7, true));
    CHECK_NEAR(children[0]->visual.scale.x, 9.0, 1e-6);
    CHECK(children[0]->visual.flags & 0x4000);           // op7 返回后子停等
    // detach 派生（op504 路径 spawn_detached）：位置/旋转继承、锚点=父位置。
    auto* detached = fixture.manager->spawn_detached(*root, 1);
    CHECK(detached != nullptr);
    CHECK_EQ(detached->visual.layer, 9);
    CHECK_NEAR(detached->visual.translation.x, root->visual.translation.x, 1e-6);
    CHECK_NEAR(detached->visual.translation.y, root->visual.translation.y, 1e-6);
    CHECK_NEAR(detached->visual.child_anchor.x, root->variables.position.x, 1e-6);
}

TEST(load_validation_and_template_tick) {
    // 装载期模板逐脚本绑定+首 tick：坏脚本在 load 即暴露。
    Fixture fixture;
    std::vector<th13::u8> valid;
    th13test::push_insn(valid, 316, 0);  // 无参指令，模板 tick 停等通过
    th13test::push_terminator(valid);
    CHECK(fixture.load(th13test::build_simple_anm({valid})));
    // 声明长度越过流尾：AnmResource::open 拒收（无终止符闭环）。
    Fixture truncated;
    std::vector<th13::u8> body;
    th13::u8 head[8];
    const th13::i16 opcode = 402;
    const th13::u16 wrong = 16;  // 声明 16 字节但流内只有 8
    std::memcpy(head, &opcode, 2);
    std::memcpy(head + 2, &wrong, 2);
    std::memset(head + 4, 0, 4);
    body.insert(body.end(), head, head + 8);
    th13test::push_terminator(body);
    CHECK(!truncated.load(th13test::build_simple_anm({body})));
    CHECK(truncated.manager->error.find("Invalid animation resource") != std::string::npos);
    // 语义坏脚本（402 需 2 参数却给 0；time=-1 保证模板 tick 即执行到它）：
    // load 报 "Animation template" 错误。
    Fixture semantic;
    std::vector<th13::u8> starved;
    th13test::push_insn(starved, 402, -1);
    th13test::push_terminator(starved);
    CHECK(!semantic.load(th13test::build_simple_anm({starved})));
    CHECK(semantic.manager->error.find("template") != std::string::npos);
}

}  // namespace

int main() { return th13test::run_all("anm-manager"); }
