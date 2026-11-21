#include "game/SpellVisuals.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
using th128::spell::Visuals;
static int checks=0;
static void require(bool v,const std::string& label){++checks;if(!v)throw std::runtime_error(label);}
int main(int argc,char** argv){try{
 require(argc==8,"six active ANMs and one alternate boss required");
 auto bytes=[](const char* n){std::ifstream f(n,std::ios::binary);if(!f)throw std::runtime_error(n);return std::vector<std::uint8_t>{std::istreambuf_iterator<char>(f),{}};};
 for(const int stage:{16,1}){
  Visuals v;std::string error;const std::array<int,6> banks{0,3,6,11,12,7};
  for(unsigned i=0;i<banks.size();++i){const auto b=bytes(argv[i==1&&stage==16?7:i+1]);require(v.load(banks[i],b,error),error);}
  require(v.start({0,96,0},error,{},stage),error);
  auto rows=v.snapshots();auto has=[&](int bank,int script){return std::any_of(rows.begin(),rows.end(),[&](const auto& n){return n.state.bank==bank&&n.state.script==script;});};
  require(has(3,stage==16?17:11),"original stage-specific cut-in");
  for(int i=0;i<473;++i)require(v.tick({0,96,0},true,error),error);
  rows=v.snapshots();
  if(stage==16){require(has(3,19)&&has(3,20),"Extra boss background needs original children 19 and 20");require(!has(12,1)&&!has(12,2)&&!has(12,3),"ordinary card art mixed into Extra");}
  else {require(has(12,3)&&has(12,1)&&has(12,2),"ordinary scrolling card backdrop");require(!has(3,19)&&!has(3,20),"Extra background leaked into ordinary play");}
  require(v.titlePose().has_value(),"original title owner missing");require(v.finish(error),error);
  for(int i=0;i<40;++i)require(v.tick({0,96,0},false,error),error);
  require(v.snapshots().empty(),"retired spell left stale art");
 }
 std::cout<<"CHECKS "<<checks<<" FAILED 0 WORLDS 0\n";return 0;
}catch(const std::exception& e){std::cerr<<"CHECKS "<<checks<<" FAILED 1 WORLDS 0: "<<e.what()<<'\n';return 1;}}
