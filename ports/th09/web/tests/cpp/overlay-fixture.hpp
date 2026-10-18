#pragma once
#include "../../cpp/game/AsciiRenderer.hpp"
#include "../../cpp/game/InGameMenu.hpp"
namespace overlay_test {
using namespace th09;
struct Fixture:ResourceReader,ResourceTextures,AsciiOutput,InGameMenuServices {
    std::vector<u8> bytes;Rng random{0x7521,0,0};AnmExecutor executor{random};GameResources resources{*this,*this,executor};
    AsciiRenderer ascii{resources,*this};InGameMenus menus{resources,*this};PopupFrame frame;
    std::vector<AnmVm> drawings;std::vector<i32> events;u32 textures=0;
    bool read(const char*,std::vector<u8>& out)override{out=bytes;return true;}
    TextureAllocation create(const AnmTextureSource& s,const u8*,u32)override{return {++textures,s.width,s.height};}
    void destroy(u32)override{}
    void ascii_view(i32 side)override{events.push_back(100+side);}
    void ascii_sprite(AnmVm& a)override{drawings.push_back(a);}
    void menu_sound(i32 id)override{events.push_back(200+id);}
    void menu_action(InGameAction a)override{events.push_back(300+i32(a));}
    void menu_view()override{events.push_back(102);}
    void menu_sprite(AnmVm& a)override{drawings.push_back(a);}
    bool initialize(const u8* p,u32 size){bytes.assign(p,p+size);return resources.load(AnimationFile::ascii,"ascii.anm")&&ascii.initialize();}
    void clear(){drawings.clear();events.clear();}
    i32 menu_step(i32 type,u16 keys){InputFrame input;input.pressed=keys;clear();if(type==0){menus.update_pause(input);return 0;}return type==1?menus.update_game_over(input):menus.update_match_end(input);}
    void configure(i32 mode,i32 difficulty,i32 continued,u32 flags,u32 capture){this->mode=GameMode(mode);this->difficulty=difficulty;continues=continued;game_flags=flags;capture_enabled=capture;}
};
}
