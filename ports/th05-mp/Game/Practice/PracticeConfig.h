#pragma once
#include "PracticeSections.h"
namespace th5w { namespace practice {
// Versioned replay payload: fixed-width x86 integers, never raw bool/padding.
struct Config {
 int mode,stage,warp,frame,lives,bombs,score,power,graze,points,totalPoints,dream,rank,lockRank,section,rage,trigger,startHP,dialogue;
 Config():mode(0),stage(0),warp(0),frame(0),lives(8),bombs(8),score(0),power(128),graze(0),points(0),totalPoints(0),dream(128),rank(32),lockRank(0),section(0),rage(0),trigger(0),startHP(-1),dialogue(0){}
 bool HasDialogue() const {
  if(!mode||warp!=2||rage)return false;
  if(!section)return true;
  const Section* selected=SelectedSection(stage,warp,section);
  if(!selected)return false;
  if(stage==3)return selected->variant?selected->phase==3:section==1;
  return section==1;
 }
 static int Bound(int n,int lo,int hi){return n<lo?lo:n>hi?hi:n;}
 static int RankMin(int difficulty){const int v[]={16,24,44,48,32};return v[Bound(difficulty,0,4)];}
 static int RankMax(int difficulty){const int v[]={32,40,54,58,32};return v[Bound(difficulty,0,4)];}
 void Clamp(int difficulty){
  mode=Bound(mode,0,1);stage=Bound(stage,0,6);warp=Bound(warp,0,3);frame=Bound(frame,0,60000);
  lives=Bound(lives,0,8);bombs=Bound(bombs,0,8);score=Bound(score,0,99999990)/10*10;
  power=Bound(power,1,128);graze=Bound(graze,0,999);points=Bound(points,0,999);totalPoints=Bound(totalPoints,0,99999);dream=Bound(dream,1,128);
  int count=0;Sections(stage,warp,count);section=mode?Bound(section,0,count):0;
  const Section* selected=SelectedSection(stage,warp,section);
  rage=selected?Bound(rage,0,selected->rageCount):0;
  const bool entryChoice=selected&&warp==1&&stage==1&&selected->phase==2;
  const bool rageChoice=selected&&warp==2&&stage==5&&selected->phase==10&&rage;
  trigger=entryChoice||rageChoice?Bound(trigger,0,1):0;
  startHP=selected&&selected->continuous&&startHP!=-1?Bound(startHP,selected->endHP+1,selected->startHP):-1;
  if(selected&&warp==1&&stage==0&&startHP>=0)rage=startHP<=200?1:0;
  dialogue=HasDialogue()&&dialogue?1:0;
  lockRank=lockRank?1:0;rank=Bound(rank,RankMin(difficulty),RankMax(difficulty));
 }
 bool Valid(int difficulty) const {
  Config copy=*this;copy.Clamp(difficulty);
  return mode==copy.mode&&stage==copy.stage&&warp==copy.warp&&frame==copy.frame&&lives==copy.lives&&bombs==copy.bombs&&score==copy.score&&power==copy.power&&graze==copy.graze&&points==copy.points&&totalPoints==copy.totalPoints&&dream==copy.dream&&rank==copy.rank&&lockRank==copy.lockRank&&section==copy.section&&rage==copy.rage&&trigger==copy.trigger&&startHP==copy.startHP&&dialogue==copy.dialogue;
 }
 // Older TPC5 recordings allowed locked Rank outside the native range.
 // Preserve their simulation values when reading or saving a replay only.
 bool ValidReplay(int difficulty) const {
  Config copy=*this;
  if(lockRank==1){
   if(rank<0||rank>64)return false;
   copy.rank=Bound(rank,RankMin(difficulty),RankMax(difficulty));
  }
  return copy.Valid(difficulty);
 }
};
static_assert(sizeof(Config)==19*4,"Practice replay field size");
} }
