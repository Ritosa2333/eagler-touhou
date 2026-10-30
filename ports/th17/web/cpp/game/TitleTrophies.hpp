#pragma once
#include "TitleSpellPractice.hpp"
#include "TitleCheat.hpp"
namespace th17 {
struct TrophyEntry {bool present=false;std::string title;std::array<std::array<std::string,3>,2> descriptions;};
class TrophyCatalog {
 std::array<TrophyEntry,40> entries;
public:
 std::string error;bool open(const u8*,u32,Rng* visual=nullptr);const TrophyEntry* entry(i32 n)const noexcept{return n>=0&&n<40&&entries[n].present?&entries[n]:nullptr;}
};
struct TitleTrophyServices:TitleSpellServices {virtual bool keyboard(TitleKeyboard&)=0;virtual bool preview_ending(i32)=0;virtual bool trophy_stage(i32 character,i32 animal)=0;};
class TitleTrophies {
 TitleState& state;RecordStore& records;const TrophyCatalog& catalog;AnmManager& animations;TitleAnimations visuals;TitleTrophyServices& host;
 bool check(bool,const char*);bool visual(bool);bool text(i32,const std::string&);bool description();u32 transition=0;
public:
 TitleCheat cheat;i32 saved_selection=0,description_age=0,description_index=0;std::string error;
 TitleTrophies(TitleState& s,RecordStore& r,const TrophyCatalog& data,AnmManager& a,TitleTrophyServices& h,i32 title=16,i32 ascii=0):state(s),records(r),catalog(data),animations(a),visuals(s,a,title,ascii),host(h),cheat(r){}
 bool update(u32 pressed,u32 repeated);bool draw(HudDrawServices&);
};
}
