#pragma once
#include "AnmVm.hpp"
#include <algorithm>
#include <array>
#include <cmath>
namespace th19 {
// effect.anm 508(1), native 0x4d9960. Four particles per frame for fifty
// frames: three subtractive clouds (143), one additive cloud (144).
// Gameplay callbacks use the shared ANM stream and original particle scripts.
struct ChargePoint {
    struct Vec {float x=0,y=0;Vec operator+(Vec b)const{return{x+b.x,y+b.y};}Vec operator-(Vec b)const{return{x-b.x,y-b.y};}Vec operator*(float s)const{return{x*s,y*s};}};
    struct Particle {Vec start,bend,end,tangent0,tangent1,tangent2;int age=0;bool light=false;AnmVm vm;};
    std::vector<Particle> particles;
    int frame=0,duration=60;
    u32 seed=0x7856ab31;
    float random(bool native=false){if(native)return AnmVm::rand_unit();seed=seed*1664525u+1013904223u;return float(seed)/4294967296.f;}
    Vec polar(float a,float r){return{std::cos(a)*r,std::sin(a)*r};}
    Vec jitter(float r,bool native=false){float radius=(random(native)-1)*r;return polar((random(native)/2-2)*3.14159265f,radius);}
    static Vec unit(Vec v){float n=std::hypot(v.x,v.y);return n>1e-6f?v*(1/n):v;}
    static Vec curve(Vec a,Vec b,Vec ta,Vec tb,float t){
        return a*((1-t)*(1-t)*(2*t+1))+b*((3-2*t)*t*t)+ta*((1-t)*(1-t)*t)+tb*((t-1)*t*t);
    }
    void tick(float angle,float bend,int life,bool native=false,const std::unordered_map<i32,AnmScriptLoc>* scripts=nullptr){
        duration=std::max(1,life);
        for(auto& p:particles){
            ++p.age;
            if(native){
                p.vm.tick();
                // 0x4da5cc..0x4da746 starts the second curve at the bend.
                if(p.age==duration){p.end=jitter(20,true);p.tangent2=jitter(20,true);}
            }
        }
        particles.erase(std::remove_if(particles.begin(),particles.end(),[&](const Particle&p){return p.age>=duration*2+10;}),particles.end());
        if(frame++>=50)return;
        std::array<Particle,4> batch;
        for(int i=0;i<4;++i){
            auto& particle=batch[i];particle.light=i==3;
            if(native&&scripts){
                const auto script=scripts->find(particle.light?144:143);
                if(script!=scripts->end()){
                    particle.vm.bind_entry(script->second.stream,script->second.len,scripts);
                    particle.vm.clock_after_execution=true;particle.vm.tick();
                }
            }
        }
        for(auto& p:batch){
            p.start=polar(angle,300)+jitter(150,native);p.bend=polar(angle+bend,150)+jitter(50,native);
            p.tangent0=unit(p.bend-p.start)*(random(native)*100);
            p.tangent1=unit(unit(p.bend-p.start)+unit(Vec{}-p.bend))*(random(native)*200);
            if(!native){p.end=jitter(20);p.tangent2=jitter(20);}
            particles.push_back(std::move(p));
        }
    }
    Vec position(const Particle&p)const{
        return p.age<=duration?curve(p.start,p.bend,p.tangent0,p.tangent1,float(p.age)/duration):
            curve(p.bend,p.end,p.tangent1,p.tangent2,std::min(1.f,float(p.age-duration)/duration));
    }
};
}
