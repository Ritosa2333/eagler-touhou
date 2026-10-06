// Hand-authored TH12 1.00b spell rules. Retail evidence: 40e060, 40db40,
// 40e5e0, 4382f3 and 41a8b8. Score is stored in display-score / 10 units.
#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <vector>
namespace th12::spell_system {
enum Flag:uint32_t {Active=1,Eligible=2,BannerShifted=4,Survival=8,TimeoutRing=16,PendingBomb=32,WallClockRunning=64};
enum class Failure:int {None=0,Miss=1,Bomb=2,Timeout=3};
enum class ResultKind:int {None=0,Captured=1,Failed=2};
struct Record {int attempts=0,captures=0;};
struct Start {
  int cardId=0,timeLimit=2400,difficulty=1,stage=1;
  bool replay=false,bombActive=false;
  float bossX=0,bossY=0;
  std::string title; // Decrypted Shift-JIS bytes; frontend can decode the bytes.
  Record loadoutRecord{},allRecord{};
};
struct Result {
  ResultKind kind=ResultKind::None;Failure failure=Failure::None;
  int cardId=0,bonusDisplayed=0,scoreDeltaStored=0,elapsedTicks=0;
  bool survivedTimeout=false;std::string title;
  Record loadoutRecord{},allRecord{};
};
struct Tick {bool valid=true;int bannerInterrupt=0;bool openedGameplay=false;};
// Semantic banks, not renderer numeric IDs. Binding must execute the original
// logical ANM bytecode separately; these descriptors consume no random numbers.
enum class VisualBank:int {Font=0,SpellText=1,Bullet=2,Stage=3,StageAlternate=4,Front=5};
struct Binding {VisualBank bank=VisualBank::Font;int script=0,layer=23;};
inline std::vector<Binding> startBindings(int stage,int stageFrame){
  if(stage<1||stage>7)return {};
  std::vector<Binding> out{{VisualBank::Font,1},{VisualBank::SpellText,74},{VisualBank::Font,2},{VisualBank::Bullet,127},{VisualBank::Bullet,137}};
  constexpr int backgrounds[7][2]={{14,21},{17,25},{11,18},{20,27},{11,18},{13,20},{12,20}};
  VisualBank bank=VisualBank::Stage;int first=backgrounds[stage-1][0],second=backgrounds[stage-1][1];
  if(stageFrame<24&&stage==5){bank=VisualBank::StageAlternate;first=11;second=14;}
  if(stageFrame<24&&stage==7){bank=VisualBank::StageAlternate;first=14;second=18;}
  out.push_back({bank,first});out.push_back({bank,second});return out;
}
inline std::vector<Binding> resultBindings(bool captured,int bonusDisplayed){
  std::vector<Binding> out{{VisualBank::Front,captured?43:44}};
  if(captured){for(int i=0;i<8;++i)out.push_back({VisualBank::Font,5+i});if(bonusDisplayed>=1000000)out.push_back({VisualBank::Font,13});if(bonusDisplayed>=1000)out.push_back({VisualBank::Font,14});}
  out.push_back({VisualBank::Front,78});return out;
}
struct BonusDigit {int script=0,sprite=243;bool visible=false;};
inline std::array<BonusDigit,8> resultDigits(int bonusDisplayed){
  std::array<BonusDigit,8> digits{};int divisor=10000000;bool visible=false;
  for(int i=0;i<8;++i){const int digit=bonusDisplayed/divisor;visible|=digit!=0;digits[i]={5+i,243+digit,visible};bonusDisplayed%=divisor;divisor/=10;}
  return digits;
}
inline std::string decryptTitle(const uint8_t* data,int length){
  if(!data||length<0||length>1024)return {};
  std::string title;uint8_t key=0x77,step=7;
  for(int i=0;i<length;++i){const uint8_t c=data[i]^key;if(!c)break;title.push_back(char(c));key=uint8_t(key+step);step=uint8_t(step+16);}
  return title;
}
inline int resolveCardId(int opcode,int base,int difficulty){return opcode==437?base+difficulty:opcode==438?base+difficulty-1:opcode==439?base+difficulty-2:base;}
struct State {
  uint32_t flags=0;int cardId=0,age=0,previousAge=-1,timeLimit=0;
  int bonusDisplayed=0,initialBonusDisplayed=0,serialTicks=0;
  float ringX=0,ringY=0;bool replay=false,timedOut=false;
  std::string title;Failure failure=Failure::None;
  Record loadoutRecord{},allRecord{};Result lastResult{};
  bool active()const{return flags&Active;}
  bool eligible()const{return flags&Eligible;}
  bool survival()const{return flags&Survival;}
  bool pendingBomb()const{return (flags&(Active|PendingBomb))==(Active|PendingBomb);}
  void reset(){*this={};}
  bool start(const Start& s){
    if(s.cardId<0||s.cardId>127||s.timeLimit<0||s.difficulty<0||s.difficulty>4||s.stage<1||s.stage>7)return false;
    cardId=s.cardId;age=0;previousAge=-1;timeLimit=s.timeLimit;serialTicks=1;replay=s.replay;timedOut=false;failure=Failure::None;
    title=s.title;ringX=s.bossX;ringY=s.bossY;loadoutRecord=s.loadoutRecord;allRecord=s.allRecord;
    flags=(flags&~uint32_t(Survival|TimeoutRing|PendingBomb|WallClockRunning))|Active|Eligible;
    if(s.bombActive)flags|=PendingBomb;
    // ECL's third numeric argument is read, but the retail function ignores it.
    bonusDisplayed=(s.difficulty+s.stage)*2000000;initialBonusDisplayed=std::min(bonusDisplayed,99999999);
    if(!replay){if(loadoutRecord.attempts<99999)++loadoutRecord.attempts;if(allRecord.attempts<99999)++allRecord.attempts;}
    return true;
  }
  void setSurvival(){flags|=Survival;}
  void markTimeoutRing(){flags|=TimeoutRing;}
  bool lose(Failure cause,bool bombActive){
    if(!active())return false;
    if(age>=60){bonusDisplayed=0;flags&=~uint32_t(Eligible|PendingBomb);if(failure==Failure::None)failure=cause;return true;}
    if(bombActive)flags|=PendingBomb;
    return false;
  }
  bool onMiss(bool bombActive){return lose(Failure::Miss,bombActive);}
  bool onBomb(bool bombActive=true){return lose(Failure::Bomb,bombActive);}
  bool onTimeout(bool bombActive){if(!active())return false;timedOut=true;return survival()?false:lose(Failure::Timeout,bombActive);}
  Tick tick(bool bombActive,float playerY,float bossX,float bossY){
    Tick result;if(!active())return result;++serialTicks;
    if(age>=300&&!survival()){
      const int denominator=timeLimit-300;
      // A malformed <=300 limit reaches native idiv0 or an unintended negative
      // divisor; report it instead of inventing a useful gameplay reward.
      if(denominator<=0){result.valid=false;return result;}
      const int decrement=(initialBonusDisplayed-initialBonusDisplayed/4)/denominator;
      const int next=bonusDisplayed-decrement;bonusDisplayed=next-next%10;
    }
    previousAge=age;++age;result.openedGameplay=previousAge==60;
    if(age>=120){if(!(flags&BannerShifted)&&playerY<96){flags|=BannerShifted;result.bannerInterrupt=3;}
      else if((flags&BannerShifted)&&playerY>128){flags&=~uint32_t(BannerShifted);result.bannerInterrupt=2;}}
    const float dx=float(double(bossX)-ringX),dy=float(double(bossY)-ringY);
    ringX=float(double(ringX)+float(double(dx)*.05000000074505806));ringY=float(double(ringY)+float(double(dy)*.05000000074505806));
    if((flags&PendingBomb)&&!bombActive)flags&=~uint32_t(PendingBomb);
    return result;
  }
  Result end(int32_t& scoreStored){
    if(!active())return {};
    const bool captured=eligible();flags&=~uint32_t(Active|PendingBomb);
    Result result;result.kind=captured?ResultKind::Captured:ResultKind::Failed;result.failure=failure;result.cardId=cardId;result.bonusDisplayed=captured?bonusDisplayed:0;result.scoreDeltaStored=captured?bonusDisplayed/10:0;result.elapsedTicks=age;result.survivedTimeout=timedOut&&survival();result.title=title;
    if(captured){scoreStored=int32_t(std::min<int64_t>(999999999,int64_t(scoreStored)+result.scoreDeltaStored));if(!replay){if(loadoutRecord.captures<99999)++loadoutRecord.captures;if(allRecord.captures<99999)++allRecord.captures;}}
    result.loadoutRecord=loadoutRecord;result.allRecord=allRecord;lastResult=result;return result;
  }
};
}
