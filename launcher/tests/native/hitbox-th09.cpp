#include "AnmResource.hpp"
#include "HitboxDisplay.hpp"
#include <fstream>
#include <iterator>
#include <iostream>
#include <cassert>
using namespace th09;
int main(int argc,char** argv){assert(argc==2);std::ifstream in(argv[1],std::ios::binary);std::vector<u8> bytes((std::istreambuf_iterator<char>(in)),{});assert(!bytes.empty());AnmResource data;assert(data.load(0,bytes.data(),bytes.size()));auto& file=data.view();Rng random;AnmExecutor executor(random);AnmVm native;executor.start(file,native,file.scripts[63]);native.flags|=0x2000;native.color1.d3dColor=-1;native.pos2={};
 presentation::HitboxDisplay display;
 for(int f=0;f<240;f++){executor.execute(native);auto before=random;auto source=data.data();auto marker=display.sample(file,{12,34,0},f);assert(random.seed==before.seed&&random.calls==before.calls&&data.data()==source);assert(native.rotation.z==marker.rotation.z&&native.scale.x==marker.scale.x&&native.scale.y==marker.scale.y&&native.color1.d3dColor==marker.color1.d3dColor&&native.activeSpriteIndex==marker.activeSpriteIndex);auto again=display.sample(file,{12,34,0},f);assert(marker.rotation.z==again.rotation.z&&marker.color1.d3dColor==again.color1.d3dColor);}
 std::cout<<"TH09 native focus script: 240 matching logical frames, repeated drawings stable, asset bytes and shared RNG unchanged\n";
}