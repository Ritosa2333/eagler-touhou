#pragma once
#include "MenuCursor.hpp"
#include "AnmManager.hpp"
namespace th15 {
struct ManualServices {virtual ~ManualServices()=default;virtual bool sound(i32)=0;virtual bool clear_page()=0;virtual bool request_page(i32)=0;virtual bool upload_page(i32)=0;};
class Manual {
 AnmManager& animations;ManualServices& services;i32 bank;bool fail(const std::string&);bool create(i32,u32&);bool list();bool retire_list();bool request();bool sound(i32);bool interrupt(u32,i32,bool);
public:
 MenuCursor menu;Timer age{0,0,0,0,0};i32 mode=0,phase=0;float offset_x=128;std::array<u32,9> choices{};u32 page=0;bool finished=false;std::string error;
 Manual(AnmManager& a,ManualServices& s,i32 bank=19):animations(a),services(s),bank(bank){age.set(0);menu.count=0;menu.wrapping=false;}
 bool page_loaded();bool update(u32 pressed,u32 repeated,float rate=1);
};
}
