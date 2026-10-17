#pragma once
#include "TitleAnimations.hpp"
namespace th15 {
struct TitleControllerSettings {std::array<i16,10> values{};};
struct TitleControllerServices {virtual ~TitleControllerServices()=default;virtual bool sound(i32)=0;virtual bool save(const TitleControllerSettings&)=0;};
class TitleControllerMenu {
 TitleState& state;AnmManager& animations;TitleAnimations visuals;TitleControllerSettings& settings;TitleControllerServices& services;
 bool check(bool);bool sound(i32);bool paint();bool refresh();bool close();void restore();
public:
 std::array<i16,6> pending{};std::string error;
 TitleControllerMenu(TitleState& s,AnmManager& a,TitleControllerSettings& settings,TitleControllerServices& services,i32 title=16):state(s),animations(a),visuals(s,a,title),settings(settings),services(services){}
 bool bind(i32 action,i32 button);
 bool update(u32 pressed,u32 repeated,u32 gamepad_buttons);
};
}
