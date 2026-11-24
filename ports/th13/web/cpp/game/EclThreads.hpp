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
    bool start(const char* name, u8 difficulty);
    bool restart(const char* name, u8 difficulty);
    bool spawn(EclContext& source, i32 id, u32 skipped_arguments) override;
    EclContext* find(i32 id) override;
    void terminate_children() override;
    int step(float elapsed);
    u32 count() const noexcept { return u32(children.size()) + 1; }
    EclContext* at(u32 index) noexcept;
    EclThreadsSnapshot snapshot() const;
    bool restore(const EclThreadsSnapshot& source);
    std::string error;
};

}
