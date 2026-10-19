#pragma once
#include "AnmText.hpp"
#include "AsciiManager.hpp"
#include "Chain.hpp"
namespace th08 {
struct TrackDescriptor {char path[64]{},title[66]{},descriptions[8][66]{};};
static_assert(sizeof(TrackDescriptor)==658);
class MusicCatalog {
public:
    bool load(const u8*,u32);
    TrackDescriptor tracks[32]{};
    i32 count=0;
};
struct MusicRoomState {
    ChainElement *calculation=nullptr,*drawing=nullptr;
    AnmLoaded* music=nullptr;
    i32 unlocked[24]{};
    i32 frames=0,input_state=0,cursor=0,selected=0,listing_offset=0,count=0;
    TrackDescriptor* tracks=nullptr;
    AnmVm main,names[31],descriptions[8];
};
static_assert(sizeof(MusicRoomState)==0x6a28);
struct MusicRoomContext {
    u16 keys=0,previous=0;
    bool scrolling=false,preload=false,software_texturing=false;
    AnmLoaded* text=nullptr;
    i32 supervisor_state=0;
};
class MusicRoom;
struct MusicRoomActions {
    virtual ~MusicRoomActions()=default;
    virtual bool load(MusicRoom&)=0;
    virtual void release()=0;
    virtual void background()=0;
    virtual void start_bgm()=0;
    virtual void play_audio(const char* path)=0;
    virtual void capture_loading(const Vec3&)=0;
    virtual void fade_music(float seconds)=0;
};
class MusicRoom {
public:
    MusicRoom(AnmExecutor& executor,AnmRenderer& renderer,AsciiManager& ascii,TextWriter& text,MusicRoomActions& actions)
        :executor(executor),renderer(renderer),ascii(ascii),text(text),actions(actions){std::memset(&state,0,sizeof(state));}
    ~MusicRoom();
    MusicRoomState state;
    MusicRoomContext context;
    MusicCatalog catalog;
    bool initialize(AnmLoaded& music,const u8* comments,u32 size,const i8* unlocked);
    i32 enable_input();
    i32 process_input();
    JobResult update();
    JobResult draw();
    bool attach(Chain& chain);
    void detach();
private:
    AnmExecutor& executor;AnmRenderer& renderer;AsciiManager& ascii;TextWriter& text;MusicRoomActions& actions;
    Chain* chain=nullptr;
    void start(AnmVm&,AnmLoaded&,i32 script);
    void selection_interrupts();
    bool pressed(u16 mask) const noexcept;
    bool scrolling(u16 mask) const noexcept;
    static i32 added(void*);
    static i32 deleted(void*);
};
}
