#include "TitleMenus.hpp"
#include "Localization.hpp"
#include <algorithm>
namespace th09 {
namespace {
#include "TitleData.inc"
}
bool TitleMenus::options(){
    auto& s=state;const auto& in=input[2];
    if(!s.state){if(!s.load_frame){interrupt(3);advance();s.state=s.frames=0;s.previous_selection=-1;s.selection_base=9;s.selection_count=9;select(s.selection,9,9);s.layout_changed=1;}s.state=1;
        constexpr const char* ids[]={"th07 Option Player","th07 Option Graphic","th09 Option BGM","th08 Option BGM Volume","th08 Option SE Volume","th07 Option Window Mode","th07 Option Reset","th09 Option Key Config","th07 Option Quit"};
        for(i32 n=0;n<9;++n)output.title_text(descriptions[n],Localization::StringById(ids[n],options_help[n]),0xfff0e0,0x300000);}
    else if(s.state!=1){tick();return true;}
    const auto digit=[&](i32 n,i32 number,bool enabled){auto& a=animations[n];if(!enabled){a.color1.a=0;a.flag1=false;}else{resources.sprite(AnimationFile::menu,a,a.baseSpriteIndex+number*2);a.color1.a=255;a.flag1=true;}};
    digit(26,0,settings.music_volume>=100);digit(27,(settings.music_volume/10)%10,settings.music_volume>=10);
    resources.sprite(AnimationFile::menu,animations[28],animations[28].baseSpriteIndex+(settings.music_volume%10)*2);resources.sprite(AnimationFile::menu,animations[29],animations[29].baseSpriteIndex);
    digit(30,0,settings.sound_volume>=100);digit(31,(settings.sound_volume/10)%10,settings.sound_volume>=10);
    if(navigate(s.selection,9,in))select(s.selection,9,9);if(s.previous_selection!=s.selection){s.description=s.selection;descriptions[s.description].pendingInterrupt=1;}s.previous_selection=s.selection;
    if(s.load_frame<4){tick();return true;}
    const auto cycle=[&](i32 direction){u8* value=nullptr;i32 base=0,count=0;switch(s.selection){case 0:value=&settings.extra_lives;base=18;count=3;break;case 1:value=&settings.windowed;base=21;count=2;break;case 2:value=&settings.music_mode;base=23;count=2;break;case 5:value=&settings.screen_mode;base=34;count=2;break;default:return;}
        output.title_sound(12);if(s.selection==2)output.title_music(-1);*value=u8((i32(*value)+direction+count)%count);if(s.selection==2){output.title_music(0);output.title_music(0);}select(*value,base,s.selection==2?3:count);output.title_sound(12);
    };
    if((in.pressed|in.repeat)&0x40)cycle(-1);
    if(s.selection==3||s.selection==4){auto& volume=s.selection==3?settings.music_volume:settings.sound_volume;
        if(in.pressed&0x40){volume=u8(std::max(0,i32(volume)-4));output.title_configuration();}if(in.pressed&0x80){volume=u8(std::min(100,i32(volume)+4));output.title_configuration();}
        if(in.pressed&0x40){if(volume)--volume;output.title_configuration();}if(in.pressed&0x80){if(volume<100)++volume;output.title_configuration();}}
    if((in.pressed|in.repeat)&0x80)cycle(1);
    if(s.selection>2&&s.selection<5&&s.load_frame%50==0)output.title_sound(29);s.animation_frames=0;
    bool close=false;
    if(in.pressed&0x1001){if(s.selection==6){settings.extra_lives=0;settings.music_mode=settings.effects=1;output.title_sound(10);select(settings.extra_lives,18,3);select(settings.windowed,21,2);select(settings.music_mode,23,3);select(settings.screen_mode,34,2);}else if(s.selection==7){s.selection=0;change(TitleScreen::key_config);output.title_sound(10);return true;}else if(s.selection==8)close=true;}
    if(in.pressed&10){if(s.selection==8)close=true;else{auto& old=animations[s.selection+9];resources.sprite(AnimationFile::menu,old,old.baseSpriteIndex+1);s.selection=8;resources.sprite(AnimationFile::menu,animations[17],animations[17].baseSpriteIndex);output.title_sound(11);}}
    if(close){s.selection=6;change(TitleScreen::main);if(!main())return false;output.title_sound(11);return true;}
    tick();return true;
}
}
