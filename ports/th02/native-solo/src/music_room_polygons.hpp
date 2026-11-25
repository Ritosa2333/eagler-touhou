#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <span>
#include "enemy_math.hpp"

namespace soew {
// TH05's native MusicRoom Step/DrawPolygons, with TH02's full set of 16.
// OP3.PI's alternate surface is the original image with palette index |= 1.
class MusicRoomPolygons {
    struct Polygon {int x,y,vx,vy,angle,turn;};
    std::array<Polygon,16> polygons_{};
    unsigned age_=0;
public:
    static constexpr int width=640,height=400;

    MusicRoomPolygons(){
        std::uint32_t seed=0x5a17;
        const auto random=[&]{seed=seed*1664525u+1013904223u;return int(seed>>16);};
        for(auto &p:polygons_){
            p={random()%width,random()%height,4-(random()&7),2+(random()&3),random()&255,4-(random()&7)};
            if(!p.vx)p.vx=1;
            if(!p.turn)p.turn=4;
        }
    }
    void tick(){
        ++age_;
        for(auto &p:polygons_){
            p.x+=p.vx;p.y+=p.vy;p.angle=(p.angle+p.turn)&255;
            if(p.x<=0||p.x>=width-1)p.vx=-p.vx;
            if(p.y>=height+100){p.y=-100;p.x=int((age_*37u)%width);}
        }
    }
    void paint(std::span<std::uint32_t> pixels,std::span<const std::uint32_t> blue) const {
        for(unsigned i=0;i<polygons_.size();++i){
            const auto &p=polygons_[i];
            const auto count=i/4+3;
            const int radius=int(i&3)*16+64;
            std::array<int,6> xs,ys;
            int top=height,bottom=0;
            for(unsigned j=0;j<count;++j){
                const auto point=enemy_vector(j*256/count+p.angle,radius);
                xs[j]=p.x+point[0];ys[j]=p.y+point[1];
                top=std::min(top,ys[j]);bottom=std::max(bottom,ys[j]);
            }
            for(int y=std::max(0,top);y<std::min(height,bottom);++y){
                double left=width,right=0;
                for(unsigned j=0,k=count-1;j<count;k=j++){
                    if((ys[j]>y)==(ys[k]>y))continue;
                    const double x=xs[k]+(y+0.5-ys[k])*(xs[j]-xs[k])/(ys[j]-ys[k]);
                    left=std::min(left,x);right=std::max(right,x);
                }
                const int from=std::clamp(int(std::ceil(left-0.5)),0,width);
                const int to=std::clamp(int(std::ceil(right-0.5)),0,width);
                if(from<to){
                    const auto at=y*width+from;
                    std::copy_n(blue.data()+at,to-from,pixels.data()+at);
                }
            }
        }
    }
};
}
