#include "GuiResources.hpp"
#include "Dialogue.hpp"
#ifdef TH_ENABLE_THPRAC
#include "PracticeRuntime.hpp"
#endif
namespace th10 {
// 0x413a20. A transition may transfer the existing MSG file through the cache.
i32 GuiResources::load_stage(){
    auto& env=environment;
    gui.stage_animations=env.load_animations(28,(*env.stage)->interface_animations);
    if(!gui.stage_animations){env.report_error();return -1;}
    if(!*env.cached_message){const char* name=(*env.stage)->messages[env.game->character];env.filename[0]=0;std::memcpy(env.filename,name,std::strlen(name)+1);gui.message_file=env.read_file(env.filename);if(!gui.message_file){env.report_error();return -1;}}
    else{gui.message_file=*env.cached_message;*env.cached_message=nullptr;}
    if(!(gui.elapsed_flags&1)){gui.elapsed.rate=env.rate;gui.elapsed_flags|=1;}gui.elapsed.initialize(-1);
    gui.countdown=gui.previous_countdown=-1;gui.displayed_score=env.game->score;return 0;
}
// 0x413980. Loading succeeds before disabled callbacks are installed.
i32 GuiResources::start(){auto& env=environment;gui.animations=env.load_animations(6,"front.anm");if(!gui.animations){env.report_error();return -1;}if(load_stage())return -1;gui.update_entry=(*env.chain)->add(env.update_callback,&gui,24,false,false,*env.callbacks);gui.draw_entry=(*env.chain)->add(env.draw_callback,&gui,43,true,false,*env.callbacks);return 0;}
// 0x413b50 / 0x413b90. These act on the shared slot without changing the GUI.
i32 GuiResources::preload(GuiResourceEnvironment& env){if(env.load_animations(6,"front.anm"))return 0;env.report_error();return -1;}
i32 GuiResources::release_front(GuiResourceEnvironment& env){auto*& file=env.slots[6];if(file){env.release_animations(*file);env.delete_object(file);file=nullptr;}return 0;}
// 0x413bc0. The original intentionally does not retain the script-1 background
// ID after creation, so repeated activation must preserve that behavior.
void GuiResources::activate(){
    auto& env=environment;
    if(gui.update_entry)gui.update_entry->flags|=2;if(gui.draw_entry)gui.draw_entry->flags|=2;
    if(!gui.border)gui.border=env.create_animation(*gui.animations,0);
    if(!gui.background)env.create_animation(*gui.animations,1);
    if(!(gui.high_score_digits[0].flags&1)){
        for(i32 i=0;i<10;++i){env.initialize_animation(*gui.animations,gui.high_score_digits[i],i+10);env.initialize_animation(*gui.animations,gui.score_digits[i],i+20);}
        for(i32 i=0;i<9;++i)env.initialize_animation(*gui.animations,gui.life_icons[i],i+30);
        for(i32 i=0;i<4;++i)env.initialize_animation(*gui.animations,gui.power_digits[i],i+47);
        for(i32 i=0;i<2;++i)env.initialize_animation(*gui.animations,gui.countdown_digits[i],i+80);
        for(i32 i=0;i<7;++i)env.initialize_animation(*gui.animations,gui.faith_digits[i],i+51);
    }
    gui.update_lives(env.game->lives);
    const i32 whole=env.game->power/20,fraction=(env.game->power%20)*100/20;
    env.bind_sprite(*gui.animations,gui.power_digits[0],wrapping_add(whole,8));env.bind_sprite(*gui.animations,gui.power_digits[2],wrapping_add(fraction/10,8));env.bind_sprite(*gui.animations,gui.power_digits[3],wrapping_add(fraction%10,8));
    // thprac_th10.cpp:2314 th10_logo. A custom practice start at a section
    // skips the stage-entry logo/title card (scripts 0/1 of stgXXlogo.anm),
    // except a stage-warp portion 1, which keeps the normal stage title.
    bool show_logo=true;
#ifdef TH_ENABLE_THPRAC
    if(env.practice)show_logo=!practice_skip_stage_logo(*env.practice);
#endif
    if(*env.current_screen!=8&&!(env.game->flags&0x20)&&show_logo){env.create_animation(*gui.stage_animations,0);env.create_animation(*gui.stage_animations,1);}
    if(env.game->flags&0x20)env.create_animation(*gui.animations,113);
    env.initialize_animation(**env.effects,gui.enemy_marker,0);
    if(env.game->stage==1&&!*env.controller_stage&&!env.game->score_units)env.create_animation(*gui.animations,79);
    if(*env.display_difficulty){gui.difficulty_badge=env.create_animation(*gui.animations,wrapping_add(env.game->difficulty,102));env.registry->interrupt(gui.difficulty_badge,3);}
    gui.difficulty_label=env.create_animation(*gui.animations,wrapping_add(env.game->difficulty,107));env.registry->interrupt(gui.difficulty_badge,3);gui.boss_lives=0;
}
// 0x414370. Preserve stage resources for retry/transition flags 1 or 8.
void GuiResources::unload_stage(){
    auto& env=environment;
    if(!(env.game->flags&9)){auto*& file=env.slots[28];if(file){env.release_animations(*file);env.delete_object(file);file=nullptr;}}
    else env.registry->discard_file(gui.stage_animations);
    gui.stage_animations=nullptr;
    if(gui.dialogue){gui.dialogue->release(*env.registry);env.delete_object(gui.dialogue);gui.dialogue=nullptr;}
    if(!(env.game->flags&9)){if(gui.message_file)env.free_bytes(gui.message_file);gui.message_file=nullptr;*env.cached_message=nullptr;}
    else *env.cached_message=gui.message_file;
    if(gui.update_entry)gui.update_entry->flags&=~2u;
    env.registry->delete_and_clear(gui.notification);env.registry->delete_and_clear(gui.power_notification);env.registry->delete_and_clear(gui.boss_name);
    for(auto& id:gui.boss_life_icons)id=0;
    for(auto& id:gui.bonus_digits)env.registry->delete_and_clear(id);
    gui.boss_lives=0;
}
// 0x414570. This earlier cleanup path leaves callback state and cache alone.
void GuiResources::discard_stage(){auto& env=environment;if(gui.dialogue){gui.dialogue->release(*env.registry);env.delete_object(gui.dialogue);gui.dialogue=nullptr;}if(!(env.game->flags&9)){auto*& file=env.slots[28];if(file){env.release_animations(*file);env.delete_object(file);file=nullptr;}gui.stage_animations=nullptr;if(gui.message_file)env.free_bytes(gui.message_file);gui.message_file=nullptr;}}
// 0x4145f0. Free embedded VM geometry in reverse construction order.
void GuiResources::shutdown(){auto& env=environment;unload_stage();(*env.chain)->remove_locked(gui.update_entry,*env.callbacks);(*env.chain)->remove_locked(gui.draw_entry,*env.callbacks);gui.update_entry=nullptr;env.registry->delete_and_clear(gui.difficulty_badge);for(auto& id:gui.bonus_digits)env.registry->delete_and_clear(id);env.registry->discard_file(gui.animations);*env.current=nullptr;
    auto release=[&](AnmVm& vm){if(vm.geometry)env.free_bytes(vm.geometry);vm.geometry=nullptr;};release(gui.enemy_marker);
    for(u32 i=7;i;--i)release(gui.faith_digits[i-1]);for(u32 i=2;i;--i)release(gui.countdown_digits[i-1]);for(u32 i=4;i;--i)release(gui.power_digits[i-1]);for(u32 i=9;i;--i)release(gui.life_icons[i-1]);for(u32 i=10;i;--i)release(gui.score_digits[i-1]);for(u32 i=10;i;--i)release(gui.high_score_digits[i-1]);
}
Gui* GuiResources::create(GuiResourceEnvironment& env){auto* gui=env.allocate();if(!gui)return nullptr;gui->initialize(env.current);GuiResources resources{*gui,env};if(resources.start()){resources.shutdown();env.delete_object(gui);return nullptr;}return gui;}
}
