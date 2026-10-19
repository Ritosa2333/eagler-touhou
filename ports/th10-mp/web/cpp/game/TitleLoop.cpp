#include "TitleLoop.hpp"
#include <initializer_list>
namespace th10 {
// 0x42cdf0 / 0x42d2e0. The original idle demo, menu dispatch and timer form a
// single frame update. Returning from a demo restores the menu difficulty.
i32 update_title(TitleMenu& t,TitleLoopEnvironment& env){
    auto& game=*env.game;
    if(t.screen==1||t.screen==2){
        *env.inactive_frames=wrapping_add(*env.inactive_frames,1);
        if(*env.held&0x160b)*env.inactive_frames=0;
        else if(*env.inactive_frames>=900){
            game.flags=(game.flags&~0x40u)|0x20;const char* source=env.demo_files[*env.demo_index];char* destination=env.replay_filename;do{*destination++=*source;}while(*source++);
            auto* replay=env.load_replay(env.replay_filename);*env.demo_index=wrapping_add(*env.demo_index,1)%4;
            i32 stage=0;while(stage<8&&!replay->readers[stage].stage)++stage;game.stage=stage;game.reserved_040=stage;*env.pending_screen=12;
            game.character=replay->info->character;*env.current_stage=env.stages+stage;game.shot_type=replay->info->shot_type;game.reserved_038=game.difficulty;game.difficulty=replay->info->difficulty;
            env.delete_replay(replay);*env.inactive_frames=0;*env.return_screen=t.screen!=1;
        }
    }
    switch(t.screen){
    case 0:{
        const auto returning=*env.return_screen;
        for(i32 index:{6,7,2,3,0})env.registry->discard_file(env.files[index]);
        env.registry->interrupt(*env.loading_animation,1);*env.loading_animation=0;
        if(returning==3){t.menu.item_count=10;t.menu.select(0);t.menu.push();t.set_screen(15,env.rate);t.create_script(91,env);t.signal_script(91,9,env);t.draw_entry->flags|=2;*env.return_screen=1;env.update_menu(t,15);break;}
        auto next=returning;
        if(!(game.flags&0x20)){env.play_title_music();next=*env.return_screen;}else game.difficulty=game.reserved_038;
        game.flags&=~0x20u;
        if(next==0){t.set_screen(1,env.rate);*env.return_screen=1;env.update_menu(t,1);}
        else if(next==1){if(game.difficulty==4)t.menu.select(1);t.set_screen(2,env.rate);t.create_script(91,env);t.draw_entry->flags|=2;env.update_menu(t,2);}
        else if(next==2){t.menu.item_count=10;t.menu.select(3);t.menu.push();t.set_screen(12,env.rate);t.create_script(91,env);t.signal_script(91,9,env);t.draw_entry->flags|=2;*env.return_screen=1;env.update_menu(t,12);}
        else env.update_menu(t,1);
        break;}
    case 2:t.draw_entry->flags|=2;env.update_menu(t,2);break;
    case 3:*env.pending_screen=((*env.engine_flags&0x1000)?0:1)|2;[[fallthrough]];
    case 10:case 13:env.stop_music();break;
    case 1:case 4:case 5:case 6:case 7:case 8:case 9:case 11:case 12:case 14:case 15:case 16:env.update_menu(t,t.screen);break;
    }
    t.elapsed.tick();return 1;
}
// 0x42d260 / 0x42d2f0. Only menus with dynamic score text need a draw callback.
i32 draw_title(TitleMenu& t,TitleLoopEnvironment& env){switch(t.screen){case 9:case 11:case 12:case 15:case 16:env.draw_menu(t,t.screen);break;}return 1;}
}
