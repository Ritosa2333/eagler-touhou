#pragma once
#include "PlayerDeathRules.hpp"
#include "PlayerHitRules.hpp"
#include <functional>
#include <string>

namespace th128::player {
struct DeathContext {
    death::MotivationSnapshot motivation;
    std::int32_t perfectCharge;
    death::AwardControl awards;
    death::TailSnapshot tail;
    death::TailControl control;
    std::array<death::CleanupRecord,8> records;
};
struct DeathCommitResult {
    DeathContext context;
    std::optional<std::int32_t> powerRaw;
    std::int32_t state=0;
    hit::Timer stateTimer,protection;
    std::optional<std::int32_t> field438f8;
    std::optional<death::CallRequest> pending;
    std::uint32_t cleanupRecord=0,unresolvedHandle=0;
    bool completed=false;
};
using DeathCallHost=std::function<bool(const death::CallRequest&,std::string&)>;
class DeathOperation {
public:
    DeathOperation(DeathContext context,int playerState,hit::Timer stateTimer,hit::Timer protection);
    bool run(const DeathCallHost& host,std::string& error);
    const DeathCommitResult& progress() const;
private:
    DeathCommitResult progress_;
    bool started_=false,executing_=false;
    std::string failure_;
    bool fail(std::string message,std::string& error);
    bool call(const death::CallRequest& request,const DeathCallHost& host,std::string& error);
};
}
