#pragma once
#include "TitleAnimalMenu.hpp"
#include "RecordStore.hpp"
#include "TextLayout.hpp"
#include "HudDrawData.hpp"
namespace th17 {
struct TitleSpellServices:TitleGameStartServices {virtual bool text(AnmVm&,const TextureText&)=0;};
struct TitleSpellMemory {i32 stage=-1,group=-1,rank=-1,last_stage=0;};
class TitleSpellPractice {
 TitleState& state;GameSelection& selection;TitleSelectionSettings& settings;RecordStore& records;AnmManager& animations;TitleAnimations visuals;TitleSpellServices& host;TitleSpellMemory& saved;i32 title_bank;
 bool check(bool,const char*);bool visual(bool);bool paint(i32);bool paint_selection();bool text(i32,const std::string&,u32);bool horizontal(u32);bool stage_menu(u32,u32);bool group_menu(u32,u32);bool rank_menu(u32,u32);bool enter_groups();bool enter_ranks();u32 transition=0;
public:
 i32 stage=0,group=0,rank=0;bool returning=false;std::array<i32,5> spells{{-2,-2,-2,-2,-2}};std::string error;
 TitleSpellPractice(TitleState& s,GameSelection& p,TitleSelectionSettings& st,RecordStore& r,AnmManager& a,TitleSpellServices& h,TitleSpellMemory& memory,i32 title=16,i32 ascii=0):state(s),selection(p),settings(st),records(r),animations(a),visuals(s,a,title,ascii),host(h),saved(memory),title_bank(title){}
 bool update(u32 pressed,u32 repeated);bool draw(HudDrawServices&);
};
}
