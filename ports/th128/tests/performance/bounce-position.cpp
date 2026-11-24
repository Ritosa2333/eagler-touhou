#include "game/BulletUpdateHost.hpp"
#include <fstream>
#include <iterator>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
using namespace th128::bullet;
std::vector<unsigned char> bytes(const char* p){std::ifstream f(p,std::ios::binary);return {std::istreambuf_iterator<char>(f),{}};}
int main(){
 std::string error;const auto raw=bytes("/profile.bin"),anm=bytes("/bullet.anm");Profile profile;BulletVisuals visuals;
 if(!profile.load(raw.data(),raw.size(),error)||!visuals.load(anm.data(),anm.size(),error)){std::cerr<<error;return 2;}
 struct Row{const char* label;Position p;float angle;int control;Position expected;};
 const Row rows[]={{"right overshoot",{195,100,1.25f},0,8,{186,100,1.25f}},
 {"left overshoot",{-195,100,1.25f},3.141592741f,4,{-186,100,1.25f}},
 {"top overshoot",{10,-3,1.25f},-1.570796371f,1,{10,6,1.25f}},
 {"bottom overshoot",{10,451,1.25f},1.570796371f,2,{10,442,1.25f}},
 {"inside",{10,100,1.25f},0,13,{13,100,1.25f}},
 {"right exact",{192,100,1.25f},0,8,{189,100,1.25f}}};
 unsigned checks=0,failed=0;
 for(const auto& row:rows){
  Birth b;b.type=8;b.color=10;b.script=43;b.speed=3;b.angle=row.angle;
  b.state.position=row.p;b.state.velocity={3*std::cos(row.angle),3*std::sin(row.angle),0};b.state.flags=0x11;b.state.motionState=1;
  b.state.transformSound=-1;b.state.activeExtensions=0x40;b.state.extensionIndex=2;b.extensionListEmpty=true;
  b.extension64={-999.f,1,row.control,0};
  BulletVisuals::CreateInfo info;info.type=8;info.color=10;info.script=43;info.birthInterrupt=8;
  for(int i=0;i<4;i++)info.spriteRemap[i]=*profile.remap(10,i);
  auto visual=visuals.create(info,error);if(!visual){std::cerr<<error;return 3;}
  visual->primary.control().integers[0]=1;bool removed=false;
  const bool ok=updateBulletWithKnownMisses(b,visuals,*visual,{1000,1000,0},0,removed,error);
  bool pass=ok&&!removed&&error.empty()&&std::abs(b.state.position.x-row.expected.x)<.00001f
    &&std::abs(b.state.position.y-row.expected.y)<.00001f&&b.state.position.z==row.expected.z;
  checks++;if(!pass)failed++;
  std::cout<<row.label<<" "<<(pass?"PASS":"FAIL")<<" position "<<b.state.position.x<<","<<b.state.position.y<<","<<b.state.position.z
    <<" want "<<row.expected.x<<","<<row.expected.y<<","<<row.expected.z<<" error "<<error<<"\n";
 }
 std::cout<<"checks "<<checks<<" failed "<<failed<<"\n";return failed?1:0;
}
