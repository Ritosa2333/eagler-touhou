#pragma once

// Opt-in, main-thread diagnostics for the TH09 development harness. These
// counters are neither game state nor rollback state. Release builds compile
// every call site away and expose no performance-control entry points.
#if TH09_DEVELOPMENT_HARNESS
#include <array>
#include <cstdint>
#include <emscripten.h>

namespace th09::sdl::performance {
enum class Phase : unsigned {
    GameOpen, PrepareLoading, PlatformInit, GraphicsInit, FontInit, FontWarm,
    AudioInit, TitleInit, StartMatch, FrameCallback, GameTick, RollbackTick,
    NetworkPump, InputCapture, Checkpoint, Restore, TickInputs, Update,
    SemanticDraw, Present, WarmResources, ConfirmedPublish, AudioUpdate,
    AudioPump, AudioMix, AudioPut, MusicSelect, GpuSubmit, GpuDirect,
    GpuCommit, TextureUpload, TextRender, Count
};
enum class Counter : unsigned {
    ForwardTicks, ReplayTicks, SpeculativeTicks, RestoredCheckpoints,
    SealedCheckpoints, PublishedFrames, PublishedCommands, PublishedSfx,
    PublishedMusic, PublishedOtherAudio, PumpReadyCalls, PumpRefillCalls,
    PcmChunks, PcmFramesRead, PcmFramesQueued, AudioReadErrors, AudioPutErrors,
    QueueEmptyCalls, MusicDecoderOpens, MusicSameTrackRestarts,
    GpuQueuedBatches, GpuDirectBatches, TextureCreates, TextureUploads,
    SfxDecoderOpens, ProgramCompiles, Readbacks, TextureUploadBytes, TimingErrors, Count
};
enum class Gauge : unsigned { QueueFramesBefore, QueueFramesAfterEstimate, LastPumpChunks, Count };
constexpr unsigned headerSize=6,phaseCount=unsigned(Phase::Count),counterCount=unsigned(Counter::Count),gaugeCount=unsigned(Gauge::Count);
constexpr unsigned counterOffset=headerSize+3*phaseCount,gaugeOffset=counterOffset+counterCount;
inline std::array<double,gaugeOffset+gaugeCount> values{};
inline bool enabled=false;
inline std::uint32_t epoch=0;
class Scope;
inline Scope* active=nullptr;

// Count, inclusive elapsed ms, exclusive elapsed ms for each completed scope.
// Exclusive subtracts only measured direct children. Uninstrumented work and
// instrumentation overhead remain in the parent's exclusive value. Scopes
// live on the C++ stack: no allocation, timestamp log, GPU query or readback.
class Scope final {
    Scope* parent=nullptr;
    unsigned offset=0;
    std::uint32_t generation=0;
    double began=0,children=0;
    bool running=false;
public:
    explicit Scope(Phase phase):offset(headerSize+3*unsigned(phase)),generation(epoch),running(enabled){
        if(!running)return;
        parent=active;active=this;began=emscripten_get_now();
    }
    ~Scope(){stop();}
    Scope(const Scope&)=delete;Scope& operator=(const Scope&)=delete;
    // Explicit stop is used only after nested scopes have finished, notably
    // before onGameFrame so the JS observer sees the completed callback.
    void stop(){
        if(!running)return;
        const double elapsed=emscripten_get_now()-began;
        const double own=elapsed-children;
        if(generation==epoch){
            // Suppress only sub-nanosecond subtraction residue. A larger
            // negative duration remains visible and is a diagnostic error.
            if(active!=this||elapsed<0||own<-1e-6)++values[counterOffset+unsigned(Counter::TimingErrors)];
            ++values[offset];values[offset+1]+=elapsed;values[offset+2]+=own<0&&own>=-1e-6?0:own;
        }
        if(parent)parent->children+=elapsed;
        active=parent;running=false;
    }
};
inline void count(Counter counter,double amount){if(enabled)values[counterOffset+unsigned(counter)]+=amount;}
inline void gauge(Gauge index,double value){if(enabled)values[gaugeOffset+unsigned(index)]=value;}
inline const double* snapshot(){
    values[0]=1;values[1]=enabled;values[2]=epoch;values[3]=phaseCount;values[4]=counterCount;values[5]=gaugeCount;
    return values.data();
}
inline void reset(){
    ++epoch;values.fill(0);
    values[gaugeOffset+unsigned(Gauge::QueueFramesBefore)]=-1;
    values[gaugeOffset+unsigned(Gauge::QueueFramesAfterEstimate)]=-1;
    snapshot();
}
inline constexpr char schema[]=R"json({
"version":1,
"header":["version","enabled","epoch","phaseCount","counterCount","gaugeCount"],
"phases":["gameOpen","prepareLoading","platformInit","graphicsInit","fontInit","fontWarm","audioInit","titleInit","startMatch","frameCallback","gameTick","rollbackTick","networkPump","inputCapture","checkpoint","restore","tickInputs","update","semanticDraw","present","warmResources","confirmedPublish","audioUpdate","audioPump","audioMix","audioPut","musicSelect","gpuSubmit","gpuDirect","gpuCommit","textureUpload","textRender"],
"counters":["forwardTicks","replayTicks","speculativeTicks","restoredCheckpoints","sealedCheckpoints","publishedFrames","publishedCommands","publishedSfx","publishedMusic","publishedOtherAudio","pumpReadyCalls","pumpRefillCalls","pcmChunks","pcmFramesRead","pcmFramesQueued","audioReadErrors","audioPutErrors","queueEmptyCalls","musicDecoderOpens","musicSameTrackRestarts","gpuQueuedBatches","gpuDirectBatches","textureCreates","textureUploads","sfxDecoderOpens","programCompiles","readbacks","textureUploadBytes","timingErrors"],
"gauges":["queueFramesBefore","queueFramesAfterEstimate","lastPumpChunks"],
"phaseFields":["calls","inclusiveMs","exclusiveMs"],
"units":"Cumulative completed-scope calls and elapsed milliseconds, not process CPU time or GPU execution time. Counters are cumulative; gauges describe the last pump attempt. Exclusive residue in [-1e-6,0) ms is clamped to zero; larger negative durations remain visible and increment timingErrors, as does invalid scope nesting.",
"scope":"Development only, disabled by default. Enable before open and reset between measurement epochs outside a running game call. Snapshot does not reset or mutate gameplay. Subtract snapshots with equal epoch. Parent exclusive time retains all uninstrumented work; inclusive phases overlap. Semantic Draw excludes present; submit and texture work may occur inside Draw, present or resource preparation. Frame callback ends before the JS onGameFrame notification; gameTick also covers manually driven live callbacks. Audio mix includes decode/resample/mixing, audioPut includes SDL enqueue, and queueFramesAfterEstimate uses the existing queue observation plus successful 1024-frame puts, with no extra device query. -1 queue gauges mean the last pump did not reach a ready stream. No audio, Draw, input or rollback work is skipped."
})json";
}
#define TH09_PERF_SCOPE(name,phase) ::th09::sdl::performance::Scope name(::th09::sdl::performance::Phase::phase)
#define TH09_PERF_STOP(name) name.stop()
#define TH09_PERF_COUNT(counter,amount) ::th09::sdl::performance::count(::th09::sdl::performance::Counter::counter,(amount))
#define TH09_PERF_GAUGE(index,value) ::th09::sdl::performance::gauge(::th09::sdl::performance::Gauge::index,(value))
#else
#define TH09_PERF_SCOPE(name,phase) ((void)0)
#define TH09_PERF_STOP(name) ((void)0)
#define TH09_PERF_COUNT(counter,amount) ((void)0)
#define TH09_PERF_GAUGE(index,value) ((void)0)
#endif
