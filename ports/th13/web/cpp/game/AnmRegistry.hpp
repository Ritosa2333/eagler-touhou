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
class AnmRegistry {
    struct Node {
        AnmVm vm;AnmVm* borrowed=nullptr;u32 slot=0x1fff,handle=0;bool active=false,registered=false,alternate=false;
        bool frozen_root=false,pending_collection=false;
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
    u32 ordered_count(bool alternate)const noexcept{return groups[alternate].size();}
    u32& generation_counter()noexcept{return generation;}
};
}
