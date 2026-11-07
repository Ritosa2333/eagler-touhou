#include "OriginalFeatures.h"
#include "../Game.h"
#include <algorithm>
#include <cstdlib>
namespace th5w { namespace original {
namespace {RunStats stats;DemoState demo;int frameDelay=1;
uint32_t Fraction(uint16_t total,uint16_t share){return total?(total==share?1000000u:(1000000u/total)*share):0;}
void ReadDemo(int file){char name[16];sprintf_s(name,"DEMO%d.REC",file);auto& pack=CGame::s_pCurGame->m_th5Dat2;const int idx=pack.GetChildFileIndex(name);demo.data.clear();if(idx>=0){auto* p=pack.GetChildFileContentPointer(idx);const int n=pack.GetChildFileLength(idx);demo.data.assign(p,p+n);}}
}
RunStats& Stats(){return stats;}
void BeginTimingFrame(){frameDelay=1;}
int FrameDelay(){return frameDelay;}
void BulletSlowdown(int bullets,int frame,int difficulty){
 // ReC98 th04/main/bullet/update.cpp (GAME=5): rank means selected difficulty.
 // Its second, higher-threshold else-if is unreachable in the original.
 if(!CGame::GVar().m_runTurboMode&&!(frame&1)&&bullets>=42+difficulty*8)frameDelay=2;
}
void BeginRun(){if(!demo.active)demo.number=0;stats=RunStats();const auto& g=CGame::GVar();stats.initialLives=g.m_nCurLife;stats.initialBombs=g.m_nCurBomb;stats.difficulty=g.m_playDifficulty;stats.stage=g.m_playStage;stats.turbo=g.m_runTurboMode;BeginTimingFrame();}
void Frame(bool scrolling,int score){++stats.frames;if(scrolling)++stats.stdFrames;stats.highestScore=(std::max)(stats.highestScore,score*10);stats.pendingFrame=true;}
void PausedFrame(){++stats.frames;++stats.slowFrames;stats.pendingFrame=false;}
void RenderTiming(bool slow){if(stats.pendingFrame&&(slow||frameDelay>1))++stats.slowFrames;stats.pendingFrame=false;}
void EndStage(int stage,int score,bool clear){stats.stage=stage;stats.finalScore=score*10;stats.highestScore=(std::max)(stats.highestScore,stats.finalScore);if(stage<6)stats.stageScores[stage]=stats.finalScore;if(clear&&stage>=5){stats.endSequence=stage==6?1:(CGame::GVar().m_nContinueUsed?2:3);stats.complete=true;}}
void ItemSpawned(){++stats.itemsSpawned;}void ItemCollected(bool point,bool maxPoint){++stats.itemsCollected;if(point)++stats.points;if(maxPoint)++stats.maxPoints;}
void EnemyGone(bool killed){++stats.enemiesGone;if(killed)++stats.enemiesKilled;}void Miss(){++stats.misses;}void Bomb(){++stats.bombs;}
Verdict Evaluate(const RunStats& s){
 Verdict v;const int rank=s.stage==6?4:s.difficulty;
 const uint16_t total=s.stage==6?12800:46000,done=s.complete?total:s.stdFrames;
 v.fractions[0]=Fraction(total,done);v.fractions[1]=Fraction(s.enemiesGone,s.enemiesKilled);v.fractions[2]=Fraction(s.itemsSpawned,s.itemsCollected);v.fractions[3]=Fraction(s.points,s.maxPoints);
 const int lives=(std::max)(0,(std::min)(5,s.initialLives-1));const int liveBonus[]={2500,2000,1500,1000,500,0};
 int spirit=liveBonus[lives]+(s.initialBombs==0?2500:s.initialBombs==1?1500:s.initialBombs==2?1000:0)+(s.turbo?2000:0)+uint16_t(s.graze*3)+(int(s.misses)-int(s.bombs))*200;
 spirit=(std::max)(0,(std::min)(10000,spirit));v.fractions[4]=uint32_t(spirit*100);
 v.fractions[5]=Fraction(uint16_t(s.frames/10),uint16_t(s.slowFrames/10));
 int skill=(int(v.fractions[0])+int(v.fractions[1])+int(v.fractions[2])+int(v.fractions[3])+int(v.fractions[4])-int(v.fractions[5]))/12+int(v.fractions[0]/4);
 skill+=(std::min)(500000,(s.highestScore/1000000)*5000);
 const int caps[]={800000,1000000,1200000,1400000,2000000};int cap=caps[rank];
 if(rank==0)skill-=50000;else if(rank==2)skill=skill*5/4+150000;else if(rank==3)skill=skill*3/2+300000;else if(rank==4)skill=skill*3/2+250000;
 const int sb[]={50000,25000,0,0,0,0},cb[]={100000,50000,0,-25000,-50000,-75000};skill+=sb[lives];cap+=cb[lives];
 const int bombSkill[]={50000,30000,20000},bombCap[]={100000,50000,25000};if(s.initialBombs<3){skill+=bombSkill[s.initialBombs];cap+=bombCap[s.initialBombs];}
 if(!s.turbo){skill-=200000;cap-=100000;}skill-=(std::min)(10,int(s.misses))*30000+(std::min)(15,int(s.bombs))*15000;if(s.endSequence<1)skill=skill*7/8;
 v.maximum=cap;v.skill=(std::max)(0,(std::min)(cap,skill));v.valid=(s.frames/2)>s.slowFrames;
 v.row=v.skill>=1500000?0:v.skill>=1350000?1:v.skill>=1200000?2:v.skill>=1050000?3:v.skill==0?25:24-v.skill/50000;
 if(s.endSequence==3||s.endSequence==1)v.comment=rank==0?4:s.finalScore<80000000?1:s.misses>=6?7:s.legacyUnknown>=15?8:0;
 else if(s.endSequence==2)v.comment=2;else v.comment=s.bombs<=s.misses*2?5:s.stage>=4&&s.points<=350?6:3;
 return v;
}
DemoState& Demo(){return demo;}bool DemoActive(){return demo.active;}
int IRandom(){if(!demo.active)return std::rand();demo.seed=demo.seed*0x015a4e35u+1;return int((demo.seed>>16)&32767);}
int Random(){
 if(!demo.active)return std::rand();
 // randring1 and randring2 share one byte cursor. The word read at 255
 // includes the cursor byte itself, before INC wraps it back to zero.
 const unsigned int i=demo.ringIndex;const int value=demo.ring[i]|((i==255?demo.ringIndex:demo.ring[i+1])<<8);++demo.ringIndex;return value;
}
void ResetDemoSeed(){if(demo.active){demo.bulletTemplateAngle=0;demo.seed=318;for(int i=255;i>=0;i--)demo.ring[i]=static_cast<unsigned char>(IRandom());demo.ringIndex=0;demo.itemDropIndex=static_cast<unsigned char>(IRandom()&15);}}
void StopDemo(bool reset){demo.active=false;demo.data.clear();demo.index=0;demo.extraPart2=false;if(reset)demo.number=0;}
bool StartDemo(bool extra){
 auto& g=CGame::GVar();if(extra){for(int i=0;i<4;i++)if(!g.m_bExtraPlayable[i]){demo.number=0;return false;}}demo.number=extra?5:demo.number%4+1;demo.frames=extra?20000:5000;demo.extraPart2=false;demo.index=0;ReadDemo(demo.number-1);if(demo.data.size()!=size_t(demo.frames*2)){StopDemo();return false;}
 const int chars[]={0,1,2,3,2},stages[]={3,1,2,4,6};g.m_bReplayMode=g.m_bPracticeMode=false;g.m_playChara=chars[demo.number-1];g.m_playStage=stages[demo.number-1];g.m_playDifficulty=extra?4:3;demo.active=true;g.OnBeginGame();
 g.m_nCurLife=3;g.m_nCurBomb=3;g.m_nBombOnMiss=3;g.m_curPower=extra?1:128;g.m_curDream=1;g.m_curScore=g.m_totalNPointItem=0;g.m_playerPerformance=extra?32:48;if(extra)g.m_playerPerformanceUpperBound=36;g.m_randomSeed[g.m_playStage]=318;g.m_nContinueLimit=0;g.m_bCanSaveReplay=false;ResetDemoSeed();return true;
}
bool DemoInput(unsigned short live,unsigned short& input){
 if(!demo.active)return false;if((live&~(15|64|128))||(!demo.extraPart2&&demo.number<=4&&demo.index>=4996)||demo.index>=demo.frames)return false;
 input=demo.data[demo.index]|(demo.data[demo.frames+demo.index]?64:0);++demo.index;return true;
}
void DemoDialogueEnded(){if(demo.active&&demo.number==5&&!demo.extraPart2){ReadDemo(5);demo.index=0;demo.extraPart2=true;}}
} }
