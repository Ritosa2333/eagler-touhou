#include "Practice.h"
#include "../Game.h"
#include "../Stage/Stage.h"
#include "../../third_party/imgui/imgui.h"
#include "../../third_party/imgui/imgui_internal.h"
#include "../../third_party/imgui/backends/imgui_impl_win32.h"
#include "../../third_party/imgui/backends/imgui_impl_opengl2.h"
#include <cstdio>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND,UINT,WPARAM,LPARAM);
namespace th5w { namespace practice {
namespace {
bool tools[ToolCount] = {}, assisted = false, initialized = false, focusPause = false;
bool overlayOpen = false,infoOpen=false,preparingSection=false;
unsigned int timeoutLockRevision=0;
float pauseAlpha = 0;
int pauseAge = 0, pauseAction = -1, actionAge = 0;
const char* keys[ToolCount] = {"F1", "F2", "F3", "F4", "F5", "F6", "F7", "U"};
const char* labels[ToolCount] = {
 u8"\u65e0\u654c", u8"\u9501\u6b8b/\u4e0d\u75ae", u8"\u9501\u68a6\u70b9", u8"\u9501Bomb", u8"\u9501\u706b\u529b", u8"\u9501\u65f6", u8"\u81ea\u52a8B", u8"\u654c\u4eba\u65e0\u654c"
};
const char* pauseLabels[] = {u8"\u7ee7\u7eed\u6e38\u620f", u8"\u9000\u51fa\u6e38\u620f", u8"\u4e0d\u5b58rep\u9000\u51fa\u6e38\u620f", u8"\u91cd\u65b0\u5f00\u59cb"};
bool Replay() { return CGame::s_pCurGame && (CGame::GVar().m_bReplayMode||original::DemoActive()); }
}
void PreparingSection(bool value){preparingSection=value;}
bool Enabled(Tool tool) { return tools[tool] && !Replay() && !preparingSection; }
void Toggle(Tool tool) { if (!Replay()) { tools[tool] = !tools[tool]; if(tool==LockTimeout)++timeoutLockRevision; if (tools[tool]) MarkAssisted(); } }
bool Assisted() { return assisted; }
bool InGameInfoOpen(){return infoOpen;}
void MarkAssisted() {
 if (Replay()) return;
 assisted = true;
 // Hotkey toggles are live tools, not serialized Practice parameters.
}
void ResetRun() { assisted = Options().showHitbox || Options().alwaysHitbox;for(int i=0;i<ToolCount;i++)if(tools[i])assisted=true; }
unsigned int TimeoutLockRevision(){return timeoutLockRevision;}
bool AllowTimeout(bool expired) { return expired && !Enabled(LockTimeout); }
bool BeforeTimeout(bool beforeDeadline) { return beforeDeadline || Enabled(LockTimeout); }
bool Initialize(HWND window) {
 IMGUI_CHECKVERSION(); ImGui::CreateContext();
 ImGuiIO& io = ImGui::GetIO(); io.IniFilename = NULL; io.LogFilename = NULL;
 io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
 ImGui::StyleColorsDark();
 char font[MAX_PATH]; GetWindowsDirectoryA(font,MAX_PATH);
 strcat_s(font,"\\Fonts\\msyh.ttc");
 static ImVector<ImWchar> glyphRanges;ImFontGlyphRangesBuilder ranges;
 ranges.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());ranges.AddText(u8"\u5e27\u9636\u6bb5\u4ece\u5173\u5361\u8d77\u59cb\u5904\u5f00\u59cb\uff08\u6ca1\u6709\u9053\u4e2d\uff09");ranges.AddText(u8"\u66f4\u591a\u6e38\u620f\u4fe1\u606f\u7075\u68a6\u9b54\u7406\u6c99\u9b45\u5e7d\u9999");ranges.AddText(AdvancedGlyphs());ranges.AddText(SectionGlyphs());ranges.AddText(u8"\u5bf9\u8bdd");ranges.BuildRanges(&glyphRanges);
 if (!io.Fonts->AddFontFromFileTTF(font,16,NULL,glyphRanges.Data)) io.Fonts->AddFontDefault();
 if (!ImGui_ImplWin32_Init(window)) { ImGui::DestroyContext(); return false; }
 if (!ImGui_ImplOpenGL2_Init()) { ImGui_ImplWin32_Shutdown(); ImGui::DestroyContext(); return false; }
 // Z activates the focused widget, matching the game's confirm key.
 io.KeyMap[ImGuiKey_Space]='Z';
 LoadAdvancedOptions();initialized=true; return true;
}
void Shutdown() { if (!initialized) return; SaveAdvancedOptions();ImGui_ImplOpenGL2_Shutdown(); ImGui_ImplWin32_Shutdown(); ImGui::DestroyContext(); initialized=false; }
void BeginFrame() { if (!initialized) return; ImGui_ImplOpenGL2_NewFrame(); ImGui_ImplWin32_NewFrame(); ImGui::NewFrame(); ImGui::GetIO().MouseDrawCursor=false; }
void EndFrame() { if (!initialized) return; ImGui::Render(); ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData()); }
void WindowMessage(HWND window,UINT message,WPARAM key,LPARAM data) {
 if (!initialized) return;
 ImGui_ImplWin32_WndProcHandler(window,message,key,data);
 AdvancedKeyEvent(message,key,data);
 if(message==WM_KEYDOWN&&key==VK_TAB&&CGame::s_pCurGame&&CGame::s_pCurGame->m_curSchemeID==4&&!AdvancedOpen()&&!ImGui::IsAnyItemActive()){
  if(!(data&(1L<<30)))infoOpen=!infoOpen;
  ImGui::GetIO().KeysDown[VK_TAB]=false;
 }
 if (message==WM_KEYDOWN && !(data & (1L<<30))) {
  if (key>=VK_F1 && key<=VK_F7) Toggle(Tool(key-VK_F1));
  else if (key=='U') Toggle(EnemyInvincible);
  else if (key==VK_BACK && !ImGui::IsAnyItemActive()) overlayOpen=!overlayOpen;
 }
 if (message==WM_KILLFOCUS) {
  ImGuiIO& io=ImGui::GetIO(); memset(io.KeysDown,0,sizeof(io.KeysDown)); memset(io.MouseDown,0,sizeof(io.MouseDown));
 }
}
// Layout follows purple thprac: TH06::THPauseMenu, THOverlay and
// GuiHotKey::OnWidgetUpdate (thprac_th06.cpp / thprac_gui_components.cpp).
// Only the renderer/backend and game-state actions are adapted for uth05win.
void FocusPause() { focusPause=true; pauseAge=0; pauseAlpha=0; pauseAction=-1; actionAge=0; }
void DrawPause(CStage& stage) {
 if (!initialized||AdvancedOpen()) return;
 ImGuiIO& io=ImGui::GetIO(); io.MouseDrawCursor=true;
 ++pauseAge;
 if (pauseAction<0) pauseAlpha=ImMin(.8f,pauseAlpha+.1f);
 else pauseAlpha=ImMax(0.f,pauseAlpha-.1f);
 if (pauseAction>=0 && ++actionAge>=10) {
  if (pauseAction==0) stage.PracticeResume();
  else stage.PracticeExit(pauseAction);
  return;
 }
 // THPauseMenu: 384x448 at (32,16), borderless, square corners,
 // 140px top spacer, 119px indent and 130x25 buttons.
 ImGui::SetNextWindowPos(ImVec2(32,16),ImGuiCond_Always);
 ImGui::SetNextWindowSize(ImVec2(384,448),ImGuiCond_Always);
 ImGui::SetNextWindowBgAlpha(pauseAlpha);
 ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0.f);
 ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0.f);
 ImGui::Begin("Pause Menu",NULL,ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|
  ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoBringToFrontOnFocus|ImGuiWindowFlags_NoSavedSettings);
 if (focusPause) ImGui::SetWindowFocus();
 if (pauseAlpha>=.8f) {
  if (pauseAge>10 && pauseAction<0) {
   if (ImGui::IsKeyPressed(VK_ESCAPE,false)) pauseAction=0;
   else if (ImGui::IsKeyPressed('Q',false)) pauseAction=2;
   else if (ImGui::IsKeyPressed('R',false)) pauseAction=3;
  }
  ImGui::Dummy(ImVec2(10,140));
  ImGui::Indent(119);
  for (int i=0;i<4;i++) {
   bool pressed=ImGui::Button(pauseLabels[i],ImVec2(130,25));
   // GuiButton explicitly accepts the game's confirm key, including after
   // mouse use (Dear ImGui's navigation highlight can then be disabled).
   if (ImGui::IsItemFocused() && (ImGui::IsKeyPressed('Z',false)||ImGui::IsKeyPressed(VK_RETURN,false))) pressed=true;
   if (pressed && pauseAction<0) pauseAction=i;
   if (focusPause && i==0) {
    ImGui::FocusWindow(ImGui::GetCurrentWindow());
    ImGui::SetFocusID(ImGui::GetItemID(),ImGui::GetCurrentWindow());
    GImGui->NavDisableHighlight=false;
   }
   ImGui::Spacing();
  }
  focusPause=false;
  ImGui::Unindent(119);
 }
 ImGui::End();
 ImGui::PopStyleVar(2);
}
void DrawStatus() {
 if (!initialized || !CGame::s_pCurGame || original::DemoActive()) return;
 if (CGame::s_pCurGame->m_curSchemeID==4 && !Replay())
  for (int i=0;i<ToolCount;i++) if (tools[i]) { MarkAssisted(); break; }
 if(CGame::s_pCurGame->m_curSchemeID==4 && !Replay() && (Options().showHitbox||Options().alwaysHitbox)) MarkAssisted();
 if(AdvancedOpen()){DrawAdvanced();return;}
 DrawRuntimeHUD();
 if (!overlayOpen) {DrawAdvanced();return;}
 ImGui::GetIO().MouseDrawCursor=true;
 ImGui::SetNextWindowPos(ImVec2(10,10),ImGuiCond_Always);
 ImGui::SetNextWindowBgAlpha(.5f);
 ImGui::Begin("Mod Menu",NULL,ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|
  ImGuiWindowFlags_NoMove|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoSavedSettings|
  ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav);
 const Tool order[]={Invincible,InfiniteLives,LockDream,InfiniteBombs,KeepPower,LockTimeout,AutoBomb,EnemyInvincible};
 for (Tool tool:order) {
  const int i=int(tool);
  char text[128]; const bool enabled=Enabled(Tool(i));
  if(keys[i][0])sprintf_s(text,enabled?"[%s: %s]":"%s: %s",keys[i],labels[i]);
  else sprintf_s(text,enabled?"[%s]":"%s",labels[i]);
  if (i==InfiniteLives&&Options().noContinue)sprintf_s(text,enabled?"[F2: %s]":"F2: %s",u8"\u4e0d\u75ae");
  if (enabled) ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(0,1,0,1));
  const ImVec2 cursor=ImGui::GetCursorPos();
  ImGui::TextUnformatted(text);
  ImGui::SetCursorPos(cursor);
  if (enabled) ImGui::PopStyleColor();
  ImGui::PushID(i);
  if (ImGui::InvisibleButton("##tool",ImGui::CalcTextSize(text))) Toggle(Tool(i));
  ImGui::PopID();
 }
 char infoText[128];sprintf_s(infoText,infoOpen?"[Tab: %s]":"Tab: %s",u8"\u66f4\u591a\u6e38\u620f\u4fe1\u606f");
 if(infoOpen)ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(0,1,0,1));
 const ImVec2 infoPos=ImGui::GetCursorPos();ImGui::TextUnformatted(infoText);ImGui::SetCursorPos(infoPos);
 if(infoOpen)ImGui::PopStyleColor();
 if(ImGui::InvisibleButton("Tab",ImGui::CalcTextSize(infoText)))infoOpen=!infoOpen;
 ImGui::End();DrawAdvanced();
}
} }
