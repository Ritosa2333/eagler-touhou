#include "InputSetup.hpp"
namespace th10 {
void InputSetup::release_driver(){if(application.input_driver){environment.release(application.input_driver);application.input_driver=nullptr;}}
i32 InputSetup::initialize(){
    auto& app=application;auto& env=environment;const u32 instance=env.window_instance(app.window);
    if(app.display_flags&8)return -1;
    if(env.create_driver(instance,env.input_interface,&app.input_driver)<0){app.input_driver=nullptr;env.report(InputSetupMessage::Driver);return -1;}
    if(env.create_device(app.input_driver,env.keyboard_guid,&app.keyboard)<0){release_driver();env.report(InputSetupMessage::Driver);return -1;}
    if(env.set_format(app.keyboard,env.keyboard_format)<0){if(app.keyboard){env.release(app.keyboard);app.keyboard=nullptr;}release_driver();env.report(InputSetupMessage::KeyboardFormat);return -1;}
    if(env.cooperative(app.keyboard,app.window,0x16)<0){if(app.keyboard){env.release(app.keyboard);app.keyboard=nullptr;}release_driver();env.report(InputSetupMessage::KeyboardCooperative);return -1;}
    env.acquire(app.keyboard);env.report(InputSetupMessage::KeyboardReady);
    env.enumerate_controllers(app.input_driver,env.controller_callback);
    if(app.controller){
        env.set_format(app.controller,env.controller_format);env.cooperative(app.controller,app.window,0xa);
        env.global->controller_capabilities[0]=44;env.capabilities(app.controller,env.global->controller_capabilities);
        env.enumerate_axes(app.controller,env.axis_callback);env.report(InputSetupMessage::ControllerReady);
    }
    return 0;
}
void InputSetup::initialize_worker(){auto& app=*environment.global;app.engine_flags&=~0x600u;InputSetup{app,environment}.initialize();app.engine_flags=(app.engine_flags&~0x600u)|(app.keyboard?0x200u:0)|(app.controller?0x400u:0);}
bool InputSetup::select_controller(const u8* instance){auto& app=*environment.global;if(!app.controller&&environment.create_device(app.input_driver,instance+4,&app.controller)<0)return true;return false;}
bool InputSetup::configure_axis(const u8* object){u32 type;std::memcpy(&type,object+24,4);if(type&3){const u32 range[]={24,16,type,2,static_cast<u32>(-1000),1000};if(environment.set_axis_range(environment.global->controller,range)<0)return false;}return true;}
}
