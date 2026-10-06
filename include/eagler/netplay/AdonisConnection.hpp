#pragma once
#include <eagler/netplay/AdonisStartup.hpp>
#include <deque>

namespace Netplay {
// Adapter between the existing title channel and its actual transport. Keeps
// calibration retries alive after commit, without swallowing native HELLOs
// sent by a peer which received the commit first. No world/input ownership.
class AdonisConnection final : public PeerTransport {
public:
    explicit AdonisConnection(PeerTransport& transport):wire_(transport){}
    void Clear(){startup_=AdonisStartup{};pending_.clear();enabled_=begun_=applied_=failed_=false;}
    void Prepare(const SessionConfig& config,AdonisMode mode,bool automatic,unsigned delay,unsigned reserve){
        Clear();config_=config;mode_=mode;requested_=automatic?AdonisStartup::Automatic:delay;
        reserve_=reserve;enabled_=true;
    }
    bool Pump(std::uint64_t us,bool worldReady){
        now_=us;if(!enabled_)return true;
        if(!begun_){if(!worldReady)return !wire_.Failed();begun_=true;
            if(!startup_.Begin(config_,us,mode_,requested_,reserve_))return false;}
        if(!applied_){std::vector<std::uint8_t> bytes;
            for(unsigned n=0;n<256&&wire_.Poll(&bytes);++n){
                if(AdonisStartup::IsPacket(bytes.data(),bytes.size())){
                    if(!startup_.Receive(wire_,bytes.data(),bytes.size(),us))return false;
                }else {SessionPacket packet;
                    if(DecodeSessionPacket(bytes.data(),bytes.size(),&packet)&&packet.sessionId==config_.sessionId){
                        if(pending_.size()>=16){failed_=true;return false;}
                        pending_.push_back(std::move(bytes));
                    }else {failed_=true;return false;}
                }
            }
        }
        return startup_.Tick(wire_,us);
    }
    bool NeedsApply()const{return enabled_&&!applied_&&startup_.Ready();}
    void Applied(){applied_=true;}
    bool Waiting()const{return enabled_&&!applied_;}
    bool Enabled()const{return enabled_;}
    const AdonisStartup& Startup()const{return startup_;}
    const char* Error()const{return failed_?"Gameplay packet before calibration commit or excessive deferred HELLOs":startup_.Error();}
    // Stable plain-u32 export, shared by all title shells.
    const std::uint32_t* Status(){
        status_.fill(0);status_[0]=1;status_[1]=enabled_?(begun_?unsigned(startup_.State())+1:1):0;
        status_[2]=startup_.Probes();status_[3]=startup_.Replies();status_[4]=config_.playerCount;
        status_[5]=config_.localPlayer;status_[6]=unsigned(mode_);status_[7]=requested_==AdonisStartup::Automatic;
        status_[11]=std::uint32_t(config_.sessionId);status_[31]=startup_.NextWakeUs();
        status_[30]=startup_.Attempt()|(unsigned(startup_.Reason())<<8);
        const auto c=startup_.Selected();status_[8]=c.delay;status_[9]=c.fullDelay;status_[10]=c.prediction;
        for(unsigned p=0;p<config_.playerCount;++p){const auto& r=startup_.ReportFor(p);auto* s=status_.data()+12+p*6;
            s[0]=r.p95Us;s[1]=r.received;s[2]=r.lost;s[3]=r.minUs;s[4]=r.maxUs;s[5]=r.meanUs;}
        return status_.data();
    }
    bool IsOpen()const override{return wire_.IsOpen();}
    bool Recovering()const override{return wire_.Recovering();}
    bool Disconnected()const override{return wire_.Disconnected();}
    bool CalibrationSuspended()const override{return wire_.CalibrationSuspended();}
    bool Failed()const override{return failed_||wire_.Failed()||(enabled_&&begun_&&startup_.Failed());}
    bool SendTo(std::uint8_t p,const std::uint8_t* b,std::size_t n)override{return wire_.SendTo(p,b,n);}
    bool SendRepairTo(std::uint8_t p,const std::uint8_t* b,std::size_t n)override{return wire_.SendRepairTo(p,b,n);}
    bool SendControl(const std::uint8_t* b,std::size_t n)override{return wire_.SendControl(b,n);}
    std::size_t BufferedAmount()const override{return wire_.BufferedAmount();}
    bool Poll(std::vector<std::uint8_t>* bytes)override{
        if(!pending_.empty()){*bytes=std::move(pending_.front());pending_.pop_front();return true;}
        for(unsigned n=0;n<256&&wire_.Poll(bytes);++n){
            if(enabled_&&AdonisStartup::IsPacket(bytes->data(),bytes->size())){
                if(!startup_.Receive(wire_,bytes->data(),bytes->size(),now_)){failed_=true;return false;}
            }else return true;
        }return false;
    }
private:
    PeerTransport& wire_;AdonisStartup startup_;SessionConfig config_{};AdonisMode mode_=AdonisMode::Rollback;
    unsigned requested_=0,reserve_=2;std::uint64_t now_=0;
    bool enabled_=false,begun_=false,applied_=false,failed_=false;
    std::deque<std::vector<std::uint8_t>> pending_;std::array<std::uint32_t,32> status_{};
};
}
