#include "AudioControl.hpp"
namespace th10 {
namespace {i32 add(i32 a,i32 b){return static_cast<i32>(static_cast<u32>(a)+static_cast<u32>(b));}}
i32 AudioControl::track_index(const char* name) const {
    // The original gives '/' precedence even if a later backslash is present.
    const char* slash=std::strrchr(name,'/');if(!slash)slash=std::strrchr(name,'\\');if(slash)name=slash+1;
    for(i32 i=0;manager.formats[i].filename[0];++i)if(!std::strcmp(manager.formats[i].filename,name))return i;
    return 0;
}
i32 AudioControl::select_music(const char* name){if(!manager.music)return -1;const auto index=track_index(name);environment.select_music(*manager.music->wave,manager.formats+index);return 0;}
void AudioControl::unload_music(){
    auto& m=manager;auto& env=environment;if(!m.music)return;env.stop_music(*m.music);
    if(m.music_thread){env.quit_thread(m.music_thread_id);while(env.wait_thread(m.music_thread,256)!=0)env.quit_thread(m.music_thread_id);env.close_handle(m.music_thread);env.close_handle(m.notification_event);m.music_thread=0;}
    if(m.music){env.delete_music(*m.music);m.music=nullptr;}
}
AudioControl::CommandResult AudioControl::execute(MusicCommand& c){
    auto& m=manager;auto& env=environment;using Result=CommandResult;
    switch(c.kind){
    case 1:
        if(*env.display_flags&16){if(c.step)return Result::Advance;unload_music();}
        env.prepare_track(m,c.argument,c.filename);return Result::RemoveAndContinue;
    case 2:
        if((*env.display_flags&16)&&c.argument>=0){
            if(!c.step)return env.start_track(m,c.argument)==0?Result::Advance:Result::Remove;
            if(c.step==2)return m.music&&env.reset_music(*m.music)<0?Result::Remove:Result::Advance;
            if(c.step==5){void* selected=m.music->buffer(0);const bool repeat=m.music->wave->format->data_bytes!=0;c.argument=repeat;return env.fill_music(*m.music,selected,repeat)<0?Result::Remove:Result::Advance;}
            if(c.step==7){env.play_music(*m.music);return Result::Advance;}
            return c.step<20?Result::Advance:Result::Remove;
        }
        if(!m.music)return Result::Remove;
        switch(c.step){
        case 0:env.stop_music(*m.music);return Result::Advance;
        case 1:if(m.music->notification_busy)return Result::Hold;env.recreate_music(*m.music);return Result::Advance;
        case 2:{const char* name=c.argument<0?c.filename:m.music_names[c.argument];const i32 index=track_index(name);env.select_music(*m.music->wave,m.formats+index);return Result::Advance;}
        case 3:{void* selected=m.music->buffer(0);env.reset_music(*m.music);const bool repeat=m.music->wave->format->data_bytes!=0;c.argument=repeat;return env.fill_music(*m.music,selected,repeat)<0?Result::Remove:Result::Advance;}
        case 4:env.play_music(*m.music);return Result::Advance;
        default:return c.step<7?Result::Advance:Result::Remove;
        }
    case 3:
        if(!m.music)return Result::Remove;if(!c.step){env.stop_music(*m.music);return Result::Advance;}return c.step==1?Result::Remove:Result::Advance;
    case 4:
        if(!m.music)return Result::Remove;
        switch(c.step){
        case 0:env.stop_music(*m.music);break;
        case 1:if(!m.music_thread)return Result::Remove;env.quit_thread(m.music_thread_id);break;
        case 2:if(env.wait_thread(m.music_thread,256)==0)m.music_thread=0;else{env.quit_thread(m.music_thread_id);c.step=add(c.step,-1);}break;
        case 3:env.close_handle(m.music_thread);env.close_handle(m.notification_event);m.music_thread=0;if(m.music){env.delete_music(*m.music);m.music=nullptr;}m.music=nullptr;break;
        case 10:return Result::Remove;
        }return Result::Advance;
    case 5:
        if(auto* music=*env.global_music){music->fade_mode=1;const i32 frames=(Extended::from_int(c.argument)*number(60)).truncate_int();music->fade_remaining=music->fade_duration=frames;}return Result::Remove;
    case 6:case 7:
        if(*env.music_enabled==1&&m.music){if(m.music->notification_busy)return Result::Hold;if(c.kind==6)env.pause_music(*m.music);else env.resume_music(*m.music);}return Result::Remove;
    case 8:if(m.music)env.volume_music(*m.music,m.music_volume);return Result::Remove;
    default:return Result::Hold;
    }
}
void AudioControl::update_effects(){
    auto& m=manager;auto& env=environment;
    for(u32 slot=0;slot<12;++slot){
        const i32 effect=m.pending_effects[slot];if(effect<0)break;const i32 count=m.pan_count[slot];m.pending_effects[slot]=-1;
        if(count<0){if(m.effect_buffers[effect])env.effect_stop(m.effect_buffers[effect]);m.pan_count[slot]=0;continue;}
        i32 sum=0;for(i32 i=0;i<count;++i){i32 pan;std::memcpy(&pan,reinterpret_cast<const u8*>(&m)+offsetof(AudioManager,pan_values)+(slot*128+static_cast<u32>(i))*4,4);sum=add(sum,pan);}
        if(!count||(sum==INT32_MIN&&count==-1))__builtin_trap();const i32 pan=sum/count;m.pan_count[slot]=0;
        if(!m.effect_buffers[effect])continue;
        env.effect_stop(m.effect_buffers[effect]);env.effect_position(m.effect_buffers[effect],0);env.effect_pan(m.effect_buffers[effect],pan);
        i32 gain=-10000;
        if(*env.effects_volume){const auto attenuation=number(1)-Extended::from_int(*env.effects_volume)*number(.01f);const i32 amplitude=static_cast<i32>(env.definitions[effect].volume)+5000;gain=add(((number(1)-attenuation*attenuation*attenuation)*Extended::from_int(amplitude)).truncate_int(),-5000);}
        env.effect_volume(m.effect_buffers[effect],gain);env.effect_play(m.effect_buffers[effect]);
    }
}
i32 AudioControl::update(){
    auto& m=manager;if(!m.driver)return 0;u32 index=0;
    for(;;){
        const auto result=execute(m.commands[index]);
        if(result==CommandResult::Advance){m.commands[index].step=add(m.commands[index].step,1);break;}
        if(result==CommandResult::Hold)break;
        // Keep the original cursor after shifting, including its immediate
        // continuation from the tail rather than from the first queued command.
        for(u32 count=0;count<31&&m.commands[index].kind;++count,++index)m.commands[index]=m.commands[index+1];
        if(result!=CommandResult::RemoveAndContinue)break;
    }
    if(*environment.effects_enabled)update_effects();return m.commands[0].kind;
}
}
