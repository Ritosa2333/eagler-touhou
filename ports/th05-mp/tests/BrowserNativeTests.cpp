// Compiled only into TH05_BROWSER_TESTS builds. Exercise the production game
// classes with actual DAT resources without rendering thousands of frames.
#include "../Game/Game.h"
#include "../Game/Stage/Stage.h"
#include "../Game/Stage/Chara/Chara.h"
#include "../Game/Stage/CharaBullet/CharaBullet.h"
#include "../Game/Stage/EnemyBullet/EnemyBullet.h"
#include "../Game/Stage/Laser/Laser.h"
#include "../Game/Stage/StageResource.h"
#include "../Game/Stage/Dialogue/Dialogue.h"
#include "../Game/Stage/Background/Background.h"
#include "../Game/Stage/Boss/BossStage5.h"
#include "../Game/Stage/Boss/BossStage4Yuki.h"
#include "../Game/Stage/Boss/BossStage6.h"
#include "../Game/Stage/Boss/BossStageEx.h"
#include "../Game/Stage/Boss/BossStage4.h"
#include "../Game/Stage/MidBoss/MidBoss.h"
#include "../portable/Graphics.hpp"
#include "../Game/Practice/Practice.h"
#include <cmath>
#include <new>
#include <stdexcept>
#include <eagler/netplay/RollbackJournal.hpp>

// A dense same-address oracle: restore the exact starting battle, execute the
// delayed-input driver, and compare every captured world byte with the exact run.
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_rollback_checkpoint(int restore){
 static Netplay::RollbackJournal oracle;
 auto* game=th5w::CGame::s_pCurGame;if(!game||game->m_curSchemeID!=4)return 0;
 if(restore)return oracle.UndoTo(0);
 Netplay::RollbackJournalConfig config;config.maxFrames=1;config.maxBytesPerFrame=1024*1024;config.maxBlocksPerFrame=64;
 if(!oracle.Reset(config)||!oracle.BeginFrame(0))return 0;
 struct Sink:th5w::coop::CaptureSink {Netplay::RollbackJournal& journal;explicit Sink(Netplay::RollbackJournal& j):journal(j){}void Bytes(void* p,unsigned n) override {if(!journal.Touch(p,n))throw std::runtime_error("Oracle capture failed");}} sink(oracle);
 static_cast<th5w::CStage*>(game->m_pCurScheme)->CaptureRollback(sink);return oracle.EndFrame();
}
namespace th5w {
struct PracticeTests {
 inline static int stagePoison=-1;
 static int CoopProbe(int op,int seat,int value){
  auto* game=CGame::s_pCurGame;if(!game||game->m_curSchemeID!=4)return 0;
  auto* s=static_cast<CStage*>(game->m_pCurScheme);if(!s->Cooperative()||seat<0||seat>=s->m_seatCount)return 0;
  if(op==34){Yumeko(s);return 1;}
  if(op==35){BossPatterns(s,CGame::GVar().m_playStage);return 1;}
  if(op==36){Stage3FairyVolley(s);return 1;}
  if(op==37)return StageEnemyWaves(s,CGame::GVar().m_playStage);
  if(op==38)return s->m_itemRankProgress;
  s->BindSeat(seat);auto* p=s->m_pChara;
  if(op==32){
   auto* bullets=s->m_pEnemyBullet;bullets->Initialize();auto& bullet=bullets->m_bullets[ENEMY_BULLET_N_RECORD_DOT];
   bullet={};bullet.flag=1;bullet.imgIdx=100;bullet.curX=p->m_curX;bullet.curY=p->m_curY;bullets->m_imageEnd=ENEMY_BULLET_N_RECORD_DOT+1;
   s->SaveSeat();s->BindSeat(0);return 1;
  }
  if(op==33){
   s->BindSeat(0);web::graphics::BeginFrame();if(value)s->DrawPlayers(2);else s->m_pEnemyBullet->Draw();web::graphics::Present();s->SaveSeat();s->BindSeat(0);return 1;
  }
  if(op==31){
   int result=value==0?p->m_invulnerableTimeLeft:value==1?p->m_nextLifeComeTimeLeft:value==2?p->m_bMiss:value==3?p->m_bombInvulnerableTime:value==4?s->m_bInCharaBomb:s->m_bombOwner;
   s->SaveSeat();s->BindSeat(0);return result;
  }
  if(op>=27&&op<=29){
   web::graphics::BeginFrame();glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT);
   if(op==27)p->DrawCollisionArea();
   else if(op==28){
    if(value<0||value>=16)return 0;
    s->m_pCharaBullet->Initialize();s->m_pCharaBullet->AddBullet(p->m_curX,p->m_curY,20+value,CHARA_BULLET_TYPE_NORMAL,0,0,0,1);s->m_pCharaBullet->Draw();
   }else{practice::Options().numericPower=value!=0;s->DrawCoopStatistics();}
   web::graphics::Present();s->SaveSeat();s->BindSeat(0);return 1;
  }
  if(op==13||op==30){
   if(value<0||value>19)return 0;
   auto* image=value==19?s->m_pStageRes->m_pCoopGhost:s->m_pStageRes->m_patternArray.GetImagePtr(value<3?value:20+value-3);
   if(!image)return 0;
   glBindTexture(GL_TEXTURE_2D,image->GetTexID());GLint width,height;
   glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&width);glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&height);
   std::vector<unsigned char> pixels(width*height*4);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels.data());
   unsigned hash=2166136261u;for(int y=0;y<image->m_height;y++)for(int x=0;x<image->m_width;x++)for(int channel=0;channel<(op==30?3:4);channel++){const auto* pixel=&pixels[(y*width+x)*4];hash=(hash^(op==30&&!pixel[3]?0:pixel[channel]))*16777619u;}
   s->SaveSeat();s->BindSeat(0);return int(hash);
  }
  switch(op){
   case 0:s->m_nCurLife=value;break;
   case 1:s->MakeGhost();p->m_bDraw=false;break;
   case 2:p->CaughtItem(p->m_curX,p->m_curY,static_cast<unsigned char>(value));break;
   case 3:p->SetPower(value);break;
   case 4:p->m_curX=value;break;
   case 5:p->m_curY=value;break;
   case 6:p->m_invulnerableTimeLeft=value;break;
   case 7:s->m_nCurBomb=value;break;
   case 8:if(s->TestPlayerCrash(p->m_curX,p->m_curY,64,64))s->MarkPlayerMiss();break;
   case 9:s->m_totalNPointItem=value;break;
   case 10:s->m_pChara->DropBomb();break;
   case 14:p->m_nextLifeComeTimeLeft=0;p->m_cannotControlTimeLeft=0;p->m_invulnerableTimeLeft=0;p->m_bDraw=true;break;
   case 15:{auto* boss=dynamic_cast<CBossStage4*>(s->m_pBoss);if(!boss)return 0;boss->m_whichDie=value;boss->m_curPhase=0xff;boss->m_curPhaseFrame=0;break;}
   case 16:if(!s->m_pMidBoss)return 0;s->m_pMidBoss->m_curPhase=0xff;s->m_pMidBoss->m_curPhaseFrame=0;break;
   case 17:s->m_pDialogue->Run();break;
   case 18:s->m_bLogo=true;s->m_logoFrame=0;break;
   case 19:s->SetQuit(STAGE_END_CLEAR);break;
   case 20:s->SaveSeat();s->BindSeat(0);return s->RollbackPredictable();
   case 22:{auto* boss=dynamic_cast<CBossStage4*>(s->m_pBoss);if(!boss)return 0;boss->m_whichDie=value;boss->m_curPhase=0xfd;boss->m_curPhaseFrame=0;boss->m_bSuccessDefeat=true;boss->m_bDrawCurvingBullet=false;s->m_pLaser->RemoveLaser(0);break;}
   case 24:stagePoison=value;break;
   case 25:return CoopTransition(value);
   case 26:s->SaveSeat();s->BindSeat(0);return CGame::GVar().m_playStage;
   case 23:if(!s->m_pBoss||dynamic_cast<CBossStage4*>(s->m_pBoss))return 0;s->m_pBoss->m_curPhase=0xfd;s->m_pBoss->m_curPhaseFrame=0;s->m_pBoss->m_bSuccessDefeat=true;break;
   case 21:{int phase=s->m_pBoss?s->m_pBoss->GetCurPhase():-1;s->SaveSeat();s->BindSeat(0);return phase;}
   case 12:s->SetQuit(STAGE_END_CLEAR);s->m_nQuitFrameLeft=0;break;
   default:return 0;
  }
  s->SaveSeat();s->BindSeat(0);return 1;
 }
 static int CoopTransition(int fill){
  auto* game=CGame::s_pCurGame;if(!game||game->m_curSchemeID!=4)return 0;auto* previous=static_cast<CStage*>(game->m_pCurScheme);
  previous->UploadVarToGlobal();previous->PersistCoop();delete previous;game->m_pCurScheme=nullptr;
  auto& g=CGame::GVar();++g.m_playStage;g.m_playChara=coop::Run().characters[0];
  void* memory=::operator new(sizeof(CStage));memset(memory,fill,sizeof(CStage));auto* next=new(memory)CStage;
  game->m_pCurScheme=next;next->Initialize();next->m_bLogo=false;return 1;
 }
 static int CoopStage(int stage,int warp){
  auto* game=CGame::s_pCurGame;if(!game||coop::Run().count<2||stage<0||stage>6||warp<0||warp>2)return 0;
  delete game->m_pCurScheme;game->m_pCurScheme=nullptr;coop::Run().continuing=false;
  auto& g=CGame::GVar();g.m_playStage=stage;g.m_playChara=coop::Run().characters[0];g.m_bPracticeMode=false;g.OnBeginGame();
  for(int i=0;i<7;i++)g.m_randomSeed[i]=coop::Run().seed+i*7919;
  CStage* s;
  if(stagePoison>=0){void* memory=::operator new(sizeof(CStage));memset(memory,stagePoison,sizeof(CStage));s=new(memory)CStage;}else s=new CStage;
  game->m_pCurScheme=s;s->Initialize();s->m_bLogo=false;
  if(warp){g.m_bPracticeMode=true;g.m_practiceConfig=practice::Config();g.m_practiceConfig.mode=1;g.m_practiceConfig.stage=stage;g.m_practiceConfig.warp=warp;s->ApplyPracticeWarp();g.m_bPracticeMode=false;g.m_practiceConfig=practice::Config();}
  return 1;
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
 static int Start(int stage,int warp){
  auto* game=CGame::s_pCurGame;if(!game||stage<0||stage>6||warp<0||warp>2)return 0;
  auto& g=CGame::GVar();delete game->m_pCurScheme;game->m_pCurScheme=nullptr;original::StopDemo();g.m_replay.CloseStage(false);
  g.m_bReplayMode=false;g.m_bPracticeMode=true;g.m_playStage=stage;g.m_playChara=0;g.m_playDifficulty=stage==6?4:1;
  g.m_practiceConfig=practice::Config();g.m_practiceConfig.mode=1;g.m_practiceConfig.stage=stage;g.m_practiceConfig.warp=warp;g.m_practiceConfig.Clamp(g.m_playDifficulty);g.OnBeginGame();
  auto* current=new CStage;game->m_pCurScheme=current;game->m_curSchemeID=4;current->Initialize();current->m_bLogo=false;current->m_bCanPause=true;
  if(!practice::Enabled(practice::Invincible))practice::Toggle(practice::Invincible);return 1;
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
 static void Yumeko(CStage* stage){
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

extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_coop_probe(int op,int seat,int value){return th5w::PracticeTests::CoopProbe(op,seat,value);}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_native_coop_stage(int stage,int warp){return th5w::PracticeTests::CoopStage(stage,warp);}

extern "C" EMSCRIPTEN_KEEPALIVE unsigned th05_net_world_hash(){
 auto* game=th5w::CGame::s_pCurGame;if(!game||game->m_curSchemeID!=4)return 0;
 struct Hash:th5w::coop::CaptureSink {
  unsigned value=2166136261u;
  void Bytes(void* address,unsigned size) override {auto* data=static_cast<unsigned char*>(address);for(unsigned i=0;i<size;i++)value=(value^data[i])*16777619u;}
 } sink;
 static_cast<th5w::CStage*>(game->m_pCurScheme)->CaptureRollback(sink);return sink.value;
}
