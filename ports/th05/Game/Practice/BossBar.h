#pragma once
namespace th5w { namespace practice {
struct BossBarData {
 bool present;int key,hp,start,end,remaining,enrage,cut2;
 BossBarData():present(false),key(0),hp(0),start(1),end(0),remaining(0),enrage(0),cut2(0){}
};
// TH06 Gui::UpdateStageElements: opacity +/-4, health fill +.01 / depletion -.02.
struct BossBarAnimation {
 float displayed;int opacity,key,rangeStart;BossBarData data;
 BossBarAnimation():displayed(0),opacity(0),key(-1),rangeStart(1){}
 void Step(BossBarData input){
  if(input.present){
   if(key!=input.key){key=input.key;rangeStart=input.start;}
   input.start=rangeStart;data=input;
   opacity=opacity<252?opacity+4:255;
   float target=float(input.hp-input.end)/float(input.start-input.end);
   if(target<0)target=0;if(target>1)target=1;
   if(displayed<target){displayed+=.01f;if(displayed>target)displayed=target;}
   else if(displayed>target){displayed-=.02f;if(displayed<target)displayed=target;}
  }else{
   opacity=opacity>4?opacity-4:0;
   if(!opacity){displayed=0;key=-1;data=BossBarData();}
  }
 }
};
// TH15 GameHudFrame: fill at 0.025/frame, clamp immediately on damage,
// and dim inside 80px with a 96px recovery threshold. Positions use native
// game pixels; the 112px ring radius in TH15's doubled HUD is 56px here.
struct BossRingData {
 bool present;int key,hp,full,markerCount;float x,y;int markers[24];
 BossRingData():present(false),key(-1),hp(0),full(1),markerCount(0),x(0),y(0),markers{}{}
};
struct BossRingAnimation {
 BossRingData data;float displayed;bool dimmed;
 BossRingAnimation():displayed(0),dimmed(false){}
 void Step(const BossRingData& input,float playerX,float playerY){
  if(!input.present){*this=BossRingAnimation();return;}
  if(data.key!=input.key){displayed=0;dimmed=false;}
  const int previousFull=data.key==input.key?data.full:input.full;
  data=input;if(data.full<previousFull)data.full=previousFull;
  float target=float(input.hp)/float(data.full);if(target<0)target=0;if(target>1)target=1;
  if(displayed<target){displayed+=.025f;if(displayed>target)displayed=target;}else if(displayed>target)displayed=target;
  const float dx=input.x-playerX,dy=input.y-playerY,distance=dx*dx+dy*dy;
  if(!dimmed&&distance<6400)dimmed=true;else if(dimmed&&distance>=9216)dimmed=false;
 }
};
} }
