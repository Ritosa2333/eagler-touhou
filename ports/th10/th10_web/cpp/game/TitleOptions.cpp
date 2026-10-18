#include "TitleOptions.hpp"
namespace th10 {
void TitleOptions::bind(i32 script,i32 sprite){auto& env=environment;if(auto* vm=env.registry->find(env.registry->find_child(title.animation_ids[1],script)))env.bind_digit(*vm,sprite);}
void TitleOptions::visible(i32 script,bool value){auto& env=environment;if(auto* vm=env.registry->find(env.registry->find_child(title.animation_ids[1],script))){if(value)vm->flags|=2;else vm->flags&=~2u;}}
void TitleOptions::display_mode(){const auto mode=environment.settings->mode;const i32 sprite=mode==0?46:mode==1?48:44;bind(31,sprite);bind(32,sprite+1);}
// 0x42e5a0. Preserve the original gain curve, including its use of the music
// volume in the effect gain calculation. Zero effects volume mutes outright.
void TitleOptions::apply_volume(){auto& env=environment;
    *env.music_volume=env.settings->music;env.apply_music_volume();*env.effects_volume=env.settings->effects;
    if(!*env.effects_volume)*env.effects_gain=-10000;
    else{auto amount=number(1.0f)-Extended::from_int(*env.music_volume)*number(.01f);amount=amount*amount;amount=amount*amount;*env.effects_gain=-5000-((number(1.0f)-amount)*number(-5000.0f)).truncate_int();}
    for(i32 channel=0;channel<2;++channel)for(i32 digit=0;digit<6;++digit){const i32 value=channel?env.settings->effects:env.settings->music;const i32 number=digit%3==0?value/100:digit%3==1?(value/10)%10:value%10;bind(37+channel*8+(digit/3)*4+digit%3,number+(digit<3?51:61));}
    for(i32 channel=0;channel<2;++channel){const i32 value=channel?env.settings->effects:env.settings->music;visible(37+channel*8,value>=100);visible(38+channel*8,value>=10);visible(41+channel*8,value>=100);visible(42+channel*8,value>=10);}
}
void TitleOptions::leave(i32 sound){auto& env=environment;title.signal_script(1,6,env);env.sound(sound);title.set_phase(4,env.rate);}
// 0x42d920. Volume previews fire once on each 60-frame boundary while the
// effects row is selected. Left and right inputs are processed independently.
i32 TitleOptions::update(){auto& t=title;auto& m=t.menu;auto& env=environment;auto& settings=*env.settings;
    switch(t.phase){
    case 0:m.item_count=6;m.select(0);t.create_script(1,env);apply_volume();t.set_phase(1,env.rate);[[fallthrough]];
    case 1:if(t.elapsed.current>6){t.set_phase(2,env.rate);env.interrupt_immediately(t.animation_ids[1],3);display_mode();t.signal_script(1,static_cast<u16>(m.selected+17),env);}break;
    case 2:
        m.reserved=m.selected;if((*env.pressed|*env.repeated)&0x10)m.move(-1);if((*env.pressed|*env.repeated)&0x20)m.move(1);
        if(m.reserved!=m.selected){env.sound(12);env.interrupt_immediately(t.animation_ids[1],3);t.signal_script(1,static_cast<u16>(m.selected+7),env);}
        if(*env.pressed&10){if(m.selected==5)leave(11);else{env.sound(11);m.select(5);env.interrupt_immediately(t.animation_ids[1],3);t.signal_script(1,static_cast<u16>(m.selected+7),env);}return 1;}
        if(m.selected==2&&t.elapsed.current!=t.elapsed.previous&&t.elapsed.current%60==0)env.sound(4);
        if((*env.pressed|*env.repeated)&0x40){if(m.selected==0){settings.mode=settings.mode?settings.mode-1:2;display_mode();env.sound(12);}else if(m.selected==1||m.selected==2){auto& volume=m.selected==1?settings.music:settings.effects;volume=volume<5?0:volume-5;apply_volume();}}
        if((*env.pressed|*env.repeated)&0x80){if(m.selected==0){settings.mode=settings.mode<2?settings.mode+1:0;env.sound(12);display_mode();}else if(m.selected==1||m.selected==2){auto& volume=m.selected==1?settings.music:settings.effects;volume=static_cast<std::int8_t>(volume+5);if(volume>100)volume=100;apply_volume();}}
        if(*env.pressed&0x1001){if(m.selected==3){leave(10);return 1;}if(m.selected==4){settings.music=100;settings.effects=80;settings.mode=0;apply_volume();display_mode();env.sound(10);return 1;}if(m.selected==5)leave(11);}break;
    case 4:if(t.elapsed.current>=10){if(m.selected==3){t.set_screen(5,env.rate);m.push();}else if(m.selected==5){env.interrupt_immediately(t.animation_ids[90],2);env.interrupt_immediately(t.animation_ids[91],2);t.set_screen(2,env.rate);m.pop();}}break;
    }return 1;
}
}
