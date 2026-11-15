#include "PlayerDeathHost.hpp"
#include <utility>

namespace th128::player {
namespace {
death::Timer converted(hit::Timer timer){return {timer.previous,timer.current,timer.fractional,timer.clockSource,timer.flags};}
hit::Timer converted(death::Timer timer){return {timer.previous,timer.current,timer.fractional,timer.clockSource,timer.flags};}
struct ExecutionGuard {
    bool& state;
    explicit ExecutionGuard(bool& executing):state(executing){state=true;}
    ~ExecutionGuard(){state=false;}
};
}
DeathOperation::DeathOperation(DeathContext context,int playerState,hit::Timer stateTimer,hit::Timer protection){
    progress_.context=std::move(context);progress_.state=playerState;
    progress_.stateTimer=stateTimer;progress_.protection=protection;
}
bool DeathOperation::fail(std::string message,std::string& error){
    if(failure_.empty())failure_=std::move(message);
    error=failure_;return false;
}
bool DeathOperation::call(const death::CallRequest& request,const DeathCallHost& host,std::string& error){
    progress_.pending=request;
    try {
        if(!host(request,error)||!error.empty()||!failure_.empty())
            return fail(error.empty()?"required death call was rejected":error,error);
    } catch(...){return fail("required death call host threw",error);}
    progress_.pending.reset();return true;
}
bool DeathOperation::run(const DeathCallHost& host,std::string& error){
    error.clear();
    if(executing_)return fail("death operation is not reentrant",error);
    if(started_||!failure_.empty())return fail(failure_.empty()?"death operation cannot be retried":failure_,error);
    started_=true;
    if(!host||progress_.state!=4)return fail("required death call host or explicit state4 is missing",error);
    ExecutionGuard guard(executing_);
    auto& context=progress_.context;
    const auto motivation=death::motivationAfterDeath(context.motivation);
    if(!motivation)return fail("unsupported explicit death motivation domain",error);
    context.motivation=*motivation;
    const auto charge=death::chargeAwardPrefix(context.perfectCharge);
    if(!charge)return fail("unsupported explicit death charge domain",error);
    context.perfectCharge=charge->updatedCharge;
    for(std::uint32_t index=0;index<charge->requestCount;++index)
        if(!call(charge->requiredCalls[index],host,error))return false;
    const auto awards=death::afterAwardsPrefix(context.awards);
    progress_.powerRaw=awards.rawPower;
    if(awards.requiredCall&&!call(*awards.requiredCall,host,error))return false;
    const auto setup=death::stateTwoSetupPrefix(converted(progress_.stateTimer),converted(progress_.protection));
    if(!setup)return fail("unsupported explicit death state timers",error);
    progress_.state=setup->stateF78;progress_.stateTimer=converted(setup->stateTimer);progress_.protection=converted(setup->protectionTimer);
    if(!call(setup->requiredCall,host,error))return false;
    for(std::uint32_t index=0;index<context.records.size();++index){
        progress_.cleanupRecord=index;
        auto& record=context.records[index];
        const auto plan=death::cleanupRecordPrefix(record,death::CleanupEntry::RecordStart);
        if(!plan)return fail("unsupported death cleanup record",error);
        record.active=plan->activeAfter;progress_.unresolvedHandle=plan->requiredHandle;
        if(plan->requiredHandle!=0)return fail("player death cleanup contains unresolved native animation handles",error);
    }
    progress_.field438f8=0;
    const auto tail=death::tailPrefix(context.tail,context.control);
    if(!tail)return fail("unsupported player death tail context",error);
    context.tail=tail->updated;progress_.field438f8=tail->field438f8;progress_.completed=true;
    return true;
}
const DeathCommitResult& DeathOperation::progress() const{return progress_;}
}
