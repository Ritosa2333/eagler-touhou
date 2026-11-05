#pragma once
#include "AnmVm.hpp"
#include <algorithm>
#include <cmath>
namespace th19 {
// effect.anm 508(1), native 0x4d9960. Four particles per frame for fifty
// frames: three subtractive clouds (143), one additive cloud (144).
// Native global RNG is not reproduced; the paths retain the native bounds.
struct ChargePoint {
    struct Vec {float x=0,y=0;Vec operator+(Vec b)const{return{x+b.x,y+b.y};}Vec operator-(Vec b)const{return{x-b.x,y-b.y};}Vec operator*(float s)const{return{x*s,y*s};}};
    struct Particle {Vec start,bend,end,tangent0,tangent1,tangent2;int age=0;bool light=false;};
    std::vector<Particle> particles;
    int frame=0,duration=60;
    u32 seed=0x7856ab31;
    float random(){seed=seed*1664525u+1013904223u;return float(seed)/4294967296.f;}
    Vec polar(float a,float r){return{std::cos(a)*r,std::sin(a)*r};}
    Vec jitter(float r){float radius=(random()-1)*r;return polar((random()/2-2)*3.14159265f,radius);}
    static Vec unit(Vec v){float n=std::hypot(v.x,v.y);return n>1e-6f?v*(1/n):v;}
    static Vec curve(Vec a,Vec b,Vec ta,Vec tb,float t){
        return a*((1-t)*(1-t)*(2*t+1))+b*((3-2*t)*t*t)+ta*((1-t)*(1-t)*t)+tb*((t-1)*t*t);
    }
    void tick(float angle,float bend,int life){
        duration=std::max(1,life);
        for(auto&p:particles)++p.age;
        particles.erase(std::remove_if(particles.begin(),particles.end(),[&](const Particle&p){return p.age>=duration*2+10;}),particles.end());
        if(frame++>=50)return;
        for(int i=0;i<4;++i){Particle p;p.light=i==3;
            p.start=polar(angle,300)+jitter(150);p.bend=polar(angle+bend,150)+jitter(50);
            p.tangent0=unit(p.bend-p.start)*(random()*100);
            p.tangent1=unit(unit(p.bend-p.start)+unit(Vec{}-p.bend))*(random()*200);
            p.end=jitter(20);p.tangent2=jitter(20);particles.push_back(p);
        }
    }
    Vec position(const Particle&p)const{
        return p.age<=duration?curve(p.start,p.bend,p.tangent0,p.tangent1,float(p.age)/duration):
            curve(p.bend,p.end,p.tangent1,p.tangent2,std::min(1.f,float(p.age-duration)/duration));
    }
};
}
