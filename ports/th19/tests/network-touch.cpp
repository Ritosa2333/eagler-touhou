#include "TouchController.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
int main(){using namespace touhou::input;
 for(int side=0;side<2;side++){
 TouchController c;c.mode=1;c.unlimited=true;
 TouchState s;s.context=1;s.ready=true;s.instance=1;s.x=0;s.y=300;s.min_x=-140;s.max_x=140;s.min_y=32;s.max_y=432;
 c.pointer(0,1,.5f,.5f,0,s,false);c.pointer(1,1,.55f,.52f,1,s,false);
 auto a=c.sample_network(s,2);assert(a.motion==2);assert(std::abs(a.x-s.x-32)<.001f);assert(std::abs(a.y-s.y-9.6f)<.001f);
 // Input lead: repeated sampling before simulation must not repeat movement.
 for(int i=0;i<8;i++){a=c.sample_network(s,3+i);assert(a.x==s.x&&a.y==s.y);}
 // A rollback changes live position, never the displacement already consumed.
 s.x=-80;s.y=200;a=c.sample_network(s,20);assert(a.x==s.x&&a.y==s.y);
 c.pointer(1,1,.56f,.52f,21,s,false);a=c.sample_network(s,22);assert(std::abs(a.x-s.x-6.4f)<.001f);
 c.pointer(1,1,.57f,.52f,23,s,false);c.reset();a=c.sample_network(s,24);assert(a.motion==0);
 c.pointer(0,2,.3f,.3f,25,s,false);c.pointer(1,2,.4f,.4f,26,s,false);s.ready=false;c.sample_network(s,27);s.ready=true;a=c.sample_network(s,28);assert(a.x==s.x&&a.y==s.y);
 }
 std::cout<<"Network touch: queued samples, rollback, cancel and blocked motion PASS\n";
}
