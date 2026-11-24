#include "EnemyPhaseRules.hpp"
#include <bit>
#include <cmath>
#include <utility>

namespace th128::enemy {
namespace {
std::int32_t subtract(std::int32_t left,std::int32_t right){
    return std::bit_cast<std::int32_t>(std::uint32_t(left)-std::uint32_t(right));
}
void reset(PhaseTimer& timer){
    if(!(timer.flags&1u)){timer.flags|=1u;timer.clockSource=0x4b6f58;}
    timer.previous=-1;timer.current=0;timer.fraction=0;
}
}

bool registerPhase(std::array<PhaseRecord,8>& records,int slot,std::int32_t threshold,
                   std::int32_t limit,std::string target){
    if(slot<0||slot>=int(records.size()))return false;
    if(threshold>=0&&(target.empty()||target.size()>200||target.find('\0')!=std::string::npos))return false;
    auto& record=records[std::size_t(slot)];record.threshold=threshold;
    if(threshold<0)return true;
    record.limit=limit;record.healthTarget=target;record.timeTarget=std::move(target);return true;
}

std::optional<PhaseDecision> queryPhase(const PhaseContext& context){
    if(!std::isfinite(context.timer.fraction))return std::nullopt;
    PhaseDecision result;result.next=context;auto& next=result.next;
    next.remaining=next.life;next.threshold=0;
    for(auto& record:next.records){
        if(record.threshold<0)continue;
        next.remaining=subtract(next.life,record.threshold);next.threshold=record.threshold;
        if(next.life<=record.threshold){
            next.life=record.threshold;record.threshold=-1;reset(next.timer);
            next.flags&=~0x2000000u;result.target=record.healthTarget;return result;
        }
        break;
    }
    for(auto& record:next.records){
        if(record.threshold<0||record.limit<=0)continue;
        const auto remaining=subtract(record.limit,next.timer.current);
        const auto seconds=remaining/60;
        const auto hundredths=(remaining%60)*100/60;
        next.countdown={std::uint32_t(seconds>99?99:seconds),std::uint32_t(seconds>99?99:hundredths)};
        if(next.timer.current<record.limit)return result;
        if(!next.spell)return std::nullopt;
        next.life=record.threshold;record.threshold=-1;reset(next.timer);next.flags|=0x2000000u;
        if(!(next.spell->flags&0x10u)){
            if(next.spell->flags&1u){
                if(next.spell->age>=60){next.spell->value80=0;next.spell->flags&=0xffffffb9u;}
                else if(next.spell->playerMode!=0)next.spell->flags|=0x40u;
            }
            next.managerWord18=0;
        }
        result.target=record.timeTarget;result.timedOut=true;return result;
    }
    return result;
}
}
