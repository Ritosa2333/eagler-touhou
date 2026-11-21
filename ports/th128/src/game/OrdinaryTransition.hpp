#pragma once
#include "NativeOrdinaryTerminalOwner.hpp"
#include "ending/EndingKind5.hpp"
#include <sstream>
namespace th128::stage {
// Original global kind5 task (update15/draw75), retained beyond BattleResources.
class OrdinaryTransition {
    ending::Kind5Fade fade_;
    const record::NativeOrdinaryTerminalOwner* owner_=nullptr;
    std::shared_ptr<record::NativeNormalRecord> record_;
    std::shared_ptr<record::NativeRunReplay> replay_;
    record::OrdinaryTerminalIdentity identity_{};
    unsigned continues_=0,lastWorldFrame_=0,requestedFrame_=0;
    std::uint64_t endingTicks_=0;
    bool priority1_=false,ending_=false;
    bool same(unsigned generation)const{
        return owner_&&generation==identity_.generation&&owner_->matchesContinuation(generation,continues_,record_,replay_);
    }
    bool refuse(std::string& error)const{error="ordinary global fade/controller owner or logical order changed";return false;}
    bool update(ending::FadeControls controls,std::string& error){
        error.clear();return !fade_.state().active||fade_.tick(controls,error);
    }
public:
    OrdinaryTransition()=default;
    OrdinaryTransition(const OrdinaryTransition&)=delete;
    OrdinaryTransition& operator=(const OrdinaryTransition&)=delete;
    OrdinaryTransition(OrdinaryTransition&&)=default;
    OrdinaryTransition& operator=(OrdinaryTransition&&)=default;
    bool begin(const record::NativeOrdinaryTerminalOwner& owner,unsigned generation,unsigned continues,
            const std::shared_ptr<record::NativeNormalRecord>& record,const std::shared_ptr<record::NativeRunReplay>& replay,
            unsigned worldFrame,std::string& error){
        error.clear();
        if(owner_||!worldFrame||!owner.matchesContinuation(generation,continues,record,replay))return refuse(error);
        if(!fade_.startTerminal(error))return false;
        owner_=&owner;record_=record;replay_=replay;identity_=owner.identity();continues_=continues;lastWorldFrame_=worldFrame;return true;
    }
    bool tickBattle(unsigned generation,unsigned frame,ending::FadeControls controls,std::string& error){
        if(!same(generation)||ending_||priority1_||requestedFrame_||frame!=lastWorldFrame_+1)return refuse(error);
        if(!update(controls,error))return false;lastWorldFrame_=frame;return true;
    }
    bool tickPaused(unsigned generation,ending::FadeControls controls,std::string& error){
        if(!same(generation)||ending_||requestedFrame_||!controls.pauseWord)return refuse(error);
        return update(controls,error);
    }
    bool requestEnding(unsigned generation,unsigned frame,std::string& error){
        error.clear();if(!same(generation)||ending_||priority1_||requestedFrame_||frame!=lastWorldFrame_)return refuse(error);
        requestedFrame_=frame;return true;
    }
    bool preupdate(unsigned generation,unsigned frame,std::string& error){
        error.clear();if(!same(generation)||ending_||priority1_||!requestedFrame_||frame!=requestedFrame_)return refuse(error);
        priority1_=true;return true;
    }
    bool priority1Ready(unsigned generation,unsigned frame)const{return same(generation)&&priority1_&&!ending_&&requestedFrame_==frame;}
    bool enterEnding(const record::NativeOrdinaryEndingPermit& permit,ending::FadeControls controls,std::string& error){
        error.clear();const auto& identity=permit.identity();
        if(!priority1Ready(identity.generation,lastWorldFrame_)||!permit.ready()||identity.route!=identity_.route
            ||identity.rank!=identity_.rank||identity.stage!=identity_.stage||identity.nativeFrames!=identity_.nativeFrames)return refuse(error);
        if(!update(controls,error))return false;ending_=true;return true;
    }
    bool tickEnding(unsigned generation,std::uint64_t ticks,ending::FadeControls controls,std::string& error){
        if(!same(generation)||!ending_||ticks!=endingTicks_+1)return refuse(error);
        if(!update(controls,error))return false;endingTicks_=ticks;return true;
    }
    bool retirePausedEnding(unsigned generation,ending::FadeControls controls,std::string& error){
        if(!same(generation)||!ending_||!controls.pauseWord)return refuse(error);
        return update(controls,error);
    }
    bool readyForReturn(unsigned generation,std::uint64_t endingTicks)const{
        return same(generation)&&ending_&&priority1_&&endingTicks_==endingTicks&&!fade_.state().active
            &&fade_.state().returnCode==7;
    }
    bool bound()const{return owner_!=nullptr;}
    int elapsed()const{return owner_?fade_.state().current:-1;}
    int alpha()const{return fade_.state().active?fade_.state().alpha:0;}
    const ending::Kind5State& fade()const{return fade_.state();}
    std::string json()const{
        const auto& s=fade_.state();std::ostringstream out;
        out<<"{\"bound\":"<<(bound()?"true":"false")<<",\"generation\":"<<identity_.generation
            <<",\"active\":"<<(s.active?"true":"false")<<",\"kind\":5,\"duration\":"<<s.duration
            <<",\"current\":"<<s.current<<",\"alpha\":"<<alpha()<<",\"returnCode\":"<<s.returnCode
            <<",\"priority1Ready\":"<<(priority1_?"true":"false")<<",\"ending\":"<<(ending_?"true":"false")
            <<",\"clockSource\":4943704,\"updatePriority\":15,\"drawPriority\":75,\"provenance\":\"ordinary-global-kind5\"}";
        return out.str();
    }
};
}
