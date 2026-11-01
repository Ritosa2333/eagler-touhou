// Compiled only into TH05_BROWSER_TESTS builds. Exercise the production game
// classes with actual DAT resources without rendering thousands of frames.
#include "../Game/Game.h"
#include "../Game/Stage/Stage.h"
#include "../Game/Stage/Chara/Chara.h"
#include "../Game/Practice/Practice.h"
#include <cmath>
#include <stdexcept>
namespace th5w {
struct PracticeTests {
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
