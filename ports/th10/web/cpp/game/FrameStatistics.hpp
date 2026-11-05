#pragma once
#include "Arithmetic.hpp"
#include "UpdateChain.hpp"
#include "GameSession.hpp"
#include "CommonResources.hpp"
namespace th10 {
struct FrameStatisticsEnvironment;
#pragma pack(push,4)
struct FrameStatistics {
    u32 flags,reserved_004,reserved_008;UpdateChainEntry* draw_entry;u32 reserved_010;
    double sampled_time;u32 consecutive_fast,frames;double actual_ticks,expected_ticks;float frames_per_second;
    u8 reserved_038[0x8c-0x38];
    void initialize(FrameStatistics** current) noexcept;
    i32 start(FrameStatisticsEnvironment& environment);
    void shutdown(FrameStatisticsEnvironment& environment);
    static FrameStatistics* create(FrameStatisticsEnvironment& environment);
    i32 sample(FrameStatisticsEnvironment& environment);
    i32 draw(FrameStatisticsEnvironment& environment);
};
#pragma pack(pop)
static_assert(sizeof(FrameStatistics)==0x8c&&offsetof(FrameStatistics,sampled_time)==0x14&&offsetof(FrameStatistics,frames_per_second)==0x34);
struct FrameStatisticsEnvironment {
    FrameStatistics** current;UpdateChain** chain;UpdateChainEnvironment* callbacks;
    GameSession** game;CommonResources** text;
    u32* timing_counters;double* timing_samples;
    i32* pending_screen;u8* frame_skip;
    CallbackToken draw_callback;
    virtual void* allocate(u32 bytes)=0;
    virtual Extended time()=0;
    virtual void draw_rate(CommonResources& text,const Vec3& position,float rate)=0;
};
}
