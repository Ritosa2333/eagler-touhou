#include "StartupScreen.hpp"
#include "HighRefresh.hpp"
namespace th10 {
// 0x41f850.
void StartupScreen::initialize(StartupScreen** current) noexcept {std::memset(this,0,sizeof(*this));flags=2;*current=this;}
// 0x41fac0. Loading does not run on the update chain; its worker enables the
// draw and update callbacks only after their respective resources are ready.
i32 StartupScreen::start(StartupEnvironment& env){update_entry=(*env.chain)->add(env.update_callback,this,3,false,false,*env.callbacks);draw_entry=(*env.chain)->add(env.draw_callback,this,2,true,false,*env.callbacks);loader.start(env.loader_callback,this,false,env);return 0;}
void StartupEnvironment::release_slot(i32 slot){if(slots[slot]){release_animations(*slots[slot]);delete_object(slots[slot]);slots[slot]=nullptr;}}
// 0x41f8d0 / 0x41f930.
i32 StartupScreen::load_stage_assets(StartupEnvironment& env){if(!env.load_animations(6,env.front_name)){env.report(StartupError::FrontAnimations);return -1;}if(!env.load_animations(7,env.bullet_name)){env.report(StartupError::BulletAnimations);return -1;}return 0;}
i32 StartupScreen::release_stage_assets(StartupEnvironment& env){env.release_slot(6);env.release_slot(7);return 0;}
// 0x41f990. Failure still enables the update callback and returns zero. Missing
// music-format data is reported but the original continues loading audio and scores.
i32 StartupScreen::load(StartupEnvironment& env){
    const auto fail=[&](){*env.pending_screen=3;update_entry->flags|=2;return 0;};
    opening_file=env.load_animations(1,env.opening_name);if(!opening_file)return fail();
    draw_entry->flags|=2;opening_ready=1;
    if(!env.create_common()){env.report(StartupError::CommonResources);return fail();}
    resources_ready=1;*env.loading_animations=env.load_animations(0,env.text_name);if(!*env.loading_animations)return fail();
    *env.music_format=env.read_file(env.format_name);if(!*env.music_format)env.report(StartupError::MusicFormat);
    env.initialize_audio();if(env.file_exists(env.music_name)){if(!(*env.display_flags&16))env.load_music(env.music_name);else std::memcpy(env.music_filename,env.music_name,10);}
    load_stage_assets(env);update_entry->flags|=2;env.create_scores();return 0;
}
// 0x41fb50. The original joins its worker twice, with resource destruction
// between the joins, and saves score data before releasing that allocation.
void StartupScreen::shutdown(StartupEnvironment& env){
    loader.stop(env);(*env.chain)->remove_locked(update_entry,*env.callbacks);(*env.chain)->remove_locked(draw_entry,*env.callbacks);
    release_stage_assets(env);env.release_slot(1);auto* resources=*env.common;*env.current=nullptr;if(resources)env.delete_common(*resources);env.release_slot(0);
    env.save_scores();env.delete_scores();if(animation.geometry)env.free_bytes(animation.geometry);animation.geometry=nullptr;
    loader.original_virtual_table=env.thread_vtable;loader.stop(env);
}
// 0x41fd00.
StartupScreen* StartupScreen::create(StartupEnvironment& env){auto* value=env.allocate();if(!value)return nullptr;value->initialize(env.current);if(value->start(env)){value->shutdown(env);env.delete_object(value);return nullptr;}return value;}
// 0x41fd90 / 0x41feb0.
i32 StartupScreen::update(StartupEnvironment& env){if(flags&2){(*env.common)->enable();*env.engine_flags&=~0x1000u;*env.pending_screen=4;flags&=~2u;}return 1;}
// 0x41fdd0 / 0x41fef0.
i32 StartupScreen::draw(StartupEnvironment& env){if(!high_refresh::render_only){if(opening_ready==1){opening_animation=env.create_opening_animation(*opening_file);opening_ready=wrapping_add(opening_ready,1);}if(resources_ready==1){auto& resources=**env.common;if(!resources.loading_animation)resources.loading_animation=env.create_loading_animation(*resources.effects);resources_ready=wrapping_add(resources_ready,1);}elapsed=wrapping_add(elapsed,1);}return 1;}
}
