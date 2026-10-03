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
// ADS/2 preserves the deployed two-player wire. ADS/3 extends the same owner
// to a three-player input mesh: every required link is sampled, each participant
// publishes its worst-link summary, and the host waits for every acceptance.
class AdonisStartup {
public:
    // Original Adonis2 sends 1..129; slot 0 is unused. Trim slots 0..9,
    // retaining exactly 120 slots (10..129), not 130 transmitted probes.
    static constexpr unsigned Warmup = 10, Samples = 120, Attempts = Warmup + Samples;
    static constexpr unsigned ProbeCount = Attempts - 1, IntervalUs = 16'000, TailUs = 200'000;
    static constexpr unsigned StabilizeUs = 1'000'000;
    static constexpr std::uint64_t PeerWaitUs = 45'000'000, MeasurementTimeoutUs = 10'000'000;
    static constexpr std::uint32_t Automatic = INVALID_FRAME;
    enum class Stage : std::uint32_t { Idle, WaitingPeer, Measuring, Negotiating, Committed, Failed };
    struct Report {
        std::uint32_t p95Us = 0, received = 0, lost = 0, minUs = 0, maxUs = 0, meanUs = 0;
        bool operator==(const Report& r) const { return p95Us==r.p95Us && received==r.received && lost==r.lost && minUs==r.minUs && maxUs==r.maxUs && meanUs==r.meanUs; }
    };
    struct Choice { unsigned fullDelay = 0, delay = 0, prediction = 0; };

    bool Begin(const SessionConfig& session, std::uint64_t nowUs, AdonisMode mode,
               std::uint32_t requestedDelay, unsigned predictionReserve = 2) {
        *this = AdonisStartup{};
        if (!session.sessionId || !session.gameplayAbi || session.playerCount<2 || session.playerCount>3 || session.localPlayer>=session.playerCount ||
            (mode!=AdonisMode::Delay && mode!=AdonisMode::Hybrid) ||
            (requestedDelay!=Automatic && requestedDelay>9) || predictionReserve<1 || predictionReserve>2)
            return Fail("Invalid Adonis startup configuration");
        session_=session; mode_=mode; requested_=requestedDelay;
        reserve_=mode==AdonisMode::Hybrid ? predictionReserve : 0;
        begin_=clock_=nowUs; stage_=Stage::WaitingPeer;
        for(auto& lane:sentAt_)lane.fill(Unsent);
        for(auto& lane:rtt_)lane.fill(Unsent);
        return true;
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
        if (stage_==Stage::WaitingPeer && nowUs-begin_>=PeerWaitUs)
            return Fail("Adonis timed out waiting for all player input channels and loaded worlds");
        if (stage_!=Stage::WaitingPeer && stage_!=Stage::Committed && nowUs-measurementBegin_>=MeasurementTimeoutUs)
            return Fail("Adonis startup timed out; no guessed delay was applied");
        if (!transport.IsOpen()) { if(!probes_)stabilityStarted_=false; return true; }
        if (stage_==Stage::Committed) {
            if (!session_.localPlayer && (commitAckMask_|LocalBit())!=AllMask() && nowUs-committedAt_<5'000'000 && nowUs>=nextControl_) {
                Send(transport, Commit); nextControl_=nowUs+200'000;
            }
            return true;
        }
        if (nowUs>=nextHello_) { Send(transport, Hello); nextHello_=nowUs+200'000; }
        if ((peerHelloMask_|LocalBit())==AllMask() && stage_==Stage::WaitingPeer) {
            stage_=Stage::Measuring;measurementBegin_=nowUs;
        }
        if(stage_==Stage::Measuring && !probes_ && !stabilityStarted_) {
            nextProbe_=nowUs+StabilizeUs;stabilityStarted_=true;
        }
        if (stage_==Stage::Measuring && probes_<ProbeCount && nowUs>=nextProbe_) {
            const unsigned sequence=++probes_;
            for(unsigned peer=0;peer<session_.playerCount;++peer)if(peer!=session_.localPlayer)
                if(Send(transport,Probe,sequence,peer))sentAt_[peer][sequence]=nowUs;
            nextProbe_=nowUs+IntervalUs;
            lastProbe_=nowUs;
        }
        // Sleep(16) also follows the final send in Adonis2, before Sleep(200).
        // Never finish early merely because every reply is already present.
        if (stage_==Stage::Measuring && probes_==ProbeCount && nowUs-lastProbe_>=IntervalUs+TailUs) {
            for(unsigned peer=0;peer<session_.playerCount;++peer)if(peer!=session_.localPlayer){
                std::array<std::uint64_t,Samples> valid{};unsigned n=0;
                for(unsigned i=Warmup;i<Attempts;++i)if(rtt_[peer][i]!=Unsent)valid[n++]=rtt_[peer][i];
                // Every link must be usable; a healthy host link cannot hide
                // a failed P2/P3 input lane. Manual D needs this proof too.
                if(n<96)return Fail("Too few input-lane samples; retry calibration");
                std::sort(valid.begin(),valid.begin()+n);
                const unsigned at=(n-1)*95/100;
                const auto tail=n%2==0&&at+1<n?(valid[at]+valid[at+1])/2:valid[at];
                std::uint64_t sum=0;for(unsigned i=0;i<n;++i)sum+=valid[i];
                links_[peer]={static_cast<std::uint32_t>(tail),n,Samples-n,
                    static_cast<std::uint32_t>(valid[0]),static_cast<std::uint32_t>(valid[n-1]),static_cast<std::uint32_t>(sum/n)};
                if(links_[peer].p95Us>local_.p95Us){local_=links_[peer];worstPeer_=peer;}
            }
            reports_[session_.localPlayer]=local_;summaryMask_|=LocalBit();
            haveLocal_=true; stage_=Stage::Negotiating; nextControl_=0;
        }
        if (haveLocal_ && nowUs>=nextControl_) {
            Send(transport, Summary);
            if(summaryMask_==AllMask()) {
                choice_=ChooseAll();
                if(choice_.delay>9) return Fail("Measured input budget exceeds 9 frames; retry or choose D manually");
                if(!session_.localPlayer) {
                    if((acceptedMask_|LocalBit())==AllMask()) {
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
        if(!IsPacket(p,n) || n!=Bytes || p[3]!=(session_.playerCount==2?2:3) || p[4]<Hello || p[4]>CommitAck)
            return Fail("Malformed Adonis startup packet");
        const auto get=[&](unsigned at,unsigned count=4){std::uint64_t v=0;for(unsigned i=0;i<count;++i)v|=std::uint64_t(p[at+i])<<(i*8);return v;};
        if(get(8,8)!=session_.sessionId)return true; // retired generation
        const unsigned peer=p[5];
        if(peer>=session_.playerCount || peer==session_.localPlayer || p[6]!=unsigned(mode_) || p[7]!=reserve_ ||
           get(16)!=session_.gameplayAbi || get(20)!=session_.seed || get(24)!=requested_)
            return Fail("Adonis startup mode, request, seed or build differs");
        const unsigned kind=p[4],sequence=unsigned(get(28));
        if(session_.playerCount==3 && (p[56]!=3 || p[57]!=(kind==Probe||kind==Echo?session_.localPlayer:255)))
            return Fail("Adonis startup player count or target differs");
        for(unsigned i=kind==Summary?56:44;i<Bytes;++i){
            if(session_.playerCount==3&&(i==56||i==57))continue;
            if(p[i])return Fail("Adonis startup reserved bytes are nonzero");
        }
        if((kind!=Probe && kind!=Echo && sequence) ||
           (kind<=Echo && (get(32)||get(36)||get(40))))return Fail("Invalid Adonis startup fields");
        if(kind==Hello){peerHelloMask_|=1u<<peer;return true;}
        if(kind==Probe || kind==Echo) {
            if(!sequence || sequence>=Attempts)return Fail("Invalid Adonis probe sequence");
            if(kind==Probe) {Send(transport,Echo,sequence,peer);return true;}
            if(!haveLocal_ && sequence>=Warmup && sentAt_[peer][sequence]!=Unsent && rtt_[peer][sequence]==Unsent) {
                const auto elapsed=nowUs-sentAt_[peer][sequence];
                if(elapsed && elapsed<=1'000'000)rtt_[peer][sequence]=elapsed;
            }
            return true; // duplicates/late echoes never rewrite frozen statistics
        }
        if(kind==Summary) {
            const Report report{unsigned(get(32)),unsigned(get(36)),unsigned(get(40)),unsigned(get(44)),unsigned(get(48)),unsigned(get(52))};
            if(!report.p95Us || report.p95Us>1'000'000 || report.received<96 ||
               report.received>Samples || report.received+report.lost!=Samples || !report.minUs ||
               report.maxUs>1'000'000 || report.minUs>report.p95Us || report.p95Us>report.maxUs ||
               report.meanUs<report.minUs || report.meanUs>report.maxUs)
                return Fail("Invalid Adonis measurement summary");
            if((summaryMask_&(1u<<peer)) && !(reports_[peer]==report))return Fail("Adonis measurement changed after publication");
            if(!(summaryMask_&(1u<<peer)))nextControl_=0;
            reports_[peer]=report;summaryMask_|=1u<<peer;return true;
        }
        if(!haveLocal_ || summaryMask_!=AllMask())return true; // retries repair a lost summary
        const auto expected=ChooseAll();
        if(get(32)!=expected.delay || get(36)!=expected.fullDelay || get(40)!=expected.prediction || expected.delay>9)
            return Fail("Adonis chosen delay disagrees with both measured summaries");
        choice_=expected;
        // Reliable control is mesh-broadcast. Other clients' host-directed
        // accepts/ACKs are observable but never grant authority on this client.
        if(session_.localPlayer!=0 && peer!=0 && (kind==Accept||kind==CommitAck))return true;
        if(kind==Proposal && session_.localPlayer!=0 && peer==0) {accepted_=true;Send(transport,Accept);return true;}
        if(kind==Accept && session_.localPlayer==0) {acceptedMask_|=1u<<peer;nextControl_=0;return true;}
        if(kind==Commit && session_.localPlayer!=0 && peer==0 && accepted_) {
            stage_=Stage::Committed;committedAt_=nowUs;Send(transport,CommitAck);return true;
        }
        if(kind==CommitAck && session_.localPlayer==0 && stage_==Stage::Committed) {commitAckMask_|=1u<<peer;return true;}
        return Fail("Invalid Adonis commit authority or phase");
    }
    // RTT is observed on the live INPUT channel at Runtime pump boundaries.
    // RTT/2 remains a symmetric-path ESTIMATE, not measured one-way latency.
    // Adonis2 converts RTT/2 to the first 60 Hz frame boundary that contains it.
    // No additional input queue frame is added.
    // Prediction reserve is removed from D, never added to it and never used
    // to shrink the rollback history or force prediction when exact input exists.
    static Choice Choose(const Report& a,const Report& b,AdonisMode mode,
                         std::uint32_t request,unsigned reserve) {
        const unsigned full=std::max(1u,unsigned((std::uint64_t(std::max(a.p95Us,b.p95Us))/2*60+999'999)/1'000'000));
        // Automatic hybrid targets one queued frame, saving at most two.
        // Manual D remains authoritative, including an explicit zero.
        const unsigned available=full-(request==Automatic?1u:0u);
        const unsigned prediction=mode==AdonisMode::Hybrid?std::min(available,std::min(reserve,2u)):0;
        return {full,request==Automatic?full-prediction:request,prediction};
    }
    Stage State() const {return stage_;}
    AdonisMode Mode()const{return mode_;}
    bool Ready() const {return stage_==Stage::Committed;}
    bool Failed() const {return stage_==Stage::Failed;}
    bool Active() const {return stage_!=Stage::Idle;}
    const char* Error() const {return error_;}
    unsigned Probes() const {return probes_;}
    unsigned NextWakeUs() const {
        if(stage_!=Stage::Measuring)return IntervalUs;
        const auto deadline=probes_<ProbeCount?nextProbe_:lastProbe_+IntervalUs+TailUs;
        return unsigned(deadline>clock_?std::min<std::uint64_t>(deadline-clock_,IntervalUs):0);
    }
    unsigned Replies() const {
        if(!Active())return 0;
        unsigned minimum=Samples;
        for(unsigned peer=0;peer<session_.playerCount;++peer)if(peer!=session_.localPlayer){
            unsigned n=0;for(unsigned i=Warmup;i<Attempts;++i)n+=rtt_[peer][i]!=Unsent;
            minimum=std::min(minimum,n);
        }
        return minimum;
    }
    const Report& Local() const {return local_;}
    const Report& Peer() const {return reports_[session_.localPlayer==0?1:0];}
    const Report& ReportFor(unsigned player) const {return reports_[player<session_.playerCount?player:session_.localPlayer];}
    const Report& LinkTo(unsigned peer) const {return links_[peer<session_.playerCount?peer:session_.localPlayer];}
    unsigned PlayerCount() const {return session_.playerCount;}
    unsigned WorstPeer() const {return worstPeer_;}
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
    unsigned LocalBit()const{return 1u<<session_.localPlayer;}
    unsigned AllMask()const{return (1u<<session_.playerCount)-1;}
    Choice ChooseAll()const{
        Report worst=local_;for(unsigned peer=0;peer<session_.playerCount;++peer)
            if(reports_[peer].p95Us>worst.p95Us)worst=reports_[peer];
        return Choose(worst,worst,mode_,requested_,reserve_);
    }
    bool Send(PeerTransport& transport,Kind kind,unsigned sequence=0,unsigned peer=MAX_PLAYERS) {
        std::array<std::uint8_t,Bytes> bytes{};
        auto put=[&](unsigned at,std::uint64_t n,unsigned count=4){for(unsigned i=0;i<count;++i)bytes[at+i]=std::uint8_t(n>>(i*8));};
        bytes[0]='A';bytes[1]='D';bytes[2]='S';bytes[3]=session_.playerCount==2?2:3;bytes[4]=kind;
        bytes[5]=session_.localPlayer;bytes[6]=std::uint8_t(mode_);bytes[7]=std::uint8_t(reserve_);
        put(8,session_.sessionId,8);put(16,session_.gameplayAbi);put(20,session_.seed);put(24,requested_);put(28,sequence);
        if(session_.playerCount==3){bytes[56]=3;bytes[57]=kind==Probe||kind==Echo?std::uint8_t(peer):255;}
        if(kind==Summary){put(32,local_.p95Us);put(36,local_.received);put(40,local_.lost);put(44,local_.minUs);put(48,local_.maxUs);put(52,local_.meanUs);}
        else if(kind>=Proposal){put(32,choice_.delay);put(36,choice_.fullDelay);put(40,choice_.prediction);}
        return kind==Probe || kind==Echo ? transport.SendTo(peer<session_.playerCount?peer:1-session_.localPlayer,bytes.data(),bytes.size()) :
            transport.SendControl(bytes.data(),bytes.size());
    }
    SessionConfig session_{};
    AdonisMode mode_=AdonisMode::Rollback;
    Stage stage_=Stage::Idle;
    unsigned reserve_=0,probes_=0;
    std::uint32_t requested_=Automatic;
    std::uint64_t begin_=0,measurementBegin_=0,clock_=0,nextHello_=0,nextControl_=0,nextProbe_=0,lastProbe_=0,committedAt_=0;
    bool haveLocal_=false,accepted_=false,stabilityStarted_=false;
    unsigned peerHelloMask_=0,summaryMask_=0,acceptedMask_=0,commitAckMask_=0,worstPeer_=0;
    std::array<std::array<std::uint64_t,Attempts>,MAX_PLAYERS> sentAt_{},rtt_{};
    Report local_{};
    std::array<Report,MAX_PLAYERS> reports_{},links_{};
    Choice choice_{};
    const char* error_="";
};
} // namespace Netplay
