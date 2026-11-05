#pragma once
#include <algorithm>
#include <array>
#include <cstdint>

namespace touhou::sdl {
// Conservative pixel requirements. Only clears remove required pixels;
// ordinary draws never claim to cover their viewport opaquely. On saturation
// keep the previous superset instead of risking a false dead-write decision.
struct RasterRect {
    std::int64_t left,top,right,bottom;
    bool empty()const{return left>=right||top>=bottom;}
    RasterRect intersect(const RasterRect& b)const{
        return {std::max(left,b.left),std::max(top,b.top),std::min(right,b.right),std::min(bottom,b.bottom)};
    }
};
struct RasterNeeded {
    std::array<RasterRect,64> regions;unsigned count=0;
    RasterRect full{};
    void require_all(){count=full.empty()?0:1;if(count)regions[0]=full;}
    bool intersects(RasterRect r)const{
        for(unsigned i=0;i<count;++i)if(!regions[i].intersect(r).empty())return true;
        return false;
    }
    void subtract(RasterRect cut){
        if(cut.empty())return;
        std::array<RasterRect,64> next;unsigned n=0;
        const auto add=[&](RasterRect r){if(r.empty())return true;if(n==next.size())return false;next[n++]=r;return true;};
        for(unsigned i=0;i<count;++i){const auto r=regions[i],hit=r.intersect(cut);
            if(hit.empty()){if(!add(r))return;continue;}
            if(!add({r.left,r.top,hit.left,r.bottom})||!add({hit.right,r.top,r.right,r.bottom})||
               !add({hit.left,r.top,hit.right,hit.top})||!add({hit.left,hit.bottom,hit.right,r.bottom}))return;
        }
        std::copy_n(next.data(),n,regions.data());count=n;
    }
};
}
