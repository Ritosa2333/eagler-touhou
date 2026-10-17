#include "FrameStatistics.hpp"
namespace th10 {
void FrameStatistics::initialize(FrameStatistics** current) noexcept {std::memset(this,0,sizeof(*this));flags=2;*current=this;}
i32 FrameStatistics::start(FrameStatisticsEnvironment& env){draw_entry=(*env.chain)->add(env.draw_callback,this,47,true,true,*env.callbacks);return 0;}
void FrameStatistics::shutdown(FrameStatisticsEnvironment& env){if(draw_entry)(*env.chain)->remove_locked(draw_entry,*env.callbacks);*env.current=nullptr;}
FrameStatistics* FrameStatistics::create(FrameStatisticsEnvironment& env){auto* value=static_cast<FrameStatistics*>(env.allocate(sizeof(FrameStatistics)));if(!value)return nullptr;value->initialize(env.current);value->start(env);return value;}
i32 FrameStatistics::sample(FrameStatisticsEnvironment& env){
    auto now=env.time();if(now<Extended::from_double(sampled_time))sampled_time=now.to_double();
    const auto elapsed=now-Extended::from_double(sampled_time);if(elapsed.is_nan()||elapsed<number(.5f))return 1;
    sampled_time=(elapsed+Extended::from_double(sampled_time)).to_double();auto count=Extended::from_int(static_cast<i32>(frames));if(frames&0x80000000u)count=count+number(4294967296.0f);
    const auto rate=count/elapsed;frames_per_second=rate.to_float();
    if(number(65)<rate){
        ++consecutive_fast;if(consecutive_fast==2||consecutive_fast==4){if(consecutive_fast==4)env.timing_counters[0]=env.timing_counters[1]=0;now=env.time();const double stamp=now.to_double();env.timing_samples[3]=stamp;env.timing_samples[2]=stamp;env.timing_samples[0]=stamp;env.timing_samples[1]=stamp;if(consecutive_fast==4)consecutive_fast=0;}
    }else consecutive_fast=0;
    if(auto* session=*env.game){if(!(session->session_flags&0x14)){expected_ticks=(Extended::from_double(expected_ticks)+number(60)).to_double();actual_ticks=(Extended::from_double(actual_ticks)+(number(57)<number(frames_per_second)?number(60):number(frames_per_second))).to_double();}session->session_flags&=~0x80u;}
    frames=0;return 1;
}
i32 FrameStatistics::draw(FrameStatisticsEnvironment& env){
    sample(env);if(*env.pending_screen!=14)if(auto* common=*env.text){common->color=frames_per_second<30?0xff5050ff:frames_per_second<40?0xffa0a0ff:0xffffffff;env.draw_rate(*common,{590,470,0},frames_per_second);(*env.text)->color=0xffffffff;}
    frames+=static_cast<u32>(*env.frame_skip)+1;return 1;
}
}
