// Portable ownership of ElBgmTest in thprac_games.h (MIT).
// TH11 call sites: play 42a183, stop 42028f, pause 42c72a, resume 42c8af.
#pragma once
#include "PracticeConfig.hpp"
namespace th11 {
enum class PracticeBgmEvent {Other,Play,Stop,Pause,Resume};
struct PracticeBgm {
    bool status=false;int locked_id=-1;
    bool filter(PracticeBgmEvent event,int id,bool hotkey,bool is_practice){
        switch(event){
        case PracticeBgmEvent::Play:
            // TH11 supplies caller 0xffffffff, matching caller_addr.
            if(locked_id==-1)locked_id=id;
            if(locked_id!=id){locked_id=-1;status=false;}
            else if(!status&&hotkey){status=true;return false;}
            if(locked_id>=0&&locked_id!=id){locked_id=-1;status=false;}
            break;
        case PracticeBgmEvent::Stop:
            if(locked_id>=0){locked_id=-1;if(!is_practice||!hotkey)status=false;}
            break;
        case PracticeBgmEvent::Pause:
            if(locked_id>=0)status=hotkey;
            break;
        case PracticeBgmEvent::Resume:
            if(locked_id>=0&&!status&&hotkey){status=true;return false;}
            break;
        default:break;
        }
        return status;
    }
};
}
