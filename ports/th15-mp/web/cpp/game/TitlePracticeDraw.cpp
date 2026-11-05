#include "TitlePracticeDraw.hpp"
#include <cstdio>
namespace th15 {
bool TitlePracticeDraw::draw(HudDrawServices& sink){
 if(!error.empty())return false;if(state.substate!=2&&state.substate!=3)return true;if(state.age.current<10&&state.substate!=3)return true;
 const i32 character=progress.character+progress.subcharacter,rank=progress.difficulty;if(character<0||character>=5||rank<0||rank>=5){error="Practice score selection outside original range";return false;}
 constexpr const char* titles[]={"Stage 1","Stage 2","Stage 3","Stage 4","Stage 5","Stage 6"};HudTextDraw text;text.style.font=0;text.style.shadow=true;
 for(i32 row=0;row<6;row++){
  const auto& entry=records.characters[character].practice[rank][row+1];const bool visited=entry.visited!=0;text.position={320,float(192+row*18),0};text.style.color=state.menu.cursor==row?(visited?(state.substate==3&&state.age.current%4>=2?0xff000000:0xffffff00):0xffdfdfdf):0xff808080;
  char line[64];if(visited)std::snprintf(line,sizeof line,"%s  %.8d0",titles[row],entry.score);else std::snprintf(line,sizeof line,"%s  ---------",titles[row]);text.text=line;if(!sink.hud_text(text)){error="Practice score text submission failed";return false;}
 }return true;
}
}
