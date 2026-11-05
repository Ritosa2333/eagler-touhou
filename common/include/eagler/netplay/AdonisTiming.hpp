#pragma once

#include <eagler/netplay/NetplayProtocol.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace Netplay {
enum class AdonisMode : std::uint8_t { Rollback = 0, Delay = 1, Hybrid = 2 };
inline bool ValidAdonisMode(AdonisMode mode) {
    return static_cast<unsigned>(mode) <= static_cast<unsigned>(AdonisMode::Hybrid);
}
// Apply AFTER the adapter's existing delay/gameplay ABI. A baseline peer cannot
// silently start with a new-policy peer, even if both selected the same D.
inline std::uint32_t AdonisGameplayAbi(std::uint32_t base, AdonisMode mode, unsigned delay) {
    return mode == AdonisMode::Rollback ? base :
        base ^ (0x41440000u | (static_cast<unsigned>(mode) << 8) | (delay << 4));
}

// Startup recommendation only; RTT/2 assumes symmetric paths. No running
// session changes its delay from this statistic. Invalid/zero samples excluded.
// The even-count averaging follows the supplied Adonis2 static analysis.
inline unsigned RecommendAdonisDelay(const std::vector<double>& rttMs) {
    std::vector<double> samples;
    samples.reserve(rttMs.size());
    for (double n : rttMs) if (std::isfinite(n) && n > 0) samples.push_back(n);
    if (samples.empty()) return 1;
    std::sort(samples.begin(), samples.end());
    const auto at = (samples.size() - 1) * 95 / 100;
    double tail = samples[at];
    if (samples.size() % 2 == 0 && at + 1 < samples.size())
        tail = (tail + samples[at + 1]) / 2;
    return static_cast<unsigned>(std::clamp(std::ceil(tail * 60 / 2000), 1.0, 9.0));
}

struct AdonisPhaseSample {
    std::uint64_t sessionId = 0;
    std::uint32_t frame = 0; // exclusive, 16-frame window boundary
    std::uint8_t senderPlayer = 0, targetPlayer = 0, waitCount = 0, leadCount = 0;
    std::uint32_t waitUs = 0, leadUs = 0;
};
inline bool ValidAdonisPhaseSample(const AdonisPhaseSample& s) {
    return s.sessionId && s.frame && s.frame != INVALID_FRAME && s.frame % 16 == 0 &&
        s.senderPlayer < MAX_PLAYERS && s.targetPlayer < MAX_PLAYERS &&
        s.senderPlayer != s.targetPlayer && unsigned(s.waitCount) + s.leadCount == 16 &&
        s.waitUs <= unsigned(s.waitCount) * 250000u &&
        s.leadUs <= unsigned(s.leadCount) * 250000u;
}
inline bool EncodeAdonisPhaseSample(const AdonisPhaseSample& s, std::vector<std::uint8_t>* out) {
    if (!out || !ValidAdonisPhaseSample(s)) return false;
    out->assign(28, 0);
    auto& b = *out; b[0]='A'; b[1]='D'; b[2]='P'; b[3]=1;
    auto put = [&](unsigned at, std::uint64_t n, unsigned count) {
        for (unsigned i=0;i<count;++i) b[at+i]=static_cast<std::uint8_t>(n >> (8*i));
    };
    put(4,s.sessionId,8); put(12,s.frame,4);
    b[16]=s.senderPlayer; b[17]=s.targetPlayer; b[18]=s.waitCount; b[19]=s.leadCount;
    put(20,s.waitUs,4); put(24,s.leadUs,4); return true;
}
inline bool DecodeAdonisPhaseSample(const std::uint8_t* b, std::size_t n, AdonisPhaseSample* out) {
    if (!b || !out || n!=28 || b[0]!='A' || b[1]!='D' || b[2]!='P' || b[3]!=1) return false;
    auto get = [&](unsigned at,unsigned count) {
        std::uint64_t x=0; for(unsigned i=0;i<count;++i)x|=std::uint64_t(b[at+i])<<(8*i); return x;
    };
    AdonisPhaseSample s;
    s.sessionId=get(4,8);s.frame=static_cast<std::uint32_t>(get(12,4));
    s.senderPlayer=b[16];s.targetPlayer=b[17];s.waitCount=b[18];s.leadCount=b[19];
    s.waitUs=static_cast<std::uint32_t>(get(20,4));s.leadUs=static_cast<std::uint32_t>(get(24,4));
    if (!ValidAdonisPhaseSample(s)) return false;
    *out=s; return true;
}

// Arrival/first-due clocks are network state, NEVER world checkpoint state.
// Retried frames and rollback replays cannot overwrite either timestamp.
// For Hybrid, statistics wait for actual input rather than treating prediction
// as an early arrival. CPU/replay duration is not added to measured waiting.
class AdonisPhase {
public:
    bool Reset(std::uint64_t id, std::uint8_t local, std::uint8_t count, unsigned predictionFrames=0) {
        *this=AdonisPhase{};
        if(!id || count<2 || count>MAX_PLAYERS || local>=count || predictionFrames>2) return false;
        id_=id;local_=local;count_=count;predictionAllowanceUs_=(predictionFrames*1000000u+59)/60;
        nextWindow_.fill(16);return true;
    }
    void ObserveArrival(std::uint8_t peer,std::uint32_t frame,std::uint64_t nowUs) {
        if(!id_ || peer>=count_ || peer==local_ || frame==INVALID_FRAME) return;
        const auto next=lastDue_==INVALID_FRAME?0:std::uint64_t(lastDue_)+1;
        if(std::uint64_t(frame)+History<=next || std::uint64_t(frame)>=next+History) return;
        auto& a=arrivals_[peer][frame%History];
        if(a.frame!=frame)a={frame,nowUs};
    }
    void ObserveDue(std::uint32_t frame,std::uint64_t nowUs) {
        if(!id_ || frame==INVALID_FRAME || (lastDue_!=INVALID_FRAME && frame<=lastDue_)) return;
        due_[frame%History]={frame,nowUs};lastDue_=frame;
    }
    bool PollLocalSample(AdonisPhaseSample* out) {
        if(!out || lastDue_==INVALID_FRAME) return false;
        for(std::uint8_t peer=0;peer<count_;++peer) {
            if(peer==local_)continue;
            auto& end=nextWindow_[peer];
            if(std::uint64_t(end)+History<=std::uint64_t(lastDue_)+1)
                end=((lastDue_+1)/16)*16; // expire statistics, NEVER gameplay input
            if(end>lastDue_+1)continue;
            AdonisPhaseSample s; s.sessionId=id_;s.frame=end;s.senderPlayer=local_;s.targetPlayer=peer;
            bool complete=true;
            for(auto f=end-16;f<end;++f) {
                const auto& d=due_[f%History];const auto& a=arrivals_[peer][f%History];
                if(d.frame!=f || a.frame!=f){complete=false;break;}
                if(a.us>d.us){++s.waitCount;s.waitUs+=static_cast<std::uint32_t>(std::min<std::uint64_t>(a.us-d.us,250000));}
                else{++s.leadCount;s.leadUs+=static_cast<std::uint32_t>(std::min<std::uint64_t>(d.us-a.us,250000));}
            }
            if(!complete)continue;
            end+=16;localSamples_[peer][(s.frame/16)%Windows]=s;
            TryAdjust(s,remoteSamples_[peer][(s.frame/16)%Windows]);
            *out=s;return true;
        }
        return false;
    }
    bool ReceiveSample(const AdonisPhaseSample& s) {
        if(!ValidAdonisPhaseSample(s) || s.sessionId!=id_ || s.senderPlayer>=count_ ||
           s.senderPlayer==local_ || s.targetPlayer!=local_)return false;
        const auto now=lastDue_==INVALID_FRAME?0:lastDue_+1;
        if(std::uint64_t(s.frame)+32<now || std::uint64_t(s.frame)>std::uint64_t(now)+32)return true;
        auto& remote=remoteSamples_[s.senderPlayer][(s.frame/16)%Windows];
        if(remote.frame==s.frame)return true;
        remote=s;TryAdjust(localSamples_[s.senderPlayer][(s.frame/16)%Windows],s);return true;
    }
    // Consume once in the outer forward scheduler. Subtract this from accrued
    // WALL time, not the fixed 1/60 simulation step. Do not also run the old
    // proportional pacer. Eight ms cap is a modern bounded policy, not a claim
    // that the original binary used the same cap.
    double TakeDelayMs(){const auto n=pendingUs_;pendingUs_=0;return n/1000.0;}
    std::uint32_t Adjustments() const {return adjustments_;}
    std::uint64_t TotalDelayUs() const {return totalDelayUs_;}
    unsigned PredictionAllowanceUs() const {return predictionAllowanceUs_;}
private:
    static constexpr unsigned History=256,Windows=8;
    struct Stamp {std::uint32_t frame=INVALID_FRAME;std::uint64_t us=0;};
    void TryAdjust(const AdonisPhaseSample& local,const AdonisPhaseSample& remote) {
        if(lastDue_==INVALID_FRAME || local.sessionId!=id_ || !local.frame)return;
        const auto now=lastDue_+1;
        if(now<60 || local.frame>now || now-local.frame>32 ||
            (lastAdjustment_!=INVALID_FRAME && now-lastAdjustment_<60))return;
        std::uint32_t shift=0;
        if(local.waitCount) {
            const auto wait=local.waitUs/local.waitCount;
            // Preserve an explicitly negotiated 1..2f hybrid prediction lead.
            // Otherwise positive-wait correction slowly adds back the latency
            // that startup deliberately removed from D. Raw reports remain raw.
            if(wait>predictionAllowanceUs_)shift=wait-predictionAllowanceUs_;
        }
        else if(remote.sessionId==id_ && remote.frame==local.frame && !remote.waitCount &&
                local.leadCount && remote.leadCount) {
            const auto here=local.leadUs/local.leadCount,there=remote.leadUs/remote.leadCount;
            if(there>here && there-here>4000)shift=(there-here)/2;
        }
        shift=std::min(shift,8000u);
        if(!shift)return;
        pendingUs_=std::max(pendingUs_,shift);lastAdjustment_=now;++adjustments_;totalDelayUs_+=shift;
    }
    std::uint64_t id_=0,totalDelayUs_=0;
    std::uint8_t local_=0,count_=0;
    std::uint32_t lastDue_=INVALID_FRAME,lastAdjustment_=INVALID_FRAME,pendingUs_=0,adjustments_=0,predictionAllowanceUs_=0;
    std::array<Stamp,History> due_{};
    std::array<std::array<Stamp,History>,MAX_PLAYERS> arrivals_{};
    std::array<std::uint32_t,MAX_PLAYERS> nextWindow_{};
    std::array<std::array<AdonisPhaseSample,Windows>,MAX_PLAYERS> localSamples_{},remoteSamples_{};
};
} // namespace Netplay
