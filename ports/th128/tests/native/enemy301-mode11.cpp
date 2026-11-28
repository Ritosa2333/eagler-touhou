#include "game/EnemyRules.hpp"
#include <bit>
#include <cassert>
#include <cstdio>
using namespace th128::enemy;
static void check(const Position& p,const Position& v,int previous,int elapsed,float ef,int duration){
    const auto bx=std::bit_cast<std::uint32_t>(p.x),by=std::bit_cast<std::uint32_t>(p.y),bz=std::bit_cast<std::uint32_t>(p.z);
    const auto vx=std::bit_cast<std::uint32_t>(v.x),vy=std::bit_cast<std::uint32_t>(v.y),vz=std::bit_cast<std::uint32_t>(v.z);
    std::printf("%u %u %u %u %u %u %d %d %d %u\n",bx,by,bz,vx,vy,vz,previous,elapsed,duration,std::bit_cast<std::uint32_t>(ef));
}
int main(){
    PositionInterpolation a;Position p{0,0,0},v{0,0,0};
    assert(beginPositionInterpolation(a,p,240,11,-150,32));
    assert(a.target.x==-150&&a.target.y==32&&a.target.z==0);
    for(int tick=1;tick<=240;++tick){
        assert(advanceInterpolatedPosition(p,v,a));
        if(tick==1){assert(p.x==-0.01f&&p.y==0);assert(a.previous==0&&a.elapsed==1&&a.elapsedFloat==1);check(p,v,a.previous,a.elapsed,a.elapsedFloat,a.duration);}
        if(tick==60){assert(p.x==-4.69f&&p.y==1);check(p,v,a.previous,a.elapsed,a.elapsedFloat,a.duration);}
        if(tick==120){assert(p.x==-75&&p.y==16);check(p,v,a.previous,a.elapsed,a.elapsedFloat,a.duration);}
    }
    assert(p.x==-150&&p.y==32&&p.z==0&&a.duration==0&&a.previous==239&&a.elapsed==240&&a.elapsedFloat==240);
    PositionInterpolation b;assert(beginPositionInterpolation(b,p,60,11,-130,160));
    for(int tick=1;tick<=60;++tick)assert(advanceInterpolatedPosition(p,v,b));
    assert(p.x==-130&&p.y==160&&b.duration==0&&b.elapsed==60);
    PositionInterpolation c;assert(beginPositionInterpolation(c,{0,0,0},60,11,150,32));
    Position q{0,0,0};Position w{0,0,0};
    for(int tick=1;tick<=60;++tick)assert(advanceInterpolatedPosition(q,w,c));
    assert(q.x==150&&q.y==32&&c.duration==0&&c.elapsed==60);
    PositionInterpolation bad;assert(!beginPositionInterpolation(bad,{},1,12,0,0));
    assert(beginPositionInterpolation(bad,{},0,12,0,0)&&bad.duration==0);
    std::printf("END %d %d\n",int(p.x),int(p.y));
}
