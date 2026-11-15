#pragma once
#include "NativeOrdinaryTerminalOwner.hpp"
#include <cstdint>
namespace th128::ending {class Runtime15;}
namespace th128::postrun {class Menu;}
namespace th128::record {
// Only the retained actual Runtime15 can mint this after its complete native return gates.
class NativeOrdinaryReturn {
    friend class th128::ending::Runtime15;
    friend class th128::postrun::Menu;
    OrdinaryTerminalIdentity identity_;
    std::shared_ptr<NativeNormalRecord> record_;
    std::shared_ptr<NativeRunReplay> replay_;
    std::vector<std::uint8_t> profile_;
    std::uint64_t endingTicks_=0;
    std::int32_t score_=0;
    unsigned continues_=0,digest_=0;
    double numerator_=0,denominator_=0;
    bool consumed_=false;
    NativeOrdinaryReturn(OrdinaryTerminalIdentity identity,std::shared_ptr<NativeNormalRecord> record,
            std::shared_ptr<NativeRunReplay> replay,std::uint64_t ticks)
        :identity_(identity),record_(std::move(record)),replay_(std::move(replay)),profile_(record_->pending()),
         endingTicks_(ticks),score_(record_->terminalScore()),continues_(record_->terminalContinues()),digest_(record_->digest()),
         numerator_(record_->statistics().numerator),denominator_(record_->statistics().denominator){}
public:
    NativeOrdinaryReturn(const NativeOrdinaryReturn&)=delete;
    NativeOrdinaryReturn& operator=(const NativeOrdinaryReturn&)=delete;
    NativeOrdinaryReturn(NativeOrdinaryReturn&&)=default;
    NativeOrdinaryReturn& operator=(NativeOrdinaryReturn&&)=default;
    const OrdinaryTerminalIdentity& identity()const noexcept{return identity_;}
    bool ready()const noexcept{
        return !consumed_&&record_&&replay_&&endingTicks_&&identity_.generation&&identity_.route<6&&identity_.rank<4
            &&record_->started()&&record_->prepared()&&record_->acknowledged()&&record_->terminalStatsPrepared()
            &&record_->route()==identity_.route&&record_->rank()==identity_.rank&&record_->stage()==identity_.stage
            &&record_->terminalScore()==score_&&record_->terminalContinues()==continues_&&record_->digest()==digest_
            &&record_->pending()==profile_&&profile_.size()==52232&&continues_<=9
            &&std::isfinite(numerator_)&&numerator_>=0&&std::isfinite(denominator_)&&denominator_>0
            &&record_->statistics().numerator==numerator_&&record_->statistics().denominator==denominator_
            &&replay_->configuredFor(identity_.stage)&&replay_->stages()==3&&replay_->route()==identity_.route
            &&replay_->rank()==identity_.rank&&replay_->frames()==record_->frames();
    }
};
}
