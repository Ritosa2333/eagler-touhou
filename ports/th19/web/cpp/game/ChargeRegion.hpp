#pragma once
#include "Interpolation.hpp"
#include <array>
namespace th19 {
// Per-character Aura vtable start/update parameters from the original executable.
struct ChargeRegion {
    struct Point{float x=0,y=0;};
    inline static constexpr int scripts[19]={15,15,16,11,11,10,11,13,8,10,13,14,15,11,10,11,10,12,11};
    inline static constexpr float targets[19]={64,14,128,30,32,32,48,50,50,28,88,96,600,40,32,48,56,120,128};
    inline static constexpr int durations[19]={20,20,60,30,30,30,30,10,10,10,10,10,600,10,10,10,10,30,20};
    inline static constexpr int modes[19]={4,4,2,6,3,6,6,6,6,4,4,4,0,4,4,4,4,4,4};
    int character=-1,age=0,release=0;
    bool held=false;
    float x=0,y=0,radius=0,release_radius=0;
    float minor=0,angle=0,last_px=0,speed=1;
    int count=1;
    std::array<Point,3> centers{};
    static bool supported(int ch){return ch>=0&&ch<19;}
    Point rotated(float dx,float dy,float px,float py)const{return {px+dx*std::cos(angle)-dy*std::sin(angle),py+dx*std::sin(angle)+dy*std::cos(angle)};}
    void tick(int ch,bool down,float px,float py,float strength=100){
        const float dx=character==ch?px-last_px:0;
        last_px=px;
        if(character!=ch){held=false;age=0;angle=0;}
        character=ch;down=down&&supported(ch);bool begin=down&&!held;
        if(begin){age=0;release=0;x=px;y=py;last_px=px;speed=1;angle=0;}
        if(!down&&held){release=20;release_radius=radius;}
        held=down;
        if(down){
            ++age;float power=std::clamp(strength,0.f,200.f)/100,start=ch==12?16.f:1.f;
            radius=start+(targets[ch]*power-start)*nativeEase(modes[ch],float(age)/durations[ch]);minor=radius;
            float tilt=(dx<-.1f?-1:dx>.1f?1:0)*(ch==17?14:ch==18?16:ch==15&&!begin?20:7)*3.141592654f/180;
            if(begin)angle=tilt;else if(ch!=7)angle+=(tilt-angle)*.1f;
            if(ch!=5&&ch!=7){x=px;y=ch==1?0:ch==6?py-160:py;}
            if(ch==5&&age<=2){x=px;y=py;}
            if(ch==7){if(begin||y<-60){x=px;y=py;speed=1;age=1;angle=(dx<-.1f?-6:dx>.1f?6:0)*3.141592654f/180;}x+=std::sin(angle)*speed;y-=std::cos(angle)*speed;speed+=.1f;}
            if(ch==4){if(age>30&&age<=40){radius=(32+32*nativeEase(5,float(age-30)/10))*power;minor=(32+160*nativeEase(5,float(age-30)/10))*power;}else if(age>40){radius=(64+16*nativeEase(2,float(age-40)/10))*power;minor=(192-176*nativeEase(2,float(age-40)/10))*power;}}
            if(ch==11)minor=1+(32*power-1)*nativeEase(4,float(age)/10);
            if(ch==18)minor=1+(64*power-1)*nativeEase(4,float(age)/20);
            if(ch==17){auto p=rotated(0,48,px,py);x=p.x;y=p.y;}
            count=ch==9||ch==13||ch==15?3:1;centers[0]={x,y};
            if(ch==9){centers[0]=rotated(0,-96,px,py);centers[1]=rotated(64,-64,px,py);centers[2]=rotated(-64,-64,px,py);}
            // Enoko 0x429c50: the leading circle is 128 units ahead, not on
            // the player; all three centers rotate with her movement tilt.
            if(ch==15){centers[0]=rotated(0,-128,px,py);x=centers[0].x;y=centers[0].y;centers[1]=rotated(96,-40,px,py);centers[2]=rotated(-96,-40,px,py);}
        }else if(release>0){--release;radius=release_radius*float(release*release)/400;}
        else radius=0;
    }
    void seek(float tx,float ty){if(character!=5||!held||age<=2)return;float dx=tx-x,dy=ty-y,d=std::hypot(dx,dy);if(d>2){dx*=4/d;dy*=4/d;}x+=dx;y+=dy;centers[0]={x,y};}
    bool contains(float px,float py,float object_radius=0)const{
        if(!held)return false;
        const float dx=px-x,dy=py-y;
        if(character==1)return std::abs(dx)<radius;
        if(character==0)return dx*dx+dy*dy<radius*radius+object_radius*object_radius;
        if(character==3)return (std::abs(dx)<=radius*3.5f+object_radius&&std::abs(dy)<=radius*.5f+object_radius)||(std::abs(dy)<=radius*3.5f+object_radius&&std::abs(dx)<=radius*.5f+object_radius);
        if(character==4||character==11){float a=character==4?-1.570796327f:angle,c=std::cos(a),s=std::sin(a),u=dx*c+dy*s,v=-dx*s+dy*c;return u*u/((minor+object_radius)*(minor+object_radius))+v*v/((radius+object_radius)*(radius+object_radius))<=1;}
        // Orin's native 0x41ca40 explicitly passes angle 0 and dimensions
        // (2r,r) to 0x4d44e0, even though her visual follows movement angle.
        if(character==7||character==8){float a=character==7?0:angle,c=std::cos(a),s=std::sin(a),u=dx*c+dy*s,v=-dx*s+dy*c,w=radius*(character==7?2:4)*.5f,h=radius*.5f;float qx=std::max(std::abs(u)-w,0.f),qy=std::max(std::abs(v)-h,0.f);return qx*qx+qy*qy<=object_radius*object_radius;}
        if(character==14)return std::abs(std::hypot(dx,dy)-128)<=radius*.5f+object_radius;
        if(character==10||character==18){int n=character==10?6:10;float a=angle-1.570796327f;bool inside=false;
            auto point=[&](int i){float r=character==18&&i%2?minor:radius,t=a+i*6.283185307f/n;return Point{std::cos(t)*r,std::sin(t)*r};};
            for(int i=0,j=n-1;i<n;j=i++){auto p=point(i),q=point(j);if((p.y>dy)!=(q.y>dy)&&dx<(q.x-p.x)*(dy-p.y)/(q.y-p.y)+p.x)inside=!inside;float vx=q.x-p.x,vy=q.y-p.y,t=std::clamp(((dx-p.x)*vx+(dy-p.y)*vy)/(vx*vx+vy*vy),0.f,1.f);if(std::hypot(dx-p.x-t*vx,dy-p.y-t*vy)<=object_radius)return true;}return inside;}
        for(int i=0;i<count;++i){float u=px-centers[i].x,v=py-centers[i].y,r=radius+object_radius;if(u*u+v*v<=r*r)return true;}return false;
    }
};
}
