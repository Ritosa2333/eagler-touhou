#include "Chain.hpp"

#include <algorithm>

namespace th16 {

ChainEntry* ChainManager::create(ChainCallback cb, i32 priority, bool update_list, void* context) {
    auto entry = std::make_unique<ChainEntry>();
    entry->priority = priority;
    entry->flags = kFlagOwned;  // 创建时未启用（与 0x43af60 `& ~2` 一致）
    entry->callback = std::move(cb);
    entry->context = context;
    entry->in_update = update_list;
    ChainEntry* raw = entry.get();
    if (update_list) insert_sorted(update_, std::move(entry));
    else insert_sorted(draw_, std::move(entry));
    return raw;
}

void ChainManager::enable(ChainEntry* entry) {
    if (entry) entry->flags |= kFlagEnabled;
}

void ChainManager::disable(ChainEntry* entry) {
    if (entry) entry->flags &= ~kFlagEnabled;
}

void ChainManager::remove(ChainEntry* entry) {
    if (entry == nullptr) return;
    auto erase_from = [entry](std::vector<std::unique_ptr<ChainEntry>>& list) {
        list.erase(std::remove_if(list.begin(), list.end(),
                                  [entry](const std::unique_ptr<ChainEntry>& p) { return p.get() == entry; }),
                   list.end());
    };
    erase_from(update_);
    erase_from(draw_);
}

void ChainManager::insert_sorted(std::vector<std::unique_ptr<ChainEntry>>& list, std::unique_ptr<ChainEntry> entry) {
    // 0x401300：插入时若有 init 回调则立即调用
    if (entry->init) {
        entry->init();
        entry->init = nullptr;
    }
    const i32 prio = entry->priority;
    auto it = list.begin();
    while (it != list.end() && (*it)->priority < prio) ++it;
    list.insert(it, std::move(entry));
}

i32 ChainManager::run_list(std::vector<std::unique_ptr<ChainEntry>>& list) {
    auto index_of = [&list](const ChainEntry* target) -> i32 {
        for (size_t k = 0; k < list.size(); ++k)
            if (list[k].get() == target) return i32(k);
        return -1;
    };
    i32 ran = 0;
    size_t i = 0;
    while (i < list.size()) {
        ChainEntry* e = list[i].get();
        if (e == nullptr || !e->callback || (e->flags & kFlagEnabled) == 0) {
            ++i;
            continue;
        }
        ChainEntry* next = i + 1 < list.size() ? list[i + 1].get() : nullptr;
        const ChainResult r = e->callback();
        bool restart = false;
        switch (r) {
        case ChainResult::Remove:
            remove(e);
            e = nullptr;
            ++ran;
            break;
        case ChainResult::Destroy: {
            // 删除回调通常会把本条（及配对的绘制链）从链表摘掉并释放
            std::function<void()> destroy = std::move(e->on_destroy);
            if (destroy) destroy();
            else remove(e);
            e = nullptr;
            ++ran;
            break;
        }
        case ChainResult::Restart:
            restart = true;
            break;
        case ChainResult::BreakExit:
            return 1;
        case ChainResult::BreakOk:
            return 0;
        case ChainResult::BreakNeg:
            return -1;
        case ChainResult::Continue:
        default:
            ++ran;
            break;
        }
        if (restart) {
            i = 0;
            ran = 0;
            continue;
        }
        const i32 next_index = next != nullptr ? index_of(next) : -1;
        if (next_index >= 0) {
            i = size_t(next_index);
        } else if (next == nullptr) {
            // 后继原本为空：若回调在尾部追加了条目，原版同样不在本帧执行
            break;
        } else {
            // 后继被回调删除：从当前项之后继续（0x4017a0 会把 +0x50 挪到被删项的后继）
            const i32 self_index = e != nullptr ? index_of(e) : -1;
            i = self_index >= 0 ? size_t(self_index + 1) : i;
        }
    }
    return ran > 0 ? 1 : 0;
}

i32 ChainManager::run_update() { return run_list(update_); }
i32 ChainManager::run_draw() { return run_list(draw_); }

}  // namespace th16
