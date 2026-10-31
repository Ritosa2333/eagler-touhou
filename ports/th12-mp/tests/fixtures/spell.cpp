#include "../../src/spell.hpp"
#include <cstring>
using namespace th12::spell_system;
State state;Tick lastTick;Result result;int32_t score;
int values[40];float floats[2];uint8_t text[1024];
extern "C" {
void spell_reset(){state.reset();score=0;lastTick={};result={};}
int spell_start(int difficulty,int stage,int flags,int bomb,int limit,int replay){
 state.flags=flags;Start start;start.cardId=3;start.timeLimit=limit;start.difficulty=difficulty;start.stage=stage;start.bombActive=bomb;start.replay=replay;start.bossX=40;start.bossY=128;start.title="Research";return state.start(start);
}
void spell_survival(){state.setSurvival();}
void spell_attempts(int before){state.loadoutRecord.attempts=before;state.allRecord.attempts=before;}
void spell_retry(int replay){Start start;start.cardId=3;start.replay=replay;start.title="Research";start.loadoutRecord=state.loadoutRecord;start.allRecord=state.allRecord;state.start(start);}
void spell_prepare(int flags,int age,int bonus,int scoreBefore,int captures,int totalCaptures){state.flags=flags;state.age=age;state.bonusDisplayed=bonus;state.loadoutRecord.captures=captures;state.allRecord.captures=totalCaptures;score=scoreBefore;}
void spell_tick(int bomb,float y,float bossX,float bossY){lastTick=state.tick(bomb,y,bossX,bossY);}
int spell_loss(int cause,int bomb){return cause==1?state.onMiss(bomb):cause==2?state.onBomb(bomb):state.onTimeout(bomb);}
void spell_end(){result=state.end(score);}
const int* spell_snapshot(){
 values[0]=state.flags;values[1]=state.age;values[2]=state.previousAge;values[3]=state.cardId;values[4]=state.bonusDisplayed;values[5]=state.initialBonusDisplayed;values[6]=state.timeLimit;values[7]=state.serialTicks;values[8]=lastTick.bannerInterrupt;values[9]=lastTick.openedGameplay;values[10]=state.loadoutRecord.attempts;values[11]=state.allRecord.attempts;values[12]=score;values[13]=int(result.kind);values[14]=state.loadoutRecord.captures;values[15]=state.allRecord.captures;values[16]=result.bonusDisplayed;values[17]=result.scoreDeltaStored;values[18]=int(result.failure);values[19]=state.pendingBomb();return values;
}
const float* spell_ring(){floats[0]=state.ringX;floats[1]=state.ringY;return floats;}
uint8_t* spell_text(){return text;}
int spell_decrypt(int n){state.title=decryptTitle(text,n);return state.title.size();}
const char* spell_title(){return state.title.c_str();}
int spell_card(int op,int base,int difficulty){return resolveCardId(op,base,difficulty);}
const int* spell_bindings(int stage,int stageFrame,int captured,int bonus){const auto bindings=stage?startBindings(stage,stageFrame):resultBindings(captured,bonus);for(int i=0;i<int(bindings.size());++i){values[i*3]=int(bindings[i].bank);values[i*3+1]=bindings[i].script;values[i*3+2]=bindings[i].layer;}return values;}
const int* spell_digits(int bonus){const auto digits=resultDigits(bonus);for(int i=0;i<8;++i){values[i*2]=digits[i].sprite;values[i*2+1]=digits[i].visible;}return values;}
}
