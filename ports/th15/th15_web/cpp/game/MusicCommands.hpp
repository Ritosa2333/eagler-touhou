#pragma once
#include "MusicLayout.hpp"
#include <array>
namespace th15 {
struct MusicRequest {i32 code=0,value=0,step=0;std::array<char,256> text{};};
static_assert(sizeof(MusicRequest)==268);
struct MusicCommandOutput {
 virtual ~MusicCommandOutput()=default;
 virtual bool music_ready()const=0;virtual bool music_busy()const=0;virtual bool music_has_intro()const=0;virtual bool release_pending()const=0;virtual bool release_waiting()=0;
 virtual void preload_reset()=0;virtual bool prepare_music(i32,const std::string&)=0;virtual bool cached_music(i32)=0;
 virtual void stop_music_stream(bool release)=0;virtual void rewind_music_stream()=0;virtual bool load_music(const std::string&,i32)=0;virtual void select_music(i32)=0;virtual i32 fill_music(bool alternate,bool initial)=0;virtual void start_music_stream()=0;
 virtual void signal_music_release()=0;virtual void close_music_stream()=0;virtual void fade_music_stream(i32)=0;virtual void pause_music_stream(bool)=0;virtual void refresh_music_volume()=0;virtual void switch_music_wave(i32)=0;
};
// Only the original command staging lives here. File/decoder work belongs to
// the SDL backend; queues never contain executable addresses or device handles.
class MusicCommands {
 MusicLayout& layout;MusicCommandOutput& output;bool fail(const char*);void pop();
public:
 std::array<MusicRequest,32> requests{};std::array<std::string,32> prepared{};std::string current_wave,error;bool alternate=false;u8 music_mode=1;
 MusicCommands(MusicLayout& l,MusicCommandOutput& o):layout(l),output(o){}
 bool enqueue(i32 code,i32 value,const std::string& text="");
 bool preload_track(i32 slot,const std::string& stem){return enqueue(1,slot,stem+".wav");}
 bool process();bool pending()const noexcept{return requests[0].code!=0;}
};
}
