// TH12 1.00b player resources and presentation state. Native evidence is recorded
// in artifacts/player-fixes-evidence.md; adjacent titles are not behavioral input.
#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <vector>
namespace th12 {
struct PlayerOffset {float x=0,y=0;};
struct ShotSpec {
  int interval=0,delay=0,damage=0,option=0,animation=0,spawn=0,update=0;
  float x=0,y=0,w=0,h=0,angle=0,speed=0;
  int type=0,hitAnimation=0,sound=0,hitCallback=0,extraCallback=0;
};
struct Loadout {
  float hit=2,speed=4.5,focus=2,diag=3.18198,focusDiag=1.41421;
  int maxLevel=4,powerStep=100;
  float attractionSpeed=6,attractionDiameter=28;
  std::array<PlayerOffset,72> optionOffsets{};
  std::vector<std::vector<ShotSpec>> groups;
  template<class T>static T read(const uint8_t* p){T out;std::memcpy(&out,p,sizeof out);return out;}
  bool open(const uint8_t* p,int n){
    if(!p||n<0x268||read<uint16_t>(p)!=3)return false;
    Loadout next;const int count=read<uint16_t>(p+2);
    next.hit=read<float>(p+4);next.attractionSpeed=read<float>(p+8);next.attractionDiameter=read<float>(p+12);
    next.speed=read<float>(p+16);next.focus=read<float>(p+20);next.diag=read<float>(p+24);next.focusDiag=read<float>(p+28);next.maxLevel=read<int32_t>(p+32);
    // Native loader 435c95 overwrites the retail 40 with 100 before group selection.
    next.powerStep=100;
    if(next.maxLevel!=4||count!=2*(next.maxLevel+1)||count>(n-0x268)/8)return false;
    for(float v:{next.hit,next.attractionSpeed,next.attractionDiameter,next.speed,next.focus,next.diag,next.focusDiag})if(!std::isfinite(v)||v<0||v>100)return false;
    for(int i=0;i<72;i++){
      auto& o=next.optionOffsets[i];o={read<float>(p+40+i*8),read<float>(p+44+i*8)};
      if(!std::isfinite(o.x)||!std::isfinite(o.y))return false;
    }
    const uint32_t tableEnd=0x268+count*8;
    for(int i=0;i<count;i++){
      uint32_t off=read<uint32_t>(p+0x268+i*8);std::vector<ShotSpec> specs;bool terminated=false;
      if(off<tableEnd||off>=uint32_t(n))return false;
      for(int k=0;k<1000;k++,off+=52){
        if(off>=uint32_t(n))return false;
        if(int8_t(p[off])<0){terminated=true;break;}
        if(off>uint32_t(n)-52)return false;
        ShotSpec s;
        s.interval=int8_t(p[off]);s.delay=int8_t(p[off+1]);s.damage=read<int16_t>(p+off+2);
        s.x=read<float>(p+off+4);s.y=read<float>(p+off+8);s.w=read<float>(p+off+12);s.h=read<float>(p+off+16);s.angle=read<float>(p+off+20);s.speed=read<float>(p+off+24);
        s.option=int8_t(p[off+28]);s.type=int8_t(p[off+29]);s.animation=read<int16_t>(p+off+30);
        s.hitAnimation=read<int16_t>(p+off+32);s.sound=read<int16_t>(p+off+34);
        s.spawn=read<uint32_t>(p+off+36);s.update=read<uint32_t>(p+off+40);s.hitCallback=read<uint32_t>(p+off+44);s.extraCallback=read<uint32_t>(p+off+48);
        if(s.interval<=0||s.delay<0||s.delay>=s.interval||s.option<0||s.option>4||s.type<0||s.type>2||s.spawn>6||s.update>5||s.hitCallback>2||s.extraCallback>1)return false;
        for(float v:{s.x,s.y,s.w,s.h,s.angle,s.speed})if(!std::isfinite(v))return false;
        if(s.w<0||s.h<0||s.speed<0)return false;
        specs.push_back(s);
      }
      if(!terminated)return false;
      next.groups.push_back(std::move(specs));
    }
    *this=std::move(next);return true;
  }
};
struct PlayerOption {int x=0,y=0,age=0;};
struct PlayerMotion {
  std::array<PlayerOption,4> options{};
  int count=0,bodyAnimation=0,bodyAge=0,previousDx=0;
  // Creation 4385b0 uses triangular indices, then low-speed offsets at +10.
  static constexpr int formationStart[4]={0,1,3,6};
  static constexpr int optionScripts[6]={18,19,11,12,15,16};
  void body(int dx){
    int script=-1;
    if(dx<0&&previousDx>=0)script=1;else if(dx>0&&previousDx<=0)script=3;
    else if(!dx&&previousDx<0)script=2;else if(!dx&&previousDx>0)script=4;
    if(script>=0){bodyAnimation=script;bodyAge=0;}else ++bodyAge;
    previousDx=dx;
  }
  void updateOptions(const Loadout& s,int power,int x,int y,bool focused,bool force=false){
    const int nextCount=std::clamp(power/s.powerStep,0,s.maxLevel);
    const bool rebuild=force||nextCount!=count;count=nextCount;
    for(int i=0;i<count;i++){
      auto& o=options[i];const auto& p=s.optionOffsets[formationStart[count-1]+i+(focused?10:0)];
      const int tx=x+int(double(p.x)*128),ty=y+int(double(p.y)*128);
      if(rebuild){o.x=tx;o.y=ty;o.age=0;}
      else {const int dx=(tx-o.x)*30/100,dy=(ty-o.y)*30/100;
        if(!dx&&!dy){o.x=tx;o.y=ty;}else{o.x+=dx;o.y+=dy;}++o.age;}
    }
  }
};
struct ShotSchedule {
  int frame=-1;
  int begin(bool held,bool allowed){
    if(!allowed){frame=-1;return -1;}
    if(frame<0){if(!held)return -1;frame=0;}
    return frame;
  }
  void finish(bool held){
    // Original 439a40: complete the 0..14 cycle after Z is released.
    if(frame<0)return;
    if(frame>=14)frame=held?frame-14:-1;else ++frame;
  }
};
}
