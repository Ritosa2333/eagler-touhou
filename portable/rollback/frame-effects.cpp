#include "../../web/cpp/sdl/FrameEffects.hpp"
#include "../../web/cpp/multiplayer/FrameFence.hpp"
#include <cstdio>
#include <cstdlib>
using th15::sdl::FrameEffects;
using th15::multiplayer::FrameFence;
static void check(bool ok,const char* why){if(!ok){std::fprintf(stderr,"%s\n",why);std::exit(1);}}
int main(){
    unsigned sounds=0,music=0,writes=0;
    FrameEffects first,second;
    for(unsigned correction=0;correction<12;++correction){
        first.clear();first.events.push_back([&]{++sounds;return true;});
        first.events.push_back([&]{++music;return true;});
    }
    second.events.push_back([&]{++sounds;return true;});
    check(!sounds&&!music,"tentative and replayed requests escaped before confirmation");
    check(first.commit()&&second.commit(),"commit confirmed requests");
    check(sounds==2&&music==1,"correction duplicated confirmed effects");
    check(first.commit()&&sounds==2&&music==1,"confirmed journal was replayed twice");
    {
        FrameFence::Scope frame(true);
        if(FrameFence::allow())++writes;
        check(FrameFence::blocked&&!writes,"speculative factory/persistence crossed its fence");
    }
    {
        FrameFence::Scope frame(false);
        check(!FrameFence::blocked&&FrameFence::allow(),"actual-input frame did not release its fence");
        ++writes;
    }
    check(writes==1&&!FrameFence::tentative,"confirmed boundary executed more than once");
    std::puts("PASS: replaced tentative sounds/music, once-only confirmation and native frame fences");
}
