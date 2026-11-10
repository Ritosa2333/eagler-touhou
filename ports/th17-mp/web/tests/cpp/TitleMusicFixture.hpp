#include "../../cpp/game/TitleMusicRoom.hpp"
struct TitleMusicFixture final:TitleMusicServices {
 struct Text {u32 handle,color;std::string bytes;};TitleMainFixture& title;MusicComments comments;TitleMusicRoom menu;std::vector<Text> texts;std::vector<std::string> events;
 explicit TitleMusicFixture(TitleMainFixture& t):title(t),menu(t.state,t.animations,comments,*this){}
 bool text(AnmVm& vm,const TextureText& t)override{texts.push_back({title.animations.registry.handle(vm),t.color,t.bytes});return true;}
 bool sound(i32 id)override{events.push_back("sound:"+std::to_string(id));return true;}
 bool music(const std::string& file)override{events.push_back("music:"+file);return true;}
 bool music_command(i32 id)override{events.push_back("command:"+std::to_string(id));return true;}bool start_title_music()override{events.push_back("command:0");return true;}
};
API(title_music_create) TitleMusicFixture* title_music_create(TitleMainFixture* t){return new TitleMusicFixture(*t);}
API(title_music_delete) void title_music_delete(TitleMusicFixture* f){delete f;}
API(title_music_configure) i32 title_music_configure(TitleMusicFixture* f,const u8* p,u32 n,u32 mask,i32 alternate){f->title.state.screen=TitleScreen::MusicRoom;f->title.state.age.set(1);for(u32 i=0;i<18;i++)f->menu.unlocked[i]=(mask>>i)&1;f->menu.alternate_audio=alternate;return f->comments.open(p,n);}
API(title_music_update) i32 title_music_update(TitleMusicFixture* f,u32 pressed,u32 repeated){f->texts.clear();f->events.clear();return f->menu.update(pressed,repeated);}
API(title_music_value) i32 title_music_value(TitleMusicFixture* f,u32 which){switch(which){case 0:return f->menu.scroll;case 1:return f->menu.comment_line;case 2:return f->menu.comment_track;case 3:return f->menu.warning;case 4:return f->comments.count();default:{u32 mask=0;for(u32 i=0;i<18;i++)if(f->menu.unlocked[i])mask|=1u<<i;return mask;}}}
API(title_music_event_count) u32 title_music_event_count(TitleMusicFixture* f){return f->events.size();}
API(title_music_event) const char* title_music_event(TitleMusicFixture* f,u32 i){return f->events[i].c_str();}
API(title_music_text_count) u32 title_music_text_count(TitleMusicFixture* f){return f->texts.size();}
API(title_music_text_value) u32 title_music_text_value(TitleMusicFixture* f,u32 i,u32 which){return which?f->texts[i].color:f->texts[i].handle;}
API(title_music_text_bytes) const char* title_music_text_bytes(TitleMusicFixture* f,u32 i){return f->texts[i].bytes.c_str();}
API(title_music_catalog) const char* title_music_catalog(TitleMusicFixture* f,u32 i,u32 which){const auto* e=f->comments.entry(i);return !e?nullptr:which==0?e->file.c_str():which==1?e->title.c_str():e->comments[which-2].c_str();}
API(title_music_error) const char* title_music_error(TitleMusicFixture* f){return f->comments.error.empty()?f->menu.error.c_str():f->comments.error.c_str();}
