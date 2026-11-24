#include "PlayerStageClearSlots.hpp"
#include <utility>

namespace th128::player {
namespace {
struct Guard {
    bool& state;
    explicit Guard(bool& value):state(value){state=true;}
    ~Guard(){state=false;}
};
}
bool PlayerStageClearSlots::fail(std::string message,std::string& output){
    if(failure_.empty())failure_=std::move(message);
    output=failure_;return false;
}
bool PlayerStageClearSlots::configure(const std::array<std::array<StageClearSlot,2>,8>& groups,
                                      bool explicitEmpty,Retire retire,Children children,std::string& error){
    error.clear();
    if(configured_||executing_||!failure_.empty()){
        error="player stage-clear slots require fresh explicit configuration";return false;
    }
    if(!retire||!children){error="player stage-clear slots require retire and child-chain bridges";return false;}
    std::uint32_t handles=0;
    for(const auto& group:groups){
        for(const auto& slot:group){
            if(slot.handle==0){
                if(slot.vm){error="zero player stage-clear handle owns a non-null VM";return false;}
            }else{
                if(!slot.vm){error="nonzero player stage-clear handle has no actual VM";return false;}
                ++handles;
            }
        }
    }
    if(handles==0&&!explicitEmpty){
        error="zero player stage-clear handle context must be explicitly declared";return false;
    }
    if(handles!=0&&explicitEmpty){
        error="explicit empty player stage-clear context contains handles";return false;
    }
    groups_=groups;retire_=std::move(retire);children_=std::move(children);
    configuredHandles_=handles;emptyContext_=explicitEmpty;configured_=true;return true;
}
bool PlayerStageClearSlots::clear(std::string& error){
    error.clear();
    if(!failure_.empty())return fail(failure_,error);
    if(!configured_){error="player stage-clear slots are not configured";return false;}
    if(retired_)return true;
    if(executing_){error="player stage-clear slots are reentered";return false;}
    Guard guard(executing_);
    if(emptyContext_){retired_=true;return true;}
    for(const auto& group:groups_){
        for(const auto& slot:group){
            if(slot.handle==0)continue;
            if(!retire_(slot.handle,*slot.vm,3,error))
                return fail(error.empty()?"required player stage-clear retire was rejected":error,error);
            ++retiredHandles_;
            bool rejected=false;
            std::string childError;
            const ChildVisit visit=[&](std::uint32_t handle,anm::VM& vm,std::string& visitError){
                if(!retire_(handle,vm,3,visitError)){rejected=true;return false;}
                ++retiredHandles_;return true;
            };
            if(!children_(slot.handle,*slot.vm,visit,childError)||rejected){
                std::string reason=childError.empty()?error:childError;
                if(reason.empty())reason="required player stage-clear child chain was rejected";
                return fail(std::move(reason),error);
            }
        }
    }
    retired_=true;return true;
}
PlayerStageClearSlotsState PlayerStageClearSlots::state() const{
    return {configured_,emptyContext_,retired_,!failure_.empty(),configuredHandles_,retiredHandles_};
}
}
