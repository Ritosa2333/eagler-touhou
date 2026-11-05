#include "TitleKeys.hpp"
namespace th10 {
// 0x42f8b0. Each button number appears in the normal and highlighted row.
void TitleKeys::draw_bindings(){auto& env=environment;
    for(i32 action=0;action<5;++action)for(i32 digit=0;digit<4;++digit){
        const i32 script=(digit<2?67:77)+action*2+(digit&1);
        const auto id=env.registry->find_child(title.animation_ids[2],script);
        if(auto* vm=env.registry->find(id)){const i32 value=static_cast<std::int16_t>(title.configured_keys[action]);env.bind_digit(*vm,51+(digit&1?value%10:value/10));}
    }
}
// 0x430250. Assigning an already used button swaps the previous binding into
// every conflicting action. Reassigning the same button has no side effects.
void TitleKeys::assign(i32 action,i32 button){
    auto* keys=title.configured_keys;const auto old=keys[action];if(static_cast<std::int16_t>(old)==button)return;
    for(i32 index=0;index<5;++index)if(index!=action&&static_cast<std::int16_t>(keys[index])==button)keys[index]=old;
    keys[action]=static_cast<u16>(button);draw_bindings();environment.sound(10);
}
void TitleKeys::restore(){for(i32 i=0;i<4;++i)title.configured_keys[i]=environment.active_bindings[i];title.configured_keys[4]=environment.active_bindings[8];draw_bindings();}
void TitleKeys::leave(){auto& env=environment;env.sound(11);title.signal_script(2,6,env);title.set_phase(4,env.rate);}
// 0x42f540. The reset row restores the live bindings; the exit row commits the
// edited values. Cancel on the exit row discards pending edits.
i32 TitleKeys::update(){auto& t=title;auto& m=t.menu;auto& env=environment;
    switch(t.phase){
    case 0:m.item_count=7;m.select(0);t.create_script(2,env);t.set_phase(1,env.rate);restore();[[fallthrough]];
    case 1:if(t.elapsed.current>6){t.set_phase(2,env.rate);env.interrupt_immediately(t.animation_ids[2],3);t.signal_script(2,static_cast<u16>(m.selected+17),env);}break;
    case 2:{
        m.reserved=m.selected;if((*env.pressed|*env.repeated)&0x10)m.move(-1);if((*env.pressed|*env.repeated)&0x20)m.move(1);
        if(m.reserved!=m.selected){env.sound(12);env.interrupt_immediately(t.animation_ids[2],3);t.signal_script(2,static_cast<u16>(m.selected+7),env);}
        const auto* buttons=env.buttons();for(i32 button=0;button<31;++button)if(buttons[button]&0x80){if(m.selected<=4)assign(m.selected,button);break;}
        if((*env.pressed&10)&&m.selected==6){restore();leave();return 1;}
        if(*env.pressed&0x1001){if(m.selected==5){restore();env.sound(10);return 1;}if(m.selected==6){for(i32 i=0;i<4;++i)env.active_bindings[i]=t.configured_keys[i];env.active_bindings[8]=t.configured_keys[4];__builtin_memcpy(env.saved_bindings,env.active_bindings,18);leave();return 1;}}
        break;}
    case 4:if(t.elapsed.current>=10){t.set_screen(4,env.rate);m.pop();}break;
    }return 1;
}
}
