#pragma once
#include "TitleAnimations.hpp"
#include "TitleCheat.hpp"
#include "TitleReplayMenu.hpp"
#include "Dialogue.hpp"
namespace th15 {
struct TitlePlayerDataServices {virtual ~TitlePlayerDataServices()=default;virtual bool sound(i32)=0;virtual bool text(AnmVm&,const DialogueText&)=0;virtual bool keyboard(TitleKeyboard&)=0;};
class TitlePlayerData {
 TitleState& state;RecordStore& records;AnmManager& animations;TitleAnimations visuals;TitlePlayerDataServices& host;i32 text_bank;
 bool check(bool,const char*);bool visual(bool);bool paint();bool bitmap(i32,const std::string&,u32);i32 spell_count()const noexcept;
public:
 MenuCursor ranks,pages;TitleCheat cheat;i32 displayed_spells=0;std::string error;
 TitlePlayerData(TitleState& s,RecordStore& r,AnmManager& a,TitlePlayerDataServices& h,i32 title=16,i32 ascii=5,i32 text=0):state(s),records(r),animations(a),visuals(s,a,title,ascii),host(h),text_bank(text),cheat(r){}
 bool update(u32 pressed,u32 repeated);bool draw(HudDrawServices&,ReplayCalendarServices&);
};
}
