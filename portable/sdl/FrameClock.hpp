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
    unsigned advance(double now,double period=interval){
        period=std::clamp(period,1./9999.,1.);
        if(previous<0){previous=now;return 0;}
        debt+=std::clamp(now-previous,0.,4*interval);previous=now;
        const unsigned count=std::min(256u,unsigned(std::floor((debt+1e-9)/period)));
        debt-=count*period;return count;
    }
    float alpha(double period=interval)const{return float(std::clamp(debt/period,0.,1.));}
    bool present(double now,bool limited){
        if(limited&&last_present>=0&&now-last_present<interval-1e-6)return false;
        last_present=now;return true;
    }
};
}
