#pragma once
#include "core_contract.hpp"
#include "enemy_math.hpp"
#include <algorithm>

namespace soew {
// TH05 shares this 170-frame sequence with TH04 (ReC98 th04/zunsoft.asm).
// Each animation frame lasts two retraces; the last 17 retraces fade to black.
struct ZunSoftOpening {
    static constexpr unsigned animation_frames=170, fade_start=2*animation_frames, end=fade_start+17;
    struct Burst {unsigned frame;int x,y;unsigned count,base;};
    static constexpr std::array<Burst,12> bursts{{
        {0,180,180,20,0},{16,460,220,20,10},{24,220,160,20,0},{32,380,240,20,10},
        {40,200,190,20,0},{44,340,200,20,10},{48,280,170,20,0},{52,380,260,20,10},
        {56,200,190,20,0},{60,440,210,20,10},{64,320,200,64,0},{68,320,200,64,10}
    }};
    struct Particle {
        bool alive=false;
        unsigned age=0,base=0,angle=0;
        int x=0,y=0,speed=0;
        unsigned cel()const{return base+age/4;}
        std::array<int,2> offset()const{return enemy_vector(angle,speed*std::max(0,int(age)-15));}
    };
    IRand rng{1};
    std::array<Particle,256> particles{};
    unsigned retrace=end;
    bool active()const{return retrace<end;}
    unsigned light()const{return retrace<fade_start?std::min(100,2*std::max(0,int(retrace/2)-15)):100-std::min(100u,6*(retrace-fade_start));}
    void restart(){*this={};retrace=0;animate(0);}
    void skip(){retrace=end;}
    bool tick(){
        ++retrace;
        return retrace<fade_start&&retrace%2==0&&animate(retrace/2);
    }
private:
    bool animate(unsigned frame){
        for(const auto &burst:bursts)if(burst.frame==frame){
            unsigned remaining=burst.count;
            for(auto &p:particles)if(!p.alive){
                const auto speed=64+int(rng.next()%224);
                p={true,0,burst.base,rng.next()%256u,burst.x,burst.y,speed};
                if(!--remaining)break;
            }
        }
        bool sound=false;
        for(auto &p:particles)if(p.alive){
            if(++p.age==40)p.alive=false;
            sound|=p.age==16;
        }
        return sound;
    }
};

inline unsigned test_zunsoft_opening(){
    unsigned checks=0;
    const auto check=[&](bool ok){++checks;if(!ok)throw std::runtime_error("ZUN Soft opening state failed");};
    ZunSoftOpening opening;
    check(!opening.active());opening.restart();
    check(opening.active()&&opening.light()==0&&opening.particles[0].age==1&&opening.particles[0].cel()==0);
    for(unsigned frame=1;frame<ZunSoftOpening::end;++frame){
        bool cue=false;
        for(const auto &burst:ZunSoftOpening::bursts)cue|=frame==2*(burst.frame+15);
        check(opening.tick()==cue);
        if(frame==30){const auto &p=opening.particles[0];check(p.age==16&&p.cel()==4&&p.offset()!=std::array{0,0});}
        if(frame==32)check(opening.light()==2&&opening.particles[20].cel()==10);
        if(frame==62)check(opening.particles[0].cel()==8);
        if(frame==ZunSoftOpening::fade_start)check(opening.light()==100);
    }
    check(opening.active()&&opening.light()==4&&!opening.tick()&&!opening.active());
    opening.restart();opening.skip();check(!opening.active());
    return checks;
}
}
