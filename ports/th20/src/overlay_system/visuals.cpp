#include "frame.hpp"
#include "../program_entry/program_entry.hpp"
#include "../gameplay/player_state.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../effect_system/effect.hpp"
#include "../ecl_vm/math.hpp"
#include <algorithm>
#include <cstring>
namespace th20::source::overlay {
namespace ps=gameplay::player_state;namespace pe=program_entry;namespace math=ecl::math;namespace n=recovered;
namespace {
sprite::Vec3 phase_position(WeaponStoneInf& owner){return ps::read<int>(*game_session::context(0).current_player,0xc)==7?*owner.main_weapon->phase_position():static_cast<player_entity::Player*>(game_session::context(0).objects_04[0])->position_614;}
int __cdecl mesh_callback(sprite::Animation*){update_mesh(*controller());return 0;} //532720→532a90
void replace_mesh(WeaponStoneInf& owner){sprite::destroy_render_mesh(owner.mesh);owner.mesh=nullptr;owner.mesh=sprite::create_render_mesh(12,12,0,owner.view_index);n::timer_set(owner.mesh_age,0);auto* animation=sprite::resolve_animation_handle(*pe::sprite_controller,owner.mesh->root_handle);if(!animation)animation=&pe::sprite_controller->animation_dc;animation->field_5dc=reinterpret_cast<std::uint32_t>(&mesh_callback);}
}
void phase_visuals(WeaponStoneInf& owner,bool ending){
 // The end effect snapshots position before destroying its old mesh. Start
 // reads the player after replacement, following each original call order.
 sprite::Vec3 position{};if(ending)position=phase_position(owner);replace_mesh(owner);if(!ending)position=static_cast<player_entity::Player*>(game_session::context(0).objects_04[0])->position_614;
 effects::Parameters p;effects::construct_parameters(p);p.vector_00=position;if(ending)p.enabled=ps::read<int>(*game_session::context(0).current_player,0xc)!=7;p.value_20=0xffffc080;
 const float pi=3.1415927410125732421875f,step=(pi*2.f)/24.f;float angle=0;
 for(int direction=0;direction<2;++direction){const float angular_speed=direction==0?pi/20.f:-pi/20.f;std::memcpy(&p.value_1c,&angular_speed,4);
  for(int i=0;i<24;++i){std::memcpy(&p.value_18,&angle,4);effects::controller(0)->spawn(ending?6:5,&p,nullptr,false);angle=math::wrap_angle(direction==0?angle+step:angle-step);}
 }
}
void update_mesh(WeaponStoneInf& owner){
 if(!owner.mesh)return;if(owner.mesh_age.current>=80){sprite::destroy_render_mesh(owner.mesh);owner.mesh=nullptr;return;}
 auto center=phase_position(owner);auto& mesh=*owner.mesh;sprite::initialize_render_mesh(mesh,center.x-120.f,center.y-120.f,240.f,240.f);
 const int ix=pe::window_state.field_0038[owner.view_index],iy=pe::window_state.field_0040[owner.view_index];const float ox=n::int_float(ix),oy=n::int_float(iy);center.x=ox+center.x;center.y=oy+center.y;
 // 533970 computes an unused wrapped phase here; it has no observable memory
 // result. All floating point operations contributing to geometry follow the
 // SSE order of the original body.
 const float pi=3.1415927410125732421875f;const float unused=math::wrap_angle((n::int_float(owner.mesh_age.current)*pi)/10.f);(void)unused;
 auto* position=mesh.positions;auto* vertex=mesh.vertices;
 for(int x=0;x<mesh.columns;++x)for(int y=0;y<mesh.rows;++y,++position,++vertex){
  sprite::Vec3 delta{position->x-center.x,position->y-center.y,position->z-center.z};const float distance=math::square_root(delta.x*delta.x+delta.y*delta.y);
  const float left=-208.f+ox,right=208.f+ox,top=n::int_float(n::signed_bits(static_cast<unsigned>(iy)-32u)),bottom=480.f+oy;
  if(!(left<=position->x&&position->x<=right&&top<=position->y&&position->y<=bottom))continue;
  if(distance>120.f){vertex->color&=0x00ffffff;continue;}
  const float ratio=distance/120.f;vertex->color=0xffffffff;
  if(ratio>.8f){const auto alpha=std::uint8_t(n::truncate32(255.f-((ratio-.8f)*255.f)/.2f));vertex->color=(vertex->color&0x00ffffff)|(std::uint32_t(alpha)<<24);}
  if(owner.mesh_age.current>60){const auto remaining=80-owner.mesh_age.current;const auto alpha=std::uint8_t(int((vertex->color>>24)*unsigned(remaining))/20);vertex->color=(vertex->color&0x00ffffff)|(std::uint32_t(alpha)<<24);}
  float angle=math::wrap_angle((pi*2.f)*ratio);angle=math::wrap_angle(angle-((n::int_float(owner.mesh_age.current)*pi)/2.f)/60.f);
  delta.x=math::sine(angle)*ratio*.5f*delta.x;delta.y=math::sine(angle)*ratio*.5f*delta.y;vertex->x=vertex->x+delta.x;vertex->y=vertex->y+delta.y;vertex->z=vertex->z+delta.z;
  if(vertex->x<left)vertex->x=left;else if(right<vertex->x)vertex->x=right;
  if(vertex->y<top)vertex->y=oy+-32.f;else if(bottom<vertex->y)vertex->y=bottom;
  vertex->z=0;position->z=0;
 }
 sprite::update_render_mesh_strips(mesh);n::timer_tick(owner.mesh_age,state::timer_rate);
}
}
