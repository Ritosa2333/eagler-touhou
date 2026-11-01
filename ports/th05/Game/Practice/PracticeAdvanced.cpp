// Adapted from purple thprac THAdvOptWnd, GameFPSOpt, DisableKeyOpt,
// KeyHUDOpt and THGuiTestReactionTest. Native TH05 runs at 56 FPS.
#include "Practice.h"
#include "KeyMonitor.h"
#include "../Game.h"
#include "../Stage/Stage.h"
#include "../../third_party/imgui/imgui.h"
#include "../../third_party/imgui/imgui_internal.h"
#include <random>
#include <cmath>
#include <cstdio>
namespace th5w { namespace practice {
namespace {
AdvancedOptions options;int fpsDraft=56;bool open=false,retryRequest=false,quickIntro=false;
const char* settingsFile=".\\thprac-uth05.ini";
double ClockMs(){LARGE_INTEGER now,freq;QueryPerformanceCounter(&now);QueryPerformanceFrequency(&freq);return double(now.QuadPart)*1000.0/double(freq.QuadPart);}
double WaitMs(){static std::mt19937 random(GetTickCount());return std::uniform_int_distribution<int>(1200,3500)(random);}
bool Shift(){const auto& io=ImGui::GetIO();return io.KeysDown[VK_SHIFT]||io.KeysDown[VK_LSHIFT]||io.KeysDown[VK_RSHIFT];}
void Help(const char* text){ImGui::SameLine();ImGui::TextDisabled("(?)");if(ImGui::IsItemHovered()){ImGui::BeginTooltip();ImGui::PushTextWrapPos(420);ImGui::TextUnformatted(text);ImGui::PopTextWrapPos();ImGui::EndTooltip();}}
}
AdvancedOptions& Options(){static AdvancedOptions native;return original::DemoActive()?native:options;}
bool AdvancedOpen(){return open&&!original::DemoActive();}
void SetAdvancedOpen(bool value){open=value;if(value)fpsDraft=options.fps;if(!value){Reaction().Reset();SaveAdvancedOptions();}}
float GameFPS(){return original::DemoActive()?56.f:float(Config::Bound(options.fps,1,6000));}
unsigned short FilterGameplayInput(unsigned short raw,bool replay){
 if(replay)return raw;
 if(options.disableX)raw&=~16;if(options.disableZ)raw&=~32;if(options.disableShift)raw&=~64;if(options.forceShift)raw|=64;
 return raw;
}
bool ConsumeLife(int life){return !Enabled(InfiniteLives)||(options.noContinue&&life>1);}
bool TakeFastRetryRequest(){const bool value=retryRequest;retryRequest=false;return value;}
bool TakeQuickIntro(){const bool value=quickIntro;quickIntro=false;return value;}
void SetQuickIntro(){quickIntro=true;}
void LoadAdvancedOptions(){
 options=AdvancedOptions();options.phaseTimeoutAsTime=GetPrivateProfileIntA("Advanced","phaseTimeoutAsTime",0,settingsFile)!=0;options.fps=Config::Bound(GetPrivateProfileIntA("Advanced","FPS",56,settingsFile),1,6000);
 options.eosdBar=GetPrivateProfileIntA("Advanced","eosdBar",0,settingsFile)!=0;
 options.phaseStars=GetPrivateProfileIntA("Advanced","phaseStars",0,settingsFile)!=0;
 options.ringBar=GetPrivateProfileIntA("Advanced","ringBar",0,settingsFile)!=0;
 options.hpPhaseMarkers=GetPrivateProfileIntA("Advanced","hpPhaseMarkers",0,settingsFile)!=0;
 options.phaseTimeout=GetPrivateProfileIntA("Advanced","phaseTimeout",0,settingsFile)!=0;
 options.numericDream=GetPrivateProfileIntA("Advanced","numericDream",0,settingsFile)!=0;
 options.numericPower=GetPrivateProfileIntA("Advanced","numericPower",0,settingsFile)!=0;
 options.freeFPS=GetPrivateProfileIntA("Advanced","freeFPS",0,settingsFile)!=0;
 options.noContinue=GetPrivateProfileIntA("Advanced","noContinue",0,settingsFile)!=0;
 options.disableX=GetPrivateProfileIntA("Advanced","disableX",0,settingsFile)!=0;
 options.disableShift=GetPrivateProfileIntA("Advanced","disableShift",0,settingsFile)!=0;
 options.disableZ=GetPrivateProfileIntA("Advanced","disableZ",0,settingsFile)!=0;
 options.forceShift=GetPrivateProfileIntA("Advanced","forceShift",0,settingsFile)!=0;
 options.showKeys=GetPrivateProfileIntA("Advanced","showKeys",0,settingsFile)!=0;
 options.lockTimer=GetPrivateProfileIntA("Advanced","lockTimer",0,settingsFile)!=0;
 options.fastRetry=GetPrivateProfileIntA("Advanced","fastRetry",0,settingsFile)!=0;
 options.showHitbox=GetPrivateProfileIntA("Advanced","showHitbox",0,settingsFile)!=0;options.alwaysHitbox=GetPrivateProfileIntA("Advanced","alwaysHitbox",0,settingsFile)!=0;fpsDraft=options.fps;
 if(options.ringBar){options.eosdBar=false;options.hpPhaseMarkers=false;options.phaseStars=false;options.phaseTimeout=true;}
 else if(options.eosdBar){options.hpPhaseMarkers=false;options.phaseTimeout=true;}
 else options.phaseStars=false;
}
void SaveAdvancedOptions(){
 if(options.ringBar){options.eosdBar=false;options.hpPhaseMarkers=false;options.phaseStars=false;options.phaseTimeout=true;}
 else if(options.eosdBar){options.hpPhaseMarkers=false;options.phaseTimeout=true;}
 else options.phaseStars=false;
 WritePrivateProfileStringA("Advanced","eosdBar",options.eosdBar?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","phaseStars",options.phaseStars?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","ringBar",options.ringBar?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","numericDream",options.numericDream?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","numericPower",options.numericPower?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","phaseTimeoutAsTime",options.phaseTimeoutAsTime?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","phaseTimeout",options.phaseTimeout?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","hpPhaseMarkers",options.hpPhaseMarkers?"1":"0",settingsFile);
 char number[24];sprintf_s(number,"%d",Config::Bound(options.fps,1,6000));WritePrivateProfileStringA("Advanced","FPS",number,settingsFile);
 WritePrivateProfileStringA("Advanced","freeFPS",options.freeFPS?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","noContinue",options.noContinue?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","disableX",options.disableX?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","disableShift",options.disableShift?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","disableZ",options.disableZ?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","forceShift",options.forceShift?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","showKeys",options.showKeys?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","lockTimer",options.lockTimer?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","fastRetry",options.fastRetry?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","showHitbox",options.showHitbox?"1":"0",settingsFile);
 WritePrivateProfileStringA("Advanced","alwaysHitbox",options.alwaysHitbox?"1":"0",settingsFile);
}
void AdvancedKeyEvent(UINT message,WPARAM key,LPARAM data){
 if(original::DemoActive())return;
 if(message==WM_KILLFOCUS){retryRequest=false;return;}
 if(message!=WM_KEYDOWN||(data&(1L<<30)))return;
 if(key==VK_F12){SetAdvancedOpen(!open);return;}
 if(open||!CGame::s_pCurGame||CGame::s_pCurGame->m_curSchemeID!=4||CGame::GVar().m_bReplayMode||!Shift()||ImGui::GetIO().WantTextInput)return;
 bool changed=false;
 if(key=='D'){options.disableX=!options.disableX;changed=true;}
 if(key=='A'){options.disableShift=!options.disableShift;changed=true;}
 if(key=='S'){options.disableZ=!options.disableZ;changed=true;}
 if(key=='F'&&options.fastRetry&&CGame::GVar().m_bPracticeMode)retryRequest=true;
 if(changed)SaveAdvancedOptions();
}
ReactionTest::ReactionTest(){Reset();}
void ReactionTest::Reset(){state=Idle;type=0;trials=5;frames=0;progress=false;deadline=delay=0;results.clear();frameCounts.clear();}
void ReactionTest::Start(double now,double wait,bool newSeries){
 if(newSeries){results.clear();frameCounts.clear();}
 delay=wait;deadline=now+wait;frames=0;state=type?WaitHold:Waiting;
}
void ReactionTest::Tick(double now,bool arrows){
 if(state==WaitHold){if(arrows){deadline=now+delay;frames=0;state=Waiting;}return;}
 if(state!=Waiting&&state!=Ready)return;
 if(state==Waiting&&now>=deadline){state=Ready;frames=0;}
 ++frames;const bool responded=type?!arrows:arrows;
 if(!responded)return;
 if(state==Waiting&&!progress){state=TooEarly;return;}
 results.push_back(float(now-deadline));frameCounts.push_back(state==Waiting?float(std::ceil(std::fabs(now-deadline)*56.0/1000.0)):float(frames));state=Result;
}
ReactionTest& Reaction(){static ReactionTest test;return test;}
namespace {
void DrawReaction(){
 auto& test=Reaction();test.trials=Config::Bound(test.trials,1,20);
 const bool arrows=ImGui::IsKeyDown(VK_UP)||ImGui::IsKeyDown(VK_DOWN)||ImGui::IsKeyDown(VK_LEFT)||ImGui::IsKeyDown(VK_RIGHT);
 test.Tick(ClockMs(),arrows);
 if(test.state==ReactionTest::Idle){
  ImGui::RadioButton(u8"\u6309\u4e0b\u53cd\u5e94\u6d4b\u8bd5\uff08\u7b49\u5f85\u53d8\u8272\u540e\u6309\u65b9\u5411\u952e\uff09",&test.type,0);ImGui::RadioButton(u8"\u5f39\u8d77\u53cd\u5e94\u6d4b\u8bd5\uff08\u6309\u4f4f\u65b9\u5411\u952e\uff0c\u53d8\u8272\u540e\u677e\u5f00\uff09",&test.type,1);
  ImGui::Checkbox(u8"\u663e\u793a\u7b49\u5f85\u65f6\u95f4\u8fdb\u5ea6\u6761",&test.progress);ImGui::SetNextItemWidth(160);ImGui::DragInt(u8"\u6d4b\u8bd5\u6b21\u6570",&test.trials,1,1,20,"%d",ImGuiSliderFlags_AlwaysClamp);
 }
 if(test.state==ReactionTest::Idle||test.state==ReactionTest::TooEarly||test.state==ReactionTest::Result){
  if(test.state==ReactionTest::TooEarly)ImGui::TextUnformatted(u8"\u6309\u592a\u5feb\u4e86");
  if(!test.results.empty()){
   const float ms=test.results.back();ImGui::Text("%s (%d): %.1f ms (%.1f frame)",u8"\u7ed3\u679c",int(test.results.size()),ms,ms*56.f/1000.f);ImGui::Text("framecount: %d",int(test.frameCounts.back()));
   if(int(test.results.size())>=test.trials){
    float total=0;for(auto value:test.results)total+=std::fabs(value);
    ImGui::Text("%s: %.1f ms (%.1f frame)",u8"\u7ed3\u679c\uff08\u5e73\u5747\u503c\uff09",total/test.results.size(),total/test.results.size()*56.f/1000.f);
    ImGui::PlotHistogram("##Reaction results",test.results.data(),int(test.results.size()),0,"ms",FLT_MAX,FLT_MAX,ImVec2(0,110));
    ImGui::PlotHistogram("##Reaction framecounts",test.frameCounts.data(),int(test.frameCounts.size()),0,"framecount",FLT_MAX,FLT_MAX,ImVec2(0,110));
   }
  }
  const bool initial=test.state==ReactionTest::Idle;
  if(ImGui::Button(initial?u8"\u5f00\u59cb\u6d4b\u8bd5\uff08\u5feb\u6377\u952e Z / Shift\uff09":u8"\u518d\u6d4b\u4e00\u6b21\uff08\u5feb\u6377\u952e Z / Shift\uff09")||ImGui::IsKeyPressed('Z',false)||ImGui::IsKeyPressed(VK_SHIFT,false)||ImGui::IsKeyPressed(VK_LSHIFT,false))test.Start(ClockMs(),WaitMs(),initial||int(test.results.size())>=test.trials);
 }
 if(test.state==ReactionTest::WaitHold||test.state==ReactionTest::Waiting||test.state==ReactionTest::Ready){
  ImGui::TextUnformatted(u8"\u7b49\u5f85\u7ea2\u8272\u53d8\u7eff\u65f6\u6309\u4e0b\u6216\u677e\u5f00\u65b9\u5411\u952e");
  if(test.progress){const float left=float((test.deadline-ClockMs())/(test.delay?test.delay:1));ImGui::ProgressBar(ImClamp(left,0.f,1.f),ImVec2(0,0));}
  ImVec4 color=test.state==ReactionTest::Ready?ImVec4(0,1,.2f,1):test.state==ReactionTest::WaitHold?ImVec4(1,1,.2f,1):ImVec4(1,.2f,.2f,1);
  ImGui::ColorButton("##Reaction color",color,ImGuiColorEditFlags_NoTooltip,ImVec2(ImGui::GetContentRegionAvail().x*.5f,120));
 }
}
}
void DrawAdvanced(){
 if(!open)return;ImGui::GetIO().MouseDrawCursor=true;
 ImGui::SetNextWindowPos(ImVec2(0,0),ImGuiCond_Always);ImGui::SetNextWindowSize(ImVec2(640,480),ImGuiCond_Always);ImGui::SetNextWindowBgAlpha(.8f);
 ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0.f);ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0.f);
 ImGui::Begin("TH05 Advanced Options",NULL,ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings);
 ImGui::TextUnformatted(u8"\u9ad8\u7ea7\u9009\u9879");ImGui::Separator();ImGui::BeginChild("Adv. Options",ImVec2(0,0));
 bool changed=false;
 if(ImGui::CollapsingHeader(u8"\u6e38\u620f\u901f\u5ea6")){
  ImGui::SetNextItemWidth(260);
  if(options.freeFPS)changed|=ImGui::DragInt(u8"\u5e27\u7387\u8c03\u6574",&fpsDraft,1,1,6000,"%d",ImGuiSliderFlags_AlwaysClamp);
  else {int preset=Config::Bound(int(std::round(fpsDraft/14.f))-1,0,11);char label[48];sprintf_s(label,"x%.2f (%d fps)",(preset+1)*.25f,(preset+1)*14);if(ImGui::SliderInt(u8"\u5e27\u7387\u8c03\u6574",&preset,0,11,label)){fpsDraft=(preset+1)*14;}}
  ImGui::SameLine();changed|=ImGui::Checkbox(u8"\u81ea\u7531\u8c03\u6574",&options.freeFPS);
  if(!options.freeFPS)fpsDraft=Config::Bound(int(std::round(fpsDraft/14.f)),1,12)*14;
  if(fpsDraft!=options.fps){ImGui::SameLine();if(ImGui::Button(u8"\u5e94\u7528")){options.fps=fpsDraft;changed=true;}}
  ImGui::TextUnformatted(u8"TH05 \u539f\u901f\u4e3a 56 fps\uff1b\u6b64\u9879\u8c03\u6574\u6e38\u620f\u8fd0\u884c\u901f\u5ea6\uff0c\u97f3\u4e50\u4fdd\u6301\u539f\u901f\u3002");
 }
 if(ImGui::CollapsingHeader(u8"\u6e38\u620f\u8fdb\u884c",ImGuiTreeNodeFlags_DefaultOpen)){
  changed|=ImGui::Checkbox(u8"\u5c06\u9501\u6b8b\u4fee\u6539\u4e3a\u4e0d\u75ae",&options.noContinue);Help(u8"F2 \u5f00\u542f\u65f6\u6b63\u5e38\u6263\u6b8b\u673a\uff0c\u6700\u540e\u4e00\u547d\u540e\u4e0d\u518d\u8fdb\u5165 Game Over\u3002");
  ImGui::Separator();ImGui::TextUnformatted(u8"\u7981\u7528\u952e\u4f4d");
  changed|=ImGui::Checkbox(u8"\u7981\u7528 X \u952e",&options.disableX);ImGui::SameLine();changed|=ImGui::Checkbox(u8"\u7981\u7528 Shift \u952e",&options.disableShift);ImGui::SameLine();changed|=ImGui::Checkbox(u8"\u7981\u7528 Z \u952e",&options.disableZ);
  ImGui::TextDisabled(u8"\u5feb\u6377\u952e\uff1aShift+D / Shift+A / Shift+S\uff1b\u672c\u4f5c\u6ca1\u6709\u7ed1\u5b9a C \u952e\u3002");changed|=ImGui::Checkbox(u8"\u5f3a\u5236\u6309\u4f4f Shift \u952e",&options.forceShift);Help(u8"\u5f3a\u5236\u4f4e\u901f\u4f18\u5148\u4e8e\u7981\u7528\u4f4e\u901f\u3002\u53ea\u5f71\u54cd\u6e38\u620f\u64cd\u4f5c\uff0c\u4e0d\u5f71\u54cd\u83dc\u5355\u6216\u5f55\u50cf\u56de\u653e\u3002");
  changed|=ImGui::Checkbox(u8"\u68a6\u69fd\u72b6\u6001\u4ee5\u6570\u5b57\u663e\u793a",&options.numericDream);
  changed|=ImGui::Checkbox(u8"\u706b\u529b\u72b6\u6001\u4ee5\u6570\u5b57\u663e\u793a",&options.numericPower);
  if(options.eosdBar||options.ringBar){options.phaseTimeout=true;ImGui::PushItemFlag(ImGuiItemFlags_Disabled,true);ImGui::PushStyleVar(ImGuiStyleVar_Alpha,ImGui::GetStyle().Alpha*.5f);}
  changed|=ImGui::Checkbox(u8"\u9636\u6bb5\u5269\u4f59\u65f6\u95f4/\u6b21\u6570\u663e\u793a",&options.phaseTimeout);
  if(options.eosdBar||options.ringBar){ImGui::PopStyleVar();ImGui::PopItemFlag();}Help(u8"\u653b\u51fb\u5faa\u73af\u53ef\u9009\u62e9\u8f6e\u6570\u6216\u65f6\u95f4\uff1b\u65f6\u95f4\u6309\u539f\u901f56\u5e27\u6bcf\u79d2\u6362\u7b97\uff0c\u968f\u673a\u79fb\u52a8\u9636\u6bb5\u7684\u9884\u8ba1\u65f6\u95f4\u7528~\u6807\u8bb0\u3002");
  if(options.phaseTimeout){
   int unit=options.phaseTimeoutAsTime?1:0;
   const char* units[]={u8"\u8f6e\u6570",u8"\u65f6\u95f4"};
   if(ImGui::Combo(u8"\u653b\u51fb\u5faa\u73af\u663e\u793a",&unit,units,2)){options.phaseTimeoutAsTime=unit==1;changed=true;}
  }
  if(ImGui::Checkbox(u8"\u8840\u6761\u63d0\u793a\u9636\u6bb5",&options.hpPhaseMarkers)){changed=true;if(options.hpPhaseMarkers){options.eosdBar=false;options.ringBar=false;}}
  if(ImGui::Checkbox(u8"\u7ea2\u9b54\u4e61\u5f0f\u8840\u6761",&options.eosdBar)){changed=true;if(options.eosdBar){options.ringBar=false;options.hpPhaseMarkers=false;options.phaseTimeout=true;}}
  if(ImGui::Checkbox(u8"\u73af\u5f62\u8840\u6761",&options.ringBar)){changed=true;if(options.ringBar){options.eosdBar=false;options.hpPhaseMarkers=false;options.phaseStars=false;options.phaseTimeout=true;}}
  if(!options.eosdBar){options.phaseStars=false;ImGui::PushItemFlag(ImGuiItemFlags_Disabled,true);ImGui::PushStyleVar(ImGuiStyleVar_Alpha,ImGui::GetStyle().Alpha*.5f);}
  changed|=ImGui::Checkbox(u8"\u6309\u661f\u661f\u663e\u793a\u9636\u6bb5\u6570",&options.phaseStars);
  if(!options.eosdBar){ImGui::PopStyleVar();ImGui::PopItemFlag();}
  changed|=ImGui::Checkbox(u8"\u6253\u5f00\u6309\u952e\u663e\u793a",&options.showKeys);
  if(options.showKeys){
   if(!g_record_key_aps){if(ImGui::Button(u8"\u5f00\u59cb\u8bb0\u5f55APS")){ClearKeyRecord();g_record_key_aps=true;}}
   else if(ImGui::Button(u8"\u505c\u6b62\u8bb0\u5f55APS"))g_record_key_aps=false;
   ImGui::SameLine();if(ImGui::Button(u8"\u5bfc\u51fa\u5230\u6e38\u620f\u6587\u4ef6\u5939")){
    if(!SaveKeyRecorded())MessageBoxW(NULL,L"APS.csv \u5199\u5165\u5931\u8d25",L"thprac",MB_OK|MB_ICONERROR);
   }
   auto& aps=GetKeyAPS();if(aps.size()>=2)ImGui::PlotLines("##APS",[](void* data,int idx)->float{auto& values=*static_cast<std::vector<int>*>(data);return float(values[values.size()>600?values.size()-600+idx:idx]);},&aps,int(aps.size()>600?600:aps.size()),0,NULL,FLT_MAX,FLT_MAX,ImVec2(0,ImGui::GetFrameHeight()*3.f));
  }

  changed|=ImGui::Checkbox(u8"\u542f\u7528\u9501\u65f6\u8ba1\u65f6",&options.lockTimer);
  changed|=ImGui::Checkbox(u8"\u7ec3\u4e60\u6a21\u5f0f\u5feb\u901fre",&options.fastRetry);Help(u8"\u7ec3\u4e60\u4e2d Miss \u540e\u81ea\u52a8\u91cd\u8bd5\uff1bShift+F \u53ef\u624b\u52a8\u89e6\u53d1\u3002");
  ImGui::Separator();
  ImGui::TextUnformatted(u8"\u663e\u793a\u5224\u5b9a\u70b9");
  changed|=ImGui::Checkbox(u8"\u4f4e\u901f\u65f6\u663e\u793a",&options.showHitbox);
  changed|=ImGui::Checkbox(u8"\u5e38\u9a7b\u5224\u5b9a\u70b9",&options.alwaysHitbox);
 }
 if(ImGui::CollapsingHeader(u8"\u53cd\u5e94\u529b\u6d4b\u8bd5"))DrawReaction();else Reaction().Reset();
 if(ImGui::CollapsingHeader(u8"\u5173\u4e8ethprac")){
  ImGui::TextUnformatted("TH05 / uth05win native Practice adapter");
  ImGui::TextWrapped("%s",u8"\u754c\u9762\u4e0e\u884c\u4e3a\u53c2\u8003\uff1aAck \u7684\u7d2b\u7248 thprac\uff0c\u57fa\u4e8e touhouworldcup/thprac\u3002\u539f\u751f\u6e38\u620f\uff1auth05win\uff1b\u539f\u7248\u6d41\u7a0b\u53c2\u8003\uff1aReC98\u3002");ImGui::TextUnformatted("https://github.com/RUEEE/thprac");
  ImGui::TextUnformatted("https://github.com/KyoriAsh/uth05win");ImGui::TextUnformatted("https://github.com/nmlgc/ReC98");
  static bool license=false;if(ImGui::Button(license?u8"\u9690\u85cf\u8bb8\u53ef\u4fe1\u606f":u8"\u663e\u793a\u8bb8\u53ef\u4fe1\u606f"))license=!license;
  if(license){ImGui::TextWrapped("%s","The MIT License (MIT)\n\nCopyright (c) 2014-2021 Omar Cornut\n\nPermission is hereby granted, free of charge, to any person obtaining a copy\nof this software and associated documentation files (the \"Software\"), to deal\nin the Software without restriction, including without limitation the rights\nto use, copy, modify, merge, publish, distribute, sublicense, and/or sell\ncopies of the Software, and to permit persons to whom the Software is\nfurnished to do so, subject to the following conditions:\n\nThe above copyright notice and this permission notice shall be included in all\ncopies or substantial portions of the Software.\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\nIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\nFITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\nLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\nSOFTWARE.\n");}
 }
 ImGui::EndChild();ImGui::End();ImGui::PopStyleVar(2);
 if(changed)SaveAdvancedOptions();
}
void DrawRuntimeHUD(){
 if(open||!CGame::s_pCurGame||CGame::s_pCurGame->m_curSchemeID!=4||!CGame::s_pCurGame->m_pCurScheme)return;
 auto* stage=static_cast<CStage*>(CGame::s_pCurGame->m_pCurScheme);
 if(options.lockTimer&&Enabled(LockTimeout)&&stage->LockTimerFrames()>0){
  char value[32];sprintf_s(value,"%.2f",stage->LockTimerFrames()/56.f);auto* draw=ImGui::GetForegroundDrawList();ImVec2 size=ImGui::CalcTextSize(value);draw->AddRectFilled(ImVec2(32,0),ImVec2(110,size.y),IM_COL32_WHITE);draw->AddText(ImVec2(110-size.x,0),IM_COL32_BLACK,value);
 }
 if(InGameInfoOpen())stage->DrawPracticeInfo();
 if(!options.showKeys)return;
 KeyRectStyle style;style.size=ImVec2(48,48);style.separated=true;style.padding=ImVec2(0,0);
 style.border_color_press=style.border_color_release=0xFFFFFFFF;
 style.fill_color_press=0xFFFF4444;style.fill_color_release=0xFFFFCCCC;
 style.text_color_press=style.text_color_release=0xFFFFFFFF;style.type=2;
 KeysHUD(5,ImVec2(1280,0),ImVec2(833,0),style);

}
const char* AdvancedGlyphs(){return u8"\u73af\u5f62\u8840\u6761\u6309\u661f\u661f\u663e\u793a\u9636\u6bb5\u6570\u653b\u51fb\u5faa\u73af\u663e\u793a\u8f6e\u6570\u65f6\u95f4\u9884\u8ba1\u968f\u673a\u79fb\u52a8\u6362\u7b97\u6807\u8bb0\u7ea2\u9b54\u4e61\u5f0f\u8840\u6761\u8840\u6761\u63d0\u793a\u9636\u6bb5\u2191\u2190\u2193\u2192\u0394\u03a3\u5f00\u59cb\u8bb0\u5f55APS\u505c\u6b62\u8bb0\u5f55APS\u5bfc\u51fa\u5230\u6e38\u620f\u6587\u4ef6\u5939\u9636\u6bb5\u5269\u4f59\u65f6\u95f4/\u6b21\u6570\u663e\u793a\u653b\u51fb\u5faa\u73af\u53ef\u9009\u62e9\u8f6e\u6570\u6216\u65f6\u95f4\uff1b\u65f6\u95f4\u6309\u539f\u901f56\u5e27\u6bcf\u79d2\u6362\u7b97\uff0c\u968f\u673a\u79fb\u52a8\u9636\u6bb5\u7684\u9884\u8ba1\u65f6\u95f4\u7528~\u6807\u8bb0\u3002\u68a6\u69fd\u72b6\u6001\u4ee5\u6570\u5b57\u663e\u793a\u706b\u529b\u72b6\u6001\u4ee5\u6570\u5b57\u663e\u793a\u4f5c\u5f0a\u4f4e\u901f\u65f6\u663e\u793a\u5e38\u9a7b\u5224\u5b9a\u70b9\u5e94\u7528\u6309\u4e0b\u53cd\u5e94\u6d4b\u8bd5\uff08\u7b49\u5f85\u53d8\u8272\u540e\u6309\u65b9\u5411\u952e\uff09\u5f39\u8d77\u53cd\u5e94\u6d4b\u8bd5\uff08\u6309\u4f4f\u65b9\u5411\u952e\uff0c\u53d8\u8272\u540e\u677e\u5f00\uff09\u663e\u793a\u7b49\u5f85\u65f6\u95f4\u8fdb\u5ea6\u6761\u6d4b\u8bd5\u6b21\u6570\u6309\u592a\u5feb\u4e86\u7ed3\u679c\u7ed3\u679c\uff08\u5e73\u5747\u503c\uff09\u5f00\u59cb\u6d4b\u8bd5\uff08\u5feb\u6377\u952e Z / Shift\uff09\u518d\u6d4b\u4e00\u6b21\uff08\u5feb\u6377\u952e Z / Shift\uff09\u7b49\u5f85\u7ea2\u8272\u53d8\u7eff\u65f6\u6309\u4e0b\u6216\u677e\u5f00\u65b9\u5411\u952e\u9ad8\u7ea7\u9009\u9879\u6e38\u620f\u901f\u5ea6\u5e27\u7387\u8c03\u6574\u81ea\u7531\u8c03\u6574\u6062\u590d\u539f\u901fTH05 \u539f\u901f\u4e3a 56 fps\uff1b\u6b64\u9879\u8c03\u6574\u6e38\u620f\u8fd0\u884c\u901f\u5ea6\uff0c\u97f3\u4e50\u4fdd\u6301\u539f\u901f\u3002\u6e38\u620f\u8fdb\u884c\u5c06\u9501\u6b8b\u4fee\u6539\u4e3a\u4e0d\u75aeF2 \u5f00\u542f\u65f6\u6b63\u5e38\u6263\u6b8b\u673a\uff0c\u6700\u540e\u4e00\u547d\u540e\u4e0d\u518d\u8fdb\u5165 Game Over\u3002\u7981\u7528\u952e\u4f4d\u7981\u7528 X \u952e\u7981\u7528 Shift \u952e\u7981\u7528 Z \u952e\u5feb\u6377\u952e\uff1aShift+D / Shift+A / Shift+S\uff1b\u672c\u4f5c\u6ca1\u6709\u7ed1\u5b9a C \u952e\u3002\u5f3a\u5236\u6309\u4f4f Shift \u952e\u5f3a\u5236\u4f4e\u901f\u4f18\u5148\u4e8e\u7981\u7528\u4f4e\u901f\u3002\u53ea\u5f71\u54cd\u6e38\u620f\u64cd\u4f5c\uff0c\u4e0d\u5f71\u54cd\u83dc\u5355\u6216\u5f55\u50cf\u56de\u653e\u3002\u6253\u5f00\u6309\u952e\u663e\u793a\u542f\u7528\u9501\u65f6\u8ba1\u65f6\u7ec3\u4e60\u6a21\u5f0f\u5feb\u901fre\u7ec3\u4e60\u4e2d Miss \u540e\u81ea\u52a8\u91cd\u8bd5\uff1bShift+F \u53ef\u624b\u52a8\u89e6\u53d1\u3002\u663e\u793a\u5224\u5b9a\u70b9\u59cb\u7ec8\u663e\u793a\u81ea\u673a\u5224\u5b9a\u70b9\uff1b\u539f\u6709\u7684\u4f4e\u901f\u663e\u793a\u4fdd\u7559\u3002\u53cd\u5e94\u529b\u6d4b\u8bd5\u5173\u4e8ethprac\u754c\u9762\u4e0e\u884c\u4e3a\u53c2\u8003\uff1aAck \u7684\u7d2b\u7248 thprac\uff0c\u57fa\u4e8e touhouworldcup/thprac\u3002\u539f\u751f\u6e38\u620f\uff1auth05win\uff1b\u539f\u7248\u6d41\u7a0b\u53c2\u8003\uff1aReC98\u3002\u9690\u85cf\u8bb8\u53ef\u4fe1\u606f\u663e\u793a\u8bb8\u53ef\u4fe1\u606f\u6309\u952e\u663e\u793a###TH05 Key Monitor";}
} }
