#include "../../cpp/game/TitleControllerMenu.hpp"
struct TitleControllerFixture final:TitleControllerServices {
 TitleMainFixture& title;TitleControllerSettings settings,saved;TitleControllerMenu menu;u32 save_calls=0;
 explicit TitleControllerFixture(TitleMainFixture& t):title(t),menu(t.state,t.animations,settings,*this){}
 bool sound(i32 id)override{title.sounds.push_back(id);return true;}
 bool save(const TitleControllerSettings& s)override{saved=s;save_calls++;return true;}
};
API(title_controller_create) TitleControllerFixture* title_controller_create(TitleMainFixture* t){t->state.screen=TitleScreen::Controller;return new TitleControllerFixture(*t);}
API(title_controller_delete) void title_controller_delete(TitleControllerFixture* f){delete f;}
API(title_controller_field) void* title_controller_field(TitleControllerFixture* f,u32 which){switch(which){case 0:return &f->settings;case 1:return f->menu.pending.data();default:return &f->saved;}}
API(title_controller_update) i32 title_controller_update(TitleControllerFixture* f,u32 pressed,u32 repeated,u32 gamepad){f->title.sounds.clear();return f->menu.update(pressed,repeated,gamepad);}
API(title_controller_error) const char* title_controller_error(TitleControllerFixture* f){return f->menu.error.c_str();}
