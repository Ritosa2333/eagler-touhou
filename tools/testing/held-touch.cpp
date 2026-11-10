#include "TouchController.hpp"
#include <cassert>
using namespace touhou::input;
int main(){
 for(bool unlimited:{false,true})for(bool moveWhileBlocked:{false,true}){
  TouchController input;input.unlimited=unlimited;
  TouchState live;live.context=1;live.instance=1;live.ready=true;live.x=192;live.y=384;live.fast=4;live.slow=2;
  input.pointer(0,7,.5f,.5f,0,live,false);input.pointer(1,7,.55f,.5f,1,live,false);
  assert(input.sample(live,2,false,false).motion&&"initial drag reaches movement");
  auto blocked=live;blocked.ready=false;
  for(int n=0;n<4;n++){
   assert(!input.sample(blocked,3+n,false,false).motion&&"immobile player receives no movement");
   assert(input.active()&&"temporary movement block must preserve the movement finger");
   if(moveWhileBlocked)input.pointer(1,7,.56f+n*.01f,.5f,8+n,blocked,false);
  }
  auto respawn=live;respawn.instance=2;respawn.x=160;respawn.y=400;
  input.sample(respawn,12,false,false);
  input.pointer(1,7,.61f,.5f,13,respawn,false);input.pointer(1,7,.63f,.5f,14,respawn,false);
  const auto resumed=input.sample(respawn,15,false,false);
  assert(resumed.motion&&resumed.x>respawn.x&&"same finger resumes after respawn");
  input.reset();input.pointer(1,7,.7f,.5f,16,respawn,false);
  assert(!input.sample(respawn,17,false,false).motion&&"lifecycle cancellation retires the old finger");
  input.pointer(0,8,.5f,.5f,18,respawn,false);input.pointer(1,8,.55f,.5f,19,respawn,false);
  assert(input.sample(respawn,20,false,false).motion&&"new finger works after cancellation");
 }
 for(int mode:{2,3}){
  TouchController input;input.mode=mode;TouchState s;s.context=1;s.instance=1;s.ready=true;s.x=192;s.y=384;s.fast=4;s.slow=2;
  input.sample(s,0,false,false);input.controls(false,false,0,0,32767,0);s.ready=false;
  auto stopped=input.sample(s,1,false,false);assert(!stopped.motion&&!stopped.keys[39]);
  s.ready=true;auto resumed=input.sample(s,2,false,false);assert(mode==3?resumed.motion!=0:resumed.keys[39]);
 }
}
