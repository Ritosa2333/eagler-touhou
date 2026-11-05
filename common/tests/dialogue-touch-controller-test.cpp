#include TOUCH_CONTROLLER_HEADER
#include <cassert>
#include <cstdio>
using namespace touhou::input;
static TouchState live(){
    TouchState s;s.context=1;s.instance=7;s.ready=true;s.x=100;s.y=200;
    s.fast=4;s.slow=2;s.min_x=-184;s.max_x=184;s.min_y=32;s.max_y=432;return s;
}
int main(){
    auto play=live(),dialogue=play;dialogue.context=2;unsigned checks=0;
    for(bool unlimited:{false,true}){
        TouchController t;t.unlimited=unlimited;t.fire=true;
        t.pointer(0,1,.5f,.5f,0,dialogue,false);
        t.pointer(1,1,.6f,.6f,10,dialogue,false);
        auto s=t.sample(dialogue,600,false,false);
        assert(s.motion==(unlimited?2:1)&&s.x>100&&s.y>200);
        assert(!s.keys[17]&&!s.keys[90]&&!s.keys[38]&&!s.keys[40]);++checks;
        // A second finger confirms while the first continues moving.
        t.pointer(0,2,.3f,.3f,700,dialogue,false);
        t.pointer(2,2,.3f,.3f,710,dialogue,false);
        s=t.sample(dialogue,711,false,false);assert(s.motion&&s.keys[90]);++checks;
        for(int n=0;n<3;++n)t.sample(dialogue,720+n,false,false);
        // A third, stationary finger can skip independently of movement.
        t.pointer(0,3,.3f,.3f,800,dialogue,false);
        s=t.sample(dialogue,1300,false,false);assert(s.motion&&s.keys[17]&&!s.keys[90]);++checks;
        t.pointer(2,3,.3f,.3f,1310,dialogue,false);
        // Returning a drag to its origin cannot confirm or skip.
        t.pointer(1,1,.5f,.5f,1320,dialogue,false);
        s=t.sample(dialogue,1400,false,false);assert(!s.keys[17]);
        t.pointer(2,1,.5f,.5f,1410,dialogue,false);
        s=t.sample(dialogue,1411,false,false);assert(!s.motion&&!s.keys[90]);++checks;
    }
    for(int mode:{2,3}){
        TouchController t;t.mode=mode;t.controls(true,true,0,0,32767,0);
        auto s=t.sample(dialogue,0,false,false);
        assert(!s.keys[90]&&s.keys[16]);
        if(mode==2)assert(s.keys[39]&&!s.motion);
        else assert(s.motion==1&&s.x>dialogue.x);++checks;
    }
    TouchController held;
    held.pointer(0,1,.5f,.5f,0,play,false);
    held.pointer(1,1,.55f,.5f,10,play,false);
    auto s=held.sample(dialogue,20,false,false);assert(s.motion&&held.active());
    held.pointer(1,1,.6f,.5f,30,dialogue,false);
    s=held.sample(play,40,false,false);assert(s.motion&&s.x>play.x);++checks;
    auto menu=play;menu.context=0;
    held.sample(menu,50,false,false);assert(!held.active());
    held.pointer(1,1,.7f,.5f,60,dialogue,false);
    held.pointer(2,1,.7f,.5f,70,dialogue,false);
    s=held.sample(dialogue,71,false,false);assert(!s.motion&&!s.keys[90]);++checks;
    auto choice=dialogue;choice.choice=true;
    for(float y:{.4f,.6f}){
        TouchController t;t.fire=true;
        t.pointer(0,1,.5f,.5f,0,choice,false);
        t.pointer(1,1,.5f,y,10,choice,false);
        s=t.sample(choice,600,false,false);
        assert(s.keys[y<.5f?38:40]&&s.motion&&!s.keys[17]&&!s.keys[90]);
        s=t.sample(choice,601,false,false);assert(!s.keys[38]&&!s.keys[40]);
        t.pointer(2,1,.5f,y,610,choice,false);
        s=t.sample(choice,611,false,false);assert(!s.motion&&!s.keys[90]);++checks;
        t.pointer(0,1,.5f,.5f,700,choice,false);
        t.pointer(2,1,.5f,.5f,710,choice,false);
        s=t.sample(choice,711,false,false);assert(s.keys[90]);++checks;
    }
    TouchController fast;
    fast.pointer(0,1,.5f,.5f,0,choice,false);
    fast.pointer(1,1,.5f,.6f,10,choice,false);
    fast.pointer(2,1,.5f,.6f,20,choice,false);
    s=fast.sample(choice,21,false,false);assert(s.keys[40]&&!s.keys[90]);++checks;
    TouchController ending;auto end=dialogue;end.ready=false;
    ending.pointer(0,1,.5f,.5f,0,end,false);
    s=ending.sample(end,500,false,false);assert(s.keys[17]&&!s.motion&&!s.keys[90]);++checks;
    ending.reset();s=ending.sample(end,510,false,false);assert(!s.keys[17]&&!s.motion);++checks;
    // A short drag with an UP back at its origin still is not a tap.
    TouchController out_and_back;
    out_and_back.pointer(0,1,.5f,.5f,0,dialogue,false);
    out_and_back.pointer(1,1,.6f,.5f,10,dialogue,false);
    out_and_back.pointer(2,1,.5f,.5f,20,dialogue,false);
    s=out_and_back.sample(dialogue,21,false,false);assert(!s.keys[90]&&!s.keys[17]);++checks;
    std::printf("Dialogue movement / independent tap and hold / choices / transitions: %u checks passed\n",checks);
}
