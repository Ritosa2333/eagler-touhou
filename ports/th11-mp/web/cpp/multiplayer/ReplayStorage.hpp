#pragma once
#include <cstdio>
#include <string>
namespace th11::multiplayer {
// A title filename seam: choose an unused legal path without changing any
// existing Replay. The caller retains this path for all later saves this run.
template<class Exists> std::string AutomaticReplayPath(Exists exists){
    char path[80];
    for(unsigned slot=1;slot<=99;++slot){
        std::snprintf(path,sizeof(path),"/save/replay/th11_%02u.rpy",slot);
        if(!exists(path))return path;
    }
    constexpr char digits[]="0123456789abcdefghijklmnopqrstuvwxyz";
    for(unsigned slot=0;slot<36u*36u*36u*36u;++slot){
        char suffix[5]="0000";unsigned value=slot;
        for(int i=3;i>=0;--i){suffix[i]=digits[value%36];value/=36;}
        std::snprintf(path,sizeof(path),"/save/replay/th11_ud%s.rpy",suffix);
        if(!exists(path))return path;
    }
    return {};
}
}
