#pragma once
#include "../../cpp/game/TitleMenus.hpp"
#include <map>
namespace title_test {
using namespace th09;
struct Fixture:ResourceReader,ResourceTextures,TitleServices {
    std::map<std::string,std::vector<u8>> files;Rng random{0x7531,0,0};AnmExecutor executor{random};GameResources resources{*this,*this,executor};PlayerRecords records;TitleSettings settings;TitleMenus menu{resources,*this,random,settings};
    std::vector<i32> events;std::vector<u8> text;u32 handles=0;WorldConfiguration launched;
    bool read(const char* n,std::vector<u8>& out)override{const auto it=files.find(n);if(it==files.end())return false;out=it->second;return true;}
    TextureAllocation create(const AnmTextureSource& s,const u8*,u32)override{return {++handles,s.width,s.height};}void destroy(u32)override{}
    void event(i32 a,i32 b=0){events.push_back(a);events.push_back(b);}
    void string(const char* p){text.insert(text.end(),p,p+std::strlen(p));text.push_back(0);}
    bool title_background(const char* n)override{event(0);string(n);return true;}
    void title_sound(i32 n)override{event(1,n);}void title_music(i32 n)override{event(2,n);}
    void title_music_file(const char* n)override{event(3);string(n);}void title_music_pause(bool p)override{event(4,p);}void title_music_fade()override{event(5);}
    void title_text(AnmVm&,const char* value,u32 color,u32 shadow)override{event(6,i32(color));event(7,i32(shadow));string(value);}
    void title_sprite(AnmVm&,bool)override{}void title_begin_draw()override{}void title_configuration()override{event(8);}
    PlayerRecords& title_records()override{return records;}
    void title_save_records()override{event(12);}
    struct Text {Vec3 position;u32 color;Vec2 scale;std::string text;};std::vector<Text> ascii;
    void title_ascii(const Vec3& p,const char* s,u32 color,const Vec2& scale)override{ascii.push_back({p,color,scale,s});}
    i32 joy_button[2]{32,32};i32 title_joy_button(i32 device)override{return joy_button[device];}
    bool title_read_replay(const char* n,std::vector<u8>& out)override{std::string name=n;if(name.rfind("./",0)==0)name.erase(0,2);return read(name.c_str(),out);}
    std::vector<std::string> title_imported_replays()override{std::vector<std::string> names;for(const auto& item:files)if(item.first.rfind("replay/th9_ud",0)==0)names.push_back(item.first.substr(7));return names;}
    bool title_save_replay(const char* path,const char* name)override{event(13);string(path);string(name);return true;}
    bool title_replay_exists(const char* path)override{std::vector<u8> bytes;return title_read_replay(path,bytes);}
    void title_play_replay(const ReplayFile& r,u32 round,const char* path)override{event(14,round);string(path);}
    u32 title_clear_count(i32,i32)override{return 0;}
    void title_launch(const WorldConfiguration& c)override{launched=c;event(9,i32(c.selection.mode));}
    void title_demo(u32 n)override{event(10,n);}void title_exit()override{event(11);}
    void clear(){events.clear();text.clear();}
    void configure(i32 difficulty,i32 versus,u32 unlocked,bool extra){settings.difficulty=u8(difficulty);settings.versus=versus;for(i32 n=0;n<16;++n)settings.story_unlocked[n]=settings.versus_unlocked[n]=settings.extra_unlocked[n]=bool(unlocked&(1u<<n));}
};
#define TITLE_FIELD(n,f) {n,offsetof(TitleState,f),sizeof(TitleState::f)}
constexpr u32 fields[][3]={TITLE_FIELD(0,selection),TITLE_FIELD(4,name_cursor),TITLE_FIELD(8,ranking_character),TITLE_FIELD(12,ranking_slot),TITLE_FIELD(0xc920,key_player),TITLE_FIELD(16,character_selection),TITLE_FIELD(24,confirmed),TITLE_FIELD(36,previous_selection),TITLE_FIELD(40,state),TITLE_FIELD(44,frames),TITLE_FIELD(0x84,previous_screen),TITLE_FIELD(0xc900,replay_scroll),TITLE_FIELD(0xc904,replay_count),TITLE_FIELD(0xc908,replay_selected),TITLE_FIELD(0xc910,animation_frames),TITLE_FIELD(0xc918,layout_changed),TITLE_FIELD(0x11b74,current_music),TITLE_FIELD(0x11b78,music_count),TITLE_FIELD(0x11b7c,music_scroll),TITLE_FIELD(0x11b80,music_paused),TITLE_FIELD(0x11b84,health),TITLE_FIELD(0x11b8c,health_adjustment),TITLE_FIELD(0x1b228,screen),TITLE_FIELD(0x1b230,load_frame),TITLE_FIELD(0x1b234,idle_frames),TITLE_FIELD(0x1b23c,return_to_versus),TITLE_FIELD(0x1b240,selection_base),TITLE_FIELD(0x1b244,selection_count)};
#undef TITLE_FIELD
}

