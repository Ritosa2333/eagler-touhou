#pragma once
#include "game/NativeNormalRecord.hpp"
#include "game/NativeRunReplay.hpp"
#include "game/StageControl.hpp"
#include <memory>
#include <algorithm>
#include <array>
#include <stdexcept>

namespace th128::ending {class Runtime15;}
namespace th128::record {
struct OrdinaryTerminalCarry {
    unsigned token=0,sourceGeneration=0,targetGeneration=0,sourceStage=0,targetStage=0,rank=0,continues=0;
    bool valid=false,targetBound=false,gameRandomRestored=false,scalarRestored=false,acknowledged=false,consumed=false;
    std::shared_ptr<NativeNormalRecord> record;
    std::shared_ptr<NativeRunReplay> replay;
};
struct OrdinaryTerminalBinding {
    unsigned generation=0,worldFrame=0,stage=0,rank=0,continues=0,runFlags=0;
    int controllerCurrent=7,controllerPending=7,sessionField74=0;
    bool stageGate=false,sessionPresent=false,messagesConfigured=false,playerConfigured=false;
    bool debugTainted=false,replayPlayback=false,directDiagnostic=false;
    std::shared_ptr<NativeNormalRecord> record;
    std::shared_ptr<NativeRunReplay> replay;
    OrdinaryTerminalCarry carry;
};
struct OrdinaryTerminalIdentity {
    unsigned generation=0,stage=0,route=0,rank=0,continues=0,settledSelector=0,nativeFrames=0;
};
struct OrdinaryTerminalEvent {
    unsigned generation=0,worldFrame=0,stage=0,rank=0,continues=0,runFlags=0,messageEntry=0,opcode=0;
    int controllerCurrent=0,controllerPending=0,sessionField74=0;
    bool worldTickExecuting=false,stageGate=false,sessionPresent=false,messagesConfigured=false,playerConfigured=false;
    bool debugTainted=false,replayPlayback=false,directDiagnostic=false;
    std::shared_ptr<NativeNormalRecord> record;
    std::shared_ptr<NativeRunReplay> replay;
};
class NativeOrdinaryEndingPermit {
    friend class NativeOrdinaryTerminalOwner;
    friend class th128::ending::Runtime15;
    OrdinaryTerminalIdentity identity_;
    std::shared_ptr<NativeNormalRecord> record_;
    std::shared_ptr<NativeRunReplay> replay_;
    NativeOrdinaryEndingPermit(OrdinaryTerminalIdentity identity,std::shared_ptr<NativeNormalRecord> record,
                              std::shared_ptr<NativeRunReplay> replay)
        :identity_(identity),record_(std::move(record)),replay_(std::move(replay)){}
public:
    NativeOrdinaryEndingPermit(const NativeOrdinaryEndingPermit&)=delete;
    NativeOrdinaryEndingPermit& operator=(const NativeOrdinaryEndingPermit&)=delete;
    const OrdinaryTerminalIdentity& identity()const noexcept{return identity_;}
    bool ready()const noexcept{
        return record_&&replay_&&identity_.generation&&identity_.route<6&&identity_.rank<4
            &&record_->started()&&(!record_->prepared()||record_->acknowledged())
            &&record_->route()==identity_.route&&record_->rank()==identity_.rank&&record_->stage()==identity_.stage
            &&replay_->configuredFor(identity_.stage)&&replay_->stages()==3
            &&replay_->route()==identity_.route&&replay_->rank()==identity_.rank
            &&record_->frames()==replay_->frames()&&record_->frames()>=identity_.nativeFrames;
    }
};

class NativeOrdinaryTerminalOwner {
public:
    using Event=OrdinaryTerminalEvent;
private:
    bool bound_=false,claimed_=false,recorded_=false,endingRetirementAttempted_=false;
    unsigned claimedWorldFrame_=0,claimedNativeFrames_=0,claimedContinues_=0;
    OrdinaryTerminalIdentity identity_{};
    std::shared_ptr<NativeNormalRecord> record_;
    std::shared_ptr<NativeRunReplay> replay_;
    unsigned validateEvent(const Event& e)const{
        if(!bound_||e.generation!=identity_.generation||e.stage!=identity_.stage||e.rank!=identity_.rank
            ||!e.worldFrame||e.continues<identity_.continues||e.continues>9||e.messageEntry!=1||e.opcode!=21
            ||!e.worldTickExecuting||e.debugTainted||e.replayPlayback||e.directDiagnostic||(e.runFlags&0x10u)
            ||e.stageGate||e.controllerCurrent!=7||e.controllerPending!=7||!e.sessionPresent||e.sessionField74!=0
            ||!e.messagesConfigured||!e.playerConfigured||e.record!=record_||e.replay!=replay_)
            throw std::runtime_error("ordinary terminal event lacks its bound natural session");
        if(!record_->started()||record_->prepared()||record_->acknowledged()||record_->stage()!=identity_.stage
            ||record_->route()!=identity_.route||record_->rank()!=identity_.rank
            ||!replay_->configuredFor(identity_.stage)||replay_->stages()!=3||replay_->route()!=identity_.route
            ||replay_->rank()!=identity_.rank||record_->frames()!=replay_->frames()
            ||record_->frames()<identity_.nativeFrames||record_->frames()-identity_.nativeFrames>e.worldFrame)
            throw std::runtime_error("ordinary terminal event differs from its actual recording history");
        return record_->frames();
    }
public:
    NativeOrdinaryTerminalOwner()=default;
    NativeOrdinaryTerminalOwner(const NativeOrdinaryTerminalOwner&)=delete;
    NativeOrdinaryTerminalOwner& operator=(const NativeOrdinaryTerminalOwner&)=delete;
    void bind(const OrdinaryTerminalBinding& b){
        static constexpr std::array<unsigned,6> terminal{3,5,8,10,13,15},previous{2,4,7,9,12,14};
        if(bound_||!b.record||!b.replay||!b.generation||b.worldFrame!=0||b.rank>=4||b.continues>9
            ||b.debugTainted||b.replayPlayback||b.directDiagnostic||(b.runFlags&0x10u)
            ||b.stageGate||b.controllerCurrent!=7||b.controllerPending!=7||!b.sessionPresent
            ||b.sessionField74!=0||!b.messagesConfigured||!b.playerConfigured)
            throw std::runtime_error("ordinary terminal binding lacks its actual normal session");
        const auto selected=std::find(terminal.begin(),terminal.end(),b.stage);
        if(selected==terminal.end())throw std::runtime_error("ordinary terminal stage is outside six original routes");
        const auto route=unsigned(selected-terminal.begin());const auto& c=b.carry;
        if(!c.token||!c.valid||!c.targetBound||!c.gameRandomRestored||!c.scalarRestored||!c.acknowledged||!c.consumed
            ||!c.sourceGeneration||c.sourceGeneration>=b.generation||c.targetGeneration!=b.generation
            ||c.sourceStage!=previous[route]||c.targetStage!=b.stage||c.rank!=b.rank||c.continues!=b.continues
            ||c.record!=b.record||c.replay!=b.replay)
            throw std::runtime_error("ordinary terminal binding differs from restored and acknowledged carry");
        if(!b.record->started()||b.record->prepared()||b.record->acknowledged()||!b.record->frames()
            ||b.record->stage()!=b.stage||b.record->route()!=route||b.record->rank()!=b.rank
            ||!b.replay->configuredFor(b.stage)||b.replay->stages()!=3||b.replay->route()!=route
            ||b.replay->rank()!=b.rank||b.replay->frames()!=b.record->frames())
            throw std::runtime_error("ordinary terminal binding differs from real recorder stage history");
        const OrdinaryTerminalIdentity identity{b.generation,b.stage,route,b.rank,b.continues,route+17,b.record->frames()};
        record_=b.record;replay_=b.replay;identity_=identity;bound_=true;
    }
    void claim(const Event& e){
        if(claimed_)throw std::runtime_error("ordinary terminal event already claimed");
        const auto frames=validateEvent(e);
        claimedWorldFrame_=e.worldFrame;claimedNativeFrames_=frames;claimedContinues_=e.continues;claimed_=true;
    }
    void commitRecord(const Event& e){
        if(!claimed_||recorded_)throw std::runtime_error("ordinary terminal record commit is out of order");
        const auto frames=validateEvent(e);
        if(e.worldFrame!=claimedWorldFrame_||frames!=claimedNativeFrames_||e.continues!=claimedContinues_)
            throw std::runtime_error("ordinary terminal record commit differs from its claimed event");
        record_->commitOrdinaryClear(identity_.stage,identity_.route,identity_.rank);
        recorded_=true;
    }
    bool matchesContinuation(unsigned generation,unsigned continues,const std::shared_ptr<NativeNormalRecord>& record,
                             const std::shared_ptr<NativeRunReplay>& replay)const noexcept{
        return bound_&&claimed_&&recorded_&&generation==identity_.generation&&continues==claimedContinues_
            &&record==record_&&replay==replay_;
    }
    bool prepareStatsForEnding(unsigned generation,unsigned continues,const std::shared_ptr<NativeNormalRecord>& record,
            const std::shared_ptr<NativeRunReplay>& replay,const th128::stage::Control& control,
            std::int32_t score,std::uint64_t timestamp,double seconds,std::string& error){
        error.clear();
        if(endingRetirementAttempted_||!matchesContinuation(generation,continues,record,replay)
            ||!control.initialized()||!control.sessionConstructed()||control.current()!=7||control.pending()!=15
            ||control.gateSet()||!record||!replay){
            error="ordinary terminal statistics lack the claimed pending ending";return false;
        }
        try{record_->prepareOrdinaryTerminalStats(score,continues,timestamp,seconds,replay_->frames());return true;}
        catch(const std::exception& failure){error=failure.what();return false;}
    }
    std::unique_ptr<NativeOrdinaryEndingPermit> retireForEnding(unsigned generation,unsigned continues,
            const std::shared_ptr<NativeNormalRecord>& record,const std::shared_ptr<NativeRunReplay>& replay,
            const th128::stage::Control& control,const std::function<bool()>& retire,std::string& error){
        error.clear();
        if(endingRetirementAttempted_||!matchesContinuation(generation,continues,record,replay)||!retire
            ||!control.initialized()||!control.sessionConstructed()
            ||control.current()!=7||control.pending()!=15||control.gateSet()){
            error="ordinary ending retirement lacks its claimed controller15 owner";return {};
        }
        auto permit=std::unique_ptr<NativeOrdinaryEndingPermit>(new NativeOrdinaryEndingPermit(identity_,record_,replay_));
        if(!permit->ready()){error="ordinary ending retirement differs from its retained record/replay";return {};}
        endingRetirementAttempted_=true;
        try{
            if(!retire()||control.current()!=7||control.pending()!=15||control.gateSet()
                ||!matchesContinuation(generation,continues,record,replay)||!permit->ready()){
                error="ordinary ending actual battle retirement failed or changed its owner";return {};
            }
        }catch(const std::exception& failure){error=failure.what();return {};}
        return permit;
    }
    bool claimed()const noexcept{return claimed_;}
    bool recorded()const noexcept{return recorded_;}
    bool bound()const noexcept{return bound_;}
    const OrdinaryTerminalIdentity& identity()const{if(!bound_)throw std::runtime_error("ordinary terminal data owner absent");return identity_;}
};
}
