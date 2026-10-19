#include "graphics_callbacks.hpp"

namespace th20::source::platform_window {
int switch_scene(GraphicsStatePrefix& g) { // 4da540, return values recovered from EAX
    if(g.field_0b08==g.field_0b0c) return 1;
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(5));
    g.field_0b10=g.field_0b08;g.clear_color=0xff000000;
    switch(g.field_0b0c) {
    case 0:
        g.field_0b0c=1;g.startup_scene=unrecovered::create_startup_scene();
        if(g.startup_scene) break;
        g.field_0b0c=3;[[fallthrough]];
    case 3:
        unrecovered::shutdown_scene_objects();return 4;
    case 4:
        switch(g.field_0b08) {
        case 1:case 2:unrecovered::create_menu();break;
        case 7:unrecovered::destroy_game();unrecovered::create_menu();break;
        case 15:runtime::retire_callback_owner(unrecovered::ending_scene);unrecovered::create_menu();break;
        }
        break;
    case 7:
        if(g.field_0b08==4) runtime::retire_callback_owner(unrecovered::menu_scene);
        g.field_0b18=1;unrecovered::create_game(0);break;
    case 10:
        unrecovered::destroy_game();g.field_0b0c=7;g.field_0b18=1;g.field_0b1c=0;
        unrecovered::restore_stage_selection();unrecovered::create_game(0);break;
    case 11:
        unrecovered::destroy_game();g.field_0b18=1;g.field_0b1c=0;g.field_0b0c=7;
        unrecovered::restore_stage_selection();unrecovered::create_game(1);break;
    case 12: {
        const int mode=unrecovered::game_restart_mode();g.field_0b18=0;
        if(g.field_0b08==7) unrecovered::destroy_game();
        g.field_0b0c=7;unrecovered::create_game(mode);break;
    }
    case 13:
        if(g.field_0b08==4) runtime::retire_callback_owner(unrecovered::menu_scene);
        g.field_0b0c=7;g.field_0b18=1;unrecovered::create_game(1);break;
    case 14:
        unrecovered::destroy_game();g.field_0b18=1;g.field_0b0c=7;unrecovered::create_game(0);break;
    case 15:
        if(g.field_0b08==7) unrecovered::destroy_game();
        unrecovered::create_ending_scene();break;
    case 16:
        switch(g.field_0b08) {
        case 2:g.field_0b0c=4;unrecovered::menu_selection=3;unrecovered::create_menu();break;
        case 7:unrecovered::destroy_game();g.field_0b0c=4;unrecovered::menu_selection=3;unrecovered::create_menu();break;
        case 15:runtime::retire_callback_owner(unrecovered::ending_scene);g.field_0b0c=4;unrecovered::menu_selection=3;unrecovered::create_menu();break;
        }
        break;
    case 17:
        unrecovered::shutdown_scene_objects();return 5;
    case 19:
        unrecovered::destroy_game();g.field_0b18=1;g.field_0b1c=1;g.field_0b0c=7;
        unrecovered::restore_stage_selection();unrecovered::create_game(0);break;
    case 22:
        unrecovered::destroy_game();g.field_0b0c=7;unrecovered::restore_stage_selection();unrecovered::create_game(0);break;
    case 24:
        if(unrecovered::stage_selection_changed()) g.field_0b0c=7;
        unrecovered::destroy_game();
        if(!unrecovered::stage_selection_changed()) g.field_0b0c=7;
        g.field_0b18=1;g.field_0b1c=0;unrecovered::restore_stage_selection();unrecovered::create_game(0);break;
    }
    g.field_0b08=g.field_0b0c;return 1;
}
}
