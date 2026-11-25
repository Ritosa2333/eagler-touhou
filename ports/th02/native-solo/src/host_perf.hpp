#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ostream>
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

namespace soew {
// Integer samples: exact through 127, then 32 equal subdivisions per power of two.
// Quantiles are bucket lower bounds (within ~3.1%); max is exact.
class HostPerfHistogram {
    static constexpr unsigned exact=128, subdivisions=32;
    static constexpr unsigned bins=exact+(64-7)*subdivisions;
    std::array<std::uint64_t,bins> counts_{};
    std::uint64_t count_=0,max_=0;
    static unsigned bucket(std::uint64_t n) noexcept {
        if(n<exact)return static_cast<unsigned>(n);
        const auto exponent=static_cast<unsigned>(std::bit_width(n)-1);
        const auto shift=exponent-5;
        return exact+(exponent-7)*subdivisions+static_cast<unsigned>((n>>shift)-subdivisions);
    }
    static std::uint64_t lower_bound(unsigned b) noexcept {
        if(b<exact)return b;
        const unsigned n=b-exact;
        return std::uint64_t(subdivisions+n%subdivisions)<<(n/subdivisions+2);
    }
public:
    void add(std::uint64_t n) noexcept {++counts_[bucket(n)];++count_;max_=(std::max)(max_,n);}
    std::uint64_t count() const noexcept {return count_;}
    std::uint64_t maximum() const noexcept {return max_;}
    std::uint64_t percentile(unsigned percent) const noexcept {
        if(!count_)return 0;
        percent=(std::clamp)(percent,1u,100u);
        auto rank=(count_/100)*percent+((count_%100)*percent+99)/100;
        for(unsigned b=0;b<bins;++b){
            if(rank<=counts_[b])return lower_bound(b);
            rank-=counts_[b];
        }
        return max_;
    }
};

// UI-thread-owned, opt-in telemetry. Hot-path calls do not allocate, lock or write.
class HostPerf {
public:
    using Clock=std::chrono::steady_clock;
    using TimePoint=Clock::time_point;
    enum class Phase : unsigned {
        Title,OpeningDoor,OpeningHold,OpeningFlash,OpeningFade,Loading,Gameplay,Dialogue,
        Paused,Ending,Other,Stage4SecondMidboss,Stage4FormationTail,Stage4MapEnd,
        Stage4Dialogue,Stage4Marisa,Count
    };
    enum class Metric : unsigned {
        TimerInterval,Simulation,Composition,OpeningFrame,PresentPrepare,PresentCommit,
        SubmittedInterval,AudioPump,AudioPublish,AudioQueue,BaseSteps,SimulationSteps,
        DiscardedMs,Count
    };
    struct BufferSize {std::size_t size=0,capacity=0;};
    struct Buffers {
        BufferSize replay_events,hit_sparks,hit_score_adds,bomb_sounds,pickup_sounds,enemy_kills;
    };
    class Span {
        HostPerf* perf_;
        Metric metric_;
        Phase phase_;
        TimePoint started_;
    public:
        Span(HostPerf* perf,Metric metric) noexcept
            :perf_(perf),metric_(metric),phase_(perf?perf->phase_:Phase::Other),started_(perf?Clock::now():TimePoint{}){}
        Span(const Span&)=delete;
        Span& operator=(const Span&)=delete;
        ~Span(){if(perf_)perf_->record(metric_,phase_,HostPerf::us(started_,Clock::now()));}
    };
private:
    static constexpr auto phase_count=static_cast<unsigned>(Phase::Count);
    static constexpr auto metric_count=static_cast<unsigned>(Metric::Count);
    static constexpr std::array epoch_names={"first_minute","minute_1_to_10","minute_10_onward"};
    static constexpr std::array epoch_metrics={Metric::TimerInterval,Metric::Simulation,Metric::Composition,
        Metric::PresentPrepare,Metric::PresentCommit,Metric::SubmittedInterval,Metric::AudioPump,Metric::AudioPublish};
    static constexpr std::array<const char*,phase_count> phase_names={
        "title","opening_door","opening_hold","opening_flash","opening_fade","loading","gameplay","dialogue",
        "paused","ending","other","stage4_second_midboss","stage4_formation_tail","stage4_map_end",
        "stage4_dialogue","stage4_marisa"
    };
    static constexpr std::array<const char*,metric_count> metric_names={
        "timer_interval_us","simulation_us","composition_us","opening_frame_us","present_prepare_us","present_commit_us",
        "submitted_interval_us","audio_pump_us","audio_publish_us","audio_queue_buffers","scheduled_base_steps",
        "simulation_steps","discarded_ms"
    };
    struct Schedule {
        std::uint64_t paused=0,steps=0,lost_ms=0,active_ms=0;
        std::array<std::uint64_t,3> pace{};
    };
    // Keep first sample, last sample of each of the first 32 minutes, and latest sample.
    // The cumulative high-water marks include samples after the minute table fills.
    static constexpr std::size_t resource_minutes=32;
    struct Resource {
        std::uint64_t at_us=0,private_bytes=0,gdi_objects=0,handles=0;
        Buffers buffers{};
        bool private_valid=false,gdi_valid=false,handles_valid=false;
    };
    std::array<std::array<HostPerfHistogram,metric_count>,phase_count> hist_{};
    std::array<std::array<HostPerfHistogram,epoch_metrics.size()>,epoch_names.size()> epoch_hist_{};
    std::array<Schedule,phase_count> schedule_{};
    std::array<Resource,resource_minutes> minute_{};
    std::array<bool,resource_minutes> minute_used_{};
    Resource first_{},last_{},peak_{};
    Phase phase_=Phase::Other;
    TimePoint started_,last_timer_{},last_submit_{},next_resource_;
    std::uint64_t resource_count_=0;
    bool has_timer_=false,has_submit_=false;

    static std::uint64_t us(TimePoint from,TimePoint to) noexcept {
        return to>from?static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(to-from).count()):0;
    }
    static constexpr unsigned index(Phase phase) noexcept {return static_cast<unsigned>(phase);}
    static constexpr unsigned index(Metric metric) noexcept {return static_cast<unsigned>(metric);}
    unsigned epoch_=0;
    void record(Metric metric,Phase phase,std::uint64_t value) noexcept {
        hist_[index(phase)][index(metric)].add(value);
        for(unsigned i=0;i<epoch_metrics.size();++i)if(metric==epoch_metrics[i]){
            epoch_hist_[epoch_][i].add(value);break;
        }
    }
    static BufferSize maximum(BufferSize a,BufferSize b) noexcept {
        return {(std::max)(a.size,b.size),(std::max)(a.capacity,b.capacity)};
    }
    static void high_water(Resource& peak,const Resource& sample) noexcept {
        if(sample.private_valid){peak.private_valid=true;peak.private_bytes=(std::max)(peak.private_bytes,sample.private_bytes);}
        if(sample.gdi_valid){peak.gdi_valid=true;peak.gdi_objects=(std::max)(peak.gdi_objects,sample.gdi_objects);}
        if(sample.handles_valid){peak.handles_valid=true;peak.handles=(std::max)(peak.handles,sample.handles);}
        auto& p=peak.buffers;const auto& s=sample.buffers;
        p={maximum(p.replay_events,s.replay_events),maximum(p.hit_sparks,s.hit_sparks),
           maximum(p.hit_score_adds,s.hit_score_adds),maximum(p.bomb_sounds,s.bomb_sounds),
           maximum(p.pickup_sounds,s.pickup_sounds),maximum(p.enemy_kills,s.enemy_kills)};
    }
    static void print_resource(std::ostream& out,const char* position,const Resource& sample) {
        const auto number=[&](const char* name,bool valid,std::uint64_t value){
            out<<' '<<name<<'=';if(valid)out<<value;else out<<"NA";
        };
        const auto buffer=[&](const char* name,BufferSize b){out<<' '<<name<<'='<<b.size<<'/'<<b.capacity;};
        out<<"host_perf_resource position="<<position;
        if(position[0]!='p')out<<" elapsed_us="<<sample.at_us;
        number("private_bytes",sample.private_valid,sample.private_bytes);
        number("gdi_objects",sample.gdi_valid,sample.gdi_objects);
        number("handles",sample.handles_valid,sample.handles);
        const auto& b=sample.buffers;
        buffer("replay_events",b.replay_events);buffer("hit_sparks",b.hit_sparks);
        buffer("hit_score_adds",b.hit_score_adds);buffer("bomb_sounds",b.bomb_sounds);
        buffer("pickup_sounds",b.pickup_sounds);buffer("enemy_kills",b.enemy_kills);
        out<<'\n';
    }
public:
    explicit HostPerf(TimePoint now=Clock::now()) noexcept :started_(now),next_resource_(now){}
    void timer(Phase phase,TimePoint now=Clock::now()) noexcept {
        if(has_timer_)record(Metric::TimerInterval,phase_,us(last_timer_,now));
        phase_=phase;last_timer_=now;has_timer_=true;
        // Separate startup, middle and late timings to expose drift hidden by the all-run histograms.
        const auto elapsed=now-started_;
        epoch_=elapsed<std::chrono::minutes(1)?0:elapsed<std::chrono::minutes(10)?1:2;
    }
    void phase(Phase phase) noexcept {phase_=phase;}
    void submitted(TimePoint now=Clock::now()) noexcept {
        if(has_submit_)record(Metric::SubmittedInterval,phase_,us(last_submit_,now));
        last_submit_=now;has_submit_=true;
    }
    void schedule(unsigned pace,bool active,unsigned steps,std::uint64_t lost_ms,std::uint64_t active_ms=0) noexcept {
        auto& s=schedule_[index(phase_)];
        if(!active){++s.paused;return;}
        ++s.pace[(std::clamp)(pace,1u,3u)-1];s.steps+=steps;s.lost_ms+=lost_ms;s.active_ms+=active_ms;
        record(Metric::BaseSteps,phase_,steps);
        if(lost_ms)record(Metric::DiscardedMs,phase_,lost_ms);
    }
    void simulation_steps(unsigned steps) noexcept {record(Metric::SimulationSteps,phase_,steps);}
    void audio_queue(unsigned buffers) noexcept {record(Metric::AudioQueue,phase_,buffers);}
    bool resource_due() const noexcept {return has_timer_&&last_timer_>=next_resource_;}
    void resources(Buffers buffers) noexcept {
        if(!resource_due())return;
        next_resource_=last_timer_+std::chrono::seconds(5);
        Resource sample{};
        sample.at_us=us(started_,last_timer_);sample.buffers=buffers;
#ifdef _WIN32
        const auto process=GetCurrentProcess();
        PROCESS_MEMORY_COUNTERS_EX memory{};memory.cb=sizeof(memory);
        sample.private_valid=K32GetProcessMemoryInfo(process,reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memory),sizeof(memory))!=FALSE;
        sample.private_bytes=memory.PrivateUsage;
        SetLastError(ERROR_SUCCESS);
        sample.gdi_objects=GetGuiResources(process,GR_GDIOBJECTS);
        sample.gdi_valid=sample.gdi_objects!=0||GetLastError()==ERROR_SUCCESS;
        DWORD handles=0;
        sample.handles_valid=GetProcessHandleCount(process,&handles)!=FALSE;
        sample.handles=handles;
#endif
        if(resource_count_++==0)first_=sample;
        last_=sample;high_water(peak_,sample);
        const auto minute=sample.at_us/60'000'000;
        if(minute<resource_minutes){minute_[minute]=sample;minute_used_[minute]=true;}
    }
    void report(std::ostream& out) const {
        out<<"host_perf schema=1 elapsed_us="<<us(started_,has_timer_?last_timer_:started_)
           <<" resource_samples="<<resource_count_<<" percentile=bucket_lower_bound\n";
        for(unsigned p=0;p<phase_count;++p){
            const auto& s=schedule_[p];
            if(s.paused||s.pace[0]||s.pace[1]||s.pace[2])out<<"host_perf_schedule phase="<<phase_names[p]
                <<" paused_callbacks="<<s.paused<<" pace1_callbacks="<<s.pace[0]<<" pace2_callbacks="<<s.pace[1]
                <<" pace3_callbacks="<<s.pace[2]<<" scheduled_steps="<<s.steps
                <<" active_ms="<<s.active_ms<<" discarded_ms="<<s.lost_ms<<'\n';
            for(unsigned m=0;m<metric_count;++m){
                const auto& h=hist_[p][m];
                if(h.count())out<<"host_perf_hist phase="<<phase_names[p]<<" metric="<<metric_names[m]<<" n="<<h.count()
                    <<" p50="<<h.percentile(50)<<" p95="<<h.percentile(95)<<" p99="<<h.percentile(99)<<" max="<<h.maximum()<<'\n';
            }
        }
        for(unsigned e=0;e<epoch_names.size();++e)for(unsigned m=0;m<epoch_metrics.size();++m){
            const auto& h=epoch_hist_[e][m];
            if(h.count())out<<"host_perf_epoch epoch="<<epoch_names[e]<<" metric="<<metric_names[index(epoch_metrics[m])]
                <<" n="<<h.count()<<" p50="<<h.percentile(50)<<" p95="<<h.percentile(95)
                <<" p99="<<h.percentile(99)<<" max="<<h.maximum()<<'\n';
        }
        if(resource_count_){
            print_resource(out,"first",first_);
            for(std::size_t i=0;i<resource_minutes;++i)if(minute_used_[i])print_resource(out,"minute",minute_[i]);
            print_resource(out,"last",last_);
            print_resource(out,"peak",peak_);
        }
    }
};
}
