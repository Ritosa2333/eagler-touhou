#include "../Game/Game.h"
#include "../Game/Stage/Stage.h"
#include "../Game/Stage/Chara/Chara.h"
#include "../Game/Practice/Practice.h"
#include "../Game/TitleScreen/TitleScreen.h"
#include "../gameLib/Graphic/PC98Font.h"
#include "../gameLib/Graphic/Th5ExtFont.h"
#include "../gameLib/CommonFunction/CommonFunctionSystem.h"
#include "../third_party/imgui/imgui.h"
#include <memory>
#include <algorithm>
#include <ctime>
#include "Localization.hpp"
#include "../Game/Coop/Rollback.h"
#include <functional>
#include <stdexcept>
namespace th5w {
unsigned short PhysicalKeys();
CGame* CGame::s_pCurGame=nullptr;
CGame::CGame(){m_windowWidth=640;m_windowHeight=480;m_playArea={32,56,416,424};m_pCurScheme=nullptr;m_curSchemeID=0;m_fpsLimit=56;m_fps=0;m_fpsTimeIdx=0;m_thisFrameDuration=1.f/56;m_thisFrameLostTime=0;memset(m_fpsTimeArray,0,sizeof(m_fpsTimeArray));}
CGame::~CGame(){}
void CGame::SetVSYNC(bool){}
// Browser presentation owns FPS; simulation catch-up must not inflate it.
void CGame::UpdateFPS(){}
Cth5DatFile* CGame::FindResource(const char* name){for(auto* pack:{&m_modDat,&m_th5Dat1,&m_th5Dat2})if(pack->GetChildFileIndex(name)>=0)return pack;return nullptr;}
void CGame::LoadModConfig(){int id=m_modDat.GetChildFileIndex("mod.ini");if(id<0)return;char line[1024];while(m_modDat.Childfgets(line,sizeof(line),id)){unsigned value;if(sscanf(line," TEXT_CODEPAGE = %u",&value)==1)GVar().m_textCodePage=value;if(sscanf(line," USE_SYSTEM_FONT = %u",&value)==1)GVar().m_bUseSystemFont=value!=0;}}
bool CGame::Initialize(bool){
 m_globalVar.Initialize();
 if(!m_gameWindow.Create(false,640,480)){web::ReportError("WebGL initialization failed");return false;}
 m_input.Initialize();
 struct Asset{const char* path;const char* hash;};
 const Asset assets[]={{"kaiki1.dat","cb6e2344aa93312c3cf40bf16f3a14f2"},{"kaiki2.dat","ca20e3f03946c173a02a62f37bacafd1"},{"zun.com","0005e6571252bed38d416799c0053c0b"}};
 for(const auto& asset:assets){unsigned char md5[16];char hex[33];if(!CCommonFunctionSystem::GetFileMD5(md5,asset.path)){web::ReportError(asset.path);return false;}for(int i=0;i<16;i++)snprintf(hex+i*2,3,"%02x",md5[i]);if(strcmp(hex,asset.hash)){web::ReportError("TH05 original asset digest mismatch");return false;}}
 if(!m_th5Dat1.LoadFile("kaiki1.dat")||!m_th5Dat2.LoadFile("kaiki2.dat")||!CTh5ExtFont::LoadZUNCOM("zun.com"))return false;
 if(GVar().m_bUseMod){if(!m_modDat.LoadFile(GVar().m_modFileName))return false;LoadModConfig();}
 if(!CPC98Font::LoadAnex86BMP("anex86.bmp"))GVar().m_bUseSystemFont=true;
 QueryPerformanceFrequency(&m_performanceFreq);QueryPerformanceCounter(&m_lastFrameTime);srand(unsigned(time(nullptr)));return true;
}
void CGame::Run(){s_pCurGame=this;if(!Initialize())return;practice::Initialize(nullptr);CSchemeSwitcher::GetFirstScheme(&m_pCurScheme,&m_curSchemeID);}
void CGame::OnDeactivate(){m_gameWindow.m_bActive=false;web::ClearKeys();CPMDPlayer::OnDeactivate();}
void CGame::OnActivate(){m_gameWindow.m_bActive=true;CPMDPlayer::OnActivate();}
void CGame::OnGameEnd(){delete m_pCurScheme;m_pCurScheme=nullptr;practice::Shutdown();m_globalVar.SaveConfig();m_globalVar.SaveHighScore();CPC98Font::FinalizeCache();CPMDPlayer::Finalize();m_gameWindow.Destroy();}
void CGame::OnPaintInactivate(){if(m_pCurScheme)m_pCurScheme->Draw();}
}
namespace {
std::unique_ptr<th5w::CGame> game;bool failed=false;uint64_t ticks=0,renderVersion=0,presentVersion=0,coopDrawTick=0,logicVersion=0;double fpsStart=0;int freshFrames=0;int lastStep=0;
int Guard(const std::function<void()>& operation){try{operation();return 1;}catch(const std::exception& e){failed=true;th5w::web::ReportError(e.what());return 0;}catch(...){failed=true;th5w::web::ReportError("TH05 runtime exception");return 0;}}
}
extern "C" {
EMSCRIPTEN_KEEPALIVE int th05_initialize(){if(game)return 0;failed=false;return Guard([]{game=std::make_unique<th5w::CGame>();game->Run();if(!game->m_pCurScheme)throw std::runtime_error("TH05 initialization failed");});}
EMSCRIPTEN_KEEPALIVE int th05_tick(int render){if(!game||failed||!game->m_pCurScheme)return 0;return Guard([render]{using namespace th5w;if(render){web::graphics::BeginFrame();practice::BeginFrame();}original::BeginTimingFrame();lastStep=practice::AdvancedOpen()?0:game->m_pCurScheme->Step();if(render){game->m_pCurScheme->Draw();practice::DrawStatus();practice::EndFrame();++renderVersion;}original::RenderTiming(false);++ticks;++logicVersion;
 if(lastStep&&coop::Run().count>1&&game->m_curSchemeID==4){
  auto& run=coop::Run();auto& g=CGame::GVar();static_cast<CStage*>(game->m_pCurScheme)->PersistCoop();
  if((lastStep==STAGE_END_CLEAR&&g.m_playStage<5)||lastStep==STAGE_END_RESTART){
   const bool restart=lastStep==STAGE_END_RESTART;delete game->m_pCurScheme;game->m_pCurScheme=nullptr;
   if(restart){run.continuing=false;g.m_playStage=run.difficulty==4?6:0;g.OnBeginGame();for(int i=0;i<7;i++)g.m_randomSeed[i]=(run.seed+i*7919)&0x7fffffff;}else ++g.m_playStage;
   g.m_playChara=run.characters[0];auto* next=new CStage;game->m_pCurScheme=next;next->Initialize();lastStep=0;
  }else if(lastStep!=STAGE_END_CLEAR){run.finished=true;lastStep=0;}
 }
 if(lastStep&&coop::Run().count>1&&(game->m_curSchemeID==12||game->m_curSchemeID==7||game->m_curSchemeID==6||(game->m_curSchemeID==11&&!original::Stats().complete))){coop::Run().finished=true;lastStep=0;}
 if(lastStep){CScheme* next=nullptr;int id=0;if(CSchemeSwitcher::SwitchScheme(&next,&id,game->m_curSchemeID,lastStep)){delete game->m_pCurScheme;game->m_pCurScheme=next;game->m_curSchemeID=id;}}});}
EMSCRIPTEN_KEEPALIVE int th05_step(){return th05_tick(1);}
EMSCRIPTEN_KEEPALIVE double th05_frame_seconds(){return game?double(th5w::original::FrameDelay())/std::max(1.f,game->m_fpsLimit):1./56;}
EMSCRIPTEN_KEEPALIVE int th05_alive(){return game&&game->m_pCurScheme&&!failed;}
EMSCRIPTEN_KEEPALIVE int th05_scene(){return game?game->m_curSchemeID:0;}
EMSCRIPTEN_KEEPALIVE int th05_ticks(){return int(ticks);}
EMSCRIPTEN_KEEPALIVE void th05_key(int key,int down){th5w::web::SetKey(key,down!=0);}
EMSCRIPTEN_KEEPALIVE void th05_clear(){th5w::web::ClearKeys();}
EMSCRIPTEN_KEEPALIVE void th05_focus(int active){if(game){if(active)game->OnActivate();else game->OnDeactivate();}}
EMSCRIPTEN_KEEPALIVE void th05_options(int alwaysHitbox,int focusHitbox){th5w::practice::Options().alwaysHitbox=alwaysHitbox!=0;th5w::practice::Options().showHitbox=focusHitbox!=0;}
EMSCRIPTEN_KEEPALIVE void th05_mouse(double x,double y,int down){if(ImGui::GetCurrentContext()){auto& io=ImGui::GetIO();io.MousePos=ImVec2(x,y);if(down>=0)io.MouseDown[0]=down!=0;}}
EMSCRIPTEN_KEEPALIVE void th05_shutdown(){if(game){Guard([]{game->OnGameEnd();});game.reset();}}
EMSCRIPTEN_KEEPALIVE void th05_save(){if(game){game->m_globalVar.SaveConfig();game->m_globalVar.SaveHighScore();}}
}

extern "C" {
EMSCRIPTEN_KEEPALIVE double th05_player_x(){if(game&&game->m_curSchemeID==4){auto* p=static_cast<th5w::CStage*>(game->m_pCurScheme)->BrowserPlayer();if(th5w::coop::Run().count>1)p=static_cast<th5w::CStage*>(game->m_pCurScheme)->Player(th5w::coop::Run().local);if(p)return 32+p->GetCurX()/16;}return 224;}
EMSCRIPTEN_KEEPALIVE double th05_player_y(){if(game&&game->m_curSchemeID==4){auto* p=static_cast<th5w::CStage*>(game->m_pCurScheme)->BrowserPlayer();if(th5w::coop::Run().count>1)p=static_cast<th5w::CStage*>(game->m_pCurScheme)->Player(th5w::coop::Run().local);if(p)return 56+p->GetCurY()/16;}return 376;}
EMSCRIPTEN_KEEPALIVE int th05_replay_open(const char* path,int stage){if(!game||stage<0||stage>6)return 0;return Guard([&]{using namespace th5w;delete game->m_pCurScheme;game->m_pCurScheme=nullptr;auto& g=CGame::GVar();g.m_replay.CloseStage();if(!g.m_replay.LoadFile(path)||!g.m_replay.m_stageFlag[stage])throw std::runtime_error("Invalid uth05win Replay");g.m_bReplayMode=true;g.m_playStage=stage;g.OnBeginGame();auto* next=new CStage;game->m_pCurScheme=next;game->m_curSchemeID=4;next->Initialize();});}
EMSCRIPTEN_KEEPALIVE int th05_replay_frame(){return game?game->m_globalVar.m_replay.ReadFrame():0;}
EMSCRIPTEN_KEEPALIVE int th05_replay_stage(){return game?game->m_globalVar.m_replay.ReadStage():-1;}
}

extern "C" EMSCRIPTEN_KEEPALIVE void th05_practice_configure(int enabled){th5w::practice::Configure(enabled!=0);}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_practice_enabled(){return th5w::practice::Available();}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_practice_tool(int tool){return tool>=0&&tool<th5w::practice::ToolCount?th5w::practice::Enabled(th5w::practice::Tool(tool)):0;}

extern "C" EMSCRIPTEN_KEEPALIVE void th05_language_configure(const char* game,const char* practice){th5w::locale::Configure(game,practice);}
extern "C" EMSCRIPTEN_KEEPALIVE const char* th05_practice_label(){return th5w::locale::Text(u8"\u65e0\u654c");}

extern "C" EMSCRIPTEN_KEEPALIVE void th05_present(){th5w::web::graphics::Present();if(!game)return;const double now=emscripten_get_now();if(!fpsStart)fpsStart=now;if(renderVersion!=presentVersion){++freshFrames;presentVersion=renderVersion;}if(now-fpsStart>=500){game->m_fps=freshFrames*1000./(now-fpsStart);freshFrames=0;fpsStart=now;}}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_replay_save(const char* path){if(!game)return 0;auto snapshot=game->m_globalVar.m_replay;snapshot.CloseStage(false);return snapshot.SaveFile(path);}

extern "C" EMSCRIPTEN_KEEPALIVE int th05_codepage(){return game?game->m_globalVar.m_textCodePage:0;}

extern "C" EMSCRIPTEN_KEEPALIVE int th05_replay_validate(const char* path){if(!game)return 0;th5w::CReplay parsed;return parsed.LoadFile(path);}
extern "C" EMSCRIPTEN_KEEPALIVE int th05_draw(){if(!game||!game->m_pCurScheme)return 0;return Guard([]{th5w::web::graphics::BeginFrame();game->m_pCurScheme->Draw();});}

extern "C" EMSCRIPTEN_KEEPALIVE int th05_input_scene(){if(th5w::original::DemoActive())return 14;if(game&&game->m_curSchemeID==4){auto* stage=static_cast<th5w::CStage*>(game->m_pCurScheme);if(stage->BrowserMenu())return 0;if(stage->BrowserDialogue())return 13;}return game?game->m_curSchemeID:0;}

extern "C" EMSCRIPTEN_KEEPALIVE double th05_display_fps(){return game?game->m_fps:0.;}

extern "C" {
EMSCRIPTEN_KEEPALIVE int th05_coop_start_stock(int count,int local,int difficulty,int seed,int c0,int c1,int c2,int challenge,int localVisibility,int spares,int bombs){
 if(!game||count<2||count>3||local<0||local>=count||difficulty<0||difficulty>4||c0<0||c0>3||c1<0||c1>3||c2<0||c2>3||spares<0||spares>5||bombs<0||bombs>3)return 0;
 return Guard([=]{using namespace th5w;
  delete game->m_pCurScheme;game->m_pCurScheme=nullptr;original::StopDemo();coop::Reset();auto& run=coop::Run();
  run.count=count;run.challenge=challenge!=0;run.localVisibility=localVisibility!=0;run.local=local;run.difficulty=difficulty;run.seed=seed;run.characters[0]=c0;run.characters[1]=c1;run.characters[2]=c2;
  run.initialSpares=spares;run.initialBombs=bombs;
  practice::Configure(false);auto& g=CGame::GVar();g.m_bReplayMode=false;g.m_bPracticeMode=false;g.m_playDifficulty=difficulty;g.m_playStage=difficulty==4?6:0;
  g.m_playChara=c0;g.m_nInitLife=spares+1;g.m_nInitBomb=bombs;g.m_turboMode=true;g.OnBeginGame();g.m_bCanSaveReplay=false;
  for(int i=0;i<7;i++)g.m_randomSeed[i]=(seed+i*7919)&0x7fffffff;
  auto* stage=new CStage;game->m_pCurScheme=stage;game->m_curSchemeID=4;stage->Initialize();
 });
}
EMSCRIPTEN_KEEPALIVE int th05_coop_start(int count,int local,int difficulty,int seed,int c0,int c1,int c2,int challenge,int localVisibility){return th05_coop_start_stock(count,local,difficulty,seed,c0,c1,c2,challenge,localVisibility,2,3);}
EMSCRIPTEN_KEEPALIVE void th05_coop_input(int seat,int buttons,float x,float y,int motionMode){
 auto& run=th5w::coop::Run();if(seat>=0&&seat<run.count&&motionMode>=0&&motionMode<=3&&std::isfinite(x)&&std::isfinite(y))run.input[seat]={static_cast<unsigned short>(buttons),x,y,motionMode};
}
EMSCRIPTEN_KEEPALIVE int th05_coop_value(int seat,int field){
 if(!game||game->m_curSchemeID!=4||th5w::coop::Run().count<2)return -1;return static_cast<th5w::CStage*>(game->m_pCurScheme)->SeatValue(seat,field);
}
EMSCRIPTEN_KEEPALIVE int th05_coop_finished(){return th5w::coop::Run().finished;}
EMSCRIPTEN_KEEPALIVE int th05_sample_buttons(){return game?th5w::PhysicalKeys():0;}
}

extern "C" EMSCRIPTEN_KEEPALIVE int th05_coop_draw(){if(!game||!game->m_pCurScheme)return 0;return Guard([]{th5w::web::graphics::BeginFrame();th5w::practice::BeginFrame();game->m_pCurScheme->Draw();th5w::practice::EndFrame();if(coopDrawTick!=logicVersion){++renderVersion;coopDrawTick=logicVersion;}});}

extern "C" EMSCRIPTEN_KEEPALIVE unsigned int th05_coop_checksum(){if(!game||game->m_curSchemeID!=4||th5w::coop::Run().count<2)return 0;return static_cast<th5w::CStage*>(game->m_pCurScheme)->CoopChecksum();}

extern "C" EMSCRIPTEN_KEEPALIVE int th05_coop_room(){if(!game)return 0;return Guard([]{using namespace th5w;delete game->m_pCurScheme;game->m_pCurScheme=nullptr;coop::Reset();auto* title=new CTitleScreen;title->Initialize(false,0,true);game->m_pCurScheme=title;game->m_curSchemeID=2;});}

// Launcher-owned presentation. It changes neither inputs nor shared world state.
extern "C" EMSCRIPTEN_KEEPALIVE void th05_coop_display(int style,int flags){
 auto& display=th5w::practice::Options();
 display.eosdBar=style==1;display.ringBar=style==2;
 display.hpPhaseMarkers=style==0&&(flags&1);
 display.phaseStars=style==1&&(flags&2);
 display.phaseTimeout=style!=0||(flags&4);display.phaseTimeoutAsTime=flags&8;
 display.numericDream=flags&16;display.numericPower=flags&32;
}

namespace th5w { namespace coop {void CaptureRuntime(CaptureSink& sink){sink.Object(&ticks);sink.Object(&lastStep);}}}
