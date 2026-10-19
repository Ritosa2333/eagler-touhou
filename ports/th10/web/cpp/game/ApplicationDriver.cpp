#include "ApplicationDriver.hpp"
namespace th10 {
DriverAction ApplicationDriver::advance(ApplicationDriverEnvironment& env,i32 callback_result){
    switch(phase){
    case Phase::Boot:{
        *env.instance=instance;auto* pool=static_cast<StartupAllocationPool*>(env.allocate(sizeof(StartupAllocationPool)));if(pool)std::memset(pool,0,sizeof(*pool));*env.allocation_pool=pool;
        for(u32 i=0;i<7;i++)env.initialize_lock(i);env.notice(DriverNotice::Starting);
        if(env.check_instance()==-1){phase=Phase::Cleanup;return DriverAction::Continue;}
        *env.application_instance=instance;env.save_system_settings();if(env.load_configuration()!=0){phase=Phase::Cleanup;return DriverAction::Continue;}
        if(env.configuration->display_flags&0x100)env.configuration->options[3]=env.choose_display(instance)!=6;
        env.checksum();phase=Phase::CreateSession;return DriverAction::Continue;
    }
    case Phase::CreateSession:{
        auto* chain=static_cast<UpdateChain*>(env.allocate(sizeof(UpdateChain)));if(chain)chain->initialize();*env.chain=chain;
        *env.graphics_driver=env.create_graphics_driver();if(!*env.graphics_driver){env.notice(DriverNotice::NoGraphicsDriver);phase=Phase::Cleanup;return DriverAction::Continue;}
        if(env.create_window(instance)!=0){phase=Phase::Cleanup;return DriverAction::Continue;}
        env.start_audio(*env.window);phase=Phase::AfterGraphics;return DriverAction::InitializeGraphics;
    }
    case Phase::AfterGraphics:{
        if(callback_result!=0){phase=Phase::Cleanup;return DriverAction::Continue;}
        env.initialize_input();auto* animations=static_cast<AnmManager*>(env.allocate(sizeof(AnmManager)));if(animations)env.initialize_animations(*animations);*env.animations=animations;
        if(!env.configuration->options[3]){env.enable_input_method(false);env.show_cursor(false);env.clear_cursor();}
        *env.initial_times[0]=0;const auto now=env.time();for(auto* value:env.initial_times)*value=now.to_double();
        env.foreground(*env.window);result=env.install_callbacks();
        if(result){if(result!=-1)result=2;phase=Phase::Shutdown;return DriverAction::Continue;}
        result=0;*env.skipped_frames=0xfc;phase=Phase::Running;return DriverAction::Continue;
    }
    case Phase::Running:
        if(*env.quit){phase=Phase::Shutdown;return DriverAction::Continue;}
        if(env.peek_message(message)){env.translate_message(message);env.dispatch_message(message);return DriverAction::Continue;}
        if(const auto status=env.cooperative_level(*env.device);status==0){phase=Phase::AfterFrame;return DriverAction::Frame;}
        else if(static_cast<u32>(status)==0x88760869){env.release_capture_textures(**env.animations);if(env.reset_device(*env.device,*env.parameters)!=0){phase=Phase::Shutdown;return DriverAction::Continue;}env.configure_graphics_defaults();*env.reset_frames=3;env.application->engine_flags|=0x10;}
        return DriverAction::Continue;
    case Phase::AfterFrame:
        result=callback_result;if(result)phase=Phase::Shutdown;else{env.application->engine_flags&=~0x10u;phase=Phase::Running;}return DriverAction::Continue;
    case Phase::Shutdown:{
        env.shutdown_application();auto* chain=*env.chain;if(chain){env.clear_chain(*chain);env.release_object(chain);}*env.chain=nullptr;phase=Phase::DrainAudio;return DriverAction::Continue;
    }
    case Phase::DrainAudio:if(!env.update_audio())phase=Phase::Cleanup;return DriverAction::Continue;
    case Phase::Cleanup:{
        env.audio->load_stop=2;env.join_audio();env.release_audio();auto* animations=*env.animations;if(animations){env.release_animations(*animations);env.release_object(animations);}*env.animations=nullptr;
        if(*env.device){env.reset_device(*env.device,*env.parameters);if(*env.device){env.release_device(*env.device);*env.device=nullptr;}}
        if(*env.graphics_driver){env.release_device(*env.graphics_driver);*env.graphics_driver=nullptr;}
        if(*env.window){env.hide_window(*env.window);env.shrink_window(*env.window);env.destroy_window(*env.window);*env.window=0;}
        env.show_cursor(true);
        if(result==2){env.log->cursor=env.log->text;env.log->text[0]=0;env.notice(DriverNotice::Restarting);if(!env.configuration->options[3])env.enable_input_method(true);for(u32 i=0;i<60;i++)if(env.peek_message(message)){env.translate_message(message);env.dispatch_message(message);}phase=Phase::CreateSession;}
        else phase=Phase::Finish;return DriverAction::Continue;
    }
    case Phase::Finish:{
        env.save_configuration();if(*env.midi){env.stop_midi(*env.midi);auto* midi=*env.midi;if(midi){env.release_midi(midi);env.release_object(midi);}*env.midi=nullptr;}
        if(env.log->cursor!=env.log->text){env.notice(DriverNotice::LogSeparator);if(env.log->has_error)env.show_log(env.log->text);env.save_log(env.log->text,std::strlen(env.log->text));}
        for(u32 i=0;i<7;i++)env.delete_lock(i);env.restore_system_settings();auto* pool=*env.allocation_pool;if(pool){if(pool->used)for(auto* allocation:pool->allocations)if(allocation)env.release_bytes(allocation);env.release_object(pool);}
        phase=Phase::Done;return DriverAction::Done;
    }
    case Phase::Done:return DriverAction::Done;
    }
    return DriverAction::Done;
}
}
