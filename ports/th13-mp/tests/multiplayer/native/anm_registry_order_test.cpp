// 独立原生回归：池外节点的序列化不能随分配地址变化，回滚须恢复创建序。
// 此文件自带 main 和受控分配器，不与公共 test main 链接。
#include "game/AnmRegistry.hpp"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <new>

namespace allocation_order {
// 只将显式指定的下一次 Node 分配置入固定块，构造相反的真实地址序。
// registry 的 map/list/vector 仍走正常分配器，不注入生产钩子。
constexpr std::size_t block_size = 65536;
alignas(std::max_align_t) unsigned char blocks[16][block_size];
int next_block = -1;
void redirect_next(int block) { next_block = block; }
bool owns(void* pointer) {
    const auto value = reinterpret_cast<std::uintptr_t>(pointer);
    const auto begin = reinterpret_cast<std::uintptr_t>(blocks);
    return value >= begin && value < begin + sizeof(blocks);
}
}

void* operator new(std::size_t size) {
    if (allocation_order::next_block >= 0) {
        const int block = allocation_order::next_block;
        allocation_order::next_block = -1;
        if (size > allocation_order::block_size) std::abort();
        return allocation_order::blocks[block];
    }
    if (void* pointer = std::malloc(size ? size : 1)) return pointer;
    std::abort();
}
void operator delete(void* pointer) noexcept {
    if (!allocation_order::owns(pointer)) std::free(pointer);
}
void operator delete(void* pointer, std::size_t) noexcept { ::operator delete(pointer); }

namespace {
using namespace th13;
using Snapshot = AnmRegistry::SnapshotData;
#define CHECK(condition) do { if (!(condition)) { \
    std::printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #condition); return false; \
} } while (false)

// 仅比较此用例的序列化顺序/关系域；真实 VM 数值全摘要由 WorldState 测试负责。
// raw 宿主地址不比较，使用同输入建立的 source_script 标记区分根身份。
bool same_order(const Snapshot& a, const Snapshot& b) {
    if (a.entries.size() != b.entries.size() || a.free_slots != b.free_slots ||
        a.generation != b.generation || a.active_count != b.active_count ||
        a.next_serialization_ordinal != b.next_serialization_ordinal ||
        a.borrowed_roots.size() != b.borrowed_roots.size()) return false;
    for (std::size_t i = 0; i < a.borrowed_roots.size(); ++i)
        if (a.borrowed_roots[i]->source_script != b.borrowed_roots[i]->source_script)
            return false;
    for (std::size_t i = 0; i < a.entries.size(); ++i) {
        const auto& x = a.entries[i]; const auto& y = b.entries[i];
        if (x.slot != y.slot || x.handle != y.handle ||
            x.serialization_ordinal != y.serialization_ordinal ||
            x.active != y.active || x.registered != y.registered ||
            x.alternate != y.alternate || x.borrowed != y.borrowed ||
            x.frozen_root != y.frozen_root || x.queued_deletion != y.queued_deletion ||
            x.parent != y.parent || x.creation_parent != y.creation_parent ||
            x.rotation_parent != y.rotation_parent || x.children != y.children ||
            x.in_overflow != y.in_overflow ||
            x.chain_index[0] != y.chain_index[0] ||
            x.chain_index[1] != y.chain_index[1] ||
            x.vm.source_script != y.vm.source_script) return false;
    }
    return true;
}

bool external_address_order() {
    AnmRegistry a, b;
    std::array<AnmVm, 2> roots_a, roots_b;
    for (u32 i = 0; i < 2; ++i) {
        auto* child_a = a.allocate(); auto* child_b = b.allocate();
        child_a->source_script = child_b->source_script = 100 + i;
        CHECK(a.submit(*child_a) == b.submit(*child_b));
        roots_a[i].source_script = roots_b[i].source_script = 200 + i;
        child_a->creation_parent = child_a->rotation_parent = &roots_a[i];
        child_b->creation_parent = child_b->rotation_parent = &roots_b[i];
        allocation_order::redirect_next(i);  // A: 第一个根地址 < 第二个根地址。
        CHECK(a.attach(*child_a, roots_a[i]));
        CHECK(allocation_order::next_block == -1);
        allocation_order::redirect_next(3 - i);  // B: 第一个根地址 > 第二个根地址。
        CHECK(b.attach(*child_b, roots_b[i]));
        CHECK(allocation_order::next_block == -1);
    }
    Snapshot sa, sb;
    CHECK(a.capture(sa)); CHECK(b.capture(sb));
    CHECK(sa.borrowed_roots[0] != sb.borrowed_roots[0]);
    CHECK(same_order(sa, sb));
    CHECK(sa.entries[2].serialization_ordinal == 1);
    CHECK(sa.entries[3].serialization_ordinal == 2);
    CHECK(sa.next_serialization_ordinal == 3);
    auto wrong = sb;
    std::swap(wrong.entries[2].serialization_ordinal, wrong.entries[3].serialization_ordinal);
    CHECK(!same_order(sa, wrong));
    wrong = sb; ++wrong.next_serialization_ordinal;
    CHECK(!same_order(sa, wrong));
    return true;
}

bool overflow_address_and_restore_order() {
    AnmRegistry a, b;
    for (u32 slot = 0; slot < 0x1fff; ++slot) {
        auto* x = a.allocate(); auto* y = b.allocate();
        CHECK(a.pool_slot(*x) == slot); CHECK(b.pool_slot(*y) == slot);
        x->source_script = y->source_script = slot;
    }
    for (u32 i = 0; i < 2; ++i) {
        allocation_order::redirect_next(4 + i);
        auto* x = a.allocate();
        CHECK(allocation_order::next_block == -1);
        allocation_order::redirect_next(7 - i);
        auto* y = b.allocate();
        CHECK(allocation_order::next_block == -1);
        x->source_script = y->source_script = 300 + i;
        CHECK(a.pool_slot(*x) == 0x1fff); CHECK(b.pool_slot(*y) == 0x1fff);
        CHECK(a.submit(*x) == b.submit(*y));
    }
    Snapshot sa, sb;
    CHECK(a.capture(sa)); CHECK(b.capture(sb));
    CHECK(same_order(sa, sb));
    CHECK(sa.entries[0x1fff].serialization_ordinal == 1);
    CHECK(sa.entries[0x2000].serialization_ordinal == 2);
    // 使 B 的 next 序先偏离，再回滚；下一创建必须与未回滚 A 完全一致。
    auto* speculative = b.allocate();
    CHECK(b.submit(*speculative) != 0);
    CHECK(b.restore(sb));
    allocation_order::redirect_next(8);
    auto* x = a.allocate();
    allocation_order::redirect_next(9);
    auto* y = b.allocate();
    x->source_script = y->source_script = 302;
    CHECK(a.submit(*x) == b.submit(*y));
    CHECK(a.capture(sa)); CHECK(b.capture(sb));
    CHECK(same_order(sa, sb));
    CHECK(sa.entries.back().serialization_ordinal == 3);
    CHECK(sa.next_serialization_ordinal == 4);
    return true;
}

bool pool_reuse_identity() {
    AnmRegistry registry;
    auto* first = registry.allocate(); auto* second = registry.allocate();
    const u32 old_handle = registry.submit(*first);
    const u32 second_handle = registry.submit(*second);
    Snapshot before;
    CHECK(registry.capture(before));
    CHECK(registry.discard(*first));
    auto* reused = registry.allocate();
    CHECK(reused == first); CHECK(registry.pool_slot(*reused) == 0);
    const u32 new_handle = registry.submit(*reused);
    CHECK(new_handle != old_handle); CHECK(registry.find(old_handle) == nullptr);
    CHECK(registry.find(new_handle) == reused);
    auto* speculative = registry.allocate();
    CHECK(registry.pool_slot(*speculative) == 2);
    CHECK(registry.restore(before));
    CHECK(registry.find(old_handle) != nullptr);
    CHECK(registry.find(second_handle) != nullptr);
    CHECK(registry.find(new_handle) == nullptr);
    Snapshot after;
    CHECK(registry.capture(after)); CHECK(same_order(before, after));
    auto* next = registry.allocate();
    CHECK(registry.pool_slot(*next) == 2);
    CHECK(registry.submit(*next) == new_handle + 2);
    CHECK(registry.capture(after));
    CHECK(after.next_serialization_ordinal == 1);
    for (const auto& entry : after.entries) CHECK(entry.serialization_ordinal == 0);
    return true;
}

bool frozen_external_keeps_ordinal() {
    AnmRegistry registry;
    AnmVm root;
    root.source_script = 400;
    auto* child = registry.allocate();
    CHECK(registry.submit(*child) != 0); CHECK(registry.attach(*child, root));
    child->creation_parent = child->rotation_parent = &root;
    CHECK(registry.preserve_embedded_children(root));
    Snapshot snapshot;
    CHECK(registry.capture(snapshot));
    CHECK(snapshot.entries.back().frozen_root);
    CHECK(snapshot.entries.back().serialization_ordinal == 1);
    CHECK(registry.restore(snapshot));
    AnmVm second_root;
    auto* next = registry.allocate();
    CHECK(registry.submit(*next) != 0); CHECK(registry.attach(*next, second_root));
    CHECK(registry.capture(snapshot));
    CHECK(snapshot.entries[snapshot.entries.size() - 2].serialization_ordinal == 1);
    CHECK(snapshot.entries.back().serialization_ordinal == 2);
    CHECK(snapshot.next_serialization_ordinal == 3);
    return true;
}

bool main_and_alternate_chain_positions() {
    AnmRegistry registry;
    std::array<AnmVm*, 8> vms;
    for (u32 i = 0; i < vms.size(); ++i) {
        vms[i] = registry.allocate();
        CHECK(registry.submit(*vms[i], i & 6) != 0);
    }
    registry.requeue_to_back(*vms[0]);
    CHECK(registry.discard(*vms[2]));
    registry.allocate(); // Allocated but unregistered slots have no chain index.
    Snapshot before;
    CHECK(registry.capture(before));
    u32 visited = 0;
    for (u32 chain = 0; chain < 2; ++chain) {
        for (u32 index = 0; const auto handle = registry.ordered_handle(chain, index); ++index) {
            bool found = false;
            for (const auto& entry : before.entries) if (entry.handle == handle) {
                CHECK(entry.registered);
                CHECK(entry.chain_index[chain] == index);
                CHECK(entry.chain_index[1 - chain] == 0xFFFFFFFFu);
                found = true;
                ++visited;
            }
            CHECK(found);
        }
    }
    CHECK(visited == 7);
    for (const auto& entry : before.entries) if (!entry.registered)
        CHECK(entry.chain_index[0] == 0xFFFFFFFFu && entry.chain_index[1] == 0xFFFFFFFFu);
    CHECK(registry.restore(before));
    Snapshot after;
    CHECK(registry.capture(after));
    CHECK(same_order(before, after));
    return true;
}
}

int main() {
    struct Case { const char* name; bool (*run)(); } cases[] = {
        {"external_address_order", external_address_order},
        {"overflow_address_and_restore_order", overflow_address_and_restore_order},
        {"pool_reuse_identity", pool_reuse_identity},
        {"frozen_external_keeps_ordinal", frozen_external_keeps_ordinal},
        {"main_and_alternate_chain_positions", main_and_alternate_chain_positions},
    };
    u32 passed = 0;
    for (const auto& test : cases) {
        const bool ok = test.run();
        std::printf("%s %s\n", ok ? "PASS" : "FAIL", test.name);
        if (ok) ++passed;
    }
    std::printf("%u/%zu passed\n", passed, sizeof(cases) / sizeof(cases[0]));
    return passed == sizeof(cases) / sizeof(cases[0]) ? 0 : 1;
}
