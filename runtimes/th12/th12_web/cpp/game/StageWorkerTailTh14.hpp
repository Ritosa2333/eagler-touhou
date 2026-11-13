#pragma once
#include "PortTitle.hpp"
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
#include "MusicCatalog.hpp"
#include "Th14MusicQueue.hpp"
#include "FrameStatistics.hpp"
#include "Timer.hpp"
#include "StageCppWorkerTh14.hpp"
namespace th11 {
// Main's Control body is disabled during loading, but read-only Player
// observations can still sample current. Other Timer fields are consumed only
// after this worker's phase publication/join. Keep the reset's current store
// and the corresponding shared readers defined as aligned scalar accesses.
inline void th14_reset_loading_clock(Timer& clock,const float* rate)noexcept{
    if(!(clock.flags&1)){clock.rate=rate;clock.flags|=1;}
    clock.previous=-1;__atomic_store_n(&clock.current,0,__ATOMIC_SEQ_CST);clock.fractional=0;
}
inline bool th14_finish_stage_audio_tail(u32 number,u32 mode_flags,bool direct_sound,
        Th14MusicQueue& music,FrameStatistics& statistics,Timer& clock,const float* rate,
        StageCppWorkerTh14& worker){
    if(worker.stop_requested())return false;
    const char* names[2]={nullptr,nullptr};
    for(u32 slot=0;slot<2;++slot){
        const auto cue=stage_music(i32(number),slot!=0);
        for(const auto& track:music_tracks)if(track.cue==cue){names[slot]=track.file;break;}
        if(!names[slot])return false;
    }
    // 435fcc..436010: each enqueue has its own queue critical section;
    // Demo omits requests, Spell Practice omits only the dummy request.
    music.enqueue_initial(mode_flags,direct_sound,names[0],names[1]);
    statistics.reset_run();th14_reset_loading_clock(clock,rate);
    // 43601a..43603b reads head.command, and sleeps16ms before retrying.
    // It does not replace that predicate with worker completion or SDL state.
    while(music.head_command())if(!worker.wait_for(16000000))return false;
    return !worker.stop_requested();
}
}
#endif
