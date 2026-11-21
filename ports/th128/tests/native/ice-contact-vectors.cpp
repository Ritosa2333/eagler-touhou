#include "game/IceContact.hpp"
#include <cassert>
#include <iostream>
int main(){
 int lifetime=0,delay=0;float x=0,y=0,ice=0,bullet=0;
 while(std::cin>>lifetime>>delay>>x>>y>>ice>>bullet){
  const auto result=th128::ice::contacts({0,0,0},ice,lifetime,delay,{x,y,0},bullet);assert(result);std::cout<<*result<<'\n';
 }
}
