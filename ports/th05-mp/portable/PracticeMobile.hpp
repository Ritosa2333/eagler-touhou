#pragma once
#include "Localization.hpp"
#include "../third_party/imgui/imgui.h"
#ifdef __EMSCRIPTEN__
#include <eagler/thprac/PracticeMobilePanels.hpp>
#endif
namespace th5w { namespace mobile {
enum Panel { Cheats, Advanced, Information, Keyboard, Indicators, Result };
inline bool Enabled() {
#ifdef __EMSCRIPTEN__
 eagler::thprac::mobile::poll();return eagler::thprac::mobile::enabled;
#else
 return false;
#endif
}
inline bool Show(Panel panel,bool desktop) {
#ifdef __EMSCRIPTEN__
 return eagler::thprac::mobile::show(static_cast<eagler::thprac::mobile::Panel>(panel),desktop);
#else
 return desktop;
#endif
}
inline bool CapturesAdvanced(bool desktop) {
#ifdef __EMSCRIPTEN__
 return eagler::thprac::mobile::captures_advanced(desktop);
#else
 return desktop;
#endif
}
inline bool Begin(Panel panel,const char* id,ImGuiWindowFlags flags) {
#ifdef __EMSCRIPTEN__
 const auto& language=locale::PracticeLanguage();
 return eagler::thprac::mobile::begin(static_cast<eagler::thprac::mobile::Panel>(panel),id,flags,language=="en-US"?1:language=="ja-JP"?2:0);
#else
 return ImGui::Begin(id,nullptr,flags);
#endif
}
} }
