#include "../selection.hpp"
bool character_extra[2];bool complete[5][2];int profiles[4][2];
void signal_event(int index,int id,bool execute){event(41,index);event(42,id);event(43,execute);}
void child_event(int index,int child,int id,bool execute){event(44,index);event(45,child);event(46,id);event(47,execute);}
void visible_event(int index,int child,bool show){event(48,index);event(49,child);event(50,show);}
void weapon_event(int slot,int character,int profile){event(53,slot);event(54,character);event(55,profile);}
struct SelectionHost final:ti::SelectionEnvironment {
 SelectionHost(Host& h):SelectionEnvironment(h,at<unsigned>(0x5c6128),at<int>(0x5c6144)){}
 void spawn_text_overlay(ti::TitleInf& o)override{event(40);o.handle390=4001;}
 void signal(ti::TitleInf&,int i,int e,bool execute)override{signal_event(i,e,execute);}
 void child_visible(ti::TitleInf&,int i,int child,bool show)override{visible_event(i,child,show);}
 void child_signal(ti::TitleInf&,int i,int child,int e,bool execute)override{child_event(i,child,e,execute);}
 bool all_cleared(int d,int c)override{return c<0?complete[d][0]&&complete[d][1]:complete[d][c];}
 bool character_extra_unlocked(int c)override{return character_extra[c];}
 int selected_profile(int slot,int character)override{return profiles[slot][character];}
 void commit_progress()override{event(52);}
 void set_weapon(int slot,int c,int p)override{weapon_event(slot,c,p);}
};
void __fastcall signal_only(ti::TitleInf*,void*,int index,int id){signal_event(index,id,false);}
void __fastcall signal_execute(ti::TitleInf*,void*,int index,int id){signal_event(index,id,true);}
unsigned* __fastcall child_lookup(ti::TitleInf*,void*,unsigned* out,int index,int child){*out=(index<<16)|(child&0xffff);return out;}
void __fastcall child_interrupt(unsigned* h,void*,int e){child_event(*h>>16,*h&0xffff,e,false);}
void __fastcall child_execute(unsigned* h,void*,int e){child_event(*h>>16,*h&0xffff,e,true);}
void __fastcall hide_child(unsigned* h,void*){visible_event(*h>>16,*h&0xffff,false);}
void __fastcall show_child(unsigned* h,void*){visible_event(*h>>16,*h&0xffff,true);}
int __fastcall extra_character(void*,void*,int c){return character_extra[c];}
int __fastcall cleared_character(void*,void*,int d,int c){return complete[d][c];}
int __fastcall cleared_difficulty(void*,void*,int d){return complete[d][0]&&complete[d][1];}
int __fastcall selected_profile(void*,void*,int slot,int character){return profiles[slot][character];}
int __fastcall commit_progress(void*,void*){event(52);return 0;}
void __fastcall main_weapon(void*,void*,int c,int p){weapon_event(0,c,p);}
void __fastcall unfocused_weapon(void*,void*,int c,int p){weapon_event(1,c,p);}
void __fastcall focused_weapon(void*,void*,int c,int p){weapon_event(2,c,p);}
void __fastcall passive_weapon(void*,void*,int c,int p){weapon_event(3,c,p);}
