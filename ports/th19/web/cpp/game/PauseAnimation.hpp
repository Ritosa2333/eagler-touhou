#pragma once
#include "AnmVm.hpp"
namespace th19 {
// front.anm 86, native pause controller 0x537d40 and creation 0x5394ed.
struct PauseAnimation {
 AnmVm root;
 int choice=0,answer=1,phase=0,age=0; // 0 menu, 1 opening confirmation, 2 confirmation, 3 cancelling, 4 closing
 bool quit=false,retryMenu=false;
 AnmVm* child(int sprite){for(auto&v:root.children)if(v.sprite==sprite&&v.visible)return &v;return nullptr;}
 void start(const std::unordered_map<i32,AnmScriptLoc>&scripts,bool cardsDisabled=true,bool retry=false){
  *this={};retryMenu=retry;auto it=scripts.find(retry?89:86);if(it==scripts.end())return;
  root.bind_entry(it->second.stream,it->second.len,&scripts);root.tick();root.interrupt_all(3);root.tick();
  if(cardsDisabled)if(auto*v=child(62))v->interrupt(5);
 }
 void select(int n){if(retryMenu){for(int s:{58,54})if(auto*v=child(s))v->interrupt(3);}else{const int sprites[]={53,54,62,56,61};for(int i=0;i<5;++i)if(auto*v=child(sprites[i]))v->interrupt(3);}choice=n;root.interrupt_all(7+n);}
 void confirm(){phase=1;age=0;if(auto*v=child(54))v->interrupt(6);}
 void selectAnswer(int n){if(auto*v=child(65-n))v->interrupt(3);answer=n;root.interrupt_all(15+n);}
 void cancel(){phase=3;age=0;select(choice);}
 void close(bool toTitle){phase=4;age=0;quit=toTitle;if(retryMenu)root.interrupt_all(1);else if(quit){if(auto*v=child(64))v->interrupt(6);}else root.interrupt_all(1);}
 // True after the exit animation, while the game remains paused underneath.
 bool tick(){
  ++age;
  if(phase==0&&age==10)select(choice);
  if(phase==1&&age==20)root.interrupt_all(14);
  if(phase==1&&age==30){phase=2;selectAnswer(1);}
  if(phase==3&&age>=20){phase=0;age=11;}
  if(phase==4&&quit&&!retryMenu&&age==20)root.interrupt_all(1);
  root.tick();
  return phase==4&&age>=(retryMenu?12:quit?30:10);
 }
 bool inputReady()const{return (phase==0&&age>=10)||phase==2;}
 // Disabled menu tint must still obey the primary confirmation/exit fade.
 static u32 menuColor(const AnmVm& v){u32 c=v.color();if(v.color_mode==1)c=(c&0xffffff)|(u32(std::min(v.alpha,v.alpha2))<<24);return c;}
};
}
