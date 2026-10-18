#include "HitboxAnimation.hpp"
#include <fstream>
#include <iterator>
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
namespace s=th20::source::sprite;namespace n=th20::recovered;
s::AnimationFile file{};s::Animation child{};float rate=1;unsigned random_calls=0;bool child_live=false;
namespace th20::source::sprite::anm_environment {
float& clock_scale(){return rate;}const float* timer_rate(){return &rate;}
AnmInstruction* script(Animation& a){return file.scripts[a.base.fields_10_28[5]];}
bool gameplay_frozen(){return false;}std::uint32_t random_next(){++random_calls;return 0;}std::uint32_t random_bounded(std::uint32_t){++random_calls;return 0;}
float random_unit(){++random_calls;return 0;}float random_signed_unit(){++random_calls;return 0;}float camera_component(std::int32_t){std::abort();}
void assign_sprite(Animation& a,std::int32_t i){assert(i>=0);assign_animation_sprite(file,a,i);}void set_layer(Animation& a,std::int32_t layer){set_animation_layer(a,layer);}
void add_camera_offset(Vec3&){std::abort();}void calculate_corners(Animation&,Vec3 (&)[4]){std::abort();}float screen_scale(){return 1;}std::int32_t screen_offset(unsigned,unsigned axis){return axis?16:320;}
void* allocate_geometry(std::uint32_t){std::abort();}
std::uint32_t spawn_child(Animation& root,std::int32_t index,std::uint32_t){assert(index==18&&!child_live);child={};select_animation_template(file,child,index);attach_animation_parent(child,&root);child_live=true;assert(execute_animation(child)==0);return 1;}
std::uint32_t spawn_detached(Animation&,std::int32_t,std::uint32_t){std::abort();}Animation& lookup_animation(std::uint32_t){return child;}void spawn_effect(Animation&,std::int32_t){std::abort();}
}
template<class T>T get(const std::vector<unsigned char>& data,unsigned offset){T out;std::memcpy(&out,data.data()+offset,sizeof(out));return out;}
void eq(float expected,float actual,const char* field,int frame,int part){if(std::abs(expected-actual)>0.00001f){std::cerr<<field<<" frame="<<frame<<" part="<<part<<" rate="<<rate<<" expected="<<expected<<" actual="<<actual<<"\n";std::abort();}}
void compare(const s::Animation& a,const s::Animation& b,int f,int i){eq(a.base.vector_38.z,b.base.vector_38.z,"rotation",f,i);eq(a.base.vector_50.x,b.base.vector_50.x,"scale",f,i);assert(a.base.field_490==b.base.field_490);assert(a.base.fields_10_28[4]==b.base.fields_10_28[4]);if(a.base.flags[2]!=b.base.flags[2]){std::cerr<<"layer/status frame="<<f<<" part="<<i<<" native="<<a.base.flags[2]<<" helper="<<b.base.flags[2]<<"\n";std::abort();}eq(a.base.vector_70.x,b.base.vector_70.x,"sprite.width",f,i);}
int main(int argc,char** argv){assert(argc==2);std::ifstream in(argv[1],std::ios::binary);std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(in)),{});assert(!bytes.empty());std::vector<s::AnmInstruction*> scripts(20);std::vector<s::SpriteData> sprites(25);std::vector<s::Animation> templates(20);
 for(unsigned base=0;;){auto ns=get<std::uint16_t>(bytes,base+4),nc=get<std::uint16_t>(bytes,base+6);assert(get<unsigned>(bytes,base)==8);
  for(unsigned i=0;i<ns;i++){auto off=base+get<unsigned>(bytes,base+64+i*4),id=get<unsigned>(bytes,off);if(id<sprites.size()){auto& sp=sprites[id];sp.left=get<float>(bytes,off+4);sp.top=get<float>(bytes,off+8);sp.extent_4c=get<float>(bytes,off+12);sp.extent_48=get<float>(bytes,off+16);sp.right=sp.left+sp.extent_4c;sp.bottom=sp.top+sp.extent_48;sp.texture_extent_20=get<std::uint16_t>(bytes,base+10);sp.texture_extent_1c=get<std::uint16_t>(bytes,base+12);sp.scale_50=sp.scale_54=1;sp.u0=sp.left/sp.texture_extent_20;sp.u1=sp.right/sp.texture_extent_20;sp.v0=sp.top/sp.texture_extent_1c;sp.v1=sp.bottom/sp.texture_extent_1c;}}
  for(unsigned i=0;i<nc;i++){auto table=base+64+ns*4+i*8,id=get<unsigned>(bytes,table);if(id<scripts.size())scripts[id]=reinterpret_cast<s::AnmInstruction*>(bytes.data()+base+get<unsigned>(bytes,table+4));}
  auto next=get<unsigned>(bytes,base+36);if(!next)break;base+=next;
 }
 file.bytes=bytes.data();file.templates=templates.data();file.scripts=scripts.data();file.sprites=sprites.data();file.script_count=20;file.sprite_count=25;
 // Templates are prepared by the native loader at rate 1 before gameplay.
 for(int i:{18,19}){s::construct_animation(templates[i]);s::initialize_animation_script(file,templates[i],i);n::timer_set(templates[i].timer_4c8,-1);n::timer_set(templates[i].timer_4d8,-1);assert(s::execute_animation(templates[i])==0);}
 for(float step:{1.f,.5f}){rate=step;child_live=false;s::Animation root{};s::select_animation_template(file,root,19);root.base.vector_38.z=0;root.base.flags[1]|=0x200;assert(s::execute_animation(root)==0);assert(child_live);th20::web::HitboxAnimation display;
  for(int f=0;f<240;f++){auto before=random_calls;auto& markers=display.sample(file,f*rate);assert(random_calls==before);compare(child,markers[0],f,0);compare(root,markers[1],f,1);auto rotation=markers[0].base.vector_38.z;display.sample(file,f*rate);assert(markers[0].base.vector_38.z==rotation);assert(s::execute_animation(child)==0&&s::execute_animation(root)==0);}
 }
 assert(random_calls==0);std::cout<<"TH20 actual effect.anm: 480 matching native frames at rates 1/0.5, sprite/scale/rotation/alpha/layer, repeated drawings stable, zero RNG calls\n";
}