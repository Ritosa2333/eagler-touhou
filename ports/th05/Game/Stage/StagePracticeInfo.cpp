// Layout/content ported from purple thprac TH10InGameInfo (revision 17780056).
// Compact spacing replaces the difficulty row without covering Power or FPS.
#include "Stage.h"
#include "../Game.h"
#include "../Practice/Practice.h"
#include "../../third_party/imgui/imgui.h"
#include <cstdio>

namespace th5w {
void CStage::DrawPracticeInfo(){
 const int flags=ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse|
  ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|
  ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoInputs|
  ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
 ImGui::SetNextWindowPos(ImVec2(432,408),ImGuiCond_Always);
 ImGui::SetNextWindowSize(ImVec2(192,48),ImGuiCond_Always);
 ImGui::SetNextWindowBgAlpha(.9f);
 ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(8,0));
 ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,ImVec2(8,0));
 ImGui::Begin("TH05 In-Game Info",NULL,flags);
 const char* difficulty[]={"Easy","Normal","Hard","Lunatic","Extra"};
 const char* character[]={u8"\u7075\u68a6",u8"\u9b54\u7406\u6c99",u8"\u9b45\u9b54",u8"\u5e7d\u9999"};
 char heading[80];sprintf_s(heading,"%s (%s)",difficulty[practice::Config::Bound(CGame::GVar().m_playDifficulty,0,4)],character[practice::Config::Bound(CGame::GVar().m_playChara,0,3)]);
 ImGui::SetCursorPosX((ImGui::GetWindowSize().x-ImGui::CalcTextSize(heading).x)*.5f);
 ImGui::TextUnformatted(heading);
 ImGui::Columns(2);
 ImGui::TextUnformatted("miss:");ImGui::NextColumn();ImGui::Text("%8d",original::Stats().misses);ImGui::NextColumn();
 ImGui::TextUnformatted("bomb:");ImGui::NextColumn();ImGui::Text("%8d",original::Stats().bombs);
 ImGui::Columns(1);ImGui::End();ImGui::PopStyleVar(2);
}
}
