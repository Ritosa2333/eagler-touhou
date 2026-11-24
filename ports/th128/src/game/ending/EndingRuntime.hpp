#pragma once
#include "host-text-vm-bridge/HostTextVmBridge.hpp"
#include "EndingKind5.hpp"
#include "game/NativeOrdinaryTerminalOwner.hpp"
#include "game/NativeOrdinaryReturn.hpp"
#include "game/OrdinaryTransition.hpp"
#include <bit>
#include <functional>
#include <iomanip>
#include <optional>
#include <sstream>
#include <utility>
namespace th128::ending {
struct RuntimeGate {bool requested=false;int runtimeScene=0,index=-1,stage=0;std::uint64_t generation=0;bool debugTainted=false;std::uint32_t systemFlags=0;};
class Runtime15 {
    hosttext::HostTextVmBridge host_;
    Kind5Fade fade_;
    RuntimeGate gate_;
    std::unique_ptr<th128::record::NativeOrdinaryEndingPermit> ordinaryPermit_;
    std::vector<MediaRequest> media_;
    std::size_t acknowledged_=0;
    std::uint32_t pauseWord_=0;
    std::uint64_t ticks_=0;
    std::vector<std::uint8_t> returnPacket_;
    bool returnStopAcknowledged_=false,returnIssued_=false;
    bool loaded_=false,cleaned_=false,failed_=false,returned_=false;
    int returnScene_=0;
    std::string failure_;
    std::function<std::optional<FadeControls>()> controls_;
    bool fail(std::string reason,std::string& error){if(failure_.empty())failure_=std::move(reason);failed_=true;error=failure_;return false;}
    bool startOwned(RuntimeGate gate,std::span<const std::uint8_t> message,std::span<const std::uint8_t> art,std::span<const std::uint8_t> base,std::span<const std::uint8_t> text,std::function<bool(std::string&)> sound,std::function<std::optional<FadeControls>()> controls,std::string& error){
        gate_=gate;controls_=std::move(controls);
        if(!host_.configureOriginalInput(1,std::move(sound),error))return fail(error,error);
        if(!host_.configureServices([this](const MediaRequest& request,std::string& failure){if(media_.size()>=2||request.serial!=media_.size()+1||!(request.operation==8&&request.durationFrames==0&&media_.empty())&&!(request.operation==5&&request.durationFrames==180&&media_.size()==1)){failure="originalendingmediaqueuecontract";return false;}media_.push_back(request);return true;},[this](int duration,std::string& failure){return fade_.start(duration,failure);},error))return fail(error,error);
        const bool loaded=ordinaryPermit_?host_.loadOrdinary(*ordinaryPermit_,message,art,base,text,error)
            :host_.load(gate.index,message,art,base,text,error);
        if(!loaded)return fail(error,error);
        loaded_=true;return true;

    }
public:
    bool start(RuntimeGate gate,std::span<const std::uint8_t> message,std::span<const std::uint8_t> art,std::span<const std::uint8_t> base,std::span<const std::uint8_t> text,std::function<bool(std::string&)> sound,std::function<std::optional<FadeControls>()> controls,std::string& error){
        static constexpr std::array<int,6> stages{3,5,8,10,13,15};error.clear();
        if(loaded_||cleaned_||failed_||!gate.requested||gate.runtimeScene!=15||gate.index<0||gate.index>5||stages[std::size_t(gate.index)]!=gate.stage||!gate.generation||!gate.debugTainted||!sound||!controls)return fail("actualsameWorldruntime15terminalgaterequired",error);
        return startOwned(gate,message,art,base,text,std::move(sound),std::move(controls),error);
    }
    bool startOrdinary(std::unique_ptr<th128::record::NativeOrdinaryEndingPermit> permit,std::uint32_t flags,
            std::span<const std::uint8_t> message,std::span<const std::uint8_t> art,std::span<const std::uint8_t> base,
            std::span<const std::uint8_t> text,std::function<bool(std::string&)> sound,
            std::function<std::optional<FadeControls>()> controls,std::string& error){
        error.clear();
        if(loaded_||cleaned_||failed_||ordinaryPermit_||!permit||!permit->ready()||(flags&0x2000u)||!sound||!controls)
            return fail("ordinary ending requires its retired typed native owner",error);
        const auto identity=permit->identity();
        const RuntimeGate gate{true,15,int(identity.route),int(identity.stage),identity.generation,false,flags};
        ordinaryPermit_=std::move(permit);
        return startOwned(gate,message,art,base,text,std::move(sound),std::move(controls),error);
    }
    bool tick(message::Input input,std::uint64_t generation,std::string& error){
        error.clear();if(failed_)return fail(failure_,error);
        if(!loaded_||cleaned_||returned_||generation!=gate_.generation||(ordinaryPermit_&&!ordinaryPermit_->ready()))return fail("endingruntime15generationorownershipchanged",error);
        if(acknowledged_!=media_.size())return fail("endingpendingmediarequiresactualconsumercommitbeforetick",error);
        auto controls=controls_();if(!controls)return fail("liveendingtimecontrolownerUNKNOWN",error);controls->pauseWord=pauseWord_;
        if(fade_.state().active&&!fade_.tick(controls,error))return fail(error,error);
        if(!host_.tick(input,error))return fail(error,error);
        ++ticks_;
        if(host_.state().scene.finished){returnScene_=(gate_.systemFlags&0x2000u)?2:16;returned_=true;}
        return true;
    }
    bool acknowledgeMedia(std::uint32_t serial,int operation,std::string& error){error.clear();if(failed_||!loaded_||acknowledged_>=media_.size()||media_[acknowledged_].serial!=serial||media_[acknowledged_].operation!=operation)return fail("exactoriginalendingmediapendingidentityrequired",error);++acknowledged_;return true;}
    bool cleanup(std::string& error){error.clear();if(failed_||!loaded_||!returned_||cleaned_||acknowledged_!=media_.size())return fail("endingcleanuprequiresoriginalfinishandmediaownership",error);if(!host_.reset(error))return fail(error,error);cleaned_=true;loaded_=false;return true;}
    bool tickReturnTail(std::uint64_t generation,std::string& error){error.clear();if(failed_||!cleaned_||!returned_||generation!=gate_.generation||!returnPacket_.empty())return fail("returntailsameownerrequired",error);auto controls=controls_();if(!controls)return fail("returntailcontrolUNKNOWN",error);controls->pauseWord=pauseWord_;if(fade_.state().active&&!fade_.tick(controls,error))return fail(error,error);++ticks_;return true;}
    bool setPause(unsigned value,std::uint64_t generation,std::string& error){
        error.clear();if(failed_||!loaded_&&!cleaned_||generation!=gate_.generation||value>1)return fail("ending pause requires its retained native owner",error);
        pauseWord_=value;if(value&&fade_.state().active){auto controls=controls_();if(!controls)return fail("ending pause controls unavailable",error);
            controls->pauseWord=pauseWord_;if(!fade_.tick(controls,error))return fail(error,error);}
        return true;
    }
    bool ordinaryReturnReady(const stage::Control& control,const stage::OrdinaryTransition& transition)const{
        return !failed_&&!returnIssued_&&ordinaryPermit_&&ordinaryPermit_->ready()&&!gate_.debugTainted&&gate_.runtimeScene==15
            &&returned_&&returnScene_==16&&cleaned_&&!loaded_&&!pauseWord_&&!fade_.state().active
            &&fade_.state().duration==60&&fade_.state().returnCode==7&&ticks_>0
            &&control.current()==15&&control.pending()==15&&!control.gateSet()&&control.sessionConstructed()
            &&media_.size()==2&&acknowledged_==2&&media_[0].serial==1&&media_[0].operation==8&&media_[0].durationFrames==0
            &&media_[1].serial==2&&media_[1].operation==5&&media_[1].durationFrames==180
            &&media_[0].path=="bgm/th128_09.wav"&&media_[1].path==media_[0].path
            &&transition.readyForReturn(unsigned(gate_.generation),ticks_)
            &&ordinaryPermit_->record_->prepared()&&ordinaryPermit_->record_->acknowledged()
            &&ordinaryPermit_->record_->terminalStatsPrepared();
    }
    bool prepareOrdinaryReturn(const stage::Control& control,const stage::OrdinaryTransition& transition,std::string& error){
        error.clear();if(!ordinaryReturnReady(control,transition)||!returnPacket_.empty()){
            error="ordinary stopped-media descriptor requires its actual completed Ending16/cleanup/fade owner";return false;}
        const auto& record=*ordinaryPermit_->record_;const auto& identity=ordinaryPermit_->identity();
        const auto& statistics=record.statistics();
        if(!std::isfinite(statistics.numerator)||statistics.numerator<0||!std::isfinite(statistics.denominator)||statistics.denominator<=0){
            error="ordinary return has no actual retained native statistics";return false;}
        const auto word=[this](unsigned value){for(unsigned i=0;i<4;++i)returnPacket_.push_back(std::uint8_t(value>>(i*8)));};
        const auto wide=[&](std::uint64_t value){word(unsigned(value));word(unsigned(value>>32));};
        word(0x3152544fu);word(1);word(128);word(1);wide(gate_.generation);wide(ticks_);
        word(identity.stage);word(identity.route);word(identity.rank);word(record.terminalContinues());
        word(unsigned(record.terminalScore()));word(record.frames());word(unsigned(record.pending().size()));word(record.digest());
        word(unsigned(media_.size()));word(media_.back().serial);word(unsigned(media_.back().operation));word(unsigned(media_.back().durationFrames));
        wide(std::bit_cast<std::uint64_t>(statistics.numerator));wide(std::bit_cast<std::uint64_t>(statistics.denominator));
        word(unsigned(transition.fade().current));word(unsigned(transition.fade().returnCode));word(unsigned(fade_.state().current));word(unsigned(fade_.state().returnCode));
        word(16);word(1);word(0x394d4350u);word(0);return true;
    }
    std::span<const std::uint8_t> ordinaryReturnPacket()const{return returnPacket_;}
    bool acknowledgeOrdinaryStop(std::span<const std::uint8_t> packet,const stage::Control& control,
            const stage::OrdinaryTransition& transition,std::string& error){
        error.clear();if(returnStopAcknowledged_||!ordinaryReturnReady(control,transition)||returnPacket_.size()!=128
            ||packet.size()!=returnPacket_.size()||!std::equal(packet.begin(),packet.end(),returnPacket_.begin())){
            error="ordinary stopped PCM full-receipt native descriptor identity rejected";return false;}
        returnStopAcknowledged_=true;return true;
    }
    std::unique_ptr<record::NativeOrdinaryReturn> releaseOrdinaryReturn(const stage::Control& control,
            const stage::OrdinaryTransition& transition,std::string& error){
        error.clear();if(!returnStopAcknowledged_||!ordinaryReturnReady(control,transition)){
            error="ordinary return requires exact acknowledged stopped PCM and retained actual Ending16 owner";return {};}
        auto permit=std::unique_ptr<record::NativeOrdinaryReturn>(new record::NativeOrdinaryReturn(
            ordinaryPermit_->identity(),ordinaryPermit_->record_,ordinaryPermit_->replay_,ticks_));
        if(!permit->ready()){error="ordinary sealed return differs from its frozen record/replay/statistics";return {};}
        returnIssued_=true;ordinaryPermit_.reset();return permit;
    }
    std::uint32_t pauseWord()const{return pauseWord_;}
    bool loaded()const{return loaded_;}bool returned()const{return returned_;}bool cleaned()const{return cleaned_;}int returnScene()const{return returnScene_;}
    std::uint64_t ticks()const{return ticks_;}
    bool visualFadeActive()const{return fade_.state().active;}
    const hosttext::BridgeState& state()const{return host_.state();}
    std::string json()const{
        const auto& state=host_.state();std::ostringstream output;output<<std::setprecision(9)<<"{\"loaded\":"<<(loaded_?"true":"false")<<",\"cleaned\":"<<(cleaned_?"true":"false")<<",\"failed\":"<<(failed_?"true":"false")<<",\"originalReturnRequested\":"<<(returned_?"true":"false")<<",\"returnScene\":"<<returnScene_<<",\"index\":"<<gate_.index<<",\"stage\":"<<gate_.stage<<",\"generation\":"<<gate_.generation<<",\"endingTicks\":"<<ticks_<<",\"messageClock\":"<<state.scene.clock<<",\"waitingConfirm\":"<<(state.scene.waitingConfirm?"true":"false")<<",\"waitRemaining\":"<<state.scene.waitRemaining<<",\"executedOp3\":"<<state.consumedOp3<<",\"textColor\":"<<state.scene.textColor<<",\"ordinaryOwner\":"<<(ordinaryPermit_?"true":"false")<<",\"normalClear\":false,\"debugTainted\":"<<(gate_.debugTainted?"true":"false")<<",\"media\":[";
        for(std::size_t index=0;index<media_.size();index++){if(index)output<<',';const auto& request=media_[index];output<<"{\"serial\":"<<request.serial<<",\"operation\":"<<request.operation<<",\"durationFrames\":"<<request.durationFrames<<",\"path\":\""<<request.path<<"\",\"acknowledged\":"<<(index<acknowledged_?"true":"false")<<'}';}const auto& fade=fade_.state();output<<"],\"fade\":{\"active\":"<<(fade.active?"true":"false")<<",\"current\":"<<fade.current<<",\"alpha\":"<<fade.alpha<<",\"returnCode\":"<<fade.returnCode<<"},\"draws\":[";
        bool comma=false;for(const auto& draw:state.scene.draws){if(comma)output<<',';comma=true;output<<"{\"id\":"<<draw.id<<",\"bank\":"<<draw.bank<<",\"script\":"<<draw.script<<",\"sprite\":"<<draw.sprite<<",\"x\":"<<draw.x<<",\"y\":"<<draw.y<<",\"alpha\":"<<draw.alpha<<",\"layer\":"<<draw.layer<<",\"visible\":"<<(draw.visible?"true":"false")<<",\"width\":"<<draw.width<<",\"height\":"<<draw.height<<",\"sx\":"<<draw.sx<<",\"sy\":"<<draw.sy<<",\"anchorX\":"<<draw.anchorX<<",\"anchorY\":"<<draw.anchorY<<",\"u0\":"<<draw.u0<<",\"v0\":"<<draw.v0<<",\"u1\":"<<draw.u1<<",\"v1\":"<<draw.v1<<",\"textureEntry\":"<<draw.textureEntry<<'}';}output<<"],\"texts\":[";
        for(std::size_t slot=0;slot<5;slot++){if(slot)output<<',';const auto& text=state.texts[slot];output<<"{\"slot\":"<<slot<<",\"script\":"<<text.script<<",\"sprite\":"<<text.sprite<<",\"x\":"<<text.x<<",\"y\":"<<text.y<<",\"sx\":"<<text.scaleX<<",\"sy\":"<<text.scaleY<<",\"alpha\":"<<text.alpha<<",\"visible\":"<<(text.visible?"true":"false")<<'}';}output<<"]}";return output.str();
    }
};
}
