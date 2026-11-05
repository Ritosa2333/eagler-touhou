#include "SoundStream.hpp"
namespace th10 {
namespace {constexpr i32 uninitialized=static_cast<i32>(0x800401f0u);}
// 0x44d850. An unsafe write interval is skipped before touching the buffer.
i32 SoundStream::update(bool loop,SoundBufferEnvironment& env){
    if(!buffers||!wave)return uninitialized;
    u32 play=0,write=0;env.current_position(buffers[0],&play,&write);
    if(write_cursor>=write-chunk_bytes&&write_cursor<write)return uninitialized;
    u32 restored=0;i32 result=restore(buffers[0],&restored,env);if(result<0)return result;
    if(restored){result=env.refill(*this,buffers[0],false);return result<0?result:0;}
    SoundLock area{};result=env.lock(buffers[0],write_cursor,chunk_bytes,area,true);if(result<0)return result;
    if(area.second)return static_cast<i32>(0x8000ffffu);
    if(silent)std::memset(area.first,wave->format->bits_per_sample==8?0x80:0,area.first_bytes);
    else{
        u32 copied=0;result=wave->read(area.first,area.first_bytes,&copied,*env.waves);if(result<0)return result;
        if(copied<area.first_bytes){
            if(!loop){std::memset(area.first+copied,wave->format->bits_per_sample==8?0x80:0,area.first_bytes-copied);silent=1;}
            else while(copied<area.first_bytes){
                result=wave->reset(true,*env.waves);if(result<0)return result;u32 count=0;
                result=wave->read(area.first+copied,area.first_bytes-copied,&count,*env.waves);if(result<0)return result;
                if(!count)return static_cast<i32>(0x80004005u);
                copied+=count;
            }
        }
    }
    env.unlock(buffers[0],area);result=env.current_position(buffers[0],&play,nullptr);if(result<0)return result;
    played_bytes+=play<previous_play_cursor?buffer_bytes-previous_play_cursor+play:play-previous_play_cursor;
    previous_play_cursor=play;
    if(silent&&played_bytes>=wave->total_bytes)env.stop(buffers[0]);
    if(!buffer_bytes)__builtin_trap();write_cursor=(write_cursor+area.first_bytes)%buffer_bytes;return 0;
}
i32 SoundStream::reset(SoundBufferEnvironment& env){
    if(!buffers||!buffers[0]||!wave)return uninitialized;
    previous_play_cursor=played_bytes=write_cursor=silent=0;u32 restored=0;
    i32 result=restore(buffers[0],&restored,env);if(result<0)return result;
    if(restored){result=env.refill(*this,buffers[0],false);if(result<0)return result;}
    wave->reset(false,*env.waves);return env.position(buffers[0],0);
}
}
