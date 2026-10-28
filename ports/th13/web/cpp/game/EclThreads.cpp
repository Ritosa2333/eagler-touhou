// th13 ECL 线程组实现（锚点见 EclThreads.hpp 文件头）。
#include "EclThreads.hpp"

namespace th13 {

EclThreadsSnapshot EclThreads::snapshot() const {
    EclThreadsSnapshot out;
    out.main = main;
    out.children.reserve(children.size());
    for (const auto& child : children) out.children.push_back(*child);
    return out;
}

bool EclThreads::restore(const EclThreadsSnapshot& source) {
    error.clear();
    if (main.program != source.main.program) {
        error = "ECL checkpoint program changed";
        return false;
    }
    auto* variables = main.variables;
    auto* commands = main.commands;
    auto* random = main.random;
    const auto rebind = [&](EclContext& target, const EclContext& input) {
        target = input;
        target.variables = variables;
        target.commands = commands;
        target.random = random;
        target.threads = this;
    };
    rebind(main, source.main);
    children.clear();
    for (const auto& child : source.children) {
        auto out = std::make_unique<EclContext>();
        rebind(*out, child);
        children.push_back(std::move(out));
    }
    return true;
}

bool EclThreads::start(const char* name, u8 difficulty) {
    if (!main.program) {
        error = "ECL program unavailable";
        return false;
    }
    const i32 id = main.program->find_index(name);
    children.clear();
    main.difficulty = difficulty;
    main.stack_pointer = main.local_base = 0;
    // 查名失败（0x4797d0 返回 -1）→ 主线程立即结束，非错误。
    if (id < 0) {
        main.routine = -1;
        main.instruction_offset = 0;
        main.time = 0;
        main.code = nullptr;
        main.code_size = 0;
        main.instruction = nullptr;
        return true;
    }
    return main.bind(*main.program, id);
}

bool EclThreads::restart(const char* name, u8 difficulty) {
    error.clear();
    main.error.clear();
    main.thread_id = -1;
    main.thread_state = 0;
    main.thread_flags &= ~1u;
    for (auto& channel : main.interpolators) channel.duration = 0;
    main.interpolation_targets.fill(nullptr);
    return start(name, difficulty);
}

// 派生（0x4793e0）：新线程难度拷自调用者，匿名 id=-1；头插链表。
bool EclThreads::spawn(EclContext& source, i32 id, u32 skipped_arguments) {
    auto context = std::make_unique<EclContext>();
    context->thread_id = id;
    context->difficulty = source.difficulty;
    context->variables = source.variables;
    context->commands = source.commands;
    context->random = source.random;
    context->threads = this;
    if (!source.call_into(*context, skipped_arguments)) {
        error = source.error;
        return false;
    }
    children.push_front(std::move(context));
    return true;
}

EclContext* EclThreads::find(i32 id) {  // 0x479480 按 id 查线程
    if (main.thread_id == id) return &main;
    for (auto& child : children)
        if (child->thread_id == id) return child.get();
    return nullptr;
}

void EclThreads::terminate_children() {  // 0x4794b0 killAllAsync
    for (auto& child : children) child->routine = child->instruction_offset = -1;
}

EclContext* EclThreads::at(u32 index) noexcept {
    if (!index) return &main;
    for (auto& child : children)
        if (--index == 0) return child.get();
    return nullptr;
}

// 每帧步进（0x478db0）：主线程先行；子线程按遍历游标步进，当帧新建者不在游标内。
// 主线程结束返回 -1（非错误，宿主自行决定存活）；子线程结束即摘链释放。
int EclThreads::step(float elapsed) {
    auto next = children.begin();
    const int result = main.step(elapsed);
    if (result) {
        error = main.error;
        return result;
    }
    while (next != children.end()) {
        auto current = next++;
        // pauseAsync（+0x11C0 bit0）：暂停线程本帧不执行（resumeAsync 清位后恢复）。
        if ((*current)->thread_flags & 1u) continue;
        const int child_result = (*current)->step(elapsed);
        if (child_result == -2) {
            error = (*current)->error;
            return -2;
        }
        if (child_result) children.erase(current);
    }
    return 0;
}

}  // namespace th13
