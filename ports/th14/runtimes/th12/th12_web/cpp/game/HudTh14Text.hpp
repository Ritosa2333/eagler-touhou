#pragma once
#include "Hud.hpp"
namespace th11 {
#if PORT_TITLE == 14
bool th14_hud_counters(AsciiText&,const GameEconomy&,const HudScore&,u8 alpha);
struct Th14HudAwards {bool result=false,time=false;i32 bonus=0,frames=0,encoded=0;u8 result_alpha=255,time_alpha=255;};
bool th14_hud_awards(AsciiText&,const Th14HudAwards&);
#endif
}
