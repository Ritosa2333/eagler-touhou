// th13 ECL 线程组：宿主任内的主线程 + 异步子线程链表（§2.2/§3.3）。
// 原版 0x4793e0 头插节点、0x478db0 每帧遍历（游标先取好 → 新线程下帧参战、
// 跑完即释放摘链）；主线程是宿主内嵌的同规格块，不进链表，不参与自动回收。
#pragma once

#include "EclContext.hpp"

#include <list>
#include <memory>

namespace th13 {

struct EclThreadsSnapshot {
    EclContext main;
    std::vector<EclContext> children;
};

class EclThreads final : public EclThreadHost {
    std::list<std::unique_ptr<EclContext>> children;

public:
    EclContext main;
    explicit EclThreads(EclProgram& program, EclVariables* variables = nullptr, GameRandom* random = nullptr) {
        main.program = &program;
        main.variables = variables;
        main.random = random;
        main.threads = this;
    }
    // 以名字启动主线程（0x4797d0 名字解析；名字缺失 → 主线程立即进入结束态，
    // 与原版查名失败置空游标的行为一致）。
    bool start(const char* name, u8 difficulty);
    bool restart(const char* name, u8 difficulty);
    bool spawn(EclContext& source, i32 id, u32 skipped_arguments) override;
    EclContext* find(i32 id) override;
    void terminate_children() override;
    // 每帧：主线程先步进，随后游标顺序步进子线程（当帧新生者下帧参战），
    // 结束者摘链。返回 0 正常；-1 主线程结束（error 为空）；-2 脚本错误（error 已置位）。
    int step(float elapsed);
    u32 count() const noexcept { return u32(children.size()) + 1; }
    EclContext* at(u32 index) noexcept;
    EclThreadsSnapshot snapshot() const;
    bool restore(const EclThreadsSnapshot& source);
    std::string error;
};

}  // namespace th13
