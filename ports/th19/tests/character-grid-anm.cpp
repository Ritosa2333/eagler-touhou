#include "AnmResource.hpp"
#include "AnmVm.hpp"
#include <fstream>
#include <cassert>
#include <iostream>
#include <cmath>
using namespace th19;
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);std::vector<u8>b((std::istreambuf_iterator<char>(f)),{});std::vector<AnmEntry> e;assert(parse_anm(b.data(),b.size(),e));std::unordered_map<i32,AnmScriptLoc> scripts;for(auto&entry:e)for(auto&s:entry.scripts)scripts[s.id]={b.data()+entry.base+s.offset,u32(b.size()-entry.base-s.offset),&entry.scripts};
for(int root:{76,77}){AnmVm v;auto s=scripts.at(root);v.bind_entry(s.stream,s.len,&scripts);v.tick();assert(v.children.size()==(root==76?19:20));assert(v.children[0].sx==-1&&v.children[0].render_mode==8);assert(v.children[18].alpha==0);bool mirrored=false;for(int i=0;i<65;i++){v.tick();auto&a=v.children[0];mirrored|=a.render_mode==8&&a.sx*std::cos(a.rot_y)<-.5f;}assert(mirrored);for(int i=0;i<19;i++){auto&a=v.children[i];assert(a.sx==1&&a.render_mode==0&&a.alpha==255&&a.r==64);}v.interrupt_all(7);for(int i=0;i<4;i++)v.tick();assert(v.children[0].r==255&&v.children[1].r==64);v.interrupt_all(1);for(int i=0;i<18;i++)v.tick();assert(v.children.empty());}
std::cout<<"Original ANM76/77: stagger, mirrored Y flip, dim, highlight and exit PASS\n";}
