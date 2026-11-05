#include "GraphicsStartup.hpp"
#include "GameMath.hpp"
namespace th10 {
i32 GraphicsStartup::initialize(){
    auto& env=environment;PresentationParameters parameters{};DisplayMode mode{};env.display_mode(*env.driver,mode);
    if(!env.settings->options[3]){
        if(env.settings->display_flags&1){parameters.format=23;env.settings->options[0]=1;}
        else if(env.settings->options[0]==255){parameters.format=22;env.settings->options[0]=0;env.notice(GraphicsNotice::DefaultDepth);}
        else parameters.format=env.settings->options[0]?23:22;
        if(*env.alternate_launch)*env.disable_vsync=1;
        if(!*env.disable_vsync){parameters.refresh=60;parameters.interval=1;env.notice(GraphicsNotice::Vsync);parameters.swap_effect=1;}
        else{parameters.refresh=0;parameters.swap_effect=1;parameters.interval=0x80000000;env.notice(GraphicsNotice::Immediate);}
    }else{parameters.format=mode.format;parameters.swap_effect=1;parameters.windowed=1;}
    *env.engine_flags|=2;parameters.width=640;parameters.height=480;parameters.automatic_depth=1;parameters.depth_format=80;parameters.flags=1;*env.fixed_refresh=1;
    bool retry=false,hardware_device=true;
    for(;;){
        if(!(env.settings->display_flags&2)){
            if(env.create_device(*env.driver,1,*env.window,0x40,parameters,env.device)>=0){env.notice(GraphicsNotice::HardwareDevice);*env.engine_flags|=1;break;}
            if(retry)env.notice(GraphicsNotice::HardwareRetry);
            if(env.create_device(*env.driver,1,*env.window,0x20,parameters,env.device)>=0){env.notice(GraphicsNotice::SoftwareVertexDevice);*env.engine_flags&=~1u;break;}
            if(retry)env.notice(GraphicsNotice::SoftwareVertexRetry);
        }
        if(env.create_device(*env.driver,2,*env.window,0x20,parameters,env.device)>=0){env.notice(GraphicsNotice::ReferenceDevice);*env.engine_flags&=~1u;hardware_device=false;break;}
        if(!*env.disable_vsync){env.notice(GraphicsNotice::RefreshFallback);parameters.refresh=0;*env.fixed_refresh=0;retry=true;}
        else if(parameters.interval==0x80000000){env.notice(GraphicsNotice::VsyncFallback);parameters.interval=1;parameters.swap_effect=3;}
        else{env.notice(GraphicsNotice::DeviceFailed);if(*env.driver){env.release(*env.driver);*env.driver=nullptr;}return 1;}
    }
    // The original keeps the tangent result in extended precision while it
    // copies the final parameters returned by CreateDevice.
    const auto half_fov=tangent(Extended::from_double(0.2617993950843811));*env.parameters=parameters;
    const Vec3 eye{320,-240,(-(number(240.0f)/half_fov)).to_float()},target{320,-240,0},up{0,1,0};
    env.look_at(*env.view,eye,target,up);env.perspective(*env.projection,0x1.0c1524p-1f,0x1.555556p+0f,100,10000);
    env.set_transform(*env.device,Matrices::View,*env.view);env.set_transform(*env.device,Matrices::Projection,*env.projection);env.get_viewport(*env.device,*env.viewport);env.get_capabilities(*env.device,env.capabilities);
    if(!(env.capabilities[36]&0x40))env.notice(GraphicsNotice::NoTextureAlpha);
    if(env.capabilities[22]<=256)env.notice(GraphicsNotice::SmallTextureLimit);
    if(!(env.settings->display_flags&1)&&hardware_device){if(env.check_argb_texture(*env.driver,parameters.format)==0)*env.engine_flags|=4;else{*env.engine_flags&=~4u;env.settings->display_flags|=1;env.notice(GraphicsNotice::NoArgbTexture);}}
    env.configure_defaults();return 0;
}
}
