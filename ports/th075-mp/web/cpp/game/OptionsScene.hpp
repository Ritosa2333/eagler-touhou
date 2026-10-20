#pragma once
#include "TitleScene.hpp"
#include "GameSettings.hpp"
#include "ImageRenderer.hpp"
namespace th075 {
struct MenuInput {std::array<i32,2> horizontal{},vertical{},confirm{},cancel{};bool escape=false;};
struct OptionsSignals:SceneSignals {virtual void render_options()=0;virtual void present_options()=0;virtual void description(u8)=0;virtual void music_volume(u8)=0;virtual void sound_volume(u8)=0;virtual void start_music(i32)=0;virtual void save_settings()=0;};
class OptionsScene {
public:
 i16 selected=0,soundWait=0;i8 horizontal=0,vertical=0,confirm=0,cancel=0;u8 alpha=5;
 void initialize(){selected=soundWait=0;alpha=5;}
 void poll(const MenuInput&);
 u16 advance(GameSettings&,OptionsSignals&);
 void render(ImageRenderer&,u32 description,const GameSettings&)const;
};
}
