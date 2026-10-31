#include "../../cpp/game/TitleOptionsMenu.hpp"
struct TitleOptionsFixture final:TitleOptionsServices {
 TitleMainFixture& title;TitleAudioSettings settings;TitleOptionsMenu menu;
 std::array<i32,3> volumes{};u32 volume_calls=0;
 explicit TitleOptionsFixture(TitleMainFixture& t):title(t),menu(t.state,settings,t.animations,*this){}
 bool sound(i32 id)override{title.sounds.push_back(id);return true;}
 bool volume(i32 bgm,i32 se,i32 attenuation)override{volumes={bgm,se,attenuation};volume_calls++;return true;}
};
API(title_options_create) TitleOptionsFixture* title_options_create(TitleMainFixture* t){return new TitleOptionsFixture(*t);}
API(title_options_delete) void title_options_delete(TitleOptionsFixture* p){delete p;}
API(title_options_settings) TitleAudioSettings* title_options_settings(TitleOptionsFixture* p){return &p->settings;}
API(title_options_update) bool title_options_update(TitleOptionsFixture* p,u32 pressed,u32 repeated){p->title.sounds.clear();p->volume_calls=0;return p->menu.update(pressed,repeated);}
API(title_options_error) const char* title_options_error(TitleOptionsFixture* p){return p->menu.error.c_str();}
API(title_options_volumes) const i32* title_options_volumes(TitleOptionsFixture* p){return p->volumes.data();}
API(title_options_volume_calls) u32 title_options_volume_calls(TitleOptionsFixture* p){return p->volume_calls;}
