#include "FrameStatistics.hpp"
#include <cstdio>
#include <cmath>
namespace th08 {
namespace {
void sample_statistics(FrameStatisticsState& s,const FrameStatisticsContext& c,FrameClock& clock,bool bookkeeping,bool presentation_started){
    bool sample=false;float seconds=0;
    if(!c.paused){
        s.frames+=1+u32(c.frameskip);
        if(!c.frequency){
            if(!(s.timer_started&1)){s.timer_started|=1;s.milliseconds_origin=clock.milliseconds();}
            const u32 now=clock.milliseconds();if(now<s.milliseconds_origin){s.frames=0;s.milliseconds_origin=now;}
            if(now-s.milliseconds_origin>=500){seconds=(Extended::from_int64(now-s.milliseconds_origin)/number(1000)).to_float();s.milliseconds_origin=now;sample=true;}
        }else{
            if(!u32(s.performance_origin))s.performance_origin=clock.performance_counter();
            const u64 now=clock.performance_counter();if(u32(now)<u32(s.performance_origin)){s.performance_origin=now;s.frames=0;}
            if(u32(now)>=u32(u32(s.performance_origin)+(c.frequency>>1))){seconds=(Extended::from_int64(u32(now)-u32(s.performance_origin))/Extended::from_int64(c.frequency)).to_float();s.performance_origin=now;++s.performance_samples;sample=true;}
        }
        if(sample){
            const float fps=(Extended::from_int64(s.frames)/number(seconds)).to_float();s.frames=0;
            if(!presentation_started){
                if(std::isfinite(fps))std::snprintf(s.fps_text,sizeof(s.fps_text),"%.02ffps",double(fps));
                else std::snprintf(s.fps_text,sizeof(s.fps_text),"%s1.#%cfps",std::signbit(fps)?"-":"",std::isnan(fps)&&!std::signbit(fps)?'R':'J');
            }
            if((c.game_flags&4)&&bookkeeping){const auto nominal=number(60);s.total=(number(s.total)+nominal).to_float();const auto amount=nominal*number(.9f)<number(fps)?nominal:nominal*number(.7f)<number(fps)?nominal*number(.8f):nominal*number(.5f)<number(fps)?nominal*number(.6f):nominal*number(.5f);s.rendered=(number(s.rendered)+amount).to_float();
                if(!(c.game_flags&8))s.replay_fps=i16((number(fps)+number(.5f)).truncate_int());
                else std::snprintf(s.replay_text,sizeof(s.replay_text),"%2d",i32(s.replay_fps));
            }
        }
    }
}
}
void FrameStatistics::draw_text(){
    const auto& c=context;auto& s=state;
    if(!c.suppress_text){
        ascii.add_string({512,464,0},s.fps_text,c.software_texturing);
        if((c.game_flags&12)==12){ascii.state.color=c.replay_warning?0xffff4040:0xffffffd0;ascii.add_string({384,448,0},s.replay_text,c.software_texturing);ascii.state.color=0xffffffff;}
    }
}
void FrameStatistics::calculate(bool draw){
    sample_statistics(state,context,clock,draw,presentation_started);if(draw)draw_text();
}
void FrameStatistics::presentation_frame(){
    const u32 now=clock.milliseconds();
    if(!presentation_started){presentation_started=true;presentation_origin=now;presentation_frames=0;}
    if(now<presentation_origin){presentation_origin=now;presentation_frames=0;}
    ++presentation_frames;
    const u32 elapsed=now-presentation_origin;if(elapsed<500)return;
    const double fps=double(presentation_frames)*1000./double(elapsed);
    std::snprintf(state.fps_text,sizeof(state.fps_text),"%.02ffps",fps);
    presentation_origin=now;presentation_frames=0;
}
}
