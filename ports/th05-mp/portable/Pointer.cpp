#include "Platform.hpp"
#include "PracticeMobile.hpp"
#include "../Game/Practice/Practice.h"
#include "../third_party/imgui/imgui_internal.h"
#include <eagler/thprac/PracticePointer.hpp>
#include <deque>
namespace {
eagler::thprac::PracticePointer pointers;
struct MouseEdge {float x,y;bool down;};
std::deque<MouseEdge> edges;
bool Hit(float x,float y) {
 if(!ImGui::GetCurrentContext())return false;
 for(auto* window:ImGui::GetCurrentContext()->Windows)
  if(!window->IsFallbackWindow&&window->Active&&!window->Hidden&&!(window->Flags&ImGuiWindowFlags_NoMouseInputs)&&window->OuterRectClipped.Contains({x,y}))return true;
 return false;
}
void Queue(float x,float y,bool down){if(edges.size()<64)edges.push_back({x,y,down});}
}
namespace th5w::web {
void BeginPointerFrame(){if(edges.empty()||!ImGui::GetCurrentContext())return;const auto edge=edges.front();edges.pop_front();auto& io=ImGui::GetIO();io.MousePos={edge.x,edge.y};io.MouseDown[0]=edge.down;}
}
extern "C" {
EMSCRIPTEN_KEEPALIVE int th05_mobile_state(){using namespace eagler::thprac::mobile;poll();return int(enabled)|(int(visible)<<1)|(int(expanded[Cheats])<<2)|(int(expanded[Advanced])<<3)|(int(expanded[Information])<<4);}
EMSCRIPTEN_KEEPALIVE void th05_pointer_cancel(){pointers.reset();edges.clear();if(ImGui::GetCurrentContext())ImGui::GetIO().MouseDown[0]=false;}
EMSCRIPTEN_KEEPALIVE int th05_pointer(int type,int id,double x,double y,int mouse){
 if(!ImGui::GetCurrentContext()||type<0||type>2)return 0;
 if(mouse){auto& io=ImGui::GetIO();io.MousePos={float(x),float(y)};if(type!=1)Queue(x,y,type==0);return Hit(x,y);}
 const bool passive=th5w::practice::Available()&&eagler::thprac::mobile::passive_pointer(x,y);
 const auto decision=pointers.route_touch(type,id,Hit(x,y),passive,x,y);
 using Route=eagler::thprac::PracticePointer::TouchRoute;
 if(decision.route==Route::Ui){if(type==1)ImGui::GetIO().MousePos={float(x),float(y)};else Queue(x,y,type==0);return 1;}
 if(decision.route==Route::Pending)return 2;
 if(decision.route==Route::BeginGame)return 3;
 if(decision.route==Route::Tap){Queue(decision.x,decision.y,true);Queue(x,y,false);return 4;}
 return 0;
}
}
