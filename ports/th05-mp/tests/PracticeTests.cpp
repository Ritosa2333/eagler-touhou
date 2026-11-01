#include "../Game/Game.h"
#include "../Game/Practice/Practice.h"
#include "../Game/Practice/KeyMonitor.h"
#include "../Game/Stage/Stage.h"
#include "../Game/SchemeSwitcher.h"
#include "../Game/MusicRoom/MusicRoom.h"
#include "../Game/MusicRoom/MusicTracks.h"
#include "../Game/SelectReplayScreen/SelectReplayScreen.h"
#include "../Game/Stage/MidBoss/MidBossStage3.h"
#include "../Game/Stage/MidBoss/MidBossStage4.h"
#include "../Game/Stage/MidBoss/MidBossStage5.h"
#include "../Game/Stage/MidBoss/MidBossStageEx.h"
#include "../Game/Stage/Boss/BossStage5.h"
#include "../Game/Stage/Boss/BossStage6.h"
#include "../Game/Stage/Boss/BossStage4.h"
#include <imm.h>
#pragma comment(lib,"imm32.lib")
#include "../Game/TitleScreen/TitleScreen.h"
#include "../Game/SelectCharaScreen/SelectCharaScreen.h"
#include "../Game/Stage/Chara/Chara.h"
#include "../Game/Stage/Dialogue/Dialogue.h"
#include "../Game/Stage/Item/Item.h"
#include "../Game/Stage/Boss/BossStage1.h"
#include "../Game/Stage/Boss/BossStage4Mai.h"
#include "../Game/Stage/Boss/BossStage4Yuki.h"
#include "../Game/Stage/MidBoss/MidBossStage1.h"
#include "../Game/Stage/CharaBullet/CharaBullet.h"
#include "../Game/Stage/EnemyBullet/EnemyBullet.h"
#include "../Game/Stage/FloatingText/FloatingText.h"
#include "../third_party/imgui/imgui.h"
#include "../third_party/imgui/imgui_internal.h"
#include <cstdio>
#include <sstream>
#include <cstdlib>
#include <new>
#include <vector>
#include <gl/GL.h>
namespace th5w {
struct PracticeTests {
 static int checks;
 static void Check(bool ok,const char* reason) { ++checks; if (!ok) { fprintf(stderr,"FAIL: %s\n",reason); exit(1); } }
 static void Key(CGame& game,int key,bool repeat=false) { practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,key,repeat?(1L<<30):0); practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,key,1L<<31); }
 static void ResetTools() { for(int i=0;i<practice::ToolCount;i++) if(practice::Enabled(practice::Tool(i))) practice::Toggle(practice::Tool(i)); }
 static CStage* Stage(CGame& game) {
  CGame::GVar().m_playStage=0; CGame::GVar().m_playChara=0; CGame::GVar().m_playDifficulty=1;
  CGame::GVar().m_bReplayMode=false; CGame::GVar().m_bPracticeMode=false;
  CGame::GVar().OnBeginGame(); CStage* s=new CStage; s->Initialize();
  s->m_bLogo=false; s->m_bCanPause=true; s->m_bInCharaBomb=false;
  s->m_pChara->m_invulnerableTimeLeft=0;s->m_pChara->m_bCharaCanShoot=false;
  s->m_pChara->m_cannotControlTimeLeft=0;s->m_curGameKeyState=s->m_lastGameKeyState=0;
  game.m_pCurScheme=s;game.m_curSchemeID=4;return s;
 }
 static void Screenshot(const char* name="pause.bmp") {
  std::vector<unsigned char> pixels(640*480*3);glPixelStorei(GL_PACK_ALIGNMENT,1);glReadPixels(0,0,640,480,GL_BGR_EXT,GL_UNSIGNED_BYTE,&pixels[0]);
  BITMAPFILEHEADER f={};f.bfType=0x4d42;f.bfOffBits=sizeof(f)+sizeof(BITMAPINFOHEADER);f.bfSize=f.bfOffBits+(DWORD)pixels.size();
  BITMAPINFOHEADER h={};h.biSize=sizeof(h);h.biWidth=640;h.biHeight=480;h.biPlanes=1;h.biBitCount=24;
  FILE* out=fopen(name,"wb");fwrite(&f,sizeof(f),1,out);fwrite(&h,sizeof(h),1,out);fwrite(&pixels[0],pixels.size(),1,out);fclose(out);
 }
 static void Frame(CStage& s) { practice::BeginFrame();s.Draw();practice::DrawStatus();practice::EndFrame();glFinish(); }
 static void CheckPracticeHud(CStage& stage) {
  Check(strcmp(stage.m_pStageRes->m_crossRow,"\x81\x40\x81\x40\x81\x7e\x81\x40\x81\x40")==0,"unmodified resources initialize the native life/bomb multiplication label");
  Frame(stage);
  std::vector<unsigned char> pixels(24*480*3);
  glPixelStorei(GL_PACK_ALIGNMENT,1);glReadPixels(0,0,24,480,GL_RGB,GL_UNSIGNED_BYTE,&pixels[0]);
  int lit=0;for(size_t i=0;i<pixels.size();i++)if(pixels[i]>32)++lit;
  if(lit) {fprintf(stderr,"Practice HUD left-margin components: %d\n",lit);Screenshot("practice-hud-regression.bmp");}
  Check(lit==0,"Practice HUD must not draw stray multiplication glyphs in the left margin");
  Screenshot("practice-hud-fixed.bmp");
 }
 static void InactiveFloatingText(CStage& stage) {
  alignas(CFloatingText) unsigned char storage[sizeof(CFloatingText)]={};
  CFloatingText* text=new(storage) CFloatingText(&stage);
  for(int cycle=0;cycle<2;cycle++) {
   for(int frame=0;frame<256;frame++) {
    glColor4f(.25f,.5f,.75f,1);text->Step();text->Draw();
    float color[4];glGetFloatv(GL_CURRENT_COLOR,color);
    Check(color[0]==.25f&&color[1]==.5f&&color[2]==.75f,"inactive floating text never draws a glyph or changes render state");
   }
   text->SetTextType(FLOATING_TEXT_BONUS,12345);
   for(int i=0;i<12;i++)text->Step();
   text->Initialize();
  }
  text->~CFloatingText();
 }
 static void NativePractice(CGame& game) {
  ResetTools(); auto& g=CGame::GVar();
  g.m_replay.CloseStage(false);
  CTitleScreen title;title.Initialize(false,2,false);title.m_bMenuOn=true;title.m_curScrFade=100;title.m_bgFade=100;
  Check(ImmGetContext(game.m_gameWindow.m_hWnd)==NULL,"game window has no IME composition context");
  Check(title.m_mainMenuNItem==8&&title.m_bMainMenuItemEnabled[2],"Practice Start is the third native title item");
  title.Draw();glFinish();Screenshot("practice-title.bmp");

  CScheme* flow=NULL;int flowID=0;
  memset(g.m_practiceUnlocked,0,sizeof(g.m_practiceUnlocked));
  memset(g.m_bClear,0,sizeof(g.m_bClear));
  g.m_playChara=0;g.m_playDifficulty=1;g.m_playStage=0;
  g.m_bPracticeMode=false;g.m_bReplayMode=false;practice::ResetRun();
  Check(!g.PracticeStageUnlocked(0),"uncleared stages start locked");
  g.OnBeginGame();CStage* entered=new CStage;entered->Initialize();
  Check(g.PracticeStageUnlocked(0)&&!g.PracticeStageUnlocked(1),"stage initialization unlocks its Practice before the first gameplay frame");
  g.m_replay.CloseStage(false);delete entered;
  g.m_playChara=1;Check(!g.PracticeStageUnlocked(0),"stage unlock is character-specific");
  g.m_playChara=0;g.m_playDifficulty=2;Check(!g.PracticeStageUnlocked(0),"stage unlock is difficulty-specific");
  g.m_playDifficulty=1;g.m_playStage=1;g.m_bPracticeMode=true;g.OnStageEnter();Check(!g.PracticeStageUnlocked(1),"Practice entry cannot unlock stages");
  g.m_bPracticeMode=false;g.m_bReplayMode=true;g.OnStageEnter();Check(!g.PracticeStageUnlocked(1),"Replay entry cannot unlock stages");
  g.m_bReplayMode=false;Key(game,VK_F1);g.OnStageEnter();Check(!g.PracticeStageUnlocked(1),"assisted entry cannot unlock stages");ResetTools();practice::ResetRun();
  memset(g.m_practiceUnlocked,0,sizeof(g.m_practiceUnlocked));g.LoadPracticeProgress();
  Check(g.PracticeStageUnlocked(0)&&!g.PracticeStageUnlocked(1),"unlock progress persists across reload");
  { FILE* cfg=fopen("game.cfg","rb");Check(cfg!=NULL,"unlock uses native game.cfg");
    fseek(cfg,0,SEEK_END);Check(ftell(cfg)==101,"native config retains its legacy prefix, progress extension and speed option");fclose(cfg); }
  g.LoadConfig();g.LoadPracticeProgress();Check(g.PracticeStageUnlocked(0),"native configuration reload preserves entry unlock");
  g.m_bClear[2][3]=true;g.LoadPracticeProgress();g.m_playChara=2;g.m_playDifficulty=3;
  Check(g.PracticeStageUnlocked(5),"existing full-game clear migrates completed stages");g.m_bClear[2][3]=false;
  g.m_bPracticeMode=true;g.m_bReplayMode=false;g.m_playChara=0;g.m_playDifficulty=1;g.m_playStage=0;

  Check(CSchemeSwitcher::SwitchScheme(&flow,&flowID,2,TITLESCREEN_END_START_PRACTICE)&&flowID==3,"Practice Start enters native character select");
  CSelectCharaScreen* select=static_cast<CSelectCharaScreen*>(flow);
  for(int i=0;i<4;i++)Check(select->m_bCharaSelectable[i],"Practice uses all normal character choices");
  select->m_bStageSelect=true;select->m_stageSelectAge=30;select->m_curScrFade=100;
  select->ParseStageKey(1);Check(select->m_stageCursor==5,"Practice stage list wraps upward to stage six");
  select->ParseStageKey(0);select->ParseStageKey(16);
  Check(select->m_bStageSelect&&select->m_stageSelectClosing&&!select->m_bQuit,"X begins closing without instantly removing the window");
  select->Step();Check(select->m_bStageSelect&&select->m_stageSelectAge==9,"closing animation moves inward on next frame");
  select->Draw();glFinish();Screenshot("practice-close-09.bmp");
  for(int frame=0;frame<9;frame++) select->Step();
  Check(!select->m_bStageSelect&&!select->m_stageSelectClosing,"closing returns to character selection after ten frames");
  select->m_bStageSelect=true;select->m_lastKeyState=0;select->m_stageSelectAge=30;
  select->Draw();glFinish();Screenshot("practice-stage.bmp");
  const int animationFrames[]={0,2,5,10};
  for(int n=0;n<4;n++) {
   select->m_stageSelectAge=animationFrames[n];
   select->Draw();glFinish();char path[64];sprintf_s(path,"practice-expand-%02d.bmp",animationFrames[n]);Screenshot(path);
  }
  select->m_stageSelectAge=30;
  select->ParseStageKey(32);
  Check(!select->m_bQuit,"confirming a locked stage does not start gameplay");
  g.m_practiceUnlocked[0][1]|=1<<5;
  select->ParseStageKey(0);select->ParseStageKey(32);
  Check(select->m_bQuit&&g.m_playStage==5&&g.m_bPracticeMode,"native stage confirmation starts selected stage");
  Check(g.m_nCurLife==9&&g.m_nCurBomb==8&&g.m_curPower==128,"Practice starts with eight spare lives, eight bombs and full power");
  delete select;
  // Exercise real production initialization, input stream, disk round trip,
  // playback state and end routing independently for every selectable stage.
  for(int stage=0;stage<6;stage++) {
   g.m_bReplayMode=false;g.m_bPracticeMode=true;g.m_playStage=stage;g.m_playChara=stage%4;g.OnBeginGame();
   CStage* record=new CStage;record->Initialize();game.m_pCurScheme=record;
   Check(record->m_nCurLife==9&&record->m_nCurBomb==8,"per-stage native resources are 8 spare lives / 8 bombs");
   std::vector<double> states;
   for(int frame=0;frame<1200;frame++) {
    unsigned short input=32 | ((frame/60)%2?64:0) | ((frame/90)%2?4:8);
    record->StepFrame(input);
    states.push_back(record->m_curFrame);states.push_back(record->m_curScore);
    states.push_back(record->m_pChara->m_curX);states.push_back(record->m_pChara->m_curY);
    states.push_back(record->m_nCurLife);states.push_back(record->m_nCurBomb);
    states.push_back(record->m_pChara->m_power);
   }
   CheckPracticeHud(*record);
   g.m_bCanSaveReplay=false; // A stale menu flag must not suppress a completed Practice save.
   record->SetQuit(STAGE_END_CLEAR);int end=0;for(int f=0;f<60&&!end;f++)end=record->Step();
   Check(end==STAGE_END_CLEAR&&g.m_replay.m_stageFlag[stage]==2,"single-stage clear closes recorded stream as cleared");
   Check(g.m_bCanSaveReplay&&g.m_replay.m_bPracticeMode,"normal Practice remains replay-save eligible");
   Check(g.m_replay.SaveFile("practice-roundtrip.rpy"),"Practice replay writes native file format");
   REPLAY_BRIEF_INFO info={};Check(CReplay::GetRepFileInfo(&info,"practice-roundtrip.rpy")&&info.stageFlag[stage]==2,"saved Practice is selectable in native replay browser");
   CScheme* save=NULL;int saveID=0;
   Check(CSchemeSwitcher::SwitchScheme(&save,&saveID,4,end)&&saveID==6&&g.m_playStage==stage,"single-stage clear goes directly to save replay without next stage");{
    CSelectReplayScreen* page=static_cast<CSelectReplayScreen*>(save);
    Check(!page->m_bQuit&&!page->m_bLoadMode,"Practice clear presents an active save page");
    page->m_bEnterNameMode=true;page->m_curRowY=(page->m_listUpperLeftY+4)*16;
    page->m_curCharX=12;page->m_curCharY=6;page->m_selectedSlot=stage;
    memset(page->m_curEnterName,0,10);memcpy(page->m_curEnterName,"PLAYER",6);
    page->m_lastKeyState=0;page->m_curKeyState=32;page->StepEnterNameMode();
    char saved[64];sprintf_s(saved,"replay/replay%02d.rpy",stage);REPLAY_BRIEF_INFO savedInfo={};
    Check(CReplay::GetRepFileInfo(&savedInfo,saved)&&savedInfo.stageFlag[stage]==2,"native name-grid END actually saves the completed Practice replay");
   }delete save;delete record;
   Check(g.m_replay.LoadFile("practice-roundtrip.rpy"),"native Practice replay reloads");
   g.m_bReplayMode=true;g.m_playStage=stage;g.OnBeginGame();
   CStage* playback=new CStage;playback->Initialize();game.m_pCurScheme=playback;
   bool equal=true;size_t n=0;
   for(int frame=0;frame<1200;frame++) {
    playback->StepFrame(0);
    double state[]={double(playback->m_curFrame),double(playback->m_curScore),double(playback->m_pChara->m_curX),double(playback->m_pChara->m_curY),double(playback->m_nCurLife),double(playback->m_nCurBomb),double(playback->m_pChara->m_power)};
    for(int i=0;i<7;i++)if(state[i]!=states[n++])equal=false;
   }
   Check(equal,"saved Practice playback matches every recorded frame state");
   playback->SetQuit(STAGE_END_CLEAR);end=0;for(int f=0;f<60&&!end;f++)end=playback->Step();
   Check(end==STAGE_END_REPLAY_END,"Practice playback clear terminates rather than advancing stages");
   delete playback;game.m_pCurScheme=NULL;
  }
  g.m_bReplayMode=false;g.m_bPracticeMode=true;g.m_playStage=2;g.OnBeginGame();
  CScheme* save=NULL;int id=0;
  Check(CSchemeSwitcher::SwitchScheme(&save,&id,4,STAGE_END_ALL_MISSED)&&id==6,"Practice game over also goes straight to replay save");delete save;
  bool oldClear=g.m_bClear[g.m_playChara][g.m_playDifficulty];g.m_bClear[g.m_playChara][g.m_playDifficulty]=false;g.OnClear();
  Check(!g.m_bClear[g.m_playChara][g.m_playDifficulty],"Practice does not unlock normal-game clears");g.m_bClear[g.m_playChara][g.m_playDifficulty]=oldClear;
 }
 static void CustomPractice(CGame& game){
  ResetTools();auto& g=CGame::GVar();g.m_replay.CloseStage(false);g.m_bReplayMode=false;g.m_initDifficulty=1;
  practice::OpenSetup(0);auto& menu=practice::MenuConfig();
  menu=practice::Config();menu.mode=1;menu.warp=3;menu.frame=1000;menu.lockRank=1;menu.rank=48;
  CSelectCharaScreen setup;setup.Initialize();setup.m_curScrFade=100;setup.m_bStageSelect=setup.m_bThpracSetup=true;setup.m_stageSelectAge=20;
  for(int i=0;i<12;i++){practice::BeginFrame();setup.Draw();practice::EndFrame();glFinish();}
  Screenshot("thprac-custom-menu.bmp");
  ImGuiWindow* window=ImGui::FindWindowByName("TH05 Practice");
  Check(window&&window->Size.x==330&&window->Size.y==430,"custom menu uses purple thprac's borderless ImGui layout");
  Check(window->ScrollMax.y==0,"all custom fields fit without clipping");
  Check(ImGui::GetFont()->FindGlyphNoFallback(0x5e27)!=NULL,"frame glyph is present instead of falling back to a question mark");
  const auto setupKey=[&](int key){
   practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,key,0);
   practice::BeginFrame();setup.Draw();practice::EndFrame();const int action=setup.m_setupAction;
   practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,key,1L<<31);
   practice::BeginFrame();setup.Draw();practice::EndFrame();return action;
  };
  ImGui::FocusWindow(window);ImGui::SetFocusID(window->GetID(u8"\u5173\u5361"),window);GImGui->NavDisableHighlight=false;
  Check(setupKey(VK_RIGHT)==0&&menu.stage==1,"focused combo uses purple left/right adjustment without starting");
  menu.stage=0;
  Check(setupKey('Z')==1,"Z starts directly from a focused parameter without an invented button");
  Check(!ImGui::IsPopupOpen(static_cast<const char*>(NULL),ImGuiPopupFlags_AnyPopupId),"Z does not also open the parameter combo");
  Check(setupKey('X')==2,"X requests return directly from the practice menu");
  setup.m_setupAction=2;setup.Step();Check(setup.m_stageSelectClosing&&setup.m_stageSelectAge==8,"X begins the purple menu's fade-out");
  for(int i=0;i<8;i++)setup.Step();Check(!setup.m_bThpracSetup&&!setup.m_bQuit,"X returns to character selection after fade-out");
  setup.m_bThpracSetup=setup.m_bStageSelect=true;setup.m_stageSelectAge=30;
  practice::SetAdvancedOpen(true);Check(setupKey('Z')==0,"F12 prevents hidden practice parameters from consuming Z");practice::SetAdvancedOpen(false);
  menu.stage=5;menu.warp=1;Check(practice::StartSetup(),"stage six midboss selection is a valid start-of-stage fallback");
  setup.m_bThpracSetup=setup.m_bStageSelect=true;setup.m_stageSelectClosing=false;setup.m_stageSelectAge=30;
  practice::BeginFrame();setup.Draw();practice::EndFrame();Screenshot("thprac-stage6-no-mid.bmp");
  menu.mode=0;menu.stage=0;memset(g.m_practiceUnlocked,0,sizeof(g.m_practiceUnlocked));g.m_playChara=0;g.m_playDifficulty=1;
  Check(!practice::StartSetup(),"normal mode still obeys native stage unlocks");
  // Locking never expands the native difficulty range, including fixed Extra.
  for(int difficulty=0;difficulty<5;difficulty++)for(int locked=0;locked<2;locked++){
   practice::Config p;p.mode=1;p.lockRank=locked;
   p.rank=0;p.Clamp(difficulty);Check(p.rank==practice::Config::RankMin(difficulty),"rank lower limit stays native with or without locking");
   p.rank=64;p.Clamp(difficulty);Check(p.rank==practice::Config::RankMax(difficulty),"rank upper limit stays native with or without locking");
   const int rank=p.rank;p.lockRank=!locked;p.Clamp(difficulty);Check(p.rank==rank,"toggling rank lock preserves the selected legal value");
  }
  practice::Config legacy;legacy.mode=1;legacy.lockRank=1;legacy.rank=0;
  Check(!legacy.Valid(1)&&legacy.ValidReplay(1),"old locked zero-rank replay remains valid only for replay input");
  legacy.rank=64;Check(!legacy.Valid(4)&&legacy.ValidReplay(4),"old Extra replay retains out-of-range locked rank");
  legacy.rank=65;Check(!legacy.ValidReplay(1),"legacy replay rank still rejects values above 64");
  // Every supported stage / destination round-trips through the real disk replay.
  for(int stage=0;stage<7;stage++)for(int warp=0;warp<4;warp++){
   menu=practice::Config();menu.mode=1;menu.stage=stage;menu.warp=warp;menu.frame=1000;
   menu.lives=3;menu.bombs=2;menu.score=123450;menu.power=69;menu.graze=321;menu.points=123;menu.totalPoints=4567;menu.dream=70;menu.rank=48;menu.lockRank=1;
   const int expectedRank=practice::Config::RankMax(stage==6?4:g.m_initDifficulty);
   g.m_bReplayMode=false;g.m_playChara=stage%4;
   Check(practice::StartSetup(),"custom mode starts locked stages and Extra independently of native unlocks");
   Check(g.m_nCurLife==4&&g.m_nCurBomb==2&&g.m_curPower==69&&g.m_curScore==12345&&g.m_curDream==70,"all configured initial resources use native units");
   Check(g.m_totalNPointItem==4567&&g.m_curNGraze==321&&g.m_playerPerformance==expectedRank&&g.m_bCanSaveReplay,"custom counters and rank preserve replay eligibility");
   CStage* record=new CStage;record->Initialize();game.m_pCurScheme=record;game.m_curSchemeID=4;
   Check(record->m_curNGraze==321&&record->m_curNPointItem==123&&record->m_totalNPointItem==4567,"HUD counters receive custom graze and blue points");
   Check(record->m_playerPerformanceLowerBound==expectedRank&&record->m_playerPerformanceUpperBound==expectedRank,"locked rank constrains both rank bounds");
   record->ChangePlayerPerformance(-100);record->ChangePlayerPerformance(100);
   Check(record->m_playerPerformance==expectedRank,"locked rank rejects increases and decreases");
   if(warp==1&&stage!=5){const int mid[]={2500,2750,5750,3900,4800,0,5800};Check(record->m_curFrame==mid[stage]-1,"midboss seek stops immediately before the native spawn frame");}
   if(warp==1&&stage==5)Check(record->m_curFrame==-1&&record->m_bLogo&&record->m_pMidBoss==NULL,"sixth-stage missing midboss starts from the original beginning");
   if(warp==2)Check(record->m_pBoss!=NULL&&!record->m_pDialogue->IsRunning(),"boss warp executes original dialogue resource loading and enters the real boss");
   if(warp==3)Check(record->m_curFrame==999,"frame warp begins at the selected timeline frame");
   Frame(*record);char shot[64];sprintf_s(shot,"custom-stage-%d-warp-%d.bmp",stage+1,warp);Screenshot(shot);
   std::vector<double> states;
   const auto capture=[&](CStage* st){
    const double v[]={double(st->m_curFrame),st->m_pChara->m_curX,st->m_pChara->m_curY,double(st->m_curScore),double(st->m_nCurLife),double(st->m_nCurBomb),double(st->m_pChara->m_power),double(st->m_curDream),double(st->m_playerPerformance),double(st->m_curNGraze),double(st->m_curNPointItem),double(st->m_totalNPointItem)};
    return std::vector<double>(v,v+12);
   };
   for(int frame=0;frame<360;frame++){
    unsigned short input=((frame/60)%2?4:8)|((frame/45)%2?64:0);
    record->StepFrame(input);const auto v=capture(record);states.insert(states.end(),v.begin(),v.end());
   }
   if(warp==1&&stage!=5)Check(record->m_pMidBoss!=NULL,"midboss teleport spawns the native midboss, not a replacement encounter");
   const int nextRandom=rand();
   record->SetQuit(STAGE_END_SAVE_EXIT);int end=0;for(int i=0;i<60&&!end;i++)end=record->StepFrame(0);
   Check(end==STAGE_END_SAVE_EXIT,"custom run closes the recording through the save action");
   Check(g.m_replay.SaveFile("custom-roundtrip.rpy"),"custom replay saves its versioned initial parameters");
   REPLAY_BRIEF_INFO info={};Check(CReplay::GetRepFileInfo(&info,"custom-roundtrip.rpy"),"custom replay is accepted by the native replay browser");
   g.m_replay.m_turboMode=false;Check(g.m_replay.SaveFile("custom-speed.rpy"),"Slow custom practice stores both TPC5 and timing extensions");
   CReplay slow;Check(slow.LoadFile("custom-speed.rpy")&&!slow.m_turboMode&&slow.m_practiceConfig.warp==warp&&slow.m_keyState[stage]==g.m_replay.m_keyState[stage],"Slow custom practice restores native parameters and complete input stream");
   Check(CReplay::GetRepFileInfo(&info,"custom-speed.rpy"),"Slow custom practice remains visible in replay browser");g.m_replay.m_turboMode=true;
   delete record;game.m_pCurScheme=NULL;
   if((stage==0&&warp==3)||(stage==3&&warp==2)){
    CScheme* restarted=NULL;int id=0;
    Check(CSchemeSwitcher::SwitchScheme(&restarted,&id,4,STAGE_END_RESTART)&&id==4,"custom restart constructs a fresh real stage");
    CStage* st=static_cast<CStage*>(restarted);
    Check(g.m_practiceConfig.warp==warp&&st->m_nCurLife==4&&st->m_curNPointItem==123&&st->m_totalNPointItem==4567&&st->m_curScore==12345,"restart restores original custom destination and independent counters");
    g.m_replay.CloseStage(false);delete st;
   }
   Check(g.m_replay.LoadFile("custom-roundtrip.rpy"),"custom replay loads the complete parameter extension");
   Check(g.m_replay.m_practiceConfig.warp==warp&&g.m_replay.m_practiceConfig.rank==expectedRank&&g.m_replay.m_practiceConfig.graze==321,"replay metadata restores destination, rank lock and counters");
   g.m_bReplayMode=true;g.m_playStage=stage;g.OnBeginGame();
   CStage* playback=new CStage;playback->Initialize();game.m_pCurScheme=playback;Frame(*playback);
   bool equal=true;size_t offset=0;
   for(int frame=0;frame<360;frame++){
    playback->StepFrame(0);const auto v=capture(playback);for(auto n:v)if(n!=states[offset++])equal=false;
   }
   Check(equal,"custom replay matches all twelve gameplay state values on every recorded frame");
   Check(rand()==nextRandom,"custom recording and playback consume identical gameplay RNG sequences");
   g.m_replay.CloseStage(false);delete playback;game.m_pCurScheme=NULL;
  }
  g.m_bReplayMode=false;menu=practice::Config();menu.mode=1;menu.totalPoints=99;menu.points=7;menu.lives=3;menu.rank=48;menu.lockRank=1;
  Check(practice::StartSetup(),"independent blue-point counters initialize");CStage* counters=new CStage;counters->Initialize();game.m_pCurScheme=counters;
  counters->m_pChara->CaughtItem(0,0,ITEM_POINT);
  Check(counters->m_totalNPointItem==100&&counters->m_curNPointItem==8&&counters->m_nCurLife==5,"point pickup increments both counters and extends from cumulative count only");
  auto& chara=*counters->m_pChara;chara.m_invulnerableTimeLeft=0;chara.m_nextLifeComeTimeLeft=33;chara.m_cannotControlTimeLeft=1;chara.m_bMiss=false;chara.Step();
  Check(counters->m_playerPerformance==practice::Config::RankMax(g.m_playDifficulty),"locked rank survives the native Miss rank reset");
  g.m_replay.CloseStage(false);delete counters;game.m_pCurScheme=NULL;
  for(int bound=0;bound<2;bound++){
   menu=practice::Config();menu.mode=1;menu.lockRank=1;menu.lives=bound?8:0;menu.bombs=bound?8:0;menu.power=bound?128:0;menu.dream=bound?128:1;menu.rank=bound?64:0;menu.points=bound?999:0;menu.totalPoints=bound?99999:0;menu.score=bound?99999990:0;
   Check(practice::StartSetup(),"custom native resource boundaries can start");CStage* st=new CStage;st->Initialize();game.m_pCurScheme=st;
   Check(st->m_nCurLife==(bound?9:1)&&st->m_nCurBomb==(bound?8:0)&&st->m_pChara->m_power==(bound?128:1)&&st->m_curDream==(bound?128:1)&&st->m_playerPerformance==(bound?practice::Config::RankMax(g.m_playDifficulty):practice::Config::RankMin(g.m_playDifficulty)),"configured lower and upper resource bounds match actual native state");
   Frame(*st);g.m_replay.CloseStage(false);delete st;game.m_pCurScheme=NULL;
  }
  // Hidden custom fields must not disable normal mode on Hard/Lunatic.
  menu.mode=0;menu.lockRank=0;menu.rank=0;g.m_initDifficulty=g.m_playDifficulty=2;g.m_practiceUnlocked[g.m_playChara][2]|=1;
  practice::BeginFrame();practice::DrawSetup(30);practice::EndFrame();
  Check(practice::StartSetup()&&!g.m_practiceConfig.mode,"normal mode can start an unlocked stage after switching from custom fields");
  g.m_initDifficulty=1;
  // Missing custom metadata must not silently play as a normal native replay.
  {FILE* fp=fopen("custom-roundtrip.rpy","rb");fseek(fp,0,SEEK_END);long size=ftell(fp);rewind(fp);std::vector<char> bytes(size);fread(bytes.data(),1,size,fp);fclose(fp);
   fp=fopen("custom-truncated.rpy","wb");fwrite(bytes.data(),1,size-1,fp);fclose(fp);CReplay invalid;
   Check(!invalid.LoadFile("custom-truncated.rpy"),"truncated custom parameters are rejected");
   REPLAY_BRIEF_INFO info={};Check(!CReplay::GetRepFileInfo(&info,"custom-truncated.rpy")&&info.infoRes==InfoRes_InvalidReplay,"browser rejects a custom replay missing required metadata");}
  Check(g.m_replay.LoadFile("practice-roundtrip.rpy")&&!g.m_replay.m_practiceConfig.mode,"legacy native Practice replay still loads without thprac metadata");
  g.m_bReplayMode=true;g.m_playStage=5;g.OnBeginGame();Check(!g.m_practiceConfig.mode&&g.m_playerPerformanceLowerBound!=48,"legacy replay does not inherit current custom rank locking");
  g.m_bReplayMode=false;g.m_bPracticeMode=false;g.m_playDifficulty=1;g.OnBeginGame();
  Check(!g.m_practiceConfig.mode&&!practice::Assisted(),"normal gameplay clears custom initial settings and assisted state");
 }
 static void PhasePractice(CGame& game){
  ResetTools();auto& g=CGame::GVar();practice::Options()=practice::AdvancedOptions();int cases=0;
  for(int stage=0;stage<7;stage++)for(int difficulty=0;difficulty<(stage==6?1:4);difficulty++)for(int warp=1;warp<=2;warp++){
   int count=0;const auto* list=practice::Sections(stage,warp,count);
   for(int index=0;index<count;index++)for(int rage=0;rage<=list[index].rageCount;rage++){
    const auto& selected=list[index];const bool entryChoice=stage==1&&warp==1&&selected.phase==2;
    const bool rageChoice=stage==5&&warp==2&&selected.phase==10&&rage;
    for(int trigger=0;trigger<=(entryChoice||rageChoice?1:0);trigger++){
     auto& menu=practice::MenuConfig();menu=practice::Config();menu.mode=1;menu.stage=stage;menu.warp=warp;menu.section=index+1;menu.rage=rage;menu.trigger=trigger;menu.score=123450;menu.graze=111;menu.totalPoints=222;menu.points=33;
     if(selected.continuous)menu.startHP=stage==0&&warp==1&&rage?100:437;
     g.m_bReplayMode=false;g.m_initDifficulty=difficulty;g.m_playChara=0;
     Check(practice::StartSetup(),"every native phase and rage preset can start");
     fprintf(stderr,"PHASE stage=%d difficulty=%d warp=%d section=%d rage=%d trigger=%d\n",stage+1,difficulty,warp,index+1,rage,trigger);
     CStage* record=new CStage;record->Initialize();game.m_pCurScheme=record;game.m_curSchemeID=4;
     const int phase=warp==1?record->m_pMidBoss->m_curPhase:record->m_pBoss->m_curPhase;
     Check(phase==selected.phase,"selected phase enters the actual native state");
     Check(record->m_curScore==12345&&record->m_curNGraze==111&&record->m_totalNPointItem==222&&record->m_curNPointItem==33,"phase initialization preserves configured score and counters");
     Check(g.m_replay.m_keyState[stage].empty(),"phase initialization records no synthetic inputs");
     if(stage==3&&warp==2){Check((selected.variant==1)==(dynamic_cast<CBossStage4Mai*>(record->m_pBoss)!=NULL),"Mai selection follows the native branch");Check((selected.variant==2)==(dynamic_cast<CBossStage4Yuki*>(record->m_pBoss)!=NULL),"Yuki selection follows the native branch");}
     if(entryChoice)Check(record->m_pMidBoss->m_curHP==(trigger?1400:400),"stage-two midboss preserves the chosen entry HP");
     if(selected.continuous)Check((warp==1?record->m_pMidBoss->m_curHP:record->m_pBoss->m_curHP)==menu.startHP,"continuous rage uses the selected starting HP");
     if(rageChoice){auto* boss=static_cast<CBossStage6*>(record->m_pBoss);Check(boss->m_curHP==(trigger?8600:rage==1?5600:3800),"Shinki rage preserves selected HP/time trigger semantics");Check(boss->m_phase10Timer>=(rage==1?65:66),"Shinki enters the native laser rage state");}
     if(stage==4&&warp==2&&rage){auto* boss=static_cast<CBossStage5*>(record->m_pBoss);Check(selected.phase==4?boss->m_phase4CrazyMode:boss->m_phase7CrazyMode,"Yumeko rage is the actual native CrazyMode");}
     if(stage==2&&warp==2&&selected.phase>=13)Check(record->m_pBoss->m_curHP==800,"Alice final native phase starts at 800 HP");
     const auto capture=[&](CStage* st){
      const int current=warp==1?st->m_pMidBoss->m_curPhase:st->m_pBoss->m_curPhase;
      const int frame=warp==1?st->m_pMidBoss->m_curPhaseFrame:st->m_pBoss->m_curPhaseFrame;
      const int hp=warp==1?st->m_pMidBoss->m_curHP:st->m_pBoss->m_curHP;
      const double v[]={double(st->m_curFrame),double(current),double(frame),double(hp),st->m_pChara->m_curX,st->m_pChara->m_curY,double(st->m_curScore),double(st->m_nCurLife),double(st->m_nCurBomb)};return std::vector<double>(v,v+9);
     };
     std::vector<double> states;for(int f=0;f<100;f++){record->StepFrame((f/30)%2?4:8);const auto v=capture(record);states.insert(states.end(),v.begin(),v.end());}
     if(stage==2&&warp==2&&selected.phase>=13)Check(record->m_pBoss->m_curHP==800,"Alice final practice keeps native 800 HP without delayed puppet death damage");
     const int nextRandom=rand();record->SetQuit(STAGE_END_SAVE_EXIT);int end=0;for(int f=0;f<60&&!end;f++)end=record->StepFrame(0);
     Check(g.m_replay.SaveFile("phase-roundtrip.rpy"),"phase preset saves the version-two TPC5 extension");delete record;game.m_pCurScheme=NULL;
     Check(g.m_replay.LoadFile("phase-roundtrip.rpy"),"phase preset reloads from the real replay file");
     const auto& saved=g.m_replay.m_practiceConfig;Check(saved.section==index+1&&saved.rage==rage&&saved.trigger==trigger&&saved.startHP==menu.startHP,"replay restores all phase preset fields");
     g.m_bReplayMode=true;g.m_playStage=stage;g.OnBeginGame();CStage* playback=new CStage;playback->Initialize();game.m_pCurScheme=playback;
     bool equal=true;size_t at=0;for(int f=0;f<100;f++){playback->StepFrame(0);const auto v=capture(playback);for(double n:v){if(n!=states[at]){if(equal)fprintf(stderr,"DIFF frame=%d field=%d actual=%f expected=%f\n",f,int(at%9),n,states[at]);equal=false;}++at;}}
     Check(equal&&rand()==nextRandom,"phase replay matches every gameplay frame and the native RNG sequence");
     if((stage==2&&warp==2&&selected.phase==5)||(rageChoice&&rage==2&&trigger==1)||(selected.continuous)){
      Frame(*playback);char shot[96];sprintf_s(shot,"phase-stage-%d-warp-%d-section-%d-rage-%d-trigger-%d.bmp",stage+1,warp,index+1,rage,trigger);Screenshot(shot);
     }
     g.m_replay.CloseStage(false);delete playback;game.m_pCurScheme=NULL;++cases;
    }
   }
  }
  // Convert a genuine whole-boss legacy-compatible fixture back to TPC5 v1.
  {FILE* fp=fopen("custom-roundtrip.rpy","rb");fseek(fp,0,SEEK_END);long size=ftell(fp);rewind(fp);std::vector<char> bytes(size);fread(bytes.data(),1,size,fp);fclose(fp);
   const int at=int(bytes.size())-sizeof(practice::Config)-12;unsigned int v=1,n=56;memcpy(bytes.data()+at+4,&v,4);memcpy(bytes.data()+at+8,&n,4);bytes.resize(bytes.size()-20);
   fp=fopen("custom-v1.rpy","wb");fwrite(bytes.data(),1,bytes.size(),fp);fclose(fp);
   Check(g.m_replay.LoadFile("custom-v1.rpy")&&g.m_replay.m_practiceConfig.section==0&&g.m_replay.m_practiceConfig.startHP==-1,"legacy fourteen-field TPC5 recordings retain whole-boss defaults");}
  g.m_bReplayMode=false;
  CBossStage4 nativePair;Check(nativePair.m_2forceGoUpOrDown==0,"ordinary stage-four boss initializes the second actor movement direction");
  practice::Toggle(practice::LockTimeout);practice::Toggle(practice::EnemyInvincible);
  auto& menu=practice::MenuConfig();menu=practice::Config();menu.mode=1;menu.stage=5;menu.warp=2;menu.section=6;menu.rage=2;menu.trigger=1;g.m_initDifficulty=1;
  Check(practice::StartSetup(),"phase preset starts with F6 and U enabled");
  CStage* guarded=new CStage;guarded->Initialize();game.m_pCurScheme=guarded;game.m_curSchemeID=4;
  Check(guarded->m_pBoss->m_curPhase==10&&static_cast<CBossStage6*>(guarded->m_pBoss)->m_phase10Timer>65,"live timeout and enemy invincibility cannot block phase preparation");
  Check(practice::Enabled(practice::LockTimeout)&&practice::Enabled(practice::EnemyInvincible),"phase preparation preserves live hotkey states");
  g.m_replay.CloseStage(false);delete guarded;game.m_pCurScheme=NULL;ResetTools();
  for(int fixture=0;fixture<3;fixture++){
   menu=practice::Config();menu.mode=1;menu.stage=fixture==0?5:fixture==1?6:0;menu.warp=fixture==2?1:2;menu.section=fixture==0?6:fixture==1?9:1;menu.rage=fixture==0?2:0;menu.trigger=fixture==0?1:0;
   for(int frame=0;frame<3;frame++){practice::BeginFrame();practice::DrawSetup(30);practice::EndFrame();glFinish();}
   ImGuiWindow* panel=ImGui::FindWindowByName("TH05 Practice");
   Check(panel&&panel->Pos.y>=0&&panel->Pos.y+panel->Size.y<=480,"phase practice panel remains inside the game viewport");
   Check(panel->ScrollMax.y==0,"stage, rage, trigger and custom resource controls fit together without scrolling");
   Check(ImGui::GetFont()->FindGlyphNoFallback(0x72c2)&&ImGui::GetFont()->FindGlyphNoFallback(0x76fe),"rage and shield labels have real glyphs");
   char shot[64];sprintf_s(shot,"phase-practice-options-%d.bmp",fixture);Screenshot(shot);
  }
  fprintf(stderr,"PHASE CASES %d\n",cases);g.m_bReplayMode=g.m_bPracticeMode=false;g.m_initDifficulty=g.m_playDifficulty=1;
 }
 static void PracticeDialogue(CGame& game){
  ResetTools();auto& g=CGame::GVar();
  for(int stage=0;stage<7;stage++)for(int entry=0;entry<(stage==3?4:2);entry++){
   auto& menu=practice::MenuConfig();menu=practice::Config();menu.mode=1;menu.stage=stage;menu.warp=2;menu.section=entry==0?0:entry==1?1:entry==2?2:6;menu.dialogue=1;
   g.m_bReplayMode=false;g.m_initDifficulty=1;g.m_playChara=stage%4;
   Check(menu.HasDialogue()&&practice::StartSetup(),"native dialogue is offered only at original boss entrances");
   fprintf(stderr,"DIALOGUE stage=%d entry=%d\n",stage,entry);
   CStage* record=new CStage;record->Initialize();game.m_pCurScheme=record;game.m_curSchemeID=4;
   Check(record->m_pDialogue->IsRunning(),"dialogue-enabled practice leaves the actual original script running");
   Check(g.m_replay.m_keyState[stage].empty(),"visible dialogue begins before any recorded input");
   Frame(*record);char shot[80];sprintf_s(shot,"practice-dialogue-stage-%d-entry-%d.bmp",stage+1,entry);Screenshot(shot);
   const auto capture=[&](CStage* st){const double values[]={double(st->m_curFrame),double(st->m_pDialogue->IsRunning()),double(st->m_pBoss?st->m_pBoss->m_curPhase:-1),double(st->m_pBoss?st->m_pBoss->m_curPhaseFrame:-1),double(st->m_practiceDialoguePending),double(st->m_curScore),st->m_pChara->m_curX,st->m_pChara->m_curY};return std::vector<double>(values,values+8);};
   std::vector<double> states;int frames=0,combatFrames=0;
   for(;frames<10000;frames++){
    record->StepFrame(frames<120?0:128);const auto v=capture(record);states.insert(states.end(),v.begin(),v.end());
    if(frames==100){Frame(*record);char textShot[80];sprintf_s(textShot,"practice-dialogue-visible-%d-entry-%d.bmp",stage+1,entry);Screenshot(textShot);}
    if(!record->m_pDialogue->IsRunning()&&!record->m_practiceDialoguePending&&record->m_pBoss&&++combatFrames>=30){++frames;break;}
   }
   if(frames>=10000)fprintf(stderr,"DIALOGUE stuck running=%d pending=%d phase=%d local=%d\n",record->m_pDialogue->IsRunning(),record->m_practiceDialoguePending,record->m_pBoss?record->m_pBoss->m_curPhase:-1,record->m_pBoss?record->m_pBoss->m_curPhaseFrame:-1);
   Check(frames<10000,"original dialogue finishes with native Ctrl skip and enters the chosen practice start");
   if(entry){const auto* section=practice::SelectedSection(stage,2,menu.section);Check(record->m_pBoss->m_curPhase==section->phase,"dialogue completion initializes the selected first phase");}
   const int nextRandom=rand();record->SetQuit(STAGE_END_SAVE_EXIT);int end=0;for(int i=0;i<60&&!end;i++)end=record->StepFrame(0);
   Check(g.m_replay.SaveFile("dialogue-roundtrip.rpy"),"dialogue practice records in TPC5 v3");delete record;game.m_pCurScheme=NULL;
   Check(g.m_replay.LoadFile("dialogue-roundtrip.rpy")&&g.m_replay.m_practiceConfig.dialogue==1,"replay restores the dialogue flag");
   g.m_bReplayMode=true;g.m_playStage=stage;g.OnBeginGame();CStage* playback=new CStage;playback->Initialize();game.m_pCurScheme=playback;
   bool equal=true;size_t at=0;for(int i=0;i<frames;i++){playback->StepFrame(0);for(double value:capture(playback))if(value!=states[at++])equal=false;}
   Check(equal&&rand()==nextRandom,"dialogue recording and playback match frame states and RNG");
   g.m_replay.CloseStage(false);delete playback;game.m_pCurScheme=NULL;
  }
  g.m_bReplayMode=false;auto& dialogueMenu=practice::MenuConfig();dialogueMenu=practice::Config();dialogueMenu.mode=1;dialogueMenu.stage=3;dialogueMenu.warp=2;dialogueMenu.section=2;dialogueMenu.dialogue=1;
  for(int i=0;i<3;i++){practice::BeginFrame();practice::DrawSetup(30);practice::EndFrame();}
  ImGuiWindow* panel=ImGui::FindWindowByName("TH05 Practice");Check(panel&&panel->ScrollMax.y==0,"dialogue toggle fits alongside first-phase practice parameters");Screenshot("practice-dialogue-options.bmp");
  practice::Config invalid;invalid.mode=1;invalid.stage=5;invalid.warp=2;invalid.section=6;invalid.dialogue=1;invalid.Clamp(1);
  Check(!invalid.HasDialogue()&&!invalid.dialogue,"later boss phases never replay an unrelated opening dialogue");
  invalid.warp=1;invalid.section=1;invalid.dialogue=1;invalid.Clamp(1);Check(!invalid.dialogue,"midbosses without original dialogue expose no dialogue option");
  {FILE* fp=fopen("custom-roundtrip.rpy","rb");fseek(fp,0,SEEK_END);long size=ftell(fp);rewind(fp);std::vector<char> bytes(size);fread(bytes.data(),1,size,fp);fclose(fp);
   const int at=int(bytes.size())-sizeof(practice::Config)-12;unsigned int version=2,length=72;memcpy(bytes.data()+at+4,&version,4);memcpy(bytes.data()+at+8,&length,4);bytes.resize(bytes.size()-4);
   fp=fopen("custom-v2.rpy","wb");fwrite(bytes.data(),1,bytes.size(),fp);fclose(fp);
   Check(g.m_replay.LoadFile("custom-v2.rpy")&&!g.m_replay.m_practiceConfig.dialogue,"old TPC5 v2 replay retains dialogue-disabled default");}
  g.m_bReplayMode=g.m_bPracticeMode=false;g.m_initDifficulty=g.m_playDifficulty=1;
 }
 static void CheckTabCounts(CStage& stage,int misses,int bombs){
  remove("tab-counts.log");practice::BeginFrame();stage.Draw();ImGui::LogToFile(-1,"tab-counts.log");practice::DrawStatus();
  ImGui::LogFinish();practice::EndFrame();glFinish();char text[1024]={};FILE* log=fopen("tab-counts.log","rb");
  Check(log!=NULL,"Tab text capture opens the production ImGui log");fread(text,1,sizeof(text)-1,log);fclose(log);std::istringstream rendered(text);
  std::string token;int shownMiss=-1,shownBomb=-1;
  while(rendered>>token){if(token=="miss:")rendered>>shownMiss;else if(token=="bomb:")rendered>>shownBomb;}
  if(shownMiss!=misses||shownBomb!=bombs)fprintf(stderr,"TAB expected=%d/%d shown=%d/%d resident=%d/%d stage=%d/%d text=[%s]\n",misses,bombs,shownMiss,shownBomb,int(original::Stats().misses),int(original::Stats().bombs),stage.m_nMiss,stage.m_nBomb,rendered.str().c_str());
  Check(shownMiss==misses&&shownBomb==bombs,"Tab renders whole-run Miss and Bomb counts through the production ImGui path");
 }
 static void CountTabActions(CStage& stage,int misses,int bombs){
  auto& player=*stage.m_pChara;
  for(int i=0;i<misses;i++){player.m_nextLifeComeTimeLeft=33;player.m_cannotControlTimeLeft=1;player.m_bMiss=false;player.Step();}
  player.m_nextLifeComeTimeLeft=0;stage.m_nCurBomb=bombs;
  for(int i=0;i<bombs;i++){stage.m_bInCharaBomb=false;player.DropBomb();}
 }
 static void TabInfo(CGame& game){
  practice::Options()=practice::AdvancedOptions();ResetTools();CStage* st=Stage(game);
  st->m_pTextOverlay->SetMode(TEXT_OVERLAY_NONE);CountTabActions(*st,2,3);
  Check(!practice::InGameInfoOpen(),"Tab information is hidden by default");Key(game,VK_TAB);Check(practice::InGameInfoOpen(),"Tab opens the purple-style in-game information panel");
  CheckTabCounts(*st,2,3);ImGuiWindow* panel=ImGui::FindWindowByName("TH05 In-Game Info");
  Check(panel&&panel->Pos.x==432&&panel->Pos.y==408&&panel->Size.x==192&&panel->Size.y==48,"Tab panel uses fixed native sidebar coordinates");
  Check(panel->Pos.x>=st->m_playAreaUpperLeftX+384&&panel->Pos.y>=408&&panel->Pos.y+panel->Size.y<=460,"Tab panel avoids playfield, Power gauge and FPS");
  Check((panel->Flags&ImGuiWindowFlags_NoInputs)==ImGuiWindowFlags_NoInputs&&(panel->Flags&ImGuiWindowFlags_NoMove),"Tab panel retains purple's non-interactive non-draggable behavior");
  Key(game,VK_TAB,true);Check(practice::InGameInfoOpen()&&!ImGui::GetIO().KeysDown[VK_TAB],"holding Tab neither flickers the panel nor steals pause-menu focus");
  const int frame=st->m_curFrame;const size_t replay=CGame::GVar().m_replay.m_keyState[0].size();Frame(*st);Frame(*st);Check(st->m_curFrame==frame&&CGame::GVar().m_replay.m_keyState[0].size()==replay,"information drawing does not advance or record gameplay");
  practice::Options().showKeys=true;practice::RecordKey(5,32|64);Frame(*st);Screenshot("tab-info-and-keys.bmp");practice::Options().showKeys=false;
  CGame::GVar().m_bReplayMode=true;Frame(*st);Check(panel->LastFrameActive==ImGui::GetFrameCount(),"Tab information remains available during native replay playback");CGame::GVar().m_bReplayMode=false;
  practice::SetAdvancedOpen(true);Key(game,VK_TAB);Check(practice::InGameInfoOpen(),"Tab inside advanced settings does not toggle the gameplay panel");practice::SetAdvancedOpen(false);
  Key(game,VK_BACK);Frame(*st);Screenshot("tab-info-mod-menu.bmp");Key(game,VK_BACK);
  Key(game,VK_TAB);Check(!practice::InGameInfoOpen(),"Tab closes the information panel");Frame(*st);Screenshot("tab-info-closed.bmp");
  Key(game,VK_TAB);st->SetQuit(STAGE_END_CLEAR);int end=0;for(int i=0;i<60&&!end;i++)end=st->StepFrame(0);
  Check(end==STAGE_END_CLEAR,"Tab fixture completes native stage-clear bookkeeping");CScheme* next=NULL;int id=0;
  Check(CSchemeSwitcher::SwitchScheme(&next,&id,4,end)&&id==4&&CGame::GVar().m_playStage==1,"Tab fixture enters the next stage through the production clear route");
  delete st;st=static_cast<CStage*>(next);game.m_pCurScheme=st;st->m_pTextOverlay->SetMode(TEXT_OVERLAY_NONE);
  Check(st->m_nMiss==0&&st->m_nBomb==0,"per-stage counters still reset for native single-stage scoring");CheckTabCounts(*st,2,3);
  CountTabActions(*st,1,1);CheckTabCounts(*st,3,4);
  st->SetQuit(STAGE_END_RESTART);end=0;for(int i=0;i<60&&!end;i++)end=st->StepFrame(0);
  Check(end==STAGE_END_RESTART&&CSchemeSwitcher::SwitchScheme(&next,&id,4,end)&&id==4,"Tab fixture starts a fresh run through native restart");
  delete st;st=static_cast<CStage*>(next);game.m_pCurScheme=st;st->m_pTextOverlay->SetMode(TEXT_OVERLAY_NONE);CheckTabCounts(*st,0,0);Key(game,VK_TAB);
  CGame::GVar().m_replay.CloseStage(false);delete st;game.m_pCurScheme=NULL;practice::Options()=practice::AdvancedOptions();practice::SaveAdvancedOptions();
 }
 static void EosdBossBar(CGame& game){
  auto& opts=practice::Options();opts=practice::AdvancedOptions();ResetTools();
  opts.eosdBar=opts.hpPhaseMarkers=true;opts.phaseTimeout=false;practice::SaveAdvancedOptions();
  Check(opts.eosdBar&&!opts.hpPhaseMarkers&&opts.phaseTimeout,"EoSD bar excludes old markers and forces native timeout display");
  opts=practice::AdvancedOptions();practice::LoadAdvancedOptions();Check(opts.eosdBar&&!opts.hpPhaseMarkers&&opts.phaseTimeout,"bar preference invariants survive a fresh settings load");
  Check(!opts.phaseStars,"phase stars default off for existing configurations");
  opts.phaseStars=true;practice::SaveAdvancedOptions();opts=practice::AdvancedOptions();practice::LoadAdvancedOptions();Check(opts.eosdBar&&opts.phaseStars,"phase star choice persists with the EoSD bar");
  opts.eosdBar=false;practice::SaveAdvancedOptions();Check(!opts.phaseStars,"disabling the EoSD bar clears dependent stars");
  opts.eosdBar=true;opts.phaseStars=false;practice::SaveAdvancedOptions();
  auto& g=CGame::GVar();auto& menu=practice::MenuConfig();
  const int remaining[]={2,3,4,4,5,6,8};
  for(int number=0;number<7;number++){
   menu=practice::Config();menu.mode=1;menu.stage=number;menu.warp=2;g.m_bReplayMode=false;g.m_playChara=0;
   Check(practice::StartSetup(),"EoSD HUD fixture starts through the real practice boss warp");
   CStage* st=new CStage;st->Initialize();game.m_pCurScheme=st;game.m_curSchemeID=4;st->m_bLogo=false;
   st->m_pTextOverlay->SetMode(TEXT_OVERLAY_NONE);CBoss* boss=st->m_pBoss;
   Check(boss!=NULL,"EoSD HUD uses native boss instances");
   if(boss->m_curPhaseFrame==0)boss->Step();
   auto data=st->GetBossBarData();Check(data.present&&data.remaining==remaining[number]&&data.start>data.end,"all seven bosses expose correct health-phase counts without enrage subphases");
   for(int i=0;i<110;i++)st->TickBossBar();
   Frame(*st);char name[64];sprintf_s(name,"eosd-stage-%d.bmp",number+1);Screenshot(name);
   const int sx=st->m_playAreaUpperLeftX,sy=st->m_playAreaUpperLeftY+18;
   std::vector<unsigned char> beforeStars(80*8*3),afterStars(80*8*3);glReadPixels(sx,480-sy-8,80,8,GL_RGB,GL_UNSIGNED_BYTE,beforeStars.data());
   opts.phaseStars=true;Frame(*st);sprintf_s(name,"eosd-stars-stage-%d.bmp",number+1);Screenshot(name);
   glReadPixels(sx,480-sy-8,80,8,GL_RGB,GL_UNSIGNED_BYTE,afterStars.data());
   for(int slot=0;slot<10;slot++){
    bool different=false;for(int yy=0;yy<8;yy++)for(int xx=slot*8;xx<slot*8+8;xx++)for(int c=0;c<3;c++)if(beforeStars[(yy*80+xx)*3+c]!=afterStars[(yy*80+xx)*3+c])different=true;
    Check(different==(slot<data.remaining),"rendered stars below Enemy match future native health phases exactly");
   }
   if(number==6){st->m_bossBar.data.remaining=0;Frame(*st);Screenshot("eosd-stars-final-phase.bmp");st->m_bossBar.data.remaining=data.remaining;}
   opts.phaseStars=false;

   if(number==4){
    boss->m_curPhase=4;boss->m_curPhaseFrame=15;boss->m_curHP=7000;boss->m_curPhaseEndHP=5700;boss->Step();
    st->TickBossBar();const int key=st->m_bossBar.data.key,count=st->m_bossBar.data.remaining;
    Check(st->m_bossBar.data.enrage==6300&&st->m_bossBar.data.start==7500&&st->m_bossBar.data.end==5700,"Yumeko enrage is a cut within its native health segment");
    for(int i=0;i<100;i++)st->TickBossBar();Frame(*st);Screenshot("eosd-yumeko-normal.bmp");
    const float full=st->m_bossBar.displayed;boss->m_curHP=6100;st->TickBossBar();
    Check(fabs(st->m_bossBar.displayed-(full-.02f))<.0001f,"EoSD depletion follows TH06's two-percent animation step");
    for(int i=0;i<30;i++)st->TickBossBar();Frame(*st);Screenshot("eosd-yumeko-enrage.bmp");
    Check(st->m_bossBar.data.key==key&&st->m_bossBar.data.remaining==count,"crossing enrage does not refill the bar or consume a health phase");
    boss->m_curHP=7000;const float low=st->m_bossBar.displayed;st->TickBossBar();Check(fabs(st->m_bossBar.displayed-(low+.01f))<.0001f,"EoSD refill follows TH06's one-percent animation step");
    const float paused=st->m_bossBar.displayed;const int alpha=st->m_bossBar.opacity;
    st->m_bPaused=true;st->StepFrame(0);Frame(*st);Check(st->m_bossBar.displayed==paused&&st->m_bossBar.opacity==alpha,"pause freezes boss-bar interpolation");st->m_bPaused=false;
    practice::SetAdvancedOpen(true);st->StepFrame(0);Frame(*st);Screenshot("f12-eosd-options.bmp");Check(st->m_bossBar.displayed==paused&&opts.phaseTimeout,"F12 freezes interpolation and keeps the forced timeout option");practice::SetAdvancedOpen(false);
    const int frame=boss->m_curPhaseFrame,hp=boss->m_curHP;srand(101);const int next=rand();srand(101);
    for(int i=0;i<10;i++){st->GetBossBarData();st->TickBossBar();Frame(*st);}Check(rand()==next&&boss->m_curPhaseFrame==frame&&boss->m_curHP==hp,"presentation does not alter native HP, attack clocks or replay RNG");
    boss->m_curPhase=0xfd;st->TickBossBar();Check(st->m_bossBar.opacity==251,"boss exit begins TH06's four-step opacity fade");
    for(int i=0;i<64;i++)st->TickBossBar();Check(st->m_bossBar.opacity==0&&st->m_bossBar.displayed==0,"completed exit clears the old bar before the next encounter");
   }
   if(number==3){
    Check(data.start==9000&&data.remaining==4,"dual boss combines HP and reserves four subsequent branch phases");
    st->m_pBoss=NULL;delete boss;boss=new CBossStage4Mai;boss->m_pStage=st;boss->Initialize();boss->Step();st->m_pBoss=boss;
    data=st->GetBossBarData();Check(data.start==7800&&data.remaining==3,"Mai branch begins its own four-phase bar");
    st->m_pBoss=NULL;delete boss;boss=new CBossStage4Yuki;boss->m_pStage=st;boss->Initialize();boss->Step();st->m_pBoss=boss;
    data=st->GetBossBarData();Check(data.start==7900&&data.remaining==3,"Yuki branch begins its own four-phase bar");
   }
   g.m_replay.CloseStage(false);delete st;game.m_pCurScheme=NULL;
  }
  opts=practice::AdvancedOptions();practice::SaveAdvancedOptions();
 }
 static void RingBossBar(CGame& game){
  auto& opts=practice::Options();opts=practice::AdvancedOptions();ResetTools();
  Check(!opts.ringBar,"legacy settings retain native or EoSD bars by default");
  opts.ringBar=opts.eosdBar=opts.hpPhaseMarkers=opts.phaseStars=true;opts.phaseTimeout=false;practice::SaveAdvancedOptions();
  Check(opts.ringBar&&!opts.eosdBar&&!opts.hpPhaseMarkers&&!opts.phaseStars&&opts.phaseTimeout,"ring style excludes the two other bar styles and forces timer and native phase stars");
  opts=practice::AdvancedOptions();practice::LoadAdvancedOptions();Check(opts.ringBar&&!opts.eosdBar&&opts.phaseTimeout,"ring selection survives configuration reload");
  auto& g=CGame::GVar();auto& menu=practice::MenuConfig();
  for(int number=0;number<7;number++){
   menu=practice::Config();menu.mode=1;menu.stage=number;menu.warp=2;g.m_bReplayMode=false;g.m_playChara=0;
   Check(practice::StartSetup(),"ring fixture starts at an actual native boss");
   CStage* st=new CStage;st->Initialize();game.m_pCurScheme=st;game.m_curSchemeID=4;st->m_bLogo=false;st->m_pTextOverlay->SetMode(TEXT_OVERLAY_NONE);
   CBoss* boss=st->m_pBoss;if(!boss->m_curPhaseFrame)boss->Step();
   const auto bar=st->GetBossBarData();Check(bar.present,"ring uses the native active phase health segment");
   for(int i=0;i<40;i++)st->TickBossBar();
   const auto initial=st->m_bossRings[0];Check(initial.data.full==bar.start-bar.end&&initial.data.hp==bar.hp-bar.end,"each phase refills a ring with only that phase health");
   Check(fabs(initial.displayed-1.f)<.0001f,"TH15 ring fills with 0.025-per-frame interpolation");
   Check(st->m_bossRings[1].data.present==(number==3),"only the dual boss has a second ring");
   Frame(*st);char shot[64];sprintf_s(shot,"ring-stage-%d.bmp",number+1);Screenshot(shot);
   if(number==3){
    auto* dual=static_cast<CBossStage4*>(boss);Check(st->m_bossRings[0].data.hp==st->m_bossRings[1].data.hp&&st->m_bossRings[0].displayed==st->m_bossRings[1].displayed,"Mai and Yuki share the same combined HP ring state");
    Check(st->m_bossRings[0].data.x==boss->m_curX/16.f&&st->m_bossRings[1].data.x==dual->m_2curX/16.f,"each shared-health ring is centered on its own actor");
    dual->m_2curHP-=1000;st->TickBossBar();Check(st->m_bossRings[0].displayed==st->m_bossRings[1].displayed&&st->m_bossRings[0].displayed<initial.displayed,"damaging either dual boss reduces both rings synchronously");
    Frame(*st);Screenshot("ring-dual-damaged.bmp");
    st->m_pBoss=NULL;delete boss;boss=new CBossStage4Mai;boss->m_pStage=st;boss->Initialize();boss->Step();st->m_pBoss=boss;
    for(int i=0;i<40;i++)st->TickBossBar();Check(st->m_bossRings[0].data.present&&!st->m_bossRings[1].data.present&&st->GetBossBarData().remaining==3,"dual-to-Mai branch replaces both shared rings with one surviving actor and consumes one star");Frame(*st);Screenshot("ring-mai-branch.bmp");

   }else{
    boss->m_curHP-=int(initial.data.full*.25f);st->TickBossBar();const auto& damaged=st->m_bossRings[0];
    Check(fabs(damaged.displayed-float(damaged.data.hp)/damaged.data.full)<.0001f,"TH15 ring clamps immediately to damage without TH06 depletion smoothing");
   }
   const float oldX=st->m_bossRings[0].data.x;boss->m_curX+=16*12;st->TickBossBar();Check(st->m_bossRings[0].data.x==oldX+12,"ring follows the native moving boss center");
   st->m_pChara->m_curX=boss->m_curX;st->m_pChara->m_curY=boss->m_curY;st->TickBossBar();Check(st->m_bossRings[0].dimmed,"ring dims when player is within 80 pixels");
   st->m_pChara->m_curY=boss->m_curY+90*16;st->TickBossBar();Check(st->m_bossRings[0].dimmed,"ring proximity dimming retains TH15 hysteresis between 80 and 96 pixels");
   st->m_pChara->m_curY=boss->m_curY+100*16;st->TickBossBar();Check(!st->m_bossRings[0].dimmed,"ring opacity recovers outside 96 pixels");
   const auto paused=st->m_bossRings[0];st->m_bPaused=true;st->StepFrame(0);Check(st->m_bossRings[0].displayed==paused.displayed&&st->m_bossRings[0].data.x==paused.data.x,"pause freezes native ring animation");st->m_bPaused=false;
   if(number==4){
    boss->m_curPhase=4;boss->m_curPhaseFrame=15;boss->m_curHP=7000;boss->m_curPhaseEndHP=5700;boss->Step();for(int i=0;i<40;i++)st->TickBossBar();
    Check(st->m_bossRings[0].data.markerCount==1&&st->m_bossRings[0].data.markers[0]==600,"Yumeko ring places rage at its actual phase-relative HP threshold");Frame(*st);Screenshot("ring-yumeko-boundary.bmp");
    const int key=st->m_bossRings[0].data.key,count=st->GetBossBarData().remaining;boss->m_curHP=6100;st->TickBossBar();
    Check(st->m_bossRings[0].data.key==key&&st->GetBossBarData().remaining==count,"rage neither refills the ring nor consumes a phase star");Frame(*st);Screenshot("ring-yumeko-rage.bmp");
   }
   const int frame=boss->m_curPhaseFrame,hp=boss->m_curHP;srand(101);const int next=rand();srand(101);for(int i=0;i<10;i++){st->TickBossBar();Frame(*st);}
   Check(rand()==next&&boss->m_curPhaseFrame==frame&&boss->m_curHP==hp,"ring rendering does not alter native combat, timers or replay RNG");
   boss->m_curPhase=0xfd;st->TickBossBar();Check(!st->m_bossRings[0].data.present&&!st->m_bossRings[1].data.present,"native boss death retires all rings immediately");
   g.m_replay.CloseStage(false);delete st;game.m_pCurScheme=NULL;
  }
  menu=practice::Config();menu.mode=1;menu.stage=1;menu.warp=1;menu.section=2;menu.trigger=1;g.m_bReplayMode=false;g.m_initDifficulty=1;
  Check(practice::StartSetup(),"midboss ring starts through native timeout-entry preset");CStage* mid=new CStage;mid->Initialize();game.m_pCurScheme=mid;mid->StepFrame(0);
  for(int i=0;i<40;i++)mid->TickBossBar();Check(mid->m_bossRings[0].data.full==1400,"midboss timeout entry preserves the native 1400 HP maximum");
  mid->m_pMidBoss->m_curHP=1000;mid->TickBossBar();Check(mid->m_bossRings[0].data.full==1400&&fabs(mid->m_bossRings[0].displayed-1000.f/1400.f)<.0001f,"midboss ring denominator stays at entry HP while native health decreases");Frame(*mid);Screenshot("ring-midboss-timeout-entry.bmp");
  const auto frozen=mid->m_bossRings[0];practice::SetAdvancedOpen(true);mid->StepFrame(0);Check(mid->m_bossRings[0].displayed==frozen.displayed,"F12 freezes ring animation alongside combat");practice::SetAdvancedOpen(false);
  g.m_replay.CloseStage(false);delete mid;game.m_pCurScheme=NULL;
  opts=practice::AdvancedOptions();practice::SaveAdvancedOptions();
 }
 static void ShinkiRingVisibility(CGame& game){
  auto& opts=practice::Options();opts=practice::AdvancedOptions();opts.ringBar=true;ResetTools();
  auto& g=CGame::GVar();auto& menu=practice::MenuConfig();
  for(int section:{3,5}){
   menu=practice::Config();menu.mode=1;menu.stage=5;menu.warp=2;menu.section=section;g.m_bReplayMode=false;g.m_initDifficulty=1;
   Check(practice::StartSetup(),"wing visibility starts via Shinki native phase initialization");CStage* st=new CStage;st->Initialize();game.m_pCurScheme=st;game.m_curSchemeID=4;st->StepFrame(0);
   Check(st->m_pBoss->m_curImage==(section==3?184:192),"fixture renders the actual original white or black wing sprite");
   for(int i=0;i<50;i++)st->TickBossBar();Check(st->m_bossRings[0].displayed==1&&!st->m_bossRings[0].dimmed,"wing contrast fixture has a full undimmed ring");
   Frame(*st);std::vector<unsigned char> composed(640*480*3),foreground(composed.size());glReadPixels(0,0,640,480,GL_RGB,GL_UNSIGNED_BYTE,composed.data());
   st->DrawBossRings();glFinish();glReadPixels(0,0,640,480,GL_RGB,GL_UNSIGNED_BYTE,foreground.data());
   Check(composed==foreground,"native wings cannot overpaint the foreground health arc or its dark contrast shoulders");
   char shot[64];sprintf_s(shot,"ring-shinki-%s-wings.bmp",section==3?"white":"black");Screenshot(shot);
   const int cx=st->m_playAreaUpperLeftX+int(st->m_bossRings[0].data.x),cy=480-1-st->m_playAreaUpperLeftY-int(st->m_bossRings[0].data.y);
   const size_t light=((cy+56)*640+cx)*3,dark=((cy+58)*640+cx)*3;
   Check(composed[light]>240&&composed[dark]<30,"white arc retains a dark edge even on a white wing");
   g.m_replay.CloseStage(false);delete st;game.m_pCurScheme=NULL;
  }
  opts=practice::AdvancedOptions();
 }
 static void CycleTimeProjection(CGame& game){
  auto& g=CGame::GVar();ResetTools();practice::Options()=practice::AdvancedOptions();
  Check(!practice::Options().phaseTimeoutAsTime,"existing settings default to native round counts");
  const auto exhausted=practice::PhaseTimeout::Timeline(4,5,1,200,[](int){return 96;},[](int){return 32;});
  Check(exhausted.value==0&&exhausted.ForDisplay(false).value==0,"locked exhausted loops show zero in seconds and rounds");
  const auto timed=practice::PhaseTimeout::Frames(112,0);
  Check(timed.ForDisplay(false).unit==practice::PhaseTimeout::Seconds&&timed.ForDisplay(true).value==2,"frame-based phases stay seconds in either display mode");

  for(int stage=0;stage<7;stage++){
   auto& menu=practice::MenuConfig();menu=practice::Config();menu.mode=1;menu.stage=stage;menu.warp=2;
   g.m_bReplayMode=false;g.m_initDifficulty=1;g.m_playChara=0;
   Check(practice::StartSetup(),"cycle timer fixture starts a real native boss");
   CStage* st=new CStage;st->Initialize();game.m_pCurScheme=st;game.m_curSchemeID=4;
   CBoss* boss=st->m_pBoss;Check(boss!=NULL,"boss timer fixture uses the production boss instance");
   std::vector<int> seconds;int phase=boss->m_curPhase;
   for(int tick=0;tick<150000&&boss->m_curPhase<0xfd;tick++){
    const auto timer=boss->GetPhaseTimeout();
    if(timer.cycles>=0&&!timer.estimated)seconds.push_back(timer.value);
    boss->Step();
    if(boss->m_curPhase!=phase){
     if(!seconds.empty()){
      bool equal=true;for(size_t i=0;i<seconds.size();i++)if(seconds[i]!=int((seconds.size()-i+55)/56)){equal=false;fprintf(stderr,"TIMER stage=%d phase=%d frame=%d got=%d expected=%d total=%d\n",stage+1,phase,int(i),seconds[i],int((seconds.size()-i+55)/56),int(seconds.size()));break;}
      Check(equal,"converted loop seconds match actual native frames until timeout");
     }
     seconds.clear();phase=boss->m_curPhase;
    }
    // Stage 3's pre-existing final attack does not finish on timeout alone.
    if(stage==2&&phase==14&&boss->m_curPhaseFrame>1100)break;
   }
   Check(phase>=0xfd||(stage==2&&phase==14),"native timeout fixture reaches its real terminal phase");
   g.m_replay.CloseStage(false);delete st;game.m_pCurScheme=NULL;
  }
  // Also run both four-stage branches and each loop-driven midboss directly.
  for(int fixture=0;fixture<6;fixture++){
   const int stages[]={3,3,2,3,4,6};
   auto& menu=practice::MenuConfig();menu=practice::Config();menu.mode=1;menu.stage=stages[fixture];menu.warp=0;
   g.m_bReplayMode=false;Check(practice::StartSetup(),"branch and midboss timer fixture starts");
   CStage* st=new CStage;st->Initialize();game.m_pCurScheme=st;game.m_curSchemeID=4;
   CBoss* boss=NULL;CMidBoss* mid=NULL;
   if(fixture<2){boss=fixture==0?static_cast<CBoss*>(new CBossStage4Mai):static_cast<CBoss*>(new CBossStage4Yuki);boss->m_pStage=st;boss->Initialize();st->m_pBoss=boss;}
   else {switch(fixture){case 2:mid=new CMidBossStage3;break;case 3:mid=new CMidBossStage4;break;case 4:mid=new CMidBossStage5;break;case 5:mid=new CMidBossStageEx;break;}mid->m_pStage=st;mid->Initialize();st->m_pMidBoss=mid;}
   int phase=boss?boss->m_curPhase:mid->m_curPhase;std::vector<int> seconds;
   for(int tick=0;tick<50000&&phase<0xfd;tick++){
    const auto timer=boss?boss->GetPhaseTimeout():mid->GetPhaseTimeout();
    if(timer.cycles>=0&&!timer.estimated)seconds.push_back(timer.value);
    if(boss)boss->Step();else mid->Step();
    const int next=boss?boss->m_curPhase:mid->m_curPhase;
    if(next!=phase){
     if(!seconds.empty()){
      bool equal=true;for(size_t i=0;i<seconds.size();i++)if(seconds[i]!=int((seconds.size()-i+55)/56)){equal=false;fprintf(stderr,"MID/BRANCH fixture=%d phase=%d frame=%d got=%d expected=%d total=%d\n",fixture,phase,int(i),seconds[i],int((seconds.size()-i+55)/56),int(seconds.size()));break;}
      Check(equal,"branch and midboss converted seconds match native timeout frames");
     }
     seconds.clear();phase=next;
    }
   }
   Check(phase>=0xfd,"branch and midboss projection fixture reaches native timeout end");
   g.m_replay.CloseStage(false);delete st;game.m_pCurScheme=NULL;
  }
  practice::Options().phaseTimeoutAsTime=true;practice::SaveAdvancedOptions();practice::Options().phaseTimeoutAsTime=false;practice::LoadAdvancedOptions();
  Check(practice::Options().phaseTimeoutAsTime,"seconds display preference survives reload");
  practice::Options().phaseTimeoutAsTime=false;practice::SaveAdvancedOptions();practice::LoadAdvancedOptions();
  Check(!practice::Options().phaseTimeoutAsTime,"rounds display preference survives reload");
  g.m_bPracticeMode=false;g.m_bReplayMode=false;g.m_initDifficulty=g.m_playDifficulty=1;
 }
 static void LockTimeoutRestart(CGame& game){
  ResetTools();practice::Options()=practice::AdvancedOptions();practice::Options().fastRetry=true;auto& g=CGame::GVar();auto& menu=practice::MenuConfig();
  menu=practice::Config();menu.mode=1;menu.stage=0;menu.warp=2;menu.section=3;g.m_initDifficulty=1;
  Check(practice::StartSetup(),"timer restart fixture starts native final boss attack");CStage* stage=new CStage;stage->Initialize();game.m_pCurScheme=stage;game.m_curSchemeID=4;
  Key(game,VK_F6);const int frozen=stage->GetPhaseTimeout().value;auto* boss=stage->m_pBoss;
  boss->m_curPhaseFrame+=112;
  Check(stage->GetPhaseTimeout().value==frozen,"F6 freezes remaining-time display while native attack time advances");
  for(int retry=0;retry<3;retry++){
   int exit=0;
   if(retry==1){stage->RequestFastRetry();for(int i=0;i<20&&!exit;i++)exit=stage->StepFrame(0);}
   else{stage->PracticeExit(3);for(int i=0;i<60&&!exit;i++)exit=stage->StepFrame(0);}
   Check(exit==STAGE_END_RESTART,"pause restart and quick retry reach the real restart exit");CScheme* next=NULL;int id=0;
   Check(CSchemeSwitcher::SwitchScheme(&next,&id,4,exit)&&id==4,"locked retry constructs a new native stage");delete stage;stage=static_cast<CStage*>(next);game.m_pCurScheme=stage;game.m_curSchemeID=4;boss=stage->m_pBoss;
   Check(practice::Enabled(practice::LockTimeout)&&boss->m_curPhase==4,"F6 remains active after repeated restart initialization");
   const int reset=stage->GetPhaseTimeout().value;Check(reset==practice::PhaseTimeout::Frames(1300,boss->m_curPhaseFrame).value,"new run locks its own fresh phase countdown");
   boss->m_curPhaseFrame+=112;Check(stage->GetPhaseTimeout().value==reset,"restarted run keeps visible countdown locked");
   stage->m_pChara->m_invulnerableTimeLeft=10000;boss->m_curPhaseFrame=1299;boss->Step();Check(boss->m_curPhase==4&&boss->m_curPhaseFrame==1300,"restart retains timeout prevention without freezing native attack clock");
   Frame(*stage);Check(stage->GetPhaseTimeout().value==reset,"rendering preserves the locked value after a native deadline");
  }
  Key(game,VK_F6);Check(stage->GetPhaseTimeout().value==0,"unlock immediately reveals current native remaining time");
  boss->m_curPhaseFrame=100;Key(game,VK_F6);Check(stage->GetPhaseTimeout().value==22,"re-enabling F6 captures a new remaining-time value");
  boss->m_curPhase=1;boss->m_curPhaseFrame=31;boss->Step();
  const auto turns=stage->GetPhaseTimeout();Check(turns.unit==practice::PhaseTimeout::Turns&&turns.value==32,"a new phase captures its own turn count");
  practice::Options().phaseTimeoutAsTime=true;const int seconds=stage->GetPhaseTimeout().value;boss->m_nModeChange=4;boss->m_curPhaseFrame=50;
  Check(stage->GetPhaseTimeout().value==seconds,"time view uses the same locked cycle snapshot");practice::Options().phaseTimeoutAsTime=false;Check(stage->GetPhaseTimeout().value==32,"switching back to turns preserves the locked snapshot");
  g.m_bReplayMode=true;Check(stage->GetPhaseTimeout().value==28,"replay isolates the live F6 display lock");g.m_bReplayMode=false;
  Key(game,VK_F6);boss->m_curPhase=4;boss->m_curPhaseFrame=1299;Key(game,VK_F6);stage->GetPhaseTimeout();boss->m_curHP=1;
  for(int i=0;i<10;i++)stage->m_pCharaBullet->AddBullet(boss->m_curX,boss->m_curY,3,0,0,0,0,255);
  boss->Step();Check(boss->m_curPhase==0xfd,"locked restarted phase remains defeatable after its native deadline");
  Check(stage->GetPhaseTimeout().unit==practice::PhaseTimeout::None,"defeated phase clears its locked countdown");
  ResetTools();g.m_replay.CloseStage(false);delete stage;game.m_pCurScheme=NULL;practice::Options()=practice::AdvancedOptions();
 }
 static void PhaseTimeoutDisplay(CGame& game){
  ResetTools();practice::Options()=practice::AdvancedOptions();CStage* stage=Stage(game);
  stage->m_pTextOverlay->SetMode(TEXT_OVERLAY_NONE);
  Check(stage->GetPhaseTimeout().unit==practice::PhaseTimeout::None,"no timer is shown outside a timed boss phase");
  CBossStage1* concrete=new CBossStage1;CBoss* boss=concrete;boss->m_pStage=stage;boss->Initialize();stage->m_pBoss=boss;boss->Step();
  boss->m_curPhase=1;boss->m_curPhaseFrame=31;boss->Step();
  practice::Options().hpPhaseMarkers=true;boss->m_lastDrawnHP=4650;Frame(*stage);Screenshot("f12-hp-phase-markers.bmp");practice::Options().hpPhaseMarkers=false;
  practice::Options().phaseTimeout=true;
  Check(stage->GetPhaseTimeout().unit==practice::PhaseTimeout::Turns&&stage->GetPhaseTimeout().value==32,"native loop phase starts with its remaining timeout cycles");
  const int mode=boss->m_curMode,count=boss->m_nModeChange,localFrame=boss->m_curPhaseFrame;
  practice::Options().phaseTimeoutAsTime=true;
  Check(stage->GetPhaseTimeout().unit==practice::PhaseTimeout::Seconds&&stage->GetPhaseTimeout().value>32,"time mode converts the native attack and movement schedule instead of treating turns as seconds");
  srand(231);const int expectedCycleRandom=rand();srand(231);
  for(int i=0;i<100;i++)stage->GetPhaseTimeout();
  Check(rand()==expectedCycleRandom,"cycle-to-seconds projection does not consume gameplay RNG");
  Frame(*stage);Screenshot("f12-phase-cycle-seconds.bmp");
  Check(boss->m_curMode==mode&&boss->m_nModeChange==count&&boss->m_curPhaseFrame==localFrame,"switching display does not advance gameplay");
  practice::Options().phaseTimeoutAsTime=false;

  boss->m_nModeChange=31;boss->m_curMode=0;boss->m_curPhaseFrame=91;
  Check(stage->GetPhaseTimeout().value==1,"last native attack loop displays one remaining turn");Frame(*stage);Screenshot("f12-phase-turns.bmp");
  boss->Step();Check(boss->m_curPhase==3&&stage->GetPhaseTimeout().value==24,"native loop timeout advances phase and the displayed count resets");
  boss->m_curPhase=4;boss->m_curPhaseFrame=1244;boss->m_curHP=450;boss->m_curPhaseEndHP=0;
  Check(stage->GetPhaseTimeout().unit==practice::PhaseTimeout::Seconds&&stage->GetPhaseTimeout().value==1,"last 56 native frames display one second");
  boss->m_curPhaseFrame=1243;Check(stage->GetPhaseTimeout().value==2,"remaining seconds round upward at the native 56-fps boundary");
  boss->m_curPhaseFrame=1299;Frame(*stage);Screenshot("f12-phase-seconds.bmp");
  srand(55);const int expected=rand();srand(55);for(int i=0;i<100;i++)stage->GetPhaseTimeout();Check(rand()==expected&&boss->m_curPhaseFrame==1299,"timer reads do not mutate the native phase or consume replay RNG");
  practice::Toggle(practice::LockTimeout);stage->GetPhaseTimeout();boss->Step();Check(boss->m_curPhase==4&&stage->GetPhaseTimeout().value==1,"F6 freezes the displayed countdown without advancing phase");
  Frame(*stage);Screenshot("f12-phase-locked.bmp");ResetTools();boss->Step();Check(boss->m_curPhase==0xfd&&stage->GetPhaseTimeout().unit==practice::PhaseTimeout::None,"releasing lock permits the real timeout and hides the finished phase timer");
  stage->m_pBoss=NULL;delete boss;
  CMidBossStage1* mid=new CMidBossStage1;mid->m_pStage=stage;mid->Initialize();stage->m_pMidBoss=mid;mid->m_curPhase=3;mid->m_curPhaseFrame=1499;mid->m_curHP=1000;
  Check(stage->GetPhaseTimeout().unit==practice::PhaseTimeout::Seconds&&stage->GetPhaseTimeout().value==1,"native midboss timeout is exposed by the same HUD");
  mid->Step();Check(mid->m_curPhase==0xfe&&stage->GetPhaseTimeout().unit==practice::PhaseTimeout::None,"midboss timeout hides at its actual death transition");
  stage->m_pMidBoss=NULL;delete mid;CGame::GVar().m_replay.CloseStage(false);delete stage;game.m_pCurScheme=NULL;
  practice::Options()=practice::AdvancedOptions();
 }
 static void AdvancedMenu(CGame& game){
  auto& opts=practice::Options();auto& g=CGame::GVar();opts=practice::AdvancedOptions();ResetTools();
  CStage* stage=Stage(game);auto& player=*stage->m_pChara;
  const int before=stage->m_curFrame;const size_t keys=g.m_replay.m_keyState[0].size();
  Key(game,VK_F12);Check(practice::AdvancedOpen(),"F12 opens advanced options");
  stage->StepFrame(32|8);Check(stage->m_curFrame==before&&g.m_replay.m_keyState[0].size()==keys,"F12 freezes gameplay without recording menu input");
  Frame(*stage);Screenshot("f12-gameplay.bmp");
  ImGuiWindow* window=ImGui::FindWindowByName("TH05 Advanced Options");
  Check(window&&window->Size.x==640&&window->Size.y==480&&(window->Flags&ImGuiWindowFlags_NoTitleBar),"advanced layout follows purple fullscreen borderless options");
  Key(game,VK_F12,true);Check(practice::AdvancedOpen(),"held F12 does not flicker the menu");
  stage->m_bPaused=true;practice::FocusPause();Key(game,'Z');Frame(*stage);Check(stage->m_bPaused,"F12 prevents hidden pause controls from resuming the game");stage->m_bPaused=false;
  const char* glyphs=practice::AdvancedGlyphs();int size=MultiByteToWideChar(CP_UTF8,0,glyphs,-1,NULL,0);std::vector<wchar_t> unicode(size);MultiByteToWideChar(CP_UTF8,0,glyphs,-1,unicode.data(),size);
  bool complete=true;for(auto ch:unicode)if(ch>=0x3000&&!ImGui::GetFont()->FindGlyphNoFallback(ch))complete=false;
  Check(complete,"all advanced labels have real glyphs instead of question marks");
  ImGuiWindow* child=window->DC.ChildWindows[0];
  const auto group=[&](const char* label,bool open){child->StateStorage.SetInt(child->GetID(label),open?1:0);};
  group(u8"\u6e38\u620f\u8fdb\u884c",false);group(u8"\u6e38\u620f\u901f\u5ea6",true);Frame(*stage);Screenshot("f12-speed.bmp");
  group(u8"\u6e38\u620f\u901f\u5ea6",false);group(u8"\u53cd\u5e94\u529b\u6d4b\u8bd5",true);
  practice::Reaction().Reset();srand(77);const int expectedRandom=rand();srand(77);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,'Z',0);Frame(*stage);practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,'Z',1L<<31);
  Check(rand()==expectedRandom,"reaction test's private random generator does not consume gameplay RNG");
  practice::Reaction().Reset();practice::Reaction().Start(-10000,1,true);Frame(*stage);Screenshot("f12-reaction.bmp");
  group(u8"\u53cd\u5e94\u529b\u6d4b\u8bd5",false);group(u8"\u5173\u4e8ethprac",true);Frame(*stage);Screenshot("f12-about.bmp");
  Key(game,VK_F12);Check(!practice::AdvancedOpen(),"F12 closes options");
  opts.disableX=opts.disableShift=opts.disableZ=true;
  Check(practice::FilterGameplayInput(16|32|64|8,false)==8,"disabled X, Z and Shift are removed from game input");
  Check(practice::FilterGameplayInput(16|32|64|8,true)==(16|32|64|8),"recorded replay input bypasses current key-disable settings");
  opts.forceShift=true;Check(practice::FilterGameplayInput(8,false)==(8|64),"force low speed overrides disabled Shift like purple thprac");opts.forceShift=false;
  stage->StepFrame(16|32|64|8);Check(stage->m_curGameKeyState==8&&g.m_replay.m_keyState[0].back()==8,"live key filtering is saved in the actual native replay input stream");
  opts=practice::AdvancedOptions();opts.fps=112;stage->StepFrame(0);Check(game.m_fpsLimit==112,"configured game speed reaches the production frame limiter");opts.fps=56;
  Key(game,VK_F2);opts.noContinue=true;stage->m_nCurLife=3;player.m_nextLifeComeTimeLeft=1;player.m_cannotControlTimeLeft=1;player.Step();
  Check(stage->m_nCurLife==2,"F2 no-continue mapping consumes spare lives normally");
  stage->m_nCurLife=1;player.m_nextLifeComeTimeLeft=1;player.m_cannotControlTimeLeft=1;player.Step();Check(stage->m_nCurLife==1&&!stage->m_bContinueScr,"F2 no-continue retains the last life without Game Over");
  opts.noContinue=false;stage->m_nCurLife=3;player.m_nextLifeComeTimeLeft=1;player.m_cannotControlTimeLeft=1;player.Step();Check(stage->m_nCurLife==3,"unmapped F2 still locks all lives");ResetTools();
  opts.numericDream=opts.numericPower=true;Frame(*stage);Screenshot("f12-numeric-hud.bmp");opts.numericDream=opts.numericPower=false;
  player.m_curX=3072;player.m_curY=5120;player.m_nextLifeComeTimeLeft=0;player.m_invulnerableTimeLeft=0;player.m_collisionAreaAlpha=0;
  stage->m_pTextOverlay->SetMode(TEXT_OVERLAY_NONE);player.EnableDraw();
  std::vector<unsigned char> hidden(20*20*3),visible(20*20*3);const int x=stage->m_playAreaUpperLeftX+192-10,y=480-(stage->m_playAreaUpperLeftY+320)-10;
  Frame(*stage);glPixelStorei(GL_PACK_ALIGNMENT,1);glReadPixels(x,y,20,20,GL_RGB,GL_UNSIGNED_BYTE,hidden.data());
  opts.alwaysHitbox=true;Frame(*stage);glReadPixels(x,y,20,20,GL_RGB,GL_UNSIGNED_BYTE,visible.data());Check(hidden!=visible,"always-show hitbox adds pixels while unfocused");
  opts.showKeys=true;practice::RecordKey(5,stage->GetCurKeyState());Frame(*stage);Screenshot("f12-key-monitor-hitbox.bmp");Check(ImGui::FindWindowByName(u8"\u6309\u952e\u663e\u793a###TH05 Key Monitor")==NULL,"purple key HUD renders directly without a custom monitor window");practice::RecordKey(5,1|32|64);Frame(*stage);Screenshot("f12-purple-keys-pressed.bmp");
  opts.showKeys=opts.alwaysHitbox=false;
  player.m_collisionAreaAlpha=100;stage->m_curGameKeyState=64;Frame(*stage);glReadPixels(x,y,20,20,GL_RGB,GL_UNSIGNED_BYTE,visible.data());Check(hidden==visible,"default PC98 display hides hitbox even at low speed");
  opts.showHitbox=true;Frame(*stage);glReadPixels(x,y,20,20,GL_RGB,GL_UNSIGNED_BYTE,visible.data());Check(hidden!=visible,"low-speed option shows hitbox during Shift");
  stage->m_curGameKeyState=0;Frame(*stage);glReadPixels(x,y,20,20,GL_RGB,GL_UNSIGNED_BYTE,visible.data());Check(hidden==visible,"low-speed option hides hitbox immediately after Shift release");
  practice::ResetRun();Check(practice::Assisted(),"hitbox cheat marks the run assisted before stage entry");opts.showHitbox=false;practice::ResetRun();
  for(int i=0;i<56;i++)practice::RecordKey(5,0);
  practice::ClearKeyRecord();practice::g_record_key_aps=true;opts.showKeys=true;
  stage->StepFrame(1|32|64);Check(practice::GetKeyAPS().size()==1&&practice::GetKeyAPS().back()==3,"purple APS counts native key press edges once per simulation update");
  Frame(*stage);Frame(*stage);stage->m_bPaused=true;stage->StepFrame(0);Check(practice::GetKeyAPS().size()==1,"HUD rendering and pause do not duplicate recorded APS frames");stage->m_bPaused=false;
  stage->StepFrame(0);Check(practice::GetKeyAPS().size()==2&&practice::GetKeyAPS().back()==6,"purple APS counts release edges as well as press edges");
  practice::SetAdvancedOpen(true);group(u8"\u5173\u4e8ethprac",false);group(u8"\u6e38\u620f\u8fdb\u884c",true);Frame(*stage);Screenshot("f12-purple-aps-options.bmp");practice::SetAdvancedOpen(false);
  Check(practice::SaveKeyRecorded(),"purple APS recording exports to the sandbox game folder");
  FILE* csv=fopen("APS.csv","rb");char buffer[512]={};const size_t csvSize=csv?fread(buffer,1,sizeof(buffer)-1,csv):0;if(csv)fclose(csv);
  Check(csvSize>0&&strstr(buffer,"frame,aps,up,down,left,right,Z,X,C,D,Ctrl,Shift")&&strstr(buffer,"1,3,O,-,-,-,O,-,-,-,-,O"),"APS CSV preserves purple column order and native TH05 key mapping");
  practice::g_record_key_aps=false;practice::ClearKeyRecord();opts.showKeys=false;
  CBossStage1* boss=new CBossStage1;boss->m_pStage=stage;boss->Initialize();boss->m_curPhase=4;stage->m_pBoss=boss;
  stage->TickLockTimer();stage->TickLockTimer();Check(stage->LockTimerFrames()==2,"phase timer counts native simulation updates");boss->m_curPhase=5;stage->TickLockTimer();Check(stage->LockTimerFrames()==1,"phase transition resets lock timing");
  stage->m_bPaused=true;const int timer=stage->LockTimerFrames();stage->StepFrame(0);Check(stage->LockTimerFrames()==timer,"paused game does not advance lock timing");stage->m_bPaused=false;
  stage->m_pBoss=NULL;delete boss;stage->TickLockTimer();Check(stage->LockTimerFrames()==0,"leaving boss battle clears lock timing");
  // Show the timer on a real resource-backed boss, then test automatic retry.
  g.m_replay.CloseStage(false);delete stage;game.m_pCurScheme=NULL;
  auto& menu=practice::MenuConfig();menu=practice::Config();menu.mode=1;menu.warp=2;menu.lives=3;
  Check(practice::StartSetup(),"advanced runtime test starts a real practice boss");stage=new CStage;stage->Initialize();game.m_pCurScheme=stage;
  opts.lockTimer=true;Key(game,VK_F6);stage->StepFrame(0);Frame(*stage);Screenshot("f12-lock-timer.bmp");ResetTools();opts.lockTimer=false;
  opts.fastRetry=true;stage->m_pChara->m_invulnerableTimeLeft=0;stage->m_pChara->m_bMiss=true;stage->m_pChara->Step();
  Check(stage->m_fastRetryCountdown==15&&stage->m_bPaused,"practice Miss opens the original fast-retry countdown");
  int end=0;for(int i=0;i<20&&!end;i++)end=stage->StepFrame(0);Check(end==STAGE_END_RESTART,"fast retry exits through the real restart route");
  CScheme* fresh=NULL;int id=0;Check(CSchemeSwitcher::SwitchScheme(&fresh,&id,4,end)&&id==4,"fast retry creates a fresh practice run");delete stage;stage=static_cast<CStage*>(fresh);game.m_pCurScheme=stage;
  Check(!stage->m_bLogo&&stage->m_nCurLife==4&&stage->m_pBoss!=NULL,"fast retry preserves practice initial values and skips the loading logo");
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,VK_SHIFT,0);Key(game,'F');practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,VK_SHIFT,1L<<31);stage->StepFrame(0);
  Check(stage->m_fastRetryCountdown==14,"Shift+F triggers the same practice retry path");
  g.m_replay.CloseStage(false);delete stage;game.m_pCurScheme=NULL;practice::TakeFastRetryRequest();opts=practice::AdvancedOptions();
  practice::ReactionTest test;test.Start(1000,1200,true);test.Tick(2000,true);Check(test.state==practice::ReactionTest::TooEarly&&test.results.empty(),"early press does not fabricate a reaction result");
  test.Start(1000,1200,true);test.Tick(2200,false);test.Tick(2250,true);Check(test.results.size()==1&&test.results[0]==50,"reaction latency is measured from the actual color-change deadline");
  test.Reset();test.type=1;test.Start(0,1500,true);test.Tick(100,true);test.Tick(1600,true);test.Tick(1640,false);Check(test.results.size()==1&&test.results[0]==40,"release reaction waits for hold then measures release latency");
  test.Reset();test.progress=true;test.Start(1000,1200,true);test.Tick(2150,true);Check(test.results.size()==1&&test.results[0]==-50&&test.frameCounts[0]==3,"progress mode preserves negative early-reaction timing");
  opts.fps=84;opts.noContinue=opts.disableX=opts.showKeys=opts.showHitbox=opts.alwaysHitbox=opts.numericDream=opts.numericPower=opts.phaseTimeout=opts.hpPhaseMarkers=true;practice::SaveAdvancedOptions();opts=practice::AdvancedOptions();practice::LoadAdvancedOptions();
  Check(opts.fps==84&&opts.noContinue&&opts.disableX&&opts.showKeys&&opts.showHitbox&&opts.alwaysHitbox&&opts.numericDream&&opts.numericPower&&opts.phaseTimeout&&opts.hpPhaseMarkers,"advanced preferences round-trip independently of game and replay saves");
  opts=practice::AdvancedOptions();practice::SaveAdvancedOptions();practice::SetAdvancedOpen(false);
 }
 static void MusicRoom(CGame& game){
  Check(CMusicRoom::Available(),"original Music Room resource package is available");
  CMusicRoom room;Check(room.Initialize(),"original MUSIC.PI, translated font, comments and archive load");
  Check(room.m_game==4&&room.m_selection==0&&room.m_playing==0,"Music Room opens at TH05 first track");
  for(int i=0;i<15;i++)room.Step();room.Draw();glFinish();Screenshot("music-room-th05.bmp");
  const int expected[]={14,18,24,28,23};int total=0;
  for(int g=0;g<5;g++){
   Check(musicTrackCounts[g]==expected[g],"original per-game track counts");room.m_game=g;
   for(int track=0;track<musicTrackCounts[g];track++){
    Check(!room.Line(g,track,0).empty(),"original fixed-record comment has a title");
    Check(room.Play(track),"actual PMDWin accepts and starts the original song");
    Check(room.m_playing==track&&!room.m_playFailed,"selected song is the playing song");++total;
   }
   room.m_selection=musicTrackCounts[g];room.m_top=musicTrackCounts[g]-11;room.m_fade=100;room.Draw();glFinish();char shot[64];sprintf_s(shot,"music-room-return-%d.bmp",g+1);Screenshot(shot);
  }
  Check(total==107,"all 107 songs use the original order and play successfully");
  room.m_game=4;room.m_selection=0;room.m_top=0;room.Play(0);room.Input(0);room.Input(1);
  Check(room.m_selection==23&&room.m_top==12,"Up wraps to Quit and scrolls to last twelve rows");room.Input(0);room.Input(2);
  Check(room.m_selection==0&&room.m_top==0,"Down from Quit wraps to the first track");
  for(int i=0;i<13;i++){room.Input(0);room.Input(2);}Check(room.m_selection==13&&room.m_top==2,"track list scrolls past twelve rows");
  room.Input(0);room.Input(8);Check(room.m_game==0&&room.m_selection==0&&room.m_playing==0,"Right wraps TH05 to TH01 and starts its title theme");
  room.Input(0);room.Input(4);Check(room.m_game==4&&room.m_top==0,"Left wraps TH01 to TH05");
  room.m_selection=22;room.Input(0);room.Input(32);Check(room.m_playing==22,"Z starts the selected track");
  room.m_fade=100;room.m_commentAge=30;room.Draw();glFinish();Screenshot("music-room-comment.bmp");
  auto notes=CPMDPlayer::PianoNotes();for(auto note:notes)Check(note==255||(note>=0&&(note&15)<12&&(note>>4)<8),"piano notes come from valid live PMD work data");
  room.Input(0);room.Input(4096);Check(room.m_quit,"Esc exits Music Room");int ended=0;for(int i=0;i<20&&!ended;i++)ended=room.Step();Check(ended==1,"Music Room fades out before returning");
  CScheme* next=NULL;int id=0;Check(CSchemeSwitcher::SwitchScheme(&next,&id,8,1)&&id==2,"Music Room exit returns to native title");
  CTitleScreen* title=static_cast<CTitleScreen*>(next);title->m_bMenuOn=true;title->m_mainMenuCursorPosition=5;title->m_curScrFade=100;title->m_bgFade=100;title->m_flagRollingTama=0;title->Draw();glFinish();Screenshot("music-room-title-hint.bmp");delete next;
 }
 static int Run() {
  DeleteFileA(".\\thprac-uth05.ini");
  CGame game; CGame::s_pCurGame=&game;
  Check(game.Initialize(false),"hidden production OpenGL/game initialization");
  Check(practice::Initialize(game.m_gameWindow.m_hWnd),"ImGui Win32/OpenGL2 initialization");
  if(getenv("TH05_LOCK_RESTART_ONLY")){LockTimeoutRestart(game);game.OnGameEnd();printf("PASS %d lock restart checks\n",checks);return 0;}if(getenv("TH05_TAB_ONLY")){TabInfo(game);game.OnGameEnd();printf("PASS %d Tab checks\n",checks);return 0;}
  CStage* s=Stage(game); CChara& p=*s->m_pChara;
  InactiveFloatingText(*s);
  for(int i=0;i<7;i++) { Key(game,VK_F1+i);Check(practice::Enabled(practice::Tool(i)),"F1-F7 enabled");Key(game,VK_F1+i,true);Check(practice::Enabled(practice::Tool(i)),"key repeat does not toggle"); }
  Key(game,'U');Check(practice::Enabled(practice::EnemyInvincible),"U enabled");Key(game,VK_F8);Check(practice::Enabled(practice::EnemyInvincible),"F8 does not modify tools");ResetTools();
  Key(game,VK_F1);p.m_bMiss=true;const int life=s->m_nCurLife,power=p.m_power,bombs=s->m_nCurBomb;p.Step();
  Check(!p.m_bMiss&&p.m_nextLifeComeTimeLeft==0&&s->m_nCurLife==life&&p.m_power==power&&s->m_nCurBomb==bombs&&s->m_nMiss==0,"F1 collision leaves resources/state intact");Check(s->m_bPlaySound[2],"F1 collision miss sound");ResetTools();p.m_invulnerableTimeLeft=0;
  Key(game,VK_F4);s->m_nCurBomb=0;p.DropBomb();Check(s->m_bInCharaBomb&&s->m_nCurBomb==0,"F4 zero-stock Bomb does not underflow");ResetTools();s->m_bInCharaBomb=false;s->m_nCurBomb=2;p.m_invulnerableTimeLeft=0;
  Key(game,VK_F7);p.m_bMiss=true;p.Step();Check(s->m_bInCharaBomb&&s->m_nCurBomb==1&&p.m_nextLifeComeTimeLeft==0&&s->m_nMiss==0,"F7 collision uses native Bomb before death");ResetTools();s->m_bInCharaBomb=false;
  p.m_invulnerableTimeLeft=0;p.m_nextLifeComeTimeLeft=33;p.m_cannotControlTimeLeft=1;p.m_bMiss=false;p.SetPower(100);s->m_nCurLife=1;
  Key(game,VK_F2);Key(game,VK_F5);p.Step();Check(p.m_power==100,"F5 preserves Miss firepower");
  p.m_nextLifeComeTimeLeft=1;p.Step();Check(s->m_nCurLife==1&&!s->m_bContinueScr,"F2 last life revives without game over");ResetTools();
  s->SetDream(64);Key(game,VK_F3);Check(practice::Enabled(practice::LockDream),"F3 toggles Dream lock");
  s->SetDream(1);Check(s->GetDream()==64,"Dream lock prevents Miss and Bomb dream loss");
  s->SetDream(128);Check(s->GetDream()==64,"Dream lock also preserves current value against item gains");
  CGame::GVar().m_bReplayMode=true;s->SetDream(63);Check(s->GetDream()==63,"live Dream lock cannot alter replay playback");CGame::GVar().m_bReplayMode=false;
  practice::PreparingSection(true);s->SetDream(62);Check(s->GetDream()==62,"Dream lock is isolated from phase preparation");practice::PreparingSection(false);
  ResetTools();s->SetDream(61);Check(s->GetDream()==61,"disabling Dream lock restores native updates");
  Check(practice::AllowTimeout(true)&&!practice::AllowTimeout(false),"normal deadline gate");Key(game,VK_F6);Check(!practice::AllowTimeout(true)&&practice::BeforeTimeout(false),"F6 blocks timeout while retaining hit path");
  // Original Phase4 runs its attack and tests damage after the time cap.
  CBossStage1 boss;boss.m_pStage=s;boss.Initialize();boss.m_curPhase=4;boss.m_curPhaseFrame=1299;boss.m_curHP=400;boss.m_curPhaseEndHP=0;
  s->m_pChara->m_invulnerableTimeLeft=10000;
  boss.Step();Check(boss.m_curPhase==4&&boss.m_curPhaseFrame==1300,"F6 keeps Boss phase advancing past deadline");
  s->m_pCharaBullet->AddBullet(boss.m_curX,boss.m_curY,3,0,0,0,0,255);
  s->m_pCharaBullet->AddBullet(boss.m_curX,boss.m_curY,3,0,0,0,0,255);
  boss.Step();Check(boss.m_curPhase==0xfd&&boss.m_bSuccessDefeat,"F6 still permits defeat after expired deadline");
  s->m_pCharaBullet->Initialize();
  CMidBossStage1 mid;mid.m_pStage=s;mid.Initialize();mid.m_curPhase=3;mid.m_curPhaseFrame=1499;
  mid.Step();Check(mid.m_curPhase==3&&mid.m_curPhaseFrame==1500,"F6 keeps MidBoss alive beyond deadline");
  const int rewardBefore=s->m_curScore;mid.m_curHP=100;
  for(int i=0;i<10;i++)s->m_pCharaBullet->AddBullet(mid.m_curX,mid.m_curY,3,0,0,0,0,255);
  mid.Step();Check(mid.m_curPhase==0xfe&&s->m_curScore>rewardBefore,"expired locked MidBoss still grants defeat reward");
  ResetTools();s->m_pCharaBullet->Initialize();boss.Initialize();boss.m_curPhase=4;boss.m_curPhaseFrame=1299;boss.m_curHP=400;
  boss.Step();Check(boss.m_curPhase==0xfd&&!boss.m_bSuccessDefeat,"disabling F6 restores original timeout");
  Key(game,'U');s->m_pCharaBullet->AddBullet(1000,1000,3,0,0,0,0,80);Check(s->GetDamageFromChara(1000,1000,300,300,true)==0,"U suppresses actual projectile damage");ResetTools();
  CGame::GVar().m_bReplayMode=true;for(int i=0;i<7;i++)Key(game,VK_F1+i);Key(game,'U');
  for(int i=0;i<practice::ToolCount;i++)Check(!practice::Enabled(practice::Tool(i)),"Replay isolates live tools");
  CGame::GVar().m_bReplayMode=false;for(int i=0;i<practice::ToolCount;i++)Check(!practice::Enabled(practice::Tool(i)),"replay hotkeys do not arm future cheats");ResetTools();
  Check(s->Pause(),"Esc pause entry");for(int i=0;i<12;i++)Frame(*s);Screenshot();
  Check(s->m_bPaused,"upstream pause drawing does not advance gameplay");
  ImGuiWindow* pause=ImGui::FindWindowByName("Pause Menu");
  Check(pause&&pause->Pos.x==32&&pause->Pos.y==16&&pause->Size.x==384&&pause->Size.y==448,"THPauseMenu position and size match purple source");
  Check(pause&&(pause->Flags&ImGuiWindowFlags_NoTitleBar),"upstream pause has no invented title bar");
  const ImVec2 button(220,176);
  ImGui::GetIO().MousePos=button;ImGui::GetIO().MouseDown[0]=true;
  ImGui::NewFrame();practice::DrawPause(*s);ImGui::Render();
  ImGui::GetIO().MousePos=button;ImGui::GetIO().MouseDown[0]=false;
  ImGui::NewFrame();practice::DrawPause(*s);ImGui::Render();
  for(int i=0;i<10;i++)Frame(*s);
  Check(!s->m_bPaused,"upstream Resume click closes pause after fade");
  s->Pause();for(int i=0;i<12;i++)Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,'Z',0);Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,'Z',1L<<31);
  for(int i=0;i<10;i++)Frame(*s);
  Check(!s->m_bPaused,"Z confirms focused Resume after mouse use");
  s->Pause();for(int i=0;i<12;i++)Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,VK_ESCAPE,0);Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,VK_ESCAPE,1L<<31);
  for(int i=0;i<10;i++)Frame(*s);
  Check(!s->m_bPaused,"Esc resumes through the upstream fade");
  s->Pause();for(int i=0;i<12;i++)Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,VK_DOWN,0);Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,VK_DOWN,1L<<31);Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,'Z',0);Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,'Z',1L<<31);
  for(int i=0;i<10;i++)Frame(*s);
  Check(s->m_bQuit&&s->m_quitCode==STAGE_END_SAVE_EXIT,"Down and Z activate upstream Exit with replay-save path");
  s->m_bQuit=false;s->m_bCanPause=true;
  s->Pause();for(int i=0;i<12;i++)Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,'Q',0);Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,'Q',1L<<31);
  for(int i=0;i<10;i++)Frame(*s);
  Check(s->m_bQuit&&s->m_quitCode==STAGE_END_PLAYER_QUIT,"Q uses Exit without replay saving");
  s->m_bQuit=false;s->m_bCanPause=true;
  Key(game,VK_BACK);Frame(*s);Frame(*s);Screenshot("overlay.bmp");
  ImGuiWindow* overlay=ImGui::FindWindowByName("Mod Menu");
  Check(overlay&&overlay->Pos.x==10&&overlay->Pos.y==10&&(overlay->Flags&ImGuiWindowFlags_NoNav),"THOverlay position and flags match purple source");
  ImGui::GetIO().MousePos=ImVec2(35,24);ImGui::GetIO().MouseDown[0]=true;
  ImGui::NewFrame();practice::DrawStatus();ImGui::Render();
  ImGui::GetIO().MousePos=ImVec2(35,24);ImGui::GetIO().MouseDown[0]=false;
  ImGui::NewFrame();practice::DrawStatus();ImGui::Render();
  Check(practice::Enabled(practice::Invincible),"upstream clickable text shares F1 state");
  Frame(*s);Screenshot("overlay-enabled.bmp");
  Key(game,VK_F1);Key(game,VK_BACK);
  s->Pause();for(int i=0;i<12;i++)Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,'R',0);Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,'R',1L<<31);
  for(int i=0;i<10;i++)Frame(*s);
  Check(s->m_bQuit&&s->m_quitCode==STAGE_END_RESTART&&!s->m_bPaused,"R requests Restart through fade");
  CGame::GVar().m_playStage=3;
  CScheme* next=NULL;int nextID=0;
  Check(CSchemeSwitcher::SwitchScheme(&next,&nextID,4,STAGE_END_RESTART)&&nextID==4&&CGame::GVar().m_playStage==0,"Restart initializes a fresh normal run");
  delete next;
  CGame::GVar().m_bCanSaveReplay=true;practice::MarkAssisted();Check(CGame::GVar().m_bCanSaveReplay,"live tools preserve the replay-save option");
  delete s;game.m_pCurScheme=NULL;NativePractice(game);CustomPractice(game);PhasePractice(game);PracticeDialogue(game);MusicRoom(game);AdvancedMenu(game);LockTimeoutRestart(game);PhaseTimeoutDisplay(game);CycleTimeProjection(game);EosdBossBar(game);RingBossBar(game);ShinkiRingVisibility(game);TabInfo(game);game.OnGameEnd();printf("PASS %d production practice checks\n",checks);return 0;
 }
};int PracticeTests::checks=0;
}
int main() {return th5w::PracticeTests::Run();}

