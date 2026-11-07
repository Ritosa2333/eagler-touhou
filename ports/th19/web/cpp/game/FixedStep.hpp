#pragma once
#include <algorithm>
#include <cmath>
namespace th19 {
// ECL/SHT/ANM durations are 60 Hz game ticks, not browser refresh callbacks.
struct FixedStep {
    double previous=-1,remaining=0;
    void reset(){previous=-1;remaining=0;}
    int advance(double now){
        if(previous<0){previous=now;return 1;}
        const double elapsed=now-previous;previous=now;
        // Do not replay time spent in a suspended/background tab.
        if(elapsed<0||elapsed>250){remaining=0;return 1;}
        remaining+=elapsed;
        int count=std::min(8,int(std::floor((remaining+1e-7)/(1000.0/60))));
        remaining-=count*(1000.0/60);return count;
    }
};
}
