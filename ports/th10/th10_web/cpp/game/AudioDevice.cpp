#include "AudioDevice.hpp"
namespace th10 {
namespace {
void make_format(u8* bytes,u32 channels,u32 rate,u32 bits){
    std::memset(bytes,0,18);const u16 format=1,ch=channels,depth=bits,alignment=((depth>>3)*channels)&0xffff;const u32 byte_rate=alignment*rate;
    std::memcpy(bytes,&format,2);std::memcpy(bytes+2,&ch,2);std::memcpy(bytes+4,&rate,4);std::memcpy(bytes+8,&byte_rate,4);std::memcpy(bytes+12,&alignment,2);std::memcpy(bytes+14,&depth,2);
}
}
void AudioDevice::begin_loading(u32 window){auto& m=manager;std::memset(&m,0,sizeof(m));m.startup_window=window;m.device_worker=environment.begin_thread(environment.device_worker_callback,&m,0,m.device_worker_id);}
void AudioDevice::begin_source_loading(){u32 id;auto& env=environment;env.global_manager->source_worker=env.begin_thread(env.source_worker_callback,env.global_manager,0,id);}
void AudioDevice::join_loading(){
    auto& m=manager;auto& env=environment;if(!m.device_worker)return;if(!m.load_stop)m.load_stop=1;
    while(env.wait_thread(m.device_worker,100)==0x102)env.sleep(1);
    while(env.wait_thread(m.source_worker,100)==0x102)env.sleep(1);
    env.close_thread(m.device_worker);env.close_thread(m.source_worker);m.device_worker=m.source_worker=0;
}
void AudioDevice::release_driver(void** driver){if(*driver){environment.resources->release(*driver);*driver=nullptr;}environment.resources->free(driver);}
void AudioDevice::release(){
    auto& m=manager;auto& env=environment;auto& resources=*env.resources;
    if(m.formats){env.free_source(m.formats);m.formats=nullptr;}
    for(u32 i=0;i<128;++i){if(m.effect_buffers[i]){resources.release(m.effect_buffers[i]);m.effect_buffers[i]=nullptr;}if(m.source_buffers[i]){resources.release(m.source_buffers[i]);m.source_buffers[i]=nullptr;}}
    for(auto& wave:m.pending_waves)if(wave){env.free_source(wave);wave=nullptr;}
    if(!m.driver)return;
    env.end_timer(m.window);env.unload_music(m);m.device=nullptr;resources.audio->stop(m.primary_buffer);
    if(m.primary_buffer){resources.release(m.primary_buffer);m.primary_buffer=nullptr;}
    if(m.music){env.delete_music(*m.music);m.music=nullptr;}
    if(m.driver){release_driver(static_cast<void**>(m.driver));m.driver=nullptr;}
    for(auto& data:m.cached_data)if(data){env.free_source(data);data=nullptr;}
}
i32 AudioDevice::configure_primary(void** driver,u32 channels,u32 rate,u32 bits){
    auto& env=environment;if(!*driver)return static_cast<i32>(0x800401f0u);
    SoundBufferDescription description{};description.size=sizeof(description);description.flags=1;void* buffer=nullptr;
    i32 result=env.resources->create_buffer(*driver,description,&buffer);if(result<0)return result;
    u8 format[18];make_format(format,channels,rate,bits);result=env.set_format(buffer,format);if(result<0)return result;
    if(buffer)env.resources->release(buffer);return 0;
}
i32 AudioDevice::initialize_hardware(u32 window){
    auto& m=manager;auto& env=environment;auto& resources=*env.resources;
    for(auto& life:m.effect_lifetimes)life=-1;for(auto& effect:m.pending_effects)effect=-1;
    auto** driver=static_cast<void**>(resources.allocate(sizeof(void*)));if(!driver)return -1;*driver=nullptr;m.driver=driver;
    if(env.create_device(driver)<0||env.cooperative_level(*driver,window)<0){env.log_device(0);if(m.driver){release_driver(static_cast<void**>(m.driver));m.driver=nullptr;}return -1;}
    configure_primary(driver,2,44100,16);m.device=*static_cast<void**>(m.driver);m.music_thread=0;
    u8 format[18];make_format(format,2,44100,16);SoundBufferDescription description{};description.size=sizeof(description);description.flags=0x8008;description.bytes=0x8000;description.format=format;
    if(resources.create_buffer(m.device,description,&m.primary_buffer)<0)return -1;
    SoundLock lock{};if(resources.audio->lock(m.primary_buffer,0,0x8000,lock,true)<0)return -1;
    std::memset(lock.first,0,0x8000);env.unlock_source(m.primary_buffer,lock);resources.audio->play(m.primary_buffer,0,1);
    m.music_volume=m.effects_volume=100;env.begin_timer(window);m.window=window;
    return 0;
}
i32 AudioDevice::initialize(u32 window){
    auto& env=environment;if(initialize_hardware(window)<0)return -1;
    for(i32 i=0;i<37;++i){if(*env.load_stop==2)return -1;if(env.load_source(*env.global_manager,i,env.source_names[i])!=0){env.log_device(1,env.source_names[i]);return -1;}}
    env.log_device(2);return 0;
}
bool AudioInitializationTask::advance(AudioManager& manager,u32 window,AudioDeviceEnvironment& env){
    if(done)return true;
    if(!started){started=true;if(AudioDevice{manager,env}.initialize_hardware(window)<0){result=-1;done=true;return true;}}
    while(index<37){
        if(!source.started){if(*env.load_stop==2){result=-1;done=true;return true;}source.index=index;source.filename=env.source_names[index];}
        if(!source.advance(*env.global_manager,env))return false;
        if(source.result){env.log_device(1,env.source_names[index]);result=-1;done=true;return true;}
        ++index;source={};
    }
    env.log_device(2);done=true;return true;
}
i32 AudioDevice::initialize_effects(){
    auto& m=manager;auto& env=environment;for(auto& effect:m.pending_effects)effect=-1;env.join_loaders(*env.global_manager);
    if(!m.driver)return -1;if(!m.device)return 0;
    for(u32 i=0;i<47;++i){env.duplicate_buffer(m.device,m.source_buffers[env.definitions[i].source],m.effect_buffers+i);env.resources->audio->position(m.effect_buffers[i],0);env.resources->audio->volume(m.effect_buffers[i],env.definitions[i].volume);}
    m.music_volume=*env.configured_music;m.effects_volume=*env.configured_effects;
    if(!m.effects_volume)m.effects_gain=-10000;
    else {const auto attenuation=number(1)-Extended::from_int(m.music_volume)*number(.01f);const auto square=attenuation*attenuation;const i32 amount=((number(1)-square*square)*number(-5000)).truncate_int();m.effects_gain=static_cast<i32>(static_cast<u32>(-5000)-static_cast<u32>(amount));}
    return 0;
}
}
