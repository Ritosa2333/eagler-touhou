#pragma once
#include "TitleMenu.hpp"
namespace th10 {
struct MusicRoomEnvironment : TitleAnimationEnvironment {
    AnmFile** effects;
    AnmFile** comment_file;
    u8* unlocked;
    const u32* display_flags;
    const u32* pressed;
    const u16* repeated;
    const char* locked_title;
    const char* const* locked_comments;
    virtual char* read_file(i32& length)=0;
    virtual void free_file(char* memory)=0;
    virtual void text(AnmVm& vm,u32 color,const char* value)=0;
    virtual void numbered_text(AnmVm& vm,u32 color,i32 track_number,const char* title)=0;
    virtual void locked_text(AnmVm& vm,u32 color,i32 track_number)=0;
    virtual void sound(i32 sound)=0;
    virtual void music_command(i32 command)=0;
    virtual void load_music(const char* filename)=0;
    virtual void play_music()=0;
};
struct MusicRoom {
    TitleMenu& menu;
    MusicRoomEnvironment& environment;
    i32 update();
    void fill_comment();
    void layout(bool create);
    void leave();
};
char* skip_music_line(char* cursor,i32& remaining) noexcept;
char* copy_music_line(char* cursor,char* destination,i32& remaining) noexcept;
}
