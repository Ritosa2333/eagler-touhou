#pragma once
#include "TitleAnimations.hpp"
#include "TitleCheat.hpp"
#include "HudDrawData.hpp"
#include "ReplayCalendar.hpp"
#include "TextLayout.hpp"
namespace th17 {
struct TitlePlayerDataServices {virtual ~TitlePlayerDataServices()=default;virtual bool sound(i32)=0;virtual bool text(AnmVm&,const TextureText&)=0;virtual bool keyboard(TitleKeyboard&)=0;};
class TitlePlayerData {
 TitleState& state;RecordStore& records;AnmManager& animations;TitleAnimations visuals;TitlePlayerDataServices& host;i32 text_bank;
 bool check(bool,const char*);bool visual(bool);bool paint();bool bitmap(i32,const std::string&,u32);bool entry(u32,u32);bool details(u32,u32);
public:
 MenuCursor ranks,pages;TitleCheat cheat;i32 kind=0,displayed_spells=0;std::string error;
 TitlePlayerData(TitleState& s,RecordStore& r,AnmManager& a,TitlePlayerDataServices& h,i32 title=16,i32 ascii=0,i32 text=6):state(s),records(r),animations(a),visuals(s,a,title,ascii),host(h),text_bank(text),cheat(r){}
 bool update(u32 pressed,u32 repeated,i32 difficulty=1);bool draw(HudDrawServices&,ReplayCalendarServices&);
 i32 game_difficulty=1;
};
}
