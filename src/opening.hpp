#pragma once
#include <algorithm>
#include <cstdint>
#include <stdexcept>

// Diagnostic accounting only; never drives playback or simulation.
struct HostTimingSample {std::uint64_t active_ms=0,lost_ms=0;};

struct RecordedHostTiming {
    HostTimingSample total{};
    bool previous_active=false;
    void boundary(){previous_active=false;}
    void observe(bool active,HostTimingSample sample){
        if(active&&previous_active){
            total.active_ms+=sample.active_ms;
            total.lost_ms+=sample.lost_ms;
        }
        previous_active=active;
    }
};

// Replay's viewer control scales the number of due steps, not the clock or recorded input.
namespace soew { inline constexpr unsigned replay_view_speed(bool held_z){return held_z?4u:1u;} }

// Host presentation clock only, not a verified PC98 simulation scheduler.
struct PresentationClock {
    static constexpr unsigned period_ms(unsigned slowdown){return 17u*std::clamp(slowdown,1u,3u);}
    std::uint64_t last=0, remainder=0;
    bool initialized=false, running=false;
    void reset(std::uint64_t now) {last=now;remainder=0;initialized=true;running=false;}
    void defer_batch(std::uint64_t now){reset(now);running=true;}
    unsigned subframe(unsigned slowdown,unsigned scale,unsigned fractional_us=0)const{
        return static_cast<unsigned>(std::min<std::uint64_t>(scale,(remainder*1000+fractional_us)*scale/(period_ms(slowdown)*1000)));
    }
    unsigned advance(std::uint64_t now,bool enabled,unsigned slowdown=1,HostTimingSample *sample=nullptr,unsigned num=1,unsigned den=1) {
        if(sample)*sample={};
        if(!initialized||now<last){reset(now);running=enabled;return 0;}
        const auto delta=now-last;last=now;
        if(!enabled){running=false;remainder=0;return 0;}
        if(!running){running=true;remainder=0;return 0;}
        if(sample)sample->active_ms=delta;
        // Discard long stalls instead of jumping through unseen animation frames.
        if(delta>250){if(sample)sample->lost_ms=delta;remainder=0;return 0;}
        remainder+=delta*num;
        const auto period=std::uint64_t(period_ms(slowdown))*den;
        const auto due=remainder/period;
        const auto limit=std::max<std::uint64_t>(8,(8ull*num+den-1)/den);
        const auto steps=std::min(limit,due);
        if(sample)sample->lost_ms=std::min(delta,(due-steps)*period/num);
        remainder=steps==limit?0:remainder-steps*period;
        return static_cast<unsigned>(steps);
    }
};
// ReC98 th02/op_01.cpp and op_03.cpp: curtains, hold, alternating title
// and effect pages, then a white-in on the bare title before menu input.
struct Opening {
    unsigned frame=0;
    bool active=true;
    static constexpr unsigned door_end=50,hold_end=68,flash_end=86;
    static constexpr unsigned fade_step=6,fade_steps=17,end=flash_end+fade_steps*fade_step;
    static constexpr unsigned flash_peak=42;
    void tick(){if(active && ++frame>=end){frame=end;active=false;}}
    void skip(){frame=end;active=false;}
    void restart(){frame=0;active=true;}
    int exposed_columns()const{return std::min(40u,frame+1)*8;}
    unsigned strip()const{return std::min(2u,(frame-hold_end)/5);}
    unsigned white()const{
        if(frame<door_end)return 55*(door_end-frame)/door_end;
        if(frame<hold_end)return 0;
        // One lift into the title cut instead of the original rapid 100/150
        // alternation. Follow the original 6-retrace fade steps at a lower peak.
        if(frame<flash_end)return 26*(frame-hold_end)/(flash_end-hold_end-1);
        return flash_peak*(100-std::min(100u,6*((frame-flash_end)/fade_step)))/100;
    }
};
constexpr std::uint32_t opening_page_pair(std::uint32_t effect,std::uint32_t title){
    // Equal exposure from the two original pages, without presenting a strobe.
    return (effect&title)+(((effect^title)&0xfefefe)>>1);
}
constexpr std::uint32_t opening_tone(std::uint32_t rgb,unsigned amount,bool white){
    std::uint32_t result=0;
    for(unsigned shift:{0u,8u,16u}){
        const auto channel=(rgb>>shift)&255;
        result|=((channel*(100-amount)+(white?255u:0u)*amount)/100)<<shift;
    }
    return result;
}
inline unsigned test_opening(){
    unsigned checks=0;
    const auto check=[&](bool ok){++checks;if(!ok)throw std::runtime_error("Opening state failed");};
    Opening o;check(o.active&&o.frame==0);
    for(unsigned i=0;i<Opening::end;++i){check(o.frame==i);check(o.exposed_columns()>=8&&o.exposed_columns()<=320);check(o.white()<=100);o.tick();}
    check(!o.active&&o.frame==Opening::end&&o.white()==0);o.tick();check(o.frame==Opening::end);
    for(unsigned f=Opening::hold_end;f<Opening::flash_end;++f){
        o.frame=f;check(o.strip()==(f<73?0u:f<78?1u:2u));
    }
    check(o.white()==26);
    o.frame=Opening::flash_end;check(o.white()==Opening::flash_peak&&opening_tone(0x123456,o.white(),true)!=0xffffff);
    o.frame+=5;check(o.white()==Opening::flash_peak);++o.frame;check(o.white()==39);
    for(unsigned previous=o.white();o.frame<Opening::end;){++o.frame;check(o.white()<=previous);previous=o.white();}
    o.frame=Opening::end;check(opening_tone(0x123456,o.white(),true)==0x123456);
    check(opening_page_pair(0xff0001,0x01ff03)==0x807f02);
    o.restart();check(o.active&&o.frame==0);o.skip();check(!o.active);
    PresentationClock clock;check(clock.advance(0,true)==0);check(clock.advance(16,true)==0);check(clock.advance(17,true)==1);
    check(clock.advance(500,false)==0);check(clock.advance(10000,true)==0);check(clock.advance(10017,true)==1);
    check(clock.advance(12000,true)==0);check(clock.advance(12017,true)==1);
    check(clock.advance(1,true)==0);check(clock.advance(18,true)==1);
    clock.reset(100);check(clock.advance(100,true,3)==0);
    check(clock.advance(150,true,3)==0);check(clock.advance(151,true,3)==1);
    clock.defer_batch(200);check(clock.advance(216,true)==0);check(clock.advance(217,true)==1);
    for(unsigned i=0;i<100;++i){check(clock.advance(1000+i*1000,false)==0);check(clock.advance(1500+i*1000,true)==0);}
    PresentationClock observed,baseline;
    RecordedHostTiming timing;
    auto measure=[&](std::uint64_t now,bool active,unsigned pace){
        HostTimingSample sample;
        check(observed.advance(now,active,pace,&sample)==baseline.advance(now,active,pace));
        timing.observe(active,sample);
    };
    measure(0,true,1);measure(12,true,1);measure(34,true,1);
    check(timing.total.active_ms==34&&timing.total.lost_ms==0);
    measure(204,true,1); // 10 due, 8 consumed.
    check(timing.total.active_ms==204&&timing.total.lost_ms==34);
    measure(504,true,1);
    check(timing.total.active_ms==504&&timing.total.lost_ms==334);
    timing.boundary();measure(804,true,2); // Focus edge even without an inactive timer.
    measure(872,true,2);measure(974,true,3);
    check(timing.total.active_ms==674&&timing.total.lost_ms==334);
    measure(1500,false,1);measure(1900,true,1);measure(1917,true,1);
    check(timing.total.active_ms==691&&timing.total.lost_ms==334);
    return checks;
}
