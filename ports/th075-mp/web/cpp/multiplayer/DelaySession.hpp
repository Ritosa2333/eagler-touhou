#pragma once
#include <eagler/netplay/AdonisStartup.hpp>
#include <eagler/netplay/SessionChannel.hpp>
#include <algorithm>
#include <deque>
#include <string>

namespace th075::multiplayer {
// Transport history and physical capture ownership never belong to game state.
// The delay and rollback policies share transport history and capture ownership.
class DelaySession final:private Netplay::PeerTransport {
    Netplay::PeerTransport& wire;
    Netplay::SessionConfig config{};
    Netplay::RollbackCore core;
    Netplay::SessionGate gate;
    Netplay::SessionChannel channel{*this};
    Netplay::AdonisStartup startup;
    std::deque<std::vector<std::uint8_t>> pending;
    std::uint32_t frame=0;
    bool configured=false;unsigned mode=1,predictionLimit=0;
    std::string error;
    struct Hash {std::uint32_t frame=Netplay::INVALID_FRAME,local=0,remote=0;bool haveLocal=false,haveRemote=false;};
    std::array<Hash,64> hashes{};
    static std::uint64_t read(const std::uint8_t* p,unsigned count){std::uint64_t n=0;for(unsigned i=0;i<count;++i)n|=std::uint64_t(p[i])<<(i*8);return n;}
    bool compare(const Hash& h){
        if(h.haveLocal&&h.haveRemote&&h.local!=h.remote){error="TH075 desync at frame "+std::to_string(h.frame);return false;}
        return true;
    }
    double phaseDebt=0;
    bool fail(const char* message){error=message;return false;}
    bool configure(std::uint64_t now){
        const auto delay=startup.Selected().delay;
        auto agreed=config;
        // Include the committed timing in the ordinary HELLO/READY contract.
        agreed.gameplayAbi^=0x07530000u|(delay<<8)|(mode<<4)|predictionLimit;
        Netplay::CoreConfig c;
        c.sessionId=agreed.sessionId;c.playerCount=2;c.localPlayer=agreed.localPlayer;
        c.inputDelay=std::uint8_t(delay);c.maxRollbackFrames=std::uint8_t(predictionLimit?predictionLimit:1);
        // Like Giuroll's EnemyInputHolder, repeat the last known held gameplay
        // state. Clearing attacks/directions manufactures releases and forces
        // corrections even while the remote player holds a constant input.
        // Escape/cancel/restart remain confirmed lifecycle commands.
        c.allowPrediction=mode==0;c.predictableButtons=0xff;c.directionButtons=0x0f;c.maxDirectionPredictionFrames=std::uint8_t(predictionLimit);
        // The positive-wait Adonis adjustment belongs to delay mode. Applying
        // it to D0 rollback converts the intended prediction lead into delay.
        Netplay::SessionChannelConfig policy;policy.adonisPhase=mode==1;
        if(!core.Reset(c)||!gate.Reset(agreed)||!channel.BeginSession(agreed,now,policy))
            return fail("TH075 session configuration failed");
        configured=true;return true;
    }
    bool IsOpen()const override{return wire.IsOpen();}
    bool Failed()const override{return !error.empty()||wire.Failed();}
    bool Recovering()const override{return wire.Recovering();}
    bool Disconnected()const override{return wire.Disconnected();}
    bool CalibrationSuspended()const override{return wire.CalibrationSuspended();}
    bool SendTo(std::uint8_t p,const std::uint8_t* b,std::size_t n)override{return wire.SendTo(p,b,n);}
    bool SendRepairTo(std::uint8_t p,const std::uint8_t* b,std::size_t n)override{return wire.SendRepairTo(p,b,n);}
    bool SendControl(const std::uint8_t* b,std::size_t n)override{return wire.SendControl(b,n);}
    std::size_t BufferedAmount()const override{return wire.BufferedAmount();}
    bool Poll(std::vector<std::uint8_t>* out)override{
        if(pending.empty())return false;
        *out=std::move(pending.front());pending.pop_front();return true;
    }
public:
    explicit DelaySession(Netplay::PeerTransport& transport):wire(transport){}
    bool Begin(const Netplay::SessionConfig& c,std::uint64_t nowUs,std::uint32_t delay,unsigned timingMode=1,unsigned limit=0){
        if(configured||startup.Active())return fail("TH075 session already configured");
        if(c.gameId!=75||c.playerCount!=2||c.localPlayer>1)return fail("Invalid TH075 seats");
        if(timingMode>1||(timingMode==0&&(limit<1||limit>8||delay!=0))||(timingMode==1&&limit))return fail("Invalid TH075 rollback policy");
        config=c;mode=timingMode;predictionLimit=limit;
        return startup.Begin(c,nowUs,Netplay::AdonisMode::Delay,delay)||fail(startup.Error());
    }
    static bool Valid(const Netplay::FrameInput& in){
        return !(in.buttons&~0x7ffu)&&in.analogMode==Netplay::AnalogMode::None&&
            in.x==0&&in.y==0&&!in.unlimited&&!in.touchUsed&&!in.touchBomb;
    }
    bool Pump(std::uint64_t nowUs){
        if(!error.empty())return false;
        std::vector<std::uint8_t> bytes;
        for(unsigned n=0;n<256&&wire.Poll(&bytes);++n){
            if(bytes.size()==24&&bytes[0]=='P'&&bytes[1]=='7'&&bytes[2]=='5'&&bytes[3]=='H'){
                if(read(bytes.data()+4,8)!=config.sessionId)continue;
                if(read(bytes.data()+20,4)!=config.gameplayAbi)return fail("TH075 state contract mismatch");
                const auto f=std::uint32_t(read(bytes.data()+12,4));auto& h=hashes[(f/60)%hashes.size()];
                if(h.frame!=f){if(h.frame!=Netplay::INVALID_FRAME&&h.frame>f)continue;h={};h.frame=f;}
                const auto value=std::uint32_t(read(bytes.data()+16,4));
                if(h.haveRemote&&h.remote!=value)return fail("Conflicting TH075 state observation");
                h.remote=value;h.haveRemote=true;if(!compare(h))return false;continue;
            }
            if(Netplay::AdonisStartup::IsPacket(bytes.data(),bytes.size())){
                if(!startup.Receive(wire,bytes.data(),bytes.size(),nowUs))return fail(startup.Error());
                continue;
            }
            Netplay::SessionPacket session;
            Netplay::AdonisPhaseSample phase;
            if(Netplay::DecodeAdonisPhaseSample(bytes.data(),bytes.size(),&phase)){
                if(!configured||phase.sessionId!=config.sessionId)continue;
            }else if(Netplay::DecodeSessionPacket(bytes.data(),bytes.size(),&session)){
                if(session.sessionId!=config.sessionId)continue;
            }else{
                Netplay::InputPacket packet;
                if(!configured||!Netplay::DecodeInputPacket(bytes.data(),bytes.size(),&packet))
                    return fail("Unexpected TH075 peer packet");
                if(packet.sessionId!=config.sessionId)continue;
                for(unsigned i=0;i<packet.inputCount;++i)if(!Valid(packet.inputs[i]))
                    return fail("Invalid TH075 logical input");
            }
            if(pending.size()>=512)return fail("TH075 receive backlog exceeded");
            pending.push_back(std::move(bytes));
        }
        if(!startup.Tick(wire,nowUs))return fail(startup.Error());
        if(!configured&&startup.Ready()&&!configure(nowUs/1000))return false;
        if(configured&&!channel.Pump(gate,core,nowUs/1000,Ready()))return fail(channel.ErrorText());
        return true;
    }
    bool Ready()const{return configured&&gate.CanStart()&&error.empty()&&!wire.Disconnected();}
    bool NeedsCapture()const{return Ready()&&!core.HasLocalCapture(frame);}
    bool Capture(std::uint16_t bits,std::uint64_t nowUs){
        if(!NeedsCapture()||!Valid(Netplay::FrameInput(bits)))return fail("Repeated/invalid TH075 capture");
        return (core.ScheduleLocalInput(frame,bits)&&channel.LocalCaptured(core,frame,nowUs/1000))||
            fail("TH075 input capture failed");
    }
    Netplay::FrameDecision Prepare()const{return Ready()?core.PrepareFrame(frame):Netplay::FrameDecision{};}
    bool Complete(const Netplay::FrameDecision& decision){
        if(!decision.canAdvance||(mode==1&&decision.predictedMask)||!core.MarkSimulated(frame,decision))
            return fail("TH075 attempted an unconfirmed frame");
        ++frame;return true;
    }
    bool Verify(std::uint32_t digest){return VerifyAt(frame,digest);}
    bool VerifyAt(std::uint32_t frame,std::uint32_t digest){
        if(frame%60)return true;auto& h=hashes[(frame/60)%hashes.size()];
        if(h.frame!=frame){h={};h.frame=frame;}h.local=digest;h.haveLocal=true;
        if(!compare(h))return false;
        std::vector<std::uint8_t> bytes{'P','7','5','H'};
        const auto put=[&](std::uint64_t n,unsigned count){for(unsigned i=0;i<count;++i)bytes.push_back(std::uint8_t(n>>(i*8)));};
        put(config.sessionId,8);put(frame,4);put(digest,4);put(config.gameplayAbi,4);
        return wire.SendControl(bytes.data(),bytes.size())||!wire.Failed();
    }
    bool Rollback()const{return mode==0;}
    bool HasRollbackRequest()const{return core.HasRollbackRequest();}
    std::uint32_t RollbackFrame()const{return core.RollbackFrame();}
    bool Rewind(std::uint32_t first){if(!core.RewindSimulationTo(first))return fail("TH075 rollback history expired");frame=first;return true;}
    bool ConfirmedInputs(std::uint32_t f,std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>* out)const{return core.ConfirmedInputs(f,out);}
    bool InputConfirmed(std::uint32_t f)const{const auto frontier=core.ConfirmedThroughAllRemotes();return frontier!=Netplay::INVALID_FRAME&&frontier>=f;}
    unsigned Mode()const{return mode;}
    unsigned PredictionLimit()const{return predictionLimit;}
    bool CanFinish()const{return frame&&channel.CanRetire(core,frame-1);}
    bool FlushFinish(std::uint64_t nowUs){return frame&&channel.FlushRetirementFence(core,frame-1,nowUs/1000);}
    std::uint32_t Frame()const{return frame;}
    std::uint32_t Confirmed()const{return core.ConfirmedThroughAllRemotes();}
    unsigned Delay()const{return startup.Selected().delay;}
    const Netplay::AdonisStartup& Startup()const{return startup;}
    const Netplay::SessionConfig& Config()const{return config;}
    const char* Error()const{return error.c_str();}
    bool ConnectionEnded()const{return wire.Disconnected()||channel.Error()==Netplay::SessionChannel::Failure::Transport||
        channel.Error()==Netplay::SessionChannel::Failure::ConfirmedTimeout||channel.Error()==Netplay::SessionChannel::Failure::HandshakeTimeout;}
    double Pace(double elapsed){phaseDebt+=channel.TakeAdonisDelayMs();const auto used=std::min(phaseDebt,elapsed);phaseDebt-=used;return (elapsed-used)/channel.IntervalScale();}
};
}
