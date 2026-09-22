#include "AudioGame.hpp"
#ifdef TH_ENABLE_THPRAC
#include "PracticeConfig.hpp"
#endif
namespace th10 {
#ifdef TH_ENABLE_THPRAC
// Port of upstream ElBgmTest (thprac_games.h:146) for TH10. The locked song
// keeps playing: duplicate play/stop/pause commands are swallowed while the
// everlasting-BGM hotkey holds, and the lock re-arms on the next play.
// Commands: 0 play, 1 stop, 2 pause, 3 resume; any other value is the upstream
// default arm and returns the current lock status unchanged.
bool practice_bgm_filter(PracticeState& p,i32 command,i32 song,bool stage_start){
    const bool el=p.everlasting_bgm&&p.active&&!p.replay;const bool is_practice=p.active&&p.run.section!=0;
    switch(command){
    case 0:
        // ElBgmTest only locks the song when the play call comes from the
        // 0x4183e6 return site (the fresh stage-entry BGM start). Other play
        // sites fall through to the shared different-song reset below.
        if(stage_start){
            if(p.el_bgm_lock==-1)p.el_bgm_lock=song;
            if(p.el_bgm_lock!=song){p.el_bgm_lock=-1;p.el_bgm_block=false;}
            else if(!p.el_bgm_block&&el){p.el_bgm_block=true;return false;}
        }
        if(p.el_bgm_lock>=0&&p.el_bgm_lock!=song){p.el_bgm_lock=-1;p.el_bgm_block=false;}
        break;
    case 1:
        if(p.el_bgm_lock>=0){p.el_bgm_lock=-1;if(!is_practice||!el)p.el_bgm_block=false;}
        break;
    case 2:
        if(p.el_bgm_lock>=0)p.el_bgm_block=el;
        break;
    case 3:
        if(p.el_bgm_lock>=0&&!p.el_bgm_block&&el){p.el_bgm_block=true;return false;}
        break;
    default:break;
    }
    return p.el_bgm_block;
}
bool AudioGame::filter(i32 command,i32 song,bool stage_start){return practice&&practice_bgm_filter(*practice,command,song,stage_start);}
#endif
bool AudioGame::wave_name(char* output,const char* name){
    const auto length=std::strlen(name);if(length>=256)return false;std::memcpy(output,name,length+1);
    char* extension=std::strrchr(output,'.');if(!extension||extension-output>251)return false;
    // Only three bytes are replaced; longer extensions retain their suffix.
    const bool short_extension=std::strlen(extension)<4;std::memcpy(extension+1,"wav",3);if(short_extension)extension[4]=0;return true;
}
void AudioGame::unlock(i32 track){reinterpret_cast<u8*>(*scores)[0x1d892+track]=1;}
i32 AudioGame::prepare(i32 slot,const char* name){char filename[256];if(!wave_name(filename,name))return 0;manager.queue_music(1,slot,filename);return 1;}
void AudioGame::play(i32 slot,i32 track,bool stage_start){
#ifdef TH_ENABLE_THPRAC
    if(filter(0,track,stage_start))return;
#endif
    if(*display_flags&16)manager.queue_music(4,0,"dummy");manager.queue_music(2,slot,"dummy");unlock(track);}
void AudioGame::play_file(const char* name,i32 track,bool stage_start){char filename[256];if(!wave_name(filename,name))return;
#ifdef TH_ENABLE_THPRAC
    if(filter(0,track,stage_start))return;
#endif
    unlock(track);manager.queue_music(2,-1,filename);}
void AudioGame::stop(){
#ifdef TH_ENABLE_THPRAC
    if(filter(1,0,false))return;
#endif
    manager.queue_music(*display_flags&16?4:3,0,"dummy");}
void AudioGame::fade(float seconds){
#ifdef TH_ENABLE_THPRAC
    // A fade is not one of the four classified commands (upstream play/stop/
    // pause/resume return addresses), so it takes ElBgmTest's default arm: no
    // state change, it only observes the current lock.
    if(filter(-1,0,false))return;
#endif
    const auto speed=number(*rate);auto duration=number(seconds);if(!(speed==number(0))&&!(number(1)<speed))duration=duration/speed;manager.queue_music(5,duration.truncate_int(),"");}
}
