// Compiled only into TH05_BROWSER_TESTS builds. Exercise the production game
// classes with actual DAT resources without rendering thousands of frames.
#include "../Game/Game.h"
#include "../Game/Stage/Stage.h"
#include "../Game/Stage/Chara/Chara.h"
#include "../Game/Stage/Laser/Laser.h"
#include "../Game/Stage/StageResource.h"
#include "../Game/Stage/Background/Background.h"
#include "../Game/Stage/Boss/BossStage5.h"
#include "../portable/Graphics.hpp"
#include "../Game/Practice/Practice.h"
#include <cmath>
#include <stdexcept>
namespace th5w {
struct PracticeTests {
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
     for(const auto& bullet:bullets->m_bullets){
      if(!bullet.flag)continue;
      const int side=bullet.curX>boss->m_curX?1:0;
      const int shot=emitted[side]++;
      const float spread=difficulty==3?(shot-1)*6.0f:0.0f;
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
  Yumeko();
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
