#include "../Game/Original/CombatThresholds.h"
// Compiled only into TH05_BROWSER_TESTS builds. Exercise the production game
// classes with actual DAT resources without rendering thousands of frames.
#include "../Game/Game.h"
#include "../Game/Original/OriginalScenes.h"
#include "../Game/Stage/Stage.h"
#include "../Game/Stage/Item/Item.h"
#include "../Game/Stage/Summary/Summary.h"
#include "../Game/Stage/Chara/Chara.h"
#include "../Game/Stage/CharaBullet/CharaBullet.h"
#include "../Game/Stage/Laser/Laser.h"
#include "../Game/Stage/StageResource.h"
#include "../Game/Stage/MidBoss/MidBoss.h"
#include "../Game/Stage/Background/Background.h"
#include "../Game/Stage/Boss/BossStage5.h"
#include "../Game/Stage/Boss/BossStage4.h"
#include "../Game/Stage/Boss/BossStage4Yuki.h"
#include "../Game/Stage/Boss/BossStage4Mai.h"
#include "../Game/Stage/Boss/BossStage6.h"
#include "../Game/Stage/Boss/BossStageEx.h"
#include "../portable/Graphics.hpp"
#include "../Game/Practice/Practice.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <cstring>
namespace th5w { namespace original { extern bool auditRandom;
static int auditSpecial[512],auditSpecialCount=0;
void AuditSpecialShot(int kind,int speed){
 if(auditRandom&&auditSpecialCount<512)auditSpecial[auditSpecialCount++]=kind*256+speed;
} } }
namespace th5w {
struct PracticeTests {
 static int AllCast(int field){
  auto* game=CGame::s_pCurGame;if(!game)return -1;
  if(field<0){
   if(field<-4)return -1;
   auto& g=CGame::GVar();g.m_playStage=6;g.m_playChara=-field-1;
   CScheme* next=nullptr;int id=0;
   if(!CSchemeSwitcher::SwitchScheme(&next,&id,5,1)||id!=10)return -1;
   delete game->m_pCurScheme;game->m_pCurScheme=next;game->m_curSchemeID=id;
   return static_cast<COriginalEnd*>(next)->m_ready?1:0;
  }
  if(game->m_curSchemeID!=10)return -1;
  const auto* cast=static_cast<const COriginalEnd*>(game->m_pCurScheme);
  if(field==0)return cast->m_castScreen;
  if(field==1)return cast->m_castState;
  if(field==2)return cast->m_measure;
  return cast->m_age;
 }
 static int Draw(int atlas){
  auto* game=CGame::s_pCurGame;if(!game||game->m_curSchemeID!=4)return 0;
  web::graphics::SpriteAtlasEnabled(atlas!=0);web::graphics::BeginFrame();practice::BeginFrame();game->m_pCurScheme->Draw();practice::DrawStatus();practice::EndFrame();web::graphics::Present();web::graphics::SpriteAtlasEnabled(true);return 1;
 }
 static int Effects(int kind){
  auto* game=CGame::s_pCurGame;if(!game||game->m_curSchemeID!=4)return 0;auto* s=static_cast<CStage*>(game->m_pCurScheme);
  if(kind==0){if(!s->m_pBoss)return 0;s->m_pBoss->AddExplodeEffect(0,192*16,100*16);s->m_pBoss->AddExplodeEffect(4,192*16,100*16);s->m_pEnemyBullet->SetClearBulletTimeLeft(20);}
  else if(kind==1){s->m_pChara->DropBomb();}return 1;
 }
 static int DemoStart(int number){
  auto* game=CGame::s_pCurGame;if(!game||number<1||number>5)return 0;delete game->m_pCurScheme;game->m_pCurScheme=nullptr;original::StopDemo(true);original::Demo().number=number-1;
  for(auto& unlocked:CGame::GVar().m_bExtraPlayable)unlocked=true;if(!original::StartDemo(number==5))return 0;
  auto* s=new CStage;game->m_pCurScheme=s;game->m_curSchemeID=4;s->Initialize();return 1;
 }
 static unsigned DemoInfo(int field){
  const auto& d=original::Demo();if(field==0)return d.active;if(field==1)return d.number;if(field==2)return d.index;if(field==3)return d.seed;
  auto* game=CGame::s_pCurGame;if(!game||game->m_curSchemeID!=4)return 0;auto* s=static_cast<CStage*>(game->m_pCurScheme);
  if(field==4)return unsigned(s->m_pChara->GetCurX());if(field==5)return unsigned(s->m_pChara->GetCurY());if(field==6)return unsigned(s->m_curFrame);
  unsigned hash=2166136261u;const auto* data=reinterpret_cast<const unsigned char*>(s->m_pEnemyBullet->m_bullets);for(size_t n=0;n<sizeof(s->m_pEnemyBullet->m_bullets);n++)hash=(hash^data[n])*16777619u;return hash;
 }
 static int Start(int stage,int warp,int character=0){
  auto* game=CGame::s_pCurGame;if(!game||stage<0||stage>6||warp<0||warp>2)return 0;
  auto& g=CGame::GVar();delete game->m_pCurScheme;game->m_pCurScheme=nullptr;original::StopDemo();g.m_replay.CloseStage(false);
  g.m_bReplayMode=false;g.m_bPracticeMode=true;g.m_playStage=stage;g.m_playChara=character;g.m_playDifficulty=stage==6?4:1;
  g.m_practiceConfig=practice::Config();g.m_practiceConfig.mode=1;g.m_practiceConfig.stage=stage;g.m_practiceConfig.warp=warp;g.m_practiceConfig.Clamp(g.m_playDifficulty);g.OnBeginGame();
  auto* current=new CStage;game->m_pCurScheme=current;game->m_curSchemeID=4;current->Initialize();current->m_bLogo=false;current->m_bCanPause=true;
  if(!practice::Enabled(practice::Invincible))practice::Toggle(practice::Invincible);return 1;
 }
 static inline int playerShots[1+64*7]={};
 static int ShotStart(int character,int power,int homing){
  if(character<0||character>3||power<1||power>128||!Start(0,0,character))return 0;
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  stage->m_pChara->SetPower(power);
  stage->m_pChara->m_curX=192*16;
  stage->m_pChara->m_curY=320*16;
  stage->m_pChara->m_cannotControlTimeLeft=0;
  stage->m_pChara->m_bCharaCanShoot=true;
  stage->m_pChara->m_invulnerableTimeLeft=10000;
  stage->m_bHomingEnemyExist=homing;
  stage->m_homingEnemyX=120*16;
  stage->m_homingEnemyY=140*16;
  return 1;
 }
 static bool ShotRules(){
  auto* game=CGame::s_pCurGame;
  return game&&game->m_curSchemeID==4&&
   static_cast<CStage*>(game->m_pCurScheme)->m_pCharaBullet->UsesOriginalShots();
 }
 static int* ShotStep(int frame){
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  stage->m_curFrame=frame;
  stage->m_curGameKeyState=32;
  struct AuditScope {
   AuditScope(){original::auditRandom=true;}
   ~AuditScope(){original::auditRandom=false;}
  } scope;
  stage->m_pChara->Step();
  auto* bullets=stage->m_pCharaBullet;
  bullets->Step();
  playerShots[0]=0;
  int* out=playerShots+1;
  for(const auto& b:bullets->m_bullets){
   if(b.flag!=1)continue;
   ++playerShots[0];
   *out++=int(b.curX*256);*out++=int(b.curY*256);
   *out++=int(b.velX*256);*out++=int(b.velY*256);
   *out++=b.imgIdx;*out++=b.bulletType;*out++=b.damage;
  }
  return playerShots;
 }
 static inline int motionData[14]={};
 static bool CombatStart(int stageNumber,int character,int phase){
  if(!Start(stageNumber,phase?2:0,character))return false;
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  auto* player=stage->m_pChara;
  stage->m_practiceSeekDamage=0;
  stage->m_nCurBomb=3;
  stage->m_curDream=128;
  stage->m_playerPerformance=32;
  stage->m_curGameKeyState=stage->m_lastGameKeyState=0;
  stage->m_pEnemyBullet->Initialize();
  player->m_curX=192*16;
  player->m_curY=300*16;
  player->m_invulnerableTimeLeft=0;
  player->m_cannotControlTimeLeft=0;
  player->m_nextLifeComeTimeLeft=0;
  player->m_bombTimer=0;
  if(phase)stage->m_pBoss->m_curPhase=phase;
  return true;
 }
 static int CombatDamage(int boss,int bombing,int age,int frame,int x,int y,int shots){
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  stage->m_curFrame=frame;
  stage->m_bInCharaBomb=bombing;
  stage->m_pChara->m_bombTimer=age;
  stage->m_pChara->m_curX=x;
  stage->m_pChara->m_curY=y;
  auto* pool=stage->m_pCharaBullet;
  pool->Initialize();
  for(int i=0;shots;i++,shots>>=8){
   auto& bullet=pool->m_bullets[i];
   bullet.flag=1;
   bullet.imgIdx=20;
   bullet.curX=192*16;
   bullet.curY=96*16;
   bullet.damage=shots&255;
  }
  return stage->GetDamageFromChara(192*16,96*16,24*16,24*16,boss);
 }
 static const int* CombatBombStep(int frame,int key){
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  auto* player=stage->m_pChara;
  stage->m_curFrame=frame;
  stage->m_lastGameKeyState=stage->m_curGameKeyState;
  stage->m_curGameKeyState=key;
  player->Step();
  stage->m_pEnemyBullet->Step();
  original::auditRandom=true;
  if(stage->m_bInCharaBomb)player->StepBomb();
  original::auditRandom=false;
  const int state[]={stage->m_bInCharaBomb,player->m_bombTimer,player->m_invulnerableTimeLeft,
   stage->m_pEnemyBullet->GetClearBulletTimeLeft(),stage->m_nCurBomb,stage->m_curDream,
   stage->m_bInCharaBombForItem,stage->m_playerPerformance,original::Demo().ringIndex};
  std::copy_n(state,9,motionData);
  return motionData;
 }
 static inline CKnifeBullet auditKnife;
 static inline CShinkiBullet auditBall;
 static void CombatSpecialStart(int kind,int angle){
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  stage->m_pChara->m_curX=3000;
  stage->m_pChara->m_curY=5000;
  if(kind==0){
   auditKnife.m_pStage=stage;
   auditKnife.Initialize();
   auditKnife.AddBullet(3000,3000,float(angle),61,16);
  }else{
   auditBall.m_pStage=stage;
   auditBall.Initialize();
   auditBall.AddBullet(3000,3000,201,float(angle),61);
  }
 }
 static const int* CombatSpecialStep(int kind,int clear){
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  stage->m_pEnemyBullet->m_clearEnemyBulletTimeLeft=clear;
  if(kind==0){
   auditKnife.Step();
   const auto& b=auditKnife.m_bullet[0];
   const int row[]={b.flag,int(b.curX),int(b.curY),int(b.velX),int(b.velY),b.img,b.cooldown};
   std::copy_n(row,7,motionData);
  }else{
   auditBall.Step();
   const auto& b=auditBall.m_bullet[0];
   const int row[]={b.flag,int(b.curX),int(b.curY),int(b.velX),int(b.velY),b.img,int(b.radius)};
   std::copy_n(row,7,motionData);
  }
  return motionData;
 }
 static bool PlayerMotionStart(int character){
  if(!Start(0,0,character))return false;
  auto* p=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme)->m_pChara;
  p->m_bCharaCanMove=true;
  p->m_curX=192*16;
  p->m_curY=184*16;
  return true;
 }
 static const int* PlayerMotionStep(int key){
  auto* p=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme)->m_pChara;
  p->MoveChara(key);
  motionData[0]=int(p->m_curX);motionData[1]=int(p->m_curY);
  motionData[2]=int(p->m_deltaX);motionData[3]=int(p->m_deltaY);
  return motionData;
 }
 static void BulletMotionStart(int type,int cloud,int x,int y,int angle,int speed,int parameter,int delta){
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  stage->m_pChara->m_curX=3000;stage->m_pChara->m_curY=5100;
  stage->m_pChara->m_invulnerableTimeLeft=255;
  auto* bullets=stage->m_pEnemyBullet;
  bullets->Initialize();
  ENEMY_SHOOT_INFO info{};
  info.shootOrigX=x;info.shootOrigY=y;info.shootAngle=angle;info.bulletSpeed=speed;
  info.bulletBornType=cloud==4?2:cloud==3?3:0;
  if(type!=-1)info.bulletBornType|=16;
  info.bulletImg=84;info.shootType=EST00_SINGLE;
  info.bulletMoveType=type;info.specialMoveVar=parameter;info.specialBulletDelta=delta;
  bullets->Shoot(&info,type>0,false);
  stage->m_curScore=stage->m_curNGraze=0;
 }
 static void BulletMotionPlayer(int x,int y,int invulnerable){
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  stage->m_pChara->m_curX=x;stage->m_pChara->m_curY=y;
  stage->m_pChara->m_invulnerableTimeLeft=invulnerable;
 }
 static const int* BulletMotionStep(int tick,int clear){
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  stage->m_curFrame=tick;
  stage->m_pChara->m_bMiss=false;
  auto* bullets=stage->m_pEnemyBullet;
  if(clear)bullets->SetClearBulletTimeLeft(20);
  bullets->Step();
  const auto& b=bullets->m_bullets[ENEMY_BULLET_N_RECORD_DOT+219];
  const int row[]={b.flag,int(b.curX),int(b.curY),int(b.velX),int(b.velY),b.imgIdx,
                   int(b.curSpeed),int(b.curAngle),b.bornStageAndGrazeFlag,b.dieStageAndMoveType,b.age,
                   stage->m_curScore,stage->m_curNGraze,stage->m_pChara->m_bMiss};
  std::copy_n(row,14,motionData);
  return motionData;
 }
 static void LaserMotionStart(int mode,int x,int y,int angle,int width,int speed,int tail,int head,int age,int grow,int shrink,int finalWidth){
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  stage->m_pEnemyBullet->Initialize();
  stage->m_pLaser->Initialize();
  auto& l=stage->m_pLaser->m_laser[0];
  l.mode=mode;l.srcX=x;l.srcY=y;l.angle=angle;l.width=width;l.speed=speed;
  l.lenLower=tail;l.lenUpper=head;l.age=age;l.ageThres=grow;l.shootAgeThres=shrink;l.expectedWidth=finalWidth;
 }
 static const int* LaserMotionStep(int clear){
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  stage->m_pChara->m_bMiss=false;
  if(clear)stage->m_pEnemyBullet->SetClearBulletTimeLeft(20);
  stage->m_pLaser->Step();
  stage->m_pLaser->FinishFrame();
  const auto& l=stage->m_pLaser->m_laser[0];
  const int row[]={l.mode,int(l.lenLower),int(l.lenUpper),int(l.speed),l.age,int(l.width),stage->m_pChara->m_bMiss};
  std::copy_n(row,7,motionData);
  return motionData;
 }
 static int SlowdownProbe(int difficulty,int count,int odd,bool turbo){
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  auto* bullets=stage->m_pEnemyBullet;
  bullets->Initialize();
  for(int i=0;i<count;i++){
   auto& b=bullets->m_bullets[i];
   b.flag=1;b.curX=b.curY=1000;b.dieStageAndMoveType=2;
  }
  CGame::GVar().m_playDifficulty=difficulty;
  CGame::GVar().m_runTurboMode=turbo;
  stage->m_curFrame=odd;
  original::BeginTimingFrame();
  bullets->Step();
  return original::FrameDelay();
 }
 static const int* RankInitial(int difficulty){
  auto& g=CGame::GVar();
  g.m_bReplayMode=g.m_bPracticeMode=false;
  g.m_playStage=difficulty==4?6:0;
  g.m_playDifficulty=difficulty;
  g.OnBeginGame();
  motionData[0]=g.m_playerPerformance;
  motionData[1]=g.m_playerPerformanceLowerBound;
  motionData[2]=g.m_playerPerformanceUpperBound;
  return motionData;
 }
 static const int* RankTune(int difficulty,int rank,int group,int ways,int spread,int stack,int delta){
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  CGame::GVar().m_playDifficulty=difficulty;
  stage->m_playerPerformance=rank;
  ENEMY_SHOOT_INFO info{};
  info.fromStageScript=true;info.shootType=group;info.nWay=ways;info.nWayUnitAngle=spread;
  info.nMulti=stack;info.deltaSpeed=delta;
  stage->m_pEnemyBullet->TuneEnemyShootInfo(&info);
  const int row[]={info.shootType,info.nWay,int(info.nWayUnitAngle),info.nMulti,int(info.deltaSpeed)};
  std::copy_n(row,5,motionData);
  return motionData;
 }
 static const int* RankItem(int kind,int pull,int dream,int y){
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  stage->m_bInCharaBombForItem=pull;
  stage->SetDream(dream);
  stage->m_pChara->CaughtItem(1000,y,kind);
  motionData[0]=stage->m_playerPerformance;
  motionData[1]=stage->m_itemRankProgress;
  return motionData;
 }
 static int RankCarry(){
  auto* game=CGame::s_pCurGame;
  auto* previous=static_cast<CStage*>(game->m_pCurScheme);
  previous->m_itemRankProgress=31;
  previous->UploadVarToGlobal();
  delete previous;
  auto& g=CGame::GVar();
  g.m_replay.CloseStage(false);
  g.m_playStage=1;
  g.m_practiceConfig=practice::Config();
  auto* next=new CStage;
  game->m_pCurScheme=next;
  next->Initialize();
  return g.m_replay.m_itemRankProgress[1]==31?next->m_itemRankProgress:-1;
 }
 static int RankSummary(int graze,int points){
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  stage->m_playerPerformance=32;
  stage->m_nMiss=stage->m_nBomb=1;
  stage->m_curNPointItem=points;
  stage->m_curNGraze=graze;
  stage->SetDream(90);
  stage->m_pSummary->Initialize();
  stage->m_pSummary->Run();
  stage->m_pSummary->Step();
  return stage->m_playerPerformance;
 }
 static int AuditStart(int number,int difficulty,int rank,int kind){
  original::auditRandom=false;
  if(!Start(number,kind==2?1:kind?2:0))return 0;
  auto* s=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  CGame::GVar().m_playDifficulty=difficulty;
  s->m_playerPerformance=rank;
  s->m_pChara->m_curX=(kind?192:-1000)*16;
  s->m_pChara->m_curY=(kind?320:1000)*16;
  s->m_pChara->m_invulnerableTimeLeft=10000;
  s->m_practiceSeekDamage=0;
  s->m_pEnemyBullet->Initialize();
  s->m_pGatherEffect->Initialize();
  if(kind==2){++s->m_curFrame;s->CheckMidBoss();}
  if(kind==3||kind==4){
   delete s->m_pBoss;
   auto* boss=kind==3?static_cast<CBoss*>(new CBossStage4Mai):static_cast<CBoss*>(new CBossStage4Yuki);
   s->m_pBoss=boss;
   boss->m_pStage=s;
   boss->m_curX=192*16;
   boss->m_curY=64*16;
   boss->Initialize();
  }
  return 1;
 }
 static inline int auditData[ENEMY_BULLET_N_RECORD+12]={};
 static inline int auditGeometry[400*9]={};
 static inline int auditTrajectory[49]={};
 static int AuditFrame(int frame,int kind,int damage,int hp,int otherHP,int flags){
  auto* s=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  s->m_curFrame=frame;
  s->m_pChara->m_bMiss=false;
  original::auditSpecialCount=0;
  struct AuditScope { AuditScope(){original::auditRandom=true;} ~AuditScope(){original::auditRandom=false;} } scope;
  if(!kind){
   s->m_pBackground->Step();
   auditData[9]=s->m_pBackground->m_curDeltaPixel;
  }
  auto* bullets=s->m_pEnemyBullet;
  for(auto& b:bullets->m_bullets)b.flag=0;
  auditData[0]=0;
  if(flags&1){
   bullets->Step();
   s->m_bInCharaBomb=(flags&2)!=0;
   s->m_pChara->m_bombTimer=0;
   auto& shot=s->m_pCharaBullet->m_bullets[0];
   shot.flag=0;
   if(flags&4){
    shot.flag=1;
    shot.imgIdx=20;
    shot.damage=16;
    shot.curX=s->m_pBoss->m_curX;
    shot.curY=s->m_pBoss->m_curY;
   }
  }
  if(!kind){
   s->m_pEnemy->StepEnemyTable();
   s->m_pEnemy->Step();
  }else if(kind==2){
   auto* mid=s->m_pMidBoss;
   if(!mid)return 0;
   if(hp>=0)mid->m_curHP=hp;
   auditData[2]=mid->m_curPhase;
   auditData[3]=mid->m_curPhaseFrame;
   auditData[4]=0;
   auditData[5]=mid->m_curHP;
   auditData[6]=0;
   auditData[7]=int(mid->m_curX);
   auditData[8]=int(mid->m_curY);
   s->m_pLaser->Step();
   mid->Step();
  }else{
   auto* boss=s->m_pBoss;
   if(!boss)return 0;
   if(damage)boss->m_curHP=boss->m_curPhaseEndHP;
   if(hp>=0)boss->m_curHP=hp;
   if(otherHP>=0)static_cast<CBossStage4*>(boss)->m_2curHP=otherHP;
   auditData[2]=boss->m_curPhase;
   auditData[3]=boss->m_curPhaseFrame;
   auditData[4]=boss->m_curMode;
   auditData[5]=boss->m_curHP;
   auditData[6]=boss->m_curPhaseEndHP;
   auditData[7]=int(boss->m_curX);
   auditData[8]=int(boss->m_curY);
   s->m_pLaser->Step();
   boss->Step();
  }
  s->m_pGatherEffect->Step();
  if(kind)s->m_pLaser->FinishFrame();
  if((flags&1)&&kind==1){
   const CCurvingBullet* curves=nullptr;
   if(const auto* twins=dynamic_cast<CBossStage4*>(s->m_pBoss))curves=&twins->m_curvingBullet;
   if(const auto* alice=dynamic_cast<CBossStageEx*>(s->m_pBoss))curves=&alice->m_curvingBullet;
   if(curves){
    int* out=auditTrajectory;
    for(const auto& curve:curves->m_bullet){
     if(!curve.flag){std::fill_n(out,7,0);out+=7;continue;}
     const int row[]={curve.mode,curve.demoHeadX,curve.demoHeadY,int(curve.velX),
      int(curve.velY),int(curve.angle),int(curve.speed)};
     out=std::copy_n(row,7,out);
    }
   }else if(dynamic_cast<CBossStage6*>(s->m_pBoss)){
    for(int i=0;i<4;i++){
     const auto& laser=s->m_pLaser->m_laser[i];
     auditTrajectory[i*2]=laser.mode;
     auditTrajectory[i*2+1]=laser.mode?int(laser.angle)&255:0;
    }
   }
  }
  for(const auto& b:bullets->m_bullets)if(b.flag==1){
   const int index=auditData[0]++;
   const int row[]={int(b.curX*256),int(b.curY*256),int(b.velX*256),int(b.velY*256),int(b.curAngle*256),int(b.origSpeed*256),b.imgIdx*256,b.bornStageAndGrazeFlag*256,b.dieStageAndMoveType*256};
   std::copy_n(row,9,auditGeometry+index*9);
   auditData[12+index]=row[5];
  }
  auditData[1]=original::Demo().ringIndex;
  return 1;
 }
 static int Background(int legacy){
  auto* game=CGame::s_pCurGame;if(!game||game->m_curSchemeID!=4)return 0;
  auto* bg=static_cast<CStage*>(game->m_pCurScheme)->m_pBackground;auto* atlas=bg->m_tileAtlas;if(legacy)bg->m_tileAtlas=nullptr;
  web::graphics::BeginFrame();glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT);bg->Draw();web::graphics::Present();bg->m_tileAtlas=atlas;return 1;
 }
 static int Paused(){auto* game=CGame::s_pCurGame;return game&&game->m_curSchemeID==4&&static_cast<CStage*>(game->m_pCurScheme)->m_bPaused;}
 static int Lasers(){
  auto* game=CGame::s_pCurGame;if(!game||game->m_curSchemeID!=4)return 0;
  auto* stage=static_cast<CStage*>(game->m_pCurScheme);auto* lasers=stage->m_pLaser;lasers->Initialize();
  float color[]={1,0,1};
  lasers->AddLaserFixed(0,20*16,80*16,color,8,0);lasers->StartGrow(0);
  lasers->AddLaserFixed(1,100*16,20*16,color,8,64);lasers->StartGrow(1);
  lasers->AddLaserFixed(2,140*16,140*16,color,8,32);lasers->StartGrow(2);
  for(int n=0;n<8;n++)lasers->Step();
  web::graphics::BeginFrame();glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT);lasers->Draw();web::graphics::Present();return 1;
 }
 static void Yumeko(){
  if(!Start(4,2))throw std::runtime_error("Could not start TH05 stage 5");
  auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
  auto* boss=dynamic_cast<CBossStage5*>(stage->m_pBoss);
  if(!boss)throw std::runtime_error("Yumeko is missing from stage 5");
  auto* bullets=stage->m_pEnemyBullet;
  boss->m_curX=192*16;
  boss->m_curY=80*16;
  boss->m_curHP=boss->m_curPhaseEndHP+700;

  for(bool blue:{false,true})for(int difficulty=0;difficulty<4;difficulty++){
   CGame::GVar().m_playDifficulty=difficulty;
   const auto shoot=blue?&CBossStage5::Phase7Shoot:&CBossStage5::Phase4Shoot;
   const auto* aim=blue?boss->m_phase7ShootAngle:boss->m_phase4ShootAngle;
   const int image=blue?116:84;
   const int perSide=difficulty<2?1:difficulty;
   boss->m_curPhaseFrame=16;
   (boss->*shoot)();

   for(int performance:{12,48}){
    stage->m_playerPerformance=performance;
    const float speed=stage->AdjustSpeedBasedOnPlayerPerformance(96);
    for(int frame=64;frame<=96;frame++){
     bullets->Initialize();
     boss->m_curPhaseFrame=frame;
     (boss->*shoot)();
     int emitted[2]={};
     for(int index=ENEMY_BULLET_N_RECORD-1;index>=0;--index){
      const auto& bullet=bullets->m_bullets[index];
      if(!bullet.flag)continue;
      const int side=bullet.curX>boss->m_curX?1:0;
      const int shot=emitted[side]++;
      const float spread=difficulty==3?(shot==2?-6.0f:shot*6.0f):0.0f;
      const float stack=difficulty==2?shot*6.0f:0.0f;
      if(bullet.imgIdx<image||bullet.imgIdx>=image+32||
         std::abs(std::remainder(bullet.curAngle-aim[side]-spread,256.0f))>0.001f||
         std::abs(bullet.origSpeed-speed-stack)>0.001f){
       throw std::runtime_error("Yumeko volley has the wrong sprite, angle or speed");
      }
     }
     const int expected=frame<96&&frame%2==0?perSide:0;
     if(emitted[0]!=expected||emitted[1]!=expected){
      throw std::runtime_error("Yumeko volley has the wrong bullet count or cadence");
     }
    }
   }
  }
  // The opening sweep ends after the knife at angle 16 on frame 66.
  for(bool clockwise:{false,true}){
   boss->m_bPhase2KnifeClockwise=clockwise;
   boss->m_curMode=1;
   boss->m_curPhaseFrame=64;
   boss->Phase2ShootType1();
   if(boss->m_curMode!=1)throw std::runtime_error("Yumeko ended her knife sweep early");
   boss->m_curPhaseFrame=66;
   boss->Phase2ShootType1();
   if(boss->m_curMode!=0||boss->m_curPhaseFrame!=0)
    throw std::runtime_error("Yumeko added an extra knife to her sweep");
  }

  stage->m_playerPerformance=32;
  CGame::GVar().m_playDifficulty=3;
  boss->m_phase8KnifePart=false;
  boss->m_phase8ShootPartSpeed=128;
  boss->m_phase8ShootPartNReset=1;
  for(int frame=64;frame<=256;frame+=4){
   bullets->Initialize();
   boss->m_curPhaseFrame=frame;
   boss->Phase8Shoot();
   int count=0;
   const float expectedSpeed=stage->AdjustSpeedBasedOnPlayerPerformance(16+(frame-64)/4%12*8);
   for(const auto& bullet:bullets->m_bullets){
    if(!bullet.flag)continue;
    ++count;
    if(bullet.origSpeed!=expectedSpeed)throw std::runtime_error("Wrong Yumeko aimed wave speed");
   }
   if(count!=(frame==256?0:5)||boss->m_phase8KnifePart!=(frame==256))
    throw std::runtime_error("Wrong Yumeko aimed wave length");
  }

  for(int frame:{32,48}){
   bullets->Initialize();
   boss->m_curPhaseFrame=frame;
   boss->Phase10Shoot();
   const float firstAngle=frame==32?0:127;
   const float turn=frame==32?-64:64;
   int count=0;
   for(int index=ENEMY_BULLET_N_RECORD-1;index>=0;--index){
    const auto& bullet=bullets->m_bullets[index];
    if(!bullet.flag)continue;
    if((count==0&&bullet.curAngle!=firstAngle)||bullet.delta!=turn||bullet.origSpeed!=38)
     throw std::runtime_error("Wrong Yumeko final ring angle, turn or speed");
    ++count;
   }
   if(count!=24)throw std::runtime_error("Wrong Yumeko final ring size");
  }

 }


 static int StageEnemyWaves(CStage* stage,int number){
  const int lastFrame[]={8240,6240,9050,11200,7960,1850,11190};
  const int waveCount[]={204,283,229,352,229,84,291};
  auto* enemies=stage->m_pEnemy;
  auto* bullets=stage->m_pEnemyBullet;
  int waves=0,shots=0;
  for(int frame=1;frame<=lastFrame[number]+256;frame++){
   stage->m_curFrame=frame;
   stage->m_pBackground->Step();
   const auto* before=enemies->m_pEnemyTableReadPtr;
   enemies->StepEnemyTable();
   if(enemies->m_pEnemyTableReadPtr!=before)++waves;
   stage->m_pGatherEffect->Step();
   enemies->Step();
   // Count births without letting the live bullet cap hide later script emissions.
   for(int start:{0,ENEMY_BULLET_N_RECORD_DOT}){
    const int limit=start?220:180;
    for(int i=start;i<start+limit;i++){
     auto& bullet=bullets->m_bullets[i];
     if(!bullet.flag)continue;
     if(!bullet.fixedPoint||!std::isfinite(bullet.curX)||!std::isfinite(bullet.curY)||
        bullet.curAngle<0||bullet.curAngle>=256||bullet.origSpeed<0||bullet.origSpeed>255)
      throw std::runtime_error("Invalid scripted enemy bullet");
     ++shots;
     bullet.flag=0;
    }
   }
  }
  if(waves!=waveCount[number]||shots==0)
   throw std::runtime_error("Stage enemy wave count or firing failed");
  return waves;
 }
 static void Stage3FairyVolley(CStage* stage){
  auto* enemies=stage->m_pEnemy;
  auto* bullets=stage->m_pEnemyBullet;
  const int speeds[]={18,24,30};
  const int increments[]={5,6,9,9};
  const int ranks[]={16,32,48};
  for(int difficulty=0;difficulty<4;difficulty++)for(int rank=0;rank<3;rank++){
   CGame::GVar().m_playDifficulty=difficulty;
   stage->m_playerPerformance=ranks[rank];
   enemies->Initialize();
   bullets->Initialize();
   // ST02 type 1, subtype 1: change the spread stack to an aimed stack.
   enemies->AddEnemy(1,1,100*16,80*16,0xfe);
   auto& enemy=enemies->m_enemy[0];
   enemies->StepEnemyScript(&enemy);
   for(int frame=0;frame<160&&!enemy.bAutoShoot;frame++)enemies->StepEnemyScript(&enemy);
   if(!enemy.bAutoShoot||enemy.shootInfo.shootType!=EST07_MULTI_TO_CHARA)
    throw std::runtime_error("Stage 3 fairy did not switch to an aimed stack");
   if(difficulty==0&&enemy.autoShootInterval!=255)
    throw std::runtime_error("Easy fairy autofire must use the original 255-frame interval");
   enemy.autoShootTimer=enemy.autoShootInterval-1;
   enemies->Step();
   int count=0;
   float angle=0;
   for(int i=179;i>=0;i--){
    const auto& bullet=bullets->m_bullets[i];
    if(!bullet.flag)continue;
    if(count==0)angle=bullet.curAngle;
    if(bullet.imgIdx!=0||bullet.curAngle!=angle||bullet.origSpeed!=speeds[rank]+count*increments[difficulty])
     throw std::runtime_error("Stage 3 fairy aimed stack has the wrong shape or speed");
    ++count;
   }
   if(count!=7+rank+(difficulty==3))
    throw std::runtime_error("Stage 3 fairy aimed stack has the wrong number of shots");
  }
 }
 static void BossPatterns(CStage* stage, int number){
  auto* boss=stage->m_pBoss;
  if(!boss)throw std::runtime_error("Could not start TH05 boss pattern check");
  auto* bullets=stage->m_pEnemyBullet;
  const auto active=[&]{
   int count=0;
   for(const auto& bullet:bullets->m_bullets)count+=bullet.flag==1;
   return count;
  };
  auto& difficulty=CGame::GVar().m_playDifficulty;
  difficulty=number==6?4:3;
  stage->m_playerPerformance=32;
  boss->m_curX=192*16;
  boss->m_curY=80*16;
  boss->m_curHP=10000;
  boss->m_curPhaseEndHP=0;
  bullets->Initialize();

  if(number==0){
   boss->m_curPhase=4;
   boss->m_curPhaseFrame=31;
   boss->Step();
   stage->m_curFrame=64;
   boss->m_curPhaseFrame=39;
   boss->Step();
   int pellets=0;
   for(const auto& bullet:bullets->m_bullets){
    if(!bullet.flag||bullet.imgIdx!=0)continue;
    ++pellets;
    if(std::abs(std::remainder(bullet.curAngle,256.0f))>4)
     throw std::runtime_error("Sara's first spread should be centered at angle zero");
   }
   if(pellets!=8||active()!=9)
    throw std::runtime_error("Wrong Sara opening volley size: "+std::to_string(pellets)+" pellets, "+std::to_string(active())+" total");

   bullets->Initialize();
   ENEMY_SHOOT_INFO stack{};
   stack.shootType=EST06_MULTI;
   stack.shootOrigX=boss->m_curX;
   stack.shootOrigY=boss->m_curY;
   stack.bulletSpeed=112;
   stack.nMulti=8;
   stack.deltaSpeed=16;
   bullets->Shoot(&stack);
   if(active()!=4)throw std::runtime_error("A stack should end with its first shot at speed 160");
   stack.deltaSpeed=7;
   bullets->TuneEnemyShootInfo(&stack);
   if(stack.deltaSpeed!=10)throw std::runtime_error("Stack speed increments must use whole subpixels");
  }else if(number==1){
   boss->m_curPhase=2;
   boss->m_curMode=0;
   boss->m_nModeChange=0;
   boss->m_velX=boss->m_velY=0;
   boss->m_curPhaseFrame=63;
   boss->Step();
   if(boss->m_curMode!=0)throw std::runtime_error("Louise ended her movement too early");
   boss->m_curPhaseFrame=95;
   boss->Step();
   if(boss->m_curMode!=2)throw std::runtime_error("Louise did not begin her red stack cycle");
   boss->m_curPhaseFrame=31;
   boss->Step();
   if(active()!=0)throw std::runtime_error("Louise fired her red stack during charge");
   boss->Step();
   boss->Step();
   if(active()!=14)throw std::runtime_error("Wrong Louise opening stack size");
   for(const auto& bullet:bullets->m_bullets)
    if(bullet.flag&&bullet.curAngle!=192)throw std::runtime_error("Wrong Louise opening stack angle");

   bullets->Initialize();
   stage->m_pLaser->Initialize();
   boss->m_curPhase=5;
   boss->m_curMode=0;
   boss->m_nModeChange=0;
   boss->m_curPhaseFrame=47;
   boss->Step();
   boss->m_curPhaseFrame=31;
   boss->Step();
   if(active()!=0||stage->m_pLaser->GetMode(0)!=LASER_MODE_SHOOT_OUT)
    throw std::runtime_error("Louise's final cycle should start with lasers");
   boss->m_curPhaseFrame=127;
   boss->Step();
   boss->m_curPhaseFrame=47;
   boss->Step();
   boss->m_curPhaseFrame=31;
   boss->Step();
   if(active()!=160)throw std::runtime_error("Wrong Louise final red ring size");
   for(const auto& bullet:bullets->m_bullets){
    if(bullet.flag&&bullet.origSpeed==20)return;
   }
   throw std::runtime_error("Wrong Louise final red ring base speed");
  }else if(number==3){
   CBossStage4 twins;
   twins.m_pStage=stage;
   twins.Initialize();
   twins.m_curPhaseFrame=48;
   stage->m_playerPerformance=48;
   twins.MaiShootType0_2__2_2();
   if(active()!=7)throw std::runtime_error("Wrong Mai spread size");
   for(const auto& bullet:bullets->m_bullets)
    if(bullet.flag&&bullet.origSpeed!=50)throw std::runtime_error("Mai spread should keep its fixed speed");

   CBossStage4Yuki yuki;
   yuki.SetStage(stage);
   yuki.SetX(192*16);
   yuki.SetY(80*16);
   yuki.Initialize();
   const int counts[]={4,6,8,10};
   for(difficulty=0;difficulty<4;difficulty++){
    bullets->Initialize();
    yuki.m_curPhaseFrame=8;
    yuki.Phase7Shoot();
    if(active()!=0||yuki.m_curImage!=208)throw std::runtime_error("Wrong Yuki charge frame");
    yuki.m_curPhaseFrame=12;
    yuki.Phase7Shoot();
    if(active()!=counts[difficulty])throw std::runtime_error("Yuki accelerated shots were tuned twice");
   }
  }else if(number==5){
   auto* shinki=static_cast<CBossStage6*>(boss);
   shinki->m_phase2Type1Angle=-1000;
   shinki->m_curPhaseFrame=32;
   shinki->Phase2ShootType2();
   int centered=0;
   for(const auto& bullet:bullets->m_bullets)
    if(bullet.flag&&bullet.curAngle==shinki->m_phase2Type2Angle)++centered;
   if(active()!=32||centered!=1)throw std::runtime_error("Shinki's rings should use their own starting angle");

   const int intervals[]={52,20,16,12};
   for(difficulty=0;difficulty<4;difficulty++){
    shinki->m_phase10Timer=0;
    shinki->m_curHP=3800;
    for(int frame=192;frame<=320&&shinki->m_phase10Timer<=65;frame++){
     bullets->Initialize();
     shinki->m_curPhaseFrame=frame;
     shinki->Phase10Shoot(shinki->m_curY);
    }
    if(shinki->m_phase10Var3!=intervals[difficulty])throw std::runtime_error("Wrong Shinki late ball interval");
   }
  }else if(number==6){
   auto* alice=static_cast<CBossStageEx*>(boss);
   const int frames[]={64,128,768,896};
   const int counts[]={8,9,14,14};
   for(int i=0;i<4;i++){
    bullets->Initialize();
    alice->m_curPhaseFrame=frames[i];
    alice->Phase8Shoot();
    if(active()!=counts[i])throw std::runtime_error("Wrong Extra Alice bouncing ring size");
   }

   alice->m_curPhaseFrame=64;
   alice->Phase4Shoot();
   alice->m_curPhaseFrame=128;
   alice->Phase4Shoot();
   if(!alice->m_bFireWave[0]||alice->m_bFireWave[1])
    throw std::runtime_error("Extra Alice's first fire wave should be on the left");
   for(int frame=0;frame<128;frame++)alice->StepFireWave();
   alice->m_curPhaseFrame=256;
   alice->Phase4Shoot();
   if(!alice->m_bFireWave[0]||!alice->m_bFireWave[1])
    throw std::runtime_error("Extra Alice's next fire wave should be on the right");
   for(int frame=0;frame<20;frame++)alice->StepFireWave();
   if(alice->m_bFireWave[0]||!alice->m_bFireWave[1])
    throw std::runtime_error("Wrong Extra Alice fire wave duration");
  }
 }
 static void BossMotionAndPractice(){
  auto* game=CGame::s_pCurGame;
  auto& g=CGame::GVar();
  delete game->m_pCurScheme;
  game->m_pCurScheme=nullptr;
  g.m_replay.CloseStage(false);
  g.m_bPracticeMode=true;
  g.m_bReplayMode=false;
  g.m_playStage=4;
  g.m_playDifficulty=3;
  g.m_playChara=0;
  auto& config=g.m_practiceConfig;
  config=practice::Config();
  config.mode=1;
  config.stage=4;
  config.warp=2;
  config.section=4;
  config.rage=1;
  config.Clamp(g.m_playDifficulty);
  g.OnBeginGame();
  auto* stage=new CStage;
  game->m_pCurScheme=stage;
  game->m_curSchemeID=4;
  stage->Initialize();
  auto* boss=static_cast<CBossStage5*>(stage->m_pBoss);
  if(!boss||!boss->m_phase7CrazyMode)throw std::runtime_error("Yumeko rage was not reached");
  for(int i=0;i<LASER_N_RECORD;i++)
   if(stage->m_pLaser->GetMode(i)!=LASER_MODE_NONE)
    throw std::runtime_error("Yumeko rage practice retained a warm-up laser");

  auto* bullets=stage->m_pEnemyBullet;
  auto& b=bullets->m_bullets[0];
  b.fixedPoint=true;
  b.curAngle=17;
  b.curSpeed=8;
  b.specialMoveType=EBMT03_SPEED_UP;
  b.specialMoveVar=2;
  for(int speed:{8,10,12}){
   bullets->SpecialMove(0);
   if(b.curSpeed!=speed+2||b.velX!=original::math::X(17,speed)||b.velY!=original::math::Y(17,speed))
    throw std::runtime_error("Boss acceleration used the next tick's speed");
  }

  b.specialMoveType=EBMT10_STRAIGHT;
  b.curSpeed=32;
  b.origX=b.curX=2000;
  b.origY=b.curY=1000;
  b.totalDistanceTravelled=0;
  for(int distance:{32,64,96}){
   bullets->SpecialMove(0);
   b.curX+=b.velX;
   b.curY+=b.velY;
   if(b.curX!=2000+original::math::X(17,distance)||b.curY!=1000+original::math::Y(17,distance))
    throw std::runtime_error("Boss straight motion accumulated quantized velocity");
  }
 }
 static void UpdatedPracticeStarts(){
  const bool enabled=practice::Available();
  if(!enabled)practice::Configure(true);
  auto& options=practice::Options();
  const auto previousOptions=options;
  options.eosdBar=false;
  options.ringBar=true;
  options.phaseTimeoutAsFrames=true;
  auto* game=CGame::s_pCurGame;
  auto& g=CGame::GVar();
  const auto reset=[&](){
   delete game->m_pCurScheme;game->m_pCurScheme=nullptr;g.m_replay.CloseStage(false);
  };
  const auto initialize=[&](){
   g.OnBeginGame();
   auto* stage=new CStage;
   game->m_pCurScheme=stage;game->m_curSchemeID=4;
   stage->Initialize();
   return stage;
  };
  const auto start=[&](int stage,int section,int rage,int trigger=0){
   reset();
   g.m_bReplayMode=false;g.m_bPracticeMode=true;g.m_playStage=stage;
   g.m_playDifficulty=1;g.m_playChara=0;
   auto& config=g.m_practiceConfig;
   config=practice::Config();
   config.mode=1;config.stage=stage;config.warp=2;
   config.section=section;config.rage=rage;config.trigger=trigger;
   config.Clamp(g.m_playDifficulty);
   return initialize();
  };
  for(int rage:{1,2}){
   auto* stage=start(3,1,rage);
   auto* boss=static_cast<CBossStage4*>(stage->m_pBoss);
   const int boundary=rage==1?original::TwinFirstRageHP:original::TwinSecondRageHP;
   if(boss->m_curHP+boss->m_2curHP!=boundary-1||boss->m_nModeChange!=(rage==1?10:15))
    throw std::runtime_error("Twin practice did not enter the strict-HP rage state");
   for(const auto& ring:stage->m_bossRings)
    if(!ring.data.present||ring.data.hp!=boundary-1||ring.data.full!=9000||
       ring.data.markerCount!=2||ring.data.markers[0]!=original::TwinFirstRageHP||
       ring.data.markers[1]!=original::TwinSecondRageHP)
     throw std::runtime_error("Twin rings did not start with shared HP and both rage markers");
  }
  options.ringBar=false;
  options.eosdBar=true;
  for(int section:{2,4}){
   auto* stage=start(4,section,1);
   auto* boss=static_cast<CBossStage5*>(stage->m_pBoss);
   const bool red=section==2;
   const auto timer=stage->GetPhaseTimeout();
   if(boss->m_curPhaseFrame!=32||!(red?boss->m_phase4CrazyMode:boss->m_phase7CrazyMode)
      ||timer.unit!=practice::PhaseTimeout::Ticks||timer.value!=1968)
    throw std::runtime_error("Yumeko practice must start at the first rage frame");
   const auto& bar=stage->m_bossBar.data;
   if(!bar.present||bar.hp!=boss->m_curHP||bar.enrage!=boss->m_curHP+1||
      bar.remaining!=(red?4:2))
    throw std::runtime_error("Yumeko's top bar did not start with the active phase and rage marker");
  }
  CReplay recorded=g.m_replay;
  recorded.CloseStage(false);
  for(unsigned version:{CReplay::CurrentPracticeVersion,3u}){
   const char* path="/th05-practice-start-test.rpy";
   recorded.m_practiceVersion=version;
   reset();
   if(!recorded.SaveFile(path)||!g.m_replay.LoadFile(path)||g.m_replay.m_practiceVersion!=version)
    throw std::runtime_error("Could not round-trip practice setup version");
   std::remove(path);
   g.m_bReplayMode=true;g.m_playStage=4;
   if(initialize()->m_pBoss->m_curPhaseFrame!=(version==3?65:32))
    throw std::runtime_error("Replay opened with the wrong practice start");
  }
  options.eosdBar=false;
  options.ringBar=true;
  for(int trigger:{0,1})for(int rage:{1,2}){
   auto* stage=start(5,6,rage,trigger);
   auto* boss=static_cast<CBossStage6*>(stage->m_pBoss);
   const int mode=rage==1?LASER_MODE_GROW:LASER_MODE_PRESENT;
   for(int laser=0;laser<4;laser++)if(stage->m_pLaser->GetMode(laser)!=mode)
    throw std::runtime_error("Shinki practice did not advance the wing lasers to the selected state");
   const auto timer=stage->GetPhaseTimeout();
   if(timer.unit!=practice::PhaseTimeout::Ticks||timer.value!=original::ShinkiWingEndFrame-boss->m_curPhaseFrame)
    throw std::runtime_error("Wing phase timer did not include the selected start frame");
   const auto& ring=stage->m_bossRings[0].data;
   if(!ring.present||ring.hp!=boss->m_curHP-2800||ring.full!=5800||
      ring.markerCount!=2||ring.markers[0]!=original::ShinkiLaserHP-2800||
      ring.markers[1]!=original::ShinkiFastHP-2800||stage->GetBossBarData().remaining!=1)
    throw std::runtime_error("Shinki's wing ring did not start with both attack thresholds");
  }
  options=previousOptions;
  if(!enabled)practice::Configure(false);
 }
 static int Matrix(){
  auto* game=CGame::s_pCurGame;if(!game)return 0;int checked=0;
  for(int character=0;character<4;character++)for(int stage=0;stage<7;stage++)for(int warp=0;warp<3;warp++){
   auto& g=CGame::GVar();delete game->m_pCurScheme;game->m_pCurScheme=nullptr;g.m_replay.CloseStage(false);
   g.m_bReplayMode=false;g.m_bPracticeMode=true;g.m_playStage=stage;g.m_playChara=character;g.m_playDifficulty=stage==6?4:1;
   g.m_practiceConfig=practice::Config();g.m_practiceConfig.mode=1;g.m_practiceConfig.stage=stage;g.m_practiceConfig.warp=warp;g.m_practiceConfig.Clamp(g.m_playDifficulty);g.OnBeginGame();
   auto* current=new CStage;game->m_pCurScheme=current;game->m_curSchemeID=4;current->Initialize();current->m_bLogo=false;
   for(int frame=0;frame<160;frame++){
    current->m_pChara->m_invulnerableTimeLeft=300;
    if(current->StepFrame(32|(frame%80<40?4:8)))break;
    if(!std::isfinite(current->m_pChara->GetCurX())||!std::isfinite(current->m_pChara->GetCurY()))throw std::runtime_error("Non-finite TH05 player position");
   }
   checked++;
  }
  int waves=0;
  for(int stage=0;stage<7;stage++){
   if(!Start(stage,0))throw std::runtime_error("Could not start TH05 enemy wave audit");
   CGame::GVar().m_playDifficulty=stage==6?4:3;
   waves+=StageEnemyWaves(static_cast<CStage*>(game->m_pCurScheme),stage);
  }
  if(waves!=1672)throw std::runtime_error("Not all TH05 enemy waves were processed");
  Yumeko();
  BossMotionAndPractice();
  if(!Start(2,0))throw std::runtime_error("Could not start TH05 stage 3");
  Stage3FairyVolley(static_cast<CStage*>(game->m_pCurScheme));
  for(int stage:{0,1,3,5,6}){
   if(!Start(stage,2))throw std::runtime_error("Could not start TH05 stage");
   BossPatterns(static_cast<CStage*>(game->m_pCurScheme),stage);
  }
  UpdatedPracticeStarts();
  return checked;
 }
};
}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_stage_matrix(){try{return th5w::PracticeTests::Matrix();}catch(const std::exception& e){th5w::web::ReportError(e.what());return 0;}}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_laser_probe(){return th5w::PracticeTests::Lasers();}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_stage_start(int stage,int warp){return th5w::PracticeTests::Start(stage,warp);}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_background_probe(int legacy){return th5w::PracticeTests::Background(legacy);}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_pause_state(){return th5w::PracticeTests::Paused();}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_draw_probe(int atlas){return th5w::PracticeTests::Draw(atlas);}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_effect_probe(int kind){return th5w::PracticeTests::Effects(kind);}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_demo_start(int number){return th5w::PracticeTests::DemoStart(number);}
extern "C" EMSCRIPTEN_KEEPALIVE unsigned th05_native_demo_info(int field){return th5w::PracticeTests::DemoInfo(field);}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_audit_start(int stage,int diff,int rank,int kind){return th5w::PracticeTests::AuditStart(stage,diff,rank,kind);}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_audit_frame(int frame,int kind,int damage,int hp,int otherHP,int flags){return th5w::PracticeTests::AuditFrame(frame,kind,damage,hp,otherHP,flags);}
extern "C" EMSCRIPTEN_KEEPALIVE const int* th05_native_audit_data(){return th5w::PracticeTests::auditData;}
extern "C" EMSCRIPTEN_KEEPALIVE unsigned char* th05_native_audit_ring(){return th5w::original::Demo().ring;}
extern "C" EMSCRIPTEN_KEEPALIVE void th05_native_audit_cursor(int index){th5w::original::Demo().ringIndex=index;}

extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_audit_special_count(){return th5w::original::auditSpecialCount;}
extern "C" EMSCRIPTEN_KEEPALIVE const int* th05_native_audit_special(){return th5w::original::auditSpecial;}

extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_shot_start(int character,int power,int homing){return th5w::PracticeTests::ShotStart(character,power,homing);}
extern "C" EMSCRIPTEN_KEEPALIVE const int* th05_native_shot_step(int frame){return th5w::PracticeTests::ShotStep(frame);}

extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_motion_rules(){return th5w::original::MotionEnabled();}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_boss_rules(){return th5w::original::BossGeometryEnabled();}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_shot_rules(){return th5w::PracticeTests::ShotRules();}

extern "C" EMSCRIPTEN_KEEPALIVE const int* th05_native_audit_trajectory(){return th5w::PracticeTests::auditTrajectory;}
extern "C" EMSCRIPTEN_KEEPALIVE const int* th05_native_audit_geometry(){return th5w::PracticeTests::auditGeometry;}

extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_player_motion_start(int character){return th5w::PracticeTests::PlayerMotionStart(character);}
extern "C" EMSCRIPTEN_KEEPALIVE const int* th05_native_player_motion_step(int key){return th5w::PracticeTests::PlayerMotionStep(key);}
extern "C" EMSCRIPTEN_KEEPALIVE void th05_native_bullet_motion_start(int type,int cloud,int x,int y,int angle,int speed,int parameter,int delta){th5w::PracticeTests::BulletMotionStart(type,cloud,x,y,angle,speed,parameter,delta);}
extern "C" EMSCRIPTEN_KEEPALIVE const int* th05_native_bullet_motion_step(int tick,int clear){return th5w::PracticeTests::BulletMotionStep(tick,clear);}

extern "C" EMSCRIPTEN_KEEPALIVE void th05_native_laser_motion_start(int mode,int x,int y,int angle,int width,int speed,int tail,int head,int age,int grow,int shrink,int finalWidth){th5w::PracticeTests::LaserMotionStart(mode,x,y,angle,width,speed,tail,head,age,grow,shrink,finalWidth);}
extern "C" EMSCRIPTEN_KEEPALIVE const int* th05_native_laser_motion_step(int clear){return th5w::PracticeTests::LaserMotionStep(clear);}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_slowdown_probe(int difficulty,int count,int odd,int turbo){return th5w::PracticeTests::SlowdownProbe(difficulty,count,odd,turbo);}

extern "C" EMSCRIPTEN_KEEPALIVE const int* th05_native_rank_initial(int difficulty){return th5w::PracticeTests::RankInitial(difficulty);}
extern "C" EMSCRIPTEN_KEEPALIVE const int* th05_native_rank_tune(int difficulty,int rank,int group,int ways,int spread,int stack,int delta){return th5w::PracticeTests::RankTune(difficulty,rank,group,ways,spread,stack,delta);}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_rank_speed(int rank,int speed){return th5w::original::math::Speed(speed,rank);}
extern "C" EMSCRIPTEN_KEEPALIVE const int* th05_native_rank_item(int kind,int pull,int dream,int y){return th5w::PracticeTests::RankItem(kind,pull,dream,y);}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_rank_carry(){return th5w::PracticeTests::RankCarry();}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_rank_summary(int graze,int points){return th5w::PracticeTests::RankSummary(graze,points);}

extern "C" EMSCRIPTEN_KEEPALIVE void th05_native_bullet_motion_player(int x,int y,int invulnerable){th5w::PracticeTests::BulletMotionPlayer(x,y,invulnerable);}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_combat_start(int stage,int character,int phase){return th5w::PracticeTests::CombatStart(stage,character,phase);}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_combat_damage(int boss,int bombing,int age,int frame,int x,int y,int shots){return th5w::PracticeTests::CombatDamage(boss,bombing,age,frame,x,y,shots);}
extern "C" EMSCRIPTEN_KEEPALIVE const int* th05_native_combat_bomb_step(int frame,int key){return th5w::PracticeTests::CombatBombStep(frame,key);}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_bomb_rage_rules(){return th5w::original::BombAndRageEnabled();}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_allcast(int field){return th5w::PracticeTests::AllCast(field);}
extern "C" EMSCRIPTEN_KEEPALIVE void th05_native_combat_special_start(int kind,int angle){th5w::PracticeTests::CombatSpecialStart(kind,angle);}
extern "C" EMSCRIPTEN_KEEPALIVE const int* th05_native_combat_special_step(int kind,int clear){return th5w::PracticeTests::CombatSpecialStep(kind,clear);}
