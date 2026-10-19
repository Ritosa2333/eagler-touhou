#include "ApplicationSystem.hpp"
namespace th10 {
void MultimediaTimer::stop(MultimediaTimerEnvironment& env){if(id)env.kill_timer(id);env.end_period(period);id=0;}
void MultimediaTimer::release(MultimediaTimerEnvironment& env){original_virtual_table=0x46f810;stop(env);env.end_period(period);}
i32 ApplicationSystem::open_resources(){
    auto& env=environment;if(!env.open_archive(env.archive_name)){env.report_archive_error(false);return -1;}
    u32 bytes=0;auto* data=env.read_version(env.version_name,bytes);*env.version_data=data;*env.version_size=bytes;
    if(!data){env.report_archive_error(true);return -1;}return 0;
}
void ApplicationSystem::restart_input(){auto& env=environment;env.input_worker->start(env.input_callback,env.global,false,*env.threads);}
i32 ApplicationSystem::initialize(){
    auto& env=environment;open_resources();*env.rate=1;env.global->background_color=0xff000000;
    Camera::initialize(env.global->world_camera,env.global->ui_camera);
    const u32 ticks=env.milliseconds();*env.initial_time=ticks;env.script_random->seed=env.visual_random->seed=static_cast<u16>(ticks);
    env.begin_audio_loading();env.create_statistics();restart_input();env.initialize_model(**env.animations);env.initialize_fonts();return 0;
}
i32 ApplicationSystem::install_callbacks(){
    auto& env=environment;env.global->screen=-2;env.global->pending_screen=0;env.global->reserved_398=0;
    auto* entry=UpdateChain::allocate(env.update_callback,*env.callbacks);entry->flags|=2;entry->owner=env.global;entry->initialize_callback=env.initialize_callback;
    const auto result=(*env.chain)->insert(*entry,1,false,*env.callbacks);if(result)return result;
    const i32 priorities[]={1,40,50};for(u32 i=0;i<3;++i)(*env.chain)->add(env.draw_callbacks[i],env.global,priorities[i],true,true,*env.callbacks);return 0;
}
i32 ApplicationSystem::shutdown(ApplicationState& app){
    auto& env=environment;env.input_worker->stop(*env.threads);env.audio->load_stop=2;env.global->resource_loader.stop(*env.threads);
    if(*env.version_data){env.free_bytes(*env.version_data);*env.version_data=nullptr;}
    env.shutdown_screens();env.destroy_statistics();auto& model=(*env.animations)->model_vertex_buffer;if(model){env.release_device(model);model=nullptr;}
    env.audio->queue_music(4,0,env.stop_music_name);env.release_fonts();
    if(app.keyboard){env.unacquire(app.keyboard);if(app.keyboard){env.release_device(app.keyboard);app.keyboard=nullptr;}}
    if(app.controller){env.unacquire(app.controller);if(app.controller){env.release_device(app.controller);app.controller=nullptr;}}
    if(app.input_driver){env.release_device(app.input_driver);app.input_driver=nullptr;}
    if(*env.timer){(*env.timer)->stop(env);auto* timer=*env.timer;if(timer){timer->release(env);env.release_object(timer);}*env.timer=nullptr;}
    env.release_archive();return 0;
}
}
