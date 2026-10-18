#include "AnmOverlay.hpp"
namespace th15 {
AnmOverlay::AnmOverlay(const AnmOverlay& source):mode(source.mode),frames(source.frames){for(u32 i=0;i<panels.size();i++)if(source.panels[i])panels[i]=std::make_unique<AnmVm>(*source.panels[i]);}
}
