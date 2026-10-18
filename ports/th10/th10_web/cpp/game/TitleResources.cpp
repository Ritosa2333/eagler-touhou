#include "TitleResources.hpp"
namespace th10 {
// 0x42caa0. Both callbacks are registered disabled before the ANM files load.
i32 TitleResources::start(){auto& env=environment;
    title.update_entry=(*env.chain)->add(env.update_callback,&title,6,false,false,*env.callbacks);
    title.draw_entry=(*env.chain)->add(env.draw_callback,&title,3,true,false,*env.callbacks);
    title.animations=env.load_animations(25,"title.anm");
    if(title.animations){title.version_animations=env.load_animations(26,"title_v.anm");if(title.version_animations){title.menu.wrap=1;*env.menu_state=0;return 0;}}
    env.report_error();return -1;
}
// 0x42c9f0. Resource loading can finish before the startup screen's 300 frames.
// The platform decides when its startup presentation is complete.
i32 TitleResources::load(TitleResourceEnvironment& env){
    TitleLoadingTask task;while(!task.advance(env))env.sleep(16);return 0;
}
bool TitleLoadingTask::advance(TitleResourceEnvironment& env){
    if(done)return true;
    if(!started){started=true;if(TitleResources{**env.current,env}.start()){*env.pending_screen=((*env.engine_flags&0x1000)?0:1)|2;done=true;return true;}waiting_for_startup=*env.startup!=nullptr;}
    if(waiting_for_startup){if(!env.startup_presentation_complete(**env.startup)&&!(*env.engine_flags&0x80))return false;auto*& file=env.slots[1];if(file){env.release_animations(*file);env.delete_object(file);file=nullptr;}}
    (*env.current)->update_entry->flags|=2;done=true;return true;
}
// 0x42cd50. The loader is a resource worker, separate from the frame callback.
TitleMenu* TitleResources::create(TitleResourceEnvironment& env){auto* title=env.allocate();if(!title)return nullptr;title->initialize(env.current);title->loader.start(env.loader_callback,title,false,env);return title;}
// 0x42cb60. Stop the worker before releasing callbacks and resources. As in
// the original, the second stop happens after resetting the worker's vtable.
void TitleResources::shutdown(){auto& env=environment;
    title.original_virtual_table=env.title_vtable;title.loader.stop(env);
    (*env.chain)->remove_locked(title.update_entry,*env.callbacks);(*env.chain)->remove_locked(title.draw_entry,*env.callbacks);
    for(i32 slot=25;slot<=26;++slot){auto*& file=env.slots[slot];if(file){env.release_animations(*file);env.delete_object(file);file=nullptr;}}
    for(auto* replay:title.previews)if(replay)env.delete_replay(replay);
    env.registry->interrupt(title.animation_ids[195],1);
    if(title.music_file){env.free_file(title.music_file);title.music_file=nullptr;}
    *env.current=nullptr;title.loader.original_virtual_table=env.thread_vtable;title.loader.stop(env);
}
}
