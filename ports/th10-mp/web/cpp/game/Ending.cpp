#include "Ending.hpp"
namespace th10 {
static void set_time(Timer& timer,u32& flags,i32 frames,const float* rate){if(!(flags&1)){timer.rate=rate;flags|=1;}timer.previous=wrapping_add(frames,-1);timer.current=frames;timer.fractional=Extended::from_int(frames).to_float();}
static MessageInstruction* first_instruction(u8* file){u32 offset;std::memcpy(&offset,file+4,4);return reinterpret_cast<MessageInstruction*>(file+offset);}
void Ending::initialize(Ending** current) noexcept {std::memset(this,0,sizeof(*this));flags=2;*current=this;}
// 0x40ba90. The five text VMs are retained across pages of the ending.
void EndingScript::initialize(MessageInstruction* script,EndingEnvironment& env){
    std::memset(this,0,sizeof(*this));for(i32 i=0;i<5;++i){lines[i]=env.create_animation(**env.text_animations,i+66);auto& vm=*env.registry->find_and_clear(lines[i]);vm.text_settings[0]=16;vm.text_settings[1]=16;vm.reserved_360[0]|=2;}
    instruction=script;set_time(elapsed,elapsed_flags,0,env.rate);set_time(script_time,script_time_flags,0,env.rate);set_time(wait,wait_flags,0,env.rate);flags|=1;color=0xffffff;
}
// 0x40b3a0. The second stop follows the base worker's vtable reset.
void EndingScript::release(EndingEnvironment& env){loader.stop(env);for(auto& id:lines)env.registry->delete_and_clear(id);loader.original_virtual_table=env.thread_vtable;loader.stop(env);}
// 0x40b480. The old file is released even when replacement loading fails.
u8* Ending::load_file(const char* name,EndingEnvironment& env){env.filename[0]=0;std::memcpy(env.filename,name,std::strlen(name)+1);auto* bytes=env.read_file(env.filename);if(file){env.free_bytes(file);file=nullptr;}file=bytes;return bytes;}
// 0x40b560. Ending selection and unlocks include all six shot types, with
// separate good endings for runs above Easy that have not used a continue.
i32 Ending::start(EndingEnvironment& env){
    update_entry=(*env.chain)->add(env.update_callback,this,28,false,true,*env.callbacks);draw_entry=(*env.chain)->add(env.draw_callback,this,6,true,true,*env.callbacks);env.show_loading();
    ending=env.game->character*3+env.game->shot_type;if(!env.game->score_units&&env.game->difficulty>0)ending+=6;
    auto* unlocked=(*env.scores)->settings.statistics;if(!unlocked[ending+1])newly_unlocked|=1;if(!unlocked[13])newly_unlocked|=2;unlocked[ending+1]=1;if(ending>5)unlocked[13]=1;
    env.filename[0]=0;std::memcpy(env.filename,env.ending_files[ending],std::strlen(env.ending_files[ending])+1);file=env.read_file(env.filename);if(!file){env.report_error();return -1;}
    script=static_cast<EndingScript*>(env.allocate(sizeof(EndingScript)));if(script)script->initialize(first_instruction(file),env);return 0;
}
// 0x40b7b0. The capture and four ending animation slots have separate owners.
void Ending::shutdown(EndingEnvironment& env){
    (*env.chain)->remove_locked(update_entry,*env.callbacks);(*env.chain)->remove_locked(draw_entry,*env.callbacks);env.release_capture();if(script){script->release(env);env.delete_object(script);}script=nullptr;
    for(i32 slot=29;slot<33;++slot){auto*& animations=env.animation_slots[slot];if(animations){env.release_animations(*animations);env.delete_object(animations);animations=nullptr;}}if(file){env.free_bytes(file);file=nullptr;}file=nullptr;*env.current=nullptr;*env.menu_state=0;
}
Ending* Ending::create(EndingEnvironment& env){auto* ending=static_cast<Ending*>(env.allocate(sizeof(Ending)));if(!ending)return nullptr;ending->initialize(env.current);if(ending->start(env)){ending->shutdown(env);env.delete_object(ending);return nullptr;}return ending;}
// 0x40bd80. MSG commands share their byte encoding with dialogue, but the
// ending has its own page waits, resource worker and credit sequence.
i32 EndingScript::update(EndingEnvironment& env){
    if(flags&4)return 0;
    if((*env.pressed&0x1001)||(!(flags&2)&&(*env.held&0x100)&&(flags&1)))set_time(script_time,script_time_flags,instruction->time,env.rate);
    while(script_time.current>=instruction->time){
        switch(instruction->opcode){
        case 0:return -1;
        case 3:{
            if(!next_line){for(auto& id:lines){env.draw_text(env.registry->find_and_clear(id),0xffffff," ");env.registry->interrupt(id,3);}}
            auto& id=lines[next_line];auto* vm=env.registry->find_and_clear(id);auto* text=Dialogue::decode(instruction->payload(),env.decoded_text);env.draw_text(vm,color,text);env.registry->interrupt(id,2);++next_line;if(next_line>=5)next_line=0;break;
        }
        case 4:for(u32 id:lines)env.registry->interrupt(id,3);break;
        case 5:case 6:{
            const bool new_page=instruction->opcode==6;if(wait.current<1)set_time(wait,wait_flags,instruction->argument<i32>(),env.rate);wait.advance(-1);
            if(!new_page&&instruction->argument<i32>()<0)set_time(wait,wait_flags,999,env.rate);
            if(!(*env.pressed&0x1001)&&wait.current>0){if(!(*env.held&0x100)||(instruction->argument<i32>()>=0&&wait.current%6))return 0;}else env.sound(0);
            set_time(wait,wait_flags,0,env.rate);if(new_page){next_line=0;*env.menu_state=0;}break;
        }
        case 7:env.show_loading();env.unload_animations(instruction->argument<i32>()+29);pending_animation_name=reinterpret_cast<const char*>(instruction->payload()+4);flags|=4;loader.start(env.loader_callback,this,false,env);instruction=instruction->next();return 0;
        case 8:case 15:case 16:case 17:{
            if(instruction->opcode>=15&&env.game->difficulty!=instruction->opcode-14)break;
            auto& id=animations[instruction->argument<i32>()];env.registry->delete_and_clear(id);id=env.create_animation(*files[instruction->argument<i32>(1)],instruction->argument<i32>(2));break;
        }
        case 9:color=instruction->argument<u32>();break;
        case 10:{const auto* name=reinterpret_cast<const char*>(instruction->payload());env.load_music(name);env.play_music(std::strcmp(name,"bgm/th10_13.wav")==0?15:16);break;}
        case 11:env.fade_music(3);flags&=~1u;break;
        case 12:{
            for(auto& id:lines)env.registry->delete_and_clear(id);auto* file=(*env.current)->load_file(reinterpret_cast<const char*>(instruction->payload()),env);if(!file)return -1;
            std::memset(this,0,sizeof(*this));instruction=first_instruction(file);set_time(elapsed,elapsed_flags,0,env.rate);set_time(script_time,script_time_flags,0,env.rate);set_time(wait,wait_flags,0,env.rate);color=0xffffff;flags|=2;continue;
        }
        case 13:case 14:env.fade(instruction->opcode==13?0:5,instruction->argument<i32>());break;
        default:break;
        }
        instruction=instruction->next();
    }
    script_time.tick();return 0;
}
// 0x40bd20 / 0x40b9f0. Credits fast-forward restarts the update chain eleven
// times between normal frame returns. A resource wait still advances elapsed.
i32 EndingScript::tick(EndingEnvironment& env){if(update(env))return 1;elapsed.tick();return 0;}
i32 Ending::update(EndingEnvironment& env){if(script->tick(env)){*env.pending_screen=(*env.engine_flags&0x1000)?2:15;return 1;}frames=wrapping_add(frames,1);if(!(script->flags&4)&&(script->flags&2)&&(*env.held&0x100)&&frames%12)return 6;return 1;}
// 0x40c3c0. Preserve both reads of the current instruction's slot around the
// blocking upload: the instruction has advanced before the worker executes.
i32 EndingScript::load_animations(EndingEnvironment& env){env.filename[0]=0;std::memcpy(env.filename,pending_animation_name,std::strlen(pending_animation_name)+1);auto* file=env.load_animations(instruction->argument<i32>()+29,env.filename);files[instruction->argument<i32>()]=file;flags&=~4u;env.registry->interrupt(*env.loading_animation,1);*env.loading_animation=0;return 0;}
}
