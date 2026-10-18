#include "ApplicationLoop.hpp"
namespace th10 {
void SystemSettings::initialize(HighResolutionClock& clock,SystemSettingsEnvironment& env){
    const u32 queries[]={0x10,0x53,0x54},actions[]={0x11,0x55,0x56};
    for(u32 i=0;i<3;++i)env.system_parameter(queries[i],0,&saved[i],0);
    for(auto action:actions)env.system_parameter(action,0,nullptr,2);
    env.query_frequency(clock.frequency);env.query_counter(clock.origin);
}
void SystemSettings::restore(SystemSettingsEnvironment& env){const u32 actions[]={0x11,0x55,0x56};for(u32 i=0;i<3;++i)env.system_parameter(actions[i],saved[i],nullptr,2);env.restore_input_method();}
i32 ApplicationLoop::disable_fog(ApplicationState& app,u32& enabled,ApplicationLoopEnvironment& env){if(!enabled)return 0;env.flush();enabled=0;
#ifdef TH_NATIVE_PLATFORM
return env.set_fog_enabled(false);
#else
return env.render_state(app.device,28,0);
#endif
}
i32 ApplicationLoop::step(ApplicationLoopEnvironment& env){
    const auto now=env.time();const bool backwards=now<Extended::from_double(previous_time);sampled_time=now.to_double();if(backwards)next_frame_time=now.to_double();
    const auto deadline=Extended::from_double(next_frame_time);previous_time=now.to_double();
    if(!(deadline<now)){env.sleep(0);return 0;}
    do{next_frame_time=(Extended::from_double(next_frame_time)+Extended::from_double(1.0/60.0)).to_double();}while(Extended::from_double(next_frame_time)<Extended::from_double(sampled_time));
    auto& app=*env.application;env.flush();app.active_camera=&app.ui_camera;env.configure_flat(app.ui_camera);env.set_viewport(app.device,app.active_camera->viewport);app.screen_space=1;
    const auto result=env.update();env.update_audio();if(!result||result==-1){env.stop_loader();return result?2:1;}
    ++skipped_frames;if(static_cast<i32>(static_cast<std::int8_t>(skipped_frames))>=static_cast<i32>(*env.frame_skip)+1){
        env.begin_scene(app.device);auto& animations=**env.animations;animations.batch_quads=0;animations.vertex_write=animations.batch_start=animations.vertex_buffer;*env.graphics_state=255;
        disable_fog(app,*env.fog_enabled,env);env.draw();env.flush();env.clear_texture(app.device);env.end_scene(app.device);skipped_frames=0;env.present();
    }
    *env.frame_duration=(env.time()-Extended::from_double(sampled_time)).to_double();return 0;
}
}
