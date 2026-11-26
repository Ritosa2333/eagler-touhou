// th16 更新/绘制 Chain——原版 DAT_004a6d94（0x401280 构造）。
//
// 条目 0x28 字节（0x401730）：
//   +0x00 priority  +0x04 flags（bit0=堆分配, bit1=启用）
//   +0x08 update/draw 回调  +0x0c 一次性 init（插入时调用后清零）
//   +0x10 删除回调  +0x14..+0x20 链表节（嵌入）  +0x24 context
//
// 更新链头 +0x14/+0x18（0x401300 插入，0x401460 遍历）；
// 绘制链头 +0x3c/+0x40（0x4013b0 插入，0x4015a0 遍历）。
#pragma once
#include "Types.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace th16 {

enum class ChainResult : i32 {
    Remove = 0,   // 从链摘除
    Continue = 1, // 继续下一项
    BreakExit = 3,
    BreakOk = 4,
    BreakNeg = 5,
    Restart = 6,
    Destroy = 7,  // 调用条目的删除回调（+0x10，如 ScreenEffect 0x45d130 自删两条链）后继续
};

using ChainCallback = std::function<ChainResult()>;

struct ChainEntry {
    i32 priority = 0;
    u32 flags = 0;          // bit0 owned, bit1 enabled
    ChainCallback callback;
    ChainCallback init;     // 插入时跑一次
    std::function<void()> on_destroy;  // +0x10：Destroy 返回时调用；为空则直接摘除本条
    void* context = nullptr;
    bool in_update = true;  // true=更新链，false=绘制链
};

class ChainManager {
public:
    static constexpr u32 kFlagOwned = 1u;
    static constexpr u32 kFlagEnabled = 2u;

    // 0x401730 + 0x401300 / 0x4013b0
    ChainEntry* create(ChainCallback cb, i32 priority, bool update_list, void* context = nullptr);
    void enable(ChainEntry* entry);
    void disable(ChainEntry* entry);
    // 0x4017a0
    void remove(ChainEntry* entry);

    // 0x401460：更新链。回调前先记下后继（+0x50），回调中在当前项之前插入的条目本帧不跑，
    // 自删 / 删后继均安全。返回非 Continue 语义汇总（0=空转完, 1=有执行, -1/其它透传 Break）
    i32 run_update();
    // 0x4015a0：绘制链
    i32 run_draw();

    u32 update_count() const { return u32(update_.size()); }
    u32 draw_count() const { return u32(draw_.size()); }

private:
    using EntryList = std::vector<std::shared_ptr<ChainEntry>>;
    i32 run_list(EntryList& list);
    void insert_sorted(EntryList& list, std::shared_ptr<ChainEntry> entry);

    EntryList update_;
    EntryList draw_;
};

}  // namespace th16
