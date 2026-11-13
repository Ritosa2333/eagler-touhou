// ANM 实例池与双链调度（原版 0x4706c0 分配 / 0x46f3f0 系插入 / 0x46f690 回收）。
// th13 差异（docs/plans/anm-pipeline.md §五）：
//   - 桶布局：主链 31 桶（层 0-30，0x46f170）+ 副链 2 桶（层 31/32，
//     0x46f240 把 ∉{31,32} 的层钳到 31）——共 33 桶，非 th15 的 42 桶；
//   - 双 tick 链合并为单 update 内两遍（先主链后副链）；
//   - 回收规则：render_flags&0x20（待回收）→ 不 tick 直接回收；tick 返回非零
//     → 回收（0x46f170:33-49），无 th15 的 mode==1 自动收集分支；
//   - 句柄 = slot(低 13 位) | generation(高 19 位)（0x46f3f0，代计数跳 0）；
//   - 副链插入后清 render_flags 0xc000（0046f820.c:65,71）。
#pragma once
#include "AnmVm.hpp"
#include <array>
#include <list>
#include <memory>
#include <unordered_map>
namespace th13 {
class AnmRegistry;
// 只读诊断（探针臂）：主链 walk 入口观测（alternate=false 时每帧一次），
// 用于在链被本帧子体追加之前抓取成员序（帧后 dump 无法判定父是否链尾）。
using ChainEntryObserver = void (*)(const AnmRegistry&, bool);
extern ChainEntryObserver chain_entry_observer;
class AnmRegistry {
    struct Node {
        AnmVm vm;AnmVm* borrowed=nullptr;u32 slot=0x1fff,handle=0;bool active=false,registered=false,alternate=false;
        bool frozen_root=false,queued_deletion=false;
        u64 serialization_ordinal=0;
        Node* parent=nullptr;std::list<Node*> children;
        std::list<Node*>::iterator position;
    };
    std::array<std::unique_ptr<Node>,0x1fff> pool;
    std::unordered_map<AnmVm*,Node*> nodes;
    std::unordered_map<Node*,std::unique_ptr<Node>> overflow;
    std::unordered_map<AnmVm*,std::unique_ptr<Node>> external;
    std::vector<u32> free_slots;
    std::array<std::list<Node*>,2> groups;
    std::array<std::vector<AnmVm*>,33> layers;
    std::vector<Node*> pending_deletion;
    u32 generation=0;u32 active_count=0;
    // 池内按槽序列化；池外/内嵌根按创建序，不能依赖分配地址。
    u64 next_serialization_ordinal=1;
    void collect(Node&);void retire(Node&);void erase(Node&);Node* node(AnmVm&)const noexcept;
    bool tick_chain(u32 alternate,GameRandom&,float rate);
public:
    std::string error;
    AnmRegistry();
    AnmVm* allocate();u32 submit(AnmVm&,u32 ordering=0);
    AnmVm* find(u32 handle)const noexcept;
    AnmVm* find_child_script(AnmVm&,i32 script,i32 occurrence)const noexcept;
    bool attach(AnmVm& child,AnmVm& parent);
    std::vector<AnmVm*> children(const AnmVm&)const;
    bool discard(AnmVm&);bool destroy_tree(AnmVm&);
    void requeue_to_back(AnmVm&);  // [f389] 链尾重排（绘制提交序，oracle 重挂同构）
    bool preserve_embedded_children(AnmVm&);
    bool retire_children(AnmVm&);
    void retire_resource(const AnmResource* resource)noexcept;
    bool references_resource(const AnmResource*)const noexcept;
    // 主链、副链各 tick 一遍并归桶（单帧调用一次）。
    bool update(GameRandom&,float rate);
    // 单链一遍（th15 调用面兼容：false=主链 31 桶，true=副链 31/32 桶）。
    bool update_chain(bool alternate,GameRandom&,float rate);
    bool pause_tree(AnmVm&,bool paused);
    // 中断向子树传播并对每个 VM 立即 tick（0x46fca0）。
    bool interrupt(AnmVm&,i32 label,GameRandom&,float rate,bool immediate);
    const std::vector<AnmVm*>& layer(u32 index)const noexcept{static const std::vector<AnmVm*> empty;return index<layers.size()?layers[index]:empty;}
    u32 count()const noexcept{return active_count;}
    u32 handle(const AnmVm&)const noexcept;
    u32 slot(const AnmVm&)const noexcept;
    u32 ordered_handle(bool alternate,u32 index)const noexcept;
    // 绘制专用加入：不进入任何 tick 链，仅把已手动 tick 的 VM 放入本帧
    // 绘制层桶（原版物品管理器的内嵌 VM：item+0x10/+0x5b4，由 0x42e380
    // 直接 0x462450 步进，从不属于 0x46f170 链）。调用点必须在主链 update
    // 重建层桶之后（after_main_update 钩子）。
    void append_draw(AnmVm& vm);
    u32 ordered_count(bool alternate)const noexcept{return groups[alternate].size();}
    u32& generation_counter()noexcept{return generation;}
    // ---- M6：回滚快照面（web/cpp/multiplayer/WorldState 专用）----
    // 快照红线（docs/MP-DEVELOPMENT.md §快照红线）：AnmRegistry 是裸指针/
    // 迭代器/borrowed/父子链的专用 capture/restore——恢复分配顺序、
    // 句柄 generation 一致。SnapshotData 是值语义 POD：VM 按值拷贝（资源/
    // 环境指针指向 bank/宿主，快照存活期不卸载——红线保证），父子链按
    // 快照内序号重建（不 hash/存地址），borrowed 根（外部内嵌 VM，如
    // RunPause overlay 面板）按其宿主对象在快照后仍存活的前提只记录
    // 指针身份（restore 校验仍可 find）。
    struct SnapshotData {
        static constexpr u32 kNoVm = 0xffffffffu;
        struct Entry {
            AnmVm vm;            // 值拷贝（含资源指针——bank 存活期稳定）
            u32 slot = 0x1fff;
            u32 handle = 0;
            u64 serialization_ordinal = 0;
            bool active = false, registered = false, alternate = false;
            bool borrowed = false, frozen_root = false, queued_deletion = false;
            u32 parent = 0xFFFFFFFFu;     // 快照内序号（borrowed 父 = 0xFFFFFFFEu 哨兵）
            u32 creation_parent = kNoVm, rotation_parent = kNoVm;
            std::vector<u32> children;     // 快照内序号
            bool in_overflow = false;     // 池外节点（restore 重新分配）
            u32 chain_index[2] = {0xFFFFFFFFu, 0xFFFFFFFFu};  // 主/副链序号
        };
        std::vector<Entry> entries;
        struct DrawReference {
            u32 entry = kNoVm, embedded = kNoVm;
            const AnmVm* original = nullptr; // restore binding only, never hashed
        };
        std::array<std::vector<DrawReference>, 33> layers;
        std::vector<u32> free_slots;
        u32 generation = 0, active_count = 0;
        u64 next_serialization_ordinal = 1;
        // borrowed 根的宿主身份（AnmVm* 原值；restore 时校验 external 键
        // 仍指向同一宿主对象——外部 VM 由宿主对象持有，快照不拥有）。
        std::vector<AnmVm*> borrowed_roots;
    };
    bool capture(SnapshotData& out,
                 const std::vector<const AnmVm*>& embedded = {}) const;
    // Restore the actual draw order without ticking. Embedded draw-only VMs
    // use their owner's semantic slot and must be rebound through remapped.
    bool restore(const SnapshotData& source,
                 const std::unordered_map<const AnmVm*, AnmVm*>& remapped = {});
    // 快照辅助：VM 在池内的槽号（池外/未分配 = 0x1fff）。
    u32 pool_slot(const AnmVm& vm) const noexcept;
};
}
