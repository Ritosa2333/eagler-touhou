#pragma once
#include <cstdint>
#include <string>
#include <vector>
#if TH15_MULTIPLAYER && TH15_WRITE_JOURNAL
#include <eagler/netplay/WriteJournal.hpp>
#define TH15_GAME_SCOPE Netplay::WriteJournal::GameScope th15_game_scope
#define TH15_SYSTEM_SCOPE Netplay::WriteJournal::SystemScope th15_system_scope
#else
#define TH15_GAME_SCOPE
#define TH15_SYSTEM_SCOPE
#endif
namespace th15::sdl::rollback {
// This output log belongs to the host, outside the reversible simulation heap.
// Strings are copied under SystemScope: no game allocation escapes into a log
// whose destruction could otherwise free memory resurrected by an undo.
enum class AudioKind { Sound, Positioned, Stop, Pan, Suspend, Resume, Music, Track, PrepareMusic,
    PauseMusic, Volume, ClearWave, Finish, Update, Pump };
struct AudioEvent {
    AudioKind kind;std::int32_t first=0,second=0;float position=0;std::string text;
};
#if TH15_MULTIPLAYER && TH15_WRITE_JOURNAL
inline bool active=false,boundary=false;
inline std::vector<AudioEvent>* audio=nullptr;
inline bool RequireConfirmed(){if(!active)return true;boundary=true;return false;}
inline bool Emit(AudioKind kind,std::int32_t first=0,std::int32_t second=0,float position=0,const std::string& text={}){
    if(!active||!audio)return false;TH15_SYSTEM_SCOPE;
    audio->push_back({kind,first,second,position,text});return true;
}
inline void Begin(std::vector<AudioEvent>& events){boundary=false;audio=&events;active=true;}
inline void End(){active=false;audio=nullptr;}
#else
inline bool RequireConfirmed(){return true;}
inline bool Emit(AudioKind,std::int32_t=0,std::int32_t=0,float=0,const std::string& text={}){return false;}
#endif
}
