#include "../../cpp/sdl/PresentationUv.hpp"
#include "../../cpp/sdl/PresentationCamera.hpp"
#include <cassert>
#include <cmath>
using touhou::graphics::Address;
bool near(float a,float b){return std::abs(a-b)<.00001f;}
int main(){
 std::array<std::array<float,16>,4> previous{},current{};
 current[1][0]=.001f;current[1][12]=.4f;
 assert(th15::sdl::presentation_camera_continuous(previous,current));
 current[1][12]=361.7028f;
 assert(!th15::sdl::presentation_camera_continuous(previous,current));
 current[1][12]=128.f;
 assert(!th15::sdl::presentation_camera_continuous(previous,current));
 current[1][12]=0;current[2][14]=-128.f;
 assert(!th15::sdl::presentation_camera_continuous(previous,current));
 using th15::sdl::presentation_uv;
 std::array<float,16> old_texture{},texture{};
 old_texture[0]=texture[0]=old_texture[5]=texture[5]=1;
 old_texture[8]=.99f;texture[8]=.01f;
 old_texture[9]=.01f;texture[9]=.99f;
 old_texture[12]=.9f;texture[12]=.1f;
 auto half=th15::sdl::presentation_texture_matrix(old_texture,texture,.5f,Address::Repeat,Address::Repeat,true);
 assert(near(half[8],1.f)&&near(half[9],0.f));
 assert(near(half[12],.5f)); // Not the texture's UV offset column.
 half=th15::sdl::presentation_texture_matrix(old_texture,texture,.5f,Address::Clamp,Address::Clamp,true);
 assert(near(half[8],.5f)&&near(half[9],.5f));
 half=th15::sdl::presentation_texture_matrix(old_texture,texture,.5f,Address::Repeat,Address::Repeat,true,false,false);
 assert(half[8]==texture[8]&&half[9]==texture[9]);
 assert(th15::sdl::presentation_texture_matrix(old_texture,texture,1,Address::Repeat,Address::Repeat,true)==texture);
 assert(near(presentation_uv(.9f,.1f,.5f,Address::Clamp,true),.5f));
 assert(near(presentation_uv(.1f,.9f,.5f,Address::Clamp,true),.5f));
 assert(near(presentation_uv(.9f,.1f,.5f,Address::Repeat,true),1.f));
 assert(near(presentation_uv(.9f,.1f,.5f,Address::Mirror,true),.5f));
 assert(near(presentation_uv(1.9f,.1f,.5f,Address::Mirror,true),2.f));
 assert(near(presentation_uv(0.f,.5f,.5f,Address::Repeat,true),.25f));
 for(const auto address:{Address::Clamp,Address::Repeat,Address::Mirror}){
  assert(presentation_uv(.9f,.1f,1,address,true)==.1f);
  assert(near(presentation_uv(.9f,.1f,.5f,address,false),.5f));
 }
}
