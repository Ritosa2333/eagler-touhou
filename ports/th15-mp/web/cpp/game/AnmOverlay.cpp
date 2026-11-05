#include "AnmOverlay.hpp"
namespace th15 {
AnmOverlay::AnmOverlay(const AnmOverlay& source){*this=source;}
AnmOverlay& AnmOverlay::operator=(const AnmOverlay& source){
 if(this==&source)return *this;
 mode=source.mode;frames=source.frames;
 for(u32 i=0;i<panels.size();++i){
  if(!source.panels[i])panels[i].reset();
  else if(panels[i])*panels[i]=*source.panels[i];
  else panels[i]=std::make_unique<AnmVm>(*source.panels[i]);
 }
 return *this;
}
}
