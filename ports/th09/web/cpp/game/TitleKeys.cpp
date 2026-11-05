#include "TitleMenus.hpp"
#include "Localization.hpp"
namespace th09 {
namespace {
#include "TitleData.inc"
bool devices_conflict(u8 a,u8 b){if(a==b)return true;return a>=2&&b>=2&&(a==2||b==2);}
}
void TitleMenus::key_value(i32 animation,i32 value){auto& a=animations[animation];resources.sprite(AnimationFile::menu,a,a.baseSpriteIndex+value*2);a.pendingInterrupt=7;}
void TitleMenus::key_numbers(){
    constexpr i32 buttons[]={0,1,2,8,3};const auto& keys=edited_bindings[state.key_player].gamepad;
    for(i32 n=0;n<5;++n){auto& a=animations[50+n*2];auto& b=animations[51+n*2];const i16 value=keys[buttons[n]];a.flag1=b.flag1=value>=0;if(value>=0){resources.sprite(AnimationFile::menu,a,a.baseSpriteIndex+(value/10)*2);resources.sprite(AnimationFile::menu,b,b.baseSpriteIndex+(value%10)*2);}}
}
void TitleMenus::key_enabled(){const u32 color=settings.devices[state.key_player]<2?0xffffffffu:0xff606060u;for(i32 n=40;n<45;++n)animations[n].color1.d3dColor=color;for(i32 n=50;n<60;++n)animations[n].color1.d3dColor=color;}
bool TitleMenus::key_config(){
    auto& s=state;const auto& in=input[2];
    if(!s.state){if(!s.load_frame){interrupt(4);advance();s.selection_base=37;s.selection_count=10;select(s.selection,37,10);s.state=s.frames=0;s.layout_changed=1;edited_bindings=settings.bindings;key_value(47,s.key_player);key_value(48,settings.devices[s.key_player]);key_value(49,settings.auto_focus[s.key_player]);key_numbers();s.previous_selection=-1;}s.state=1;
        constexpr const char* ids[]={"th09 Key Side","th09 Key Control Type","th09 Key Charge Type","th09 Key Shot","th09 Key Bomb","th09 Key Slow","th07 Key Skip","th07 Key Pause","th07 Key Reset","th07 Key Quit"};
        for(i32 n=0;n<10;++n)output.title_text(descriptions[n],Localization::StringById(ids[n],key_help[n]),0xfff0e0,0x300000);}
    else if(s.state!=1){tick();return true;}
    const i32 direction=navigate(s.selection,10,in);if(direction){if(settings.devices[s.key_player]>1)while(s.selection>2&&s.selection<8)s.selection+=direction;select(s.selection,37,10);}
    if(s.previous_selection!=s.selection){s.description=s.selection;descriptions[s.selection].pendingInterrupt=1;}s.previous_selection=s.selection;key_numbers();
    i32 button=settings.previous_button;if(settings.devices[s.key_player]<2){button=output.title_joy_button(settings.devices[s.key_player]);if(button>=0&&button<32&&button!=settings.previous_button&&s.selection>=3&&s.selection<=7){constexpr i32 map[]={0,1,2,8,3};auto& keys=edited_bindings[s.key_player].gamepad;const i32 target=map[s.selection-3];const i16 previous=keys[target];for(auto& key:keys)if(key==button)key=previous;keys[target]=i16(button);output.title_sound(10);}}settings.previous_button=button;
    const auto change_value=[&](i32 step){if(s.selection==0){s.key_player=1-s.key_player;key_value(47,s.key_player);key_value(49,settings.auto_focus[s.key_player]);key_value(48,settings.devices[s.key_player]);key_value(42,settings.auto_focus[s.key_player]);key_enabled();}
        else if(s.selection==1){auto& device=settings.devices[s.key_player];do{device=u8((i32(device)+step+5)%5);}while(devices_conflict(device,settings.devices[1-s.key_player]));key_value(48,device);key_enabled();}
        else if(s.selection==2){auto& focus=settings.auto_focus[s.key_player];focus=u8((i32(focus)+step+2)%2);key_value(49,focus);key_value(42,focus);}else return;output.title_sound(12);};
    if((in.pressed|in.repeat)&64)change_value(-1);if((in.pressed|in.repeat)&128)change_value(1);
    if(in.pressed&0x1001){if(s.selection==8){output.title_sound(10);edited_bindings=settings.bindings;settings.auto_focus[s.key_player]=0;settings.devices[s.key_player]=u8(s.key_player);key_value(47,s.key_player);key_value(49,settings.auto_focus[s.key_player]);key_value(48,settings.devices[s.key_player]);key_value(42,settings.auto_focus[s.key_player]);key_enabled();}
        else if(s.selection==9){output.title_sound(11);change(TitleScreen::options);settings.bindings=edited_bindings;s.selection=7;return true;}}
    tick();return true;
}
}
