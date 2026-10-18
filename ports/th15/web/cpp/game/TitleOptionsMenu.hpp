#pragma once
#include "TitleAnimations.hpp"
namespace th15 {
struct TitleAudioSettings {i32 music_volume=100,sound_volume=80;u8 controller_option=0;};
struct TitleOptionsServices {
 virtual ~TitleOptionsServices()=default;
 virtual bool sound(i32)=0;
 virtual bool volume(i32 music,i32 sound,i32 sound_attenuation)=0;
};
class TitleOptionsMenu {
 TitleState& state;TitleAudioSettings& settings;AnmManager& animations;TitleAnimations visuals;TitleOptionsServices& services;
 AnmVm* child(i32);bool check(bool);bool paint();bool refresh();bool visibility(i32,bool);bool sprite(i32,i32);bool sound(i32);
public:
 std::string error;
 TitleOptionsMenu(TitleState& s,TitleAudioSettings& settings,AnmManager& a,TitleOptionsServices& services,i32 title=16):state(s),settings(settings),animations(a),visuals(s,a,title),services(services){}
 bool update(u32 pressed,u32 repeated);
};
}
