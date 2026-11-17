#include "AnmVm.hpp"
#include <cassert>
#include <iostream>
using namespace th19;
int main(){
 // Child deletes itself; emitter creates one child every tick indefinitely.
 const u8 child[]={1,0,8,0,0,0,0,0};
 const u8 emitter[]={244,1,12,0,0,0,0,0,1,0,0,0,
                     200,0,16,0,1,0,0,0,0,0,0,0,255,255,255,255};
 std::unordered_map<i32,AnmScriptLoc> scripts{{1,{child,sizeof(child),nullptr}}};
 AnmVm vm;vm.bind_entry(emitter,sizeof(emitter),&scripts);
 for(int i=0;i<10000;++i){vm.tick();assert(vm.children.empty());assert(!vm.deleted);}
 AnmVm stopped;stopped.halted=true;stopped.visible=false;
 vm.children.push_back(stopped);vm.tick();assert(vm.children.size()==1);
 vm.children[0].bind(child,sizeof(child));vm.children[0].halted=false;
 vm.tick();assert(vm.children.empty());
 const u8 delayed[]={1,0,8,0,5,0,0,0};
 AnmVm parent;parent.bind(child,sizeof(child));
 AnmVm survivor;survivor.bind(delayed,sizeof(delayed));parent.children.push_back(survivor);
 parent.tick();assert(parent.deleted&&parent.children.size()==1);
 for(int i=0;i<5;++i)parent.tick();assert(parent.children.empty());
 std::cout<<"ANM deletion retires children; hidden stopped children retained PASS\n";
}
