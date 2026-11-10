#pragma once
#include <array>
#include <algorithm>
#include <cmath>
namespace th19 {
    // Unenhanced release branch of each native attack-controller vtable.
// These clocks drive SHT group 10, independently of the ordinary Z-shot clock.
// Character 10/12 use native area effects instead of this SHT path.
struct ChargeAttack {
    int character=0,frame=-1,emitted=0,group=10;
    // Shared Skill entry 0x423f80, followed by each controller's update.
    // Other characters have independent Skill objects/state, not group 11.
    static int skill_fire_frames(int ch){return ch==2?60:ch==4?90:ch==6?200:ch==7?10:ch==11?60:ch==13?180:ch==14?80:ch==15?60:ch==17?200:0;}
    bool release_skill(int ch){
        if(!skill_fire_frames(ch)||(frame>=0&&group==11))return false;
        character=ch;frame=0;group=11;return true;
    }
    static constexpr std::array<int,19> fire_frames{21,1,30,50,30,120,30,10,50,50,0,50,0,80,50,10,50,100,90};
    static constexpr std::array<int,19> end_frames{21,21,41,61,41,121,41,11,61,61,0,61,0,91,61,16,61,111,101};
    // 0x4e83b9..0x4e8440: C3/C4 still reach the ordinary release.
    // Only the specialized C2 paths defer/suppress it in this runtime.
    static bool ordinary_after_release(int ch,int tier,bool skillAccepted){
        return tier>=1&&(tier!=2||!(ch==18&&skillAccepted));
    }
    bool release(int ch){
        if(ch<0||ch>=19||fire_frames[ch]==0)return false;
        // 0x423ef0 and 0x42e630 reject retriggering an active release.
        if(frame>=0&&(group==11||ch==11||ch>=13))return false;
        character=ch;frame=0;group=10;return true;
    }
    bool firing()const{return frame>=0&&frame<(group==11?skill_fire_frames(character):fire_frames[character]);}
    int secondary_clock()const{return character>=11?frame:0;}
    // Orin 0x41d1b0: both branches reset when the release clock exceeds 10.
    int end_frame()const{return group==11?(character==7?11:skill_fire_frames(character)+11):end_frames[character];}
    bool delayed_versus_beam(bool versus)const{return versus&&character==1&&group==10;}
    // Marisa updates the controller after ordinary fire checks 0x417210.
    // The charge manager sees the idle controller on that final update.
    bool busy_for_charge(bool versus)const{
        return frame>=0&&!(delayed_versus_beam(versus)&&frame==end_frame());
    }
    void tick(bool versus=false){if(frame>=0&&++frame>=end_frame()+int(delayed_versus_beam(versus)))frame=-1;}
};
// Ran 0x419ae0 / 0x419c00: timed power state, independent of shot clock.
struct RanSkill {
    int remaining=0;
    bool release(){if(remaining>0)return false;remaining=600;return true;}
    bool active()const{return remaining>0;}
    void tick(){if(remaining>0)--remaining;}
    int power(int normal)const{return active()?4:normal;}
};
// Mamizou 0x420380 -> 0x537b90: one fragment, three fragments per bomb.
inline bool mamizouSkillFragment(int& stock,int& fragments,int capacity=7){
    if(stock>=capacity){fragments=0;return false;}
    if(++fragments>=3){fragments=0;++stock;}return true;
}
// 0x421f80/0x4222d0: Yachie, fixed release origin, six-sided area.
// 0x4251f0/0x425520: Yuuma, circle follows the player. Normal branch only.
struct ChargeArea {
    int character=-1,frame=-1;
    int converted_count=0,conversion_sequence=0;
    bool enhanced=false;
    float x=0,y=0;
    bool release(int ch,float px,float py){
        if((ch!=10&&ch!=12)||frame>=0)return false;
        character=ch;frame=0;x=px;y=py;enhanced=false;return true;
    }
    bool release_skill(int ch,float px,float py){if(ch==5||ch==7||ch==8||ch==9||ch==18){if(frame>=0)return false;character=ch;frame=0;x=px;y=py;converted_count=0;}else if(!release(ch,px,py))return false;enhanced=true;return true;}
    int duration()const{return character==7?60:character==8?170:character==5||character==18?90:character==9?1:character==10?(enhanced?110:40):(enhanced?40:20);}
    int script()const{return character==7||character==8?12:character==18?10:character==5||character==9?9:enhanced?10:character==10?8:9;}
    bool active()const{return frame>=0&&frame<duration();}
    int damage()const{return character==9?20:character==10?1:25;}
    float radius()const {
        if(character==7||character==8){float t=std::clamp((frame+1)/float(duration()),0.f,1.f);
            float a=character==7?1.f:20.f,b=character==7?500.f:420.f;return a+(b-a)*(1-(1-t)*(1-t));}
        if(character==18){float t=std::clamp((frame+1)/90.f,0.f,1.f);return 1+319*(1-(1-t)*(1-t));}
        if(character==5){float t=std::clamp((frame+1)/90.f,0.f,1.f);return 1+599*t*t;}
        if(character==9)return 40;
        const float t=std::clamp((frame+1)/float(enhanced?(character==10?60:50):10),0.f,1.f);
        if(enhanced&&character==12)return 64+(260-64)*t; // 0x425390: linear, 50 frames.
        // Native float interpolator mode 4, radius stat at its default 100%.
        return 1+((character==10?(enhanced?128.f:48.f):64.f)-1)*(1-(1-t)*(1-t));
    }
    bool contains(float px,float py)const {
        if(!active())return false;
        float dx=px-x,dy=py-y,r=radius();
        if(character==5||character==7||character==8||character==9||character==12||character==18)return dx*dx+dy*dy<=r*r;
        for(int i=0;i<6;++i){float a=(i+.5f)*6.28318530718f/6;if(dx*std::cos(a)+dy*std::sin(a)>r*.866025403784f)return false;}
        return true;
    }
    void tick(float px,float py){if(frame<0)return;if(character==12){x=px;y=py;}if(++frame>duration())frame=-1;}
};
}
