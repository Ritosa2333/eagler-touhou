#pragma once

#include <eagler/netplay/AdonisTiming.hpp>
#include <eagler/netplay/NetplaySession.hpp>
#include <eagler/netplay/PeerTransport.hpp>
#include <array>
#include <limits>

namespace Netplay {
// Pre-frame-zero calibration on the SAME PeerTransport used for gameplay.
// Probe/echo use SendTo (the input lane); negotiation uses reliable control.
// No gameplay input, world snapshot, sleep, new socket or cross-machine clock
// subtraction. The caller keeps resources ready and gameplay paused throughout.
// This first consumer is two-player TH09; do not advertise a 3P policy yet.
class AdonisStartup {
public:
    static constexpr unsigned Warmup = 10, Samples = 120, Attempts = Warmup + Samples;
    static constexpr std::uint32_t Automatic = INVALID_FRAME;
    enum class Stage : std::uint32_t { Idle, WaitingPeer, Measuring, Negotiating, Committed, Failed };
    struct Report {
        std::uint32_t p95Us = 0, received = 0, lost = 0;
        bool operator==(const Report& r) const { return p95Us==r.p95Us && received==r.received && lost==r.lost; }
    };
    struct Choice { unsigned fullDelay = 0, delay = 0, prediction = 0; };

    bool Begin(const SessionConfig& session, std::uint64_t nowUs, AdonisMode mode,
               std::uint32_t requestedDelay, unsigned predictionReserve = 2) {
        *this = AdonisStartup{};
        if (!session.sessionId || !session.gameplayAbi || session.playerCount!=2 || session.localPlayer>1 ||
            (mode!=AdonisMode::Delay && mode!=AdonisMode::Hybrid) ||
            (requestedDelay!=Automatic && requestedDelay>9) || predictionReserve<1 || predictionReserve>2)
            return Fail("Invalid Adonis startup configuration");
        session_=session; mode_=mode; requested_=requestedDelay;
        reserve_=mode==AdonisMode::Hybrid ? predictionReserve : 0;
        begin_=clock_=nowUs; stage_=Stage::WaitingPeer;
        sentAt_.fill(Unsent); rtt_.fill(Unsent); return true;
    }
    static bool IsPacket(const std::uint8_t* p, std::size_t n) {
        return p && n>=3 && p[0]=='A' && p[1]=='D' && p[2]=='S';
    }
    // Send deadlines admit at most ONE fresh probe per pump: a late callback
    // cannot burst many synthetic samples and make a stalled device look fast.
    bool Tick(PeerTransport& transport, std::uint64_t nowUs) {
        if (!ObserveClock(nowUs)) return false;
        if (stage_==Stage::Idle) return Fail("Adonis startup was not begun");
        if (transport.Failed()) return Fail("Adonis startup transport failed");
        if (stage_!=Stage::Committed && nowUs-begin_>=10'000'000)
            return Fail("Adonis startup timed out; no guessed delay was applied");
        if (!transport.IsOpen()) return true;
        if (stage_==Stage::Committed) {
            if (!session_.localPlayer && !commitAck_ && nowUs-committedAt_<5'000'000 && nowUs>=nextControl_) {
                Send(transport, Commit); nextControl_=nowUs+200'000;
            }
            return true;
        }
        if (nowUs>=nextHello_) { Send(transport, Hello); nextHello_=nowUs+200'000; }
        if (peerHello_ && stage_==Stage::WaitingPeer) {
            stage_=Stage::Measuring; nextProbe_=nowUs;
        }
        if (stage_==Stage::Measuring && probes_<Attempts && nowUs>=nextProbe_) {
            const unsigned sequence=probes_++;
            if (Send(transport, Probe, sequence)) sentAt_[sequence]=nowUs;
            nextProbe_+=16'667;
            if(nextProbe_<nowUs)nextProbe_=nowUs+16'667;
            lastProbe_=nowUs;
        }
        if (stage_==Stage::Measuring && probes_==Attempts &&
            (Replies()==Samples || nowUs-lastProbe_>=500'000)) {
            std::array<std::uint64_t,Samples> valid{}; unsigned n=0;
            for (unsigned i=Warmup;i<Attempts;++i) if(rtt_[i]!=Unsent) valid[n++]=rtt_[i];
            // A near-empty fast tail is not a valid automatic estimate. A user
            // chosen D still requires a functioning input lane and handshake.
            if(n<96) return Fail("Too few input-lane samples; retry calibration");
            std::sort(valid.begin(),valid.begin()+n);
            const unsigned at=(n-1)*95/100;
            const auto tail=n%2==0 && at+1<n ? (valid[at]+valid[at+1])/2 : valid[at];
            local_={static_cast<std::uint32_t>(std::max<std::uint64_t>(1,tail)),n,Samples-n};
            haveLocal_=true; stage_=Stage::Negotiating; nextControl_=0;
        }
        if (haveLocal_ && nowUs>=nextControl_) {
            Send(transport, Summary);
            if(havePeer_) {
                choice_=Choose(local_,peer_,mode_,requested_,reserve_);
                if(choice_.delay>9) return Fail("Measured input budget exceeds 9 frames; retry or choose D manually");
                if(!session_.localPlayer) {
                    if(accepted_) {
                        if(Send(transport,Commit)) {stage_=Stage::Committed;committedAt_=nowUs;}
                    } else Send(transport,Proposal);
                } else if(accepted_) Send(transport,Accept);
            }
            nextControl_=nowUs+200'000;
        }
        return true;
    }
    bool Receive(PeerTransport& transport, const std::uint8_t* p, std::size_t n, std::uint64_t nowUs) {
        if(!ObserveClock(nowUs))return false;
        if(!IsPacket(p,n) || n!=Bytes || p[3]!=1 || p[4]<Hello || p[4]>CommitAck)
            return Fail("Malformed Adonis startup packet");
        const auto get=[&](unsigned at,unsigned count=4){std::uint64_t v=0;for(unsigned i=0;i<count;++i)v|=std::uint64_t(p[at+i])<<(i*8);return v;};
        if(get(8,8)!=session_.sessionId)return true; // retired generation
        if(p[5]!=1-session_.localPlayer || p[6]!=unsigned(mode_) || p[7]!=reserve_ ||
           get(16)!=session_.gameplayAbi || get(20)!=session_.seed || get(24)!=requested_)
            return Fail("Adonis startup mode, request, seed or build differs");
        const unsigned kind=p[4],sequence=unsigned(get(28));
        for(unsigned i=44;i<Bytes;++i)if(p[i])return Fail("Adonis startup reserved bytes are nonzero");
        if((kind!=Probe && kind!=Echo && sequence) ||
           (kind<=Echo && (get(32)||get(36)||get(40))))return Fail("Invalid Adonis startup fields");
        if(kind==Hello){peerHello_=true;return true;}
        if(kind==Probe || kind==Echo) {
            if(sequence>=Attempts)return Fail("Invalid Adonis probe sequence");
            if(kind==Probe) {Send(transport,Echo,sequence);return true;}
            if(!haveLocal_ && sequence>=Warmup && sentAt_[sequence]!=Unsent && rtt_[sequence]==Unsent) {
                const auto elapsed=nowUs-sentAt_[sequence];
                if(elapsed<=1'000'000)rtt_[sequence]=elapsed;
            }
            return true; // duplicates/late echoes never rewrite frozen statistics
        }
        if(kind==Summary) {
            const Report report{unsigned(get(32)),unsigned(get(36)),unsigned(get(40))};
            if(!report.p95Us || report.p95Us>1'000'000 || report.received<96 ||
               report.received>Samples || report.received+report.lost!=Samples)
                return Fail("Invalid Adonis measurement summary");
            if(havePeer_ && !(peer_==report))return Fail("Adonis measurement changed after publication");
            if(!havePeer_)nextControl_=0;
            peer_=report;havePeer_=true;return true;
        }
        if(!haveLocal_ || !havePeer_)return true; // re-requested control repairs a lost summary
        const auto expected=Choose(local_,peer_,mode_,requested_,reserve_);
        if(get(32)!=expected.delay || get(36)!=expected.fullDelay || get(40)!=expected.prediction || expected.delay>9)
            return Fail("Adonis chosen delay disagrees with both measured summaries");
        choice_=expected;
        if(kind==Proposal && session_.localPlayer==1) {accepted_=true;Send(transport,Accept);return true;}
        if(kind==Accept && session_.localPlayer==0) {accepted_=true;nextControl_=0;return true;}
        if(kind==Commit && session_.localPlayer==1 && accepted_) {
            stage_=Stage::Committed;committedAt_=nowUs;Send(transport,CommitAck);return true;
        }
        if(kind==CommitAck && session_.localPlayer==0 && stage_==Stage::Committed) {commitAck_=true;return true;}
        return Fail("Invalid Adonis commit authority or phase");
    }
    // RTT is observed on the live INPUT channel at Runtime pump boundaries.
    // RTT/2 remains a symmetric-path ESTIMATE, not measured one-way latency.
    // One explicit frame protects the input-consumption scheduling boundary.
    // This is a modern conservative policy, not an exact Adonis2 reproduction.
    // Prediction reserve is removed from D, never added to it and never used
    // to shrink the rollback history or force prediction when exact input exists.
    static Choice Choose(const Report& a,const Report& b,AdonisMode mode,
                         std::uint32_t request,unsigned reserve) {
        const unsigned full=unsigned((std::uint64_t(std::max(a.p95Us,b.p95Us))*60+1'999'999)/2'000'000)+1;
        const unsigned prediction=mode==AdonisMode::Hybrid?std::min(full,reserve):0;
        return {full,request==Automatic?full-prediction:request,prediction};
    }
    Stage State() const {return stage_;}
    bool Ready() const {return stage_==Stage::Committed;}
    bool Failed() const {return stage_==Stage::Failed;}
    bool Active() const {return stage_!=Stage::Idle;}
    const char* Error() const {return error_;}
    unsigned Probes() const {return probes_;}
    unsigned Replies() const {if(!Active())return 0;unsigned n=0;for(unsigned i=Warmup;i<Attempts;++i)n+=rtt_[i]!=Unsent;return n;}
    const Report& Local() const {return local_;}
    const Report& Peer() const {return peer_;}
    const Choice& Selected() const {return choice_;}
    std::uint32_t Request() const {return requested_;}
private:
    enum Kind : std::uint8_t {Hello=1,Probe,Echo,Summary,Proposal,Accept,Commit,CommitAck};
    static constexpr std::size_t Bytes=64;
    static constexpr auto Unsent=std::numeric_limits<std::uint64_t>::max();
    bool Fail(const char* text){stage_=Stage::Failed;error_=text;return false;}
    bool ObserveClock(std::uint64_t now) {
        if(Failed())return false;
        if(now<clock_)return Fail("Adonis startup clock reversed");
        if(stage_==Stage::Measuring && now-clock_>500'000)
            return Fail("Adonis measurement interrupted; retry with both pages active");
        clock_=now;return true;
    }
    bool Send(PeerTransport& transport,Kind kind,unsigned sequence=0) {
        std::array<std::uint8_t,Bytes> bytes{};
        auto put=[&](unsigned at,std::uint64_t n,unsigned count=4){for(unsigned i=0;i<count;++i)bytes[at+i]=std::uint8_t(n>>(i*8));};
        bytes[0]='A';bytes[1]='D';bytes[2]='S';bytes[3]=1;bytes[4]=kind;
        bytes[5]=session_.localPlayer;bytes[6]=std::uint8_t(mode_);bytes[7]=std::uint8_t(reserve_);
        put(8,session_.sessionId,8);put(16,session_.gameplayAbi);put(20,session_.seed);put(24,requested_);put(28,sequence);
        if(kind==Summary){put(32,local_.p95Us);put(36,local_.received);put(40,local_.lost);}
        else if(kind>=Proposal){put(32,choice_.delay);put(36,choice_.fullDelay);put(40,choice_.prediction);}
        return kind==Probe || kind==Echo ? transport.SendTo(1-session_.localPlayer,bytes.data(),bytes.size()) :
            transport.SendControl(bytes.data(),bytes.size());
    }
    SessionConfig session_{};
    AdonisMode mode_=AdonisMode::Rollback;
    Stage stage_=Stage::Idle;
    unsigned reserve_=0,probes_=0;
    std::uint32_t requested_=Automatic;
    std::uint64_t begin_=0,clock_=0,nextHello_=0,nextControl_=0,nextProbe_=0,lastProbe_=0,committedAt_=0;
    bool peerHello_=false,haveLocal_=false,havePeer_=false,accepted_=false,commitAck_=false;
    std::array<std::uint64_t,Attempts> sentAt_{},rtt_{};
    Report local_{},peer_{};
    Choice choice_{};
    const char* error_="";
};
} // namespace Netplay
