#include "native-world.hpp"
#include "../../web/cpp/game/ScreenViews.hpp"
#include <algorithm>
struct GrazeGraphics final:ZunGraphics {
 touhou::graphics::PipelineState state;u32 submitted=0;std::vector<u32> colors;
 touhou::graphics::PipelineState& pipeline()override{return state;}
 u32 texture(const AnmResource&,u32 index)override{return index+1;}void bind_texture(u32)override{}
 void set_layout(touhou::graphics::VertexLayout)override{}void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
 void triangles(u32 n,const void* data,u32 stride)override{submitted+=n;if(stride==sizeof(AnmGeometryVertex)){auto* v=static_cast<const AnmGeometryVertex*>(data);for(u32 i=0;i<n*3;++i)colors.push_back(v[i].color);}}void primitives(touhou::graphics::Topology,u32 n,const void*,u32)override{submitted+=n;}
 bool select_target(const AnmResource*,u32)override{return true;}bool clear_target(u32,const GraphicsViewport*)override{return true;}bool clear_depth(const GraphicsViewport*)override{return true;}void set_viewport(const GraphicsViewport&)override{}
};

int main(int argc,char** argv){
 require(argc==2,"Retail archive required");RetailAssets files(argv[1]);
 for(unsigned count:{2u,3u})for(unsigned grazing=0;grazing<count;++grazing){
  World w(files,count,0,{0,1,3});auto& b=*w.battle;
  for(unsigned s=0;s<count;++s){auto& p=*b.pilots[s]->player;p.motion.position={s==grazing?0.f:140.f,400,0};p.motion.position_fixed={i32(p.motion.position.x*128),51200};}
  BulletShooter shot;shot.position={20,400,0};shot.speed=0;shot.count=shot.rows=1;require(b.bullet_scene->manager.emit(shot,0),b.error);
  for(unsigned n=0;n<5;++n)w.step({});const auto index=b.bullet_scene->manager.first_active();auto* bullet=b.bullet_scene->manager.state(index);require(bullet,"Bullet fixture absent");
  require(b.score.graze_total>0&&(bullet->visual_flags&0x60000u),"Shared graze accounting or authoritative glow lost");
  const auto flags=bullet->visual_flags,color=bullet->graze_color;const auto jitter=bullet->visual_jitter;const auto game=w.environment.game_rng.calls,visual=w.visual.calls;const auto graze=b.score.graze_total;
  GrazeGraphics graphics;AnmRenderer renderer(graphics);ScreenViews views(renderer,w.environment);require(views.camera(DrawCamera::Fullscreen,false),"Camera unavailable");
  std::vector<u32> own_colors,other_colors;
  for(unsigned local=0;local<count;++local){b.mp_options.local_seat=local;const auto* chosen=b.bullet_scene->viewpoint_graze(index);require(chosen,"Viewpoint graze missing");require(bool(chosen->flags)==(local==grazing),"Teammate graze tinted this viewpoint's bullet");
   b.mp_prepare_presentation(renderer);graphics.colors.clear();require(b.bullet_scene->draw(renderer),b.bullet_scene->error);renderer.flush();if(local==grazing)own_colors=graphics.colors;else other_colors=graphics.colors;
   auto& pilot=*b.pilots[grazing];auto* ring=w.animations.registry.find(pilot.player->visuals.barrier_handle);require(ring,"Native player graze ring absent");
   const bool hidden=std::find(renderer.multiplayer_hidden_hitboxes.begin(),renderer.multiplayer_hidden_hitboxes.end(),ring)!=renderer.multiplayer_hidden_hitboxes.end();require(hidden==(local!=grazing),"Player graze ring visible to teammate or hidden for self");
   if(hidden){graphics.submitted=0;require(renderer.draw(*ring)==0,renderer.error);renderer.flush();require(!graphics.submitted,"Remote player graze ring submitted pixels");}
   require(bullet->visual_flags==flags&&bullet->graze_color==color&&bullet->visual_jitter.x==jitter.x&&bullet->visual_jitter.y==jitter.y,"Viewpoint draw mutated authoritative bullet");
   require(game==w.environment.game_rng.calls&&visual==w.visual.calls&&graze==b.score.graze_total,"Viewpoint draw changed RNG or shared Graze");
  }
  require(!own_colors.empty()&&!other_colors.empty()&&own_colors!=other_colors,"Actual bullet draw colors did not follow the selected Graze viewpoint");
  auto& pilot=*b.pilots[grazing];const auto handle=pilot.player->visuals.barrier_handle;pilot.player->visuals.barrier_handle=0;b.mp_options.local_seat=(grazing+1)%count;b.mp_prepare_presentation(renderer);
  require(std::find(renderer.multiplayer_hidden_hitboxes.begin(),renderer.multiplayer_hidden_hitboxes.end(),w.animations.registry.find(handle))!=renderer.multiplayer_hidden_hitboxes.end(),"Fading graze ring leaked after owner lost handle");
 }
 std::puts("PASS: all 2P/3P grazer/viewpoint combinations, local-only bullet tint/jitter and player graze rings including fade-out, shared Graze/bullet/RNG unchanged");
}
