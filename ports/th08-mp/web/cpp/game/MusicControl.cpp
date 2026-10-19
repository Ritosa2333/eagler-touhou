#include "MusicControl.hpp"
namespace th08 {
bool MusicControl::wave_path(const char* path,char (&out)[260]){
    if(!path)return false;const auto size=std::strlen(path);if(size>=sizeof(out))return false;std::memcpy(out,path,size+1);auto* dot=std::strrchr(out,'.');if(!dot||dot-out>255)return false;
    // The original changes three extension bytes without truncating a longer
    // extension. All shipped music filenames have three-byte extensions.
    dot[1]='w';dot[2]='a';dot[3]='v';return true;
}
void MusicControl::unlock(i32 song){if(!(game_flags&10)&&u32(song)<32)records.music_unlocked[song]=1;}
i32 MusicControl::load(i32 slot,const char* path){if(config.music==2){if(midi_available)actions.midi_load(slot,path);return 0;}if(config.music==1){char wave[260]{};if(!wave_path(path,wave))return -1;actions.command(1,slot,wave);}return 1;}
i32 MusicControl::play(i32 slot,i32 song){if(config.music==2){if(midi_available){actions.midi_stop();actions.midi_play(slot);actions.midi_start();}unlock(song);}else if(config.music==1){if(config.options&8192)actions.command(4,0,"dummy");actions.command(2,slot,"dummy");unlock(song);}return 0;}
i32 MusicControl::audio(const char* path,i32 song){if(config.music==2){if(midi_available){actions.midi_stop();actions.midi_file(path);actions.midi_start();}unlock(song);}else{if(config.music!=1)return -1;char wave[260]{};if(!wave_path(path,wave))return -1;actions.command(2,-1,wave);unlock(song);}return 0;}
i32 MusicControl::stop(){if(config.music==2){if(midi_available)actions.midi_stop();}else{if(config.music!=1)return -1;actions.command(config.options&8192?4:3,0,"dummy");}return 0;}
i32 MusicControl::fade(float seconds){if(config.music==2){if(midi_available)actions.midi_fade((number(1000)*number(seconds)).truncate_int());}else{if(config.music!=1)return -1;if(frame_rate!=0&&!(frame_rate>1))seconds=Scalar::div(seconds,frame_rate);actions.command(5,Scalar::truncate(seconds),"");}return 0;}
}
