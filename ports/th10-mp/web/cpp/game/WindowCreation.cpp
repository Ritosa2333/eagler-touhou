#include "WindowCreation.hpp"
namespace th10 {
i32 WindowCreation::create(u32 instance){
    auto& env=environment;WindowClass definition{};definition.background=env.stock_object(4);definition.cursor=env.load_cursor(0,0x7f00);definition.instance=instance;definition.procedure=env.procedure;definition.name=env.class_name;
    *env.active=1;*env.inactive=0;env.register_class(definition);
    u32 window;
    if(!*env.windowed)window=env.create_window(env.class_name,env.title,0xcf0000,0,0,640,480,instance);
    else{const auto horizontal=env.metric(7);const auto vertical=env.metric(8);const auto caption=env.metric(4);window=env.create_window(env.class_name,env.title,0x100a0000,static_cast<i32>(0x80000000),static_cast<i32>(0x80000000),wrapping_add(wrapping_add(horizontal,horizontal),640),wrapping_add(wrapping_add(caption,wrapping_add(vertical,vertical)),480),instance);}
    *env.loop_window=window;*env.window=window;if(!window)return 1;
    env.send_message(window,0x112,0xf020,0);env.sleep(16);env.send_message(*env.loop_window,0x112,0xf120,0);return 0;
}
}
