#include "TouchController.hpp"
using namespace touhou::input;
extern "C" __attribute__((export_name("verify"))) int verify(){
 TouchState live;live.context=1;live.instance=1;live.ready=true;live.x=192;live.y=384;live.fast=4;live.slow=2;
 TouchState hit=live;hit.ready=false;
 // A deathbomb is an action during immobility, including the B button pulse.
 TouchController button;button.sample(live,0,false,false);button.controls(true,true,1,0,32767,0);button.mode=3;
 for(int n=0;n<3;n++){auto s=button.sample(hit,n,false,false);if(!s.keys[88]||!s.keys[16]||s.motion||s.keys[39])return 1;}
 if(button.sample(live,4,false,false).keys[88])return 2;
 // A tap made after the deadline must not be retained until respawn.
 button.controls(true,false,2,0,0,0);for(int n=0;n<40;n++)button.sample(hit,n,false,false);
 if(button.sample(live,41,false,false).keys[88])return 3;
 // A double tap can straddle the hit or start during the deathbomb window.
 for(bool firstBeforeHit:{false,true}){
  TouchController t;t.double_tap=true;const auto& first=firstBeforeHit?live:hit;
  t.pointer(0,1,.5f,.5f,0,first,false);t.pointer(2,1,.5f,.5f,40,first,false);
  t.sample(hit,50,false,false);t.pointer(0,2,.5f,.5f,80,hit,false);
  auto s=t.sample(hit,81,false,false);if(!s.keys[88]||s.motion)return 4;
 }
 // Paused/menu, dialogue and replay input cannot leak a bomb into gameplay.
 for(int context:{0,2,3}){TouchController t;TouchState blocked=hit;blocked.context=context;t.sample(blocked,0,false,false);t.controls(false,false,1,0,0,0);
  for(int n=0;n<10;n++)if(t.sample(blocked,n,false,false).keys[88])return 5;
  if(t.sample(live,11,false,false).keys[88])return 6;
 }
 // Cancellation releases the gesture; normal movement still works afterward.
 TouchController drag;drag.pointer(0,1,.5f,.5f,0,live,false);drag.pointer(1,1,.6f,.5f,10,live,false);
 if(!drag.sample(live,11,false,false).motion)return 7;
 if(drag.sample(hit,12,false,false).motion||drag.active())return 8;
 drag.reset();if(drag.sample(live,13,false,false).keys[88])return 9;
 return 0;
}
