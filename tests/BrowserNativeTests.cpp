// Compiled only into TH05_BROWSER_TESTS builds. Exercise the production game
// classes with actual DAT resources without rendering thousands of frames.
#include "../Game/Game.h"
#include "../Game/Stage/Stage.h"
#include "../Game/Stage/Chara/Chara.h"
#include "../Game/Stage/Laser/Laser.h"
#include "../Game/Stage/StageResource.h"
#include "../Game/Stage/Dialogue/Dialogue.h"
#include "../Game/Stage/Background/Background.h"
#include "../Game/Stage/Boss/Boss.h"
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
  s->BindSeat(seat);auto* p=s->m_pChara;
  if(op==13){
   if(value<0||value>19)return 0;
   auto* image=value==19?s->m_pStageRes->m_pCoopGhost:s->m_pStageRes->m_patternArray.GetImagePtr(value<3?value:20+value-3);
   if(!image)return 0;
   glBindTexture(GL_TEXTURE_2D,image->GetTexID());GLint width,height;
   glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&width);glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&height);
   std::vector<unsigned char> pixels(width*height*4);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels.data());
   unsigned hash=2166136261u;for(int y=0;y<image->m_height;y++)for(int x=0;x<image->m_width*4;x++)hash=(hash^pixels[y*width*4+x])*16777619u;
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
