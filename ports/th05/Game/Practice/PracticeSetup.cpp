// Layout and widget order follow purple TH06::THGuiPrac::PracticeMenu.
#include "Practice.h"
#include "../Game.h"
#include "../../third_party/imgui/imgui.h"
#include "../../third_party/imgui/imgui_internal.h"
namespace th5w { namespace practice {
namespace {
void SetupCombo(const char* label,int* value,const char* const* items,int count){
 ImGui::Combo(label,value,items,count);
 if(ImGui::IsItemFocused()&&!ImGui::IsPopupOpen(static_cast<const char*>(NULL),ImGuiPopupFlags_AnyPopupId)){
  if(ImGui::IsKeyPressed(VK_LEFT))*value=(*value+count-1)%count;
  else if(ImGui::IsKeyPressed(VK_RIGHT))*value=(*value+1)%count;
 }
}
void SetupAdjust(int* value,int lo,int hi,int step,bool wrap){
 if(!ImGui::IsItemFocused()||ImGui::IsItemActive())return;
 const int direction=ImGui::IsKeyPressed(VK_LEFT)?-1:ImGui::IsKeyPressed(VK_RIGHT)?1:0;
 if(direction){*value+=direction*step;if(wrap&&step==1){if(*value<lo)*value=hi;else if(*value>hi)*value=lo;}else *value=Config::Bound(*value,lo,hi);}
}
void SetupSlider(const char* label,int* value,int lo,int hi){
 ImGui::SliderInt(label,value,lo,hi);SetupAdjust(value,lo,hi,1,true);
}
void SetupDrag(const char* label,int* value,float speed,int lo,int hi){
 ImGui::DragInt(label,value,speed,lo,hi,"%d",ImGuiSliderFlags_AlwaysClamp);SetupAdjust(value,lo,hi,int(speed),false);
}
}
Config& MenuConfig(){static Config value;return value;}
void OpenSetup(int stage){
 static bool first=true;
 if(first){MenuConfig().mode=1;MenuConfig().stage=stage;first=false;}
}
bool StartSetup(){
 auto& g=CGame::GVar();Config p=MenuConfig();
 const int difficulty=p.stage==6?4:g.m_initDifficulty;
 p.Clamp(difficulty);
 if(!p.Valid(difficulty))return false;
 if(!p.mode&&(p.stage>=6||!g.PracticeStageUnlocked(p.stage)))return false;
 g.m_practiceConfig=p;g.m_playStage=p.stage;g.m_playDifficulty=difficulty;
 g.m_bPracticeMode=true;g.m_bReplayMode=false;g.OnBeginGame();return true;
}
int DrawSetup(int age){
 if(!ImGui::GetCurrentContext()||AdvancedOpen())return 0;
 Config& p=MenuConfig();int action=0;
 const bool popup=ImGui::IsPopupOpen(static_cast<const char*>(NULL),ImGuiPopupFlags_AnyPopupId);
 const bool idle=!ImGui::IsAnyItemActive()&&!popup;
 const bool confirm=age>=10&&idle&&ImGui::IsKeyPressed('Z',false);
 const bool cancel=age>=10&&idle&&ImGui::IsKeyPressed('X',false);
 // Purple practice uses the game's Z/X, not ImGui button activation.
 // Do not let Z also open the focused combo or activate a slider.
 if(!popup&&ImGui::GetIO().KeysDown['Z']){GImGui->NavActivateId=GImGui->NavActivateDownId=GImGui->NavActivatePressedId=GImGui->NavInputId=0;}

 ImGui::GetIO().MouseDrawCursor=true;
 const float alpha=ImMin(.8f,age*.1f);
 ImGui::SetNextWindowPos(ImVec2(260,25),ImGuiCond_Always);
 ImGui::SetNextWindowSize(ImVec2(330,430),ImGuiCond_Always);
 ImGui::SetNextWindowBgAlpha(alpha);
 ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0.f);
 ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0.f);
 ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,ImVec2(8,2));
 ImGui::Begin("TH05 Practice",NULL,ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoSavedSettings|(age<10?ImGuiWindowFlags_NoInputs:0));
 ImGui::TextUnformatted(u8"\u7ec3\u4e60\u6a21\u5f0f");ImGui::Separator();ImGui::PushItemWidth(-85);
 if(age==10){ImGui::SetWindowFocus();ImGui::SetKeyboardFocusHere();}
 const char* modes[]={u8"\u6b63\u5e38\u6a21\u5f0f",u8"\u81ea\u5b9a\u4e49\u6a21\u5f0f"};
 SetupCombo(u8"\u6a21\u5f0f",&p.mode,modes,2);
 const char* stages[]={"STAGE 1","STAGE 2","STAGE 3","STAGE 4","STAGE 5","STAGE 6","EXTRA"};
 if(!p.mode&&p.stage==6)p.stage=0;
 const int oldStage=p.stage;SetupCombo(u8"\u5173\u5361",&p.stage,stages,p.mode?7:6);
 if(oldStage!=p.stage){p.section=p.rage=p.trigger=0;p.startHP=-1;}
 if(p.mode){
  const char* warps[]={u8"\u65e0",u8"\u9053\u4e2dBoss",u8"\u5173\u5e95Boss",u8"\u5e27"};
  const int oldWarp=p.warp;SetupCombo(u8"\u4f20\u9001",&p.warp,warps,4);
  if(oldWarp!=p.warp){p.section=p.rage=p.trigger=0;p.startHP=-1;}
  if(p.stage==5&&p.warp==1){
   int section=0;const char* choices[]={u8"\u4ece\u5173\u5361\u8d77\u59cb\u5904\u5f00\u59cb\uff08\u6ca1\u6709\u9053\u4e2d\uff09"};
   ImGui::SetNextItemWidth(-45);SetupCombo(u8"\u9636\u6bb5",&section,choices,1);
  }
  if((p.warp==1||p.warp==2)&&!(p.stage==5&&p.warp==1)){
   int count=0;const Section* list=Sections(p.stage,p.warp,count);
   const char* names[32];names[0]=u8"\u6b63\u5e38\u5f00\u59cb";
   for(int i=0;i<count;i++)names[i+1]=list[i].name;
   p.section=Config::Bound(p.section,0,count);
   const int oldSection=p.section;
   SetupCombo(u8"\u9636\u6bb5",&p.section,names,count+1);
   if(oldSection!=p.section){p.rage=p.trigger=0;p.startHP=-1;}
   const Section* selected=SelectedSection(p.stage,p.warp,p.section);
   if(selected){
    const int oldRage=p.rage;
    if(selected->rageCount){const char* rageNames[]={u8"\u6b63\u5e38",u8"\u53d1\u72c2",u8"\u53d1\u72c2 2"};SetupCombo(u8"\u72b6\u6001",&p.rage,rageNames,selected->rageCount+1);}
    if(p.warp==1&&p.stage==1&&selected->phase==2){const char* entries[]={u8"\u51fb\u7834\u8fdb\u5165",u8"\u8d85\u65f6\u8fdb\u5165"};SetupCombo(u8"\u8fdb\u5165\u65b9\u5f0f",&p.trigger,entries,2);}
    if(p.warp==2&&p.stage==5&&selected->phase==10&&p.rage){const char* triggers[]={u8"\u8840\u91cf\u89e6\u53d1",u8"\u65f6\u95f4\u89e6\u53d1"};SetupCombo(u8"\u89e6\u53d1\u65b9\u5f0f",&p.trigger,triggers,2);}
    if(p.warp==1&&p.stage==0&&oldRage!=p.rage)p.startHP=p.rage?200:selected->startHP;
    if(selected->continuous){if(p.startHP<0)p.startHP=p.warp==1&&p.stage==0&&p.rage?200:selected->startHP;SetupSlider(u8"\u8d77\u59cb\u8840\u91cf",&p.startHP,selected->endHP+1,selected->startHP);}
   }
  }
  if(p.HasDialogue()){
   bool dialogue=p.dialogue!=0;ImGui::Checkbox(u8"\u5bf9\u8bdd",&dialogue);p.dialogue=dialogue?1:0;
  }
  if(p.warp==3)SetupDrag(u8"\u5e27",&p.frame,1,0,60000);
  SetupSlider(u8"\u6b8b\u673a",&p.lives,0,8);
  SetupSlider(u8"Bomb",&p.bombs,0,8);
  SetupDrag(u8"\u5206\u6570",&p.score,10,0,99999990);
  SetupSlider(u8"\u706b\u529b",&p.power,1,128);
  SetupDrag(u8"\u64e6\u5f39",&p.graze,1,0,999);
  SetupDrag(u8"\u7d2f\u8ba1\u84dd\u70b9",&p.totalPoints,1,0,99999);
  SetupDrag(u8"\u5355\u5173\u84dd\u70b9",&p.points,1,0,999);
  SetupSlider(u8"\u68a6\u70b9",&p.dream,1,128);
  const int difficulty=p.stage==6?4:CGame::GVar().m_initDifficulty;
  SetupSlider("Rank",&p.rank,Config::RankMin(difficulty),Config::RankMax(difficulty));
  bool locked=p.lockRank!=0;ImGui::Checkbox(u8"\u9501Rank",&locked);
  if(ImGui::IsItemFocused()&&(ImGui::IsKeyPressed(VK_LEFT)||ImGui::IsKeyPressed(VK_RIGHT)))locked=!locked;
  p.lockRank=locked?1:0;
 }
 p.Clamp(p.stage==6?4:CGame::GVar().m_initDifficulty);
 ImGui::PopItemWidth();
 const bool valid=p.Valid(p.stage==6?4:CGame::GVar().m_initDifficulty);
 const bool unlocked=p.mode||CGame::GVar().PracticeStageUnlocked(p.stage);
 if(!valid)ImGui::TextColored(ImVec4(1,.5f,.5f,1),u8"\u53c2\u6570\u65e0\u6548");
 else if(!unlocked)ImGui::TextColored(ImVec4(1,.5f,.5f,1),u8"\u5b9e\u6218\u8fdb\u5165\u8be5\u5173\u5361\u540e\u89e3\u9501\u7ec3\u4e60");
 if(confirm&&valid&&unlocked)action=1;
 if(cancel)action=2;
 ImGui::End();ImGui::PopStyleVar(3);return action;
}
} }
