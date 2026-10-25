#include "AnmCamera.hpp"
#include "AnmTransforms.hpp"
#include <array>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <limits>
using namespace th15;
namespace th15 {
Vec3 reference_project_vertex(const Vec3&,const Matrix4&,const AnmCamera&,const GraphicsViewport&)noexcept;
int reference_anm_projected_quad(AnmVm&,const AnmCamera&,const GraphicsViewport&,Vec3 (&)[4])noexcept;
}
static unsigned checks=0,visible=0,culled=0,invalid=0;
static void need(bool v,const char* reason){++checks;if(!v){std::fprintf(stderr,"FAIL %s check=%u\n",reason,checks);std::exit(1);}}
int main(){
 std::mt19937 rng(0x735139);AnmVm vm;std::array<AnmVm,70> parents;
 const auto value=[&](){return float(int(rng()%8193)-4096)/1024.f;};
 for(unsigned i=0;i<100000;++i){
  vm.visual={};vm.variables={};vm.visual.flags=3u|((i&1?4u:6u)<<25)|((i%4)<<21)|(((i/4)%4)<<23);
  vm.visual.sprite_size={value()*32,value()*32};vm.visual.scale={value(),value()};vm.visual.secondary_scale={value(),value()};
  vm.visual.translation={value(),value(),value()};vm.visual.child_anchor={value(),value(),value()};vm.variables.position={value(),value(),value()};
  vm.variables.rotation={value()*5,value()*5,value()*5};
  const unsigned depth=i%71;
  for(unsigned j=0;j<70;++j){parents[j].rotation_parent=j+1<depth?&parents[j+1]:nullptr;parents[j].variables.rotation={value()*4,value()*4,value()*4};parents[j].visual.render_flags=0;}
  vm.rotation_parent=depth?&parents[0]:nullptr;
  if(i%37==0)vm.rotation_parent=&vm;
  if(i%41==0)vm.visual.render_flags=0x10000;
  AnmCamera camera;camera.reference={value()*.03f,value()*.03f,value()*.03f};
  for(unsigned j=0;j<16;++j){camera.view.m[j]=value();camera.projection.m[j]=value();}
  if(i&1){camera.view.identity();camera.projection.identity();vm.variables.position={0,0,.5f};vm.visual.translation={};vm.visual.child_anchor={};}
  if(i%101==0)camera.projection.m[15]=0;
  if(i%103==0)camera.projection.m[15]=std::numeric_limits<float>::infinity();
  if(i%107==0)camera.projection.m[15]=std::numeric_limits<float>::quiet_NaN();
  GraphicsViewport viewport{u32(rng()%500),u32(rng()%500),u32(1+rng()%2000),u32(1+rng()%1500),value(),value()};
  if(i&1){viewport.near_depth=0;viewport.far_depth=1;}
  Vec3 expected[4],actual[4];for(auto& p:expected)p={-11,-12,-13};std::memcpy(actual,expected,sizeof actual);
  std::array<Vec3,71> before,after;
  before[0]=vm.variables.rotation;for(unsigned j=0;j<70;++j)before[j+1]=parents[j].variables.rotation;
  const int old=reference_anm_projected_quad(vm,camera,viewport,expected);
  after[0]=vm.variables.rotation;for(unsigned j=0;j<70;++j)after[j+1]=parents[j].variables.rotation;
  vm.variables.rotation=before[0];for(unsigned j=0;j<70;++j)parents[j].variables.rotation=before[j+1];
  const int current=anm_projected_quad(vm,camera,viewport,actual);
  need(current==old,"projected quad result");need(std::memcmp(actual,expected,sizeof actual)==0,"projected vertex bits");
  need(std::memcmp(&vm.variables.rotation,&after[0],sizeof(Vec3))==0,"child normalization");
  for(unsigned j=0;j<70;++j)need(std::memcmp(&parents[j].variables.rotation,&after[j+1],sizeof(Vec3))==0,"parent normalization");
  visible+=old==0;culled+=old==-1;invalid+=old==-2;
  Matrix4 world;for(auto& f:world.m)f=value();const Vec3 point{value(),value(),value()};
  const Vec3 a=reference_project_vertex(point,world,camera,viewport),b=project_vertex(point,world,camera,viewport);
  need(std::memcmp(&a,&b,sizeof(Vec3))==0,"general project vertex bits");
 }
 need(visible>20000&&culled>10000&&invalid>10000,"insufficient coverage");
 std::printf("TH15_CAMERA_REFERENCE {\"cases\":100000,\"checks\":%u,\"visible\":%u,\"culled\":%u,\"invalid\":%u,\"maximumParentDepth\":70,\"bitwiseVertices\":true,\"parentPostState\":true,\"gpu\":false}\n",checks,visible,culled,invalid);
}
