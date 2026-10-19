#pragma once
#include "TitleAnimations.hpp"
#include "Dialogue.hpp"
#include <vector>
namespace th15 {
struct MusicComment {std::string file,title;std::array<std::string,8> comments;};
class MusicComments {
 std::vector<MusicComment> entries;
public:
 std::string error;bool open(const u8* bytes,u32 size);
 u32 count()const noexcept{return entries.size();}
 const MusicComment* entry(u32 index)const noexcept{return index<entries.size()?&entries[index]:nullptr;}
};
struct TitleMusicServices {
 virtual ~TitleMusicServices()=default;
 virtual bool text(AnmVm&,const DialogueText&)=0;
 virtual bool sound(i32)=0;virtual bool music(const std::string&)=0;
 virtual bool music_command(i32)=0;
 virtual bool start_title_music()=0;
};
class TitleMusicRoom {
 TitleState& state;AnmManager& animations;TitleAnimations visuals;const MusicComments& catalog;TitleMusicServices& services;i32 text_bank,title_bank;
 bool check(bool);bool paint_comment();bool create_entries();bool position_entries();bool cancel();bool interrupt(u32,i32);bool text(u32,const std::string&,u32 color,bool detail=false);
public:
 i32 scroll=0,comment_line=0,comment_track=0;bool warning=false;std::array<bool,20> unlocked{};bool alternate_audio=false;std::string error;
 TitleMusicRoom(TitleState& s,AnmManager& a,const MusicComments& c,TitleMusicServices& services,i32 title=16,i32 ascii=5,i32 text=2):state(s),animations(a),visuals(s,a,title,ascii),catalog(c),services(services),text_bank(text),title_bank(title){}
 bool update(u32 pressed,u32 repeated);
};
}
