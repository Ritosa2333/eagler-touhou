#include "WindowMessages.hpp"
namespace th10 {
u32 WindowMessages::dispatch(u32 window,u32 message,u32 parameter,u32 detail){
    auto& env=environment;
    switch(message){
    case 0x10:*env.engine_flags|=0x80;return 1;
    case 0x14:return 1;
    case 0x1c:*env.active=parameter;*env.inactive=parameter==0;break;
    case 0x20:
        if(!*env.windowed&&!*env.inactive){env.show_cursor(false);env.set_cursor(0);}
        else{const u32 cursor=env.load_cursor(0,0x7f00);env.set_cursor(cursor);env.show_cursor(true);}return 1;
    case 0x201:env.foreground(window);break;
    case 0x3c9:if(*env.midi)env.midi_completed(*env.midi,detail);break;
    }
    return env.default_message(window,message,parameter,detail);
}
}
