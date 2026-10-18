#pragma once
#include <cstdio>
namespace th08 {
// Purple TH08 hooks 00417C6B/00417DF6. Coordinates are the original ANM
// coordinates (the renderer, not this hook, owns screen scaling).
template<class Draw, class Add>
bool practice_history_overflow(int count, bool captures, float& x, Draw draw, Add add) {
    if(count<=999)return false;
    char text[16];const int length=std::snprintf(text,sizeof text,"%d",count);
    x=add(x,captures?28.f:7.f);
    const float increment=25.f/float(length);
    for(int i=0;i<length;++i){draw(text[i]-'0');x=add(x,increment);}
    if(captures)x=add(x,-increment);
    return true;
}
}
