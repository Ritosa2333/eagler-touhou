#pragma once
#include "Hud.hpp"
namespace th11 {
#if PORT_TITLE == 14
bool th14_hud_counters(AsciiText&,const GameEconomy&,const HudScore&,u8 alpha);
void th14_grouped_number(char (&out)[64],u32 value);
void th14_score_number(char (&out)[64],u32 value,i32 continues);
struct Th14HudAwards {bool result=false,time=false;i32 bonus=0,frames=0,encoded=0;u8 result_alpha=255,time_alpha=255;};
bool th14_hud_awards(AsciiText&,const Th14HudAwards&);
#endif
}
