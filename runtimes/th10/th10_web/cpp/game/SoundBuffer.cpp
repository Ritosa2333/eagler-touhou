#include "SoundBuffer.hpp"
#include <initializer_list>
namespace th10 {
namespace {
constexpr i32 uninitialized=static_cast<i32>(0x800401f0u),failed=static_cast<i32>(0x80004005u);
i32 add(i32 a,i32 b){return static_cast<i32>(static_cast<u32>(a)+static_cast<u32>(b));}
i32 fade_gain(i32 remaining,i32 scale,i32 duration){
    const i32 product=static_cast<i32>(static_cast<u32>(remaining)*static_cast<u32>(scale));
    if(!duration||(product==INT32_MIN&&duration==-1))__builtin_trap();
    return product/duration;
}
}
void* SoundBuffer::buffer(u32 index) const noexcept {return buffers&&index<buffer_count?buffers[index]:nullptr;}
void* SoundBuffer::available(SoundBufferEnvironment& env){
    if(!buffers)return nullptr;u32 index=0;
    for(;index<buffer_count;++index)if(buffers[index]){u32 flags=0;env.status(buffers[index],&flags);if(!(flags&1))break;}
    if(index==buffer_count){if(!buffer_count)__builtin_trap();index=env.random_buffer()%buffer_count;}
    return buffers[index];
}
u32 SoundBuffer::playing(SoundBufferEnvironment& env){
    if(!buffers)return 0;u32 result=0;
    for(u32 i=0;i<buffer_count;++i)if(buffers[i]){u32 flags=0;env.status(buffers[i],&flags);result|=flags&1;}
    return result;
}
i32 SoundBuffer::restore(void* buffer,u32* restored,SoundBufferEnvironment& env){
    if(!buffer)return uninitialized;if(restored)*restored=0;
    u32 flags=0;const i32 result=env.status(buffer,&flags);if(result<0)return result;if(!(flags&2))return 1;
    do{if(static_cast<u32>(env.restore(buffer))==0x88780096u)env.sleep(10);}while(env.restore(buffer)!=0);
    if(restored)*restored=1;return 0;
}
i32 SoundBuffer::set_volume(i32 adjustment,SoundBufferEnvironment& env){
    i32 gain=-10000;
    if(*env.music_volume){
        const auto attenuation=number(1)-Extended::from_int(*env.music_volume)*number(.01f);
        gain=add(((number(1)-attenuation*attenuation)*Extended::from_int(add(adjustment,5000))).truncate_int(),-5000);
    }
    return env.volume(buffers[0],gain);
}
i32 SoundBuffer::play(u32 requested_priority,u32 flags,SoundBufferEnvironment& env){
    if(!buffers)return uninitialized;void* selected=available(env);if(!selected)return failed;
    u32 restored=0;i32 result=restore(selected,&restored,env);if(result<0)return result;
    if(restored){result=env.refill(*this,selected,false);if(result<0)return result;rewind(env);}
    fade_mode=fade_remaining=fade_duration=0;set_volume(0,env);
    active=1;priority=requested_priority;play_flags=flags;play_position=0;
    return env.play(selected,requested_priority,flags);
}
i32 SoundBuffer::stop_all(SoundBufferEnvironment& env){
    if(!buffers)return uninitialized;active=0;u32 result=0;
    for(u32 i=0;i<buffer_count;++i){result|=env.stop(buffers[i]);result|=env.position(buffers[i],0);}
    fade_mode=0;return static_cast<i32>(result);
}
i32 SoundBuffer::stop_first(SoundBufferEnvironment& env){if(!buffers)return uninitialized;active=0;return env.stop(buffers[0]);}
i32 SoundBuffer::resume(SoundBufferEnvironment& env){if(!buffers)return uninitialized;void* first=buffers[0];const u32 flags=play_flags;active=1;return env.play(first,priority,flags);}
i32 SoundBuffer::rewind(SoundBufferEnvironment& env){if(!buffers)return uninitialized;u32 result=0;for(u32 i=0;i<buffer_count;++i)result|=env.position(buffers[i],0);return static_cast<i32>(result);}
i32 SoundBuffer::query(u32 index,void** result,SoundBufferEnvironment& env){if(!buffers)return uninitialized;if(index>=buffer_count)return static_cast<i32>(0x80070057u);*result=nullptr;return env.query(buffers[index],result);}
i32 SoundBuffer::fill(void* destination,bool repeat,SoundBufferEnvironment& env){
    i32 result=restore(destination,nullptr,env);if(result<0)return result;
    SoundLock area{};result=env.lock(destination,0,buffer_bytes,area,false);if(result<0)return result;
    wave->reset(false,*env.waves);u32 copied=0;result=wave->read(area.first,area.first_bytes,&copied,*env.waves);if(result<0)return result;
    if(!copied)std::memset(area.first,wave->format->bits_per_sample==8?0x80:0,area.first_bytes);
    else if(copied<area.first_bytes){
        if(!repeat)std::memset(area.first+copied,wave->format->bits_per_sample==8?0x80:0,area.first_bytes-copied);
        else while(copied<area.first_bytes){
            result=wave->reset(false,*env.waves);if(result<0)return result;u32 count=0;
            result=wave->read(area.first+copied,area.first_bytes-copied,&count,*env.waves);if(result<0)return result;
            if(!count)return failed; // A broken source must not spin forever.
            copied+=count;
        }
    }
    env.unlock(destination,area);return 0;
}
// 0x421e00. The original checks all four modes in this order and reloads the
// owning pointer after each platform call; callbacks may advance another mode.
void SoundBuffer::advance_fades(SoundBuffer** current,SoundBufferEnvironment& env){
    if(!*current)return;
    for(const i32 mode:{1,2,4,3})(*current)->advance_fade(mode,env);
}
i32 SoundBuffer::advance_fade(i32 mode,SoundBufferEnvironment& env){
    if(fade_mode!=mode)return 0;fade_remaining=add(fade_remaining,-1);
    if(fade_remaining<=0){fade_mode=0;if(mode==1)env.stop(buffers[0]);return 1;}
    const i32 scale=mode<=2?5000:1000;
    const i32 adjustment=mode==1||mode==4?add(fade_gain(fade_remaining,scale,fade_duration),-scale):fade_gain(fade_remaining,-scale,fade_duration);
    set_volume(adjustment,env);return 0;
}
}
