#include "FighterControls.hpp"
#include <cassert>
int main(){
 th075::ControlCounts sampled;th075::FighterControls fighter;
 // Held axes from BrowserRuntime keep arriving during hitstun and knockdown.
 for(int action:{0,300,400,500,0})for(int frame=0;frame<20;frame++){
  sampled.from_held(2);fighter.player_frame(sampled,1,action);
  assert((fighter.current.held()&2)&&fighter.current.horizontal>0);
 }
 sampled.from_held(0);fighter.player_frame(sampled,1,0);
 assert(fighter.current.held()==0);
 sampled.clear();fighter.player_frame(sampled,1,0);assert(fighter.current.held()==0);
}
