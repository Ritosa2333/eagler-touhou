#pragma once
#include <algorithm>
#include <cmath>
namespace th12host {
// The same clock drives SDL callbacks on desktop and requestAnimationFrame on Web.
class FrameClock {
    double previous=-1,debt=0,last_present=-1;
public:
    static constexpr double interval=1./60.;
    void reset(){previous=-1;debt=0;last_present=-1;}
    unsigned advance(double now,double period=interval,double (*pace)(double)=nullptr){
        period=std::clamp(period,1./9999.,1.);
        if(previous<0){previous=now;return 0;}
        const double elapsed=std::clamp(now-previous,0.,4*interval);
        debt+=pace?pace(elapsed):elapsed;previous=now;
        const unsigned count=std::min(256u,unsigned(std::floor((debt+1e-9)/period)));
        debt-=count*period;return count;
    }
    float alpha(double period=interval)const{return float(std::clamp(debt/period,0.,1.));}
    bool present(double now,bool limited){
        if(limited&&last_present>=0&&now-last_present<interval-1e-6)return false;
        last_present=now;return true;
    }
};
// New visible game time is counted once. Redrawing a stalled network world
// cannot publish a fictitious 60 FPS; interpolated/extrapolated subframes count.
class PresentationCounter {
    double since=-1,progress=-1;unsigned pictures=0;int stage=-1;
    float value_=0;
public:
    float value()const{return value_;}
    float frame(double now,int current_stage,double position,bool playing){
        if(since<0||now<since){since=now;pictures=0;stage=-1;progress=-1;return value_;}
        bool fresh=true;
        if(playing){
            fresh=stage!=current_stage||position>progress+1e-7;
            if(stage!=current_stage){stage=current_stage;progress=position;}
            else progress=std::max(progress,position);
        }else{stage=-1;progress=-1;}
        pictures+=fresh;
        if(now-since>=1000){value_=float(pictures*1000/(now-since));since=now;pictures=0;}
        return value_;
    }
};
}
